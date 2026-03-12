#include "CommonIncludes.h"
#include "LuaCoroutineHelpers.h"
#include "LuaSystem.h"
#include <UnigineGame.h>
#include <lua.hpp>
#include <unordered_map>
#include <thread>
#include <algorithm>

using namespace Unigine;

// Static members
std::vector<sol::coroutine> LuaCoroutineHelpers::active_coroutines;

void LuaCoroutineHelpers::registerHelpers(sol::state& lua)
{
    // WaitForSeconds - Returns a function that yields for the specified seconds
    // Usage in Lua: 
    //   local co = coroutine.create(function()
    //       WaitForSeconds(2.0)()
    //   end)
    lua["WaitForSeconds"] = [](float seconds) {
        return sol::as_function([seconds](sol::this_state L) {
            static thread_local std::unordered_map<lua_State*, float> elapsed_times;
            lua_State* L_ptr = L;
            
            if (elapsed_times.find(L_ptr) == elapsed_times.end())
                elapsed_times[L_ptr] = 0.0f;
            
            elapsed_times[L_ptr] += Game::getIFps();
            
            if (elapsed_times[L_ptr] < seconds)
            {
                // Yield
                return lua_yield(L, 0);
            }
            else
            {
                elapsed_times.erase(L_ptr);
                return 0;
            }
        });
    };
    
    // WaitUntil - Returns a function that yields until condition is true
    lua["WaitUntil"] = [](sol::function condition) {
        return sol::as_function([condition](sol::this_state L) {
            bool result = false;
            try
            {
                sol::object result_obj = condition();
                if (result_obj.is<bool>())
                    result = result_obj.as<bool>();
            }
            catch (...)
            {
                return 0; // Exit on error
            }
            
            if (!result)
            {
                // Yield
                return lua_yield(L, 0);
            }
            
            return 0;
        });
    };
    
    // StartCoroutine - Creates and starts a coroutine, managing it automatically
    lua["StartCoroutine"] = [](sol::function coroutine_func) {
        if (!coroutine_func.valid())
        {
            Log::warning("[Coroutine] StartCoroutine called with invalid function\n");
            return;
        }
        
        try
        {
            // Create coroutine from function (sol::coroutine constructs from sol::function)
            sol::coroutine co(coroutine_func);
            
            // Try to start it (first resume)
            auto result = co();
            
            // If coroutine is still alive (not finished), add to active list
            if (co.status() == sol::call_status::yielded)
            {
                active_coroutines.push_back(co);
            }
        }
        catch (const sol::error& e)
        {
            Log::error("[Coroutine] Error starting coroutine: %s\n", e.what());
        }
    };
    
    // Lerp - Linear interpolation helper
    lua["Lerp"] = sol::overload(
        [](float a, float b, float t) -> float {
            return a + (b - a) * t;
        },
        [](const Math::vec3& a, const Math::vec3& b, float t) -> Math::vec3 {
            return Math::lerp(a, b, t);
        }
    );
    
    // Tween - Simple tweening helper (simplified - doesn't use coroutines)
    lua["Tween"] = [](float start, float end, float duration, sol::function on_update) {
        // This is a simplified version that doesn't use coroutines
        // For a proper coroutine version, you'd need to structure it differently
        float elapsed = 0.0f;
        while (elapsed < duration)
        {
            float t = elapsed / duration;
            float value = start + (end - start) * t;
            
            if (on_update.valid())
            {
                try
                {
                    on_update(value);
                }
                catch (const sol::error& e)
                {
                    Log::error("[Tween] Error: %s\n", e.what());
                    break;
                }
            }
            
            elapsed += Game::getIFps();
            // Note: This won't actually yield - for proper yielding, use WaitForSeconds in a coroutine
        }
        
        // Final value
        if (on_update.valid())
        {
            try
            {
                on_update(end);
            }
            catch (...) {}
        }
    };
    
    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaCoroutineHelpers] Coroutine helpers registered\n");
}

void LuaCoroutineHelpers::updateCoroutines(float dt)
{
    (void)dt; // dt is available if needed for timing
    
    // Resume all active coroutines
    for (int i = static_cast<int>(active_coroutines.size()) - 1; i >= 0; --i)
    {
        sol::coroutine& co = active_coroutines[i];
        
        if (!co.valid())
        {
            active_coroutines.erase(active_coroutines.begin() + i);
            continue;
        }
        
        try
        {
            auto result = co();
            
            // Check if coroutine is finished or dead
            if (co.status() != sol::call_status::yielded)
            {
                // Coroutine finished or errored, remove it
                active_coroutines.erase(active_coroutines.begin() + i);
            }
        }
        catch (const sol::error& e)
        {
            Log::error("[Coroutine] Error resuming coroutine: %s\n", e.what());
            active_coroutines.erase(active_coroutines.begin() + i);
        }
    }
}

void LuaCoroutineHelpers::stopAll()
{
    active_coroutines.clear();
}

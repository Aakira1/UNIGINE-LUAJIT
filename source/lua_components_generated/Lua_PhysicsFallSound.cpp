#include "CommonIncludes.h"
#include "Lua_PhysicsFallSound.h"
#include <UnigineComponentSystem.h>
#include <UnigineGame.h>
#include <UnigineVisualizer.h>
#include <UnigineProfiler.h>
#include <UnigineConsole.h>
#include <UnigineInput.h>

REGISTER_COMPONENT(Lua_PhysicsFallSound);

using namespace Unigine;

void Lua_PhysicsFallSound::init()
{
    LuaSystem* ls = LuaSystem::get();
    bool should_log = ls ? ls->getLogInit() : false;
    
    if (should_log) {
        Log::message("[Lua_PhysicsFallSound C++] init() called\n");
    }
    
    LuaComponentDescriptor* desc = ls->getComponentDescriptor("PhysicsFallSound");
    if (!desc) {
        Log::error("[Lua_PhysicsFallSound] Component descriptor not found\n");
        return;
    }

    lua_instance = ls->getState().create_table();
    // Pass the node to Lua instance
    lua_instance["node"] = getNode();

    // Sync properties to Lua
    lua_instance["cooldown_time"] = cooldown_time.get();
    lua_instance["sound_path"] = sound_path.get();
    lua_instance["min_velocity"] = min_velocity.get();
    lua_instance["impact_threshold"] = impact_threshold.get();
    lua_instance["min_pitch"] = min_pitch.get();
    lua_instance["max_pitch"] = max_pitch.get();

    // Register this instance with LuaSystem
    ls->registerComponentInstance(getNode(), *desc, lua_instance);

    // Call Lua init
    sol::function init_fn = ls->getState()["fall_sound_init"];
    if (init_fn.valid())
    {
        try {
            if (should_log) {
                Log::message("[Lua_PhysicsFallSound] Calling Lua init...\n");
            }
            init_fn(lua_instance);
            lua_ready = true;
            if (should_log) {
                Log::message("[Lua_PhysicsFallSound] Lua init completed\n");
            }
        }
        catch (const sol::error & e) {
            Log::error("[Lua_PhysicsFallSound] Lua init error: %s\n", e.what());
        }
    }
    else
    {
        lua_ready = true;
    }
}

void Lua_PhysicsFallSound::update()
{
    static int update_count = 0;
    update_count++;
    
    if (!lua_ready) return;

    LuaSystem* ls = LuaSystem::get();

    // Sync properties to Lua
    lua_instance["cooldown_time"] = cooldown_time.get();
    lua_instance["sound_path"] = sound_path.get();
    lua_instance["min_velocity"] = min_velocity.get();
    lua_instance["impact_threshold"] = impact_threshold.get();
    lua_instance["min_pitch"] = min_pitch.get();
    lua_instance["max_pitch"] = max_pitch.get();

    // Call Lua update
    sol::function update_fn = ls->getState()["fall_sound_update"];
    if (update_fn.valid())
    {
        try {
            update_fn(lua_instance, Game::getIFps());
        }
        catch (const sol::error & e) {
            Log::error("[Lua_PhysicsFallSound] Lua update error: %s\n", e.what());
        }
    }

    // Sync properties back from Lua to C++ (bidirectional sync)
    if (lua_instance["cooldown_time"].valid())
    {
        sol::object lua_prop = lua_instance["cooldown_time"];
        if (lua_prop.is<float>())
            cooldown_time = lua_prop.as<float>();
        else if (lua_prop.is<double>())
            cooldown_time = static_cast<float>(lua_prop.as<double>());
    }
    if (lua_instance["sound_path"].valid())
    {
        sol::object lua_prop = lua_instance["sound_path"];
        if (lua_prop.is<std::string>())
            sound_path = lua_prop.as<std::string>().c_str();
    }
    if (lua_instance["min_velocity"].valid())
    {
        sol::object lua_prop = lua_instance["min_velocity"];
        if (lua_prop.is<float>())
            min_velocity = lua_prop.as<float>();
        else if (lua_prop.is<double>())
            min_velocity = static_cast<float>(lua_prop.as<double>());
    }
    if (lua_instance["impact_threshold"].valid())
    {
        sol::object lua_prop = lua_instance["impact_threshold"];
        if (lua_prop.is<float>())
            impact_threshold = lua_prop.as<float>();
        else if (lua_prop.is<double>())
            impact_threshold = static_cast<float>(lua_prop.as<double>());
    }
    if (lua_instance["min_pitch"].valid())
    {
        sol::object lua_prop = lua_instance["min_pitch"];
        if (lua_prop.is<float>())
            min_pitch = lua_prop.as<float>();
        else if (lua_prop.is<double>())
            min_pitch = static_cast<float>(lua_prop.as<double>());
    }
    if (lua_instance["max_pitch"].valid())
    {
        sol::object lua_prop = lua_instance["max_pitch"];
        if (lua_prop.is<float>())
            max_pitch = lua_prop.as<float>();
        else if (lua_prop.is<double>())
            max_pitch = static_cast<float>(lua_prop.as<double>());
    }
}


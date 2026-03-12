#include "CommonIncludes.h"
#include "Lua_PlayerController.h"
#include <UnigineComponentSystem.h>
#include <UnigineGame.h>
#include <UnigineVisualizer.h>
#include <UnigineProfiler.h>
#include <UnigineConsole.h>
#include <UnigineInput.h>

REGISTER_COMPONENT(Lua_PlayerController);

using namespace Unigine;

void Lua_PlayerController::init()
{
    LuaSystem* ls = LuaSystem::get();
    bool should_log = ls ? ls->getLogInit() : false;
    
    if (should_log) {
        Log::message("[Lua_PlayerController C++] init() called\n");
    }
    
    LuaComponentDescriptor* desc = ls->getComponentDescriptor("PlayerController");
    if (!desc) {
        Log::error("[Lua_PlayerController] Component descriptor not found\n");
        return;
    }

    lua_instance = ls->getState().create_table();
    // Pass the node to Lua instance
    lua_instance["node"] = getNode();

    // Sync properties to Lua
    lua_instance["move_speed"] = move_speed.get();
    lua_instance["jump_power"] = jump_power.get();
    lua_instance["crouch_speed"] = crouch_speed.get();
    lua_instance["crouch_height"] = crouch_height.get();
    lua_instance["mouse_sensitivity"] = mouse_sensitivity.get();
    lua_instance["camera_height"] = camera_height.get();

    // Register this instance with LuaSystem
    ls->registerComponentInstance(getNode(), *desc, lua_instance);

    // Call Lua init
    sol::function init_fn = ls->getState()["player_init"];
    if (init_fn.valid())
    {
        try {
            if (should_log) {
                Log::message("[Lua_PlayerController] Calling Lua init...\n");
            }
            init_fn(lua_instance);
            lua_ready = true;
            if (should_log) {
                Log::message("[Lua_PlayerController] Lua init completed\n");
            }
        }
        catch (const sol::error & e) {
            Log::error("[Lua_PlayerController] Lua init error: %s\n", e.what());
        }
    }
    else
    {
        lua_ready = true;
    }
}

void Lua_PlayerController::update()
{
    static int update_count = 0;
    update_count++;
    
    if (!lua_ready) return;

    LuaSystem* ls = LuaSystem::get();

    // Sync properties to Lua
    lua_instance["move_speed"] = move_speed.get();
    lua_instance["jump_power"] = jump_power.get();
    lua_instance["crouch_speed"] = crouch_speed.get();
    lua_instance["crouch_height"] = crouch_height.get();
    lua_instance["mouse_sensitivity"] = mouse_sensitivity.get();
    lua_instance["camera_height"] = camera_height.get();

    // Call Lua update
    sol::function update_fn = ls->getState()["player_update"];
    if (update_fn.valid())
    {
        try {
            update_fn(lua_instance, Game::getIFps());
        }
        catch (const sol::error & e) {
            Log::error("[Lua_PlayerController] Lua update error: %s\n", e.what());
        }
    }

    // Sync properties back from Lua to C++ (bidirectional sync)
    if (lua_instance["move_speed"].valid())
    {
        sol::object lua_prop = lua_instance["move_speed"];
        if (lua_prop.is<float>())
            move_speed = lua_prop.as<float>();
        else if (lua_prop.is<double>())
            move_speed = static_cast<float>(lua_prop.as<double>());
    }
    if (lua_instance["jump_power"].valid())
    {
        sol::object lua_prop = lua_instance["jump_power"];
        if (lua_prop.is<float>())
            jump_power = lua_prop.as<float>();
        else if (lua_prop.is<double>())
            jump_power = static_cast<float>(lua_prop.as<double>());
    }
    if (lua_instance["crouch_speed"].valid())
    {
        sol::object lua_prop = lua_instance["crouch_speed"];
        if (lua_prop.is<float>())
            crouch_speed = lua_prop.as<float>();
        else if (lua_prop.is<double>())
            crouch_speed = static_cast<float>(lua_prop.as<double>());
    }
    if (lua_instance["crouch_height"].valid())
    {
        sol::object lua_prop = lua_instance["crouch_height"];
        if (lua_prop.is<float>())
            crouch_height = lua_prop.as<float>();
        else if (lua_prop.is<double>())
            crouch_height = static_cast<float>(lua_prop.as<double>());
    }
    if (lua_instance["mouse_sensitivity"].valid())
    {
        sol::object lua_prop = lua_instance["mouse_sensitivity"];
        if (lua_prop.is<float>())
            mouse_sensitivity = lua_prop.as<float>();
        else if (lua_prop.is<double>())
            mouse_sensitivity = static_cast<float>(lua_prop.as<double>());
    }
    if (lua_instance["camera_height"].valid())
    {
        sol::object lua_prop = lua_instance["camera_height"];
        if (lua_prop.is<float>())
            camera_height = lua_prop.as<float>();
        else if (lua_prop.is<double>())
            camera_height = static_cast<float>(lua_prop.as<double>());
    }
}


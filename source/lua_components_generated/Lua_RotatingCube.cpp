#include "CommonIncludes.h"
#include "Lua_RotatingCube.h"
#include <UnigineComponentSystem.h>
#include <UnigineGame.h>
#include <UnigineVisualizer.h>
#include <UnigineProfiler.h>
#include <UnigineConsole.h>
#include <UnigineInput.h>

REGISTER_COMPONENT(Lua_RotatingCube);

using namespace Unigine;

void Lua_RotatingCube::init()
{
    LuaSystem* ls = LuaSystem::get();
    bool should_log = ls ? ls->getLogInit() : false;
    
    if (should_log) {
        Log::message("[Lua_RotatingCube C++] init() called\n");
    }
    
    LuaComponentDescriptor* desc = ls->getComponentDescriptor("RotatingCube");
    if (!desc) {
        Log::error("[Lua_RotatingCube] Component descriptor not found\n");
        return;
    }

    lua_instance = ls->getState().create_table();
    // Pass the node to Lua instance
    lua_instance["node"] = getNode();

    // Sync properties to Lua
    lua_instance["rotation_speed"] = rotation_speed.get();
    lua_instance["use_world_transform"] = use_world_transform.get();

    // Register this instance with LuaSystem
    ls->registerComponentInstance(getNode(), *desc, lua_instance);

    // Call Lua init
    sol::function init_fn = ls->getState()["rotate_init"];
    if (init_fn.valid())
    {
        try {
            if (should_log) {
                Log::message("[Lua_RotatingCube] Calling Lua init...\n");
            }
            init_fn(lua_instance);
            lua_ready = true;
            if (should_log) {
                Log::message("[Lua_RotatingCube] Lua init completed\n");
            }
        }
        catch (const sol::error & e) {
            Log::error("[Lua_RotatingCube] Lua init error: %s\n", e.what());
        }
    }
    else
    {
        lua_ready = true;
    }
}

void Lua_RotatingCube::update()
{
    static int update_count = 0;
    update_count++;
    
    if (!lua_ready) return;

    LuaSystem* ls = LuaSystem::get();

    // Sync properties to Lua
    lua_instance["rotation_speed"] = rotation_speed.get();
    lua_instance["use_world_transform"] = use_world_transform.get();

    // Call Lua update
    sol::function update_fn = ls->getState()["rotate_update"];
    if (update_fn.valid())
    {
        try {
            update_fn(lua_instance, Game::getIFps());
        }
        catch (const sol::error & e) {
            Log::error("[Lua_RotatingCube] Lua update error: %s\n", e.what());
        }
    }

    // Sync properties back from Lua to C++ (bidirectional sync)
    if (lua_instance["rotation_speed"].valid())
    {
        sol::object lua_prop = lua_instance["rotation_speed"];
        if (lua_prop.is<float>())
            rotation_speed = lua_prop.as<float>();
        else if (lua_prop.is<double>())
            rotation_speed = static_cast<float>(lua_prop.as<double>());
    }
    if (lua_instance["use_world_transform"].valid())
    {
        sol::object lua_prop = lua_instance["use_world_transform"];
        if (lua_prop.is<bool>())
            use_world_transform = lua_prop.as<bool>();
    }
}


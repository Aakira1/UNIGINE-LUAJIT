#pragma once
#ifndef LUA_SYSTEM_TYPES_H
#define LUA_SYSTEM_TYPES_H

// Type definitions for LuaSystem - separate from implementation
// This allows headers to use these types without including full implementations

#include "LuaSystemForward.h"
#include <UnigineProperties.h>

// Note: We need full sol.hpp definitions here because structs contain sol::table and sol::function
// These are template types that require full definition
#pragma warning(push, 0)
#include <sol.hpp>
#pragma warning(pop)

// Struct definitions (needed by multiple files)
struct LuaPropertyDef {
    Unigine::PropertyParameterPtr param;   // engine parameter
    Unigine::String lua_name;
    Unigine::String name;
    Unigine::String type;
    Unigine::String title;
    Unigine::String group;
    Unigine::String tooltip;
    Unigine::String args;
    Unigine::String sdefault;
};

struct LuaComponentDescriptor {
    Unigine::String guid;
    Unigine::String name;
    Unigine::String description;
    Unigine::Vector<LuaPropertyDef> properties;
    
    // Component dependencies - list of required component names
    Unigine::Vector<Unigine::String> required_components;

    // All Unigine component lifecycle methods
    Unigine::String fn_on_init;
    Unigine::String fn_on_update_async_thread;
    Unigine::String fn_on_update_sync_thread;
    Unigine::String fn_on_update;
    Unigine::String fn_on_post_update;
    Unigine::String fn_on_update_physics;
    Unigine::String fn_on_swap;
    Unigine::String fn_on_shutdown;
    
    // Multi-file component support - list of additional file paths
    Unigine::Vector<Unigine::String> additional_files;
};

struct LuaComponentInstance {
    Unigine::NodePtr node;
    LuaComponentDescriptor descriptor;
    sol::table instance;

    // All lifecycle function references
    sol::function on_init;
    sol::function on_update_async_thread;
    sol::function on_update_sync_thread;
    sol::function on_update;
    sol::function on_post_update;
    sol::function on_update_physics;
    sol::function on_swap;
    sol::function on_shutdown;

    bool initialized = false;
};

#endif // LUA_SYSTEM_TYPES_H

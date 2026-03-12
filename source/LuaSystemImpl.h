#pragma once
#ifndef LUA_SYSTEM_IMPL_H
#define LUA_SYSTEM_IMPL_H

// Implementation details for LuaSystem (Pimpl)
// This header is only included in LuaSystem.cpp to hide implementation

#include "CommonIncludes.h"
#include "LuaSystemTypes.h"
#include "LuaPropertyManager.h"
#include "LuaComponentLifecycle.h"
#include "LuaFileScanner.h"
#include <memory>
#include <unordered_map>

namespace Unigine {
    // Pimpl class - contains all implementation details
    class LuaSystemImpl {
    public:
        sol::state* lua = nullptr;

        Vector<LuaComponentInstance>* active_components = nullptr;
        HashMap<String, LuaComponentDescriptor>* components = nullptr;
        std::unordered_map<std::string, Vector<LuaPropertyDef>>* bindings = nullptr;

        bool initialized = false;
        bool post_initialized = false;
        bool property_files_created = false;
        bool is_shutting_down = false;

        time_t last_lua_check = 0;
        float lua_check_interval = 1.0f;

        Unigine::Vector<LuaComponentDescriptor> pending_components;

        // Helper classes - using smart pointers for automatic cleanup
        std::unique_ptr<LuaPropertyManager> property_manager;
        std::unique_ptr<LuaComponentLifecycle> lifecycle_manager;
        std::unique_ptr<LuaFileScanner> file_scanner;
    };
}

#endif // LUA_SYSTEM_IMPL_H

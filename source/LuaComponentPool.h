#pragma once
#ifndef LUA_COMPONENT_POOL_H
#define LUA_COMPONENT_POOL_H

#include "LuaSystemForward.h"
#include "LuaSystemTypes.h"
#include <unordered_map>
#include <vector>

namespace Unigine {
    // Component Pool Manager - Reuses component instances instead of creating/destroying
    class LuaComponentPool {
    public:
        LuaComponentPool(sol::state* lua_state);
        ~LuaComponentPool();

        // Get a pooled instance or create a new one
        sol::table acquire(const Unigine::String& component_name, Unigine::NodePtr node);
        
        // Return an instance to the pool
        void release(const Unigine::String& component_name, sol::table instance);
        
        // Clear all pools
        void clear();
        
        // Get pool statistics
        size_t getPoolSize(const Unigine::String& component_name) const;
        size_t getTotalPooledInstances() const;

    private:
        sol::state* lua;
        
        // Pool storage: component_name -> vector of available instances
        std::unordered_map<std::string, std::vector<sol::table>> pools;
        
        // Maximum pool size per component type (0 = unlimited)
        static constexpr size_t MAX_POOL_SIZE = 50;
    };
}

#endif // LUA_COMPONENT_POOL_H

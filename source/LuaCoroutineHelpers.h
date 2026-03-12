#pragma once
#ifndef LUA_COROUTINE_HELPERS_H
#define LUA_COROUTINE_HELPERS_H

#include "LuaSystemForward.h"
#include <vector>

namespace Unigine {
    class LuaCoroutineHelpers {
    public:
        // Register coroutine helper functions to Lua state
        static void registerHelpers(sol::state& lua);
        
        // Update active coroutines (call each frame)
        static void updateCoroutines(float dt);
        
        // Stop all coroutines
        static void stopAll();
        
    private:
        static std::vector<sol::coroutine> active_coroutines;
    };
}

#endif // LUA_COROUTINE_HELPERS_H

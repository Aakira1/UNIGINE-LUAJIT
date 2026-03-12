#pragma once
#ifndef LUA_DEBUGGER_H
#define LUA_DEBUGGER_H

#include "LuaSystemForward.h"
#include <lua.hpp>
#include <set>
#include <string>
#include <utility>

namespace Unigine {
    class LuaDebugger {
    public:
        // Debugger protocol types
        enum class Protocol {
            MobDebug,      // ZeroBrane Studio / MobDebug protocol
            DAP,           // Debug Adapter Protocol (VS Code)
            None
        };
        
        static LuaDebugger& get();
        
        // Initialize debugger
        bool initialize(Protocol protocol, int port = 8172);
        
        // Shutdown debugger
        void shutdown();
        
        // Check if debugger is active
        bool isActive() const { return active; }
        
        // Set breakpoint
        void setBreakpoint(const std::string& file, int line);
        
        // Remove breakpoint
        void removeBreakpoint(const std::string& file, int line);
        
        // Step execution
        void step();
        void stepOver();
        void stepOut();
        void continueExecution();
        
        // Register debug hooks with Lua state
        void registerHooks(sol::state& lua);
        
    private:
        LuaDebugger() = default;
        bool active = false;
        Protocol current_protocol = Protocol::None;
        int debug_port = 8172;
        void* debug_server = nullptr;

        using BreakpointKey = std::pair<std::string, int>;
        std::set<BreakpointKey> breakpoints_;
        bool paused_ = false;
        int step_mode_ = 0;  // 0 none, 1 step, 2 step over, 3 step out
        int step_over_depth_ = 0;
        int step_out_depth_ = 0;

        static void debugHook(lua_State* L, lua_Debug* ar);
        bool checkBreakpoint(const char* source, int line) const;
        static const char* normalizePath(const char* path);
    };
}

#endif // LUA_DEBUGGER_H

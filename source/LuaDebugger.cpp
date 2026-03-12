#include "CommonIncludes.h"
#include "LuaDebugger.h"
#include <lua.hpp>

using namespace Unigine;

LuaDebugger& LuaDebugger::get()
{
    static LuaDebugger instance;
    return instance;
}

const char* LuaDebugger::normalizePath(const char* path)
{
    if (!path || !path[0])
        return path;
    const char* p = path;
    const char* last = path;
    while (*p)
    {
        if (*p == '/' || *p == '\\')
            last = p + 1;
        ++p;
    }
    return last;
}

bool LuaDebugger::checkBreakpoint(const char* source, int line) const
{
    if (!source || line <= 0)
        return false;
    const char* norm = normalizePath(source);
    auto it = breakpoints_.find(BreakpointKey(norm, line));
    if (it != breakpoints_.end())
        return true;
    it = breakpoints_.find(BreakpointKey(source, line));
    return it != breakpoints_.end();
}

void LuaDebugger::debugHook(lua_State* L, lua_Debug* ar)
{
    LuaDebugger& self = get();
    if (!self.active)
        return;

    if (ar->event == LUA_HOOKLINE)
    {
        lua_getinfo(L, "Sl", ar);
        const char* src = ar->source && ar->source[0] == '@' ? ar->source + 1 : ar->source;
        int line = ar->currentline;
        if (line <= 0)
            return;

        if (self.step_mode_ == 1)
        {
            self.step_mode_ = 0;
            Log::message("[LuaDebugger] Step stopped at %s:%d\n", src, line);
            return;
        }

        if (self.checkBreakpoint(src, line))
        {
            Log::message("[LuaDebugger] Breakpoint hit at %s:%d\n", src, line);
            return;
        }
    }
    else if (ar->event == LUA_HOOKCALL && self.step_mode_ == 2)
    {
        self.step_over_depth_++;
    }
    else if (ar->event == LUA_HOOKRET && self.step_mode_ == 2)
    {
        if (self.step_over_depth_ > 0)
            self.step_over_depth_--;
        if (self.step_over_depth_ == 0)
        {
            self.step_mode_ = 1;
            self.step_over_depth_ = 0;
        }
    }
    else if (ar->event == LUA_HOOKRET && self.step_mode_ == 3)
    {
        if (self.step_out_depth_ > 0)
            self.step_out_depth_--;
        if (self.step_out_depth_ == 0)
        {
            self.step_mode_ = 1;
            self.step_out_depth_ = 0;
        }
    }
}

bool LuaDebugger::initialize(Protocol protocol, int port)
{
    if (active)
    {
        Log::warning("[LuaDebugger] Debugger already initialized\n");
        return false;
    }

    current_protocol = protocol;
    debug_port = port;
    breakpoints_.clear();
    paused_ = false;
    step_mode_ = 0;

    if (protocol == Protocol::None)
    {
        active = false;
        return true;
    }

    Log::message("[LuaDebugger] Initialized debugger (protocol: %d, port: %d)\n",
        static_cast<int>(protocol), port);

    active = true;
    return true;
}

void LuaDebugger::shutdown()
{
    if (!active)
        return;

    breakpoints_.clear();
    debug_server = nullptr;
    active = false;
    current_protocol = Protocol::None;
    paused_ = false;
    step_mode_ = 0;

    Log::message("[LuaDebugger] Debugger shutdown\n");
}

void LuaDebugger::setBreakpoint(const std::string& file, int line)
{
    if (!active)
        return;

    breakpoints_.insert(BreakpointKey(file, line));
    Log::message("[LuaDebugger] Breakpoint set: %s:%d\n", file.c_str(), line);
}

void LuaDebugger::removeBreakpoint(const std::string& file, int line)
{
    if (!active)
        return;

    breakpoints_.erase(BreakpointKey(file, line));
    Log::message("[LuaDebugger] Breakpoint removed: %s:%d\n", file.c_str(), line);
}

void LuaDebugger::step()
{
    if (!active)
        return;

    step_mode_ = 1;
    step_over_depth_ = 0;
    step_out_depth_ = 0;
    Log::message("[LuaDebugger] Step (next line hook active)\n");
}

void LuaDebugger::stepOver()
{
    if (!active)
        return;

    step_mode_ = 2;
    step_over_depth_ = 0;
    step_out_depth_ = 0;
    Log::message("[LuaDebugger] Step over (next line at current depth)\n");
}

void LuaDebugger::stepOut()
{
    if (!active)
        return;

    step_mode_ = 3;
    step_over_depth_ = 0;
    step_out_depth_ = 1;
    Log::message("[LuaDebugger] Step out (stop on return to caller)\n");
}

void LuaDebugger::continueExecution()
{
    if (!active)
        return;

    step_mode_ = 0;
    step_over_depth_ = 0;
    step_out_depth_ = 0;
    paused_ = false;
    Log::message("[LuaDebugger] Continue\n");
}

void LuaDebugger::registerHooks(sol::state& lua)
{
    if (!active)
        return;

    lua_State* L = lua.lua_state();
    if (!L)
        return;

    lua_sethook(L, debugHook, LUA_MASKLINE | LUA_MASKCALL | LUA_MASKRET, 0);
    Log::message("[LuaDebugger] Debug hooks registered (breakpoints + step)\n");
}

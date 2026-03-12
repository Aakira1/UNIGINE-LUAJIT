#pragma once
#ifndef LUA_PROFILER_H
#define LUA_PROFILER_H

#include "LuaSystemForward.h"
#include <UnigineProfiler.h>
#include <string>
#include <unordered_map>

namespace Unigine {
    class LuaProfiler {
    public:
        struct ProfileData {
            std::string name;
            float total_time = 0.0f;
            int call_count = 0;
            float min_time = FLT_MAX;
            float max_time = 0.0f;
        };
        
        static LuaProfiler& get();
        
        // Start profiling a function
        void beginProfile(const std::string& name);
        
        // End profiling a function
        void endProfile(const std::string& name);
        
        // Get profile data
        ProfileData getProfileData(const std::string& name) const;
        
        // Get all profile data
        std::unordered_map<std::string, ProfileData> getAllProfileData() const;
        
        // Reset all profiling data
        void reset();
        
        // Register to Lua
        static void registerToLua(sol::state& lua);
        
    private:
        LuaProfiler() = default;
        std::unordered_map<std::string, float> start_times;
        std::unordered_map<std::string, ProfileData> profile_data;
    };
}

#endif // LUA_PROFILER_H

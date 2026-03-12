#include "CommonIncludes.h"
#include "LuaProfiler.h"
#include "LuaSystem.h"
#include <UnigineProfiler.h>
#include <UnigineGame.h>
#include <float.h>

using namespace Unigine;

LuaProfiler& LuaProfiler::get()
{
    static LuaProfiler instance;
    return instance;
}

void LuaProfiler::beginProfile(const std::string& name)
{
    start_times[name] = Game::getTime();
    
    #ifdef USE_PROFILER
    Profiler::begin(name.c_str());
    #endif
}

void LuaProfiler::endProfile(const std::string& name)
{
    auto it = start_times.find(name);
    if (it == start_times.end())
        return;
    
    float elapsed = Game::getTime() - it->second;
    start_times.erase(it);
    
    ProfileData& data = profile_data[name];
    data.name = name;
    data.total_time += elapsed;
    data.call_count++;
    data.min_time = (elapsed < data.min_time) ? elapsed : data.min_time;
    data.max_time = (elapsed > data.max_time) ? elapsed : data.max_time;
    
    #ifdef USE_PROFILER
    Profiler::end();
    #endif
}

LuaProfiler::ProfileData LuaProfiler::getProfileData(const std::string& name) const
{
    auto it = profile_data.find(name);
    if (it != profile_data.end())
        return it->second;
    
    ProfileData empty;
    empty.name = name;
    return empty;
}

std::unordered_map<std::string, LuaProfiler::ProfileData> LuaProfiler::getAllProfileData() const
{
    return profile_data;
}

void LuaProfiler::reset()
{
    profile_data.clear();
    start_times.clear();
}

void LuaProfiler::registerToLua(sol::state& lua)
{
    auto& profiler = get();
    
    lua["Profiler"] = lua.create_table();
    
    lua["Profiler"]["begin"] = [&profiler](const std::string& name) {
        profiler.beginProfile(name);
    };
    
    lua["Profiler"]["end"] = [&profiler](const std::string& name) {
        profiler.endProfile(name);
    };
    
    lua["Profiler"]["getData"] = [&profiler, &lua](const std::string& name) -> sol::table {
        sol::table data = lua.create_table();
        LuaProfiler::ProfileData pd = profiler.getProfileData(name);
        
        data["name"] = pd.name;
        data["total_time"] = pd.total_time;
        data["call_count"] = pd.call_count;
        data["avg_time"] = pd.call_count > 0 ? pd.total_time / pd.call_count : 0.0f;
        data["min_time"] = pd.min_time;
        data["max_time"] = pd.max_time;
        
        return data;
    };
    
    lua["Profiler"]["reset"] = [&profiler]() {
        profiler.reset();
    };
    
    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaProfiler] Profiler registered to Lua\n");
}

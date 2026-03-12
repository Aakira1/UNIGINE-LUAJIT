#include "CommonIncludes.h"
#include "LuaComponentPool.h"
#include "LuaSystem.h"

using namespace Unigine;

LuaComponentPool::LuaComponentPool(sol::state* lua_state) : lua(lua_state)
{
}

LuaComponentPool::~LuaComponentPool()
{
    clear();
}

sol::table LuaComponentPool::acquire(const Unigine::String& component_name, Unigine::NodePtr node)
{
    std::string key = component_name.get();
    
    // Check if we have a pooled instance available
    auto it = pools.find(key);
    if (it != pools.end() && !it->second.empty())
    {
        sol::table instance = it->second.back();
        it->second.pop_back();
        
        // Reset the instance for reuse
        instance["node"] = node;
        instance["_pooled"] = false; // Mark as in use
        
        Log::message("[ComponentPool] Reused instance for '%s' (pool size: %zu)\n", 
            component_name.get(), it->second.size());
        
        return instance;
    }
    
    // Create a new instance
    if (!lua)
    {
        Log::error("[ComponentPool] Cannot acquire: Lua state is null\n");
        return sol::nil;
    }
    
    sol::table instance = lua->create_table();
    instance["node"] = node;
    instance["_pooled"] = false;
    
    Log::message("[ComponentPool] Created new instance for '%s'\n", component_name.get());
    
    return instance;
}

void LuaComponentPool::release(const Unigine::String& component_name, sol::table instance)
{
    if (!instance.valid())
        return;
    
    std::string key = component_name.get();
    
    // Mark as pooled
    instance["_pooled"] = true;
    
    // Clear node reference to allow garbage collection
    instance["node"] = sol::nil;
    
    // Add to pool if under max size
    auto& pool = pools[key];
    if (pool.size() < MAX_POOL_SIZE)
    {
        pool.push_back(instance);
        Log::message("[ComponentPool] Returned instance for '%s' to pool (pool size: %zu)\n",
            component_name.get(), pool.size());
    }
    else
    {
        Log::message("[ComponentPool] Pool full for '%s', discarding instance\n", component_name.get());
    }
}

void LuaComponentPool::clear()
{
    pools.clear();
    Log::message("[ComponentPool] All pools cleared\n");
}

size_t LuaComponentPool::getPoolSize(const Unigine::String& component_name) const
{
    std::string key = component_name.get();
    auto it = pools.find(key);
    return (it != pools.end()) ? it->second.size() : 0;
}

size_t LuaComponentPool::getTotalPooledInstances() const
{
    size_t total = 0;
    for (const auto& pair : pools)
    {
        total += pair.second.size();
    }
    return total;
}

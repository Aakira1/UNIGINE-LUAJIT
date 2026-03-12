#pragma once
#ifndef LUA_COMPONENT_LIFECYCLE_H
#define LUA_COMPONENT_LIFECYCLE_H

#include "LuaSystemForward.h"
#include "LuaSystemTypes.h"
#include <unordered_map>

namespace Unigine {
    /// <summary>
    /// Manages lifecycle for Lua component instances.
    /// 
    /// NOTE: For components with generated C++ wrappers (Lua_* classes):
    /// - Generated wrappers handle init() and update() themselves
    /// - They call registerComponentInstance() which sets initialized=true
    /// - This class only runs on_init for Lua-only instances (!initialized)
    /// 
    /// For generated components:
    /// - Lua_*::init() creates lua_instance, syncs props, calls Lua init
    /// - Lua_*::update() syncs props C++→Lua, calls Lua update
    /// - Lifecycle::update() skips on_update (already handled by generated wrapper)
    /// </summary>
    class LuaComponentLifecycle {
    public:
        LuaComponentLifecycle(sol::state* lua_state) : lua(lua_state) {}

        void update(float ifps, Unigine::Vector<LuaComponentInstance>& active_components);
        static void shutdown(Unigine::Vector<LuaComponentInstance>& active_components,
            Unigine::HashMap<Unigine::String, LuaComponentDescriptor>& components,
            std::unordered_map<std::string, Unigine::Vector<LuaPropertyDef>>& bindings,
            bool& is_shutting_down,
            sol::state& lua
        );

        void removeLuaComponent(const Unigine::NodePtr& node, const Unigine::String& component_name,
            Unigine::Vector<LuaComponentInstance>& active_components);

        void registerComponentInstance(Unigine::NodePtr node, const LuaComponentDescriptor& desc, sol::table instance,
            Unigine::Vector<LuaComponentInstance>& active_components);
        void unregisterComponentInstance(Unigine::NodePtr node, const Unigine::String& component_name,
            Unigine::Vector<LuaComponentInstance>& active_components);

    private:
        sol::state* lua;
    };
}

#endif // LUA_COMPONENT_LIFECYCLE_H
#pragma once
#ifndef LUA_COMPONENT_QUERIES_H
#define LUA_COMPONENT_QUERIES_H

#include "LuaSystemForward.h"
#include "LuaSystemTypes.h"

namespace Unigine {
    class LuaComponentQueries {
    public:
        // Find all components of a specific type
        static Unigine::Vector<LuaComponentInstance*> findByType(
            const Unigine::String& component_type,
            Unigine::Vector<LuaComponentInstance>& active_components);
        
        // Find components by tag (stored in instance table)
        static Unigine::Vector<LuaComponentInstance*> findByTag(
            const Unigine::String& tag,
            Unigine::Vector<LuaComponentInstance>& active_components);
        
        // Find component on a specific node
        static LuaComponentInstance* findByNode(
            Unigine::NodePtr node,
            const Unigine::String& component_type,
            Unigine::Vector<LuaComponentInstance>& active_components);
        
        // Find all components on a node
        static Unigine::Vector<LuaComponentInstance*> findByNodeAll(
            Unigine::NodePtr node,
            Unigine::Vector<LuaComponentInstance>& active_components);
        
        // Find components within a radius
        static Unigine::Vector<LuaComponentInstance*> findByRadius(
            const Unigine::Math::vec3& position,
            float radius,
            Unigine::Vector<LuaComponentInstance>& active_components);
    };
}

#endif // LUA_COMPONENT_QUERIES_H

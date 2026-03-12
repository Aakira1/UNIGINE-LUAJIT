#pragma once
#ifndef LUA_PROPERTY_SERIALIZATION_H
#define LUA_PROPERTY_SERIALIZATION_H

#include "LuaSystemForward.h"
#include "LuaSystemTypes.h"
#include <string>

namespace Unigine {
    class LuaPropertySerialization {
    public:
        // Serialize component instance to JSON string
        static std::string serializeToJSON(const LuaComponentInstance& instance);
        
        // Deserialize JSON string to component instance
        static bool deserializeFromJSON(const std::string& json,
                                       LuaComponentInstance& instance,
                                       sol::state& lua);
        
        // Save component state to file
        static bool saveToFile(const LuaComponentInstance& instance,
                              const Unigine::String& filepath);
        
        // Load component state from file
        static bool loadFromFile(const Unigine::String& filepath,
                                LuaComponentInstance& instance,
                                sol::state& lua);
        
        // Serialize all components on a node
        static std::string serializeNodeComponents(Unigine::NodePtr node,
                                                   Unigine::Vector<LuaComponentInstance>& active_components);
        
        // Deserialize components to a node
        static bool deserializeNodeComponents(const std::string& json,
                                             Unigine::NodePtr node,
                                             Unigine::Vector<LuaComponentInstance>& active_components,
                                             sol::state& lua);
    };
}

#endif // LUA_PROPERTY_SERIALIZATION_H

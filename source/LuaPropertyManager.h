#pragma once
#ifndef LUA_PROPERTY_MANAGER_H
#define LUA_PROPERTY_MANAGER_H

#include "LuaSystemForward.h"
#include "LuaSystemTypes.h"
#include <unordered_map>

namespace Unigine {
    class LuaPropertyManager {
    public:
        int luaTypeToPropertyType(const Unigine::String& lua_type);
        void setDefaultValue(const Unigine::PropertyParameterPtr& param, const LuaPropertyDef& def);

        void createPropertyFilesImmediately(Unigine::HashMap<Unigine::String, LuaComponentDescriptor>& components,
            bool& property_files_created);

        bool bindPropertyParameters(const LuaComponentDescriptor& d, const Unigine::PropertyPtr& prop,
            std::unordered_map<std::string, Unigine::Vector<LuaPropertyDef>>& bindings);

        void syncLuaComponents(Unigine::Vector<LuaComponentInstance>& active_components,
            std::unordered_map<std::string, Unigine::Vector<LuaPropertyDef>>& bindings);

        void parseProperties(const sol::table& props, Unigine::Vector<LuaPropertyDef>& out);
    };
}

#endif // LUA_PROPERTY_MANAGER_H
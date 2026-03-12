#pragma once
#ifndef LUA_MATERIAL_RUNTIME_H
#define LUA_MATERIAL_RUNTIME_H

#include "LuaSystemForward.h"
#include <UnigineMaterial.h>

namespace Unigine {
    class LuaMaterialRuntime {
    public:
        // Create material instance at runtime
        static MaterialPtr createInstance(const Unigine::String& base_material_path);
        
        // Apply material to object
        static bool applyToObject(Unigine::ObjectPtr obj, MaterialPtr material, int surface = 0);
        
        // Get material from object
        static MaterialPtr getFromObject(Unigine::ObjectPtr obj, int surface = 0);
        
        // Set shader parameter (convenience wrapper)
        static bool setShaderParameter(MaterialPtr material, 
                                      const Unigine::String& parameter_name,
                                      float value);
        
        static bool setShaderParameter(MaterialPtr material,
                                      const Unigine::String& parameter_name,
                                      const Math::vec3& value);
        
        static bool setShaderParameter(MaterialPtr material,
                                      const Unigine::String& parameter_name,
                                      const Math::vec4& value);
        
        // Get shader parameter
        static float getShaderParameterFloat(MaterialPtr material, const Unigine::String& name);
        static Math::vec3 getShaderParameterVec3(MaterialPtr material, const Unigine::String& name);
        static Math::vec4 getShaderParameterVec4(MaterialPtr material, const Unigine::String& name);
        
        // Register to Lua
        static void registerToLua(sol::state& lua);
    };
}

#endif // LUA_MATERIAL_RUNTIME_H

#include "CommonIncludes.h"
#include "LuaMaterialRuntime.h"
#include "LuaSystem.h"
#include <UnigineMaterials.h>
#include <UnigineObjects.h>

using namespace Unigine;

MaterialPtr LuaMaterialRuntime::createInstance(const Unigine::String& base_material_path)
{
    MaterialPtr base = Materials::loadMaterial(base_material_path.get());
    if (!base)
    {
        Log::error("[MaterialRuntime] Failed to load base material: %s\n", base_material_path.get());
        return nullptr;
    }
    
    MaterialPtr instance = base->clone();
    Log::message("[MaterialRuntime] Created material instance from: %s\n", base_material_path.get());
    return instance;
}

bool LuaMaterialRuntime::applyToObject(Unigine::ObjectPtr obj, MaterialPtr material, int surface)
{
    if (!obj || !material)
        return false;
    
    if (surface < 0 || surface >= obj->getNumSurfaces())
    {
        Log::error("[MaterialRuntime] Invalid surface index: %d\n", surface);
        return false;
    }
    
    obj->setMaterial(material, surface);
    return true;
}

MaterialPtr LuaMaterialRuntime::getFromObject(Unigine::ObjectPtr obj, int surface)
{
    if (!obj)
        return nullptr;
    
    if (surface < 0 || surface >= obj->getNumSurfaces())
        return nullptr;
    
    return obj->getMaterial(surface);
}

bool LuaMaterialRuntime::setShaderParameter(MaterialPtr material,
                                            const Unigine::String& parameter_name,
                                            float value)
{
    if (!material)
        return false;
    
    int param_index = material->findParameter(parameter_name.get());
    if (param_index < 0)
    {
        Log::warning("[MaterialRuntime] Parameter not found: %s\n", parameter_name.get());
        return false;
    }
    
    material->setParameterFloat(param_index, value);
    return true;
}

bool LuaMaterialRuntime::setShaderParameter(MaterialPtr material,
                                            const Unigine::String& parameter_name,
                                            const Math::vec3& value)
{
    if (!material)
        return false;
    
    int param_index = material->findParameter(parameter_name.get());
    if (param_index < 0)
    {
        Log::warning("[MaterialRuntime] Parameter not found: %s\n", parameter_name.get());
        return false;
    }
    
    material->setParameterFloat3(param_index, value);
    return true;
}

bool LuaMaterialRuntime::setShaderParameter(MaterialPtr material,
                                            const Unigine::String& parameter_name,
                                            const Math::vec4& value)
{
    if (!material)
        return false;
    
    int param_index = material->findParameter(parameter_name.get());
    if (param_index < 0)
    {
        Log::warning("[MaterialRuntime] Parameter not found: %s\n", parameter_name.get());
        return false;
    }
    
    material->setParameterFloat4(param_index, value);
    return true;
}

float LuaMaterialRuntime::getShaderParameterFloat(MaterialPtr material, const Unigine::String& name)
{
    if (!material)
        return 0.0f;
    
    int param_index = material->findParameter(name.get());
    if (param_index < 0)
        return 0.0f;
    
    return material->getParameterFloat(param_index);
}

Math::vec3 LuaMaterialRuntime::getShaderParameterVec3(MaterialPtr material, const Unigine::String& name)
{
    if (!material)
        return Math::vec3(0, 0, 0);
    
    int param_index = material->findParameter(name.get());
    if (param_index < 0)
        return Math::vec3(0, 0, 0);
    
    return material->getParameterFloat3(param_index);
}

Math::vec4 LuaMaterialRuntime::getShaderParameterVec4(MaterialPtr material, const Unigine::String& name)
{
    if (!material)
        return Math::vec4(0, 0, 0, 0);
    
    int param_index = material->findParameter(name.get());
    if (param_index < 0)
        return Math::vec4(0, 0, 0, 0);
    
    return material->getParameterFloat4(param_index);
}

void LuaMaterialRuntime::registerToLua(sol::state& lua)
{
    lua["MaterialRuntime"] = lua.create_table();
    
    lua["MaterialRuntime"]["createInstance"] = &createInstance;
    lua["MaterialRuntime"]["applyToObject"] = sol::overload(
        [](Unigine::ObjectPtr obj, MaterialPtr mat) { return applyToObject(obj, mat, 0); },
        [](Unigine::ObjectPtr obj, MaterialPtr mat, int surface) { return applyToObject(obj, mat, surface); }
    );
    lua["MaterialRuntime"]["getFromObject"] = sol::overload(
        [](Unigine::ObjectPtr obj) { return getFromObject(obj, 0); },
        [](Unigine::ObjectPtr obj, int surface) { return getFromObject(obj, surface); }
    );
    
    lua["MaterialRuntime"]["setParameter"] = sol::overload(
        [](MaterialPtr mat, const std::string& name, float value) {
            return setShaderParameter(mat, name.c_str(), value);
        },
        [](MaterialPtr mat, const std::string& name, const Math::vec3& value) {
            return setShaderParameter(mat, name.c_str(), value);
        },
        [](MaterialPtr mat, const std::string& name, const Math::vec4& value) {
            return setShaderParameter(mat, name.c_str(), value);
        }
    );
    
    lua["MaterialRuntime"]["getParameterFloat"] = &getShaderParameterFloat;
    lua["MaterialRuntime"]["getParameterVec3"] = &getShaderParameterVec3;
    lua["MaterialRuntime"]["getParameterVec4"] = &getShaderParameterVec4;
    
    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[MaterialRuntime] Runtime material API registered\n");
}

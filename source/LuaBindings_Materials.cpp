#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include <UnigineMaterial.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

void Unigine::LuaBindings::registerMaterialTypes(sol::state& lua)
{
    



    // Material Enumerations
    

    lua.new_enum("MaterialOption",
        "TRANSPARENT", Material::OPTION_TRANSPARENT,
        "ORDER", Material::OPTION_ORDER,
        "ORDER_CLOUDS", Material::OPTION_ORDER_CLOUDS,
        "SHADOW_MASK", Material::OPTION_SHADOW_MASK,
        "VIEWPORT_MASK", Material::OPTION_VIEWPORT_MASK,
        "BLEND_SRC", Material::OPTION_BLEND_SRC,
        "BLEND_DEST", Material::OPTION_BLEND_DEST,
        "BLEND_ALPHA_SRC", Material::OPTION_BLEND_ALPHA_SRC,
        "DEPTH_TEST", Material::OPTION_DEPTH_TEST,
        "TWO_SIDED", Material::OPTION_TWO_SIDED,
        "CAST_PROJ_OMNI_SHADOW", Material::OPTION_CAST_PROJ_OMNI_SHADOW,
        "CAST_WORLD_SHADOW", Material::OPTION_CAST_WORLD_SHADOW,
        "OVERLAP", Material::OPTION_OVERLAP
    );

    lua.new_enum("MaterialTransparent",
        "NONE", Material::TRANSPARENT_NONE,
        "ALPHA_TEST", Material::TRANSPARENT_ALPHA_TEST,
        "BLEND", Material::TRANSPARENT_BLEND,
        "WATER", Material::TRANSPARENT_WATER
    );

    lua.new_enum("MaterialState",
        "TOGGLE", Material::STATE_TOGGLE,
        "SWITCH", Material::STATE_SWITCH,
        "INT", Material::STATE_INT
    );

    lua.new_enum("MaterialParameter",
        "FLOAT", Material::PARAMETER_FLOAT,
        "FLOAT2", Material::PARAMETER_FLOAT2,
        "FLOAT3", Material::PARAMETER_FLOAT3,
        "FLOAT4", Material::PARAMETER_FLOAT4,
        "INT", Material::PARAMETER_INT,
        "INT2", Material::PARAMETER_INT2,
        "INT3", Material::PARAMETER_INT3,
        "INT4", Material::PARAMETER_INT4
    );

    
    // MaterialPtr Type
    

    lua.new_usertype<MaterialPtr>("MaterialPtr",
        sol::no_constructor,

        // Static creation
        "create", []() { return Material::create(); },

        // Parent/hierarchy
        "setParent", sol::overload(
            [](MaterialPtr& mat, const MaterialPtr& parent) { return mat->setParent(parent); },
            [](MaterialPtr& mat, const MaterialPtr& parent, bool save_all) { return mat->setParent(parent, save_all); }
        ),
        "getParent", [](const MaterialPtr& mat) { return mat->getParent(); },
        "isParent", sol::overload(
            [](const MaterialPtr& mat, const MaterialPtr& parent) { return mat->isParent(parent); },
            [](const MaterialPtr& mat, const UGUID& guid) { return mat->isParent(guid); }
        ),
        "getBaseMaterial", [](const MaterialPtr& mat) { return mat->getBaseMaterial(); },
        "getNumChildren", [](const MaterialPtr& mat) { return mat->getNumChildren(); },
        "getChild", [](const MaterialPtr& mat, int num) { return mat->getChild(num); },

        // Clone/inherit
        "clone", sol::overload(
            [](const MaterialPtr& mat) { return mat->clone(); },
            [](const MaterialPtr& mat, const UGUID& guid) { return mat->clone(guid); }
        ),
        "inherit", sol::overload(
            [](MaterialPtr& mat) { return mat->inherit(); },
            [](MaterialPtr& mat, const UGUID& guid) { return mat->inherit(guid); }
        ),

        // Identity
        "getNamespaceName", [](const MaterialPtr& mat) { return mat->getNamespaceName(); },
        "getManualName", [](const MaterialPtr& mat) { return mat->getManualName(); },
        "getGUID", [](const MaterialPtr& mat) { return mat->getGUID(); },
        "getFileGUID", [](const MaterialPtr& mat) { return mat->getFileGUID(); },
        "getFilePath", [](const MaterialPtr& mat) { return mat->getFilePath(); },

        // Node support
        "isNodeTypeSupported", [](const MaterialPtr& mat, int type) { return mat->isNodeTypeSupported((Node::TYPE)type); },
        "isNodeSupported", [](const MaterialPtr& mat, const NodePtr& node) { return mat->isNodeSupported(node); },
        "canRenderNode", [](const MaterialPtr& mat) { return mat->canRenderNode(); },

        
        // Options
        

        "setOption", [](MaterialPtr& mat, int num, int value) { mat->setOption(num, value); },
        "getOption", [](const MaterialPtr& mat, int num) { return mat->getOption(num); },
        "isOptionOverridden", [](const MaterialPtr& mat, int num) { return mat->isOptionOverridden(num); },
        "resetOption", [](MaterialPtr& mat, int num) { mat->resetOption(num); },

        // Transparency
        "setTransparent", [](MaterialPtr& mat, int transparent) { mat->setTransparent(transparent); },
        "getTransparent", [](const MaterialPtr& mat) { return mat->getTransparent(); },
        "isWater", [](const MaterialPtr& mat) { return mat->isWater(); },
        "isDeferred", [](const MaterialPtr& mat) { return mat->isDeferred(); },
        "isForward", [](const MaterialPtr& mat) { return mat->isForward(); },
        "isAlphaTest", [](const MaterialPtr& mat) { return mat->isAlphaTest(); },

        // Blending
        "setBlendDestFunc", [](MaterialPtr& mat, int func) { mat->setBlendDestFunc(func); },
        "getBlendDestFunc", [](const MaterialPtr& mat) { return mat->getBlendDestFunc(); },
        "setBlendSrcFunc", [](MaterialPtr& mat, int func) { mat->setBlendSrcFunc(func); },
        "getBlendSrcFunc", [](const MaterialPtr& mat) { return mat->getBlendSrcFunc(); },
        "setBlendAlphaSrcFunc", [](MaterialPtr& mat, int func) { mat->setBlendAlphaSrcFunc(func); },
        "getBlendAlphaSrcFunc", [](const MaterialPtr& mat) { return mat->getBlendAlphaSrcFunc(); },

        // Masks and order
        "setShadowMask", [](MaterialPtr& mat, int mask) { mat->setShadowMask(mask); },
        "getShadowMask", [](const MaterialPtr& mat) { return mat->getShadowMask(); },
        "setViewportMask", [](MaterialPtr& mat, int mask) { mat->setViewportMask(mask); },
        "getViewportMask", [](const MaterialPtr& mat) { return mat->getViewportMask(); },
        "setOrder", [](MaterialPtr& mat, int order) { mat->setOrder(order); },
        "getOrder", [](const MaterialPtr& mat) { return mat->getOrder(); },
        "setOrderClouds", [](MaterialPtr& mat, int clouds) { mat->setOrderClouds(clouds); },
        "getOrderClouds", [](const MaterialPtr& mat) { return mat->getOrderClouds(); },

        // Rendering options
        "setCastShadow", [](MaterialPtr& mat, bool shadow) { mat->setCastShadow(shadow); },
        "isCastShadow", [](const MaterialPtr& mat) { return mat->isCastShadow(); },
        "setCastWorldShadow", [](MaterialPtr& mat, bool shadow) { mat->setCastWorldShadow(shadow); },
        "isCastWorldShadow", [](const MaterialPtr& mat) { return mat->isCastWorldShadow(); },
        "setDepthTest", [](MaterialPtr& mat, bool test) { mat->setDepthTest(test); },
        "isDepthTest", [](const MaterialPtr& mat) { return mat->isDepthTest(); },
        "setTwoSided", [](MaterialPtr& mat, bool sided) { mat->setTwoSided(sided); },
        "isTwoSided", [](const MaterialPtr& mat) { return mat->isTwoSided(); },
        "setOverlap", [](MaterialPtr& mat, bool overlap) { mat->setOverlap(overlap); },
        "isOverlap", [](const MaterialPtr& mat) { return mat->isOverlap(); },

        
        // Parameters (Float)
        

        "getNumParameters", [](const MaterialPtr& mat) { return mat->getNumParameters(); },
        "findParameter", [](const MaterialPtr& mat, const char* name) { return mat->findParameter(name); },
        "resetParameter", [](MaterialPtr& mat, int num) { mat->resetParameter(num); },
        "checkParameterConditions", [](const MaterialPtr& mat, int num) { return mat->checkParameterConditions(num); },
        "getParameterType", [](const MaterialPtr& mat, int num) { return mat->getParameterType(num); },
        "isParameterInt", [](const MaterialPtr& mat, int num) { return mat->isParameterInt(num); },
        "isParameterFloat", [](const MaterialPtr& mat, int num) { return mat->isParameterFloat(num); },
        "isParameterOverridden", [](const MaterialPtr& mat, int num) { return mat->isParameterOverridden(num); },
        "getParameterName", [](const MaterialPtr& mat, int num) { return mat->getParameterName(num); },

        // Parameter expressions
        "isParameterExpressionEnabled", [](const MaterialPtr& mat, int num) { return mat->isParameterExpressionEnabled(num); },
        "setParameterExpressionEnabled", [](MaterialPtr& mat, int num, bool enabled) { mat->setParameterExpressionEnabled(num, enabled); },
        "getParameterExpression", [](const MaterialPtr& mat, int num) { return mat->getParameterExpression(num); },
        "setParameterExpression", [](MaterialPtr& mat, int num, const char* expr) { return mat->setParameterExpression(num, expr); },

        // Float parameters
        "setParameterFloat", sol::overload(
            [](MaterialPtr& mat, int num, float value) { mat->setParameterFloat(num, value); },
            [](MaterialPtr& mat, const char* name, float value) { mat->setParameterFloat(name, value); }
        ),
        "getParameterFloat", sol::overload(
            [](const MaterialPtr& mat, int num) { return mat->getParameterFloat(num); },
            [](const MaterialPtr& mat, const char* name) { return mat->getParameterFloat(name); }
        ),

        // Float2 parameters
        "setParameterFloat2", sol::overload(
            [](MaterialPtr& mat, int num, const vec2& value) { mat->setParameterFloat2(num, value); },
            [](MaterialPtr& mat, const char* name, const vec2& value) { mat->setParameterFloat2(name, value); }
        ),
        "getParameterFloat2", sol::overload(
            [](const MaterialPtr& mat, int num) { return mat->getParameterFloat2(num); },
            [](const MaterialPtr& mat, const char* name) { return mat->getParameterFloat2(name); }
        ),

        // Float3 parameters
        "setParameterFloat3", sol::overload(
            [](MaterialPtr& mat, int num, const vec3& value) { mat->setParameterFloat3(num, value); },
            [](MaterialPtr& mat, const char* name, const vec3& value) { mat->setParameterFloat3(name, value); }
        ),
        "getParameterFloat3", sol::overload(
            [](const MaterialPtr& mat, int num) { return mat->getParameterFloat3(num); },
            [](const MaterialPtr& mat, const char* name) { return mat->getParameterFloat3(name); }
        ),

        // Float4 parameters (colors)
        "setParameterFloat4", sol::overload(
            [](MaterialPtr& mat, int num, const vec4& value) { mat->setParameterFloat4(num, value); },
            [](MaterialPtr& mat, const char* name, const vec4& value) { mat->setParameterFloat4(name, value); }
        ),
        "getParameterFloat4", sol::overload(
            [](const MaterialPtr& mat, int num) { return mat->getParameterFloat4(num); },
            [](const MaterialPtr& mat, const char* name) { return mat->getParameterFloat4(name); }
        ),

        
        // Parameters (Int)
        

        "setParameterInt", sol::overload(
            [](MaterialPtr& mat, int num, int value) { mat->setParameterInt(num, value); },
            [](MaterialPtr& mat, const char* name, int value) { mat->setParameterInt(name, value); }
        ),
        "getParameterInt", sol::overload(
            [](const MaterialPtr& mat, int num) { return mat->getParameterInt(num); },
            [](const MaterialPtr& mat, const char* name) { return mat->getParameterInt(name); }
        ),

        // Int2 parameters
        "setParameterInt2", sol::overload(
            [](MaterialPtr& mat, int num, const ivec2& value) { mat->setParameterInt2(num, value); },
            [](MaterialPtr& mat, const char* name, const ivec2& value) { mat->setParameterInt2(name, value); }
        ),
        "getParameterInt2", sol::overload(
            [](const MaterialPtr& mat, int num) { return mat->getParameterInt2(num); },
            [](const MaterialPtr& mat, const char* name) { return mat->getParameterInt2(name); }
        ),

        // Int3 parameters
        "setParameterInt3", sol::overload(
            [](MaterialPtr& mat, int num, const ivec3& value) { mat->setParameterInt3(num, value); },
            [](MaterialPtr& mat, const char* name, const ivec3& value) { mat->setParameterInt3(name, value); }
        ),
        "getParameterInt3", sol::overload(
            [](const MaterialPtr& mat, int num) { return mat->getParameterInt3(num); },
            [](const MaterialPtr& mat, const char* name) { return mat->getParameterInt3(name); }
        ),

        // Int4 parameters
        "setParameterInt4", sol::overload(
            [](MaterialPtr& mat, int num, const ivec4& value) { mat->setParameterInt4(num, value); },
            [](MaterialPtr& mat, const char* name, const ivec4& value) { mat->setParameterInt4(name, value); }
        ),
        "getParameterInt4", sol::overload(
            [](const MaterialPtr& mat, int num) { return mat->getParameterInt4(num); },
            [](const MaterialPtr& mat, const char* name) { return mat->getParameterInt4(name); }
        ),

        
        // States
        "getNumStates", [](const MaterialPtr& mat) { return mat->getNumStates(); },
        "findState", [](const MaterialPtr& mat, const char* name) { return mat->findState(name); },
        "isStateOverridden", [](const MaterialPtr& mat, int num) { return mat->isStateOverridden(num); },
        "isStateInternal", [](const MaterialPtr& mat, int num) { return mat->isStateInternal(num); },
        "resetState", [](MaterialPtr& mat, int num) { mat->resetState(num); },
        "checkStateConditions", [](const MaterialPtr& mat, int num) { return mat->checkStateConditions(num); },
        "getStateName", [](const MaterialPtr& mat, int num) { return mat->getStateName(num); },
        "getStateSwitchItem", [](const MaterialPtr& mat, int num, int item) { return mat->getStateSwitchItem(num, item); },
        "getStateSwitchNumItems", [](const MaterialPtr& mat, int num) { return mat->getStateSwitchNumItems(num); },
        "getStateType", [](const MaterialPtr& mat, int num) { return mat->getStateType(num); },

        "setState", sol::overload(
            [](MaterialPtr& mat, int num, int value) { mat->setState(num, value); },
            [](MaterialPtr& mat, const char* name, int value) { mat->setState(name, value); }
        ),
        "getState", sol::overload(
            [](const MaterialPtr& mat, int num) { return mat->getState(num); },
            [](const MaterialPtr& mat, const char* name) { return mat->getState(name); }
        ),

        
        // Textures
        

        "getNumTextures", [](const MaterialPtr& mat) { return mat->getNumTextures(); },
        "findTexture", [](const MaterialPtr& mat, const char* name) { return mat->findTexture(name); },
        "isTextureOverridden", [](const MaterialPtr& mat, int num) { return mat->isTextureOverridden(num); },
        "isTextureLoaded", [](const MaterialPtr& mat, int num) { return mat->isTextureLoaded(num); },
        "isTextureInternal", [](const MaterialPtr& mat, int num) { return mat->isTextureInternal(num); },
        "resetTexture", [](MaterialPtr& mat, int num) { mat->resetTexture(num); },
        "checkTextureConditions", [](const MaterialPtr& mat, int num) { return mat->checkTextureConditions(num); },
        "getTextureName", [](const MaterialPtr& mat, int num) { return mat->getTextureName(num); },
        "getTextureUnit", [](const MaterialPtr& mat, int num) { return mat->getTextureUnit(num); },
        "isTextureEditable", [](const MaterialPtr& mat, int num) { return mat->isTextureEditable(num); },
        "getTextureSource", [](const MaterialPtr& mat, int num) { return mat->getTextureSource(num); },

        // Texture sampler and format
        "getTextureSamplerFlags", [](const MaterialPtr& mat, int num) { return mat->getTextureSamplerFlags(num); },
        "setTextureSamplerFlags", [](MaterialPtr& mat, int num, int flags) { mat->setTextureSamplerFlags(num, flags); },
        "getTextureFormatFlags", [](const MaterialPtr& mat, int num) { return mat->getTextureFormatFlags(num); },

        // Texture streaming
        "setTextureStreamingDensityMultiplier", [](MaterialPtr& mat, int num, float mult) {
            mat->setTextureStreamingDensityMultiplier(num, mult);
        },
        "getTextureStreamingDensityMultiplier", [](const MaterialPtr& mat, int num) {
            return mat->getTextureStreamingDensityMultiplier(num);
        },

        // Texture get/set
        "getTexture", sol::overload(
            [](MaterialPtr& mat, int num) { return mat->getTexture(num); },
            [](MaterialPtr& mat, const char* name) { return mat->getTexture(name); }
        ),
        "setTexture", sol::overload(
            [](MaterialPtr& mat, int num, const TexturePtr& tex) { mat->setTexture(num, tex); },
            [](MaterialPtr& mat, const char* name, const TexturePtr& tex) { mat->setTexture(name, tex); }
        ),

        // Texture path
        "setTexturePath", sol::overload(
            [](MaterialPtr& mat, int num, const char* path) { mat->setTexturePath(num, path); },
            [](MaterialPtr& mat, const char* name, const char* path) { mat->setTexturePath(name, path); }
        ),
        "getTexturePath", sol::overload(
            [](const MaterialPtr& mat, int num) { return mat->getTexturePath(num); },
            [](const MaterialPtr& mat, const char* name) { return mat->getTexturePath(name); }
        ),

        
        // Properties/Flags
        

        "isEditable", [](const MaterialPtr& mat) { return mat->isEditable(); },
        "isHidden", [](const MaterialPtr& mat) { return mat->isHidden(); },
        "isBase", [](const MaterialPtr& mat) { return mat->isBase(); },
        "isBrush", [](const MaterialPtr& mat) { return mat->isBrush(); },
        "isLegacy", [](const MaterialPtr& mat) { return mat->isLegacy(); },
        "isPreviewHidden", [](const MaterialPtr& mat) { return mat->isPreviewHidden(); },
        "isReflection2D", [](const MaterialPtr& mat) { return mat->isReflection2D(); },
        "isInternal", [](const MaterialPtr& mat) { return mat->isInternal(); },
        "isManual", [](const MaterialPtr& mat) { return mat->isManual(); },
        "canSave", [](const MaterialPtr& mat) { return mat->canSave(); },
        "isAutoSave", [](const MaterialPtr& mat) { return mat->isAutoSave(); },
        "isFileEngine", [](const MaterialPtr& mat) { return mat->isFileEngine(); },
        "isEmpty", [](const MaterialPtr& mat) { return mat->isEmpty(); },
        "hasOverrides", [](const MaterialPtr& mat) { return mat->hasOverrides(); },

        
        // Save/Load
        

        "load", [](MaterialPtr& mat, const char* path) { return mat->load(path); },
        "save", [](MaterialPtr& mat) { return mat->save(); },
        "reload", [](MaterialPtr& mat) { return mat->reload(); },
        "createMaterialFile", [](MaterialPtr& mat, const char* path) { return mat->createMaterialFile(path); },

        // Shader management
        "createShaderCache", sol::overload(
            [](MaterialPtr& mat) { mat->createShaderCache(); },
            [](MaterialPtr& mat, bool recursive) { mat->createShaderCache(recursive); }
        ),
        "createShaders", sol::overload(
            [](MaterialPtr& mat) { mat->createShaders(); },
            [](MaterialPtr& mat, bool recursive) { mat->createShaders(recursive); }
        ),
        "destroyTextures", [](MaterialPtr& mat) { mat->destroyTextures(); },

        // String representation
        sol::meta_function::to_string, [](const MaterialPtr& mat) {
            if (!mat) return std::string("Material(null)");
            return std::string("Material(") + mat->getFilePath().get() + ")";
        }
    );

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] Material types registered (150+ properties)\n");
}

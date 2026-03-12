#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include <UnigineLights.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

void Unigine::LuaBindings::registerLightTypes(sol::state& lua)
{
    // ============================================================================
    // Light Enumerations
    // ============================================================================

    lua.new_enum("LightShape",
        "DEFAULT", Light::SHAPE_DEFAULT,
        "POINT", Light::SHAPE_POINT,
        "SPHERE", Light::SHAPE_SPHERE,
        "CAPSULE", Light::SHAPE_CAPSULE,
        "RECTANGLE", Light::SHAPE_RECTANGLE
    );

    lua.new_enum("LightMode",
        "DYNAMIC", Light::MODE_DYNAMIC,
        "STATIC", Light::MODE_STATIC
    );

    lua.new_enum("LightShadowMode",
        "MIXED", Light::SHADOW_MODE_MIXED,
        "STATIC", Light::SHADOW_MODE_STATIC
    );

    lua.new_enum("LightColorMode",
        "CLASSIC", Light::COLOR_MODE_CLASSIC,
        "TEMPERATURE", Light::COLOR_MODE_TEMPERATURE
    );

    lua.new_enum("LightShadowFilter",
        "GLOBAL", Light::SHADOW_FILTER_GLOBAL,
        "DISABLED", Light::SHADOW_FILTER_DISABLED,
        "LOW", Light::SHADOW_FILTER_LOW,
        "MEDIUM", Light::SHADOW_FILTER_MEDIUM,
        "HIGH", Light::SHADOW_FILTER_HIGH,
        "ULTRA", Light::SHADOW_FILTER_ULTRA
    );

    lua.new_enum("LightShadowPenumbra",
        "GLOBAL", Light::SHADOW_PENUMBRA_GLOBAL,
        "DISABLED", Light::SHADOW_PENUMBRA_DISABLED,
        "LOW", Light::SHADOW_PENUMBRA_LOW,
        "MEDIUM", Light::SHADOW_PENUMBRA_MEDIUM,
        "HIGH", Light::SHADOW_PENUMBRA_HIGH,
        "ULTRA", Light::SHADOW_PENUMBRA_ULTRA
    );

    lua.new_enum("LightShadowResolution",
        "MODE_DEFAULT", Light::SHADOW_RESOLUTION_MODE_DEFAULT,
        "MODE_64", Light::SHADOW_RESOLUTION_MODE_64,
        "MODE_128", Light::SHADOW_RESOLUTION_MODE_128,
        "MODE_256", Light::SHADOW_RESOLUTION_MODE_256,
        "MODE_512", Light::SHADOW_RESOLUTION_MODE_512,
        "MODE_1024", Light::SHADOW_RESOLUTION_MODE_1024,
        "MODE_2048", Light::SHADOW_RESOLUTION_MODE_2048,
        "MODE_4096", Light::SHADOW_RESOLUTION_MODE_4096,
        "MODE_8192", Light::SHADOW_RESOLUTION_MODE_8192,
        "MODE_16384", Light::SHADOW_RESOLUTION_MODE_16384
    );

    lua.new_enum("LightShadowColorMode",
        "IES", Light::SHADOW_COLOR_MODE_IES,
        "SIMPLE", Light::SHADOW_COLOR_MODE_SIMPLE
    );

    // LightWorld enums
    lua.new_enum("LightWorldShadowCascadeMode",
        "DYNAMIC", LightWorld::SHADOW_CASCADE_MODE_DYNAMIC,
        "STATIC", LightWorld::SHADOW_CASCADE_MODE_STATIC
    );

    lua.new_enum("LightWorldScattering",
        "NONE", LightWorld::SCATTERING_NONE,
        "SUN", LightWorld::SCATTERING_SUN,
        "MOON", LightWorld::SCATTERING_MOON
    );

    // LightVoxelProbe enums
    lua.new_enum("LightVoxelProbeBakeInternalVolume",
        "HALF", LightVoxelProbe::BAKE_INTERNAL_VOLUME_HALF,
        "FULL", LightVoxelProbe::BAKE_INTERNAL_VOLUME_FULL
    );

    lua.new_enum("LightVoxelProbeBakeQuality",
        "DRAFT", LightVoxelProbe::BAKE_QUALITY_DRAFT,
        "LOW", LightVoxelProbe::BAKE_QUALITY_LOW,
        "MEDIUM", LightVoxelProbe::BAKE_QUALITY_MEDIUM,
        "HIGH", LightVoxelProbe::BAKE_QUALITY_HIGH,
        "ULTRA", LightVoxelProbe::BAKE_QUALITY_ULTRA
    );

    lua.new_enum("LightVoxelProbeBlend",
        "ALPHA_BLEND", LightVoxelProbe::BLEND_ALPHA_BLEND,
        "ADDITIVE", LightVoxelProbe::BLEND_ADDITIVE,
        "MULTIPLICATIVE", LightVoxelProbe::BLEND_MULTIPLICATIVE
    );

    // LightEnvironmentProbe enums
    lua.new_enum("LightEnvironmentProbeProjectionMode",
        "SPHERE", LightEnvironmentProbe::PROJECTION_MODE_SPHERE,
        "BOX", LightEnvironmentProbe::PROJECTION_MODE_BOX,
        "RAYMARCHING", LightEnvironmentProbe::PROJECTION_MODE_RAYMARCHING
    );

    lua.new_enum("LightEnvironmentProbeGrabMode",
        "BAKED", LightEnvironmentProbe::GRAB_MODE_BAKED,
        "DYNAMIC", LightEnvironmentProbe::GRAB_MODE_DYNAMIC
    );

    lua.new_enum("LightEnvironmentProbeGrabResolution",
        "MODE_32", LightEnvironmentProbe::GRAB_RESOLUTION_MODE_32,
        "MODE_64", LightEnvironmentProbe::GRAB_RESOLUTION_MODE_64,
        "MODE_128", LightEnvironmentProbe::GRAB_RESOLUTION_MODE_128,
        "MODE_256", LightEnvironmentProbe::GRAB_RESOLUTION_MODE_256,
        "MODE_512", LightEnvironmentProbe::GRAB_RESOLUTION_MODE_512,
        "MODE_1024", LightEnvironmentProbe::GRAB_RESOLUTION_MODE_1024,
        "MODE_2048", LightEnvironmentProbe::GRAB_RESOLUTION_MODE_2048,
        "MODE_4096", LightEnvironmentProbe::GRAB_RESOLUTION_MODE_4096
    );

    // LightPlanarProbe enums
    lua.new_enum("LightPlanarProbeReflectionResolution",
        "MODE_HEIGHT", LightPlanarProbe::REFLECTION_RESOLUTION_MODE_HEIGHT,
        "MODE_HALF_HEIGHT", LightPlanarProbe::REFLECTION_RESOLUTION_MODE_HALF_HEIGHT,
        "MODE_QUART_HEIGHT", LightPlanarProbe::REFLECTION_RESOLUTION_MODE_QUART_HEIGHT,
        "MODE_128", LightPlanarProbe::REFLECTION_RESOLUTION_MODE_128,
        "MODE_256", LightPlanarProbe::REFLECTION_RESOLUTION_MODE_256,
        "MODE_512", LightPlanarProbe::REFLECTION_RESOLUTION_MODE_512,
        "MODE_1024", LightPlanarProbe::REFLECTION_RESOLUTION_MODE_1024,
        "MODE_2048", LightPlanarProbe::REFLECTION_RESOLUTION_MODE_2048,
        "MODE_4096", LightPlanarProbe::REFLECTION_RESOLUTION_MODE_4096
    );

    // ============================================================================
    // LightLensFlare
    // ============================================================================

    lua.new_usertype<LightLensFlarePtr>("LightLensFlare",
        sol::no_constructor,

        "setName", [](LightLensFlarePtr& flare, const char* name) { flare->setName(name); },
        "getName", [](const LightLensFlarePtr& flare) { return flare->getName(); },
        "setSize", [](LightLensFlarePtr& flare, float size) { flare->setSize(size); },
        "getSize", [](const LightLensFlarePtr& flare) { return flare->getSize(); },
        "setOffset", [](LightLensFlarePtr& flare, float offset) { flare->setOffset(offset); },
        "getOffset", [](const LightLensFlarePtr& flare) { return flare->getOffset(); },
        "setOffsetScale", [](LightLensFlarePtr& flare, float scale) { flare->setOffsetScale(scale); },
        "getOffsetScale", [](const LightLensFlarePtr& flare) { return flare->getOffsetScale(); },
        "setUVLowerLeft", [](LightLensFlarePtr& flare, const vec2& left) { flare->setUVLowerLeft(left); },
        "getUVLowerLeft", [](const LightLensFlarePtr& flare) { return flare->getUVLowerLeft(); },
        "setUVUpperRight", [](LightLensFlarePtr& flare, const vec2& right) { flare->setUVUpperRight(right); },
        "getUVUpperRight", [](const LightLensFlarePtr& flare) { return flare->getUVUpperRight(); },
        "setRotate", [](LightLensFlarePtr& flare, bool rotate) { flare->setRotate(rotate); },
        "isRotate", [](const LightLensFlarePtr& flare) { return flare->isRotate(); },
        "setColor", [](LightLensFlarePtr& flare, const vec4& color) { flare->setColor(color); },
        "getColor", [](const LightLensFlarePtr& flare) { return flare->getColor(); },
        "setIntensity", [](LightLensFlarePtr& flare, float intensity) { flare->setIntensity(intensity); },
        "getIntensity", [](const LightLensFlarePtr& flare) { return flare->getIntensity(); }
    );

    // ============================================================================
    // Base Light Class
    // ============================================================================

    lua.new_usertype<LightPtr>("Light",
        sol::no_constructor,

        // Lens Flares
        "getLensFlare", [](const LightPtr& light, int num) { return light->getLensFlare(num); },
        "setLensFlaresEnabled", [](LightPtr& light, bool enabled) { light->setLensFlaresEnabled(enabled); },
        "isLensFlaresEnabled", [](const LightPtr& light) { return light->isLensFlaresEnabled(); },
        "setLensFlaresTextureName", [](LightPtr& light, const char* name) { light->setLensFlaresTextureName(name); },
        "getLensFlaresTextureName", [](const LightPtr& light) { return light->getLensFlaresTextureName(); },
        "setLensFlaresIntensity", [](LightPtr& light, float intensity) { light->setLensFlaresIntensity(intensity); },
        "getLensFlaresIntensity", [](const LightPtr& light) { return light->getLensFlaresIntensity(); },
        "setLensFlaresOcclusionFade", [](LightPtr& light, float fade) { light->setLensFlaresOcclusionFade(fade); },
        "getLensFlaresOcclusionFade", [](const LightPtr& light) { return light->getLensFlaresOcclusionFade(); },
        "setLensFlaresOcclusionFadeBorder", [](LightPtr& light, float border) { light->setLensFlaresOcclusionFadeBorder(border); },
        "getLensFlaresOcclusionFadeBorder", [](const LightPtr& light) { return light->getLensFlaresOcclusionFadeBorder(); },
        "setLensFlaresWorldPositionOffset", [](LightPtr& light, const vec3& offset) { light->setLensFlaresWorldPositionOffset(offset); },
        "getLensFlaresWorldPositionOffset", [](const LightPtr& light) { return light->getLensFlaresWorldPositionOffset(); },
        "setLensFlaresUseLightColor", [](LightPtr& light, bool color) { light->setLensFlaresUseLightColor(color); },
        "isLensFlaresUseLightColor", [](const LightPtr& light) { return light->isLensFlaresUseLightColor(); },
        "allocateLensFlares", [](LightPtr& light, int num) { light->allocateLensFlares(num); },
        "getNumLensFlares", [](const LightPtr& light) { return light->getNumLensFlares(); },
        "addLensFlare", [](LightPtr& light) { light->addLensFlare(); },
        "removeLensFlare", [](LightPtr& light, int num) { light->removeLensFlare(num); },
        "cloneLensFlare", [](LightPtr& light, int num) { light->cloneLensFlare(num); },
        "clearLensFlares", [](LightPtr& light) { light->clearLensFlares(); },
        "calculateFinalColor", [](const LightPtr& light) { return light->calculateFinalColor(); },

        // Visibility and Distance
        "setVisibleDistance", [](LightPtr& light, float distance) { light->setVisibleDistance(distance); },
        "getVisibleDistance", [](const LightPtr& light) { return light->getVisibleDistance(); },
        "setFadeDistance", [](LightPtr& light, float distance) { light->setFadeDistance(distance); },
        "getFadeDistance", [](const LightPtr& light) { return light->getFadeDistance(); },
        "setShadowDistance", [](LightPtr& light, float distance) { light->setShadowDistance(distance); },
        "getShadowDistance", [](const LightPtr& light) { return light->getShadowDistance(); },
        "setShadowFadeDistance", [](LightPtr& light, float distance) { light->setShadowFadeDistance(distance); },
        "getShadowFadeDistance", [](const LightPtr& light) { return light->getShadowFadeDistance(); },

        // Rendering
        "setRenderOnWater", [](LightPtr& light, bool water) { light->setRenderOnWater(water); },
        "isRenderOnWater", [](const LightPtr& light) { return light->isRenderOnWater(); },
        "setRenderOnTransparent", [](LightPtr& light, bool transparent) { light->setRenderOnTransparent(transparent); },
        "isRenderOnTransparent", [](const LightPtr& light) { return light->isRenderOnTransparent(); },
        "setOrder", [](LightPtr& light, int order) { light->setOrder(order); },
        "getOrder", [](const LightPtr& light) { return light->getOrder(); },

        // Masks
        "setShadowMask", [](LightPtr& light, int mask) { light->setShadowMask(mask); },
        "getShadowMask", [](const LightPtr& light) { return light->getShadowMask(); },
        "setViewportMask", [](LightPtr& light, int mask) { light->setViewportMask(mask); },
        "getViewportMask", [](const LightPtr& light) { return light->getViewportMask(); },

        // Color and Intensity
        "setColorMode", [](LightPtr& light, int mode) { light->setColorMode((Light::COLOR_MODE)mode); },
        "getColorMode", [](const LightPtr& light) { return (int)light->getColorMode(); },
        "setColor", [](LightPtr& light, const vec4& color) { light->setColor(color); },
        "getColor", [](const LightPtr& light) { return light->getColor(); },
        "setIntensity", [](LightPtr& light, float intensity) { light->setIntensity(intensity); },
        "getIntensity", [](const LightPtr& light) { return light->getIntensity(); },
        "setLux", [](LightPtr& light, float lux) { light->setLux(lux); },
        "getLux", [](const LightPtr& light) { return light->getLux(); },
        "setColorTemperature", [](LightPtr& light, float temperature) { light->setColorTemperature(temperature); },
        "getColorTemperature", [](const LightPtr& light) { return light->getColorTemperature(); },
        "setColorFilter", [](LightPtr& light, const vec4& filter) { light->setColorFilter(filter); },
        "getColorFilter", [](const LightPtr& light) { return light->getColorFilter(); },
        "setAttenuationPower", [](LightPtr& light, float power) { light->setAttenuationPower(power); },
        "getAttenuationPower", [](const LightPtr& light) { return light->getAttenuationPower(); },

        // Shadows
        "setShadow", [](LightPtr& light, bool shadow) { light->setShadow(shadow); },
        "getShadow", [](const LightPtr& light) { return light->getShadow(); },
        "setShadowResolution", [](LightPtr& light, int resolution) { light->setShadowResolution((Light::SHADOW_RESOLUTION)resolution); },
        "getShadowResolution", [](const LightPtr& light) { return (int)light->getShadowResolution(); },
        "setShadowBias", [](LightPtr& light, float bias) { light->setShadowBias(bias); },
        "getShadowBias", [](const LightPtr& light) { return light->getShadowBias(); },
        "setShadowNormalBias", [](LightPtr& light, float bias) { light->setShadowNormalBias(bias); },
        "getShadowNormalBias", [](const LightPtr& light) { return light->getShadowNormalBias(); },
        "setMode", [](LightPtr& light, int mode) { light->setMode((Light::MODE)mode); },
        "getMode", [](const LightPtr& light) { return (int)light->getMode(); },
        "setShadowMode", [](LightPtr& light, int mode) { light->setShadowMode((Light::SHADOW_MODE)mode); },
        "getShadowMode", [](const LightPtr& light) { return (int)light->getShadowMode(); },
        "setShadowFilterMode", [](LightPtr& light, int mode) { light->setShadowFilterMode((Light::SHADOW_FILTER)mode); },
        "getShadowFilterMode", [](const LightPtr& light) { return (int)light->getShadowFilterMode(); },
        "setShadowFilter", [](LightPtr& light, float filter) { light->setShadowFilter(filter); },
        "getShadowFilter", [](const LightPtr& light) { return light->getShadowFilter(); },
        "setShadowPenumbraMode", [](LightPtr& light, int mode) { light->setShadowPenumbraMode((Light::SHADOW_PENUMBRA)mode); },
        "getShadowPenumbraMode", [](const LightPtr& light) { return (int)light->getShadowPenumbraMode(); },
        "setShadowPenumbra", [](LightPtr& light, float penumbra) { light->setShadowPenumbra(penumbra); },
        "getShadowPenumbra", [](const LightPtr& light) { return light->getShadowPenumbra(); },
        "setShadowColorTextureMode", [](LightPtr& light, int mode) { light->setShadowColorTextureMode((Light::SHADOW_COLOR_MODE)mode); },
        "getShadowColorTextureMode", [](const LightPtr& light) { return (int)light->getShadowColorTextureMode(); },
        "setShadowScreenSpace", [](LightPtr& light, bool space) { light->setShadowScreenSpace(space); },
        "isShadowScreenSpace", [](const LightPtr& light) { return light->isShadowScreenSpace(); },
        "setShadowScreenSpaceNumRays", [](LightPtr& light, int rays) { light->setShadowScreenSpaceNumRays(rays); },
        "getShadowScreenSpaceNumRays", [](const LightPtr& light) { return light->getShadowScreenSpaceNumRays(); },
        "setShadowScreenSpaceNumSteps", [](LightPtr& light, int steps) { light->setShadowScreenSpaceNumSteps(steps); },
        "getShadowScreenSpaceNumSteps", [](const LightPtr& light) { return light->getShadowScreenSpaceNumSteps(); },
        "getDynamicDepthTexture", [](const LightPtr& light) { return light->getDynamicDepthTexture(); },
        "setBakedDepthTextureFilePath", [](LightPtr& light, const char* path) { light->setBakedDepthTextureFilePath(path); },
        "getBakedDepthTextureFilePath", [](const LightPtr& light) { return light->getBakedDepthTextureFilePath(); },
        "getBakedDepthTexture", [](const LightPtr& light) { return light->getBakedDepthTexture(); },
        "getDynamicShadowColorTexture", [](const LightPtr& light) { return light->getDynamicShadowColorTexture(); },
        "getShadowTexture", [](const LightPtr& light) { return light->getShadowTexture(); },

        // String representation
        sol::meta_function::to_string, [](const LightPtr& light) {
            if (!light) return std::string("Light(null)");
            return std::string("Light(") + std::string(light->getName()) + ")";
        }
    );

    // ============================================================================
    // LightOmni
    // ============================================================================

    lua.new_usertype<LightOmniPtr>("LightOmni",
        sol::no_constructor,

        "create", sol::overload(
            [](const vec4& color, float attenuation_distance) { return LightOmni::create(color, attenuation_distance); },
            [](const vec4& color, float attenuation_distance, const char* name) { return LightOmni::create(color, attenuation_distance, name); }
        ),

        "setAttenuationDistance", [](LightOmniPtr& light, float distance) { light->setAttenuationDistance(distance); },
        "getAttenuationDistance", [](const LightOmniPtr& light) { return light->getAttenuationDistance(); },
        "setNearAttenuationDistance", [](LightOmniPtr& light, float distance) { light->setNearAttenuationDistance(distance); },
        "getNearAttenuationDistance", [](const LightOmniPtr& light) { return light->getNearAttenuationDistance(); },
        "setNearAttenuationGradientLength", [](LightOmniPtr& light, float length) { light->setNearAttenuationGradientLength(length); },
        "getNearAttenuationGradientLength", [](const LightOmniPtr& light) { return light->getNearAttenuationGradientLength(); },
        "setShadowNearClippingDistance", [](LightOmniPtr& light, float distance) { light->setShadowNearClippingDistance(distance); },
        "getShadowNearClippingDistance", [](const LightOmniPtr& light) { return light->getShadowNearClippingDistance(); },
        "setShapeRadius", [](LightOmniPtr& light, float radius) { light->setShapeRadius(radius); },
        "getShapeRadius", [](const LightOmniPtr& light) { return light->getShapeRadius(); },
        "setShapeLength", [](LightOmniPtr& light, float length) { light->setShapeLength(length); },
        "getShapeLength", [](const LightOmniPtr& light) { return light->getShapeLength(); },
        "setShapeHeight", [](LightOmniPtr& light, float height) { light->setShapeHeight(height); },
        "getShapeHeight", [](const LightOmniPtr& light) { return light->getShapeHeight(); },
        "setShapeType", [](LightOmniPtr& light, int type) { light->setShapeType((Light::SHAPE)type); },
        "getShapeType", [](const LightOmniPtr& light) { return (int)light->getShapeType(); },
        "getSize", [](const LightOmniPtr& light) { return light->getSize(); },
        "getShadowDepthRange", [](const LightOmniPtr& light) { return light->getShadowDepthRange(); },
        "setTextureFilePath", [](LightOmniPtr& light, const char* path) { light->setTextureFilePath(path); },
        "getTextureFilePath", [](const LightOmniPtr& light) { return light->getTextureFilePath(); },
        "setTexture", [](LightOmniPtr& light, const TexturePtr& texture) { light->setTexture(texture); },
        "getTexture", [](const LightOmniPtr& light) { return light->getTexture(); },
        "setShadowSideEnabled", [](LightOmniPtr& light, int side, int enable) { light->setShadowSideEnabled(side, enable); },
        "isShadowSideEnabled", [](const LightOmniPtr& light, int side) { return light->isShadowSideEnabled(side); }
    );

    // ============================================================================
    // LightProj
    // ============================================================================

    lua.new_usertype<LightProjPtr>("LightProj",
        sol::no_constructor,

        "create", sol::overload(
            [](const vec4& color, float attenuation_distance, float fov) { return LightProj::create(color, attenuation_distance, fov); },
            [](const vec4& color, float attenuation_distance, float fov, const char* name) { return LightProj::create(color, attenuation_distance, fov, name); }
        ),

        "setAttenuationDistance", [](LightProjPtr& light, float distance) { light->setAttenuationDistance(distance); },
        "getAttenuationDistance", [](const LightProjPtr& light) { return light->getAttenuationDistance(); },
        "setNearAttenuationDistance", [](LightProjPtr& light, float distance) { light->setNearAttenuationDistance(distance); },
        "getNearAttenuationDistance", [](const LightProjPtr& light) { return light->getNearAttenuationDistance(); },
        "setNearAttenuationGradientLength", [](LightProjPtr& light, float length) { light->setNearAttenuationGradientLength(length); },
        "getNearAttenuationGradientLength", [](const LightProjPtr& light) { return light->getNearAttenuationGradientLength(); },
        "setShadowNearClippingDistance", [](LightProjPtr& light, float distance) { light->setShadowNearClippingDistance(distance); },
        "getShadowNearClippingDistance", [](const LightProjPtr& light) { return light->getShadowNearClippingDistance(); },
        "setShapeType", [](LightProjPtr& light, int type) { light->setShapeType((Light::SHAPE)type); },
        "getShapeType", [](const LightProjPtr& light) { return (int)light->getShapeType(); },
        "setShapeRadius", [](LightProjPtr& light, float radius) { light->setShapeRadius(radius); },
        "getShapeRadius", [](const LightProjPtr& light) { return light->getShapeRadius(); },
        "setShapeLength", [](LightProjPtr& light, float length) { light->setShapeLength(length); },
        "getShapeLength", [](const LightProjPtr& light) { return light->getShapeLength(); },
        "setShapeHeight", [](LightProjPtr& light, float height) { light->setShapeHeight(height); },
        "getShapeHeight", [](const LightProjPtr& light) { return light->getShapeHeight(); },
        "setFov", [](LightProjPtr& light, float fov) { light->setFov(fov); },
        "getFov", [](const LightProjPtr& light) { return light->getFov(); },
        "setPenumbra", [](LightProjPtr& light, float penumbra) { light->setPenumbra(penumbra); },
        "getPenumbra", [](const LightProjPtr& light) { return light->getPenumbra(); },
        "setIESRelativeToFov", [](LightProjPtr& light, bool fov) { light->setIESRelativeToFov(fov); },
        "isIESRelativeToFov", [](const LightProjPtr& light) { return light->isIESRelativeToFov(); },
        "getShadowDepthRange", [](const LightProjPtr& light) { return light->getShadowDepthRange(); },
        "getProjection", [](const LightProjPtr& light) { return light->getProjection(); },
        "getShadowProjection", [](const LightProjPtr& light) { return light->getShadowProjection(); },
        "setTextureFilePath", [](LightProjPtr& light, const char* path) { light->setTextureFilePath(path); },
        "getTextureFilePath", [](const LightProjPtr& light) { return light->getTextureFilePath(); },
        "setTexture", [](LightProjPtr& light, const TexturePtr& texture) { light->setTexture(texture); },
        "getTexture", [](const LightProjPtr& light) { return light->getTexture(); }
    );

    // ============================================================================
    // LightWorld
    // ============================================================================

    lua.new_usertype<LightWorldPtr>("LightWorld",
        sol::no_constructor,

        "create", [](const vec4& color) { return LightWorld::create(color); },

        "setScattering", [](LightWorldPtr& light, int scattering) { light->setScattering((LightWorld::SCATTERING)scattering); },
        "getScattering", [](const LightWorldPtr& light) { return (int)light->getScattering(); },
        "setDisableAngle", [](LightWorldPtr& light, float angle) { light->setDisableAngle(angle); },
        "getDisableAngle", [](const LightWorldPtr& light) { return light->getDisableAngle(); },
        "setShadowCascadeMode", [](LightWorldPtr& light, int mode) { light->setShadowCascadeMode((LightWorld::SHADOW_CASCADE_MODE)mode); },
        "getShadowCascadeMode", [](const LightWorldPtr& light) { return (int)light->getShadowCascadeMode(); },
        "setNumShadowCascades", [](LightWorldPtr& light, int cascades) { light->setNumShadowCascades(cascades); },
        "getNumShadowCascades", [](const LightWorldPtr& light) { return light->getNumShadowCascades(); },
        "setOneCascadePerFrame", [](LightWorldPtr& light, bool frame) { light->setOneCascadePerFrame(frame); },
        "isOneCascadePerFrame", [](const LightWorldPtr& light) { return light->isOneCascadePerFrame(); },
        "setShadowCascadeBorder", [](LightWorldPtr& light, int num, float r) { light->setShadowCascadeBorder(num, r); },
        "getShadowCascadeBorder", [](const LightWorldPtr& light, int num) { return light->getShadowCascadeBorder(num); },
        "getRenderShadowDepthRange", [](const LightWorldPtr& light) { return light->getRenderShadowDepthRange(); },
        "getRenderShadowCascadeModelview", [](const LightWorldPtr& light, int num) { return light->getRenderShadowCascadeModelview(num); },
        "getRenderShadowCascadeProjection", [](const LightWorldPtr& light, int num) { return light->getRenderShadowCascadeProjection(num); },
        "updateRenderShadowCascadeMatrices", [](LightWorldPtr& light, const Vec3& camera_position, float zfar) { light->updateRenderShadowCascadeMatrices(camera_position, zfar); },
        "setShadowHeight", [](LightWorldPtr& light, float height) { light->setShadowHeight(height); },
        "getShadowHeight", [](const LightWorldPtr& light) { return light->getShadowHeight(); },
        "setShadowWidth", [](LightWorldPtr& light, float width) { light->setShadowWidth(width); },
        "getShadowWidth", [](const LightWorldPtr& light) { return light->getShadowWidth(); },
        "setShadowZFar", [](LightWorldPtr& light, float zfar) { light->setShadowZFar(zfar); },
        "getShadowZFar", [](const LightWorldPtr& light) { return light->getShadowZFar(); },
        "setMode", [](LightWorldPtr& light, int mode) { light->setMode(mode); },
        "getMode", [](const LightWorldPtr& light) { return light->getMode(); }
    );

    // ============================================================================
    // LightVoxelProbe
    // ============================================================================

    lua.new_usertype<LightVoxelProbePtr>("LightVoxelProbe",
        sol::no_constructor,

        "create", []() { return LightVoxelProbe::create(); },

        "setBoxSize", [](LightVoxelProbePtr& probe, const vec3& size) { probe->setBoxSize(size); },
        "getBoxSize", [](const LightVoxelProbePtr& probe) { return probe->getBoxSize(); },
        "setVoxelSize", [](LightVoxelProbePtr& probe, float size) { probe->setVoxelSize(size); },
        "getVoxelSize", [](const LightVoxelProbePtr& probe) { return probe->getVoxelSize(); },
        "setAttenuationDistance", [](LightVoxelProbePtr& probe, const vec3& distance) { probe->setAttenuationDistance(distance); },
        "getAttenuationDistance", [](const LightVoxelProbePtr& probe) { return probe->getAttenuationDistance(); },
        "setAttenuationPower", [](LightVoxelProbePtr& probe, float power) { probe->setAttenuationPower(power); },
        "getAttenuationPower", [](const LightVoxelProbePtr& probe) { return probe->getAttenuationPower(); },
        "setUseSkyColor", [](LightVoxelProbePtr& probe, bool color) { probe->setUseSkyColor(color); },
        "isUseSkyColor", [](const LightVoxelProbePtr& probe) { return probe->isUseSkyColor(); },
        "setBlendMode", [](LightVoxelProbePtr& probe, int mode) { probe->setBlendMode((LightVoxelProbe::BLEND)mode); },
        "getBlendMode", [](const LightVoxelProbePtr& probe) { return (int)probe->getBlendMode(); },
        "setBakeQuality", [](LightVoxelProbePtr& probe, int quality) { probe->setBakeQuality((LightVoxelProbe::BAKE_QUALITY)quality); },
        "getBakeQuality", [](const LightVoxelProbePtr& probe) { return (int)probe->getBakeQuality(); },
        "setTextureFilePath", [](LightVoxelProbePtr& probe, const char* path) { probe->setTextureFilePath(path); },
        "getTextureFilePath", [](const LightVoxelProbePtr& probe) { return probe->getTextureFilePath(); },
        "getResolution", [](const LightVoxelProbePtr& probe) { return probe->getResolution(); },
        "getVideoMemoryUsage", [](const LightVoxelProbePtr& probe) { return probe->getVideoMemoryUsage(); }
    );

    // ============================================================================
    // LightEnvironmentProbe
    // ============================================================================

    lua.new_usertype<LightEnvironmentProbePtr>("LightEnvironmentProbe",
        sol::no_constructor,

        "create", sol::overload(
            [](const vec4& color, const vec3& attenuation_distance) { return LightEnvironmentProbe::create(color, attenuation_distance); },
            [](const vec4& color, const vec3& attenuation_distance, const char* name) { return LightEnvironmentProbe::create(color, attenuation_distance, name); }
        ),

        "setProjectionMode", [](LightEnvironmentProbePtr& probe, int mode) { probe->setProjectionMode((LightEnvironmentProbe::PROJECTION_MODE)mode); },
        "getProjectionMode", [](const LightEnvironmentProbePtr& probe) { return (int)probe->getProjectionMode(); },
        "setAttenuationDistance", [](LightEnvironmentProbePtr& probe, const vec3& distance) { probe->setAttenuationDistance(distance); },
        "getAttenuationDistance", [](const LightEnvironmentProbePtr& probe) { return probe->getAttenuationDistance(); },
        "setTextureFilePath", [](LightEnvironmentProbePtr& probe, const char* path) { probe->setTextureFilePath(path); },
        "getTextureFilePath", [](const LightEnvironmentProbePtr& probe) { return probe->getTextureFilePath(); },
        "setGrabMode", [](LightEnvironmentProbePtr& probe, int mode) { probe->setGrabMode((LightEnvironmentProbe::GRAB_MODE)mode); },
        "getGrabMode", [](const LightEnvironmentProbePtr& probe) { return (int)probe->getGrabMode(); },
        "setGrabResolution", [](LightEnvironmentProbePtr& probe, int resolution) { probe->setGrabResolution((LightEnvironmentProbe::GRAB_RESOLUTION)resolution); },
        "getGrabResolution", [](const LightEnvironmentProbePtr& probe) { return (int)probe->getGrabResolution(); }
    );

    // ============================================================================
    // LightPlanarProbe
    // ============================================================================

    lua.new_usertype<LightPlanarProbePtr>("LightPlanarProbe",
        sol::no_constructor,

        "create", []() { return LightPlanarProbe::create(); },

        "setProjectionSize", [](LightPlanarProbePtr& probe, const vec3& size) { probe->setProjectionSize(size); },
        "getProjectionSize", [](const LightPlanarProbePtr& probe) { return probe->getProjectionSize(); },
        "setAttenuationDistance", [](LightPlanarProbePtr& probe, const vec3& distance) { probe->setAttenuationDistance(distance); },
        "getAttenuationDistance", [](const LightPlanarProbePtr& probe) { return probe->getAttenuationDistance(); },
        "setReflectionResolution", [](LightPlanarProbePtr& probe, int resolution) { probe->setReflectionResolution((LightPlanarProbe::REFLECTION_RESOLUTION)resolution); },
        "getReflectionResolution", [](const LightPlanarProbePtr& probe) { return (int)probe->getReflectionResolution(); },
        "setTwoSided", [](LightPlanarProbePtr& probe, bool sided) { probe->setTwoSided(sided); },
        "isTwoSided", [](const LightPlanarProbePtr& probe) { return probe->isTwoSided(); },
        "setReflectionViewportMask", [](LightPlanarProbePtr& probe, int mask) { probe->setReflectionViewportMask(mask); },
        "getReflectionViewportMask", [](const LightPlanarProbePtr& probe) { return probe->getReflectionViewportMask(); },
        "setZNear", [](LightPlanarProbePtr& probe, float znear) { probe->setZNear(znear); },
        "getZNear", [](const LightPlanarProbePtr& probe) { return probe->getZNear(); },
        "setZFar", [](LightPlanarProbePtr& probe, float zfar) { probe->setZFar(zfar); },
        "getZFar", [](const LightPlanarProbePtr& probe) { return probe->getZFar(); }
    );

    // ============================================================================
    // Cast Functions
    // ============================================================================

    lua["Light"] = lua.create_table();
    lua["Light"]["cast"] = [](const NodePtr& node) -> LightPtr {
        if (!node || !node->isLight())
            return LightPtr();
        return static_ptr_cast<Light>(node);
    };

    lua["LightOmni"] = lua.create_table();
    lua["LightOmni"]["cast"] = [](const NodePtr& node) -> LightOmniPtr {
        if (!node || node->getType() != Node::LIGHT_OMNI)
            return LightOmniPtr();
        return static_ptr_cast<LightOmni>(node);
    };

    lua["LightProj"] = lua.create_table();
    lua["LightProj"]["cast"] = [](const NodePtr& node) -> LightProjPtr {
        if (!node || node->getType() != Node::LIGHT_PROJ)
            return LightProjPtr();
        return static_ptr_cast<LightProj>(node);
    };

    lua["LightWorld"] = lua.create_table();
    lua["LightWorld"]["cast"] = [](const NodePtr& node) -> LightWorldPtr {
        if (!node || node->getType() != Node::LIGHT_WORLD)
            return LightWorldPtr();
        return static_ptr_cast<LightWorld>(node);
    };

    lua["LightVoxelProbe"] = lua.create_table();
    lua["LightVoxelProbe"]["cast"] = [](const NodePtr& node) -> LightVoxelProbePtr {
        if (!node || node->getType() != Node::LIGHT_VOXEL_PROBE)
            return LightVoxelProbePtr();
        return static_ptr_cast<LightVoxelProbe>(node);
    };

    lua["LightEnvironmentProbe"] = lua.create_table();
    lua["LightEnvironmentProbe"]["cast"] = [](const NodePtr& node) -> LightEnvironmentProbePtr {
        if (!node || node->getType() != Node::LIGHT_ENVIRONMENT_PROBE)
            return LightEnvironmentProbePtr();
        return static_ptr_cast<LightEnvironmentProbe>(node);
    };

    lua["LightPlanarProbe"] = lua.create_table();
    lua["LightPlanarProbe"]["cast"] = [](const NodePtr& node) -> LightPlanarProbePtr {
        if (!node || node->getType() != Node::LIGHT_PLANAR_PROBE)
            return LightPlanarProbePtr();
        return static_ptr_cast<LightPlanarProbe>(node);
    };

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] Light types registered (Omni, Proj, World, VoxelProbe, EnvironmentProbe, PlanarProbe, LensFlare)\n");
}

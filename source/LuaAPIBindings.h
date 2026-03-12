#pragma once
#ifndef LUA_API_BINDINGS_H
#define LUA_API_BINDINGS_H

#include "LuaSystemForward.h"

namespace Unigine {
    class LuaBindings {
    public:
        static void registerAll(sol::state& lua);

    private:
        // Registration functions
        // this is registered in a structured manor and goes from 
        // most demanding to less demanding.
        static void registerMathTypes(sol::state& lua);
		static void registerMathUtilities(sol::state& lua);
        static void registerNodeTypes(sol::state& lua);
        static void registerGameAPI(sol::state& lua);
        static void registerInputAPI(sol::state& lua);
        static void registerWorldAPI(sol::state& lua);
        static void registerUtilities(sol::state& lua);
		static void registerObjectTypes(sol::state& lua);
		static void registerMaterialTypes(sol::state& lua);
        static void registerEventInputs(sol::state& lua);
        static void registerPhysicsAPI(sol::state& lua);
		static void registerVizualizerAPI(sol::state& lua);
        static void registerSoundAPI(sol::state& lua);
		static void registerPlayerAPI(sol::state& lua);
		static void registerLightTypes(sol::state& lua);
		static void registerCameraTypes(sol::state& lua);
		static void registerTextureTypes(sol::state& lua);
		static void registerObjectTypesExtended(sol::state& lua);
		static void registerDecalTypes(sol::state& lua);
		static void registerPhysicsExtended(sol::state& lua);
    };
}

#endif // LUA_API_BINDINGS_H
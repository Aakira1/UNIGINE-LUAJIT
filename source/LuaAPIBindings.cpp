#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

/// <summary>
/// registers all Unigine engine APIs to the given Lua state
/// </summary>
/// <param name="lua"></param>
void Unigine::LuaBindings::registerAll(sol::state& lua)
{
    LuaSystem* ls = LuaSystem::get();
    bool should_log = ls ? ls->getLogInit() : true;
    
    try {
        registerUtilities(lua);
    }
    catch (const sol::error& e) {
        Log::error("[LuaBindings] Failed to register Utilities: %s\n", e.what());
    }
    catch (const std::exception& e) {
        Log::error("[LuaBindings] Failed to register Utilities: %s\n", e.what());
    }
    
    std::function<void(const char*, std::function<void()>)> registerWithErrorHandling = [&](const char* name, std::function<void()> func) {
        try {
            func();
        }
        catch (const sol::error& e) {
            Log::error("[LuaBindings] Failed to register %s: %s\n", name, e.what());
        }
        catch (const std::exception& e) {
            Log::error("[LuaBindings] Failed to register %s (std::exception): %s\n", name, e.what());
        }
        catch (...) {
            Log::error("[LuaBindings] Failed to register %s (unknown exception)\n", name);
        }
    };
    
    registerWithErrorHandling("MathTypes", [&]() { registerMathTypes(lua); });
    registerWithErrorHandling("TextureTypes", [&]() { registerTextureTypes(lua); });
    registerWithErrorHandling("MathUtilities", [&]() { registerMathUtilities(lua); });
    registerWithErrorHandling("PhysicsAPI", [&]() { registerPhysicsAPI(lua); });
    registerWithErrorHandling("NodeTypes", [&]() { registerNodeTypes(lua); });
    registerWithErrorHandling("GameAPI", [&]() { registerGameAPI(lua); });
    registerWithErrorHandling("InputAPI", [&]() { registerInputAPI(lua); });
    registerWithErrorHandling("WorldAPI", [&]() { registerWorldAPI(lua); });
    registerWithErrorHandling("ObjectTypes", [&]() { registerObjectTypes(lua); });
    registerWithErrorHandling("MaterialTypes", [&]() { registerMaterialTypes(lua); });
    registerWithErrorHandling("VisualizerAPI", [&]() { registerVizualizerAPI(lua); });
    registerWithErrorHandling("SoundAPI", [&]() { registerSoundAPI(lua); });
    registerWithErrorHandling("PlayerAPI", [&]() { registerPlayerAPI(lua); });
    registerWithErrorHandling("EventInputs", [&]() { registerEventInputs(lua); });
    registerWithErrorHandling("LightTypes", [&]() { registerLightTypes(lua); });
    registerWithErrorHandling("CameraTypes", [&]() { registerCameraTypes(lua); });
    registerWithErrorHandling("ObjectTypesExtended", [&]() { registerObjectTypesExtended(lua); });
    registerWithErrorHandling("DecalTypes", [&]() { registerDecalTypes(lua); });
    registerWithErrorHandling("PhysicsExtended", [&]() { registerPhysicsExtended(lua); });

    if (should_log)
        Log::message("[LuaSystem] APIs registered\n");
}


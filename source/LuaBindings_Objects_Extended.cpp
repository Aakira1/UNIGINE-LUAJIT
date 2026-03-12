#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include <UnigineObjects.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

void Unigine::LuaBindings::registerObjectTypesExtended(sol::state& lua)
{
    
    // ObjectDummy
    lua.new_usertype<ObjectDummyPtr>("ObjectDummy",
        sol::no_constructor,

        "create", []() { return ObjectDummy::create(); }
    );

    
    // ObjectMeshStatic - Static mesh rendering
    lua.new_usertype<ObjectMeshStaticPtr>("ObjectMeshStatic",
        sol::no_constructor,

        "create", sol::overload(
            []() { return ObjectMeshStatic::create(); },
            [](const char* path) { return ObjectMeshStatic::create(path); }
        ),

        // Mesh loading
        "setMeshPath", [](ObjectMeshStaticPtr& obj, const char* path) { obj->setMeshPath(path); },
        "getMeshPath", [](const ObjectMeshStaticPtr& obj) { return obj->getMeshPath(); },

        "isMeshNull", [](const ObjectMeshStaticPtr& obj) { return obj->isMeshNull(); },
        "isMeshLoadedRAM", [](const ObjectMeshStaticPtr& obj) { return obj->isMeshLoadedRAM(); },
        "isMeshLoadedVRAM", [](const ObjectMeshStaticPtr& obj) { return obj->isMeshLoadedVRAM(); }
    );

    
    // ObjectMeshDynamic - Dynamic deformable mesh
    lua.new_usertype<ObjectMeshDynamicPtr>("ObjectMeshDynamic",
        sol::no_constructor,

        "create", sol::overload(
            []() { return ObjectMeshDynamic::create(); },
            [](const char* path) { return ObjectMeshDynamic::create(path); }
        )
    );

    
    // ObjectMeshSkinned - Skeletal animation
    lua.new_usertype<ObjectMeshSkinnedPtr>("ObjectMeshSkinned",
        sol::no_constructor,

        "create", sol::overload(
            []() { return ObjectMeshSkinned::create(); },
            [](const char* path) { return ObjectMeshSkinned::create(path); }
        ),

        // Mesh loading
        "setMeshPath", [](ObjectMeshSkinnedPtr& obj, const char* path) { obj->setMeshPath(path); },
        "getMeshPath", [](const ObjectMeshSkinnedPtr& obj) { return obj->getMeshPath(); },

        // Bones
        "getNumBones", [](const ObjectMeshSkinnedPtr& obj) { return obj->getNumBones(); },
        "findBone", [](const ObjectMeshSkinnedPtr& obj, const char* name) { return obj->findBone(name); },
        "getBoneName", [](const ObjectMeshSkinnedPtr& obj, int bone) { return obj->getBoneName(bone); }
    );

    
    // ObjectParticles - Particle system
    lua.new_usertype<ObjectParticlesPtr>("ObjectParticles",
        sol::no_constructor,

        "create", []() { return ObjectParticles::create(); }
    );

    
    // ObjectSky - Sky rendering
    lua.new_usertype<ObjectSkyPtr>("ObjectSky",
        sol::no_constructor,

        "create", []() { return ObjectSky::create(); }
    );

    
    // ObjectGrass - Grass/vegetation
    lua.new_usertype<ObjectGrassPtr>("ObjectGrass",
        sol::no_constructor,

        "create", []() { return ObjectGrass::create(); }
    );

    
    // ObjectWaterGlobal - Global water system
    lua.new_usertype<ObjectWaterGlobalPtr>("ObjectWaterGlobal",
        sol::no_constructor,

        "create", []() { return ObjectWaterGlobal::create(); }
    );

    
    // ObjectText - 3D text rendering


    lua.new_usertype<ObjectTextPtr>("ObjectText",
        sol::no_constructor,

        "create", []() { return ObjectText::create(); },

        "setText", [](ObjectTextPtr& obj, const char* text) { obj->setText(text); },
        "getText", [](const ObjectTextPtr& obj) { return obj->getText(); }
    );

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] Extended object types registered (Static, Dynamic, Skinned, Particles, Sky, Grass, Water, Text)\n");
}

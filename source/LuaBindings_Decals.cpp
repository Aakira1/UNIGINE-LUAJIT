#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include <UnigineDecals.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

void Unigine::LuaBindings::registerDecalTypes(sol::state& lua)
{
    
    // Decal Base Class
    

    lua.new_usertype<DecalPtr>("Decal",
        sol::no_constructor,

        // Decal dimensions
        "setRadius", [](DecalPtr& decal, float radius) { decal->setRadius(radius); },
        "getRadius", [](const DecalPtr& decal) { return decal->getRadius(); },

        "setOpacity", [](DecalPtr& decal, float opacity) { decal->setOpacity(opacity); },
        "getOpacity", [](const DecalPtr& decal) { return decal->getOpacity(); },

        // Material
        "setMaterial", [](DecalPtr& decal, const MaterialPtr& mat) { decal->setMaterial(mat); },
        "getMaterial", [](const DecalPtr& decal) { return decal->getMaterial(); },

        "setMaterialFilePath", [](DecalPtr& decal, const char* path) { decal->setMaterialFilePath(path); },
        "getMaterialFilePath", [](const DecalPtr& decal) { return decal->getMaterialFilePath(); },

        // Visibility/fade distances
        "setMaxVisibleDistance", [](DecalPtr& decal, float distance) { decal->setMaxVisibleDistance(distance); },
        "getMaxVisibleDistance", [](const DecalPtr& decal) { return decal->getMaxVisibleDistance(); },

        "setMinVisibleDistance", [](DecalPtr& decal, float distance) { decal->setMinVisibleDistance(distance); },
        "getMinVisibleDistance", [](const DecalPtr& decal) { return decal->getMinVisibleDistance(); },

        "setMaxFadeDistance", [](DecalPtr& decal, float distance) { decal->setMaxFadeDistance(distance); },
        "getMaxFadeDistance", [](const DecalPtr& decal) { return decal->getMaxFadeDistance(); },

        "setMinFadeDistance", [](DecalPtr& decal, float distance) { decal->setMinFadeDistance(distance); },
        "getMinFadeDistance", [](const DecalPtr& decal) { return decal->getMinFadeDistance(); },

        // Masks
        "setViewportMask", [](DecalPtr& decal, int mask) { decal->setViewportMask(mask); },
        "getViewportMask", [](const DecalPtr& decal) { return decal->getViewportMask(); },

        "setIntersectionMask", [](DecalPtr& decal, int mask) { decal->setIntersectionMask(mask); },
        "getIntersectionMask", [](const DecalPtr& decal) { return decal->getIntersectionMask(); },

        // Node base methods
        "getName", [](const DecalPtr& decal) { return decal->getName(); },
        "setName", [](DecalPtr& decal, const char* name) { decal->setName(name); },

        "getPosition", [](const DecalPtr& decal) { return decal->getPosition(); },
        "setPosition", [](DecalPtr& decal, const Vec3& pos) { decal->setPosition(pos); },

        "getWorldPosition", [](const DecalPtr& decal) { return decal->getWorldPosition(); },
        "setWorldPosition", [](DecalPtr& decal, const Vec3& pos) { decal->setWorldPosition(pos); },

        "getRotation", [](const DecalPtr& decal) { return decal->getRotation(); },
        "setRotation", [](DecalPtr& decal, const quat& rot) { decal->setRotation(rot); },

        "setEnabled", [](DecalPtr& decal, bool enabled) { decal->setEnabled(enabled); },
        "isEnabled", [](const DecalPtr& decal) { return decal->isEnabled(); },

        sol::meta_function::to_string, [](const DecalPtr& decal) {
            if (!decal) return std::string("Decal(null)");
            return std::string("Decal(") + decal->getName() + ")";
        }
    );

    // Create Decal table with cast function
    lua["Decal"] = lua.create_table();
    lua["Decal"]["cast"] = [](const NodePtr& node) -> DecalPtr {
        if (!node) {
            Log::warning("Decal.cast: node is null\n");
            return DecalPtr();
        }
        if (!node->isDecal()) {
            Log::warning("Decal.cast: node '%s' is not a Decal (type: %s)\n",
                node->getName(), node->getTypeName());
            return DecalPtr();
        }
        return checked_ptr_cast<Decal>(node);
    };

    
    // DecalProj - Projected Decal
    

    lua.new_usertype<DecalProjPtr>("DecalProj",
        sol::no_constructor,

        "create", []() { return DecalProj::create(); },

        // Projection
        "setZNear", [](DecalProjPtr& decal, float znear) { decal->setZNear(znear); },
        "getZNear", [](const DecalProjPtr& decal) { return decal->getZNear(); },

        "setFov", [](DecalProjPtr& decal, float fov) { decal->setFov(fov); },
        "getFov", [](const DecalProjPtr& decal) { return decal->getFov(); },

        "setAspect", [](DecalProjPtr& decal, float aspect) { decal->setAspect(aspect); },
        "getAspect", [](const DecalProjPtr& decal) { return decal->getAspect(); }
    );

    
    // DecalOrtho - Orthographic Decal
    

    lua.new_usertype<DecalOrthoPtr>("DecalOrtho",
        sol::no_constructor,

        "create", []() { return DecalOrtho::create(); },

        // Orthographic dimensions
        "setWidth", [](DecalOrthoPtr& decal, float width) { decal->setWidth(width); },
        "getWidth", [](const DecalOrthoPtr& decal) { return decal->getWidth(); },

        "setHeight", [](DecalOrthoPtr& decal, float height) { decal->setHeight(height); },
        "getHeight", [](const DecalOrthoPtr& decal) { return decal->getHeight(); },

        "setZNear", [](DecalOrthoPtr& decal, float znear) { decal->setZNear(znear); },
        "getZNear", [](const DecalOrthoPtr& decal) { return decal->getZNear(); }
    );

    
    // DecalMesh - Mesh-based Decal
    

    lua.new_usertype<DecalMeshPtr>("DecalMesh",
        sol::no_constructor,

        "create", sol::overload(
            []() { return DecalMesh::create(); },
            [](const char* path) { return DecalMesh::create(path); }
        ),

        // Mesh
        "setMeshPath", [](DecalMeshPtr& decal, const char* path) { decal->setMeshPath(path); },
        "getMeshPath", [](const DecalMeshPtr& decal) { return decal->getMeshPath(); },

        "isMeshNull", [](const DecalMeshPtr& decal) { return decal->isMeshNull(); },
        "isMeshLoadedRAM", [](const DecalMeshPtr& decal) { return decal->isMeshLoadedRAM(); },
        "isMeshLoadedVRAM", [](const DecalMeshPtr& decal) { return decal->isMeshLoadedVRAM(); }
    );

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] Decal types registered (Proj, Ortho, Mesh)\n");
}

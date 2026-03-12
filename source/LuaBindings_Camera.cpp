#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include <UnigineCamera.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

void Unigine::LuaBindings::registerCameraTypes(sol::state& lua)
{
    // Camera Enumerations
    

    lua.new_enum("CameraFOVMode",
        "VERTICAL", Camera::FOV_MODE_VERTICAL,
        "PHYSICALLY_BASED", Camera::FOV_MODE_PHYSICALLY_BASED_CAMERA
    );

    lua.new_enum("CameraFOVFixed",
        "VERTICAL", Camera::FOV_FIXED_VERTICAL,
        "HORIZONTAL", Camera::FOV_FIXED_HORIZONTAL
    );

    lua.new_enum("CameraProjectionMode",
        "PERSPECTIVE", Camera::PROJECTION_MODE_PERSPECTIVE,
        "ORTHOGRAPHIC", Camera::PROJECTION_MODE_ORTHOGRAPHIC
    );

    
    // Camera Type
    

    lua.new_usertype<CameraPtr>("Camera",
        sol::no_constructor,

        // Static creation
        "create", []() { return Camera::create(); },

        // Modelview matrix
        "setModelview", [](CameraPtr& cam, const Mat4& modelview) { cam->setModelview(modelview); },
        "getModelview", [](const CameraPtr& cam) { return cam->getModelview(); },
        "getIModelview", [](const CameraPtr& cam) { return cam->getIModelview(); },

        // Position
        "setPosition", [](CameraPtr& cam, const Vec3& position) { cam->setPosition(position); },
        "getPosition", [](const CameraPtr& cam) { return cam->getPosition(); },

        // Offset
        "setOffset", [](CameraPtr& cam, const mat4& offset) { cam->setOffset(offset); },
        "getOffset", [](const CameraPtr& cam) { return cam->getOffset(); },

        // Projection
        "setProjection", [](CameraPtr& cam, const mat4& projection) { cam->setProjection(projection); },
        "getProjection", [](const CameraPtr& cam) { return cam->getProjection(); },
        "getAspectCorrectedProjection", [](const CameraPtr& cam, float aspect) {
            return cam->getAspectCorrectedProjection(aspect);
        },

        // FOV Mode
        "setFovMode", [](CameraPtr& cam, int mode) {
            cam->setFovMode((Camera::FOV_MODE)mode);
        },
        "getFovMode", [](const CameraPtr& cam) {
            return (int)cam->getFovMode();
        },

        // Projection Mode
        "setProjectionMode", [](CameraPtr& cam, int mode) {
            cam->setProjectionMode((Camera::PROJECTION_MODE)mode);
        },
        "getProjectionMode", [](const CameraPtr& cam) {
            return (int)cam->getProjectionMode();
        },

        // Orthographic height
        "setOrthoHeight", [](CameraPtr& cam, float height) { cam->setOrthoHeight(height); },
        "getOrthoHeight", [](const CameraPtr& cam) { return cam->getOrthoHeight(); },

        // FOV Fixed
        "getFovFixed", [](const CameraPtr& cam) {
            return (int)cam->getFovFixed();
        },

        // Field of View
        "setFov", [](CameraPtr& cam, float fov) { cam->setFov(fov); },
        "getFov", [](const CameraPtr& cam) { return cam->getFov(); },

        // Film gate (physically-based camera)
        "setFilmGate", [](CameraPtr& cam, float gate) { cam->setFilmGate(gate); },
        "getFilmGate", [](const CameraPtr& cam) { return cam->getFilmGate(); },

        // Focal length (physically-based camera)
        "setFocalLength", [](CameraPtr& cam, float length) { cam->setFocalLength(length); },
        "getFocalLength", [](const CameraPtr& cam) { return cam->getFocalLength(); },

        // Near/Far clipping planes
        "setZNear", [](CameraPtr& cam, float znear) { cam->setZNear(znear); },
        "getZNear", [](const CameraPtr& cam) { return cam->getZNear(); },

        "setZFar", [](CameraPtr& cam, float zfar) { cam->setZFar(zfar); },
        "getZFar", [](const CameraPtr& cam) { return cam->getZFar(); },

        // Up vector
        "setUp", [](CameraPtr& cam, const vec3& up) { cam->setUp(up); },
        "getUp", [](const CameraPtr& cam) { return cam->getUp(); },

        // Oblique frustum
        "setObliqueFrustumPlane", [](CameraPtr& cam, const Vec4& plane) {
            cam->setObliqueFrustumPlane(plane);
        },
        "getObliqueFrustumPlane", [](const CameraPtr& cam) {
            return cam->getObliqueFrustumPlane();
        },

        "setObliqueFrustum", [](CameraPtr& cam, bool frustum) { cam->setObliqueFrustum(frustum); },
        "isObliqueFrustum", [](const CameraPtr& cam) { return cam->isObliqueFrustum(); },

        // Masks
        "setViewportMask", [](CameraPtr& cam, int mask) { cam->setViewportMask(mask); },
        "getViewportMask", [](const CameraPtr& cam) { return cam->getViewportMask(); },

        "setReflectionViewportMask", [](CameraPtr& cam, int mask) {
            cam->setReflectionViewportMask(mask);
        },
        "getReflectionViewportMask", [](const CameraPtr& cam) {
            return cam->getReflectionViewportMask();
        },

        "setSourceMask", [](CameraPtr& cam, int mask) { cam->setSourceMask(mask); },
        "getSourceMask", [](const CameraPtr& cam) { return cam->getSourceMask(); },

        "setReverbMask", [](CameraPtr& cam, int mask) { cam->setReverbMask(mask); },
        "getReverbMask", [](const CameraPtr& cam) { return cam->getReverbMask(); },

        // Scriptable materials (post-processing)
        "clearScriptableMaterials", [](CameraPtr& cam) { cam->clearScriptableMaterials(); },
        "getNumScriptableMaterials", [](const CameraPtr& cam) {
            return cam->getNumScriptableMaterials();
        },

        "getScriptableMaterial", [](const CameraPtr& cam, int num) {
            return cam->getScriptableMaterial(num);
        },
        "setScriptableMaterial", [](CameraPtr& cam, int num, const MaterialPtr& material) {
            cam->setScriptableMaterial(num, material);
        },

        "insertScriptableMaterial", [](CameraPtr& cam, int num, const MaterialPtr& material) {
            cam->insertScriptableMaterial(num, material);
        },
        "findScriptableMaterial", [](const CameraPtr& cam, const MaterialPtr& material) {
            return cam->findScriptableMaterial(material);
        },

        "addScriptableMaterial", [](CameraPtr& cam, const MaterialPtr& material) {
            cam->addScriptableMaterial(material);
        },
        "removeScriptableMaterial", [](CameraPtr& cam, int num) {
            cam->removeScriptableMaterial(num);
        },

        "swapScriptableMaterials", [](CameraPtr& cam, int num_0, int num_1) {
            cam->swapScriptableMaterials(num_0, num_1);
        },

        "setScriptableMaterialEnabled", [](CameraPtr& cam, int num, bool enabled) {
            cam->setScriptableMaterialEnabled(num, enabled);
        },
        "getScriptableMaterialEnabled", [](const CameraPtr& cam, int num) {
            return cam->getScriptableMaterialEnabled(num);
        },

        // Screen projection utilities
        "getDirectionFromScreen", [](const CameraPtr& cam, float screen_x, float screen_y, float aspect) {
            return cam->getDirectionFromScreen(screen_x, screen_y, aspect);
        },

        "getProjectionFromScreen", [](const CameraPtr& cam, float screen_x0, float screen_y0,
                                       float screen_x1, float screen_y1, float aspect) {
            return cam->getProjectionFromScreen(screen_x0, screen_y0, screen_x1, screen_y1, aspect);
        },

        // Clone/copy/swap
        "clone", [](const CameraPtr& cam) { return cam->clone(); },
        "copy", [](const CameraPtr& cam, const CameraPtr& other) { return cam->copy(other); },
        "swap", [](CameraPtr& cam, const CameraPtr& other) { cam->swap(other); },

        // String representation
        sol::meta_function::to_string, [](const CameraPtr& cam) {
            if (!cam) return std::string("Camera(null)");

            std::string mode_str;
            switch (cam->getProjectionMode()) {
                case Camera::PROJECTION_MODE_PERSPECTIVE:
                    mode_str = "Perspective";
                    break;
                case Camera::PROJECTION_MODE_ORTHOGRAPHIC:
                    mode_str = "Orthographic";
                    break;
                default:
                    mode_str = "Unknown";
                    break;
            }

            return std::string("Camera(") + mode_str + ", FOV:" +
                   std::to_string(cam->getFov()) + ")";
        }
    );

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] Camera types registered (Perspective, Orthographic, FOV, Scriptable materials)\n");
}

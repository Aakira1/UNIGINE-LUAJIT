#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include <UniginePlayers.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

/// <summary>
/// Registers the Player API in the provided Lua state by creating PlayerDummy usertype for camera control.
/// </summary>
/// <param name="lua">Reference to the sol::state representing the Lua environment where the Player API will be registered.</param>
void Unigine::LuaBindings::registerPlayerAPI(sol::state& lua)
{
    // PlayerDummy usertype - used for camera control in first-person controllers
    // All methods guard against null to avoid UniginePtr !isNull() assertion.
    lua.new_usertype<PlayerDummyPtr>("PlayerDummy",
        sol::no_constructor,

        "create", []() { return PlayerDummy::create(); },

        "setFov", [](PlayerDummyPtr& p, float fov) { if (p) p->setFov(fov); },
        "getFov", [](const PlayerDummyPtr& p) { return p ? p->getFov() : 60.0f; },
        "setZNear", [](PlayerDummyPtr& p, float znear) { if (p) p->setZNear(znear); },
        "getZNear", [](const PlayerDummyPtr& p) { return p ? p->getZNear() : 0.01f; },
        "setZFar", [](PlayerDummyPtr& p, float zfar) { if (p) p->setZFar(zfar); },
        "getZFar", [](const PlayerDummyPtr& p) { return p ? p->getZFar() : 20000.0f; },

        "setMainPlayer", [](PlayerDummyPtr& p, bool main) { if (p) p->setMainPlayer(main); },
        "isMainPlayer", [](const PlayerDummyPtr& p) { return p ? p->isMainPlayer() : false; },

        "setPosition", [](PlayerDummyPtr& p, const Vec3& pos) { if (p) p->setPosition(pos); },
        "getPosition", [](const PlayerDummyPtr& p) { return p ? p->getPosition() : Vec3(0, 0, 0); },
        "setWorldPosition", [](PlayerDummyPtr& p, const Vec3& pos) { if (p) p->setWorldPosition(pos); },
        "getWorldPosition", [](const PlayerDummyPtr& p) { return p ? p->getWorldPosition() : Vec3(0, 0, 0); },
        "setWorldDirection", [](PlayerDummyPtr& p, const Vec3& dir, const Vec3& up) {
            if (p) p->setWorldDirection(vec3(dir), vec3(up));
        },
        "getWorldDirection", [](const PlayerDummyPtr& p) {
            return p ? Vec3(p->getWorldDirection()) : Vec3(0, 1, 0);
        },
        "getDirection", [](const PlayerDummyPtr& p) { return p ? p->getDirection() : vec3(0, 1, 0); },
        "setParent", [](PlayerDummyPtr& p, const NodePtr& parent) {
            if (p && parent) p->setParent(parent);
        },
        "getParent", [](const PlayerDummyPtr& p) { return p ? p->getParent() : NodePtr(); },
        "setEnabled", [](PlayerDummyPtr& p, bool enabled) { if (p) p->setEnabled(enabled); },
        "isEnabled", [](const PlayerDummyPtr& p) { return p ? p->isEnabled() : false; }
    );

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] Player API registered\n");
}

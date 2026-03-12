#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;


void Unigine::LuaBindings::registerObjectTypes(sol::state& lua)
{
    // Register ObjectPtr type
    lua.new_usertype<ObjectPtr>("ObjectPtr",
        sol::no_constructor,

        // Basic properties
        "getName", [](const ObjectPtr& obj) { return obj->getName(); },
        "isEnabled", [](const ObjectPtr& obj) { return obj->isEnabled(); },
        "setEnabled", [](ObjectPtr& obj, bool enabled) { obj->setEnabled(enabled); },

        // Material methods
        "getNumSurfaces", [](const ObjectPtr& obj) { return obj->getNumSurfaces(); },
        "getMaterial", [](const ObjectPtr& obj, int surface) {
            return obj->getMaterial(surface);
        },
        "setMaterial", [](ObjectPtr& obj, const MaterialPtr& material, int surface) {
            obj->setMaterial(material, surface);
        },

        // Physics body access
        "getBody", [](const ObjectPtr& obj) { return obj ? obj->getBody() : BodyPtr(); },
        "setBody", [](ObjectPtr& obj, const BodyPtr& body) { if (obj) obj->setBody(body); },

        // Transform
        "getWorldTransform", [](const ObjectPtr& obj) { return obj ? obj->getWorldTransform() : Mat4(); },
        "setWorldTransform", [](ObjectPtr& obj, const Mat4& transform) { if (obj) obj->setWorldTransform(transform); },

        // String representation
        sol::meta_function::to_string, [](const ObjectPtr& obj) {
            if (!obj) return std::string("Object(null)");
            return std::string("Object(") + std::string(obj->getName()) + ")";
        }
    );

    // Create Object table with cast function
    lua["Object"] = lua.create_table();
    lua["Object"]["cast"] = [](const NodePtr& node) -> ObjectPtr {
        if (!node) {
            Log::warning("Object.cast: node is null\n");
            return ObjectPtr();
        }
        if (!node->isObject()) {
            Log::warning("Object.cast: node '%s' is not an Object (type: %s)\n",
                node->getName(), node->getTypeName());
            return ObjectPtr();
        }
        return static_ptr_cast<Object>(node);
        };

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] Object types registered\n");

    // Register LightPtr type (minimal binding)
    lua.new_usertype<LightPtr>("LightPtr",
        sol::no_constructor,

        sol::meta_function::to_string, [](const LightPtr& light) {
            if (!light) return std::string("Light(null)");
            return std::string("Light(") + light->getName() + ")";
        }
    );

	// Register PlayerPtr type (minimal binding)
    lua.new_usertype<PlayerPtr>("PlayerPtr",
        sol::no_constructor,

        // Node base class methods (Player inherits from Node)
        "getWorldPosition", [](const PlayerPtr& player) {
            if (!player) return Vec3(0, 0, 0);
            return player->getWorldPosition();
        },
        "setWorldPosition", [](PlayerPtr& player, const Vec3& pos) {
            if (player) player->setWorldPosition(pos);
        },
        "getPosition", [](const PlayerPtr& player) {
            if (!player) return Vec3(0, 0, 0);
            return player->getPosition();
        },
        "setPosition", [](PlayerPtr& player, const Vec3& pos) {
            if (player) player->setPosition(pos);
        },
        "getName", [](const PlayerPtr& player) {
            if (!player) return std::string("");
            return std::string(player->getName());
        },

        sol::meta_function::to_string, [](const PlayerPtr& player) {
            if (!player) return std::string("Player(null)");
            return std::string("Player(") + player->getName() + ")";
        }
    );
}
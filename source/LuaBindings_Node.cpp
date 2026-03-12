#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include <UnigineNodes.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

/// <summary>
/// registers Node type and its methods to the given Lua state
/// </summary>
/// <param name="lua"></param>
void Unigine::LuaBindings::registerNodeTypes(sol::state& lua)
{
    lua.new_usertype<Node>("Node",
        sol::no_constructor,

        "setRotation", [](Node* n, const Unigine::Math::quat& q) {
            n->setRotation(q);
        }
    );

    // Register the NodePtr type
    lua.new_usertype<NodePtr>("NodePtr",
		sol::constructors<
		NodePtr(),                                 // Default constructor (null)
		NodePtr(const NodePtr & other)              // Copy constructor
        >(),

        // Name
        "getName", [](const NodePtr& node) { return node ? std::string(node->getName()) : std::string(); },
        "setName", [](NodePtr& node, const char* name) { if (node) node->setName(name); },

        // Position (local)
        "getPosition", [](const NodePtr& node) { return node ? node->getPosition() : Vec3(0, 0, 0); },
        "setPosition", [](NodePtr& node, const Vec3& pos) { if (node) node->setPosition(pos); },

        // Rotation (local)
        "getRotation", [](const NodePtr& node) { return node ? node->getRotation() : quat(); },
        "setRotation", [](NodePtr& node, const quat& rot) { if (node) node->setRotation(rot); },

        // Scale (local)
        "getScale", [](const NodePtr& node) { return node ? node->getScale() : vec3(1, 1, 1); },
        "setScale", [](NodePtr& node, const vec3& scale) { if (node) node->setScale(scale); },

        // World Position
        "getWorldPosition", [](const NodePtr& node) { return node ? node->getWorldPosition() : Vec3(0, 0, 0); },
        "setWorldPosition", [](NodePtr& node, const Vec3& pos) { if (node) node->setWorldPosition(pos); },

        // World Rotation
        "getWorldRotation", [](const NodePtr& node) { return node ? node->getWorldRotation() : quat(); },
        "setWorldRotation", [](NodePtr& node, const quat& rot) { if (node) node->setWorldRotation(rot); },

        // World Scale
        "getWorldScale", [](const NodePtr& node) { return node ? node->getWorldScale() : vec3(1, 1, 1); },
        "setWorldScale", [](NodePtr& node, const vec3& scale) { if (node) node->setWorldScale(scale); },

        // Transform matrices
        "getTransform", [](const NodePtr& node) { return node ? node->getTransform() : Mat4(); },
        "setTransform", [](NodePtr& node, const Mat4& transform) { if (node) node->setTransform(transform); },
        "getWorldTransform", [](const NodePtr& node) { return node ? node->getWorldTransform() : Mat4(); },
        "setWorldTransform", [](NodePtr& node, const Mat4& transform) { if (node) node->setWorldTransform(transform); },
        "getIWorldTransform", [](const NodePtr& node) { return node ? node->getIWorldTransform() : Mat4(); },

        // Translate and rotate
        "translate", sol::overload(
            [](NodePtr& node, const Vec3& t) { if (node) node->translate(t); },
            [](NodePtr& node, float x, float y, float z) { if (node) node->translate(x, y, z); }
        ),
        "rotate", sol::overload(
            [](NodePtr& node, const quat& r) { if (node) node->rotate(r); },
            [](NodePtr& node, const vec3& angles) { if (node) node->rotate(angles); },
            [](NodePtr& node, float x, float y, float z) { if (node) node->rotate(x, y, z); }
        ),
        "worldTranslate", sol::overload(
            [](NodePtr& node, const Vec3& t) { if (node) node->worldTranslate(t); },
            [](NodePtr& node, float x, float y, float z) { if (node) node->worldTranslate(x, y, z); }
        ),
        "worldRotate", sol::overload(
            [](NodePtr& node, const quat& r) { if (node) node->worldRotate(r); },
            [](NodePtr& node, const vec3& angles) { if (node) node->worldRotate(angles); },
            [](NodePtr& node, float x, float y, float z) { if (node) node->worldRotate(x, y, z); }
        ),

        // Look at
        "worldLookAt", sol::overload(
            [](NodePtr& node, const Vec3& target) { if (node) node->worldLookAt(target); },
            [](NodePtr& node, const Vec3& target, const vec3& up) { if (node) node->worldLookAt(target, up); }
        ),

        // Direction
        "getDirection", [](const NodePtr& node) { return node ? node->getDirection() : vec3(0, 1, 0); },
        "setDirection", [](NodePtr& node, const vec3& dir, const vec3& up) { if (node) node->setDirection(dir, up); },
        "getWorldDirection", [](const NodePtr& node) { return node ? node->getWorldDirection() : vec3(0, 1, 0); },
        "setWorldDirection", [](NodePtr& node, const vec3& dir, const vec3& up) { if (node) node->setWorldDirection(dir, up); },

        // Coordinate transforms
        "toWorld", [](const NodePtr& node, const vec3& p) { return node ? node->toWorld(p) : Vec3(0, 0, 0); },
        "toLocal", [](const NodePtr& node, const Vec3& p) { return node ? node->toLocal(p) : Vec3(0, 0, 0); },

        // Enabled state
        "isEnabled", [](const NodePtr& node) { return node ? node->isEnabled() : false; },
        "setEnabled", [](NodePtr& node, bool enabled) { if (node) node->setEnabled(enabled); },

        // Hierarchy
        "getParent", [](const NodePtr& node) -> NodePtr {
            if (!node) return NodePtr();
            return node->getParent();
        },
        "setParent", [](NodePtr& node, const NodePtr& parent) { if (node) node->setParent(parent); },
        "getNumChildren", [](const NodePtr& node) -> int {
            if (!node) return 0;
            return node->getNumChildren();
        },
        "getChild", [](const NodePtr& node, int num) -> NodePtr {
            if (!node) return NodePtr();
            return node->getChild(num);
        },
        "addChild", [](NodePtr& node, const NodePtr& child) { if (node && child) node->addChild(child); },
        "removeChild", [](NodePtr& node, const NodePtr& child) { if (node && child) node->removeChild(child); },
        "findChild", [](const NodePtr& node, const char* name) -> NodePtr {
            if (!node) return NodePtr();
            return node->findNode(name, 0);
        },
        "findNode", sol::overload(
            [](const NodePtr& node, const char* name) -> NodePtr {
                if (!node) return NodePtr();
                return node->findNode(name, 0);
            },
            [](const NodePtr& node, const char* name, int recursive) -> NodePtr {
                if (!node) return NodePtr();
                return node->findNode(name, recursive);
            }
        ),

        // ID and type
        "getID", [](const NodePtr& node) -> int {
            if (!node) return 0;
            return node->getID();
        },
        "getType", [](const NodePtr& node) -> Node::TYPE {
            if (!node) return static_cast<Node::TYPE>(0);
            return node->getType();
        },
        "getTypeName", [](const NodePtr& node) {
            if (!node) return std::string();
            return std::string(node->getTypeName());
        },

        // Bounds (match return types: getWorld* returns WorldBound*, so use same for null case)
        "getBoundBox", [](const NodePtr& node) {
            if (!node) return BoundBox();
            return node->getBoundBox();
        },
        "getBoundSphere", [](const NodePtr& node) {
            if (!node) return BoundSphere();
            return node->getBoundSphere();
        },
        "getWorldBoundBox", [](const NodePtr& node) {
            if (!node) return WorldBoundBox();
            return node->getWorldBoundBox();
        },
        "getWorldBoundSphere", [](const NodePtr& node) {
            if (!node) return WorldBoundSphere();
            return node->getWorldBoundSphere();
        },

        // String representation
        sol::meta_function::to_string, [](const NodePtr& node) {
            if (!node) {
                return std::string("Node(null)");
            }
            return std::string("Node(") + node->getName() + ")";
        }
    );

    lua["NodePtr"]["addChild"] = [](NodePtr& parent, const NodePtr& child) {
        if (parent && child) parent->addChild(child);
    };
}
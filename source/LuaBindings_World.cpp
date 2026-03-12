#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include <UnigineWorlds.h>
#include <UnigineWorld.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

void Unigine::LuaBindings::registerWorldAPI(sol::state& lua)
{
#pragma region WorldIntersection Types
    lua.new_usertype<WorldIntersectionPtr>("WorldIntersection",
        sol::constructors<WorldIntersectionPtr()>(),

        "create", []() { return WorldIntersection::create(); },

        "getType", [](const WorldIntersectionPtr& wi) { return wi->getType(); },
        "getTypeName", [](const WorldIntersectionPtr& wi) { return wi->getTypeName(); },

        "setPoint", [](WorldIntersectionPtr& wi, const Vec3& point) { wi->setPoint(point); },
        "getPoint", [](const WorldIntersectionPtr& wi) { return wi->getPoint(); },

        "setIndex", [](WorldIntersectionPtr& wi, int index) { wi->setIndex(index); },
        "getIndex", [](const WorldIntersectionPtr& wi) { return wi->getIndex(); },

        "setInstance", [](WorldIntersectionPtr& wi, int instance) { wi->setInstance(instance); },
        "getInstance", [](const WorldIntersectionPtr& wi) { return wi->getInstance(); },

        "setSurface", [](WorldIntersectionPtr& wi, int surface) { wi->setSurface(surface); },
        "getSurface", [](const WorldIntersectionPtr& wi) { return wi->getSurface(); },

        sol::meta_function::to_string, [](const WorldIntersectionPtr& wi) {
            if (!wi) return std::string("WorldIntersection(null)");
            return std::string("WorldIntersection(") + wi->getTypeName() + ")";
        }
    );

    lua.new_usertype<WorldIntersectionNormalPtr>("WorldIntersectionNormal",
        sol::constructors<WorldIntersectionNormalPtr()>(),

        "create", []() { return WorldIntersectionNormal::create(); },

        "setNormal", [](WorldIntersectionNormalPtr& win, const vec3& normal) {
            if (win) win->setNormal(normal);
        },
        "getNormal", [](const WorldIntersectionNormalPtr& win) {
            return win ? win->getNormal() : vec3(0, 0, 1);
        },

        // Base WorldIntersection (point, etc.)
        "setPoint", [](WorldIntersectionNormalPtr& win, const Vec3& point) {
            if (win) win->setPoint(point);
        },
        "getPoint", [](const WorldIntersectionNormalPtr& win) {
            return win ? win->getPoint() : Vec3(0, 0, 0);
        },

        sol::meta_function::to_string, [](const WorldIntersectionNormalPtr& win) {
            if (!win) return std::string("WorldIntersectionNormal(null)");
            return std::string("WorldIntersectionNormal");
        }
    );


    lua.new_usertype<WorldIntersectionTexCoordPtr>("WorldIntersectionTexCoord",
        sol::constructors<WorldIntersectionTexCoordPtr()>(),

        "create", []() { return WorldIntersectionTexCoord::create(); },

        "setTexCoord", [](WorldIntersectionTexCoordPtr& witc, const vec4& coord) {
            witc->setTexCoord(coord);
        },
        "getTexCoord", [](const WorldIntersectionTexCoordPtr& witc) {
            return witc->getTexCoord();
        },

        sol::meta_function::to_string, [](const WorldIntersectionTexCoordPtr& witc) {
            if (!witc) return std::string("WorldIntersectionTexCoord(null)");
            return std::string("WorldIntersectionTexCoord");
        }
    );


    lua["WorldIntersection"]["WORLD_INTERSECTION"] =
        WorldIntersection::WORLD_INTERSECTION;
    lua["WorldIntersection"]["WORLD_INTERSECTION_NORMAL"] =
        WorldIntersection::WORLD_INTERSECTION_NORMAL;
    lua["WorldIntersection"]["WORLD_INTERSECTION_TEX_COORD"] =
        WorldIntersection::WORLD_INTERSECTION_TEX_COORD;
    lua["WorldIntersection"]["NUM_WORLD_INTERSECTIONS"] =
        WorldIntersection::NUM_WORLD_INTERSECTIONS;

#pragma endregion

#pragma region World Static API

    lua["World"] = lua.create_table();

    // Node queries (return nil when not found so Lua can safely use "if node then")
    lua["World"]["getNodeByName"] = [&lua](const std::string& name) -> sol::object {
        NodePtr n = World::getNodeByName(name.c_str());
        if (!n) return sol::lua_nil;
        return sol::make_object(lua, n);
    };

    lua["World"]["getNodesByName"] = [&lua](const std::string& name) -> sol::table {
        Unigine::Vector<Unigine::NodePtr> nodes;
        World::getNodesByName(name.c_str(), nodes);
        sol::table result = lua.create_table();
        for (int i = 0; i < nodes.size(); i++)
        {
            result[i + 1] = nodes[i];
        }
        return result;
    };

    lua["World"]["getNodes"] = [&lua](sol::optional<bool> expand_node_ref, sol::optional<bool> expand_world_clutter) -> sol::table {
        Unigine::Vector<Unigine::NodePtr> nodes;
        World::getNodes(nodes, expand_node_ref.value_or(true), expand_world_clutter.value_or(true));
        sol::table result = lua.create_table();
        for (int i = 0; i < nodes.size(); i++)
        {
            result[i + 1] = nodes[i];
        }
        return result;
    };

    lua["World"]["getNodesByType"] = [&lua](int type) -> sol::table {
        Unigine::Vector<Unigine::NodePtr> nodes;
        World::getNodesByType(type, nodes);
        sol::table result = lua.create_table();
        for (int i = 0; i < nodes.size(); i++)
        {
            result[i + 1] = nodes[i];
        }
        return result;
    };

    // Intersection queries (guard null intersection to avoid UniginePtr assertion)
    lua["World"]["getIntersection"] = sol::overload(
        [](const Math::Vec3& p0, const Math::Vec3& p1, int mask) {
            return World::getIntersection(p0, p1, mask);
        },
        [](const Math::Vec3& p0, const Math::Vec3& p1, int mask, const WorldIntersectionPtr& intersection) {
            return intersection ? World::getIntersection(p0, p1, mask, intersection) : World::getIntersection(p0, p1, mask);
        },
        [](const Math::Vec3& p0, const Math::Vec3& p1, int mask, const WorldIntersectionNormalPtr& intersection) {
            return intersection ? World::getIntersection(p0, p1, mask, intersection) : World::getIntersection(p0, p1, mask);
        },
        [](const Math::Vec3& p0, const Math::Vec3& p1, int mask, const WorldIntersectionTexCoordPtr& intersection) {
            return intersection ? World::getIntersection(p0, p1, mask, intersection) : World::getIntersection(p0, p1, mask);
        }
    );

#pragma endregion

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] World API registered\n");
}
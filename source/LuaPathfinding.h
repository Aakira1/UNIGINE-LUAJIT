#pragma once
#ifndef LUA_PATHFINDING_H
#define LUA_PATHFINDING_H

#include "LuaSystemForward.h"
#include <UniginePathFinding.h>

namespace Unigine {
    class LuaPathfinding {
    public:
        // Find path between two points
        static Unigine::Vector<Math::vec3> findPath(const Math::vec3& start,
                                                    const Math::vec3& end,
                                                    float radius = 0.5f);
        
        // Check if point is walkable
        static bool isWalkable(const Math::vec3& position, float radius = 0.5f);
        
        // Get nearest walkable point
        static Math::vec3 getNearestWalkable(const Math::vec3& position, float radius = 0.5f);
        
        // Find path using PathRoute object
        static bool findPathWithPath(Unigine::PathRoutePtr path,
                                    const Math::vec3& start,
                                    const Math::vec3& end,
                                    Unigine::Vector<Math::vec3>& out_path);
        
        // Register to Lua
        static void registerToLua(sol::state& lua);
    };
}

#endif // LUA_PATHFINDING_H

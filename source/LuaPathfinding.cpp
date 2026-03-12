#include "CommonIncludes.h"
#include "LuaPathfinding.h"
#include "LuaSystem.h"
#include <UniginePathFinding.h>
#include <UnigineWorld.h>
#include <UnigineNode.h>

using namespace Unigine;

Unigine::Vector<Math::vec3> LuaPathfinding::findPath(const Math::vec3& start,
                                                     const Math::vec3& end,
                                                     float radius)
{
    Unigine::Vector<Math::vec3> path_points;
    
    // Find path using PathRoute
    PathRoutePtr path_route = PathRoute::create(radius);
    if (!path_route)
    {
        Log::error("[Pathfinding] Failed to create path route object\n");
        return path_points;
    }
    
    // Create 3D path (use create2D for 2D pathfinding)
    Math::Vec3 start_vec3(start);
    Math::Vec3 end_vec3(end);
    path_route->create3D(start_vec3, end_vec3);
    
    // TODO: In a real implementation, you'd want to check if the path is ready over time
    // since pathfinding can be async. For now we just check once.
    if (path_route->isReady())
    {
        int num_points = path_route->getNumPoints();
        for (int i = 0; i < num_points; i++)
        {
            Math::Vec3 point = path_route->getPoint(i);
            path_points.append(Math::vec3(point));
        }
    }
    else
    {
        Log::warning("[Pathfinding] No path found from (%.2f, %.2f, %.2f) to (%.2f, %.2f, %.2f)\n",
            start.x, start.y, start.z, end.x, end.y, end.z);
    }
    
    return path_points;
}

bool LuaPathfinding::isWalkable(const Math::vec3& position, float radius)
{
    // Check if position is inside any navigation area
    Math::Vec3 pos(position);
    
    // Get all nodes in the world and check navigation areas
    Vector<NodePtr> nodes;
    World::getNodes(nodes);
    
    for (int i = 0; i < nodes.size(); ++i)
    {
        if (!nodes[i] || !nodes[i]->isNavigation())
            continue;
        NavigationPtr nav = static_ptr_cast<Navigation>(nodes[i]);
        if (nav && nav->inside3D(pos, radius))
            return true;
    }
    
    return false;
}

Math::vec3 LuaPathfinding::getNearestWalkable(const Math::vec3& position, float radius)
{
    Math::Vec3 pos(position);
    Math::Vec3 nearest_pos = pos;
    float nearest_distance = 1000000.0f; // Large initial distance
    
    // If the position is already walkable, return it
    if (isWalkable(position, radius))
    {
        return position;
    }
    
    // Get all navigation areas
    Vector<NodePtr> nodes;
    World::getNodes(nodes);
    
    for (int i = 0; i < nodes.size(); ++i)
    {
        if (!nodes[i] || !nodes[i]->isNavigation())
            continue;
        NavigationPtr nav = static_ptr_cast<Navigation>(nodes[i]);
        if (!nav)
            continue;
        
        // Try to find a path from position to a point on this navigation area
        // We'll use a simple approach: check points around the navigation area
        // For a more accurate solution, we'd need to query the navigation mesh geometry
        
        // Try the center of the navigation area (if it's a sector)
        if (nav->getType() == Node::NAVIGATION_SECTOR)
        {
            NavigationSectorPtr sector = static_ptr_cast<NavigationSector>(nav);
            if (sector)
            {
                Math::Vec3 sector_pos = sector->getWorldPosition();
                float dist = static_cast<float>((pos - sector_pos).length());
                if (dist < nearest_distance && nav->inside3D(sector_pos, radius))
                {
                    nearest_distance = dist;
                    nearest_pos = sector_pos;
                }
            }
        }
        
        // For navigation meshes, we can try to find a path to a nearby point
        // This is a simplified approach - a full implementation would query mesh geometry
        if (nav->getType() == Node::NAVIGATION_MESH)
        {
            NavigationMeshPtr mesh = static_ptr_cast<NavigationMesh>(nav);
            if (mesh)
            {
                for (float x_offset = -2.0f; x_offset <= 2.0f; x_offset += 1.0f)
                {
                    for (float y_offset = -2.0f; y_offset <= 2.0f; y_offset += 1.0f)
                    {
                        Math::Vec3 test_pos = pos + Math::Vec3(x_offset, y_offset, 0.0f);
                        if (nav->inside3D(test_pos, radius))
                        {
                            float dist = static_cast<float>((pos - test_pos).length());
                            if (dist < nearest_distance)
                            {
                                nearest_distance = dist;
                                nearest_pos = test_pos;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // If we found a walkable point, use PathRoute to get a more accurate nearest point
    if (nearest_distance < 1000000.0f)
    {
        PathRoutePtr path_route = PathRoute::create(radius);
        if (path_route)
        {
            // Try to find a path from position to the nearest walkable point
            path_route->create3D(pos, nearest_pos);
            if (path_route->isReady() && path_route->getNumPoints() > 0)
            {
                // Return the first point of the path (closest walkable point)
                return Math::vec3(path_route->getPoint(0));
            }
        }
    }
    
    // Fallback: return the original position if no walkable point found
    return position;
}

bool LuaPathfinding::findPathWithPath(Unigine::PathRoutePtr path,
                                     const Math::vec3& start,
                                     const Math::vec3& end,
                                     Unigine::Vector<Math::vec3>& out_path)
{
    if (!path)
        return false;
    
    out_path.clear();
    
    // Create 3D path
    Math::Vec3 start_vec3(start);
    Math::Vec3 end_vec3(end);
    path->create3D(start_vec3, end_vec3);
    
    // Check if path is ready
    if (path->isReady())
    {
        int num_points = path->getNumPoints();
        for (int i = 0; i < num_points; i++)
        {
            Math::Vec3 point = path->getPoint(i);
            out_path.append(Math::vec3(point));
        }
        return true;
    }
    
    return false;
}

void LuaPathfinding::registerToLua(sol::state& lua)
{
    lua["Pathfinding"] = lua.create_table();
    
    lua["Pathfinding"]["findPath"] = sol::overload(
        [](const Math::vec3& start, const Math::vec3& end) {
            return findPath(start, end, 0.5f);
        },
        [](const Math::vec3& start, const Math::vec3& end, float radius) {
            return findPath(start, end, radius);
        }
    );
    
    lua["Pathfinding"]["isWalkable"] = sol::overload(
        [](const Math::vec3& pos) { return isWalkable(pos, 0.5f); },
        [](const Math::vec3& pos, float radius) { return isWalkable(pos, radius); }
    );
    
    lua["Pathfinding"]["getNearestWalkable"] = sol::overload(
        [](const Math::vec3& pos) { return getNearestWalkable(pos, 0.5f); },
        [](const Math::vec3& pos, float radius) { return getNearestWalkable(pos, radius); }
    );
    
    lua["Pathfinding"]["findPathWithPath"] = [&lua](Unigine::PathRoutePtr path,
                                               const Math::vec3& start,
                                               const Math::vec3& end) -> sol::table {
        Unigine::Vector<Math::vec3> path_points;
        if (findPathWithPath(path, start, end, path_points))
        {
            sol::table result = lua.create_table();
            for (int i = 0; i < path_points.size(); i++)
            {
                result[i + 1] = path_points[i];
            }
            return result;
        }
        return sol::nil;
    };
    
    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[Pathfinding] Pathfinding system registered\n");
}

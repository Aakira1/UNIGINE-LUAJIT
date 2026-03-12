#pragma once
#ifndef LUA_SYSTEM_FORWARD_H
#define LUA_SYSTEM_FORWARD_H

// Forward declarations header - use this in headers instead of full includes
// This reduces compilation dependencies and speeds up builds

// Standard library includes (lightweight)
#include <memory>
#include <string>
#include <unordered_map>

// Forward declarations for sol2 (only non-template types can be forward declared)
namespace sol {
    class state;
    class state_view;
}

// Note: sol::table, sol::function, sol::object are template types that require full definitions
// Headers that need them must include <sol.hpp>

// Note: UNIGINE types like NodePtr, PropertyPtr are typedefs/aliases, not classes
// They require the actual header files. Forward declarations don't work for typedefs.
// Include the minimal UNIGINE headers needed for basic types
#pragma warning(push, 0)
#include <UnigineString.h>
#include <UnigineVector.h>
#include <UnigineHashMap.h>
#include <UnigineNode.h>
#pragma warning(pop)

// Forward declarations for internal types (these are actual structs, can be forward declared)
struct LuaPropertyDef;
struct LuaComponentDescriptor;
struct LuaComponentInstance;

// Forward declarations for helper classes
namespace Unigine {
    class LuaPropertyManager;
    class LuaComponentLifecycle;
    class LuaFileScanner;
    class LuaEventSystem;
    class LuaComponentPool;
}

#endif // LUA_SYSTEM_FORWARD_H

#pragma once

// Minimal common includes - only what's needed in headers
// Heavy includes should go in .cpp files

// Forward declarations
namespace sol {
    class state;
    class state_view;
    class table;
    class function;
    class object;
    template<typename T> class basic_table_core;
}

namespace Unigine {
    class String;
    template<typename T> class Vector;
    template<typename K, typename V> class HashMap;
    class NodePtr;
    class PropertyParameterPtr;
}

// Only include lightweight headers that are needed in headers
#include <UnigineString.h>
#include <UnigineVector.h>
#include <UnigineHashMap.h>
#include <UnigineLog.h>

// Suppress warnings from external libraries
#pragma warning(push, 0)
#pragma warning(pop)

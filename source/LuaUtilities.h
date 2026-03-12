#pragma once
#ifndef LUA_UTILITIES_H
#define LUA_UTILITIES_H

#include "LuaSystemForward.h"
#include "LuaSystemTypes.h"

// Forward declaration
struct LuaComponentDescriptor;

namespace Unigine {
    class LuaUtilities {
    public:
        static Unigine::String generateSimpleGUID(const Unigine::String& name);
        static Unigine::String generateSimpleHash(const Unigine::String& name);

        static bool ComponentsNeedRegistration(Unigine::HashMap<Unigine::String, LuaComponentDescriptor>& components);
        static time_t getNewestLuaFileTime();
        static time_t getNewestGeneratedFileTime();
    };
}

#endif // LUA_UTILITIES_H
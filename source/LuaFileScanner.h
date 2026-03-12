#pragma once
#ifndef LUA_FILE_SCANNER_H
#define LUA_FILE_SCANNER_H

#include "LuaSystemForward.h"

namespace Unigine {
    class LuaFileScanner {
    public:
        void mountLuaPaths();
        void LuaScanDirectory(const char* path, sol::state* lua);
    };
}

#endif // LUA_FILE_SCANNER_H
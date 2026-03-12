#include "CommonIncludes.h"
#include "LuaAPIBindings.h"

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

// Register the Log and Console APIs to Lua
void Unigine::LuaBindings::registerUtilities(sol::state& lua)
{
    lua["Log"] = lua.create_table();
    
    // Support both string and const char* for convenience
    lua["Log"]["message"] = sol::overload(
        [](const std::string& msg) { Log::message("\033[32m[Lua Log]\033[0m %s\n", msg.c_str()); },
        [](const char* msg) { Log::message("\033[32m[Lua Log]\033[0m %s\n", msg); }
    );
    
    lua["Log"]["warning"] = sol::overload(
        [](const std::string& msg) { Log::warning("[Lua Warning] %s\n", msg.c_str()); },
        [](const char* msg) { Log::warning("[Lua Warning] %s\n", msg); }
    );
    
    lua["Log"]["error"] = sol::overload(
        [](const std::string& msg) { Log::error("[Lua Error] %s\n", msg.c_str()); },
        [](const char* msg) { Log::error("[Lua Error] %s\n", msg); }
    );

    // Console API
    sol::table console_table = lua.create_table("Console");
    //console_table.set_function("run", &Console::run);
    //console_table.set_function("getInt", &Console::getInt);
    //console_table.set_function("getFloat", &Console::getFloat);
    //console_table.set_function("getString", &Console::getString);


    console_table["run"] = &Console::run;
    console_table["getInt"] = &Console::getInt;
    console_table["getFloat"] = &Console::getFloat;
    console_table["getString"] = &Console::getString;


}

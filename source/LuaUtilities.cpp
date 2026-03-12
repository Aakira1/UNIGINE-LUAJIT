#include "CommonIncludes.h"
#include "LuaUtilities.h"
#include "LuaSystem.h"
#include <filesystem>
#include <chrono>

using namespace Unigine;

bool LuaUtilities::ComponentsNeedRegistration(HashMap<String, LuaComponentDescriptor>& components)
{
    // check if the generated header files exist for all components
    for (HashMap<String, LuaComponentDescriptor>::Iterator it = components.begin();
        it != components.end();
        ++it)
    {
        String class_name = String::format("Lua_%s", it->data.name.get());
        String header_file = String::format("lua_components_generated/%s.h", class_name.get());

        if (!std::filesystem::exists(header_file.get())) return true;
    }

    std::time_t lua_time = getNewestLuaFileTime();
    std::time_t gen_time = getNewestGeneratedFileTime();

    return lua_time > gen_time;
}

time_t LuaUtilities::getNewestLuaFileTime()
{
    std::time_t newest = 0;


    std::filesystem::path lua_dir = "data/lua_components";

    if (!std::filesystem::exists(lua_dir)) {
        return 0;  // Directory doesn't exist yet
    }

    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(lua_dir))
    {
        if (entry.path().extension() == ".lua")
        {
            std::filesystem::file_time_type ftime = std::filesystem::last_write_time(entry);
            std::chrono::system_clock::time_point sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
            );
            std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);

            if (cftime > newest)
                newest = cftime;
        }
    }


    return newest;
}

time_t LuaUtilities::getNewestGeneratedFileTime()
{
    std::filesystem::path gen_dir = "source/lua_components_generated";

    if (!std::filesystem::exists(gen_dir))
        return 0;

    std::time_t newest = 0;

    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(gen_dir))
    {
        if (entry.path().extension() == ".h" || entry.path().extension() == ".cpp")
        {
            std::filesystem::file_time_type ftime = std::filesystem::last_write_time(entry);
            std::chrono::system_clock::time_point sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
            );
            std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);

            if (cftime > newest)
                newest = cftime;
        }
    }

    return newest;
}

String LuaUtilities::generateSimpleGUID(const String& name)
{
    // Simple GUID generation based on name
    // In production, you'd want a proper GUID generator

    unsigned int hash = 0;
    const char* str = name.get();

    for (int i = 0; str[i] != '\0'; i++)
    {
        hash = hash * 31 + str[i];
    }

    // Generate a pseudo-GUID format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    // This is NOT a real GUID but sufficient for our purposes (fake guid)
    return String::format("%08x%04x%04x%04x%012x",
        hash,
        (hash >> 16) & 0xFFFF,
        (hash >> 8) & 0xFFFF,
        hash & 0xFFFF,
        hash * 0x123456
    );
}

String LuaUtilities::generateSimpleHash(const String& name)
{
    // Simple 32-bit hash
    unsigned int hash = 0;
    const char* str = name.get();

    for (int i = 0; str[i] != '\0'; i++)
    {
        hash = hash * 31 + str[i];
    }

    return String::format("%08x", hash);
}
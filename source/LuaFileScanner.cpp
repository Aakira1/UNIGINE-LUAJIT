#include "CommonIncludes.h"
#include "LuaFileScanner.h"
#include "LuaSystem.h"
#include <filesystem>

using namespace Unigine;
namespace fs = std::filesystem;

void LuaFileScanner::mountLuaPaths()
{
    FileSystem::addMount("lua_components/");
}

void LuaFileScanner::LuaScanDirectory(const char* path, sol::state* lua)
{
    Vector<String> files;

    Unigine::String resolved_path = Unigine::FileSystem::getAbsolutePath(path);

    if (resolved_path.empty())
    {
        Unigine::Log::warning("[LuaSystem] Invalid path: %s\n", path);
        return;
    }

    fs::path root(resolved_path.get());
    if (!fs::exists(root))
    {
        Unigine::Log::warning("[LuaSystem] Creating missing folder: %s\n", resolved_path.get());
        fs::create_directories(root);
        return;
    }

    try
    {
        for (const fs::directory_entry& entry : fs::recursive_directory_iterator(root))
        {
            if (entry.is_regular_file() && entry.path().extension() == ".lua")
            {
                files.append(entry.path().string().c_str());
            }
        }
    }
    catch (const fs::filesystem_error& e)
    {
        Unigine::Log::error("[LuaSystem] Filesystem error in '%s': %s\n",
            resolved_path.get(), e.what());
        return;
    }

    LuaSystem* ls = LuaSystem::get();
    bool should_log_scan = ls ? ls->getLogScan() : true;
    bool should_log_load = ls ? ls->getLogLoad() : false;
    
    if (files.empty())
    {
        Unigine::Log::warning("[LuaSystem] No Lua files found\n");
    }
    else if (should_log_scan)
    {
        Unigine::Log::message("[LuaSystem] Found %d file(s)\n", files.size());
    }

    for (const Unigine::String& file : files)
    {
        try
        {
            if (should_log_load)
                Unigine::Log::message("[LuaSystem] Loading: %s\n", file.get());
            lua->script_file(file.get());
        }
        catch (const sol::error& e)
        {
            Unigine::Log::error("[LuaSystem] Error loading %s: %s\n",
                file.get(), e.what());
        }
    }
}
#include "CommonIncludes.h"
#include "LuaSystem.h"
#include "LuaComponentGenerator.h"
#include <filesystem>
#include <fstream>
#include <tuple>
#include "LuaAPIBindings.h"
#include "LuaPropertyManager.h"
#include "LuaComponentLifecycle.h"
#include "LuaFileScanner.h"
#include "LuaUtilities.h"
#include "LuaEventSystem.h"
#include "LuaComponentPool.h"
#include "LuaCoroutineHelpers.h"
#include "LuaInputSystem.h"
#include "LuaProfiler.h"
#include "LuaComponentQueries.h"
#include "LuaPropertySerialization.h"
#include "LuaPropertyValidation.h"
#include "LuaAutocompleteGenerator.h"
#include "LuaDebugger.h"
#include "LuaMaterialRuntime.h"
#include "LuaAnimationSystem.h"
#include "LuaPathfinding.h"
#include <UnigineConsole.h>

using namespace Unigine;
using namespace Unigine::Math;
namespace fs = std::filesystem;

LuaSystem::LuaSystem()
{
    property_manager = std::make_unique<LuaPropertyManager>();
    file_scanner = std::make_unique<LuaFileScanner>();

    // lifecycle_manager created after lua state
    active_components = new Unigine::Vector<LuaComponentInstance>();
    components = new Unigine::HashMap<Unigine::String, LuaComponentDescriptor>();
    bindings = new std::unordered_map<std::string, Unigine::Vector<LuaPropertyDef>>();
    
    // Load persisted log settings, or use defaults
    loadLogSettings();
}

LuaSystem::~LuaSystem()
{
    // Clean up Lua state before destroying helper classes
    if (lua) {
        // Clear all active components first to release Lua references
        if (active_components) {
            active_components->clear();
        }
        
        // Clear event listeners
        LuaEventSystem::Clear();
        
        // Force garbage collection to clean up Lua objects
        lua->collect_garbage();
        
        // Note: We don't delete lua here because:
        // 1. sol::state manages its own lifetime through RAII
        // 2. Deleting it here can cause crashes if Lua objects still hold references
        // 3. The singleton pattern ensures proper destruction order
        // The lua state will be destroyed when the singleton is destroyed
    }
    
    // Clean up containers
    delete active_components;
    delete components;
    delete bindings;
}

LuaSystem* LuaSystem::get()
{
    static LuaSystem instance;
    return &instance;
}

void Unigine::LuaSystem::initialize()
{
    if (initialized)
    {
        Log::error("[LuaSystem] initialize() called more than once\n");
        return;
    }

    if (log_init)
        Log::message("[LuaSystem] Initializing...\n");

    lua = new sol::state();
    lua->open_libraries(sol::lib::base, sol::lib::package,
        sol::lib::string, sol::lib::math,
        sol::lib::table, sol::lib::io,
        sol::lib::os, sol::lib::coroutine);

    // Set up lifecycle manager and component pool
    lifecycle_manager = std::make_unique<LuaComponentLifecycle>(lua);
    component_pool = std::make_unique<LuaComponentPool>(lua);

    mountLuaPaths();
    Unigine::LuaBindings::registerAll(*lua);
    
    // Hook up all the helper systems
    Unigine::LuaCoroutineHelpers::registerHelpers(*lua);
    Unigine::LuaInputSystem::registerToLua(*lua);
    Unigine::LuaProfiler::registerToLua(*lua);
    Unigine::LuaMaterialRuntime::registerToLua(*lua);
    Unigine::LuaAnimationSystem::registerToLua(*lua);
    Unigine::LuaPathfinding::registerToLua(*lua);

    // Expose component registration to Lua
    if (lua) {
        lua->set_function("component",
            [this](const std::string& name, const sol::table& def)
            {
                registerLuaComponent(name, def);
            });
    }

    initialized = true;

    // Wire up the event system 
    if (lua) {
        // Core event functions
        lua->set_function("subscribe", sol::overload(
            [](const std::string& event_name, sol::function callback) {
                LuaEventSystem::Subscribe(event_name, callback, 0);
            },
            [](const std::string& event_name, sol::function callback, int priority) {
                LuaEventSystem::Subscribe(event_name, callback, priority);
            }
        ));
        
        lua->set_function("subscribeOnce", sol::overload(
            [](const std::string& event_name, sol::function callback) {
                LuaEventSystem::SubscribeOnce(event_name, callback, 0);
            },
            [](const std::string& event_name, sol::function callback, int priority) {
                LuaEventSystem::SubscribeOnce(event_name, callback, priority);
            }
        ));
        
        lua->set_function("subscribeWithId", sol::overload(
            [](const std::string& event_name, const std::string& id, sol::function callback) {
                LuaEventSystem::SubscribeWithId(event_name, id, callback, 0);
            },
            [](const std::string& event_name, const std::string& id, sol::function callback, int priority) {
                LuaEventSystem::SubscribeWithId(event_name, id, callback, priority);
            }
        ));
        
        lua->set_function("unsubscribe", [](const std::string& event_name, sol::function callback) {
            return LuaEventSystem::Unsubscribe(event_name, callback);
        });
        
        lua->set_function("unsubscribeById", [](const std::string& event_name, const std::string& id) {
            return LuaEventSystem::UnsubscribeById(event_name, id);
        });
        
        lua->set_function("unsubscribeAll", [](const std::string& event_name) {
            LuaEventSystem::UnsubscribeAll(event_name);
        });
        
        lua->set_function("emit", &LuaEventSystem::Emit);
        lua->set_function("emitDeferred", &LuaEventSystem::EmitDeferred);
        
        lua->set_function("clearEvents", sol::overload(
            []() { LuaEventSystem::Clear(); },
            [](const std::string& event_name) { LuaEventSystem::ClearEvent(event_name); }
        ));
        
        // Keep the old clear function for backwards compatibility
        lua->set_function("clear", &LuaEventSystem::Clear);
        
        // Helper functions for checking event state
        lua->set_function("hasListeners", &LuaEventSystem::HasListeners);
        lua->set_function("getListenerCount", &LuaEventSystem::GetListenerCount);
        lua->set_function("getEmitCount", &LuaEventSystem::GetEmitCount);
        
        // Enable/disable
        lua->set_function("enableListener", [](const std::string& event_name, const std::string& id, bool enable) {
            LuaEventSystem::EnableListener(event_name, id, enable);
        });
        
        lua->set_function("enableAllListeners", [](const std::string& event_name, bool enable) {
            LuaEventSystem::EnableAllListeners(event_name, enable);
        });
        
        // Statistics
        lua->set_function("printEventStats", &LuaEventSystem::PrintStatistics);
        lua->set_function("resetEventStats", &LuaEventSystem::ResetStatistics);
    }

    if (log_init)
        Log::message("[LuaSystem] Waiting for engine systems...\n");
}

// Console Commands:
// Tools: lua_tool_autocomplete, lua_tool_debugger_enable/disable, lua_tool_profiler_reset/stats,
//        lua_tool_reload, lua_tool_components
// Logs: lua_tool_log_init/scan/load/parse/registration/generation/properties/pending/all/status
// See CONSOLE_COMMANDS.md for details
void Unigine::LuaSystem::registerConsoleCommands()
{
    // Autocomplete generation
    Console::addCommand("lua_tool_autocomplete", "Generate autocomplete files for Lua API (usage: lua_tool_autocomplete [output_dir])", 
        MakeCallback(this, &LuaSystem::consoleGenerateAutocomplete));
    
    // Debugger controls
    Console::addCommand("lua_tool_debugger_enable", "Enable Lua debugger (usage: lua_tool_debugger_enable [port])", 
        MakeCallback(this, &LuaSystem::consoleEnableDebugger));
    Console::addCommand("lua_tool_debugger_disable", "Disable Lua debugger", 
        MakeCallback(this, &LuaSystem::consoleDisableDebugger));
    
    // Profiler controls
    Console::addCommand("lua_tool_profiler_reset", "Reset Lua profiler data", 
        MakeCallback(this, &LuaSystem::consoleProfilerReset));
    Console::addCommand("lua_tool_profiler_stats", "Print Lua profiler statistics", 
        MakeCallback(this, &LuaSystem::consoleProfilerStats));
    
    // Hot reload
    Console::addCommand("lua_tool_reload", "Reload all Lua scripts", 
        MakeCallback(this, &LuaSystem::consoleReloadScripts));
    
    // Component info
    Console::addCommand("lua_tool_components", "List all registered Lua components", 
        MakeCallback(this, &LuaSystem::consoleListComponents));
    
    // Log control commands
    Console::addCommand("lua_tool_log_init", "Toggle initialization logs (usage: lua_tool_log_init [0/1])", 
        MakeCallback(this, &LuaSystem::consoleLogInit));
    Console::addCommand("lua_tool_log_scan", "Toggle file scanning logs (usage: lua_tool_log_scan [0/1])", 
        MakeCallback(this, &LuaSystem::consoleLogScan));
    Console::addCommand("lua_tool_log_load", "Toggle file loading logs (usage: lua_tool_log_load [0/1])", 
        MakeCallback(this, &LuaSystem::consoleLogLoad));
    Console::addCommand("lua_tool_log_parse", "Toggle component parsing logs (usage: lua_tool_log_parse [0/1])", 
        MakeCallback(this, &LuaSystem::consoleLogParse));
    Console::addCommand("lua_tool_log_registration", "Toggle component registration logs (usage: lua_tool_log_registration [0/1])", 
        MakeCallback(this, &LuaSystem::consoleLogRegistration));
    Console::addCommand("lua_tool_log_generation", "Toggle code generation logs (usage: lua_tool_log_generation [0/1])", 
        MakeCallback(this, &LuaSystem::consoleLogGeneration));
    Console::addCommand("lua_tool_log_properties", "Toggle property file logs (usage: lua_tool_log_properties [0/1])", 
        MakeCallback(this, &LuaSystem::consoleLogProperties));
    Console::addCommand("lua_tool_log_pending", "Toggle pending component warnings (usage: lua_tool_log_pending [0/1])", 
        MakeCallback(this, &LuaSystem::consoleLogPending));
    Console::addCommand("lua_tool_log_all", "Toggle all logs (usage: lua_tool_log_all [0/1])", 
        MakeCallback(this, &LuaSystem::consoleLogAll));
    Console::addCommand("lua_tool_log_status", "Show current log settings", 
        MakeCallback(this, &LuaSystem::consoleLogStatus));
}

void Unigine::LuaSystem::consoleGenerateAutocomplete(int argc, char** argv)
{
    Unigine::String output_dir = "data";
    if (argc > 1)
        output_dir = argv[1];
    
    generateAutocompleteFiles(output_dir);
    Console::write("Autocomplete files generated in: " + output_dir + "\n");
}

void Unigine::LuaSystem::consoleEnableDebugger(int argc, char** argv)
{
    int port = 8172;
    if (argc > 1)
        port = atoi(argv[1]);
    
    enableDebugger(port);
    Console::write(String::format("Lua debugger enabled on port %d\n", port));
}

void Unigine::LuaSystem::consoleDisableDebugger([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    disableDebugger();
    Console::write("Lua debugger disabled\n");
}

void Unigine::LuaSystem::consoleProfilerReset([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    LuaProfiler::get().reset();
    Console::write("Lua profiler data reset\n");
}

void Unigine::LuaSystem::consoleProfilerStats([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    auto all_data = LuaProfiler::get().getAllProfileData();
    if (all_data.empty())
    {
        Console::write("No profiler data available\n");
        return;
    }
    
    Console::write("=== Lua Profiler Statistics ===\n");
    for (const auto& pair : all_data)
    {
        const auto& data = pair.second;
        float avg = data.call_count > 0 ? data.total_time / data.call_count : 0.0f;
        Console::write(String::format("%s: calls=%d, total=%.3fms, avg=%.3fms, min=%.3fms, max=%.3fms\n",
            data.name.c_str(), data.call_count, data.total_time * 1000.0f, avg * 1000.0f,
            data.min_time * 1000.0f, data.max_time * 1000.0f));
    }
}

void Unigine::LuaSystem::consoleReloadScripts([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    reloadAllLuaScripts();
    Console::write("All Lua scripts reloaded\n");
}

void Unigine::LuaSystem::consoleListComponents([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    Console::write(String::format("=== Registered Lua Components (%d) ===\n", components->size()));
    for (auto it = components->begin(); it != components->end(); ++it)
    {
        Console::write(String::format("  - %s\n", it->data.name.get()));
    }
}

void Unigine::LuaSystem::consoleLogInit(int argc, char** argv)
{
    if (argc > 1)
        log_init = (atoi(argv[1]) != 0);
    Console::write(String::format("Initialization logs: %s\n", log_init ? "ON" : "OFF"));
    saveLogSettings();
}

void Unigine::LuaSystem::consoleLogScan(int argc, char** argv)
{
    if (argc > 1)
        log_scan = (atoi(argv[1]) != 0);
    Console::write(String::format("File scanning logs: %s\n", log_scan ? "ON" : "OFF"));
    saveLogSettings();
}

void Unigine::LuaSystem::consoleLogLoad(int argc, char** argv)
{
    if (argc > 1)
        log_load = (atoi(argv[1]) != 0);
    Console::write(String::format("File loading logs: %s\n", log_load ? "ON" : "OFF"));
    saveLogSettings();
}

void Unigine::LuaSystem::consoleLogParse(int argc, char** argv)
{
    if (argc > 1)
        log_parse = (atoi(argv[1]) != 0);
    Console::write(String::format("Component parsing logs: %s\n", log_parse ? "ON" : "OFF"));
    saveLogSettings();
}

void Unigine::LuaSystem::consoleLogRegistration(int argc, char** argv)
{
    if (argc > 1)
        log_registration = (atoi(argv[1]) != 0);
    Console::write(String::format("Component registration logs: %s\n", log_registration ? "ON" : "OFF"));
    saveLogSettings();
}

void Unigine::LuaSystem::consoleLogGeneration(int argc, char** argv)
{
    if (argc > 1)
        log_generation = (atoi(argv[1]) != 0);
    Console::write(String::format("Code generation logs: %s\n", log_generation ? "ON" : "OFF"));
    saveLogSettings();
}

void Unigine::LuaSystem::consoleLogProperties(int argc, char** argv)
{
    if (argc > 1)
        log_properties = (atoi(argv[1]) != 0);
    Console::write(String::format("Property file logs: %s\n", log_properties ? "ON" : "OFF"));
    saveLogSettings();
}

void Unigine::LuaSystem::consoleLogPending(int argc, char** argv)
{
    if (argc > 1)
        log_pending = (atoi(argv[1]) != 0);
    Console::write(String::format("Pending component warnings: %s\n", log_pending ? "ON" : "OFF"));
    saveLogSettings();
}

void Unigine::LuaSystem::consoleLogAll(int argc, char** argv)
{
    bool enable = true;
    if (argc > 1)
        enable = (atoi(argv[1]) != 0);
    
    log_init = enable;
    log_scan = enable;
    log_load = enable;
    log_parse = enable;
    log_registration = enable;
    log_generation = enable;
    log_properties = enable;
    log_pending = enable;
    
    Console::write(String::format("All logs: %s\n", enable ? "ON" : "OFF"));
    saveLogSettings();
}

void Unigine::LuaSystem::consoleLogStatus([[maybe_unused]] int argc, [[maybe_unused]] char** argv) const
{
    Console::write("=== Lua System Log Settings ===\n");
    Console::write(String::format("  Init: %s\n", log_init ? "ON" : "OFF"));
    Console::write(String::format("  Scan: %s\n", log_scan ? "ON" : "OFF"));
    Console::write(String::format("  Load: %s\n", log_load ? "ON" : "OFF"));
    Console::write(String::format("  Parse: %s\n", log_parse ? "ON" : "OFF"));
    Console::write(String::format("  Registration: %s\n", log_registration ? "ON" : "OFF"));
    Console::write(String::format("  Generation: %s\n", log_generation ? "ON" : "OFF"));
    Console::write(String::format("  Properties: %s\n", log_properties ? "ON" : "OFF"));
    Console::write(String::format("  Pending: %s\n", log_pending ? "ON" : "OFF"));
}

void Unigine::LuaSystem::postInitialize()
{
    if (post_initialized)
        return;

    if (!initialized)
    {
        Log::error("[LuaSystem] Must call initialize() before postInitialize()\n");
        return;
    }

    if (log_init)
        Log::message("[LuaSystem] Post-initializing...\n");
    
    // Register console commands after engine systems are ready
    registerConsoleCommands();
    
    if (log_scan)
        Log::message("[LuaSystem] Scanning lua_components...\n");
    LuaScanDirectory("lua_components");

    if (log_generation)
        Log::message("[LuaSystem] Generating wrappers...\n");
    LuaComponentGenerator::generateAllComponents(*components);

    if (log_properties)
        Log::message("[LuaSystem] Creating property files...\n");
    createPropertyFilesImmediately();

    post_initialized = true;

    // Handle components that were waiting for their dependencies
    processPendingComponents();

    if (log_registration)
        Log::message("[LuaSystem] Registered %d component(s)\n", components->size());
    
    if (lua) {
        lua->set_function("findComponentsByType", [this](const std::string& type) {
            return LuaComponentQueries::findByType(type.c_str(), *active_components);
        });
        
        lua->set_function("findComponentsByTag", [this](const std::string& tag) {
            return LuaComponentQueries::findByTag(tag.c_str(), *active_components);
        });
        
        lua->set_function("findComponentOnNode", [this](Unigine::NodePtr node, const std::string& type) {
            return LuaComponentQueries::findByNode(node, type.c_str(), *active_components);
        });
        
        // Property serialization
        lua->set_function("serializeComponent", [](const LuaComponentInstance& instance) {
            return LuaPropertySerialization::serializeToJSON(instance);
        });
        
        lua->set_function("saveComponentToFile", [](const LuaComponentInstance& instance, const std::string& path) {
            return LuaPropertySerialization::saveToFile(instance, path.c_str());
        });
        
        // Node state save/load
        lua->set_function("saveNodeToFile", [this](Unigine::NodePtr node, const std::string& path) {
            if (!node || !active_components)
                return false;
            std::string json = LuaPropertySerialization::serializeNodeComponents(node, *active_components);
            if (json.empty())
                return false;
            
            std::ofstream file(path);
            if (!file.is_open())
                return false;
            file << json;
            file.close();
            return true;
        });
        
        lua->set_function("loadNodeFromFile", [this](Unigine::NodePtr node, const std::string& path) {
            if (!node || !active_components || !lua)
                return false;
            
            std::ifstream file(path);
            if (!file.is_open())
                return false;
            
            std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            
            return LuaPropertySerialization::deserializeNodeComponents(json, node, *active_components, *lua);
        });
    }

    if (log_init)
        Log::message("[LuaSystem] Ready\n");
}

void Unigine::LuaSystem::generateAutocompleteFiles(const Unigine::String& output_dir)
{
    Unigine::String lsp_path = output_dir + "/unigine_lsp.lua";
    Unigine::String stub_path = output_dir + "/unigine_stubs.lua";
    Unigine::String emmy_path = output_dir + "/unigine_emmy.lua";
    
    LuaAutocompleteGenerator::generateLSPDefinitions(lsp_path);
    LuaAutocompleteGenerator::generateLuaStubs(stub_path);
    LuaAutocompleteGenerator::generateEmmyLuaAnnotations(emmy_path);
}

void Unigine::LuaSystem::enableDebugger(int port)
{
    LuaDebugger::get().initialize(LuaDebugger::Protocol::MobDebug, port);
    if (lua)
    {
        LuaDebugger::get().registerHooks(*lua);
    }
}

void Unigine::LuaSystem::disableDebugger()
{
    LuaDebugger::get().shutdown();
}

void Unigine::LuaSystem::processPendingComponents()
{
    // Try to register components that were waiting for dependencies
    bool progress = true;
    int max_iterations = 100; // Safety limit to avoid infinite loops
    int iteration = 0;

    while (progress && iteration < max_iterations && pending_components.size() > 0)
    {
        progress = false;
        iteration++;

        for (int i = pending_components.size() - 1; i >= 0; i--)
        {
            LuaComponentDescriptor& desc = pending_components[i];
            bool dependencies_met = true;

            for (int j = 0; j < desc.required_components.size(); j++)
            {
                const Unigine::String& dep_name = desc.required_components[j];
                if (components->find(dep_name) == components->end())
                {
                    dependencies_met = false;
                    break;
                }
            }

            if (dependencies_met)
            {
                // Copy desc before removing from vector (since remove invalidates the reference)
                LuaComponentDescriptor desc_copy = desc;
                
                (*components)[desc_copy.name] = desc_copy;
                if (log_registration)
                    Log::message("[LuaSystem] Registered: %s\n", desc_copy.name.get());
                
                loadAdditionalComponentFiles(desc_copy);
                
                pending_components.remove(i);
                progress = true;
            }
        }
    }

    if (pending_components.size() > 0 && log_pending)
    {
        Log::warning("[LuaSystem] %d pending (missing deps): ", pending_components.size());
        for (int i = 0; i < pending_components.size(); i++)
        {
            if (i > 0) Log::warning(", ");
            Log::warning("%s", pending_components[i].name.get());
        }
        Log::warning("\n");
    }
}

void Unigine::LuaSystem::loadAdditionalComponentFiles(const LuaComponentDescriptor& desc)
{
    if (desc.additional_files.size() == 0 || !lua)
        return;

    if (log_load)
        Log::message("[LuaSystem] Loading additional files for '%s':\n", desc.name.get());
    
    for (int i = 0; i < desc.additional_files.size(); i++)
    {
        const Unigine::String& file_path = desc.additional_files[i];
        Unigine::String resolved_path = Unigine::FileSystem::getAbsolutePath(file_path.get());
        
        if (resolved_path.empty())
        {
            if (log_load)
                Log::warning("[LuaSystem] Could not resolve: %s\n", file_path.get());
            continue;
        }

        try
        {
            if (log_load)
                Log::message("[LuaSystem] Loading: %s\n", resolved_path.get());
            lua->script_file(resolved_path.get());
        }
        catch (const sol::error& e)
        {
            Log::error("[LuaSystem] Error loading additional file %s: %s\n",
                resolved_path.get(), e.what());
        }
    }
}

bool Unigine::LuaSystem::isShuttingDown() const noexcept
{
    return is_shutting_down;
}

sol::state& Unigine::LuaSystem::getState() noexcept
{
    return *lua;
}

void Unigine::LuaSystem::shutdown()
{
    is_shutting_down = true;  

    if (lifecycle_manager && lua) {
        LuaComponentLifecycle::shutdown(
            *active_components,
            *components,
            *bindings,
            is_shutting_down,
            *lua
        );
    }
    
    // Clean up Lua state
    if (lua) {
        // Clear event listeners
        LuaEventSystem::Clear();
        
        // Clear active components to release Lua references
        if (active_components) {
            active_components->clear();
        }
        
        // Force garbage collection
        lua->collect_garbage();
    }
}

// ============================================
// Update & Shutdown - DELEGATED
// ============================================
void Unigine::LuaSystem::update(float ifps)
{
    if (is_shutting_down || !initialized || !post_initialized || !lua || lua->lua_state() == nullptr)
		return;

    checkForHotReload(ifps);

    LuaInputSystem::get().update();
    LuaAnimationSystem::updateBlends(ifps);
    LuaCoroutineHelpers::updateCoroutines(ifps);

    if (lifecycle_manager && active_components) {
        lifecycle_manager->update(ifps, *active_components);
    }
}

// ============================================
// Component Registration - from Lua
// ============================================
void Unigine::LuaSystem::registerLuaComponent(const std::string& raw_name, const sol::table& definition)
{
    LuaComponentDescriptor desc;
    desc.name = raw_name.c_str();
    desc.description = definition.get_or<std::string>("description", "").c_str();

    // Read component dependencies (requires field)
    sol::object requires_obj = definition["requires"];
    if (requires_obj.valid() && requires_obj.is<sol::table>())
    {
        sol::table requires_table = requires_obj.as<sol::table>();
        requires_table.for_each([&desc]([[maybe_unused]] const sol::object& key, const sol::object& value) {
            if (value.is<std::string>())
            {
                desc.required_components.append(value.as<std::string>().c_str());
            }
        });
    }

    // Read additional files for multi-file components
    sol::object files_obj = definition["files"];
    if (files_obj.valid() && files_obj.is<sol::table>())
    {
        sol::table files_table = files_obj.as<sol::table>();
        files_table.for_each([&desc]([[maybe_unused]] const sol::object& key, const sol::object& value) {
            if (value.is<std::string>())
            {
                desc.additional_files.append(value.as<std::string>().c_str());
            }
        });
    }

    // Read all lifecycle function names
    desc.fn_on_init = definition.get_or<std::string>("on_init", "").c_str();
    desc.fn_on_update_async_thread = definition.get_or<std::string>("on_update_async_thread", "").c_str();
    desc.fn_on_update_sync_thread = definition.get_or<std::string>("on_update_sync_thread", "").c_str();
    desc.fn_on_update = definition.get_or<std::string>("on_update", "").c_str();
    desc.fn_on_post_update = definition.get_or<std::string>("on_post_update", "").c_str();
    desc.fn_on_update_physics = definition.get_or<std::string>("on_update_physics", "").c_str();
    desc.fn_on_swap = definition.get_or<std::string>("on_swap", "").c_str();
    desc.fn_on_shutdown = definition.get_or<std::string>("on_shutdown", "").c_str();

    // Read property table - DELEGATED
    sol::object props = definition["properties"];
    if (props.valid() && props.is<sol::table>())
    {
        parseProperties(props.as<sol::table>(), desc.properties);
    }

    // Check dependencies before registering
    bool dependencies_met = true;
    for (int i = 0; i < desc.required_components.size(); i++)
    {
        const Unigine::String& dep_name = desc.required_components[i];
        if (components->find(dep_name) == components->end())
        {
            if (log_pending)
            Log::warning("[LuaSystem] Component '%s' requires '%s' (not registered yet)\n",
                desc.name.get(), dep_name.get());
            dependencies_met = false;
        }
    }

    if (dependencies_met)
    {
        (*components)[desc.name] = desc;
        
        if (log_parse)
        {
            Log::message("[LuaSystem] Parsed: %s", desc.name.get());
            if (desc.required_components.size() > 0)
            {
                Log::message(" (requires: ");
                for (int i = 0; i < desc.required_components.size(); i++)
                {
                    if (i > 0) Log::message(", ");
                    Log::message("%s", desc.required_components[i].get());
                }
                Log::message(")");
            }
            Log::message("\n");
        }
        
        processPendingComponents();
    }
    else
    {
        pending_components.append(desc);
        if (log_pending)
            Log::message("[LuaSystem] Pending: %s\n", desc.name.get());
    }
}
void Unigine::LuaSystem::removeLuaComponent(const Unigine::NodePtr& node, const Unigine::String& component_name)
{
    if (!lifecycle_manager || !active_components) {
        Log::error("[LuaSystem] Cannot remove component: lifecycle_manager or active_components is null\n");
        return;
    }
    lifecycle_manager->removeLuaComponent(node, component_name, *active_components);
}

// ============================================
// Instance Management - DELEGATED
// ============================================
LuaComponentDescriptor* LuaSystem::getComponentDescriptor(const String& name)
{
    HashMap<String, LuaComponentDescriptor>::Iterator it = components->find(name);
    if (it != components->end())
        return &it->data;
    return nullptr;
}
void LuaSystem::registerComponentInstance(NodePtr node, const LuaComponentDescriptor& desc, sol::table instance)
{
    if (!lifecycle_manager || !active_components) {
        Log::error("[LuaSystem] Cannot register instance: lifecycle_manager or active_components is null\n");
        return;
    }
    lifecycle_manager->registerComponentInstance(node, desc, instance, *active_components);
}
void LuaSystem::unregisterComponentInstance(NodePtr node, const String& component_name)
{
    if (!lifecycle_manager || !active_components) {
        Log::error("[LuaSystem] Cannot unregister instance: lifecycle_manager or active_components is null\n");
        return;
    }
    lifecycle_manager->unregisterComponentInstance(node, component_name, *active_components);
}

// ============================================
// Engine Registration (Legacy)
// ============================================
bool Unigine::LuaSystem::registerLuaComponentWithEngine(const LuaComponentDescriptor& d)
{
    ComponentSystem* cs = ComponentSystem::get();
    if (!cs)
        return false;

    cs->createPropertyFile(d.name.get());
    return true;
}
bool Unigine::LuaSystem::registerLuaPropertyWithEngine(const LuaComponentDescriptor& d)
{
    PropertyPtr prop = Properties::findManualProperty(d.name.get());
    if (!prop)
        return false;

    return bindPropertyParameters(d, prop);
}
void Unigine::LuaSystem::attachLuaInstance(NodePtr node, const LuaComponentDescriptor& desc, [[maybe_unused]] PropertyPtr prop_instance)
{
    if (!lua) {
        Log::error("[LuaSystem] Cannot attach instance: Lua state is null\n");
        return;
    }
    
    sol::table instance = lua->create_table();

    LuaComponentInstance ci;
    ci.node = node;
    ci.descriptor = desc;
    ci.instance = instance;

    // Bind lifecycle functions
    ci.on_init = (*lua)[desc.fn_on_init.get()];
    ci.on_update = (*lua)[desc.fn_on_update.get()];
	ci.on_update_async_thread = (*lua)[desc.fn_on_update_async_thread.get()];
	ci.on_update_sync_thread = (*lua)[desc.fn_on_update_sync_thread.get()];
	ci.on_update_physics = (*lua)[desc.fn_on_update_physics.get()];
    ci.on_shutdown = (*lua)[desc.fn_on_shutdown.get()];
	ci.on_post_update = (*lua)[desc.fn_on_post_update.get()];
	ci.on_swap = (*lua)[desc.fn_on_swap.get()];

    // Seed Lua instance with property values
    std::unordered_map<std::string, Unigine::Vector<LuaPropertyDef>>::iterator bind_it = bindings->find(desc.name.get());
    if (bind_it != bindings->end())
    {
        const Unigine::Vector<LuaPropertyDef>& bind_list = bind_it->second;
        for (const LuaPropertyDef& entry : bind_list)
        {
            const char* key = entry.lua_name.get();
            PropertyParameterPtr param = entry.param;


            if (entry.type == String("float"))
                instance[key] = param->getValueFloat();
            else if (entry.type == String("int"))
                instance[key] = param->getValueInt();
            else if (entry.type == String("bool"))
                instance[key] = param->getValueToggle();
            else if (entry.type == String("string"))
                instance[key] = String(param->getValueString());
            else if (entry.type == String("vec3") || entry.type == String("Vec3"))
                instance[key] = param->getValueVec3();
            else if (entry.type == String("vec4") || entry.type == String("color"))  // Combined
                instance[key] = param->getValueVec4();
            else if (entry.type == String("quat"))  // Add quaternion support (figure out conversion later)
                instance[key] = param->getValueVec4();
            else if (entry.type == String("switch"))
                instance[key] = param->getValueSwitch();
            else if (entry.type == String("mask"))
                instance[key] = param->getValueMask();
            else
            {
                Log::warning("Unknown property type '%s' for property '%s'\n",
                    entry.type.get(), key);
            }
        }
    }

    active_components->append(ci);
}

// ============================================
// Property Methods - DELEGATED
// ============================================
bool Unigine::LuaSystem::bindPropertyParameters(const LuaComponentDescriptor& d, const Unigine::PropertyPtr& prop)
{
    if (!property_manager || !bindings) {
        Log::error("[LuaSystem] Cannot bind property parameters: property_manager or bindings is null\n");
        return false;
    }
    return property_manager->bindPropertyParameters(d, prop, *bindings);
}
int Unigine::LuaSystem::luaTypeToPropertyType(const Unigine::String& lua_type)
{
    if (!property_manager) {
        Log::error("[LuaSystem] Cannot convert type: property_manager is null\n");
        return 0;
    }
    return property_manager->luaTypeToPropertyType(lua_type);
}
void Unigine::LuaSystem::setDefaultValue(const Unigine::PropertyParameterPtr& param, const LuaPropertyDef& def)
{
    if (!property_manager) {
        Log::error("[LuaSystem] Cannot set default value: property_manager is null\n");
        return;
    }
    property_manager->setDefaultValue(param, def);
}
void Unigine::LuaSystem::createPropertyFilesImmediately()
{
    if (!property_manager || !components) {
        Log::error("[LuaSystem] Cannot create property files: property_manager or components is null\n");
        return;
    }
    property_manager->createPropertyFilesImmediately(*components, property_files_created);
}
void Unigine::LuaSystem::parseProperties(const sol::table& props, Unigine::Vector<LuaPropertyDef>& out)
{
    if (!property_manager) {
        Log::error("[LuaSystem] Cannot parse properties: property_manager is null\n");
        return;
    }
    property_manager->parseProperties(props, out);
}
void Unigine::LuaSystem::syncLuaComponents()
{
    if (!property_manager || !active_components || !bindings) {
        Log::error("[LuaSystem] Cannot sync components: property_manager, active_components, or bindings is null\n");
        return;
    }
    property_manager->syncLuaComponents(*active_components, *bindings);
}

// ============================================
// Utility Methods - DELEGATED
// ============================================
bool Unigine::LuaSystem::ComponentsNeedRegistration()
{
    return LuaUtilities::ComponentsNeedRegistration(*components);
}
time_t Unigine::LuaSystem::getNewestLuaFileTime()
{
    return LuaUtilities::getNewestLuaFileTime();
}
time_t Unigine::LuaSystem::getNewestGeneratedFileTime()
{
    return LuaUtilities::getNewestGeneratedFileTime();
}
Unigine::String Unigine::LuaSystem::generateSimpleGUID(const Unigine::String& name)
{
    return LuaUtilities::generateSimpleGUID(name);
}
Unigine::String Unigine::LuaSystem::generateSimpleHash(const Unigine::String& name)
{
    return LuaUtilities::generateSimpleHash(name);
}

// ============================================
// File Operations - DELEGATED
// ============================================
void Unigine::LuaSystem::mountLuaPaths()
{
    if (!file_scanner) {
        Log::error("[LuaSystem] Cannot mount paths: file_scanner is null\n");
        return;
    }
    file_scanner->mountLuaPaths();
}
void Unigine::LuaSystem::LuaScanDirectory(const char* path)
{
    if (!file_scanner || !lua) {
        Log::error("[LuaSystem] Cannot scan directory: file_scanner or lua state is null\n");
        return;
    }
    file_scanner->LuaScanDirectory(path, lua);
}
void Unigine::LuaSystem::checkForHotReload(float dt)
{
    if (!post_initialized)  // Don't check until fully initialized
        return;

    static float time_since_check = 0.0f;
    time_since_check += dt;

    if (time_since_check < lua_check_interval)
        return;

    time_since_check = 0.0f;

    try {
        time_t newest_lua_time = getNewestLuaFileTime();

        if (newest_lua_time > last_lua_check && last_lua_check != 0)
        {
            if (log_init)
                Log::message("[LuaSystem] Hot reload: file changed\n");
            reloadAllLuaScripts();
        }

        last_lua_check = newest_lua_time;
    }
    catch (const std::exception& e) {
        Log::warning("[LuaSystem] Hot-reload check error: %s\n", e.what());
    }
    catch (...) {
        Log::warning("[LuaSystem] Hot-reload check error: Unknown exception\n");
    }
}
void Unigine::LuaSystem::reloadAllLuaScripts()
{
    if (log_init)
        Log::message("[LuaSystem] Reloading scripts...\n");

    struct SavedState {
        String component_name;
        sol::table saved_instance;
    };
    Vector <SavedState> saved_state;

    for (int i = 0; i < active_components->size(); i++) {
        LuaComponentInstance& ci = (*active_components)[i];

        if (!lua) {
            Log::error("[LuaSystem] Cannot reload: Lua state is null\n");
            return;
        }
        
        // deep copy of the instance
        sol::table copy = lua->create_table();

        // copy all fields - backup
        ci.instance.for_each([&copy](const sol::object& key, const sol::object& value) {
            copy[key] = value;
        });

        SavedState state;
        state.component_name = ci.descriptor.name;
        state.saved_instance = copy;
        saved_state.append(state);
    }

    // clear event listeners before reload
    LuaEventSystem::Clear();

    // -- second half of script
    // Step 1: Clear old component definitions
    components->clear();

    // Step 2: Reload all Lua files (this re-registers components)
    LuaScanDirectory("lua_components");

    // Step 3: Re-bind all active component instances to new functions
    if (!lua) {
        Log::error("[LuaSystem] Cannot reload: Lua state is null\n");
        return;
    }
    
    for (int i = 0; i < active_components->size(); i++)
    {
        LuaComponentInstance& ci = (*active_components)[i];

        // Find the updated descriptor
        HashMap<String, LuaComponentDescriptor>::Iterator it = components->find(ci.descriptor.name);
        if (it == components->end()) {
            if (log_registration)
                Log::warning("[LuaSystem] Component '%s' no longer exists\n", ci.descriptor.name.get());
            continue;
        }

        LuaComponentDescriptor& updated_desc = it->data;

        // Re-cache all lifecycle function references from the fresh Lua state
        if (!updated_desc.fn_on_init.empty())
            ci.on_init = (*lua)[updated_desc.fn_on_init.get()];
        else
            ci.on_init = sol::nil;

        if (!updated_desc.fn_on_update.empty())
            ci.on_update = (*lua)[updated_desc.fn_on_update.get()];
        else
            ci.on_update = sol::nil;

        if (!updated_desc.fn_on_shutdown.empty())
            ci.on_shutdown = (*lua)[updated_desc.fn_on_shutdown.get()];
        else
            ci.on_shutdown = sol::nil;

        if (!updated_desc.fn_on_update_async_thread.empty())
            ci.on_update_async_thread = (*lua)[updated_desc.fn_on_update_async_thread.get()];
        else
            ci.on_update_async_thread = sol::nil;

        if (!updated_desc.fn_on_update_sync_thread.empty())
            ci.on_update_sync_thread = (*lua)[updated_desc.fn_on_update_sync_thread.get()];
        else
            ci.on_update_sync_thread = sol::nil;

        if (!updated_desc.fn_on_post_update.empty())
            ci.on_post_update = (*lua)[updated_desc.fn_on_post_update.get()];
        else
            ci.on_post_update = sol::nil;

        if (!updated_desc.fn_on_update_physics.empty())
            ci.on_update_physics = (*lua)[updated_desc.fn_on_update_physics.get()];
        else
            ci.on_update_physics = sol::nil;

        if (!updated_desc.fn_on_swap.empty())
            ci.on_swap = (*lua)[updated_desc.fn_on_swap.get()];
        else
            ci.on_swap = sol::nil;

        if (log_registration)
            Log::message("[LuaSystem] Rebound: %s\n", ci.descriptor.name.get());
    }

    if (log_init)
        Log::message("[LuaSystem] Reload complete (%d instances)\n", active_components->size());
}

void Unigine::LuaSystem::loadLogSettings()
{
    // Set defaults first
    log_init = true;
    log_scan = true;
    log_load = false;  // Too verbose by default
    log_parse = false;  // Too verbose by default
    log_registration = true;
    log_generation = true;
    log_properties = false;  // Only show when creating/updating
    log_pending = true;
    
    // Try to load from config
    if (Config::isInitialized())
    {
        Config::setPath("configs/lua_system.config");
        if (Config::load())
        {
            log_init = Config::getBool("lua_log_init", log_init);
            log_scan = Config::getBool("lua_log_scan", log_scan);
            log_load = Config::getBool("lua_log_load", log_load);
            log_parse = Config::getBool("lua_log_parse", log_parse);
            log_registration = Config::getBool("lua_log_registration", log_registration);
            log_generation = Config::getBool("lua_log_generation", log_generation);
            log_properties = Config::getBool("lua_log_properties", log_properties);
            log_pending = Config::getBool("lua_log_pending", log_pending);
        }
    }
}

void Unigine::LuaSystem::saveLogSettings()
{
    if (!Config::isInitialized())
        return;
    
    Config::setPath("configs/lua_system.config");
    Config::setBool("lua_log_init", log_init);
    Config::setBool("lua_log_scan", log_scan);
    Config::setBool("lua_log_load", log_load);
    Config::setBool("lua_log_parse", log_parse);
    Config::setBool("lua_log_registration", log_registration);
    Config::setBool("lua_log_generation", log_generation);
    Config::setBool("lua_log_properties", log_properties);
    Config::setBool("lua_log_pending", log_pending);
    Config::save();
}

Unigine::Vector<LuaComponentInstance*> Unigine::LuaSystem::findComponentsByType(const Unigine::String& type)
{
    if (!active_components)
        return Unigine::Vector<LuaComponentInstance*>();
    return LuaComponentQueries::findByType(type, *active_components);
}

Unigine::Vector<LuaComponentInstance*> Unigine::LuaSystem::findComponentsByTag(const Unigine::String& tag)
{
    if (!active_components)
        return Unigine::Vector<LuaComponentInstance*>();
    return LuaComponentQueries::findByTag(tag, *active_components);
}

LuaComponentInstance* Unigine::LuaSystem::findComponentOnNode(Unigine::NodePtr node, const Unigine::String& type)
{
    if (!active_components || !node)
        return nullptr;
    return LuaComponentQueries::findByNode(node, type, *active_components);
}

void Unigine::LuaSystem::setDefaultValueXml(const Unigine::XmlPtr& param_node, const LuaPropertyDef& def)
{
    if (!param_node || def.sdefault.empty())
        return;
    
    // Set default value as an XML argument
    param_node->setArg("value", def.sdefault.get());
}
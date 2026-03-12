#pragma once
#ifndef LUA_SYSTEM_H
#define LUA_SYSTEM_H

#include "LuaSystemForward.h"
#include "LuaSystemTypes.h"
#include <memory>
#include <ctime>  // for time_t

// Forward declarations for helper classes
namespace Unigine {
    class LuaPropertyManager;
    class LuaComponentLifecycle;
    class LuaFileScanner;
    class LuaComponentPool;
}

namespace Unigine {
    class LuaSystem
    {
    public:
        // ------------------- Public Methods -------------------
        static LuaSystem* get();
        void initialize();
        void postInitialize();
        void update(float ifps);
        void shutdown();

        void mountLuaPaths();
        void LuaScanDirectory(const char* path);

        void registerLuaComponent(const std::string& name, const sol::table& definition);
        void removeLuaComponent(const Unigine::NodePtr& node, const Unigine::String& component_name);

        bool registerLuaComponentWithEngine(const LuaComponentDescriptor& d);
        bool registerLuaPropertyWithEngine(const LuaComponentDescriptor& d);
        void attachLuaInstance(Unigine::NodePtr node, const LuaComponentDescriptor& desc, Unigine::PropertyPtr prop_instance);

        void registerComponentInstance(Unigine::NodePtr node, const LuaComponentDescriptor& desc, sol::table instance);
        void unregisterComponentInstance(Unigine::NodePtr node, const Unigine::String& component_name);
        bool isShuttingDown() const noexcept;

        LuaComponentDescriptor* getComponentDescriptor(const Unigine::String& name);

        bool bindPropertyParameters(const LuaComponentDescriptor& d, const Unigine::PropertyPtr& prop);
        int luaTypeToPropertyType(const Unigine::String& lua_type);
        void setDefaultValue(const Unigine::PropertyParameterPtr& param, const LuaPropertyDef& def);
        void setDefaultValueXml(const Unigine::XmlPtr& param_node, const LuaPropertyDef& def);
        void createPropertyFilesImmediately();

        Unigine::String generateSimpleGUID(const Unigine::String& name);
        Unigine::String generateSimpleHash(const Unigine::String& name);

        sol::state& getState() noexcept;
        
        // Log level getters (for file scanner, property manager, and API bindings)
        bool getLogInit() const { return log_init; }
        bool getLogScan() const { return log_scan; }
        bool getLogLoad() const { return log_load; }
        bool getLogProperties() const { return log_properties; }
        bool getLogGeneration() const { return log_generation; }
        
        // Autocomplete generation
        void generateAutocompleteFiles(const Unigine::String& output_dir);
        
        // Debugger control
        void enableDebugger(int port = 8172);
        void disableDebugger();
        
        // Component queries
        Unigine::Vector<LuaComponentInstance*> findComponentsByType(const Unigine::String& type);
        Unigine::Vector<LuaComponentInstance*> findComponentsByTag(const Unigine::String& tag);
        LuaComponentInstance* findComponentOnNode(Unigine::NodePtr node, const Unigine::String& type);

        // hot reload
        void checkForHotReload(float dt);
        void reloadAllLuaScripts();

    private:
        // Console command handlers
        void registerConsoleCommands();
        void consoleGenerateAutocomplete(int argc, char** argv);
        void consoleEnableDebugger(int argc, char** argv);
        void consoleDisableDebugger(int argc, char** argv);
        void consoleProfilerReset(int argc, char** argv);
        void consoleProfilerStats(int argc, char** argv);
        void consoleReloadScripts(int argc, char** argv);
        void consoleListComponents(int argc, char** argv);
        
        // Log control commands
        void consoleLogInit(int argc, char** argv);
        void consoleLogScan(int argc, char** argv);
        void consoleLogLoad(int argc, char** argv);
        void consoleLogParse(int argc, char** argv);
        void consoleLogRegistration(int argc, char** argv);
        void consoleLogGeneration(int argc, char** argv);
        void consoleLogProperties(int argc, char** argv);
        void consoleLogPending(int argc, char** argv);
        void consoleLogAll(int argc, char** argv);
        void consoleLogStatus(int argc, char** argv) const;
        
        // Log settings persistence
        void loadLogSettings();
        void saveLogSettings();
        LuaSystem();
        ~LuaSystem();

        LuaSystem(const LuaSystem&) = delete;
        LuaSystem& operator=(const LuaSystem&) = delete;

        void parseProperties(const sol::table& props, Unigine::Vector<LuaPropertyDef>& out);
        void syncLuaComponents();
        bool ComponentsNeedRegistration();
        void processPendingComponents();
        void loadAdditionalComponentFiles(const LuaComponentDescriptor& desc);

        time_t getNewestLuaFileTime();
        time_t getNewestGeneratedFileTime();

        // Implementation details - using pointers to reduce header dependencies
        sol::state* lua;

        Unigine::Vector<LuaComponentInstance>* active_components;
        Unigine::HashMap<Unigine::String, LuaComponentDescriptor>* components;
        std::unordered_map<std::string, Unigine::Vector<LuaPropertyDef>>* bindings;

        bool initialized;
        bool post_initialized;
        bool property_files_created;
        bool is_shutting_down;

        time_t last_lua_check;
        float lua_check_interval;

        Unigine::Vector<LuaComponentDescriptor> pending_components;
        
        // Log level controls
        bool log_init;
        bool log_scan;
        bool log_load;
        bool log_parse;
        bool log_registration;
        bool log_generation;
        bool log_properties;
        bool log_pending;

        // Helper classes - using smart pointers for automatic cleanup
        std::unique_ptr<LuaPropertyManager> property_manager;
        std::unique_ptr<LuaComponentLifecycle> lifecycle_manager;
        std::unique_ptr<LuaFileScanner> file_scanner;
        std::unique_ptr<LuaComponentPool> component_pool;
    };
}

#endif // LUA_SYSTEM_H

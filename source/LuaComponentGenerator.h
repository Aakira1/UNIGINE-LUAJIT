#pragma once
#include "LuaSystem.h"
#include <fstream>
#include <filesystem>

namespace Unigine
{
    class LuaComponentGenerator
    {
    public:
        // Generate C++ components for all Lua components
        static void generateAllComponents(const HashMap<String, LuaComponentDescriptor>& components)
        {
            LuaSystem* ls = LuaSystem::get();
            bool should_log = ls ? ls->getLogGeneration() : true;
            
            std::filesystem::path current_path = std::filesystem::current_path();
            std::filesystem::path project_root = current_path;
            if (project_root.filename() == "bin")
            {
                project_root = project_root.parent_path();
            }

            std::filesystem::path source_path = project_root / "source" / "lua_components_generated";
            String source_dir = String(source_path.string().c_str()) + "/";

            cleanGeneratedFiles(source_dir);

            int count = 0;
            for (HashMap<String, LuaComponentDescriptor>::ConstIterator it = components.begin(); it != components.end(); ++it)
            {
                const LuaComponentDescriptor& desc = it->data;
                generateComponent(desc, source_dir, should_log);
                count++;
            }

            generateRegistrationFile(components, source_dir, should_log);
            
            if (should_log)
                Log::message("[Generator] Generated %d component(s)\n", count);
        }

    private:
        static void cleanGeneratedFiles(const String& dir)
        {
            std::filesystem::path path(dir.get());
            if (std::filesystem::exists(path))
            {
                std::filesystem::remove_all(path);
            }
            std::filesystem::create_directories(path);
        }
        static void generateComponent(const LuaComponentDescriptor& desc, const String& dir, bool log = true)
        {
            String class_name = String::format("Lua_%s", desc.name.get());
            generateHeaderFile(desc, class_name, dir);
            generateCppFile(desc, class_name, dir);
            if (log)
                Log::message("[Generator] Generated: %s\n", class_name.get());
        }

        
        // Generate Header File
        static void generateHeaderFile(const LuaComponentDescriptor& desc,
            const String& class_name,
            const String& dir)
        {
            String file_path = String::format("%s%s.h", dir.get(), class_name.get());
            std::filesystem::path path(file_path.get());
            std::filesystem::create_directories(path.parent_path());

            std::ofstream file(file_path.get());

            file << "#pragma once\n";
            file << "#include \"CommonIncludes.h\"\n";
            file << "#include <UnigineComponentSystem.h>\n";
            file << "#include <UnigineInterface.h>\n";
            file << "#include \"../LuaSystem.h\"\n\n";

            file << "// Auto-generated wrapper for Lua component: " << desc.name.get() << "\n";
            file << "class " << class_name.get() << " : public Unigine::ComponentBase\n{\n";
            file << "public:\n";
            file << "    COMPONENT_DEFINE(" << class_name.get() << ", Unigine::ComponentBase);\n";

            if (!desc.description.empty())
                file << "    COMPONENT_DESCRIPTION(\"" << escapeString(desc.description.get()) << "\");\n";

            file << "\n";

            // Generate properties
            for (const LuaPropertyDef& prop : desc.properties)
            {
                String unigine_type = luaTypeToUnigineComponentType(prop.type);
                String default_val = getDefaultValueForType(prop);

                file << "    PROP_PARAM(" << unigine_type.get() << ", " << prop.name.get();

                if (!default_val.empty())
                    file << ", " << default_val.get();

                if (!prop.title.empty())
                    file << ", \"" << escapeString(prop.title.get()) << "\"";
                if (!prop.tooltip.empty())
                    file << ", \"" << escapeString(prop.tooltip.get()) << "\"";
                if (!prop.group.empty())
                    file << ", \"" << escapeString(prop.group.get()) << "\"";

                file << ");\n";
            }

            file << "\n";

            // Register all lifecycle methods that are defined
            if (!desc.fn_on_init.empty())
                file << "    COMPONENT_INIT(init);\n";
            if (!desc.fn_on_update_async_thread.empty())
                file << "    COMPONENT_UPDATE_ASYNC_THREAD(updateAsyncThread);\n";
            if (!desc.fn_on_update_sync_thread.empty())
                file << "    COMPONENT_UPDATE_SYNC_THREAD(updateSyncThread);\n";
            if (!desc.fn_on_update.empty())
                file << "    COMPONENT_UPDATE(update);\n";
            if (!desc.fn_on_post_update.empty())
                file << "    COMPONENT_POST_UPDATE(postUpdate);\n";
            if (!desc.fn_on_update_physics.empty())
                file << "    COMPONENT_UPDATE_PHYSICS(updatePhysics);\n";
            if (!desc.fn_on_swap.empty())
                file << "    COMPONENT_SWAP(swap);\n";
            if (!desc.fn_on_shutdown.empty())
                file << "    COMPONENT_SHUTDOWN(shutdown);\n";

            file << "\n";
            file << "protected:\n";

            // Declare all lifecycle methods that are defined
            if (!desc.fn_on_init.empty())
                file << "    void init();\n";
            if (!desc.fn_on_update_async_thread.empty())
                file << "    void updateAsyncThread();\n";
            if (!desc.fn_on_update_sync_thread.empty())
                file << "    void updateSyncThread();\n";
            if (!desc.fn_on_update.empty())
                file << "    void update();\n";
            if (!desc.fn_on_post_update.empty())
                file << "    void postUpdate();\n";
            if (!desc.fn_on_update_physics.empty())
                file << "    void updatePhysics();\n";
            if (!desc.fn_on_swap.empty())
                file << "    void swap();\n";
            if (!desc.fn_on_shutdown.empty())
                file << "    void shutdown();\n";

            file << "\n";
            file << "private:\n";
            file << "    sol::table lua_instance;\n";
            file << "    bool lua_ready = false;\n";
            file << "};\n";

            file.close();
        }

        // Generate Cpp File
        static void generateCppFile(const LuaComponentDescriptor& desc,
            const String& class_name,
            const String& dir)
        {
            String file_path = String::format("%s%s.cpp", dir.get(), class_name.get());
            std::filesystem::path path(file_path.get());
            std::filesystem::create_directories(path.parent_path());

            std::ofstream file(file_path.get());

            file << "#include \"CommonIncludes.h\"\n";
            file << "#include \"" << class_name.get() << ".h\"\n";
            file << "#include <UnigineComponentSystem.h>\n";
            file << "#include <UnigineGame.h>\n";
            file << "#include <UnigineVisualizer.h>\n";
            file << "#include <UnigineProfiler.h>\n";
            file << "#include <UnigineConsole.h>\n";
            file << "#include <UnigineInput.h>\n\n";
            file << "REGISTER_COMPONENT(" << class_name.get() << ");\n\n";
            file << "using namespace Unigine;\n\n";

            
            // init() method - PRESERVE ORIGINAL WORKING LOGIC
            if (!desc.fn_on_init.empty())
            {
                file << "void " << class_name.get() << "::init()\n{\n";
                file << "    LuaSystem* ls = LuaSystem::get();\n";
                file << "    bool should_log = ls ? ls->getLogInit() : false;\n";
                file << "    \n";
                file << "    if (should_log) {\n";
                file << "        Log::message(\"[" << class_name.get() << " C++] init() called\\n\");\n";
                file << "    }\n";
                file << "    \n";
                file << "    LuaComponentDescriptor* desc = ls->getComponentDescriptor(\"" << desc.name.get() << "\");\n";
                file << "    if (!desc) {\n";
                file << "        Log::error(\"[" << class_name.get() << "] Component descriptor not found\\n\");\n";
                file << "        return;\n";
                file << "    }\n\n";

                file << "    lua_instance = ls->getState().create_table();\n";
				file << "    // Pass the node to Lua instance\n";
                file << "    lua_instance[\"node\"] = getNode();\n\n";

                file << "    // Sync properties to Lua\n";
                for (const LuaPropertyDef& prop : desc.properties)
                {
                    file << "    lua_instance[\"" << prop.name.get() << "\"] = ";

                    if (prop.type == "vec3")
                    {
                        file << "std::make_tuple(" << prop.name.get() << ".get().x, "
                            << prop.name.get() << ".get().y, " << prop.name.get() << ".get().z);\n";
                    }
                    else if (prop.type == "vec4" || prop.type == "color")
                    {
                        file << "std::make_tuple(" << prop.name.get() << ".get().x, "
                            << prop.name.get() << ".get().y, " << prop.name.get() << ".get().z, "
                            << prop.name.get() << ".get().w);\n";
                    }
                    else if (prop.type == "vec2")
                    {
                        file << "std::make_tuple(" << prop.name.get() << ".get().x, "
                            << prop.name.get() << ".get().y);\n";
                    }
                    else if (prop.type == "string")
                    {
                        file << prop.name.get() << ".get();\n";
                    }
                    else
                    {
                        file << prop.name.get() << ".get();\n";
                    }
                }

                file << "\n";
                file << "    // Register this instance with LuaSystem\n";
                file << "    ls->registerComponentInstance(getNode(), *desc, lua_instance);\n\n";

                // CRITICAL: Call Lua init - THIS IS FROM THE ORIGINAL!
                file << "    // Call Lua init\n";
                file << "    sol::function init_fn = ls->getState()[\"" << desc.fn_on_init.get() << "\"];\n";
                file << "    if (init_fn.valid())\n";
                file << "    {\n";
                file << "        try {\n";
                file << "            if (should_log) {\n";
                file << "                Log::message(\"[" << class_name.get() << "] Calling Lua init...\\n\");\n";
                file << "            }\n";
                file << "            init_fn(lua_instance);\n";
                file << "            lua_ready = true;\n";
                file << "            if (should_log) {\n";
                file << "                Log::message(\"[" << class_name.get() << "] Lua init completed\\n\");\n";
                file << "            }\n";
                file << "        }\n";
                file << "        catch (const sol::error & e) {\n";
                file << "            Log::error(\"[" << class_name.get() << "] Lua init error: %s\\n\", e.what());\n";
                file << "        }\n";
                file << "    }\n";
                file << "    else\n";
                file << "    {\n";
                file << "        lua_ready = true;\n";
                file << "    }\n";
                file << "}\n\n";
            }

            // updateAsyncThread() method - NEW
            if (!desc.fn_on_update_async_thread.empty())
            {
                generateLifecycleMethod(file, desc, class_name, "updateAsyncThread", desc.fn_on_update_async_thread.get(), true);
            }

            // updateSyncThread() method - NEW
            if (!desc.fn_on_update_sync_thread.empty())
            {
                generateLifecycleMethod(file, desc, class_name, "updateSyncThread", desc.fn_on_update_sync_thread.get(), true);
            }

            // update() method - PRESERVE ORIGINAL
            if (!desc.fn_on_update.empty())
            {
                file << "void " << class_name.get() << "::update()\n{\n";
                file << "    static int update_count = 0;\n";
                file << "    update_count++;\n";
                // add debugger here
                //file << "    if (update_count % 60 == 0) {\n";
                //file << "        Log::message(\"[" << class_name.get() << " C++] update() frame %d, lua_ready=%d\\n\", update_count, lua_ready ? 1 : 0);\n";
                //file << "    }\n";
                file << "    \n";
                file << "    if (!lua_ready) return;\n\n";

                // FIXED: Only declare ls if we're going to use it
                file << "    LuaSystem* ls = LuaSystem::get();\n\n";

                file << "    // Sync properties to Lua\n";
                for (const LuaPropertyDef& prop : desc.properties)
                {
                    file << "    lua_instance[\"" << prop.name.get() << "\"] = ";

                    if (prop.type == "vec3")
                    {
                        file << "std::make_tuple(" << prop.name.get() << ".get().x, "
                            << prop.name.get() << ".get().y, " << prop.name.get() << ".get().z);\n";
                    }
                    else if (prop.type == "vec4" || prop.type == "color")
                    {
                        file << "std::make_tuple(" << prop.name.get() << ".get().x, "
                            << prop.name.get() << ".get().y, " << prop.name.get() << ".get().z, "
                            << prop.name.get() << ".get().w);\n";
                    }
                    else if (prop.type == "vec2")
                    {
                        file << "std::make_tuple(" << prop.name.get() << ".get().x, "
                            << prop.name.get() << ".get().y);\n";
                    }
                    else if (prop.type == "string")
                    {
                        file << prop.name.get() << ".get();\n";
                    }
                    else
                    {
                        file << prop.name.get() << ".get();\n";
                    }
                }

                file << "\n";
                file << "    // Call Lua update\n";
                file << "    sol::function update_fn = ls->getState()[\"" << desc.fn_on_update.get() << "\"];\n";
                file << "    if (update_fn.valid())\n";
                file << "    {\n";
                file << "        try {\n";
                file << "            update_fn(lua_instance, Game::getIFps());\n";
                file << "        }\n";
                file << "        catch (const sol::error & e) {\n";
                file << "            Log::error(\"[" << class_name.get() << "] Lua update error: %s\\n\", e.what());\n";
                file << "        }\n";
                file << "    }\n";
                file << "\n";
                file << "    // Sync properties back from Lua to C++ (bidirectional sync)\n";
                for (const LuaPropertyDef& prop : desc.properties)
                {
                    file << "    if (lua_instance[\"" << prop.name.get() << "\"].valid())\n";
                    file << "    {\n";
                    file << "        sol::object lua_prop = lua_instance[\"" << prop.name.get() << "\"];\n";
                    
                    if (prop.type == "int")
                    {
                        file << "        if (lua_prop.is<int>())\n";
                        file << "            " << prop.name.get() << " = lua_prop.as<int>();\n";
                    }
                    else if (prop.type == "float")
                    {
                        file << "        if (lua_prop.is<float>())\n";
                        file << "            " << prop.name.get() << " = lua_prop.as<float>();\n";
                        file << "        else if (lua_prop.is<double>())\n";
                        file << "            " << prop.name.get() << " = static_cast<float>(lua_prop.as<double>());\n";
                    }
                    else if (prop.type == "bool" || prop.type == "toggle")
                    {
                        file << "        if (lua_prop.is<bool>())\n";
                        file << "            " << prop.name.get() << " = lua_prop.as<bool>();\n";
                    }
                    else if (prop.type == "string")
                    {
                        file << "        if (lua_prop.is<std::string>())\n";
                        file << "            " << prop.name.get() << " = lua_prop.as<std::string>().c_str();\n";
                    }
                    else if (prop.type == "vec2")
                    {
                        file << "        if (lua_prop.is<std::tuple<float, float>>())\n";
                        file << "        {\n";
                        file << "            auto t = lua_prop.as<std::tuple<float, float>>();\n";
                        file << "            " << prop.name.get() << " = Unigine::Math::vec2(std::get<0>(t), std::get<1>(t));\n";
                        file << "        }\n";
                        file << "        else if (lua_prop.is<sol::table>())\n";
                        file << "        {\n";
                        file << "            sol::table t = lua_prop.as<sol::table>();\n";
                        file << "            float x = t.get_or(1, 0.0f);\n";
                        file << "            float y = t.get_or(2, 0.0f);\n";
                        file << "            " << prop.name.get() << " = Unigine::Math::vec2(x, y);\n";
                        file << "        }\n";
                    }
                    else if (prop.type == "vec3")
                    {
                        file << "        if (lua_prop.is<std::tuple<float, float, float>>())\n";
                        file << "        {\n";
                        file << "            auto t = lua_prop.as<std::tuple<float, float, float>>();\n";
                        file << "            " << prop.name.get() << " = Unigine::Math::vec3(std::get<0>(t), std::get<1>(t), std::get<2>(t));\n";
                        file << "        }\n";
                        file << "        else if (lua_prop.is<sol::table>())\n";
                        file << "        {\n";
                        file << "            sol::table t = lua_prop.as<sol::table>();\n";
                        file << "            float x = t.get_or(1, 0.0f);\n";
                        file << "            float y = t.get_or(2, 0.0f);\n";
                        file << "            float z = t.get_or(3, 0.0f);\n";
                        file << "            " << prop.name.get() << " = Unigine::Math::vec3(x, y, z);\n";
                        file << "        }\n";
                    }
                    else if (prop.type == "vec4" || prop.type == "color")
                    {
                        file << "        if (lua_prop.is<std::tuple<float, float, float, float>>())\n";
                        file << "        {\n";
                        file << "            auto t = lua_prop.as<std::tuple<float, float, float, float>>();\n";
                        file << "            " << prop.name.get() << " = Unigine::Math::vec4(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t));\n";
                        file << "        }\n";
                        file << "        else if (lua_prop.is<sol::table>())\n";
                        file << "        {\n";
                        file << "            sol::table t = lua_prop.as<sol::table>();\n";
                        file << "            float x = t.get_or(1, 0.0f);\n";
                        file << "            float y = t.get_or(2, 0.0f);\n";
                        file << "            float z = t.get_or(3, 0.0f);\n";
                        file << "            float w = t.get_or(4, 0.0f);\n";
                        file << "            " << prop.name.get() << " = Unigine::Math::vec4(x, y, z, w);\n";
                        file << "        }\n";
                    }
                    else if (prop.type == "file")
                    {
                        file << "        if (lua_prop.is<std::string>())\n";
                        file << "            " << prop.name.get() << " = lua_prop.as<std::string>().c_str();\n";
                    }
                    
                    file << "    }\n";
                }
                file << "}\n\n";
            }

            
            // postUpdate() method - NEW
            if (!desc.fn_on_post_update.empty())
            {
                generateLifecycleMethod(file, desc, class_name, "postUpdate", desc.fn_on_post_update.get(), true);
            }

            
            // updatePhysics() method - NEW
            if (!desc.fn_on_update_physics.empty())
            {
                generateLifecycleMethod(file, desc, class_name, "updatePhysics", desc.fn_on_update_physics.get(), true);
            }

            
            // swap() method - NEW
            if (!desc.fn_on_swap.empty())
            {
                generateLifecycleMethod(file, desc, class_name, "swap", desc.fn_on_swap.get(), false);
            }

            
            // shutdown() method - PRESERVE ORIGINAL
            if (!desc.fn_on_shutdown.empty())
            {
                file << "void " << class_name.get() << "::shutdown()\n{\n";
                file << "    Log::message(\"[" << class_name.get() << " C++] shutdown() CALLED!\\n\");\n";
                file << "    if (!lua_ready) return;\n\n";
                file << "    LuaSystem* ls = LuaSystem::get();\n";
                file << "    if (!ls) return;\n\n";

                file << "    try {\n";
                file << "        sol::function shutdown_fn = ls->getState()[\"" << desc.fn_on_shutdown.get() << "\"];\n";
                file << "        if (shutdown_fn.valid())\n";
                file << "        {\n";
                file << "            shutdown_fn(lua_instance);\n";
                file << "        }\n";
                file << "    }\n";
                file << "    catch (...) {\n";
                file << "        // Lua state may be destroyed during global shutdown\n";
                file << "    }\n";

                file << "\n    // Unregister from LuaSystem\n";
                file << "    try {\n";
                file << "        ls->unregisterComponentInstance(getNode(), \"" << desc.name.get() << "\");\n";
                file << "    }\n";
                file << "    catch (...) {\n";
                file << "        // Ignore errors during shutdown\n";
                file << "    }\n";
                file << "}\n";
            }

            file.close();
        }


        // Helper for generating new lifecycle methods
        static void generateLifecycleMethod(std::ofstream& file, [[maybe_unused]] const LuaComponentDescriptor& desc,
            const String& class_name, const char* method_name, const char* lua_fn_name, bool pass_dt)
        {
            file << "void " << class_name.get() << "::" << method_name << "()\n{\n";
            file << "    if (!lua_ready) return;\n\n";
            file << "    LuaSystem* ls = LuaSystem::get();\n\n";

            file << "    // Call Lua " << method_name << "\n";
            file << "    sol::function fn = ls->getState()[\"" << lua_fn_name << "\"];\n";
            file << "    if (fn.valid())\n";
            file << "    {\n";
            file << "        try {\n";
            if (pass_dt)
                file << "            fn(lua_instance, Game::getIFps());\n";
            else
                file << "            fn(lua_instance);\n";
            file << "        }\n";
            file << "        catch (const sol::error & e) {\n";
            file << "            Log::error(\"[" << class_name.get() << "] Lua " << method_name << " error: %s\\n\", e.what());\n";
            file << "        }\n";
            file << "    }\n";
            file << "}\n\n";
        }

        static void generateRegistrationFile(const HashMap<String, LuaComponentDescriptor>& components,
            const String& dir, [[maybe_unused]] bool log = true)
        {
            String file_path = String::format("%s_register_all.h", dir.get());
            std::ofstream file(file_path.get());

            file << "#pragma once\n";
            file << "// Auto-generated: Include all Lua component wrappers\n";
            file << "// Include this file from your main source files\n\n";

            for (HashMap<String, LuaComponentDescriptor>::ConstIterator it = components.begin(); it != components.end(); ++it)
            {
                String class_name = String::format("Lua_%s", it->data.name.get());
                file << "#include \"" << class_name.get() << ".h\"\n";
            }

            file << "\n// All components are registered via REGISTER_COMPONENT() macro in their .cpp files\n";

            file.close();
        }

        static String luaTypeToUnigineComponentType(const String& lua_type)
        {
            if (lua_type == "int") return "Int";
            if (lua_type == "float") return "Float";
            if (lua_type == "double") return "Double";
            if (lua_type == "bool" || lua_type == "toggle") return "Toggle";
            if (lua_type == "string") return "String";
            if (lua_type == "vec2") return "Vec2";
            if (lua_type == "vec3") return "Vec3";
            if (lua_type == "vec4") return "Vec4";
            if (lua_type == "color") return "Color";
            if (lua_type == "switch") return "Switch";
            if (lua_type == "mask") return "Mask";
            if (lua_type == "file") return "File";
            if (lua_type == "node") return "Node";
            if (lua_type == "material") return "Material";
            if (lua_type == "property") return "Property";
            return "String";
        }

        static String getDefaultValueForType(const LuaPropertyDef& prop)
        {
            if (!prop.sdefault.empty())
            {
                // String properties need quotes around the default value
                if (prop.type == "string")
                {
                    Unigine::String result = "\"";
                    result += prop.sdefault;
                    result += "\"";
                    return result;
                }
                else if (prop.type == "file") 
                {
                    Unigine::String result = "\"";
                    result += prop.sdefault;
                    result += "\"";
					return result;
                }
                // FIXED: Handle vec2, vec3, vec4, color - convert "x y z w" to "vec4(x, y, z, w)"
                else if (prop.type == "vec2")
                {
                    String result = "Unigine::Math::vec2(";
                    String values = prop.sdefault;
                    for (int i = 0; i < values.size(); i++)
                    {
                        if (values[i] == ' ')
                            result += ", ";
                        else
                            result += values[i];
                    }
                    result += ")";
                    return result;
                }
                else if (prop.type == "vec3")
                {
                    String result = "Unigine::Math::vec3(";
                    String values = prop.sdefault;
                    for (int i = 0; i < values.size(); i++)
                    {
                        if (values[i] == ' ')
                            result += ", ";
                        else
                            result += values[i];
                    }
                    result += ")";
                    return result;
                }
                else if (prop.type == "vec4" || prop.type == "color")
                {
                    String result = "Unigine::Math::vec4(";
                    String values = prop.sdefault;
                    for (int i = 0; i < values.size(); i++)
                    {
                        if (values[i] == ' ')
                            result += ", ";
                        else
                            result += values[i];
                    }
                    result += ")";
                    return result;
                }
                return prop.sdefault;
            }

            if (prop.type == "int" || prop.type == "mask")
                return "0";
			if (prop.type == "bool" || prop.type == "toggle" || prop.type == "switch")
				return "false";
            if (prop.type == "float")
                return "0.0f";
            if (prop.type == "double")
                return "0.0";
            if (prop.type == "string")
                return "\"\"";
            if (prop.type == "file")
				return "\"\"";
            if (prop.type == "vec2")
                return "Unigine::Math::vec2(0, 0)";
            if (prop.type == "vec3")
                return "Unigine::Math::vec3(0, 0, 0)";
            if (prop.type == "vec4" || prop.type == "color")
                return "Unigine::Math::vec4(0, 0, 0, 0)";
            if (prop.type == "dmat4")
                return "Unigine::Math::dmat4(0.0);";

            return "";
        }

        static const char* escapeString(const char* str)
        {
            return str;
        }
    };
}
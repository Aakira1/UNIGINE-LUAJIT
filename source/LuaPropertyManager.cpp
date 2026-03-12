#include "CommonIncludes.h"
#include "LuaPropertyManager.h"
#include "LuaSystem.h"
#include <filesystem>

using namespace Unigine;
using namespace Unigine::Math;

int LuaPropertyManager::luaTypeToPropertyType(const String& lua_type)
{
    if (lua_type == "int") return Property::PARAMETER_INT;
    if (lua_type == "float") return Property::PARAMETER_FLOAT;
    if (lua_type == "double") return Property::PARAMETER_DOUBLE;
    if (lua_type == "bool" ||
        lua_type == "toggle") return Property::PARAMETER_TOGGLE;
    if (lua_type == "string") return Property::PARAMETER_STRING;
    if (lua_type == "vec2") return Property::PARAMETER_VEC2;
    if (lua_type == "vec3") return Property::PARAMETER_VEC3;
    if (lua_type == "vec4") return Property::PARAMETER_VEC4;
    if (lua_type == "color") return Property::PARAMETER_COLOR;
    if (lua_type == "switch") return Property::PARAMETER_SWITCH;
    if (lua_type == "mask") return Property::PARAMETER_MASK;
    if (lua_type == "file") return Property::PARAMETER_FILE;
    if (lua_type == "node") return Property::PARAMETER_NODE;
    if (lua_type == "material") return Property::PARAMETER_MATERIAL;
    if (lua_type == "property") return Property::PARAMETER_PROPERTY;
    if (lua_type == "quat") return Property::PARAMETER_VEC4; // Quat stored as vec4

    Log::warning("[LuaSystem] Unknown Lua type '%s', defaulting to string\n", lua_type.get());
    return Property::PARAMETER_STRING;
}
void LuaPropertyManager::setDefaultValue(const Unigine::PropertyParameterPtr& param, const LuaPropertyDef& def)
{
    if (def.sdefault.empty())
        return;

    if (def.type == "int")
    {
        param->setValueInt(atoi(def.sdefault.get()));
    }
    else if (def.type == "bool" || def.type == "toggle")
    {
        param->setValueToggle((bool)def.sdefault.get());
    }
    else if (def.type == "switch")
    {
        param->setValueSwitch(atoi(def.sdefault.get()));
    }
    else if (def.type == "mask")
    {
        param->setValueMask(atoi(def.sdefault.get()));
    }
    else if (def.type == "float")
    {
        param->setValueFloat((float)atof(def.sdefault.get()));
    }
    else if (def.type == "double")
    {
        param->setValueDouble(atof(def.sdefault.get()));
    }
    else if (def.type == "string")
    {
        param->setValueString(def.sdefault.get());
    }
    else if (def.type == "file")
    {
                param->setValueFile(def.sdefault.get());
    }
    else if (def.type == "vec2")
    {
        // Parse "x y" format
        float x = 0, y = 0;
        #pragma warning(push)
        #pragma warning(disable: 4996) // sscanf is safe here - we control the input format
        sscanf(def.sdefault.get(), "%f %f", &x, &y);
        #pragma warning(pop)
        param->setValueVec2(Math::vec2(x, y));
    }
    else if (def.type == "vec3")
    {
        // Parse "x y z" format
        float x = 0, y = 0, z = 0;
        #pragma warning(push)
        #pragma warning(disable: 4996) // sscanf is safe here - we control the input format
        sscanf(def.sdefault.get(), "%f %f %f", &x, &y, &z);
        #pragma warning(pop)
        param->setValueVec3(Math::vec3(x, y, z));
    }
    else if (def.type == "vec4" || def.type == "color")
    {
        // Parse "x y z w" format
        float x = 0, y = 0, z = 0, w = 0;
        #pragma warning(push)
        #pragma warning(disable: 4996) // sscanf is safe here - we control the input format
        sscanf(def.sdefault.get(), "%f %f %f %f", &x, &y, &z, &w);
        #pragma warning(pop)
        param->setValueVec4(Math::vec4(x, y, z, w));
    }
    else if (def.type == "quat")
    {
        // Parse "x y z w" format
        float x = 0, y = 0, z = 0, w = 0;
        #pragma warning(push)
        #pragma warning(disable: 4996) // sscanf is safe here - we control the input format
        sscanf(def.sdefault.get(), "%f %f %f %f", &x, &y, &z, &w);
        #pragma warning(pop)
        param->setValueVec4(Math::vec4(x, y, z, w)); // Store as vec4
    }
    else
    {
        Log::warning("[LuaSystem] Unknown property type '%s' for default value setting\n", def.type.get());
	}
}

void LuaPropertyManager::createPropertyFilesImmediately(Unigine::HashMap<Unigine::String, LuaComponentDescriptor>& components,
    bool& property_files_created)
{
    LuaSystem* ls = LuaSystem::get();
    bool should_log = ls ? ls->getLogProperties() : true;
    
    if (property_files_created) {
        if (should_log)
            Log::message("[LuaSystem] Property files already created\n");
        return;
    }
    if (components.empty()) {
        Log::error("[LuaSystem] No components registered\n");
        return;
    }

    // Find existing ComponentSystem folder by checking if we can resolve paths in it
    Unigine::String prop_dir;
    bool found = false;

    // Try common locations for ComponentSystem folder
    const char* possible_paths[] = {
        "ComponentSystem/",
        nullptr
    };

    for (int i = 0; possible_paths[i] != nullptr; i++)
    {
        // Check if this path resolves to an actual location
        Unigine::String test_path = possible_paths[i];
        Unigine::String absolute = FileSystem::getAbsolutePath(test_path.get());

        // If getAbsolutePath returns a non-empty result, the path exists in Unigine's filesystem
        if (!absolute.empty())
        {
            // Verify it's actually accessible by checking if parent exists
            try {
                std::filesystem::path fs_path(absolute.get());
                if (std::filesystem::exists(fs_path.parent_path()))
                {
                    prop_dir = test_path;
                    found = true;
                    if (should_log)
                        Log::message("[LuaSystem] Found ComponentSystem at: %s\n", absolute.get());
                    break;
                }
            }
            catch (...) {
                // Try next path
            }
        }
    }

    // If still not found, try creating in data/ComponentSystem/ and verify
    if (!found)
    {
        prop_dir = "data/ComponentSystem/";

        // Try to create the directory
        try {
            Unigine::String absolute = FileSystem::getAbsolutePath("data");
            if (!absolute.empty())
            {
                std::filesystem::path base_path(absolute.get());
                std::filesystem::path component_path = base_path / "ComponentSystem";

                if (std::filesystem::exists(component_path) ||
                    std::filesystem::create_directories(component_path))
                {
                    found = true;
                    if (should_log)
                        Log::message("[LuaSystem] Created ComponentSystem at: %s\n", component_path.string().c_str());
                }
            }
        }
        catch (...) {
            // Creation failed
        }
    }
    if (!found)
    {
        Log::error("[LuaSystem] [ERROR] ComponentSystem folder not found and could not be created!\n");
        Log::error("[LuaSystem] Please manually create: data/ComponentSystem/\n");
        return;
    }

    int created_count = 0;
    int skipped_count = 0;

    for (Unigine::HashMap<Unigine::String, LuaComponentDescriptor>::Iterator it = components.begin();
        it != components.end();
        ++it)
    {
        const LuaComponentDescriptor& desc = it->data;
        Unigine::String prop_name = Unigine::String::format("Lua_%s", desc.name.get());
        Unigine::String prop_file = Unigine::String::format("%s%s.prop", prop_dir.get(), prop_name.get());
        Unigine::String meta_file = Unigine::String::format("%s%s.prop.meta", prop_dir.get(), prop_name.get());

        // check if property file needs to be updated (Compare timestamps)
// Check if we need to update the property file
        bool needs_update = false;

        if (!FileSystem::isFileExist(prop_file.get()) ||
            !FileSystem::isFileExist(meta_file.get()))
        {
            needs_update = true;  // Files don't exist
        }
        else
        {
            // Load existing .prop file and compare parameters
            Unigine::XmlPtr existing_xml = Unigine::Xml::create();

            if (existing_xml && existing_xml->load(prop_file.get()))
            {
                int existing_param_count = existing_xml->getNumChildren();
                int new_param_count = desc.properties.size();

                if (existing_param_count != new_param_count)
                {
                    needs_update = true;
                    if (should_log)
                        Log::message("[LuaSystem] Update: %s (%d -> %d params)\n",
                            prop_name.get(), existing_param_count, new_param_count);
                }
                else
                {
                    // Check if parameter names or types changed // *Refactor
                    for (const LuaPropertyDef& prop : desc.properties)
                    {
                        bool param_found = false; // corrected variable name

                        for (int i = 0; i < existing_xml->getNumChildren(); i++)
                        {
                            Unigine::XmlPtr param = existing_xml->getChild(i);
                            if (param && param->getName() == Unigine::String("parameter"))
                            {
                                if (Unigine::String(param->getArg("name")) == prop.name)
                                {
                                    Unigine::String existing_type = param->getArg("type");
                                    Unigine::String new_type = Property::parameterNameByType(luaTypeToPropertyType(prop.type));

                                    if (existing_type != new_type)
                                    {
                                        needs_update = true;
                                        if (should_log)
                                            Log::message("[LuaSystem] Update: %s (type changed: %s)\n",
                                                prop_name.get(), prop.name.get());
                                        break;
                                    }
                                    param_found = true;
                                    break;
                                }
                            }
                        }
                        if (!param_found)
                        {
                            needs_update = true;
                            if (should_log)
                                Log::message("[LuaSystem] Update: %s (new property: %s)\n",
                                    prop_name.get(), prop.name.get());
                            break;
                        }
                    }
                }
            }
            else
            {
                needs_update = true;  // Couldn't load existing file
            }
        }

        if (!needs_update)
        {
            Log::message("[LuaSystem] [SKIP] %s (up to date)\n", prop_name.get());
            skipped_count++;
            continue;
        }

        Log::message("[LuaSystem] %s: %s\n",
            FileSystem::isFileExist(prop_file.get()) ? "[UPDATING]" : "[CREATING]",
            prop_name.get());

        // ============================================ end check

        Log::message("[LuaSystem] Creating: %s\n", prop_name.get());

        try {
            Unigine::XmlPtr xml = Unigine::Xml::create("property");
            if (!xml)
            {
                Log::error("[LuaSystem] Failed to create XML for %s\n", prop_name.get());
                continue;
            }

            xml->setArg("version", "2.20.0.0");
            xml->setArg("name", prop_name.get());
            xml->setArg("manual", "1");
            xml->setArg("parent_name", "node_base");

            // Add parameters
            int param_count = 0;
            for (const LuaPropertyDef& prop : desc.properties)
            {
                Unigine::XmlPtr param = xml->addChild("parameter");
                if (!param) continue;

                param->setArg("name", prop.name.get());
                param->setArg("type", Property::parameterNameByType(luaTypeToPropertyType(prop.type)));

                if (!prop.title.empty())
                    param->setArg("title", prop.title.get());
                if (!prop.tooltip.empty())
                    param->setArg("tooltip", prop.tooltip.get());
                if (!prop.group.empty())
                    param->setArg("group", prop.group.get());

                // Set default value
                if (!prop.sdefault.empty())
                {
                    param->setData(prop.sdefault.get());
                }
                else
                {
                    // Sensible defaults
                    if (prop.type == "int")
                        param->setData(0);
                    else if (prop.type == "toggle" || prop.type == "switch")
                        param->setData(false);
                    else if (prop.type == "mask")
                        param->setData(0);
                    else if (prop.type == "float")
                        param->setData(0.0f);
                    else if (prop.type == "double")
                        param->setData(0.0);
                    else if (prop.type == "vec2")
                        param->setData(vec2(0, 0));
                    else if (prop.type == "vec3")
                        param->setData(vec3(0, 0, 0));
                    else if (prop.type == "vec4" || prop.type == "color")
                        param->setData(vec4(0, 0, 0, 0));
                    else if (prop.type == "dvec2")
                        param->setData(dvec2(0, 0));
                    else if (prop.type == "dvec3")
                        param->setData(dvec3(0, 0, 0));
                    else if (prop.type == "dvec4")
                        param->setData(dvec4(0, 0, 0, 0));
                    else if (prop.type == "mat4")
                        param->setData(mat4(1.0f));
                    else if (prop.type == "dmat4")
                        param->setData(dmat4(1.0));
                    else if (prop.type == "quat")
                        param->setData(quat(0, 0, 0, 1));
                    else if (prop.type == "Vec4")
                        param->setData(vec4(0, 0, 0, 0));
                    else if (prop.type == "file")
                    {
						Unigine::XmlPtr param_node = xml->getChild("parameter");
						param_node->setArg("type", "file");
						param->setArg("name", prop.name.get());
						if (!prop.title.empty()) param_node->setData(prop.title.get());
						if (!prop.args.empty()) param_node->setArg("filter", prop.args.get());
						else param_node->setArg("filter", "*.*");

						if (!prop.sdefault.empty()) param_node->setArg("default", prop.sdefault.get());
                    }
                    else if (prop.type == "string")
						param->setData("");
                }

                param_count++;
            }

            if (!xml->save(prop_file.get()))
            {
                Log::error("[LuaSystem] Failed to save %s.prop\n", prop_name.get());
                continue;
            }

            if (should_log)
                Log::message("[LuaSystem] Created: %s.prop (%d params)\n",
                    prop_name.get(), param_count);

            // Generate GUID using simple hash
            unsigned int hash = 5381;
            const char* str = prop_name.get();
            for (int i = 0; str[i]; i++)
                hash = ((hash << 5) + hash) + str[i];

            // Create GUID string (40 hex characters)
            char guid_str[41];
            snprintf(guid_str, sizeof(guid_str),
                "%08x%08x%08x%08x%08x",
                hash, hash ^ 0x12345678, hash ^ 0x87654321,
                hash ^ 0xABCDEF00, hash ^ 0xFEDCBA00);

            // Create hash string (8 hex characters)
            char hash_str[9];
            snprintf(hash_str, sizeof(hash_str), "%08x", hash);

            Unigine::XmlPtr meta = Unigine::Xml::create("asset");
            if (!meta)
            {
                Log::error("[LuaSystem] [ERROR] Failed to create meta XML for %s\n", prop_name.get());
                continue;
            }

            meta->setArg("version", "2.20.0.0");

            Unigine::XmlPtr guid_node = meta->addChild("guid");
            if (guid_node) guid_node->setData(guid_str);

            Unigine::XmlPtr type_node = meta->addChild("type");
            if (type_node) type_node->setData("property");

            Unigine::XmlPtr hash_node = meta->addChild("hash");
            if (hash_node) hash_node->setData(hash_str);

            Unigine::XmlPtr runtimes = meta->addChild("runtimes");
            if (runtimes)
            {
                Unigine::XmlPtr runtime = runtimes->addChild("runtime");
                if (runtime)
                {
                    runtime->setArg("id", guid_str);
                    runtime->setArg("name", prop_name.get());
                    runtime->setArg("link", "0");
                    runtime->setArg("type", "4");
                }
            }

            // Save .prop.meta file
            if (!meta->save(meta_file.get()))
            {
                Log::error("[LuaSystem] [ERROR] Failed to save %s_prop.meta\n", prop_name.get());
                continue;
            }

            Log::message("[LuaSystem] [OK] %s_prop.meta\n", prop_name.get());

            created_count++;

        }
        catch (const std::exception& e) {
            Log::error("[LuaSystem] [ERROR] Exception creating %s: %s\n", prop_name.get(), e.what());
            continue;
        }
        catch (...) {
            Log::error("[LuaSystem] [ERROR] Unknown exception creating %s\n", prop_name.get());
            continue;
        }
    }

    if (should_log)
        Log::message("[LuaSystem] Created %d, skipped %d\n", created_count, skipped_count);

    property_files_created = true;

    // Try to refresh properties (optional, don't crash if fails)
    // NOTE: This might fail if ComponentSystem isn't fully initialized yet
    try {
        ComponentSystem* cs = ComponentSystem::get();
        if (cs)
        {
            if (should_log)
                Log::message("[LuaSystem] Attempting to refresh properties...\n");
            int refreshed = 0;
            int failed = 0;

            for (Unigine::HashMap<Unigine::String, LuaComponentDescriptor>::Iterator it2 = components.begin();
                it2 != components.end();
                ++it2)
            {
                Unigine::String prop_name = Unigine::String::format("Lua_%s", it2->data.name.get());

                try {
                    // Check if property exists first
                    PropertyPtr prop = Properties::findManualProperty(prop_name.get());
                    if (prop)
                    {
                        cs->refreshProperty(prop_name.get());
                        refreshed++;
                        if (should_log)
                            Log::message("[LuaSystem] Refreshed: %s\n", prop_name.get());
                    }
                    else
                    {
                        failed++;
                        if (should_log)
                            Log::message("[LuaSystem] Not found: %s (will load on restart)\n", prop_name.get());
                    }
                }
                catch (const std::exception& e) {
                    failed++;
                    Log::warning("[LuaSystem]   [ERROR] Failed to refresh %s: %s\n", prop_name.get(), e.what());
                }
                catch (...) {
                    failed++;
                    Log::warning("[LuaSystem]   [ERROR] Failed to refresh %s: Unknown error\n", prop_name.get());
                }
            }

            if (should_log)
            {
                if (refreshed > 0)
                    Log::message("[LuaSystem] Refreshed %d properties (%d failed/skipped)\n", refreshed, failed);
                else
                    Log::message("[LuaSystem] No properties refreshed - restart editor to load new properties\n");
            }
        }
        else
        {
            Log::message("[LuaSystem] ComponentSystem not available - restart editor to load new properties\n");
        }
    }
    catch (const std::exception& e) {
        Log::warning("[LuaSystem] [WARNING] Exception during property refresh: %s\n", e.what());
        Log::warning("[LuaSystem] [WARNING] Properties will load on restart\n");
    }
    catch (...) {
        Log::warning("[LuaSystem] [WARNING] Could not refresh properties (will load on restart)\n");
    }
}

bool LuaPropertyManager::bindPropertyParameters(const LuaComponentDescriptor& d, const Unigine::PropertyPtr& prop,
    std::unordered_map<std::string, Unigine::Vector<LuaPropertyDef>>& bindings)
{
    Unigine::Vector<LuaPropertyDef> bindings_list;

    // Get existing parameters from the property (already defined in XML)
    for (const LuaPropertyDef& p : d.properties)
    {
        Unigine::PropertyParameterPtr param = prop->getParameterPtr(p.name.get());

        if (!param || !param->isExist())
        {
            Log::warning("[LuaSystem] Parameter '%s' not found in property '%s'\n",
                p.name.get(), d.name.get());
            continue;
        }

        LuaPropertyDef binding;
        binding.param = param;
        binding.lua_name = p.name;
        binding.name = p.name;
        binding.type = p.type;
        binding.title = p.title;
        binding.group = p.group;
        binding.tooltip = p.tooltip;
        binding.args = p.args;
        binding.sdefault = p.sdefault;

        bindings_list.append(binding);
    }

    bindings[std::string(d.name.get())] = bindings_list;

    return true;
}

void LuaPropertyManager::syncLuaComponents(Unigine::Vector<LuaComponentInstance>& active_components,
    std::unordered_map<std::string, Unigine::Vector<LuaPropertyDef>>& bindings)
{
    for (LuaComponentInstance& ci : active_components)
    {
        std::unordered_map<std::string, Unigine::Vector<LuaPropertyDef>>::iterator map_it = bindings.find(ci.descriptor.name.get());
        if (map_it == bindings.end())
            continue;

        const Unigine::Vector<LuaPropertyDef>& bind_list = map_it->second;

        for (const LuaPropertyDef& entry : bind_list)
        {
            const char* key = entry.lua_name.get();
            sol::object lua_value = ci.instance[key];

            if (!lua_value.valid())
                continue;

            Unigine::PropertyParameterPtr param = entry.param;
            const Unigine::String& type = entry.type;

            if (type == "int")
            {
                if (lua_value.is<int>())
                    param->setValueInt(lua_value.as<int>());
            }
            else if (type == "float")
            {
                if (lua_value.is<float>())
                    param->setValueFloat(lua_value.as<float>());
            }
            else if (type == "double")
            {
                if (lua_value.is<double>())
                    param->setValueDouble(lua_value.as<double>());
            }
            else if (type == "bool")
            {
                if (lua_value.is<bool>())
                    param->setValueToggle(lua_value.as<bool>());
            }
            else if (type == "string")
            {
                if (lua_value.is<std::string>())
                    param->setValueString(lua_value.as<std::string>().c_str());
            }
            else if (type == "switch")
            {
                if (lua_value.is<int>())
                    param->setValueSwitch(lua_value.as<bool>());
            }
            else if (type == "mask")
            {
                if (lua_value.is<int>())
                    param->setValueMask(lua_value.as<int>());
            }
            else if (type == "vec3")
            {
                float x, y, z;
                Unigine::Math::vec3 current_vec = param->getValueVec3();
                x = current_vec.x; y = current_vec.y; z = current_vec.z;

                if (lua_value.is<sol::table>())
                {
                    sol::table t = lua_value.as<sol::table>();

                    if (t[1].valid()) x = t[1].get<float>();
                    if (t[2].valid()) y = t[2].get<float>();
                    if (t[3].valid()) z = t[3].get<float>();

                    if (t["x"].valid()) x = t["x"].get<float>();
                    if (t["y"].valid()) y = t["y"].get<float>();
                    if (t["z"].valid()) z = t["z"].get<float>();
                }
                else if (lua_value.is<std::tuple<float, float, float>>())
                {
                    std::tie(x, y, z) = lua_value.as<std::tuple<float, float, float>>();
                }

                param->setValueVec3(Unigine::Math::vec3(x, y, z));
            }
            else if (type == "vec4" || type == "color")
            {
                float x, y, z, w;
                Unigine::Math::vec4 current_vec = param->getValueVec4();
                x = current_vec.x; y = current_vec.y; z = current_vec.z; w = current_vec.w;
                if (lua_value.is<sol::table>())
                {
                    sol::table t = lua_value.as<sol::table>();
                    if (t[1].valid()) x = t[1].get<float>();
                    if (t[2].valid()) y = t[2].get<float>();
                    if (t[3].valid()) z = t[3].get<float>();
                    if (t[4].valid()) w = t[4].get<float>();
                    if (t["x"].valid()) x = t["x"].get<float>();
                    if (t["y"].valid()) y = t["y"].get<float>();
                    if (t["z"].valid()) z = t["z"].get<float>();
                    if (t["w"].valid()) w = t["w"].get<float>();
                }
                else if (lua_value.is<std::tuple<float, float, float, float>>())
                {
                    std::tie(x, y, z, w) = lua_value.as<std::tuple<float, float, float, float>>();
                }
                param->setValueVec4(Unigine::Math::vec4(x, y, z, w));
            }
            else if (type == "toggle")
            {
                if (lua_value.is<bool>())
                    param->setValueToggle(lua_value.as<bool>());
            }
            else if (type == "file")
            {
                if (lua_value.is<std::string>())
                    param->setValueString(lua_value.as<std::string>().c_str());
            }
            else if (type == "mask")
            {
                if (lua_value.is<int>())
                    param->setValueMask(lua_value.as<int>());
            }
            else if (type == "dmat4") // add later
            {
                // Expecting a 4x4 matrix as a table of tables
                return;
            }
            else
            {
                Log::warning("[LuaSystem] [WARNING] Unsupported property type for sync: %s\n", type.get());
            }
        }
    }
}

// Lines 1240-1296 from original
void LuaPropertyManager::parseProperties(const sol::table& props, Unigine::Vector<LuaPropertyDef>& out)
{
    if (!props.valid())
        return;

    for (const std::pair<sol::object, sol::object>& pair : props)
    {
        sol::object key = pair.first;
        sol::object val = pair.second;

        if (key.get_type() != sol::type::string || val.get_type() != sol::type::table)
            continue;

        sol::table p = val.as<sol::table>();

        LuaPropertyDef def;
        def.name = key.as<std::string>().c_str();
        def.type = p.get_or<std::string>("type", "string").c_str();
        def.title = p.get_or<std::string>("title", "").c_str();
        def.group = p.get_or<std::string>("group", "").c_str();
        def.tooltip = p.get_or<std::string>("tooltip", "").c_str();
        def.args = p.get_or<std::string>("args", "").c_str();

        // Handle default value conversion
        if (p["default"].valid())
        {
            sol::object d = p["default"];

            if (d.is<bool>())
            {
                def.sdefault = d.as<bool>() ? "1" : "0";
            }
            else if (d.is<std::string>())
            {
                def.sdefault = d.as<std::string>().c_str();
            }
            else if (d.is<int>())
            {
                def.sdefault = String::itoa(d.as<int>());
            }
            else if (d.is<double>())
            {
                double value = d.as<double>();

                if (value == (int)value)
                {
                    def.sdefault = String::format("%d", (int)value);
                }
                else
                {
                    def.sdefault = String::format("%.6g", value);
                }
            }
        }
        out.append(def);
    }
}
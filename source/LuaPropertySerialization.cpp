#include "CommonIncludes.h"
#include "LuaPropertySerialization.h"
#include "LuaSystemTypes.h"
#include <UnigineJson.h>
#include <fstream>
#include <sstream>

using namespace Unigine;

// Simple JSON serialization (for production, consider using a proper JSON library)
std::string LuaPropertySerialization::serializeToJSON(const LuaComponentInstance& instance)
{
    std::ostringstream json;
    json << "{\n";
    json << "  \"component_name\": \"" << instance.descriptor.name.get() << "\",\n";
    json << "  \"node_id\": " << (instance.node ? instance.node->getID() : 0) << ",\n";
    json << "  \"properties\": {\n";
    
    bool first = true;
    if (instance.instance.valid())
    {
        instance.instance.for_each([&json, &first](const sol::object& key, const sol::object& value) {
            if (!key.is<std::string>())
                return;
            
            std::string key_str = key.as<std::string>();
            
            // Skip internal fields
            if (key_str == "node" || key_str == "_pooled")
                return;
            
            if (!first)
                json << ",\n";
            first = false;
            
            json << "    \"" << key_str << "\": ";
            
            if (value.is<bool>())
            {
                json << (value.as<bool>() ? "true" : "false");
            }
            else if (value.is<int>())
            {
                json << value.as<int>();
            }
            else if (value.is<double>())
            {
                json << value.as<double>();
            }
            else if (value.is<std::string>())
            {
                json << "\"" << value.as<std::string>() << "\"";
            }
            else if (value.is<Math::vec3>())
            {
                Math::vec3 v = value.as<Math::vec3>();
                json << "[" << v.x << ", " << v.y << ", " << v.z << "]";
            }
            else
            {
                json << "null";
            }
        });
    }
    
    json << "\n  }\n";
    json << "}";
    
    return json.str();
}

bool LuaPropertySerialization::deserializeFromJSON(const std::string& json,
                                                   LuaComponentInstance& instance,
                                                   sol::state& lua)
{
    // Simplified JSON parsing - for production, use a proper JSON library
    // This is a basic implementation that handles simple cases
    
    // Find properties section
    size_t props_start = json.find("\"properties\":");
    if (props_start == std::string::npos)
        return false;
    
    // This is a simplified parser - in production, use nlohmann/json or similar
    // For now, we'll use Lua to parse JSON
    try
    {
        // Use Lua's JSON parsing if available, or implement a simple parser
        sol::function json_parse = lua["json"]["decode"];
        if (json_parse.valid())
        {
            sol::table data = json_parse(json);
            sol::table props = data["properties"];
            
            if (props.valid() && instance.instance.valid())
            {
                props.for_each([&instance](const sol::object& key, const sol::object& value) {
                    if (key.is<std::string>())
                    {
                        instance.instance[key] = value;
                    }
                });
                return true;
            }
        }
    }
    catch (const sol::error& e)
    {
        Log::error("[PropertySerialization] Error parsing JSON: %s\n", e.what());
    }
    
    return false;
}

bool LuaPropertySerialization::saveToFile(const LuaComponentInstance& instance,
                                          const Unigine::String& filepath)
{
    std::string json = serializeToJSON(instance);
    
    std::ofstream file(filepath.get());
    if (!file.is_open())
    {
        Log::error("[PropertySerialization] Failed to open file for writing: %s\n", filepath.get());
        return false;
    }
    
    file << json;
    file.close();
    
    Log::message("[PropertySerialization] Saved component to: %s\n", filepath.get());
    return true;
}

bool LuaPropertySerialization::loadFromFile(const Unigine::String& filepath,
                                            LuaComponentInstance& instance,
                                            sol::state& lua)
{
    std::ifstream file(filepath.get());
    if (!file.is_open())
    {
        Log::error("[PropertySerialization] Failed to open file for reading: %s\n", filepath.get());
        return false;
    }
    
    std::string json((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    return deserializeFromJSON(json, instance, lua);
}

std::string LuaPropertySerialization::serializeNodeComponents(Unigine::NodePtr node,
                                                             Unigine::Vector<LuaComponentInstance>& active_components)
{
    std::ostringstream json;
    json << "{\n";
    json << "  \"node_id\": " << (node ? node->getID() : 0) << ",\n";
    json << "  \"components\": [\n";
    
    bool first_component = true;
    for (int i = 0; i < active_components.size(); i++)
    {
        if (active_components[i].node == node)
        {
            if (!first_component)
                json << ",\n";
            first_component = false;
            
            std::string comp_json = serializeToJSON(active_components[i]);
            // Indent the component JSON
            std::string indented = comp_json;
            size_t pos = 0;
            while ((pos = indented.find("\n", pos)) != std::string::npos)
            {
                indented.insert(pos + 1, "    ");
                pos += 5;
            }
            json << "    " << indented;
        }
    }
    
    json << "\n  ]\n";
    json << "}";
    
    return json.str();
}

namespace
{
    void applyPropertyToInstance(sol::table& instance, const std::string& key,
                                 const sol::object& value, sol::state& lua)
    {
        if (!instance.valid())
            return;
        if (value.is<sol::table>())
        {
            sol::table t = value.as<sol::table>();
            size_t n = t.size();
            if (n == 3)
            {
                sol::object o1 = t[1], o2 = t[2], o3 = t[3];
                if (o1.is<double>() && o2.is<double>() && o3.is<double>())
                {
                    float x = static_cast<float>(o1.as<double>());
                    float y = static_cast<float>(o2.as<double>());
                    float z = static_cast<float>(o3.as<double>());
                    sol::function vec3_ctor = lua["vec3"];
                    if (vec3_ctor.valid())
                    {
                        instance[key] = vec3_ctor(x, y, z);
                        return;
                    }
                }
            }
        }
        instance[key] = value;
    }
}

bool LuaPropertySerialization::deserializeNodeComponents(const std::string& json,
                                                         Unigine::NodePtr node,
                                                         Unigine::Vector<LuaComponentInstance>& active_components,
                                                         sol::state& lua)
{
    if (!node)
    {
        Log::error("[PropertySerialization] deserializeNodeComponents: null node\n");
        return false;
    }

    bool applied = false;

    sol::function json_decode = lua["json"]["decode"];
    if (json_decode.valid())
    {
        try
        {
            sol::table data = json_decode(json);
            if (!data.valid())
                return false;
            sol::table components = data["components"];
            if (!components.valid())
                return false;

            components.for_each([&](const sol::object&, const sol::object& comp_val) {
                if (!comp_val.is<sol::table>())
                    return;
                sol::table comp = comp_val.as<sol::table>();
                sol::object name_obj = comp["component_name"];
                sol::table props = comp["properties"];
                if (!name_obj.is<std::string>() || !props.valid())
                    return;
                std::string comp_name = name_obj.as<std::string>();

                for (int i = 0; i < active_components.size(); ++i)
                {
                    LuaComponentInstance& inst = active_components[i];
                    if (inst.node != node)
                        continue;
                    if (inst.descriptor.name.get() != comp_name)
                        continue;
                    if (!inst.instance.valid())
                        continue;

                    props.for_each([&](const sol::object& k, const sol::object& v) {
                        if (k.is<std::string>())
                            applyPropertyToInstance(inst.instance, k.as<std::string>(), v, lua);
                    });
                    applied = true;
                    break;
                }
            });
            return applied;
        }
        catch (const sol::error& e)
        {
            Log::error("[PropertySerialization] JSON decode error: %s\n", e.what());
            return false;
        }
    }

    JsonPtr root = Json::create();
    if (!root || root->parse(json.c_str()) != 1)
    {
        Log::error("[PropertySerialization] Failed to parse JSON (Unigine Json fallback)\n");
        return false;
    }

    JsonPtr comps = root->getChild("components");
    if (!comps || !comps->isArray())
        return false;

    int n = comps->getNumChildren();
    for (int i = 0; i < n; ++i)
    {
        JsonPtr comp = comps->getChild(i);
        if (!comp)
            continue;
        JsonPtr name_node = comp->getChild("component_name");
        JsonPtr props_node = comp->getChild("properties");
        if (!name_node || !name_node->isString() || !props_node || !props_node->isObject())
            continue;
        const char* comp_name = name_node->getString();

        for (int j = 0; j < active_components.size(); ++j)
        {
            LuaComponentInstance& inst = active_components[j];
            if (inst.node != node || inst.descriptor.name.get() != comp_name)
                continue;
            if (!inst.instance.valid())
                continue;

            int np = props_node->getNumChildren();
            for (int k = 0; k < np; ++k)
            {
                JsonPtr prop = props_node->getChild(k);
                if (!prop)
                    continue;
                const char* key = prop->getName();
                if (!key || !key[0])
                    continue;

                if (prop->isBool())
                    inst.instance[key] = prop->getBool();
                else if (prop->isNumber())
                    inst.instance[key] = prop->getNumber();
                else if (prop->isString())
                    inst.instance[key] = std::string(prop->getString());
                else if (prop->isArray() && prop->getNumChildren() == 3)
                {
                    JsonPtr c1 = prop->getChild(0), c2 = prop->getChild(1), c3 = prop->getChild(2);
                    if (c1 && c2 && c3 && c1->isNumber() && c2->isNumber() && c3->isNumber())
                    {
                        Math::vec3 v(static_cast<float>(c1->getNumber()),
                                     static_cast<float>(c2->getNumber()),
                                     static_cast<float>(c3->getNumber()));
                        inst.instance[key] = v;
                    }
                }
            }
            applied = true;
            break;
        }
    }

    return applied;
}

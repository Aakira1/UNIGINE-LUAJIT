#include "CommonIncludes.h"
#include "LuaComponentQueries.h"
#include <UnigineMathLib.h>

using namespace Unigine;
using namespace Unigine::Math;

Unigine::Vector<LuaComponentInstance*> LuaComponentQueries::findByType(
    const Unigine::String& component_type,
    Unigine::Vector<LuaComponentInstance>& active_components)
{
    Unigine::Vector<LuaComponentInstance*> results;
    
    for (int i = 0; i < active_components.size(); i++)
    {
        if (active_components[i].descriptor.name == component_type)
        {
            results.append(&active_components[i]);
        }
    }
    
    return results;
}

Unigine::Vector<LuaComponentInstance*> LuaComponentQueries::findByTag(
    const Unigine::String& tag,
    Unigine::Vector<LuaComponentInstance>& active_components)
{
    Unigine::Vector<LuaComponentInstance*> results;
    
    for (int i = 0; i < active_components.size(); i++)
    {
        LuaComponentInstance& ci = active_components[i];
        if (ci.instance.valid())
        {
            sol::object tag_obj = ci.instance["tag"];
            if (tag_obj.valid() && tag_obj.is<std::string>())
            {
                if (tag_obj.as<std::string>() == tag.get())
                {
                    results.append(&ci);
                }
            }
        }
    }
    
    return results;
}

LuaComponentInstance* LuaComponentQueries::findByNode(
    Unigine::NodePtr node,
    const Unigine::String& component_type,
    Unigine::Vector<LuaComponentInstance>& active_components)
{
    if (!node)
        return nullptr;
    
    for (int i = 0; i < active_components.size(); i++)
    {
        LuaComponentInstance& ci = active_components[i];
        if (ci.node == node && ci.descriptor.name == component_type)
        {
            return &ci;
        }
    }
    
    return nullptr;
}

Unigine::Vector<LuaComponentInstance*> LuaComponentQueries::findByNodeAll(
    Unigine::NodePtr node,
    Unigine::Vector<LuaComponentInstance>& active_components)
{
    Unigine::Vector<LuaComponentInstance*> results;
    
    if (!node)
        return results;
    
    for (int i = 0; i < active_components.size(); i++)
    {
        if (active_components[i].node == node)
        {
            results.append(&active_components[i]);
        }
    }
    
    return results;
}

Unigine::Vector<LuaComponentInstance*> LuaComponentQueries::findByRadius(
    const Math::vec3& position,
    float radius,
    Unigine::Vector<LuaComponentInstance>& active_components)
{
    Unigine::Vector<LuaComponentInstance*> results;
    float radius_sq = radius * radius;
    
    for (int i = 0; i < active_components.size(); i++)
    {
        LuaComponentInstance& ci = active_components[i];
        if (ci.node)
        {
            Math::Vec3 node_pos_d = ci.node->getWorldPosition();
            Math::vec3 node_pos = Math::vec3(node_pos_d);
            float dist_sq = Math::length2(node_pos - position);
            if (dist_sq <= radius_sq)
            {
                results.append(&ci);
            }
        }
    }
    
    return results;
}

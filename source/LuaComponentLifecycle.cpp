#include "CommonIncludes.h"
#include "LuaComponentLifecycle.h"
#include "LuaSystem.h"
#include <UnigineNodes.h>
#include <UnigineNode.h>

using namespace Unigine;

void LuaComponentLifecycle::update(float ifps, Vector<LuaComponentInstance>& active_components)
{
    (void)ifps;
    for (LuaComponentInstance& ci : active_components)
    {
        if (!ci.initialized && ci.on_init.valid())
        {
            try {
                ci.on_init(ci.instance);
                ci.initialized = true;
            }
            catch (const sol::error& e) {
                Log::error("[LuaSystem] Init error: %s\n", e.what());
            }
        }
    }
}

void LuaComponentLifecycle::shutdown(Vector<LuaComponentInstance>& /*active_components*/,
    HashMap<String, LuaComponentDescriptor>& /*components*/,
    std::unordered_map<std::string, Vector<LuaPropertyDef>>& /*bindings*/,
    bool& is_shutting_down, sol::state& /*lua*/)
{
    Log::message("[LuaSystem] Shutting down Lua runtime...\n");
    is_shutting_down = true;

    Log::message("[LuaSystem] Lua runtime successfully shut down.\n");
}

void LuaComponentLifecycle::removeLuaComponent(const Unigine::NodePtr& node, const Unigine::String& component_name,
    Vector<LuaComponentInstance>& active_components)
{
    for (int i = 0; i < active_components.size(); i++)
    {
        const LuaComponentInstance& ci = active_components[i];

        if (ci.node == node && ci.descriptor.name == component_name)
        {
            if (ci.on_shutdown.valid())
            {
                try {
                    ci.on_shutdown(ci.instance);
                }
                catch (const sol::error& e) {
                    Log::error("[LuaSystem] Shutdown error: %s\n", e.what());
                }
            }
            active_components.remove(i);
            break;
        }
    }
}

void LuaComponentLifecycle::registerComponentInstance(NodePtr node, const LuaComponentDescriptor& desc, sol::table instance,
    Vector<LuaComponentInstance>& active_components)
{
    LuaComponentInstance ci;
    ci.node = node;
    ci.descriptor = desc;
    ci.instance = instance;

    // Cache all lifecycle functions
    if (!desc.fn_on_init.empty())
        ci.on_init = (*lua)[desc.fn_on_init.get()];
    if (!desc.fn_on_update_async_thread.empty())
        ci.on_update_async_thread = (*lua)[desc.fn_on_update_async_thread.get()];
    if (!desc.fn_on_update_sync_thread.empty())
        ci.on_update_sync_thread = (*lua)[desc.fn_on_update_sync_thread.get()];
    if (!desc.fn_on_update.empty())
        ci.on_update = (*lua)[desc.fn_on_update.get()];
    if (!desc.fn_on_post_update.empty())
        ci.on_post_update = (*lua)[desc.fn_on_post_update.get()];
    if (!desc.fn_on_update_physics.empty())
        ci.on_update_physics = (*lua)[desc.fn_on_update_physics.get()];
    if (!desc.fn_on_swap.empty())
        ci.on_swap = (*lua)[desc.fn_on_swap.get()];
    if (!desc.fn_on_shutdown.empty())
        ci.on_shutdown = (*lua)[desc.fn_on_shutdown.get()];

    ci.initialized = true;  // Already initialized by generated component

    active_components.append(ci);

    Log::message("[LuaSystem] Registered instance: %s on node %s\n",
        desc.name.get(), node->getName());
}

void LuaComponentLifecycle::unregisterComponentInstance(NodePtr node, const String& component_name,
    Vector<LuaComponentInstance>& active_components)
{
    for (int i = active_components.size() - 1; i >= 0; i--)
    {
        LuaComponentInstance& ci = active_components[i];
        if (ci.node == node && ci.descriptor.name == component_name)
        {
            Log::message("[LuaSystem] Unregistered instance: %s from node %s\n",
                component_name.get(), node->getName());
            active_components.remove(i);
            break;
        }
    }
}
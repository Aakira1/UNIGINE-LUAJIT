#pragma once
#include "CommonIncludes.h"
#include <UnigineComponentSystem.h>
#include <UnigineInterface.h>
#include "../LuaSystem.h"

// Auto-generated wrapper for Lua component: RotatingCube
class Lua_RotatingCube : public Unigine::ComponentBase
{
public:
    COMPONENT_DEFINE(Lua_RotatingCube, Unigine::ComponentBase);
    COMPONENT_DESCRIPTION("Rotating cube with debug logging");

    PROP_PARAM(Float, rotation_speed, 45);
    PROP_PARAM(Toggle, use_world_transform, 0);

    COMPONENT_INIT(init);
    COMPONENT_UPDATE(update);

protected:
    void init();
    void update();

private:
    sol::table lua_instance;
    bool lua_ready = false;
};

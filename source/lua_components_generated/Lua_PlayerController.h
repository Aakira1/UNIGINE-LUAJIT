#pragma once
#include "CommonIncludes.h"
#include <UnigineComponentSystem.h>
#include <UnigineInterface.h>
#include "../LuaSystem.h"

// Auto-generated wrapper for Lua component: PlayerController
class Lua_PlayerController : public Unigine::ComponentBase
{
public:
    COMPONENT_DEFINE(Lua_PlayerController, Unigine::ComponentBase);
    COMPONENT_DESCRIPTION("Simple first-person controller (movement, jump, crouch)");

    PROP_PARAM(Float, move_speed, 5, "Move Speed");
    PROP_PARAM(Float, jump_power, 6, "Jump Power");
    PROP_PARAM(Float, crouch_speed, 3, "Crouch Speed");
    PROP_PARAM(Float, crouch_height, 1, "Crouch Height");
    PROP_PARAM(Float, mouse_sensitivity, 0, "Mouse Sensitivity");
    PROP_PARAM(Float, camera_height, 2, "Camera Height");

    COMPONENT_INIT(init);
    COMPONENT_UPDATE(update);

protected:
    void init();
    void update();

private:
    sol::table lua_instance;
    bool lua_ready = false;
};

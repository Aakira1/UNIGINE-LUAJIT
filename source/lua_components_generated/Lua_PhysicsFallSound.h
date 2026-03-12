#pragma once
#include "CommonIncludes.h"
#include <UnigineComponentSystem.h>
#include <UnigineInterface.h>
#include "../LuaSystem.h"

// Auto-generated wrapper for Lua component: PhysicsFallSound
class Lua_PhysicsFallSound : public Unigine::ComponentBase
{
public:
    COMPONENT_DEFINE(Lua_PhysicsFallSound, Unigine::ComponentBase);
    COMPONENT_DESCRIPTION("Plays a sound when a physics-driven cube impacts any surface, with pitch variation based on impact velocity");

    PROP_PARAM(Float, cooldown_time, 0, "Impact Cooldown (seconds)");
    PROP_PARAM(File, sound_path, "sounds/preview.mp3", "Impact Sound");
    PROP_PARAM(Float, min_velocity, 1, "Minimum Impact Velocity (m/s)");
    PROP_PARAM(Float, impact_threshold, 1, "Impact Distance Threshold");
    PROP_PARAM(Float, min_pitch, 1, "Minimum Pitch");
    PROP_PARAM(Float, max_pitch, 2, "Maximum Pitch");

    COMPONENT_INIT(init);
    COMPONENT_UPDATE(update);

protected:
    void init();
    void update();

private:
    sol::table lua_instance;
    bool lua_ready = false;
};

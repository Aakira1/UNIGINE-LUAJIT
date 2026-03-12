#pragma once
#ifndef LUA_ANIMATION_SYSTEM_H
#define LUA_ANIMATION_SYSTEM_H

#include "LuaSystemForward.h"
#include <UnigineNode.h>
#include <unordered_map>
#include <vector>

namespace Unigine {
    class LuaAnimationSystem {
    public:
        // Animation playback control
        struct AnimationState {
            bool is_playing = false;
            float current_time = 0.0f;
            float duration = 0.0f;
            float speed = 1.0f;
            bool loop = false;
        };
        
        // Active blend operation
        struct BlendOperation {
            Unigine::ObjectPtr obj;
            int from_layer = -1;
            int to_layer = -1;
            float start_time = 0.0f;
            float blend_time = 0.0f;
            float from_start_weight = 1.0f;
            float to_start_weight = 0.0f;
        };
        
        // Play animation on object
        static bool playAnimation(Unigine::ObjectPtr obj, 
                                 const Unigine::String& animation_name,
                                 float speed = 1.0f,
                                 bool loop = false);
        
        // Stop animation
        static bool stopAnimation(Unigine::ObjectPtr obj, const Unigine::String& animation_name);
        
        // Pause animation
        static bool pauseAnimation(Unigine::ObjectPtr obj, const Unigine::String& animation_name);
        
        // Get animation state
        static AnimationState getAnimationState(Unigine::ObjectPtr obj, const Unigine::String& animation_name);
        
        // Set animation time
        static bool setAnimationTime(Unigine::ObjectPtr obj, 
                                    const Unigine::String& animation_name,
                                    float time);
        
        // Blend animations (now supports time-based blending)
        static bool blendAnimations(Unigine::ObjectPtr obj,
                                   const Unigine::String& from_anim,
                                   const Unigine::String& to_anim,
                                   float blend_time);
        
        // Update active blend operations (call each frame)
        static void updateBlends(float dt);
        
        // Register to Lua
        static void registerToLua(sol::state& lua);
        
    private:
        static std::vector<BlendOperation> active_blends;
        static float current_time;
    };
}

#endif // LUA_ANIMATION_SYSTEM_H

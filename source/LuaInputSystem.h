#pragma once
#ifndef LUA_INPUT_SYSTEM_H
#define LUA_INPUT_SYSTEM_H

#include "LuaSystemForward.h"
#include <unordered_map>
#include <vector>

namespace Unigine {
    // Input action mapping system
    class LuaInputSystem {
    public:
        struct InputAction {
            Unigine::String name;
            std::vector<int> keyboard_keys;
            std::vector<int> mouse_buttons;
            std::vector<int> gamepad_buttons;
            float deadzone = 0.2f;
        };
        
        struct InputAxis {
            Unigine::String name;
            int positive_key = -1;
            int negative_key = -1;
            int gamepad_axis = -1;
            float sensitivity = 1.0f;
            float deadzone = 0.2f;
        };
        
        static LuaInputSystem& get();
        
        // Action mapping
        void registerAction(const Unigine::String& name, 
                           const std::vector<int>& keyboard_keys = {},
                           const std::vector<int>& mouse_buttons = {},
                           const std::vector<int>& gamepad_buttons = {});
        
        bool isActionPressed(const Unigine::String& name);
        bool isActionDown(const Unigine::String& name);
        bool isActionReleased(const Unigine::String& name);
        
        // Axis mapping
        void registerAxis(const Unigine::String& name,
                         int positive_key = -1,
                         int negative_key = -1,
                         int gamepad_axis = -1,
                         float sensitivity = 1.0f);
        
        float getAxis(const Unigine::String& name);
        
        // Update (call each frame)
        void update();
        
        // Register to Lua
        static void registerToLua(sol::state& lua);
        
    private:
        LuaInputSystem() = default;
        std::unordered_map<std::string, InputAction> actions;
        std::unordered_map<std::string, InputAxis> axes;
        std::unordered_map<std::string, bool> action_states;
        std::unordered_map<std::string, bool> action_states_prev;
    };
}

#endif // LUA_INPUT_SYSTEM_H

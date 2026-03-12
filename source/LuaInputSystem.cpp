#include "CommonIncludes.h"
#include "LuaInputSystem.h"
#include "LuaSystem.h"
#include <UnigineInput.h>

using namespace Unigine;

LuaInputSystem& LuaInputSystem::get()
{
    static LuaInputSystem instance;
    return instance;
}

void LuaInputSystem::registerAction(const Unigine::String& name,
                                   const std::vector<int>& keyboard_keys,
                                   const std::vector<int>& mouse_buttons,
                                   const std::vector<int>& gamepad_buttons)
{
    InputAction action;
    action.name = name;
    action.keyboard_keys = keyboard_keys;
    action.mouse_buttons = mouse_buttons;
    action.gamepad_buttons = gamepad_buttons;
    
    actions[name.get()] = action;
    action_states[name.get()] = false;
    action_states_prev[name.get()] = false;
}

bool LuaInputSystem::isActionPressed(const Unigine::String& name)
{
    auto it = actions.find(name.get());
    if (it == actions.end())
        return false;
    
    const InputAction& action = it->second;
    
    // Check keyboard keys
    for (int key : action.keyboard_keys)
    {
        if (Input::isKeyPressed(static_cast<Input::KEY>(key)))
            return true;
    }
    
    // Check mouse buttons
    for (int button : action.mouse_buttons)
    {
        if (Input::isMouseButtonPressed(static_cast<Input::MOUSE_BUTTON>(button)))
            return true;
    }
    
    // Check gamepad buttons (if gamepad is available)
    if (!action.gamepad_buttons.empty() && Input::getNumGamePads() > 0)
    {
        // Use first available gamepad (index 0)
        InputGamePadPtr gamepad = Input::getGamePad(0);
        if (gamepad && gamepad->isAvailable())
        {
            for (int button : action.gamepad_buttons)
            {
                if (gamepad->isButtonPressed(static_cast<Input::GAMEPAD_BUTTON>(button)))
                    return true;
            }
        }
    }
    
    return false;
}

bool LuaInputSystem::isActionDown(const Unigine::String& name)
{
    return isActionPressed(name) && !action_states_prev[name.get()];
}

bool LuaInputSystem::isActionReleased(const Unigine::String& name)
{
    return !isActionPressed(name) && action_states_prev[name.get()];
}

void LuaInputSystem::registerAxis(const Unigine::String& name,
                                  int positive_key,
                                  int negative_key,
                                  int gamepad_axis,
                                  float sensitivity)
{
    InputAxis axis;
    axis.name = name;
    axis.positive_key = positive_key;
    axis.negative_key = negative_key;
    axis.gamepad_axis = gamepad_axis;
    axis.sensitivity = sensitivity;
    
    axes[name.get()] = axis;
}

float LuaInputSystem::getAxis(const Unigine::String& name)
{
    auto it = axes.find(name.get());
    if (it == axes.end())
        return 0.0f;
    
    const InputAxis& axis = it->second;
    float value = 0.0f;
    
    // Keyboard input
    if (axis.positive_key >= 0 && Input::isKeyPressed(static_cast<Input::KEY>(axis.positive_key)))
        value += 1.0f;
    if (axis.negative_key >= 0 && Input::isKeyPressed(static_cast<Input::KEY>(axis.negative_key)))
        value -= 1.0f;
    
    // Gamepad input
    if (axis.gamepad_axis >= 0 && Input::getNumGamePads() > 0)
    {
        InputGamePadPtr gamepad = Input::getGamePad(0);
        if (gamepad && gamepad->isAvailable())
        {
            float gamepad_value = 0.0f;
            Input::GAMEPAD_AXIS axis_enum = static_cast<Input::GAMEPAD_AXIS>(axis.gamepad_axis);
            
            switch (axis_enum)
            {
                case Input::GAMEPAD_AXIS_LEFT_X:
                    gamepad_value = gamepad->getAxesLeft().x;
                    break;
                case Input::GAMEPAD_AXIS_LEFT_Y:
                    gamepad_value = gamepad->getAxesLeft().y;
                    break;
                case Input::GAMEPAD_AXIS_RIGHT_X:
                    gamepad_value = gamepad->getAxesRight().x;
                    break;
                case Input::GAMEPAD_AXIS_RIGHT_Y:
                    gamepad_value = gamepad->getAxesRight().y;
                    break;
                case Input::GAMEPAD_AXIS_LEFT_TRIGGER:
                    gamepad_value = gamepad->getTriggerLeft();
                    break;
                case Input::GAMEPAD_AXIS_RIGHT_TRIGGER:
                    gamepad_value = gamepad->getTriggerRight();
                    break;
                default:
                    break;
            }
            
            // Apply deadzone (default 0.1) and use gamepad value if keyboard hasn't set it
            const float deadzone = 0.1f;
            if (std::abs(gamepad_value) > deadzone && std::abs(value) < 0.5f)
            {
                value = gamepad_value;
            }
        }
    }
    
    return value * axis.sensitivity;
}

void LuaInputSystem::update()
{
    // Store previous frame states
    for (auto& pair : action_states)
    {
        action_states_prev[pair.first] = pair.second;
        pair.second = isActionPressed(Unigine::String(pair.first.c_str()));
    }
}

void LuaInputSystem::registerToLua(sol::state& lua)
{
    auto& input_system = get();
    
    lua["InputActions"] = lua.create_table();
    
    // Register action
    lua["InputActions"]["register"] = [&input_system](const std::string& name,
                                                       sol::optional<sol::table> keys,
                                                       sol::optional<sol::table> mouse,
                                                       sol::optional<sol::table> gamepad) {
        std::vector<int> keyboard_keys;
        std::vector<int> mouse_buttons;
        std::vector<int> gamepad_buttons;
        
        if (keys)
        {
            keys->for_each([&keyboard_keys]([[maybe_unused]] const sol::object& key, const sol::object& value) {
                if (value.is<int>())
                    keyboard_keys.push_back(value.as<int>());
            });
        }
        
        if (mouse)
        {
            mouse->for_each([&mouse_buttons]([[maybe_unused]] const sol::object& key, const sol::object& value) {
                if (value.is<int>())
                    mouse_buttons.push_back(value.as<int>());
            });
        }
        
        if (gamepad)
        {
            gamepad->for_each([&gamepad_buttons]([[maybe_unused]] const sol::object& key, const sol::object& value) {
                if (value.is<int>())
                    gamepad_buttons.push_back(value.as<int>());
            });
        }
        
        input_system.registerAction(name.c_str(), keyboard_keys, mouse_buttons, gamepad_buttons);
    };
    
    // Check action states
    lua["InputActions"]["isPressed"] = [&input_system](const std::string& name) {
        return input_system.isActionPressed(name.c_str());
    };
    
    lua["InputActions"]["isDown"] = [&input_system](const std::string& name) {
        return input_system.isActionDown(name.c_str());
    };
    
    lua["InputActions"]["isReleased"] = [&input_system](const std::string& name) {
        return input_system.isActionReleased(name.c_str());
    };
    
    // Axis
    lua["InputActions"]["registerAxis"] = [&input_system](const std::string& name,
                                                          sol::optional<int> positive,
                                                          sol::optional<int> negative,
                                                          sol::optional<int> gamepad_axis,
                                                          sol::optional<float> sensitivity) {
        input_system.registerAxis(name.c_str(),
                                  positive.value_or(-1),
                                  negative.value_or(-1),
                                  gamepad_axis.value_or(-1),
                                  sensitivity.value_or(1.0f));
    };
    
    lua["InputActions"]["getAxis"] = [&input_system](const std::string& name) {
        return input_system.getAxis(name.c_str());
    };
    
    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaInputSystem] Input action system registered\n");
}

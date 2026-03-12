#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include <UnigineInput.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

/// <summary>
/// Registers the Input API in the provided Lua state by creating an 'Input' table and binding Input functions (isKeyPressed, isKeyDown, isKeyUp).
/// </summary>
/// <param name="lua">Reference to the sol::state representing the Lua environment where the Input table and functions will be registered. The function modifies this Lua state.</param>
void Unigine::LuaBindings::registerInputAPI(sol::state& lua)
{
    lua["Input"] = lua.create_table();

    // Key state functions
    lua["Input"]["isKeyPressed"] = [](Input::KEY key) { return Input::isKeyPressed(static_cast<Input::KEY>(key)); };
    lua["Input"]["isKeyDown"] = [](Input::KEY key) { return Input::isKeyDown(static_cast<Input::KEY>(key)); };
    lua["Input"]["isKeyUp"] = [](Input::KEY key) { return Input::isKeyUp(static_cast<Input::KEY>(key)); };

    // Mouse button state
    lua["Input"]["isMouseButtonPressed"] = [](Input::MOUSE_BUTTON button) {
        return Input::isMouseButtonPressed(static_cast<Input::MOUSE_BUTTON>(button));
        };
    lua["Input"]["isMouseButtonDown"] = [](Input::MOUSE_BUTTON button) {
        return Input::isMouseButtonDown(static_cast<Input::MOUSE_BUTTON>(button));
        };
    lua["Input"]["isMouseButtonUp"] = [](Input::MOUSE_BUTTON button) {
        return Input::isMouseButtonUp(static_cast<Input::MOUSE_BUTTON>(button));
        };

    // Mouse position functions
    lua["Input"]["getMousePosition"] = []() {
        Math::ivec2 pos = Input::getMousePosition();
        return std::make_tuple(pos.x, pos.y);
        };
    lua["Input"]["getMouseX"] = []() { return Input::getMousePosition().x; };
    lua["Input"]["getMouseY"] = []() { return Input::getMousePosition().y; };

    lua["Input"]["getMouseDelta"] = []() {
        Math::ivec2 delta = Input::getMouseDeltaPosition();
        return std::make_tuple(delta.x, delta.y);
        };
    lua["Input"]["getMouseDX"] = []() { return Input::getMouseDeltaPosition().x; };
    lua["Input"]["getMouseDY"] = []() { return Input::getMouseDeltaPosition().y; };

    lua["Input"]["getMouseDeltaRaw"] = []() {
        Math::ivec2 delta = Input::getMouseDeltaRaw();
        return std::make_tuple(delta.x, delta.y);
        };

    // Mouse grab (for camera control)
    lua["Input"]["isMouseGrab"] = []() { return Input::isMouseGrab(); };
    lua["Input"]["setMouseGrab"] = [](bool grab) { Input::setMouseGrab(grab); };

    
    // ALPHABET KEYS
    lua["Input"]["KEY_A"] = Input::KEY_A;
    lua["Input"]["KEY_B"] = Input::KEY_B;
    lua["Input"]["KEY_C"] = Input::KEY_C;
    lua["Input"]["KEY_D"] = Input::KEY_D;
    lua["Input"]["KEY_E"] = Input::KEY_E;
    lua["Input"]["KEY_F"] = Input::KEY_F;
    lua["Input"]["KEY_G"] = Input::KEY_G;
    lua["Input"]["KEY_H"] = Input::KEY_H;
    lua["Input"]["KEY_I"] = Input::KEY_I;
    lua["Input"]["KEY_J"] = Input::KEY_J;
    lua["Input"]["KEY_K"] = Input::KEY_K;
    lua["Input"]["KEY_L"] = Input::KEY_L;
    lua["Input"]["KEY_M"] = Input::KEY_M;
    lua["Input"]["KEY_N"] = Input::KEY_N;
    lua["Input"]["KEY_O"] = Input::KEY_O;
    lua["Input"]["KEY_P"] = Input::KEY_P;
    lua["Input"]["KEY_Q"] = Input::KEY_Q;
    lua["Input"]["KEY_R"] = Input::KEY_R;
    lua["Input"]["KEY_S"] = Input::KEY_S;
    lua["Input"]["KEY_T"] = Input::KEY_T;
    lua["Input"]["KEY_U"] = Input::KEY_U;
    lua["Input"]["KEY_V"] = Input::KEY_V;
    lua["Input"]["KEY_W"] = Input::KEY_W;
    lua["Input"]["KEY_X"] = Input::KEY_X;
    lua["Input"]["KEY_Y"] = Input::KEY_Y;
    lua["Input"]["KEY_Z"] = Input::KEY_Z;

    
    // NUMBER KEYS (top row)
    lua["Input"]["KEY_0"] = Input::KEY_ANY_DIGIT_0;
    lua["Input"]["KEY_1"] = Input::KEY_ANY_DIGIT_1;
    lua["Input"]["KEY_2"] = Input::KEY_ANY_DIGIT_2;
    lua["Input"]["KEY_3"] = Input::KEY_ANY_DIGIT_3;
    lua["Input"]["KEY_4"] = Input::KEY_ANY_DIGIT_4;
    lua["Input"]["KEY_5"] = Input::KEY_ANY_DIGIT_5;
    lua["Input"]["KEY_6"] = Input::KEY_ANY_DIGIT_6;
    lua["Input"]["KEY_7"] = Input::KEY_ANY_DIGIT_7;
    lua["Input"]["KEY_8"] = Input::KEY_ANY_DIGIT_8;
    lua["Input"]["KEY_9"] = Input::KEY_ANY_DIGIT_9;

    
    // FUNCTION KEYS
    lua["Input"]["KEY_F1"] = Input::KEY_F1;
    lua["Input"]["KEY_F2"] = Input::KEY_F2;
    lua["Input"]["KEY_F3"] = Input::KEY_F3;
    lua["Input"]["KEY_F4"] = Input::KEY_F4;
    lua["Input"]["KEY_F5"] = Input::KEY_F5;
    lua["Input"]["KEY_F6"] = Input::KEY_F6;
    lua["Input"]["KEY_F7"] = Input::KEY_F7;
    lua["Input"]["KEY_F8"] = Input::KEY_F8;
    lua["Input"]["KEY_F9"] = Input::KEY_F9;
    lua["Input"]["KEY_F10"] = Input::KEY_F10;
    lua["Input"]["KEY_F11"] = Input::KEY_F11;
    lua["Input"]["KEY_F12"] = Input::KEY_F12;

    
    // CONTROL KEYS
    lua["Input"]["KEY_ESC"] = Input::KEY_ESC;
    lua["Input"]["KEY_TAB"] = Input::KEY_TAB;
    lua["Input"]["KEY_CAPS_LOCK"] = Input::KEY_CAPS_LOCK;
    lua["Input"]["KEY_BACKSPACE"] = Input::KEY_BACKSPACE;
    lua["Input"]["KEY_RETURN"] = Input::KEY_BACKSPACE;
    lua["Input"]["KEY_ENTER"] = Input::KEY_ENTER;  // Alias
    lua["Input"]["KEY_SPACE"] = Input::KEY_SPACE;

    
    // MODIFIER KEYS (any shift/ctrl for FPC)
    lua["Input"]["KEY_ANY_SHIFT"] = Input::KEY_ANY_SHIFT;
    lua["Input"]["KEY_ANY_CTRL"] = Input::KEY_ANY_CTRL;
    lua["Input"]["KEY_LEFT_SHIFT"] = Input::KEY_LEFT_SHIFT;
    lua["Input"]["KEY_RIGHT_SHIFT"] = Input::KEY_RIGHT_SHIFT;
    lua["Input"]["KEY_LEFT_CTRL"] = Input::KEY_LEFT_CTRL;
    lua["Input"]["KEY_RIGHT_CTRL"] = Input::KEY_RIGHT_CTRL;
    lua["Input"]["KEY_LEFT_ALT"] = Input::KEY_LEFT_ALT;
    lua["Input"]["KEY_RIGHT_ALT"] = Input::KEY_RIGHT_ALT;
    lua["Input"]["KEY_LEFT_CMD"] = Input::KEY_LEFT_CMD;
    lua["Input"]["KEY_RIGHT_CMD"] = Input::KEY_RIGHT_CMD;

    
    // ARROW KEYS
    lua["Input"]["KEY_UP"] = Input::KEY_UP;
    lua["Input"]["KEY_DOWN"] = Input::KEY_DOWN;
    lua["Input"]["KEY_LEFT"] = Input::KEY_LEFT;
    lua["Input"]["KEY_RIGHT"] = Input::KEY_RIGHT;

    
    // NAVIGATION KEYS
    lua["Input"]["KEY_INSERT"] = Input::KEY_INSERT;
    lua["Input"]["KEY_DELETE"] = Input::KEY_DELETE;
    lua["Input"]["KEY_HOME"] = Input::KEY_HOME;
    lua["Input"]["KEY_END"] = Input::KEY_END;
    lua["Input"]["KEY_PGUP"] = Input::KEY_PGUP;
    lua["Input"]["KEY_PGDOWN"] = Input::KEY_PGDOWN;

    // NUMPAD KEYS
    lua["Input"]["KEY_NUM_LOCK"] = Input::KEY_NUM_LOCK;
    lua["Input"]["KEY_NUMPAD_0"] = Input::KEY_NUMPAD_DIGIT_0;
    lua["Input"]["KEY_NUMPAD_1"] = Input::KEY_NUMPAD_DIGIT_1;
    lua["Input"]["KEY_NUMPAD_2"] = Input::KEY_NUMPAD_DIGIT_2;
    lua["Input"]["KEY_NUMPAD_3"] = Input::KEY_NUMPAD_DIGIT_3;
    lua["Input"]["KEY_NUMPAD_4"] = Input::KEY_NUMPAD_DIGIT_4;
    lua["Input"]["KEY_NUMPAD_5"] = Input::KEY_NUMPAD_DIGIT_5;
    lua["Input"]["KEY_NUMPAD_6"] = Input::KEY_NUMPAD_DIGIT_6;
    lua["Input"]["KEY_NUMPAD_7"] = Input::KEY_NUMPAD_DIGIT_7;
    lua["Input"]["KEY_NUMPAD_8"] = Input::KEY_NUMPAD_DIGIT_8;
    lua["Input"]["KEY_NUMPAD_9"] = Input::KEY_NUMPAD_DIGIT_9;
    lua["Input"]["KEY_NUMPAD_ENTER"] = Input::KEY_NUMPAD_ENTER;
    lua["Input"]["KEY_NUMPAD_MULTIPLY"] = Input::KEY_NUMPAD_MULTIPLY;
    lua["Input"]["KEY_NUMPAD_PLUS"] = Input::KEY_NUMPAD_PLUS;
    lua["Input"]["KEY_NUMPAD_MINUS"] = Input::KEY_NUMPAD_MINUS;
    lua["Input"]["KEY_NUMPAD_DIVIDE"] = Input::KEY_NUMPAD_DIVIDE;

    // SPECIAL CHARACTERS
    lua["Input"]["KEY_MINUS"] = Input::KEY_MINUS;
    lua["Input"]["KEY_EQUALS"] = Input::KEY_EQUALS;
    lua["Input"]["KEY_LEFT_BRACKET"] = Input::KEY_LEFT_BRACKET;
    lua["Input"]["KEY_RIGHT_BRACKET"] = Input::KEY_RIGHT_BRACKET;
    lua["Input"]["KEY_SEMICOLON"] = Input::KEY_SEMICOLON;
    lua["Input"]["KEY_APOSTROPHE"] = Input::KEY_COMMA;
    lua["Input"]["KEY_BACKSLASH"] = Input::KEY_BACK_SLASH;
    lua["Input"]["KEY_COMMA"] = Input::KEY_COMMA;
    lua["Input"]["KEY_SLASH"] = Input::KEY_SLASH;

    
    // MOUSE BUTTON CONSTANTS
    lua["Input"]["MOUSE_BUTTON_LEFT"] = Input::MOUSE_BUTTON_LEFT;
    lua["Input"]["MOUSE_BUTTON_RIGHT"] = Input::MOUSE_BUTTON_RIGHT;
    lua["Input"]["MOUSE_BUTTON_MIDDLE"] = Input::MOUSE_BUTTON_MIDDLE;
    lua["Input"]["MOUSE_BUTTON_DCLICK"] = Input::MOUSE_BUTTON_DCLICK;
    lua["Input"]["MOUSE_BUTTON_AUX_0"] = Input::MOUSE_BUTTON_AUX_0;
    lua["Input"]["MOUSE_BUTTON_AUX_1"] = Input::MOUSE_BUTTON_AUX_1;

    // Gamepad
    lua["Input"]["getNumGamePads"] = []() { return Input::getNumGamePads(); };
    lua["Input"]["getGamePad"] = [](int i) {
        if (i < 0 || i >= Input::getNumGamePads()) return InputGamePadPtr();
        return Input::getGamePad(i);
    };

    lua.new_usertype<InputGamePadPtr>("InputGamePad",
        sol::no_constructor,
        "isAvailable", [](const InputGamePadPtr& g) { return g && g->isAvailable(); },
        "isButtonDown", [](const InputGamePadPtr& g, int btn) {
            return g && g->isButtonDown(static_cast<Input::GAMEPAD_BUTTON>(btn));
        },
        "isButtonPressed", [](const InputGamePadPtr& g, int btn) {
            return g && g->isButtonPressed(static_cast<Input::GAMEPAD_BUTTON>(btn));
        },
        "getAxesLeft", [](const InputGamePadPtr& g) {
            return g ? g->getAxesLeft() : Math::vec2(0.0f, 0.0f);
        },
        "getAxesRight", [](const InputGamePadPtr& g) {
            return g ? g->getAxesRight() : Math::vec2(0.0f, 0.0f);
        }
    );

    lua["Input"]["GAMEPAD_BUTTON_A"] = Input::GAMEPAD_BUTTON_A;
    lua["Input"]["GAMEPAD_BUTTON_B"] = Input::GAMEPAD_BUTTON_B;
    lua["Input"]["GAMEPAD_BUTTON_X"] = Input::GAMEPAD_BUTTON_X;
    lua["Input"]["GAMEPAD_BUTTON_Y"] = Input::GAMEPAD_BUTTON_Y;
    lua["Input"]["GAMEPAD_BUTTON_SHOULDER_LEFT"] = Input::GAMEPAD_BUTTON_SHOULDER_LEFT;
    lua["Input"]["GAMEPAD_BUTTON_SHOULDER_RIGHT"] = Input::GAMEPAD_BUTTON_SHOULDER_RIGHT;
}
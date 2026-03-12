# PlayerController Lua – Bindings & Logic vs FirstPersonController

This document confirms that the Lua **PlayerController** uses the same input bindings and logic as the C++ **FirstPersonController** (`source/FirstPersonController.h`, `source/FirstPersonController.cpp`) and the UNIGINE libraries exposed to Lua.

## Key bindings (match FirstPersonController.h)

| Action    | FirstPersonController (C++)     | Lua PlayerController / Input API      |
|----------|----------------------------------|----------------------------------------|
| Forward  | `forward_key` = `Input::KEY_W`  | `Input.KEY_W` / `Input.isKeyPressed(Input.KEY_W)` |
| Backward | `backward_key` = `Input::KEY_S` | `Input.KEY_S`                          |
| Left     | `left_key` = `Input::KEY_A`     | `Input.KEY_A`                          |
| Right    | `right_key` = `Input::KEY_D`    | `Input.KEY_D`                          |
| Jump     | `jump_key` = `Input::KEY_SPACE` | `Input.KEY_SPACE` / `Input.isKeyDown(Input.KEY_SPACE)` |
| Crouch   | `crouch_key` = `Input::KEY_ANY_CTRL` | `Input.KEY_ANY_CTRL` / `Input.isKeyPressed(Input.KEY_ANY_CTRL)` |
| Run      | `run_key` = `Input::KEY_ANY_SHIFT`   | Not used in simple Lua controller (optional) |

All of the above `Input::KEY_*` values are exposed in Lua via `source/LuaBindings_Input.cpp` (e.g. `Input.KEY_W`, `Input.KEY_SPACE`, `Input.KEY_ANY_CTRL`).

## Input API usage (UNIGINE → Lua)

- **Movement (WASD)**  
  - C++: `Input::isKeyPressed(static_cast<Input::KEY>(forward_key.get()))` in `update_move_directions()`.  
  - Lua: `Input.isKeyPressed(Input.KEY_W)` (and S/A/D). Same semantics (key held).  
  - Lua also registers and uses `InputActions` when available; raw `Input` is used as fallback.

- **Jump**  
  - C++: `Input::isKeyDown(static_cast<Input::KEY>(jump_key.get()))` when `is_ground` (adds `jump_power` once per frame while held).  
  - Lua: uses one-shot (first frame of key down or `InputActions.isDown("jump")`) so a single tap gives one jump; fallback uses `Input.isKeyDown(Input.KEY_SPACE)` with a held flag to avoid repeated application.

- **Crouch**  
  - C++: `Input::isKeyPressed(static_cast<Input::KEY>(crouch_key.get()))`.  
  - Lua: `Input.isKeyPressed(Input.KEY_ANY_CTRL)` (and `InputActions.isPressed("crouch")` when available). Same semantics (key held).

- **Mouse look**  
  - C++: `Input::getMouseDeltaPosition().x` (yaw), `.y` (pitch), with `Input::isMouseGrab()`.  
  - Lua: `Input.getMouseDX()`, `Input.getMouseDY()` (from `Input::getMouseDeltaPosition()` in `LuaBindings_Input.cpp`), and `Input.isMouseGrab()`.

- **Mouse grab**  
  - C++: does not set grab in the component; engine/app does.  
  - Lua: `Input.setMouseGrab(true)` in init so the player has focus; input is only read when `Input.isMouseGrab()` is true.

## Libraries used (Lua bindings)

- **Input** – `LuaBindings_Input.cpp`: `isKeyPressed`, `isKeyDown`, `isKeyUp`, `getMouseDX`, `getMouseDY`, `isMouseGrab`, `setMouseGrab`, `KEY_W`, `KEY_S`, `KEY_A`, `KEY_D`, `KEY_SPACE`, `KEY_ANY_CTRL`, `KEY_ANY_SHIFT`, etc.
- **InputActions** (optional) – `LuaInputSystem.cpp`: `register`, `isPressed`, `isDown`; used for the same logical actions as above when available.
- **Game** – `Game.getIFps()` for delta time.
- **Physics** – `Physics.getGravity()`, `Physics.getScale()`.
- **Object / Node / BodyDummy / ShapeCapsule / PlayerDummy / Mat4 / Vec3** – standard UNIGINE types and APIs used by the controller.

## Summary

- **Bindings**: Lua uses the same logical keys as FirstPersonController (W/S/A/D, Space, Ctrl), via `Input.KEY_*` from the UNIGINE Input API exposed in `LuaBindings_Input.cpp`.
- **Logic**: Movement and crouch use “key held” (`isKeyPressed`); jump uses a single application per press in Lua (C++ uses `isKeyDown` and adds once per frame while grounded, which effectively behaves as one shot after leave ground).
- **Mouse**: Same source of data (`getMouseDeltaPosition()` → `getMouseDX`/`getMouseDY`) and same condition (`isMouseGrab()`).

The Lua PlayerController is intentionally simpler (no run, no gamepad, no auto-stepping) but uses the same core bindings and Input API as the C++ FirstPersonController.

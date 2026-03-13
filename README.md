## Note: this will require sol2 & luajit libraries as I was unable to push the libraries to the top level.
- https://github.com/LuaJIT/LuaJIT
- https://github.com/ThePhD/sol2

##### I will work on fixing this
---

**Keep noted this is a first project and will have alot of bugs, please feel free to improve upon this codebase.**

---
# Lua Integration Features - Implementation Summary

This document summarizes all the new features added to the Lua component system.

## ✅ Implemented Features

### 1. Property Validation ✅
**Files:** `source/LuaPropertyValidation.h`, `source/LuaPropertyValidation.cpp`

**Features:**
- Type checking for properties
- Range constraints (min/max) for numeric types
- Step validation for numeric sliders
- Regex pattern validation for strings
- Enum/choice validation
- Automatic value clamping

**Usage:**
```lua
component("MyComponent", {
    properties = {
        health = { type = "float", default = 100.0, min = 0.0, max = 100.0 },
        name = { type = "string", pattern = "^[A-Za-z]+$" },
        weapon_type = { type = "string", choices = {"sword", "bow", "staff"} }
    }
})
```

### 2. Component Queries ✅
**Files:** `source/LuaComponentQueries.h`, `source/LuaComponentQueries.cpp`

**Features:**
- Find components by type
- Find components by tag
- Find components on specific nodes
- Find components within radius
- Query all components on a node

**Usage:**
```lua
-- In Lua
local enemies = findComponentsByTag("enemy")
local health_components = findComponentsByType("HealthComponent")
local component = findComponentOnNode(node, "WeaponSystem")
```

### 3. Component Templates ✅
**File:** `data/lua_components/ComponentTemplates.lua`

**Templates Provided:**
- `HealthComponent` - Health system with damage/healing
- `CharacterController` - Character movement
- `Interactable` - Interaction system
- `DamageDealer` - Damage dealing system
- `Spawner` - Object spawning
- `Rotator` - Continuous rotation
- `Oscillator` - Position oscillation
- `AutoDestroy` - Auto-destruction after delay

**Usage:**
```lua
-- Use a template
register_template("MyHealth", create_health_component_template)
```

### 4. Coroutine Helpers ✅
**Files:** `source/LuaCoroutineHelpers.h`, `source/LuaCoroutineHelpers.cpp`

**Features:**
- `WaitForSeconds(seconds)` - Wait for time
- `WaitUntil(condition)` - Wait until condition is true
- `StartCoroutine(func)` - Start a coroutine
- `Lerp(a, b, t)` - Linear interpolation
- `Tween(start, end, duration, on_update)` - Tweening helper

**Usage:**
```lua
function my_coroutine()
    WaitForSeconds(2.0)
    Log.message("2 seconds passed\n")
    
    WaitUntil(function() return some_condition end)
    Log.message("Condition met\n")
end

StartCoroutine(my_coroutine)
```

### 5. Input System ✅
**Files:** `source/LuaInputSystem.h`, `source/LuaInputSystem.cpp`

**Features:**
- Action mapping (keyboard, mouse, gamepad)
- Axis mapping
- Action states (pressed, down, released)
- Deadzone support
- Sensitivity control

**Usage:**
```lua
-- Register actions
InputActions.register("jump", {Input.KEY_SPACE}, {}, {})
InputActions.register("fire", {}, {Input.MOUSE_BUTTON_LEFT}, {})

-- Register axis
InputActions.registerAxis("move_horizontal", Input.KEY_D, Input.KEY_A)

-- Check in update
if InputActions.isDown("jump") then
    -- Jump
end

local move = InputActions.getAxis("move_horizontal")
```

### 6. Property Serialization ✅
**Files:** `source/LuaPropertySerialization.h`, `source/LuaPropertySerialization.cpp`

**Features:**
- Serialize component to JSON
- Deserialize JSON to component
- Save/load to files
- Serialize all components on a node

**Usage:**
```lua
-- Serialize
local json = serializeComponent(component_instance)
saveComponentToFile(component_instance, "save.json")

-- Deserialize (would need implementation)
loadComponentFromFile("save.json", component_instance)
```

### 7. Lua Profiler ✅
**Files:** `source/LuaProfiler.h`, `source/LuaProfiler.cpp`

**Features:**
- Function timing
- Call count tracking
- Min/max/average time
- Integration with Unigine profiler
- Reset functionality

**Usage:**
```lua
Profiler.begin("my_function")
-- ... code ...
Profiler.end("my_function")

local data = Profiler.getData("my_function")
Log.message("Avg time: " .. data.avg_time .. "\n")
```

## 🚧 Partially Implemented / Needs Integration

### 8. Auto-completion Data Export
**Status:** Structure created, needs LSP definition generation

**Needed:**
- Generate `.lua` stub files from C++ bindings
- Create LSP definition files
- Export function signatures
- Export type definitions

### 9. Lua Debugging Support
**Status:** Not yet implemented

**Needed:**
- ZeroBrane Studio integration
- VS Code Lua debugger integration
- Breakpoint support
- Hot-reload compatibility

### 10. Material/Shader Access
**Status:** Basic material bindings exist, needs runtime modification API

**Needed:**
- Runtime material property modification
- Shader parameter access
- Material instance creation

### 11. Animation System
**Status:** Not implemented

**Needed:**
- Animation playback control
- Animation state machine
- Blend trees
- Event callbacks

### 12. Pathfinding/Navigation
**Status:** Not implemented

**Needed:**
- Navigation mesh queries
- Pathfinding API
- Agent movement
- Obstacle avoidance

## Integration Status

All implemented features are integrated into `LuaSystem::initialize()`:
- ✅ Coroutine helpers registered
- ✅ Input system registered
- ✅ Profiler registered
- ✅ Component queries available via Lua functions
- ✅ Property serialization available via Lua functions

## Next Steps

1. **Complete Property Validation Integration**
   - Integrate validation into property sync
   - Add validation callbacks

2. **Complete Auto-completion Export**
   - Generate stub files
   - Create LSP definitions

3. **Add Debugging Support**
   - Integrate debugger protocol
   - Add breakpoint support

4. **Enhance Material Access**
   - Add runtime modification API
   - Add shader parameter access

5. **Add Animation System**
   - Basic animation playback
   - State machine support

6. **Add Pathfinding**
   - Navigation mesh queries
   - Pathfinding API

## Files Created

### C++ Source Files
- `source/LuaPropertyValidation.h/cpp`
- `source/LuaComponentQueries.h/cpp`
- `source/LuaCoroutineHelpers.h/cpp`
- `source/LuaInputSystem.h/cpp`
- `source/LuaPropertySerialization.h/cpp`
- `source/LuaProfiler.h/cpp`

### Lua Files
- `data/lua_components/ComponentTemplates.lua`

### Documentation
- `FEATURES_IMPLEMENTATION_SUMMARY.md` (this file)

## Usage Examples

See `data/lua_components/ComponentFeatures_Examples.lua` for comprehensive usage examples of all features.
---
# How to Use All Lua Integration Features

This guide explains how to use every feature of the Lua integration system in your Unigine project.

---

## 1. Lua components

### 1.1 Defining a component

Create a `.lua` file in `data/lua_components/`. Call `component()` to register it:

```lua
component("MyComponent", {
    description = "Optional short description",

    -- Lifecycle hooks: names of global Lua functions
    on_init    = "my_init",
    on_update  = "my_update",
    on_shutdown = "my_shutdown",           -- optional
    on_update_physics = "my_update_physics", -- optional
    on_swap    = "my_swap",                -- optional
    on_update_async_thread = "my_async",   -- optional
    on_update_sync_thread  = "my_sync",    -- optional

    -- Dependencies: load order
    requires = { "OtherComponent" },

    -- Extra Lua files to load with this component
    files = { "utils.lua" },

    properties = {
        speed = {
            type = "float",
            title = "Speed",
            default = 5.0,
            min = 0, max = 100,
            description = "Movement speed"
        },
        name = { type = "string", default = "unnamed" },
        enabled = { type = "toggle", default = true },
        color = { type = "vec4", default = {1,1,1,1} },
        mesh_path = { type = "file", default = "meshes/box.mesh" }
    }
})

function my_init(self)
    -- self.node: Node this component is on
    -- self.speed, self.name, etc.: property values (C++ → Lua)
    Log.message("[MyComponent] init on node %s\n", self.node:getName())
end

function my_update(self, dt)
    -- Changes to self.speed etc. sync back to C++ (bidirectional)
    self.speed = self.speed + 0.01
end

function my_shutdown(self)
    Log.message("[MyComponent] shutdown\n")
end
```

**Property types:** `int`, `float`, `bool` / `toggle`, `string`, `file`, `vec2`, `vec3`, `vec4` / `color`.

### 1.2 Using a component in the editor

1. Run the project and let the Lua system load components (or use **F5** / `lua_tool_reload`).
2. Add the generated C++ component (e.g. `Lua_MyComponent`) to a node via the Unigine Editor.
3. Adjust properties in the Properties panel. They appear as `PROP_PARAM` and are synced to Lua each frame.

### 1.3 Bidirectional property sync

- **C++ → Lua:** Each frame, `update()` copies `PROP_PARAM` values into `self` (e.g. `self.speed`).
- **Lua → C++:** After `on_update`, the generator reads `self.speed` etc. and writes them back into the C++ props.

So you can change `self.speed` in Lua and have it persist in the editor.

---

## 2. Input (InputActions)

### 2.1 Setup

Register actions and axes **before** using them (e.g. in `on_init` or a startup script). The input system is updated every frame automatically.

### 2.2 Actions

```lua
-- InputActions.register(name, keyboard_keys, mouse_buttons, gamepad_buttons)
-- Each argument is a Lua table of Unigine Input constants.

InputActions.register("jump", { Input.KEY_SPACE }, {}, {})
InputActions.register("fire", {}, { Input.MOUSE_BUTTON_LEFT }, {})
InputActions.register("interact", { Input.KEY_E }, {}, {})

-- With gamepad (when Input.GAMEPAD_BUTTON_* / GAMEPAD_AXIS_* are exposed)
-- InputActions.register("jump", { Input.KEY_SPACE }, {}, { Input.GAMEPAD_BUTTON_A })
```

**Keyboard:** `Input.KEY_W`, `Input.KEY_SPACE`, `Input.KEY_ESC`, `Input.KEY_F1`, etc.  
**Mouse:** `Input.MOUSE_BUTTON_LEFT`, `Input.MOUSE_BUTTON_RIGHT`, `Input.MOUSE_BUTTON_MIDDLE`.

### 2.3 Action state

```lua
-- Held this frame
if InputActions.isPressed("fire") then
    -- shooting
end

-- Just pressed (down this frame)
if InputActions.isDown("jump") then
    -- jump
end

-- Just released
if InputActions.isReleased("interact") then
    -- interact
end
```

### 2.4 Axes

```lua
-- registerAxis(name, positive_key, negative_key, gamepad_axis, sensitivity)
InputActions.registerAxis("move_x", Input.KEY_D, Input.KEY_A, -1, 1.0)
InputActions.registerAxis("move_y", Input.KEY_W, Input.KEY_S, -1, 1.0)

-- Gamepad: use Input.GAMEPAD_AXIS_LEFT_X, GAMEPAD_AXIS_LEFT_Y, etc. if exposed
-- InputActions.registerAxis("move_x", -1, -1, Input.GAMEPAD_AXIS_LEFT_X, 1.0)
```

```lua
local dx = InputActions.getAxis("move_x")
local dy = InputActions.getAxis("move_y")
```

Gamepad sticks and triggers use a 0.1 deadzone and override keyboard when active. If `Input.GAMEPAD_BUTTON_*` or `Input.GAMEPAD_AXIS_*` are not in the bindings, use the raw Unigine enum values from the SDK.

---

## 3. Animation (ObjectMeshSkinned)

`Animation` operates on `ObjectMeshSkinned` (or cast from `Object`).

### 3.1 Playback

```lua
local obj = Object.cast(node)  -- or your skinned object

Animation.play(obj, "idle")
Animation.play(obj, "walk", 1.2)           -- speed
Animation.play(obj, "run", 1.5, true)      -- speed, loop

Animation.stop(obj, "walk")
Animation.pause(obj, "run")
Animation.setTime(obj, "idle", 0.5)
```

### 3.2 Blending over time

```lua
-- Blend from "idle" to "walk" over 0.5 seconds
Animation.blend(obj, "idle", "walk", 0.5)
```

Weights are interpolated each frame; no extra code needed.

### 3.3 State

```lua
local state = Animation.getState(obj, "walk")
-- state.is_playing, state.current_time, state.duration, state.speed, state.loop
if state.is_playing then
    Log.message("time %.2f / %.2f\n", state.current_time, state.duration)
end
```

---

## 4. Pathfinding

### 4.1 Find path

```lua
local start = vec3(0, 0, 1)
local dest  = vec3(10, 5, 1)

local points = Pathfinding.findPath(start, dest)
-- or with agent radius:
local points = Pathfinding.findPath(start, dest, 0.5)

for i, p in ipairs(points) do
    -- p is vec3 waypoint
end
```

### 4.2 Walkability

```lua
local pos = vec3(1, 2, 0)
local ok = Pathfinding.isWalkable(pos)
local ok2 = Pathfinding.isWalkable(pos, 0.5)
```

### 4.3 Nearest walkable

```lua
local snap = Pathfinding.getNearestWalkable(pos)
local snap2 = Pathfinding.getNearestWalkable(pos, 0.5)
```

### 4.4 Custom PathRoute

```lua
local path = PathRoute.create(0.5)
local points_table = Pathfinding.findPathWithPath(path, start, dest)
-- points_table: array of vec3 or nil
```

---

## 5. Coroutines and timing

### 5.1 StartCoroutine

```lua
StartCoroutine(function()
    Log.message("start\n")
    WaitForSeconds(2.0)()   -- yield for 2 seconds (note the ()!)
    Log.message("2 sec later\n")
    WaitUntil(function() return some_flag end)()
    Log.message("condition met\n")
end)
```

Coroutines are resumed every frame automatically. No manual `coroutine.resume`.

### 5.2 WaitForSeconds

`WaitForSeconds(seconds)` returns a **function**. Call it inside a coroutine to yield:

```lua
WaitForSeconds(1.5)()
```

### 5.3 WaitUntil

`WaitUntil(condition)` returns a function. The `condition` is a Lua function that returns a boolean. Call the result to yield until it’s true:

```lua
WaitUntil(function() return player_ready end)()
```

### 5.4 Lerp and Tween

```lua
local x = Lerp(0, 100, 0.5)           -- 50
local v = Lerp(vec3(0,0,0), vec3(1,1,1), 0.5)

-- Tween: blocks (no yield), calls on_update with interpolated value
Tween(0, 1, 2.0, function(t)
    node:setWorldScale(vec3(t, t, t))
end)
```

`Tween` runs to completion in one go. For frame-based tweens inside a coroutine, use `WaitForSeconds` + `Lerp` in `on_update`.

---

## 6. Save / load node state

Persist all Lua component state on a node to JSON and restore it.

### 6.1 Save

```lua
local ok = saveNodeToFile(node, "save/my_node.json")
```

### 6.2 Load

```lua
local ok = loadNodeFromFile(node, "save/my_node.json")
```

Paths are project-relative. Ensures the node has the right Lua components before loading.

---

## 7. Component queries

### 7.1 By type

```lua
local list = findComponentsByType("HealthComponent")
for i = 1, #list do
    local ci = list[i]
    local node = ci.node
    local instance = ci.instance  -- Lua table (self) for that instance
end
```

If the API returns a C++ container, use the iteration style that matches (e.g. `pairs` or indexed loop as appropriate).

### 7.2 By tag

Set `self.tag = "enemy"` (or similar) in your component. Then:

```lua
local enemies = findComponentsByTag("enemy")
```

### 7.3 On a specific node

```lua
local ci = findComponentOnNode(node, "WeaponSystem")
if ci then
    local inst = ci.instance
    -- use inst
end
```

### 7.4 Serialize a single component

```lua
local ci = findComponentOnNode(node, "MyComponent")
if ci then
    local json_str = serializeComponent(ci)
    saveComponentToFile(ci, "save/component.json")
end
```

`saveNodeToFile` / `loadNodeFromFile` handle all Lua components on a node; `serializeComponent` / `saveComponentToFile` work on one instance.

---

## 8. Events (subscribe / emit)

### 8.1 Subscribe

```lua
subscribe("player_died", function()
    Log.message("player died\n")
end)

subscribe("score", function(points) end, 10)  -- priority

local id = subscribeWithId("respawn", "my_handler", function() end)
```

### 8.2 Emit

```lua
emit("player_died")
emit("score", 100)
emitDeferred("heavy_event", data)  -- deferred
```

### 8.3 Unsubscribe

```lua
unsubscribe("player_died", callback)
unsubscribeById("respawn", "my_handler")
unsubscribeAll("score")
```

### 8.4 Helpers

```lua
hasListeners("player_died")
getListenerCount("player_died")
clearEvents("player_died")
clearEvents()  -- all
```

---

## 9. Profiler

Wrap Lua code to measure it:

```lua
Profiler.begin("my_section")
-- expensive code
Profiler.end("my_section")

local data = Profiler.getData("my_section")
-- data.name, data.total_time, data.call_count, data.avg_time, data.min_time, data.max_time

Profiler.reset()
```

Use `lua_tool_profiler_stats` in the console to print all profiler stats.

---

## 10. MaterialRuntime

Create and apply runtime material instances, and change parameters:

```lua
local mat = MaterialRuntime.createInstance("mesh_base")
MaterialRuntime.applyToObject(obj, mat)
MaterialRuntime.applyToObject(obj, mat, 0)  -- surface index

local m = MaterialRuntime.getFromObject(obj)
MaterialRuntime.getFromObject(obj, 0)

MaterialRuntime.setParameter(mat, "albedo_color", vec4(1,0,0,1))
MaterialRuntime.setParameter(mat, "roughness", 0.5)
local f = MaterialRuntime.getParameterFloat(mat, "roughness")
local v3 = MaterialRuntime.getParameterVec3(mat, "param")
local v4 = MaterialRuntime.getParameterVec4(mat, "param")
```

---

## 11. Logging

```lua
Log.message("info: %s\n", msg)
Log.warning("warn: %d\n", n)
Log.error("err: %s\n", err)
```

---

## 12. Physics (Body, Shapes)

Use **BodyRigid** for mass and forces. Base **Body** supports shapes, enable/disable, and render visualizer.

### 12.1 Body

```lua
local body = Body.createBody(Body.BODY_RIGID)
body:setObject(obj)
body:addShape(shape)
body:removeShape(shape)
body:setEnabled(true)
ok = body:isEnabled()
body:renderVisualizer()
```

### 12.2 BodyRigid

```lua
local rigid = BodyRigid.create()
rigid:setMass(10.0)
rigid:addForce(vec3(0, 0, 100))
rigid:addImpulse(vec3(0,0,0), vec3(0,0,5))
rigid:setFrozen(false)
```

### 12.3 Shapes

```lua
local sphere = ShapeSphere.create(0.5)
local box = ShapeBox.create(vec3(1,1,1))
local capsule = ShapeCapsule.create(0.5, 2.0)
local convex = ShapeConvex.create()

sphere:setTransform(Mat4(...))
sphere:setEnabled(true)
sphere:isEnabled()
```

Attach shapes to a body with `body:addShape(shape)`.

---

## 13. Console commands

Open the in-game console (`~` or `F1`), then:

| Command | Description |
|--------|-------------|
| `lua_tool_log_init [0\|1]` | Init logs |
| `lua_tool_log_scan [0\|1]` | File scan logs |
| `lua_tool_log_load [0\|1]` | File load logs |
| `lua_tool_log_parse [0\|1]` | Parse logs |
| `lua_tool_log_registration [0\|1]` | Registration logs |
| `lua_tool_log_generation [0\|1]` | Codegen logs |
| `lua_tool_log_properties [0\|1]` | Property logs |
| `lua_tool_log_pending [0\|1]` | Pending-component warnings |
| `lua_tool_log_all [0\|1]` | All logs |
| `lua_tool_log_status` | Current log flags |
| `lua_tool_reload` | Reload all Lua (like F5) |
| `lua_tool_components` | List registered components |
| `lua_tool_autocomplete [dir]` | Generate LSP/stub files (default `data/`) |
| `lua_tool_debugger_enable [port]` | Enable debugger (default 8172) |
| `lua_tool_debugger_disable` | Disable debugger |
| `lua_tool_profiler_reset` | Reset profiler |
| `lua_tool_profiler_stats` | Print profiler stats |

Log settings are stored in `data/configs/lua_system.config`.

---

## 14. Autocomplete / IDE

Run:

```
lua_tool_autocomplete
lua_tool_autocomplete data/autocomplete
```

This generates `unigine_lsp.lua`, `unigine_stubs.lua`, and `unigine_emmy.lua` for LSP/EmmyLua-style autocomplete.

---

## 15. Debugger

```text
lua_tool_debugger_enable
lua_tool_debugger_enable 9999
lua_tool_debugger_disable
```

Use a MobDebug- or DAP-compatible client to attach to the given port.

---

## 16. Other bindings

The project also exposes many Unigine APIs to Lua, including:

- **Math:** `vec2`, `vec3`, `vec4`, `Mat4`, `quat`, `Vec2`, `Vec3`, `Vec4`, etc.
- **Nodes:** `Node`, `NodePtr`, get/set world position, rotation, scale, parenting.
- **Objects:** `Object.cast(node)`, mesh operations.
- **Physics:** `Physics`, `PhysicsIntersection`, `PhysicsIntersectionNormal`, rays, etc.
- **Lights:** `Light`, `LightOmni`, `LightProj`, `LightWorld`, etc.
- **Sound:** `Sound`, `SoundSource`, `Sounds`.
- **Materials:** `Material`, `MaterialRuntime`.
- **World:** `World`, node iteration.
- **Game:** `Game.getIFps()`, `Game.getTime()`, etc.
- **Visualizer:** `Visualizer` for debug drawing.

Refer to the generated stubs and binding sources for full signatures.

---

## Quick reference

| Feature | API |
|--------|-----|
| Components | `component("Name", { ... })`, `on_init` / `on_update`, `self.node`, `self.prop` |
| Input | `InputActions.register`, `isPressed` / `isDown` / `isReleased`, `registerAxis`, `getAxis` |
| Animation | `Animation.play` / `stop` / `pause` / `blend` / `getState` |
| Pathfinding | `Pathfinding.findPath`, `isWalkable`, `getNearestWalkable` |
| Coroutines | `StartCoroutine`, `WaitForSeconds(n)()`, `WaitUntil(fn)()` |
| Helpers | `Lerp`, `Tween` |
| Save/load | `saveNodeToFile`, `loadNodeFromFile` |
| Queries | `findComponentsByType`, `findComponentsByTag`, `findComponentOnNode` |
| Events | `subscribe`, `emit`, `unsubscribe`, etc. |
| Profiler | `Profiler.begin` / `end`, `getData`, `reset` |
| Materials | `MaterialRuntime.createInstance`, `applyToObject`, `setParameter`, … |
| Logging | `Log.message` / `warning` / `error` |
| Console | `lua_tool_*` commands |

---

For more detail, see `CONSOLE_COMMANDS.md`, `CODE_REVIEW_AND_FEATURES.md`, and `FEATURES_AND_FIXES_COMPLETE.md`.

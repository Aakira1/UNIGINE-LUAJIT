# Code Review & Feature Roadmap

## 1. Wrapper Architecture Review

### 1.1 Overview

The **Lua component wrapper** bridges Unigine’s C++ Component System with Lua:

1. **Lua declaration**  
   `component("Name", { ... })` in `data/lua_components/*.lua` registers a `LuaComponentDescriptor` (name, description, lifecycle hooks, properties, `requires`, `files`).

2. **Code generation**  
   `LuaComponentGenerator::generateAllComponents()` produces, per component:
   - `Lua_<Name>.h` – `ComponentBase` subclass with `PROP_PARAM`-driven properties and `COMPONENT_*` lifecycle hooks.
   - `Lua_<Name>.cpp` – `init()` / `update()` / etc. that:
     - Build a `sol::table` `lua_instance` with `node` + properties (synced from C++ `PROP_PARAM`).
     - Call `registerComponentInstance(node, desc, lua_instance)`.
     - Invoke Lua lifecycle by name (e.g. `fall_sound_init`, `fall_sound_update`).

3. **Registration**  
   `_register_all.h` includes all `Lua_*.h`. Each `Lua_*.cpp` uses `REGISTER_COMPONENT(Lua_<Name>)` so Unigine picks them up as components.

4. **Property flow**  
   C++ → Lua only: each `update()` (and `init()`) overwrites `lua_instance["prop"]` from `PROP_PARAM` values. Lua never writes back into Unigine properties.

### 1.2 What Works Well

- Clear split: Lua defines behavior, C++ provides Unigine integration and property UI.
- Lifecycle coverage: init, update, postUpdate, updatePhysics, swap, shutdown (plus async/sync thread hooks).
- Multi-file components (`files`), dependencies (`requires`), and hot-reload–aware generation.
- Shared `LuaComponentInstance` (node, descriptor, `instance` table, lifecycle refs) used by both generated wrappers and query/event systems.

### 1.3 Gaps / Issues

| Issue | Location | Impact |
|-------|----------|--------|
| **Duplicate Lua `update` calls** | Generated `update()` + `LuaComponentLifecycle::update()` | Both run. Generated wrapper already calls Lua `on_update`; lifecycle also iterates `active_components` and calls `on_update`. Result: every Lua update runs twice per frame. |
| **`LuaInputSystem::update()` never called** | `LuaSystem::update()` | `action_states` / `action_states_prev` are never updated. `isDown` / `isReleased` stay false. |
| **Property sync is one-way** | Generated init/update | Lua can change `self.xxx` but C++ never reads it back. Editor / runtime stay driven only by C++ params. |
| **Heavy init logging** | Generated `init()` | `Log::message("===...")` etc. on every component init. Should be behind `log_init` (or similar). |
| **`consoleListComponents` iteration** | `LuaSystem.cpp` | Loop is correct; ensure `components` is non-null before use. |

---

## 2. Wrapper Outline Summary

```
┌─────────────────────────────────────────────────────────────────────────┐
│  Lua (data/lua_components/*.lua)                                         │
│  component("Name", { properties = {...}, on_init = "...", ... })         │
│  function foo_init(self) ... end                                         │
│  function foo_update(self, dt) ... end                                   │
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                    parse (registerLuaComponent) + scan
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│  LuaComponentDescriptor (name, properties, fn_on_*, requires, files)     │
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                    LuaComponentGenerator::generateAllComponents
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│  Lua_<Name> : ComponentBase                                              │
│  - PROP_PARAM(...) for each property                                     │
│  - COMPONENT_INIT(init); COMPONENT_UPDATE(update); ...                   │
│  - sol::table lua_instance; bool lua_ready;                              │
│  init(): create table, sync props, registerInstance, call Lua init       │
│  update(): sync props C++→Lua, call Lua update                           │
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                    REGISTER_COMPONENT + Unigine runtime
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│  LuaComponentInstance in active_components (node, descriptor, instance)  │
│  Used by: lifecycle update, findComponentsByType/Tag/Node, serialization │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 3. Recommended Fixes (Before New Features)

1. **Remove duplicate Lua update** ✅ **DONE**  
   - `LuaComponentLifecycle::update()` no longer calls `on_update`. Only generated `Lua_*::update()` invokes Lua update. Lifecycle still runs `on_init` for any Lua-only instances that set `initialized=false`.

2. **Call `LuaInputSystem::update()` each frame** ✅ **DONE**  
   - `LuaSystem::update()` now calls `LuaInputSystem::get().update()` before the lifecycle update, so `isDown` / `isReleased` work.

3. **Tone down generated init logging**  
   - Gate “========================================” / “C++ init() CALLED!” etc. behind `LuaSystem::get()->getLogInit()` (or a dedicated `log_generation`), or remove.

4. **Clarify lifecycle vs generated**  
   - Document when instances come from generated C++ vs future Lua-only path, and which system is responsible for init/update.

---

## 4. Features to Add / Extend

### 4.1 High Value, Moderate Effort

| Feature | Description |
|--------|-------------|
| **Lua → C++ property sync** | After calling Lua `on_update`, read back `lua_instance["prop"]` for supported types and write into `PROP_PARAM` (or a parallel store). Enables Lua-driven tweaks that persist in the editor. |
| **`InputActions.update` wiring** | Implement the fix above so `isDown` / `isReleased` work. |
| **Animation blending over time** | In `LuaAnimationSystem::blendAnimations`, use a coroutine or per-frame callback to lerp layer weights over `blend_time` instead of snapping. |
| **Async pathfinding** | Optionally poll `PathRoute::isReady()` over multiple frames (or a job) instead of a single immediate check in `findPath` / `getNearestWalkable`. |

### 4.2 Tooling & DX

| Feature | Description |
|--------|-------------|
| **Debugger socket server** | MobDebug/DAP-compatible listener so IDEs can attach, set breakpoints, and resume. Build on existing `LuaDebugger` hooks. |
| **Autocomplete / LSP** | Extend `LuaAutocompleteGenerator` (and any stub output) so generated APIs + `component` props are visible in IDE hover/signature help. |
| **`component` schema** | Document or enforce (e.g. via a small Lua schema) allowed keys: `properties`, `on_init`, `on_update`, etc., and property `type`/`default` to reduce mistakes. |

### 4.3 Engine Bindings

| Feature | Description |
|--------|-------------|
| **Physics** | `ShapeConvex`, `Body::setEnabled`/`getEnabled`, `removeShape`, `setMass`/`getMass`, `setDensity`/`getDensity`. |
| **Objects** | More `ObjectMeshSkinned` / `ObjectParticles` (or similar) surface APIs, per-mesh helpers. |
| **Nodes** | Helpers for creation, reparenting, visibility, enabled state, layer masks. |

### 4.4 Gameplay

| Feature | Description |
|--------|-------------|
| **Gamepad in InputActions** | Implement `Input::isGamepadButtonPressed` (or equivalent) in `isActionPressed` and use `gamepad_axis` in `getAxis`. |
| **`StartCoroutine`** | Replace placeholder with logic that creates a coroutine from `coroutine_func` and runs it via your existing coroutine helpers (e.g. `WaitForSeconds` / `WaitUntil`). |
| **Tags on instances** | Store tags (e.g. `self.tags = {"enemy","elite"}`) and extend `findComponentsByTag` to filter by multiple tags or tag expressions. |

### 4.5 Persistence & Editor

| Feature | Description |
|--------|-------------|
| **Save/load node state** | Wrap `serializeNodeComponents` / `deserializeNodeComponents` in simple `saveNodeToFile(path)` / `loadNodeFromFile(path)` Lua API. |
| **`setDefaultValueXml`** | Already stubbed; ensure it’s used where property defaults are applied from XML (e.g. creation flow) and that it supports all relevant types. |

---

## 5. Summary

- **Wrapper**: Lua `component()` → descriptor → generated `Lua_*` C++ components → `LuaComponentInstance`. Flow is clear; main problems are duplicate updates, missing `LuaInputSystem::update`, and one-way property sync.
- **Fixes first**: Deduplicate Lua update, wire `LuaInputSystem::update`, reduce init logging, document lifecycle vs generated.
- **Next features**: Lua→C++ prop sync, input fix, animation blend-over-time, async pathfinding, then debugger server, autocomplete, extra bindings, and gameplay helpers (gamepad, coroutines, tags).

Use this as a roadmap: address Section 3, then pick from Section 4 by priority.

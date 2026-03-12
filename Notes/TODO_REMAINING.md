# Remaining TODO Items

**Last updated:** After implementing remaining features (deserialization, debugger, autocomplete).

## ✅ Recently Completed

### 1. **Lights Bindings** ✅
**File:** `source/LuaBindings_Lights.cpp`  
**Status:** **DONE** – Full bindings for LightOmni, LightProj, LightWorld, VoxelProbe, EnvironmentProbe, PlanarProbe, LensFlare, enums, and base Light methods.

### 2. **Animation System** ✅
**File:** `source/LuaAnimationSystem.cpp`  
**Status:** **DONE** – Wired to Unigine `ObjectMeshSkinned` layer API (play, stop, pause, setTime, blend, getState).

### 3. **Pathfinding Walkability** ✅
**File:** `source/LuaPathfinding.cpp`  
**Status:** **DONE** – `isWalkable()` uses `Navigation::inside3D()`; `getNearestWalkable()` queries navigation areas and PathRoute.

### 4. **Property Serialization – Deserialization** ✅
**File:** `source/LuaPropertySerialization.cpp`  
**Status:** **DONE** – `deserializeNodeComponents()` parses JSON (Lua `json.decode` or Unigine `Json::parse` fallback), matches components by name, and applies properties (including vec3).

### 5. **Lua Debugger – Breakpoints & Hooks** ✅
**File:** `source/LuaDebugger.cpp`  
**Status:** **DONE** – Breakpoint storage, `lua_sethook` (LINE/CALL/RET), breakpoint hit logging, step / step over / step out logic. No blocking pause; full MobDebug/DAP server not implemented.

### 6. **Autocomplete – Dynamic Introspection** ✅
**File:** `source/LuaAutocompleteGenerator.cpp`  
**Status:** **DONE** – `extractSignatures()` walks `_G`, collects functions and table “types” with methods (depth 2), and uses that for LSP/stub/EmmyLua generation. Fallback to hardcoded entries if none found.

---

## 🟡 Optional / Future

### 7. **Debugger – Full IDE Integration**
- MobDebug/DAP **socket server** (listen, handle protocol).
- Blocking pause with external “continue” (e.g. separate thread or debugger client).

### 8. **Physics / Object Bindings (Low Priority)**
- **Physics:** `ShapeConvex`, `Shape::setMass`/`getMass`, `setDensity`/`getDensity`, `Body::removeShape`, `Body::setEnabled`/`getEnabled`.
- **Objects:** More ObjectMeshSkinned / ObjectParticles (or similar) methods, surface-specific APIs.

### 9. **Animation Blending Over Time**
- **File:** `source/LuaAnimationSystem.cpp`  
- **Current:** `blendAnimations()` applies final weights only.  
- **Enhancement:** Use coroutine or update callback to lerp layer weights over `blend_time`.

---

## Summary

All previously tracked “remaining” items are implemented. Optional follow-ups: full debugger server, extra bindings, and time-based animation blending.

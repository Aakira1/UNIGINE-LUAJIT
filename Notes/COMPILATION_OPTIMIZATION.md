# Compilation Speed Optimization Guide

## Current Issues
- Full rebuild takes ~1:38 minutes
- Precompiled headers are disabled
- `CommonIncludes.h` includes many heavy headers
- Every binding file includes multiple UNIGINE headers

## Quick Wins (No Project File Changes)

### 1. Use Incremental Builds
Instead of "Rebuild All", use "Build" (F7) for incremental compilation. Only changed files will recompile.

### 2. Enable Precompiled Headers (Recommended)
Edit `source/unigine_project_5.vcxproj`:

1. Find the `<ClCompile>` section in `<ItemDefinitionGroup>` for Debug|x64
2. Change:
   ```xml
   <PrecompiledHeader>NotUsing</PrecompiledHeader>
   ```
   To:
   ```xml
   <PrecompiledHeader>Use</PrecompiledHeader>
   <PrecompiledHeaderFile>pch.h</PrecompiledHeaderFile>
   ```

3. For each `<ClCompile>` entry, change:
   ```xml
   <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">NotUsing</PrecompiledHeader>
   ```
   To:
   ```xml
   <PrecompiledHeader Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">Use</PrecompiledHeader>
   ```

4. Update `pch.h` to include the most common headers:
   ```cpp
   #pragma once
   #include "CommonIncludes.h"  // This includes sol.hpp and all UNIGINE headers
   ```

5. Make sure `pch.h` is the FIRST include in each .cpp file:
   ```cpp
   #include "pch.h"  // Must be first
   #include "LuaAPIBindings.h"
   // ... other includes
   ```

### 3. Optimize Include Order
- Put `pch.h` first (if using PCH)
- Put project headers second
- Put system headers last

### 4. Reduce Template Instantiation
The explicit types we added (replacing `auto`) can actually help with compilation speed by reducing template deduction overhead, but they increase code size slightly.

## Expected Improvements
- **With PCH enabled**: Full rebuild should drop to ~30-45 seconds
- **Incremental builds**: Should be < 5 seconds for single file changes
- **Parallel compilation**: Already enabled with `/MP8`

## Alternative: Use Unity Builds (Advanced)
For even faster compilation, consider unity builds where multiple .cpp files are combined. This requires more project configuration changes.

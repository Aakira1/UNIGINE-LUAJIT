# Lua System Console Commands

All debugging and tool commands are accessible through the **Unigine in-game console** (press `~` or `F1` to open). These are native Unigine console commands registered using `Console::addCommand()`. All commands are prefixed with `lua_tool_` for easy discovery and organization.

## Log Control Commands

Control the verbosity of different log categories. These are the most commonly used commands. **All log settings persist across runs** - your preferences are automatically saved and restored.

**`lua_tool_log_init [0/1]`** - Toggle initialization logs  
**`lua_tool_log_scan [0/1]`** - Toggle file scanning logs  
**`lua_tool_log_load [0/1]`** - Toggle file loading logs  
**`lua_tool_log_parse [0/1]`** - Toggle component parsing logs  
**`lua_tool_log_registration [0/1]`** - Toggle component registration logs  
**`lua_tool_log_generation [0/1]`** - Toggle code generation logs  
**`lua_tool_log_properties [0/1]`** - Toggle property file logs  
**`lua_tool_log_pending [0/1]`** - Toggle pending component warnings  
**`lua_tool_log_all [0/1]`** - Toggle all logs at once  
**`lua_tool_log_status`** - Show current log settings

**Usage:**
- `lua_tool_log_init` - Show current state
- `lua_tool_log_init 0` - Turn off (saved automatically)
- `lua_tool_log_init 1` - Turn on (saved automatically)

**Persistence:**
- Settings are automatically saved to `data/configs/lua_system.config` when changed
- Your preferences persist across editor sessions and game runs
- If the config file doesn't exist, default settings are used

**Default Settings (used on first run):**
- `log_init`: ON
- `log_scan`: ON
- `log_load`: OFF (too verbose)
- `log_parse`: OFF (too verbose)
- `log_registration`: ON
- `log_generation`: ON
- `log_properties`: OFF (only when creating/updating)
- `log_pending`: ON

## Tool Commands

### Autocomplete Generation

**`lua_tool_autocomplete [output_dir]`**
- Generates autocomplete files for the Lua API
- Creates three files: `unigine_lsp.lua`, `unigine_stubs.lua`, and `unigine_emmy.lua`
- **Usage:**
  - `lua_tool_autocomplete` - Generates files in `data/` directory (default)
  - `lua_tool_autocomplete data/autocomplete` - Generates files in specified directory
- **Output:** LSP definitions, Lua stubs, and EmmyLua annotations for IDE autocomplete support

### Debugger Controls

**`lua_tool_debugger_enable [port]`**
- Enables the Lua debugger for remote debugging
- **Usage:**
  - `lua_tool_debugger_enable` - Enables debugger on default port 8172
  - `lua_tool_debugger_enable 9999` - Enables debugger on custom port
- **Note:** Requires MobDebug or DAP protocol support

**`lua_tool_debugger_disable`**
- Disables the Lua debugger
- **Usage:** `lua_tool_debugger_disable`

### Profiler Controls

**`lua_tool_profiler_reset`**
- Resets all Lua profiler statistics
- Clears accumulated timing data and call counts
- **Usage:** `lua_tool_profiler_reset`

**`lua_tool_profiler_stats`**
- Prints detailed profiler statistics to the console
- Shows for each profiled function:
  - Total call count
  - Total time spent
  - Average time per call
  - Minimum time
  - Maximum time
- **Usage:** `lua_tool_profiler_stats`
- **Example Output:**
  ```
  === Lua Profiler Statistics ===
  MyFunction: calls=150, total=45.230ms, avg=0.302ms, min=0.100ms, max=2.500ms
  ```

### Script Management

**`lua_tool_reload`**
- Reloads all Lua scripts (hot reload)
- Useful for development when making changes to Lua component files
- **Usage:** `lua_tool_reload`
- **Note:** Equivalent to pressing F5 in-game

### Component Information

**`lua_tool_components`**
- Lists all registered Lua components
- Shows the name of each component that has been loaded
- **Usage:** `lua_tool_components`
- **Example Output:**
  ```
  === Registered Lua Components (5) ===
    - GameManager
    - HealthSystem
    - EnemyAI
    - CollectibleItem
    - WeaponSystem
  ```

## Command Organization

All commands are organized under the `lua_tool_` prefix:
- Type `lua_tool_` in the console and use tab completion to see all available commands
- Commands are grouped by functionality (tools, logs)

## Usage Tips

1. **Tab Completion:** Use tab to autocomplete command names in the console
2. **Help:** Type a command name without arguments to see its description
3. **Log Settings Persistence:** 
   - Configure your preferred log levels once with `lua_tool_log_*` commands
   - Settings are automatically saved and will persist across sessions
   - Use `lua_tool_log_status` to check your current settings
4. **Development Workflow:**
   - Use `lua_tool_reload` frequently during development
   - Use `lua_tool_profiler_stats` to check performance
   - Use `lua_tool_components` to verify components are loaded
   - Use `lua_tool_autocomplete` to generate IDE support files
   - Configure log verbosity once - it will be remembered

## Integration with F5 Hot Reload

The `lua_tool_reload` command provides the same functionality as pressing F5 in-game. Both trigger a full reload of all Lua scripts.

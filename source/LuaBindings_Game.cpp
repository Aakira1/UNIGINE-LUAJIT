#include "CommonIncludes.h"
#include "LuaAPIBindings.h"

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

/// <summary>
/// Registers the Game API in the provided Lua state by creating a 'Game' table and binding Game functions (getIFps, getTime, getScale, setScale).
/// </summary>
/// <param name="lua">Reference to the sol::state representing the Lua environment where the Game table and functions will be registered. The function modifies this Lua state.</param>
void Unigine::LuaBindings::registerGameAPI(sol::state& lua)
{
    sol::table game_table = lua.create_table("Game");
    game_table["getIFps"] = &Game::getIFps;
    game_table["getTime"] = &Game::getTime;
    game_table["getScale"] = &Game::getScale;
    game_table["setScale"] = &Game::setScale;

    // Player (camera) management. Guard null to avoid UniginePtr !isNull() assertion.
    game_table["setPlayer"] = sol::overload(
        [](const PlayerPtr& player) { if (player) Game::setPlayer(player); },
        [](const PlayerDummyPtr& player) { if (player) Game::setPlayer(PlayerPtr(player)); }
    );
    game_table["getPlayer"] = []() { return Game::getPlayer(); };

    game_table.set(sol::meta_function::to_string, []() {
        return "Game API Table";
        });
}
#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include "LuaEventSystem.h"

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

/// <summary>
/// Registers the Event API in the provided Lua state by creating an 'Event' table with all event system functions.
/// </summary>
/// <param name="lua">Reference to the sol::state representing the Lua environment where the Event table and functions will be registered.</param>
void Unigine::LuaBindings::registerEventInputs(sol::state& lua)
{
	lua["Event"] = lua.create_table();
	
	// Core subscription functions
	lua["Event"]["subscribe"] = sol::overload(
		[](const std::string& event_name, sol::function callback) {
			LuaEventSystem::Subscribe(event_name, callback, 0);
		},
		[](const std::string& event_name, sol::function callback, int priority) {
			LuaEventSystem::Subscribe(event_name, callback, priority);
		}
	);
	
	lua["Event"]["subscribeOnce"] = sol::overload(
		[](const std::string& event_name, sol::function callback) {
			LuaEventSystem::SubscribeOnce(event_name, callback, 0);
		},
		[](const std::string& event_name, sol::function callback, int priority) {
			LuaEventSystem::SubscribeOnce(event_name, callback, priority);
		}
	);
	
	lua["Event"]["subscribeWithId"] = sol::overload(
		[](const std::string& event_name, const std::string& id, sol::function callback) {
			LuaEventSystem::SubscribeWithId(event_name, id, callback, 0);
		},
		[](const std::string& event_name, const std::string& id, sol::function callback, int priority) {
			LuaEventSystem::SubscribeWithId(event_name, id, callback, priority);
		}
	);
	
	// Unsubscribe functions
	lua["Event"]["unsubscribe"] = [](const std::string& event_name, sol::function callback) {
		return LuaEventSystem::Unsubscribe(event_name, callback);
	};
	
	lua["Event"]["unsubscribeById"] = [](const std::string& event_name, const std::string& id) {
		return LuaEventSystem::UnsubscribeById(event_name, id);
	};
	
	lua["Event"]["unsubscribeAll"] = [](const std::string& event_name) {
		LuaEventSystem::UnsubscribeAll(event_name);
	};
	
	// Emit functions
	lua["Event"]["emit"] = &LuaEventSystem::Emit;
	lua["Event"]["emitDeferred"] = &LuaEventSystem::EmitDeferred;
	
	// Clear functions
	lua["Event"]["clear"] = sol::overload(
		[]() { LuaEventSystem::Clear(); },
		[](const std::string& event_name) { LuaEventSystem::ClearEvent(event_name); }
	);
	
	// Utility functions
	lua["Event"]["hasListeners"] = &LuaEventSystem::HasListeners;
	lua["Event"]["getListenerCount"] = &LuaEventSystem::GetListenerCount;
	lua["Event"]["getEmitCount"] = &LuaEventSystem::GetEmitCount;
	
	// Enable/disable functions
	lua["Event"]["enableListener"] = [](const std::string& event_name, const std::string& id, bool enable) {
		LuaEventSystem::EnableListener(event_name, id, enable);
	};
	
	lua["Event"]["enableAllListeners"] = [](const std::string& event_name, bool enable) {
		LuaEventSystem::EnableAllListeners(event_name, enable);
	};
	
	// Statistics functions
	lua["Event"]["printStats"] = &LuaEventSystem::PrintStatistics;
	lua["Event"]["resetStats"] = &LuaEventSystem::ResetStatistics;

	LuaSystem* ls = LuaSystem::get();
	if (ls && ls->getLogInit())
		Log::message("[Event Inputs Registered]");
}
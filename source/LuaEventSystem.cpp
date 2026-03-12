#include "CommonIncludes.h"
#include "LuaEventSystem.h"
#include <algorithm>
#include <vector>
#include <unordered_map>

using namespace Unigine;

// Define static members
std::unordered_map<std::string, Vector<EventListener>> LuaEventSystem::event_listeners;
Vector<DeferredEvent> LuaEventSystem::deferred_events;
std::unordered_map<std::string, int> LuaEventSystem::event_emit_count;
std::unordered_map<std::string, int> LuaEventSystem::event_listener_count;
std::unordered_map<std::string, float> LuaEventSystem::event_total_time;

// Configuration defaults
bool LuaEventSystem::enable_logging = true;
bool LuaEventSystem::enable_statistics = true;
bool LuaEventSystem::auto_cleanup_invalid = true;

void Unigine::LuaEventSystem::CleanupInvalidListeners(const std::string& event_name)
{
	std::unordered_map<std::string, Vector<EventListener>>::iterator it = event_listeners.find(event_name);
	if (it == event_listeners.end()) {
		return;
	}

	Vector<EventListener>& listeners = it->second;
	
	// Remove invalid callbacks
	for (int i = listeners.size() - 1; i >= 0; i--) {
		if (!listeners[i].callback.valid()) {
			listeners.remove(i);
		}
	}
}

void Unigine::LuaEventSystem::EnsureSorted(Vector<EventListener>& listeners)
{
	// Check if sorting is needed
	bool needs_sort = false;
	for (int i = 0; i < listeners.size(); i++) {
		if (listeners[i].needs_sort) {
			needs_sort = true;
			break;
		}
	}
	
	if (!needs_sort && listeners.size() > 1) {
		// Check if already sorted
		for (int i = 1; i < listeners.size(); i++) {
			if (listeners[i-1].priority < listeners[i].priority) {
				needs_sort = true;
				break;
			}
		}
	}
	
	if (needs_sort) {
		// Sort by priority (higher priority first)
		std::sort(listeners.begin(), listeners.end(), 
			[](const EventListener& a, const EventListener& b) {
				return a.priority > b.priority;
			});
		
		// Clear sort flags
		for (int i = 0; i < listeners.size(); i++) {
			listeners[i].needs_sort = false;
		}
	}
}

void Unigine::LuaEventSystem::Subscribe(const std::string& event_name, sol::function callback, int priority)
{
	if (!callback.valid()) {
		Log::warning("[Lua-Event] Invalid callback for event '%s'\n", event_name.c_str());
		return;
	}

	Vector<EventListener>& listeners = event_listeners[event_name];
	
	// Check if we need to sort after adding
	bool needs_sort = listeners.size() > 0 && priority > listeners[0].priority;
	
	EventListener listener(callback, priority, false);
	listener.needs_sort = needs_sort;
	listeners.append(listener);
	
	// Only sort if necessary (new listener has higher priority)
	if (needs_sort) {
		EnsureSorted(listeners);
	}
	
	event_listener_count[event_name] = listeners.size();
	
	if (enable_logging) {
		Log::message("[Lua-Event] Subscribed to event: %s (priority: %d, total: %d listeners)\n",
			event_name.c_str(), priority, listeners.size());
	}
}

void Unigine::LuaEventSystem::SubscribeOnce(const std::string& event_name, sol::function callback, int priority)
{
	if (!callback.valid()) {
		Log::warning("[Lua-Event] Invalid callback for event '%s'\n", event_name.c_str());
		return;
	}

	Vector<EventListener>& listeners = event_listeners[event_name];
	
	bool needs_sort = listeners.size() > 0 && priority > listeners[0].priority;
	
	EventListener listener(callback, priority, true);
	listener.needs_sort = needs_sort;
	listeners.append(listener);
	
	if (needs_sort) {
		EnsureSorted(listeners);
	}
	
	event_listener_count[event_name] = listeners.size();
	
	if (enable_logging) {
		Log::message("[Lua-Event] Subscribed once to event: %s (priority: %d, total: %d listeners)\n",
			event_name.c_str(), priority, listeners.size());
	}
}

void Unigine::LuaEventSystem::SubscribeWithId(const std::string& event_name, const std::string& listener_id, sol::function callback, int priority)
{
	if (!callback.valid()) {
		Log::warning("[Lua-Event] Invalid callback for event '%s'\n", event_name.c_str());
		return;
	}

	// Remove existing listener with same ID if present
	UnsubscribeById(event_name, listener_id);

	Vector<EventListener>& listeners = event_listeners[event_name];
	
	bool needs_sort = listeners.size() > 0 && priority > listeners[0].priority;
	
	EventListener listener(callback, priority, false, listener_id);
	listener.needs_sort = needs_sort;
	listeners.append(listener);
	
	if (needs_sort) {
		EnsureSorted(listeners);
	}
	
	event_listener_count[event_name] = listeners.size();
	
	if (enable_logging) {
		Log::message("[Lua-Event] Subscribed to event: %s with ID '%s' (priority: %d, total: %d listeners)\n",
			event_name.c_str(), listener_id.c_str(), priority, listeners.size());
	}
}

bool Unigine::LuaEventSystem::Unsubscribe(const std::string& event_name, sol::function callback)
{
	std::unordered_map<std::string, Vector<EventListener>>::iterator it = event_listeners.find(event_name);
	if (it == event_listeners.end()) {
		return false;
	}

	Vector<EventListener>& listeners = it->second;
	
	for (int i = 0; i < listeners.size(); i++) {
		// Compare function references (sol2 allows this)
		if (listeners[i].callback == callback) {
			listeners.remove(i);
			event_listener_count[event_name] = listeners.size();
			Log::message("[Lua-Event] Unsubscribed from event: %s (remaining: %d listeners)\n",
				event_name.c_str(), listeners.size());
			return true;
		}
	}
	
	return false;
}

bool Unigine::LuaEventSystem::UnsubscribeById(const std::string& event_name, const std::string& listener_id)
{
	std::unordered_map<std::string, Vector<EventListener>>::iterator it = event_listeners.find(event_name);
	if (it == event_listeners.end()) {
		return false;
	}

	Vector<EventListener>& listeners = it->second;
	
	for (int i = 0; i < listeners.size(); i++) {
		if (listeners[i].id == listener_id) {
			listeners.remove(i);
			event_listener_count[event_name] = listeners.size();
			Log::message("[Lua-Event] Unsubscribed from event: %s by ID '%s' (remaining: %d listeners)\n",
				event_name.c_str(), listener_id.c_str(), listeners.size());
			return true;
		}
	}
	
	return false;
}

void Unigine::LuaEventSystem::UnsubscribeAll(const std::string& event_name)
{
	std::unordered_map<std::string, Vector<EventListener>>::iterator it = event_listeners.find(event_name);
	if (it != event_listeners.end()) {
		int count = it->second.size();
		event_listeners.erase(it);
		event_listener_count[event_name] = 0;
		Log::message("[Lua-Event] Unsubscribed all listeners from event: %s (%d removed)\n",
			event_name.c_str(), count);
	}
}

void Unigine::LuaEventSystem::Emit(const std::string& event_name, sol::variadic_args args)
{
	// First, emit to direct listeners
	std::unordered_map<std::string, Vector<EventListener>>::iterator it = event_listeners.find(event_name);
	if (it != event_listeners.end()) {
		Vector<EventListener>& listeners = it->second;
		
		// Ensure sorted before emitting
		EnsureSorted(listeners);
		
		// Cleanup invalid listeners (only if enabled and needed)
		if (auto_cleanup_invalid) {
			CleanupInvalidListeners(event_name);
		}
		
		if (listeners.size() > 0) {
			// Update statistics
			if (enable_statistics) {
				event_emit_count[event_name]++;
			}

			if (enable_logging) {
				Log::message("[Lua-Event] Emitting event: %s to %d listeners\n",
					event_name.c_str(), listeners.size());
			}
			
			// Performance timing
			float start_time = 0.0f;
			if (enable_statistics) {
				start_time = static_cast<float>(Game::getTime());
			}

			// Store listeners to remove (for once() callbacks)
			Vector<int> to_remove;
			
			// Call listeners in priority order
			for (int i = 0; i < listeners.size(); i++) {
				EventListener& listener = listeners[i];
				
				// Skip disabled listeners
				if (!listener.enabled) {
					continue;
				}
				
				// Mark for removal if it's a once() callback
				if (listener.once) {
					to_remove.append(i);
				}
				
				try {
					listener.callback(args);
				}
				catch (const sol::error& e) {
					Log::error("[Lua-Event] Event '%s' listener error: %s\n",
						event_name.c_str(), e.what());
				}
				catch (const std::exception& e) {
					Log::error("[Lua-Event] Event '%s' listener std::exception: %s\n",
						event_name.c_str(), e.what());
				}
				catch (...) {
					Log::error("[Lua-Event] Event '%s' listener unknown error\n", event_name.c_str());
				}
			}
			
			// Update performance statistics
			if (enable_statistics && start_time > 0) {
				float elapsed = static_cast<float>(Game::getTime()) - start_time;
				event_total_time[event_name] += elapsed;
			}
			
			// Remove once() callbacks (in reverse order to maintain indices)
			for (int i = to_remove.size() - 1; i >= 0; i--) {
				listeners.remove(to_remove[i]);
			}
			
			if (to_remove.size() > 0) {
				event_listener_count[event_name] = listeners.size();
				if (enable_logging) {
					Log::message("[Lua-Event] Removed %d 'once' listeners from event: %s\n",
						to_remove.size(), event_name.c_str());
				}
			}
		}
	}
	
	// Also check and emit to wildcard listeners
	for (std::pair<const std::string, Vector<EventListener>>& pair : event_listeners) {
		const std::string& pattern = pair.first;
		
		// Skip if it's the same event (already processed) or not a wildcard
		if (pattern == event_name || pattern.find("*") == std::string::npos) {
			continue;
		}
		
		// Check if pattern matches this event
		if (MatchWildcard(pattern, event_name)) {
			Vector<EventListener>& wildcard_listeners = pair.second;
			
			EnsureSorted(wildcard_listeners);
			
			if (auto_cleanup_invalid) {
				// Cleanup would need pattern parameter, skip for now
			}
			
			for (int i = 0; i < wildcard_listeners.size(); i++) {
				EventListener& listener = wildcard_listeners[i];
				
				if (!listener.enabled || listener.id != "wildcard") {
					continue;
				}
				
				try {
					listener.callback(args);
				}
				catch (const sol::error& e) {
					Log::error("[Lua-Event] Wildcard pattern '%s' listener error for event '%s': %s\n",
						pattern.c_str(), event_name.c_str(), e.what());
				}
				catch (const std::exception& e) {
					Log::error("[Lua-Event] Wildcard pattern '%s' listener std::exception for event '%s': %s\n",
						pattern.c_str(), event_name.c_str(), e.what());
				}
				catch (...) {
					Log::error("[Lua-Event] Wildcard pattern '%s' listener unknown error for event '%s'\n",
						pattern.c_str(), event_name.c_str());
				}
			}
		}
	}
}

void Unigine::LuaEventSystem::EmitDeferred(const std::string& event_name, sol::variadic_args args, float delay)
{
	DeferredEvent deferred(event_name, args, delay);
	deferred_events.append(deferred);
	
	if (enable_logging) {
		Log::message("[Lua-Event] Queued deferred event: %s (delay: %.3f, queue size: %d)\n",
			event_name.c_str(), delay, deferred_events.size());
	}
}

void Unigine::LuaEventSystem::ProcessDeferredEvents(float ifps)
{
	if (deferred_events.size() == 0) {
		return;
	}
	
	// Process deferred events
	for (int i = deferred_events.size() - 1; i >= 0; i--) {
		DeferredEvent& deferred = deferred_events[i];
		deferred.elapsed += ifps;
		
		if (deferred.elapsed >= deferred.delay) {
			// Time to emit this event
			// Convert stored objects back to variadic_args-like call
			std::unordered_map<std::string, Vector<EventListener>>::iterator it = event_listeners.find(deferred.event_name);
			if (it != event_listeners.end()) {
				Vector<EventListener>& listeners = it->second;
				
				EnsureSorted(listeners);
				
				if (auto_cleanup_invalid) {
					CleanupInvalidListeners(deferred.event_name);
				}
				
				if (listeners.size() > 0) {
					// Update statistics
					if (enable_statistics) {
						event_emit_count[deferred.event_name]++;
					}
					
					// Call listeners with stored arguments
					Vector<int> to_remove;
					
					for (int j = 0; j < listeners.size(); j++) {
						EventListener& listener = listeners[j];
						
						if (!listener.enabled) {
							continue;
						}
						
						if (listener.once) {
							to_remove.append(j);
						}
						
						try {
							// Call with stored arguments using sol::as_args
							listener.callback(sol::as_args(deferred.args));
						}
						catch (const sol::error& e) {
							Log::error("[Lua-Event] Deferred event '%s' listener error: %s\n",
								deferred.event_name.c_str(), e.what());
						}
						catch (const std::exception& e) {
							Log::error("[Lua-Event] Deferred event '%s' listener std::exception: %s\n",
								deferred.event_name.c_str(), e.what());
						}
						catch (...) {
							Log::error("[Lua-Event] Deferred event '%s' listener unknown error\n",
								deferred.event_name.c_str());
						}
					}
					
					// Remove once() callbacks
					for (int j = to_remove.size() - 1; j >= 0; j--) {
						listeners.remove(to_remove[j]);
					}
					
					if (to_remove.size() > 0) {
						event_listener_count[deferred.event_name] = listeners.size();
					}
				}
			}
			
			// Also check wildcard listeners for this event
			for (std::pair<const std::string, Vector<EventListener>>& pair : event_listeners) {
				const std::string& pattern = pair.first;
				
				// Skip if it's the same event (already processed) or not a wildcard
				if (pattern == deferred.event_name || pattern.find("*") == std::string::npos) {
					continue;
				}
				
				// Check if pattern matches this event
				if (MatchWildcard(pattern, deferred.event_name)) {
					Vector<EventListener>& wildcard_listeners = pair.second;
					
					EnsureSorted(wildcard_listeners);
					
					for (int j = 0; j < wildcard_listeners.size(); j++) {
						EventListener& listener = wildcard_listeners[j];
						
						if (!listener.enabled || listener.id != "wildcard") {
							continue;
						}
						
						try {
							listener.callback(sol::as_args(deferred.args));
						}
						catch (const sol::error& e) {
							Log::error("[Lua-Event] Deferred wildcard pattern '%s' listener error for event '%s': %s\n",
								pattern.c_str(), deferred.event_name.c_str(), e.what());
						}
						catch (const std::exception& e) {
							Log::error("[Lua-Event] Deferred wildcard pattern '%s' listener std::exception for event '%s': %s\n",
								pattern.c_str(), deferred.event_name.c_str(), e.what());
						}
						catch (...) {
							Log::error("[Lua-Event] Deferred wildcard pattern '%s' listener unknown error for event '%s'\n",
								pattern.c_str(), deferred.event_name.c_str());
						}
					}
				}
			}
			
			// Remove processed event
			deferred_events.remove(i);
		}
	}
}

void Unigine::LuaEventSystem::ProcessDeferred(float ifps)
{
	ProcessDeferredEvents(ifps);
}

void Unigine::LuaEventSystem::Clear()
{
	int total_listeners = 0;
	for (const std::pair<const std::string, Vector<EventListener>>& pair : event_listeners) {
		total_listeners += pair.second.size();
	}
	
	event_listeners.clear();
	event_emit_count.clear();
	event_listener_count.clear();
	
	Log::message("[Lua-Event] Cleared all event listeners (%d total)\n", total_listeners);
}

void Unigine::LuaEventSystem::ClearEvent(const std::string& event_name)
{
	UnsubscribeAll(event_name);
}

int Unigine::LuaEventSystem::GetListenerCount(const std::string& event_name)
{
	std::unordered_map<std::string, Vector<EventListener>>::iterator it = event_listeners.find(event_name);
	if (it == event_listeners.end()) {
		return 0;
	}
	
	// Cleanup invalid listeners before counting
	CleanupInvalidListeners(event_name);
	
	return event_listeners[event_name].size();
}

bool Unigine::LuaEventSystem::HasListeners(const std::string& event_name)
{
	return GetListenerCount(event_name) > 0;
}

int Unigine::LuaEventSystem::GetEmitCount(const std::string& event_name)
{
	std::unordered_map<std::string, int>::iterator it = event_emit_count.find(event_name);
	if (it == event_emit_count.end()) {
		return 0;
	}
	return it->second;
}

void Unigine::LuaEventSystem::EnableListener(const std::string& event_name, const std::string& listener_id, bool enable)
{
	std::unordered_map<std::string, Vector<EventListener>>::iterator it = event_listeners.find(event_name);
	if (it == event_listeners.end()) {
		return;
	}

	Vector<EventListener>& listeners = it->second;
	
	for (int i = 0; i < listeners.size(); i++) {
		if (listeners[i].id == listener_id) {
			listeners[i].enabled = enable;
			Log::message("[Lua-Event] %s listener '%s' for event '%s'\n",
				enable ? "Enabled" : "Disabled", listener_id.c_str(), event_name.c_str());
			return;
		}
	}
}

void Unigine::LuaEventSystem::EnableAllListeners(const std::string& event_name, bool enable)
{
	std::unordered_map<std::string, Vector<EventListener>>::iterator it = event_listeners.find(event_name);
	if (it == event_listeners.end()) {
		return;
	}

	Vector<EventListener>& listeners = it->second;
	
	for (int i = 0; i < listeners.size(); i++) {
		listeners[i].enabled = enable;
	}
	
	Log::message("[Lua-Event] %s all listeners for event '%s' (%d listeners)\n",
		enable ? "Enabled" : "Disabled", event_name.c_str(), listeners.size());
}

void Unigine::LuaEventSystem::PrintStatistics()
{
	Log::message("[Lua-Event] ========== Event System Statistics ==========\n");
	
	if (event_listeners.empty()) {
		Log::message("[Lua-Event] No events registered\n");
		Log::message("[Lua-Event] ============================================\n");
		return;
	}
	
	for (const std::pair<const std::string, Vector<EventListener>>& pair : event_listeners) {
		const std::string& event_name = pair.first;
		int listener_count = pair.second.size();
		int emit_count = GetEmitCount(event_name);
		float total_time = GetEventTotalTime(event_name);
		
		Log::message("[Lua-Event] Event: '%s'\n", event_name.c_str());
		Log::message("[Lua-Event]   Listeners: %d\n", listener_count);
		Log::message("[Lua-Event]   Emitted: %d times\n", emit_count);
		if (enable_statistics && emit_count > 0) {
			Log::message("[Lua-Event]   Total Time: %.4f seconds\n", total_time);
			Log::message("[Lua-Event]   Avg Time: %.4f seconds per emit\n", total_time / emit_count);
		}
	}
	
	if (deferred_events.size() > 0) {
		Log::message("[Lua-Event] Deferred Events: %d queued\n", deferred_events.size());
	}
	
	Log::message("[Lua-Event] ============================================\n");
}

void Unigine::LuaEventSystem::ResetStatistics()
{
	event_emit_count.clear();
	event_total_time.clear();
	Log::message("[Lua-Event] Statistics reset\n");
}

void Unigine::LuaEventSystem::ClearDeferred()
{
	int count = deferred_events.size();
	deferred_events.clear();
	
	if (enable_logging) {
		Log::message("[Lua-Event] Cleared %d deferred events\n", count);
	}
}

float Unigine::LuaEventSystem::GetEventTotalTime(const std::string& event_name)
{
	std::unordered_map<std::string, float>::iterator it = event_total_time.find(event_name);
	if (it == event_total_time.end()) {
		return 0.0f;
	}
	return it->second;
}

void Unigine::LuaEventSystem::SetLoggingEnabled(bool enabled)
{
	enable_logging = enabled;
	Log::message("[Lua-Event] Logging %s\n", enabled ? "enabled" : "disabled");
}

void Unigine::LuaEventSystem::SetStatisticsEnabled(bool enabled)
{
	enable_statistics = enabled;
	Log::message("[Lua-Event] Statistics %s\n", enabled ? "enabled" : "disabled");
}

void Unigine::LuaEventSystem::SetAutoCleanupInvalid(bool enabled)
{
	auto_cleanup_invalid = enabled;
	Log::message("[Lua-Event] Auto-cleanup of invalid listeners %s\n", enabled ? "enabled" : "disabled");
}

bool Unigine::LuaEventSystem::MatchWildcard(const std::string& pattern, const std::string& event_name)
{
	// Simple wildcard matching: "player.*" matches "player.move", "player.jump", etc.
	// "*" matches everything
	if (pattern == "*") {
		return true;
	}
	
	// Check if pattern ends with ".*"
	if (pattern.size() >= 2 && pattern[pattern.size() - 2] == '.' && pattern[pattern.size() - 1] == '*') {
		std::string prefix = pattern.substr(0, pattern.size() - 2);
		if (event_name.size() >= prefix.size() && 
			event_name.substr(0, prefix.size()) == prefix &&
			event_name[prefix.size()] == '.') {
			return true;
		}
	}
	
	// Check if pattern starts with "*."
	if (pattern.size() >= 2 && pattern[0] == '*' && pattern[1] == '.') {
		std::string suffix = pattern.substr(2);
		if (event_name.size() >= suffix.size() &&
			event_name.substr(event_name.size() - suffix.size()) == suffix) {
			return true;
		}
	}
	
	return false;
}

void Unigine::LuaEventSystem::SubscribeWildcard(const std::string& pattern, sol::function callback, int priority)
{
	// Store wildcard pattern as-is in event_listeners
	// When emitting, check all wildcard patterns
	if (!callback.valid()) {
		Log::warning("[Lua-Event] Invalid callback for wildcard pattern '%s'\n", pattern.c_str());
		return;
	}
	
	Vector<EventListener>& listeners = event_listeners[pattern];
	
	bool needs_sort = listeners.size() > 0 && priority > listeners[0].priority;
	
	EventListener listener(callback, priority, false);
	listener.needs_sort = needs_sort;
	listener.id = "wildcard";  // Mark as wildcard listener
	listeners.append(listener);
	
	if (needs_sort) {
		EnsureSorted(listeners);
	}
	
	event_listener_count[pattern] = listeners.size();
	
	if (enable_logging) {
		Log::message("[Lua-Event] Subscribed to wildcard pattern: %s (priority: %d, total: %d listeners)\n",
			pattern.c_str(), priority, listeners.size());
	}
}

void Unigine::LuaEventSystem::UnsubscribeWildcard(const std::string& pattern, sol::function callback)
{
	std::unordered_map<std::string, Vector<EventListener>>::iterator it = event_listeners.find(pattern);
	if (it == event_listeners.end()) {
		return;
	}
	
	Vector<EventListener>& listeners = it->second;
	
	for (int i = 0; i < listeners.size(); i++) {
		if (listeners[i].callback == callback && listeners[i].id == "wildcard") {
			listeners.remove(i);
			event_listener_count[pattern] = listeners.size();
			
			if (enable_logging) {
				Log::message("[Lua-Event] Unsubscribed from wildcard pattern: %s (remaining: %d listeners)\n",
					pattern.c_str(), listeners.size());
			}
			return;
		}
	}
}
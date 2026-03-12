#pragma once
#ifndef LUA_EVENT_SYSTEM_H
#define LUA_EVENT_SYSTEM_H

#include "LuaSystemForward.h"
#include <vector>
#include <string>
#include <unordered_map>

#pragma warning(push, 0)
#include <sol.hpp>
#pragma warning(pop)

// Vector is already included via LuaSystemForward.h

namespace Unigine
{
	// Event listener entry with metadata
	struct EventListener {
		sol::function callback;
		int priority;           // Higher priority = called first
		bool once;              // Auto-remove after first call
		bool enabled;           // Can be temporarily disabled
		std::string id;         // Optional unique ID for easy removal
		bool needs_sort;        // Flag to track if sorting is needed
		
		EventListener() : priority(0), once(false), enabled(true), needs_sort(false) {}
		EventListener(sol::function cb, int prio = 0, bool one_time = false, const std::string& listener_id = "")
			: callback(cb), priority(prio), once(one_time), enabled(true), id(listener_id), needs_sort(false) {}
	};

	// Deferred event entry
	struct DeferredEvent {
		std::string event_name;
		std::vector<sol::object> args;  // Stored arguments for deferred execution
		float delay;  // Delay in seconds (0 = next frame)
		float elapsed;  // Time elapsed since queued
		
		DeferredEvent() : delay(0.0f), elapsed(0.0f) {}
		DeferredEvent(const std::string& name, sol::variadic_args va, float d)
			: event_name(name), delay(d), elapsed(0.0f) {
			args.reserve(va.size());
			for (sol::variadic_args::iterator it = va.begin(); it != va.end(); ++it) {
				args.push_back(sol::object(*it));
			}
		}
	};

	class LuaEventSystem
	{
	private:
		// Event listeners with metadata
		static std::unordered_map<std::string, Unigine::Vector<EventListener>> event_listeners;
		
		// Deferred event queue
		static Unigine::Vector<DeferredEvent> deferred_events;
		
		// Event statistics
		static std::unordered_map<std::string, int> event_emit_count;
		static std::unordered_map<std::string, int> event_listener_count;
		static std::unordered_map<std::string, float> event_total_time;  // Total time spent in event handlers
		
		// Configuration
		static bool enable_logging;
		static bool enable_statistics;
		static bool auto_cleanup_invalid;
		
		// Helper to remove invalid callbacks
		static void CleanupInvalidListeners(const std::string& event_name);
		
		// Helper to sort listeners by priority (only if needed)
		static void EnsureSorted(Unigine::Vector<EventListener>& listeners);
		
		// Process deferred events
		static void ProcessDeferredEvents(float ifps);

	public:
		// Core functionality
		static void Subscribe(const std::string& event_name, sol::function callback, int priority = 0);
		static void SubscribeOnce(const std::string& event_name, sol::function callback, int priority = 0);
		static void SubscribeWithId(const std::string& event_name, const std::string& listener_id, sol::function callback, int priority = 0);
		
		static bool Unsubscribe(const std::string& event_name, sol::function callback);
		static bool UnsubscribeById(const std::string& event_name, const std::string& listener_id);
		static void UnsubscribeAll(const std::string& event_name);
		
		static void Emit(const std::string& event_name, sol::variadic_args args);
		static void EmitDeferred(const std::string& event_name, sol::variadic_args args, float delay = 0.0f);
		static void ProcessDeferred(float ifps);  // Call this from update loop
		
		static void Clear();
		static void ClearEvent(const std::string& event_name);
		static void ClearDeferred();
		
		// Utility functions
		static int GetListenerCount(const std::string& event_name);
		static bool HasListeners(const std::string& event_name);
		static int GetEmitCount(const std::string& event_name);
		static float GetEventTotalTime(const std::string& event_name);
		
		// Enable/disable listeners
		static void EnableListener(const std::string& event_name, const std::string& listener_id, bool enable);
		static void EnableAllListeners(const std::string& event_name, bool enable);
		
		// Configuration
		static void SetLoggingEnabled(bool enabled);
		static void SetStatisticsEnabled(bool enabled);
		static void SetAutoCleanupInvalid(bool enabled);
		
		// Statistics and debugging
		static void PrintStatistics();
		static void ResetStatistics();
		
		// Wildcard subscriptions (e.g., "player.*" matches all player events)
		static void SubscribeWildcard(const std::string& pattern, sol::function callback, int priority = 0);
		static void UnsubscribeWildcard(const std::string& pattern, sol::function callback);
		
		// Helper to match wildcard patterns
		static bool MatchWildcard(const std::string& pattern, const std::string& event_name);
	};
}

#endif // LUA_EVENT_SYSTEM_H

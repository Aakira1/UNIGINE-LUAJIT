#pragma once
#ifndef LUA_PROPERTY_VALIDATION_H
#define LUA_PROPERTY_VALIDATION_H

#include "LuaSystemForward.h"
#include "LuaSystemTypes.h"

namespace Unigine {
    // Property validation constraints
    struct PropertyConstraints {
        bool has_min = false;
        bool has_max = false;
        bool has_step = false;
        double min_value = 0.0;
        double max_value = 0.0;
        double step_value = 0.0;
        Unigine::String pattern;  // For string regex validation
        Unigine::Vector<Unigine::String> enum_values;  // For enum/choice validation
    };

    class LuaPropertyValidation {
    public:
        // Parse validation constraints from property definition
        static PropertyConstraints parseConstraints(const sol::table& prop_def);
        
        // Validate a property value against constraints
        static bool validateValue(const PropertyConstraints& constraints, 
                                 const Unigine::String& property_type,
                                 const sol::object& value,
                                 Unigine::String& error_message);
        
        // Clamp a value to constraints
        static sol::object clampValue(const PropertyConstraints& constraints,
                                     const Unigine::String& property_type,
                                     const sol::object& value);
    };
}

#endif // LUA_PROPERTY_VALIDATION_H

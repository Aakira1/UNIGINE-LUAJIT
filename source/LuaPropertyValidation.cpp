#include "CommonIncludes.h"
#include "LuaPropertyValidation.h"
#include <regex>

using namespace Unigine;

PropertyConstraints LuaPropertyValidation::parseConstraints(const sol::table& prop_def)
{
    PropertyConstraints constraints;
    
    // Parse min/max/step for numeric types
    if (prop_def["min"].valid())
    {
        constraints.has_min = true;
        sol::object min_obj = prop_def["min"];
        if (min_obj.is<double>())
            constraints.min_value = min_obj.as<double>();
        else if (min_obj.is<int>())
            constraints.min_value = static_cast<double>(min_obj.as<int>());
    }
    
    if (prop_def["max"].valid())
    {
        constraints.has_max = true;
        sol::object max_obj = prop_def["max"];
        if (max_obj.is<double>())
            constraints.max_value = max_obj.as<double>();
        else if (max_obj.is<int>())
            constraints.max_value = static_cast<double>(max_obj.as<int>());
    }
    
    if (prop_def["step"].valid())
    {
        constraints.has_step = true;
        sol::object step_obj = prop_def["step"];
        if (step_obj.is<double>())
            constraints.step_value = step_obj.as<double>();
        else if (step_obj.is<int>())
            constraints.step_value = static_cast<double>(step_obj.as<int>());
    }
    
    // Parse regex pattern for strings
    sol::object pattern_obj = prop_def["pattern"];
    if (pattern_obj.valid() && pattern_obj.is<std::string>())
    {
        constraints.pattern = pattern_obj.as<std::string>().c_str();
    }
    
    // Parse enum/choice values
    sol::object choices_obj = prop_def["choices"];
    if (choices_obj.valid() && choices_obj.is<sol::table>())
    {
        sol::table choices = choices_obj.as<sol::table>();
        choices.for_each([&constraints](const sol::object& key, const sol::object& value) {
            if (value.is<std::string>())
            {
                constraints.enum_values.append(value.as<std::string>().c_str());
            }
        });
    }
    
    return constraints;
}

bool LuaPropertyValidation::validateValue(const PropertyConstraints& constraints,
                                          const Unigine::String& property_type,
                                          const sol::object& value,
                                          Unigine::String& error_message)
{
    error_message.clear();
    
    // Type-specific validation
    if (property_type == "float" || property_type == "int")
    {
        if (!value.is<double>() && !value.is<int>())
        {
            error_message = "Value must be a number";
            return false;
        }
        
        double num_value = value.is<double>() ? value.as<double>() : static_cast<double>(value.as<int>());
        
        if (constraints.has_min && num_value < constraints.min_value)
        {
            error_message = String::format("Value %.2f is below minimum %.2f", num_value, constraints.min_value);
            return false;
        }
        
        if (constraints.has_max && num_value > constraints.max_value)
        {
            error_message = String::format("Value %.2f is above maximum %.2f", num_value, constraints.max_value);
            return false;
        }
        
        if (constraints.has_step)
        {
            double remainder = fmod(num_value - (constraints.has_min ? constraints.min_value : 0.0), constraints.step_value);
            if (remainder > 0.0001 && remainder < constraints.step_value - 0.0001)
            {
                error_message = String::format("Value %.2f does not align with step %.2f", num_value, constraints.step_value);
                return false;
            }
        }
    }
    else if (property_type == "string")
    {
        if (!value.is<std::string>())
        {
            error_message = "Value must be a string";
            return false;
        }
        
        std::string str_value = value.as<std::string>();
        
        // Regex pattern validation
        if (!constraints.pattern.empty())
        {
            try
            {
                std::regex pattern(constraints.pattern.get());
                if (!std::regex_match(str_value, pattern))
                {
                    error_message = String::format("String does not match pattern: %s", constraints.pattern.get());
                    return false;
                }
            }
            catch (const std::regex_error& e)
            {
                error_message = String::format("Invalid regex pattern: %s", e.what());
                return false;
            }
        }
        
        // Enum/choice validation
        if (constraints.enum_values.size() > 0)
        {
            bool found = false;
            for (int i = 0; i < constraints.enum_values.size(); i++)
            {
                if (constraints.enum_values[i] == str_value.c_str())
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                error_message = "Value is not in the allowed choices";
                return false;
            }
        }
    }
    
    return true;
}

sol::object LuaPropertyValidation::clampValue(const PropertyConstraints& constraints,
                                              const Unigine::String& property_type,
                                              const sol::object& value)
{
    if (property_type == "float" || property_type == "int")
    {
        if (!value.is<double>() && !value.is<int>())
            return value;
        
        double num_value = value.is<double>() ? value.as<double>() : static_cast<double>(value.as<int>());
        double clamped = num_value;
        
        if (constraints.has_min && clamped < constraints.min_value)
            clamped = constraints.min_value;
        
        if (constraints.has_max && clamped > constraints.max_value)
            clamped = constraints.max_value;
        
        if (constraints.has_step && constraints.has_min)
        {
            double steps = round((clamped - constraints.min_value) / constraints.step_value);
            clamped = constraints.min_value + steps * constraints.step_value;
        }
        
        if (property_type == "int")
            return sol::make_object(value.lua_state(), static_cast<int>(clamped));
        else
            return sol::make_object(value.lua_state(), clamped);
    }
    
    return value;
}

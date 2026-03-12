#pragma once
#ifndef LUA_AUTOCOMPLETE_GENERATOR_H
#define LUA_AUTOCOMPLETE_GENERATOR_H

#include "LuaSystemForward.h"
#include <string>
#include <vector>

namespace Unigine {
    struct FunctionSignature {
        std::string name;
        std::string return_type;
        std::vector<std::string> parameters;
        std::string description;
    };
    
    struct TypeDefinition {
        std::string name;
        std::vector<FunctionSignature> methods;
        std::vector<std::string> properties;
    };
    
    class LuaAutocompleteGenerator {
    public:
        // Generate LSP definition file (sumneko-lua format)
        static bool generateLSPDefinitions(const Unigine::String& output_path);
        
        // Generate .lua stub files with type annotations
        static bool generateLuaStubs(const Unigine::String& output_path);
        
        // Generate EmmyLua annotations
        static bool generateEmmyLuaAnnotations(const Unigine::String& output_path);
        
        // Extract function signatures from Lua state
        static void extractSignatures(sol::state& lua, 
                                     std::vector<FunctionSignature>& functions,
                                     std::vector<TypeDefinition>& types);
    };
}

#endif // LUA_AUTOCOMPLETE_GENERATOR_H

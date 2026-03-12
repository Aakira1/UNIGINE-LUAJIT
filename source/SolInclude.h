#pragma once
// Ensure we're using at least C++17 — required by Sol2
#if __cplusplus < 201703L
#if defined(_MSC_VER)
#pragma message("Forcing C++17 standard for Sol2 under MSVC.")
#pragma warning(push)
#pragma warning(disable: 4005)
// MSVC fallback macro injection (works for clang-cl too)
#define _HAS_STD_BYTE 0
#define _HAS_CXX17 1
#elif defined(__clang__)
#pragma message("Forcing C++17 features for Sol2 under Clang.")
#endif
#endif

// Silence older STL comparators if missing
#if !defined(__cpp_lib_transparent_operators)
namespace std {
    template <typename = void>
    struct less {
        template <typename L, typename R>
        constexpr bool operator()(const L& lhs, const R& rhs) const noexcept {
            return lhs < rhs;
        }
    };
    template <typename = void>
    struct equal_to {
        template <typename L, typename R>
        constexpr bool operator()(const L& lhs, const R& rhs) const noexcept {
            return lhs == rhs;
        }
    };
}
#endif

// Safety toggles for Sol2 build
#ifndef SOL_ALL_SAFETIES_ON
#define SOL_ALL_SAFETIES_ON 1
#endif
#ifndef SOL_ENABLE_INTEROP
#define SOL_ENABLE_INTEROP 1
#endif
#ifndef SOL_PRINT_ERRORS
#define SOL_PRINT_ERRORS 1
#endif
#ifndef SOL_NO_EXCEPTIONS
#define SOL_NO_EXCEPTIONS 0
#endif

// Finally include Sol2
#include <sol.hpp>

// Optional: diagnostics
#if defined(_MSC_VER)
#pragma message("Sol2 included successfully with C++17 fallback")
#endif


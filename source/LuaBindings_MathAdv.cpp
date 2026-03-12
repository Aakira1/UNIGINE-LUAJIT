#include "CommonIncludes.h"
#include "LuaAPIBindings.h" 
#include "LuaSystem.h"
#include <UnigineMathLib2d.h>
#include <UnigineMathLibRandom.h>


using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

void Unigine::LuaBindings::registerMathTypes(sol::state& lua)
{

#pragma region Vec2(double) & vec2(float) registration
    lua.new_usertype<Vec2>("Vec2",
        // Constructors
        sol::constructors<
        Vec2(),                          // Default: (0, 0)
        Vec2(double),                     // All components same
        Vec2(double, double)              // Individual components
        > (),
        // Properties (direct member access)
        "x", &Vec2::x,
        "y", &Vec2::y,
        "length", &Vec2::length,
        "length2", &Vec2::length2,
        "normalize", [](Vec2& v) { v.normalize(); return v; },
        "normalized", [](const Vec2& v) {
            Vec2 result = v;
            result.normalize();
            return result;
        },
        // Operators
        sol::meta_function::addition, [](const Vec2& a, const Vec2& b) { return a + b; },
        sol::meta_function::subtraction, [](const Vec2& a, const Vec2& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const Vec2& v, double s) { return v * s; },
            [](double s, const Vec2& v) { return v * s; }
        ),
        sol::meta_function::division, [](const Vec2& v, double s) { return v / s; },
        sol::meta_function::unary_minus, [](const Vec2& v) { return -v; },
        sol::meta_function::equal_to, [](const Vec2& a, const Vec2& b) {
            return a.x == b.x && a.y == b.y;
        },
        // String representation for debugging
        sol::meta_function::to_string, [](const Vec2& v) {
            return "Vec2(" + std::to_string(v.x) + ", " +
                std::to_string(v.y) + ")";
        }
    );

    lua.new_usertype<vec2>("vec2",
        sol::constructors<
        vec2(),                          // Default: (0, 0)
        vec2(float),                     // All components same
        vec2(float, float)               // Individual components
        > (),
        // Make it callable as a function
        sol::meta_function::call, sol::overload(
            []() { return vec2(0.0f, 0.0f); },
            [](float value) { return vec2(value); },
            [](float x, float y) { return vec2(x, y); }
        ),
        "x", &vec2::x,
        "y", &vec2::y,
        "length", &vec2::length,
        "length2", &vec2::length2,
        "normalize", [](vec2& v) { v.normalize(); return v; },
        "normalized", [](const vec2& v) {
            vec2 result = v;
            result.normalize();
            return result;
        },
        sol::meta_function::addition, [](const vec2& a, const vec2& b) { return a + b; },
        sol::meta_function::subtraction, [](const vec2& a, const vec2& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const vec2& v, float s) { return v * s; },
            [](float s, const vec2& v) { return v * s; }
        ),
        sol::meta_function::division, [](const vec2& v, float s) { return v / s; },
        sol::meta_function::unary_minus, [](const vec2& v) { return -v; },
        sol::meta_function::equal_to, [](const vec2& a, const vec2& b) {
            return a.x == b.x && a.y == b.y;
        },
        sol::meta_function::to_string, [](const vec2& v) {
            return "vec2(" + std::to_string(v.x) + ", " +
                std::to_string(v.y) + ")";
        }
    );

    lua["Vec2"][sol::meta_function::call] = sol::overload(
        []() { return Vec2(0.0, 0.0); },
        [](double value) { return Vec2(value); },
        [](double x, double y) { return Vec2(x, y); }
    );

    lua["Vec2"]["dot"] = [](const Vec2& a, const Vec2& b) { return dot(a, b); };
    lua["Vec2"]["lerp"] = [](const Vec2& a, const Vec2& b, double t) { return lerp(a, b, t); };
    lua["Vec2"]["min"] = [](const Vec2& a, const Vec2& b) { return min(a, b); };
    lua["Vec2"]["max"] = [](const Vec2& a, const Vec2& b) { return max(a, b); };
    lua["Vec2"]["clamp"] = [](const Vec2& v, const Vec2& minVal, const Vec2& maxVal) { return clamp(v, minVal, maxVal); };
    lua["Vec2"]["distance"] = [](const Vec2& a, const Vec2& b) { return distance(a, b); };
    lua["Vec2"]["distance2"] = [](const Vec2& a, const Vec2& b) { return distance2(a, b); };

    lua["Vec2"]["ZERO"] = Vec2(0.0, 0.0);
    lua["Vec2"]["ONE"] = Vec2(1.0, 1.0);
    lua["Vec2"]["UP"] = Vec2(0.0, 1.0);
    lua["Vec2"]["DOWN"] = Vec2(0.0, -1.0);
    lua["Vec2"]["RIGHT"] = Vec2(1.0, 0.0);
    lua["Vec2"]["LEFT"] = Vec2(-1.0, 0.0);

    lua["vec2"]["dot"] = [](const vec2& a, const vec2& b) { return dot(a, b); };
    lua["vec2"]["lerp"] = [](const vec2& a, const vec2& b, float t) { return lerp(a, b, t); };
    lua["vec2"]["min"] = [](const vec2& a, const vec2& b) { return min(a, b); };
    lua["vec2"]["max"] = [](const vec2& a, const vec2& b) { return max(a, b); };
    lua["vec2"]["clamp"] = [](const vec2& v, const vec2& minVal, const vec2& maxVal) { return clamp(v, minVal, maxVal); };
    lua["vec2"]["distance"] = [](const vec2& a, const vec2& b) { return distance(a, b); };
    lua["vec2"]["distance2"] = [](const vec2& a, const vec2& b) { return distance2(a, b); };

    lua["vec2"]["ZERO"] = vec2(0.0f, 0.0f);
    lua["vec2"]["ONE"] = vec2(1.0f, 1.0f);
    lua["vec2"]["UP"] = vec2(0.0f, 1.0f);
    lua["vec2"]["DOWN"] = vec2(0.0f, -1.0f);
    lua["vec2"]["RIGHT"] = vec2(1.0f, 0.0f);
    lua["vec2"]["LEFT"] = vec2(-1.0f, 0.0f);
#pragma endregion

#pragma region Vec3(double) & vec3(float) registration
    lua.new_usertype<Vec3>("Vec3",
        // Constructors
        sol::constructors<
        Vec3(),                          // Default: (0, 0, 0)
        Vec3(double),                     // All components same
        Vec3(double, double, double)        // Individual components
        > (),
        // Properties (direct member access)
        "x", &Vec3::x,
        "y", &Vec3::y,
        "z", &Vec3::z,
        // Methods that return values
        "length", &Vec3::length,
        "length2", &Vec3::length2,
        // Methods that return values
        "normalize", [](Vec3& v) { v.normalize(); return v; },
        "normalized", [](const Vec3& v) {
            Vec3 result = v;
            result.normalize();
            return result;
        },
        "cross", [](const Vec3& a, const Vec3& b) { return cross(a, b); },
        // Operators
        sol::meta_function::addition, [](const Vec3& a, const Vec3& b) { return a + b; },
        sol::meta_function::subtraction, [](const Vec3& a, const Vec3& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const Vec3& v, double s) { return v * s; },
            [](double s, const Vec3& v) { return v * s; }
        ),
        sol::meta_function::division, [](const Vec3& v, double s) { return v / s; },
        sol::meta_function::unary_minus, [](const Vec3& v) { return -v; },
        sol::meta_function::equal_to, [](const Vec3& a, const Vec3& b) {
            return a.x == b.x && a.y == b.y && a.z == b.z;
        },
        // String representation for debugging
        sol::meta_function::to_string, [](const Vec3& v) {
            return "Vec3(" + std::to_string(v.x) + ", " +
                std::to_string(v.y) + ", " +
                std::to_string(v.z) + ")";
        }
    );

    lua.new_usertype<vec3>("vec3",
        // Constructors
        sol::constructors<
        vec3(),                          // Default: (0, 0, 0)
        vec3(float),                     // All components same
        vec3(float, float, float)        // Individual components
        > (),
        // Make it callable as a function
        sol::meta_function::call, sol::overload(
            []() { return vec3(0.0f, 0.0f, 0.0f); },
            [](float value) { return vec3(value); },
            [](float x, float y, float z) { return vec3(x, y, z); }
        ),
        // Properties (direct member access)
        "x", &vec3::x,
        "y", &vec3::y,
        "z", &vec3::z,
        // Methods that return values
        "length", &vec3::length,
        "length2", &vec3::length2,

        // normalize() modifies in-place and returns void, so wrap it
        "normalize", [](vec3& v) { v.normalize(); return v; },

        // normalized() - returns a copy
        "normalized", [](const vec3& v) {
            vec3 result = v;
            result.normalize();
            return result;
        },
        "cross", [](const vec3& a, const vec3& b) { return cross(a, b); },

        // Operators
        sol::meta_function::addition, [](const vec3& a, const vec3& b) { return a + b; },
        sol::meta_function::subtraction, [](const vec3& a, const vec3& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const vec3& v, float s) { return v * s; },
            [](float s, const vec3& v) { return v * s; }
        ),
        sol::meta_function::division, [](const vec3& v, float s) { return v / s; },
        sol::meta_function::unary_minus, [](const vec3& v) { return -v; },
        sol::meta_function::equal_to, [](const vec3& a, const vec3& b) {
            return a.x == b.x && a.y == b.y && a.z == b.z;
        },

        // String representation for debugging
        sol::meta_function::to_string, [](const vec3& v) {
            return "vec3(" + std::to_string(v.x) + ", " +
                std::to_string(v.y) + ", " +
                std::to_string(v.z) + ")";
        }
    );

    lua["Vec3"][sol::meta_function::call] = sol::overload(
        []() { return Vec3(0.0, 0.0, 0.0); },
        [](double value) { return Vec3(value); },
        [](double x, double y, double z) { return Vec3(x, y, z); }
    );

    // Add .new() method for convenience (matches API guide)
    lua["Vec3"]["new"] = sol::overload(
        []() { return Vec3(0.0, 0.0, 0.0); },
        [](double value) { return Vec3(value); },
        [](double x, double y, double z) { return Vec3(x, y, z); }
    );

    lua["Vec3"]["dot"] = [](const Vec3& a, const Vec3& b) { return dot(a, b); };
    lua["Vec3"]["cross"] = [](const Vec3& a, const Vec3& b) { return cross(a, b); };
    lua["Vec3"]["lerp"] = [](const Vec3& a, const Vec3& b, double t) { return lerp(a, b, t); };
    lua["Vec3"]["min"] = [](const Vec3& a, const Vec3& b) { return min(a, b); };
    lua["Vec3"]["max"] = [](const Vec3& a, const Vec3& b) { return max(a, b); };
    lua["Vec3"]["clamp"] = [](const Vec3& v, const Vec3& minVal, const Vec3& maxVal) { return clamp(v, minVal, maxVal); };
    lua["Vec3"]["reflect"] = [](const Vec3& v, const Vec3& normal) { return reflect(v, normal); };
    lua["Vec3"]["distance"] = [](const Vec3& a, const Vec3& b) { return distance(a, b); };
    lua["Vec3"]["distance2"] = [](const Vec3& a, const Vec3& b) { return distance2(a, b); };

    lua["Vec3"]["ZERO"] = Vec3(0.0, 0.0, 0.0);
    lua["Vec3"]["UP"] = Math::vec3_up;
    lua["Vec3"]["DOWN"] = Math::vec3_down;
    lua["Vec3"]["FORWARD"] = Math::Vec3_forward;
    lua["Vec3"]["BACK"] = Math::Vec3_back;
    lua["Vec3"]["RIGHT"] = Math::Vec3_right;
    lua["Vec3"]["LEFT"] = Math::Vec3_left;
    lua["Vec3"]["ONE"] = Vec3(1.0, 1.0, 1.0);
    lua["Vec3"]["UP"] = Vec3(0.0, 0.0, 1.0);
    lua["Vec3"]["DOWN"] = Vec3(0.0, 0.0, -1.0);
    lua["Vec3"]["FORWARD"] = Vec3(0.0, 1.0, 0.0);
    lua["Vec3"]["BACK"] = Vec3(0.0, -1.0, 0.0);
    lua["Vec3"]["RIGHT"] = Vec3(1.0, 0.0, 0.0);
    lua["Vec3"]["LEFT"] = Vec3(-1.0, 0.0, 0.0);

    // static utility functions
    lua["vec3"]["dot"] = [](const vec3& a, const vec3& b) { return dot(a, b); };
    lua["vec3"]["cross"] = [](const vec3& a, const vec3& b) { return cross(a, b); };
    lua["vec3"]["lerp"] = [](const vec3& a, const vec3& b, float t) { return lerp(a, b, t); };
    lua["vec3"]["min"] = [](const vec3& a, const vec3& b) { return min(a, b); };
    lua["vec3"]["max"] = [](const vec3& a, const vec3& b) { return max(a, b); };
    lua["vec3"]["clamp"] = [](const vec3& v, const vec3& minVal, const vec3& maxVal) { return clamp(v, minVal, maxVal); };
    lua["vec3"]["reflect"] = [](const vec3& v, const vec3& normal) { return reflect(v, normal); };
    lua["vec3"]["distance"] = [](const vec3& a, const vec3& b) { return distance(a, b); };
    lua["vec3"]["distance2"] = [](const vec3& a, const vec3& b) { return distance2(a, b); };

    // vec3 constants
    lua["vec3"]["ZERO"] = vec3(0.0f, 0.0f, 0.0f);
    lua["vec3"]["ONE"] = vec3(1.0f, 1.0f, 1.0f);
    lua["vec3"]["UP"] = vec3(0.0f, 0.0f, 1.0f);
    lua["vec3"]["DOWN"] = vec3(0.0f, 0.0f, -1.0f);
    lua["vec3"]["FORWARD"] = vec3(0.0f, 1.0f, 0.0f);
    lua["vec3"]["BACK"] = vec3(0.0f, -1.0f, 0.0f);
    lua["vec3"]["RIGHT"] = vec3(1.0f, 0.0f, 0.0f);
    lua["vec3"]["LEFT"] = vec3(-1.0f, 0.0f, 0.0f);
#pragma endregion

#pragma region Quat registration
    lua.new_usertype<quat>("Quat",
        // Constructors
        sol::constructors<
        quat(),                              // Identity quaternion
        quat(float, float, float, float),    // Direct: x, y, z, w
        quat(const vec3&, float),            // Axis-angle (axis, angle_degrees)
        quat(float, float, float)            // Euler angles (x, y, z in degrees)
        > (),

        // Properties
        "x", &quat::x,
        "y", &quat::y,
        "z", &quat::z,
        "w", &quat::w,

        // normalize() modifies in-place
        "normalize", [](quat& q) { q.normalize(); return q; },
        "normalized", [](const quat& q) {
            quat result = q;
            result.normalize();
            return result;
        },
        "getAngle", [](const quat& q, const Vec3& axis) {
            return q.getAngle(vec3(axis));
        },

        // Operators
        sol::meta_function::multiplication, sol::overload(
            [](const quat& q1, const quat& q2) { return q1 * q2; },
            [](const quat& q, const vec3& v) { return q * v; },
            [](const quat& q, float s) { return q * s; }
        ),
        sol::meta_function::addition, [](const quat& q1, const quat& q2) { return q1 + q2; },
        sol::meta_function::subtraction, [](const quat& q1, const quat& q2) { return q1 - q2; },
        sol::meta_function::unary_minus, [](const quat& q) { return -q; },
        sol::meta_function::equal_to, [](const quat& q1, const quat& q2) {
            return q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w;
        },

        // String representation
        sol::meta_function::to_string, [](const quat& q) {
            return "Quat(" + std::to_string(q.x) + ", " +
                std::to_string(q.y) + ", " +
                std::to_string(q.z) + ", " +
                std::to_string(q.w) + ")";
        }
    );

    // quat static utility functions
    lua["Quat"]["slerp"] = [](const quat& q1, const quat& q2, float t) {
        return slerp(q1, q2, t);
        };
    lua["Quat"]["dot"] = [](const quat& q1, const quat& q2) {
        return dot(q1, q2);
        };
    lua["Quat"]["inverse"] = [](const quat& q) {
        return inverse(q);
        };
    lua["Quat"]["fromAxisAngle"] = [](const vec3& axis, float angle_degrees) {
        return quat(axis, angle_degrees);
        };
    lua["Quat"]["fromEuler"] = [](float x_deg, float y_deg, float z_deg) {
        return quat(x_deg, y_deg, z_deg);
        };
    lua["Quat"]["fromMat4"] = [](const mat4& m) {
        return quat(m);
        };
    lua["Quat"]["lookAt"] = [](const vec3& from, const vec3& to, const vec3& up) {
        vec3 direction = to - from;
        direction.normalize();

        vec3 right = cross(direction, up);
        right.normalize();

        vec3 newUp = cross(right, direction);

        mat3 rotMatrix;
        rotMatrix.setColumn(0, right);
        rotMatrix.setColumn(1, direction);
        rotMatrix.setColumn(2, newUp);

        return quat(rotMatrix);
        };

    // quat static constants
    lua["Quat"]["IDENTITY"] = quat();
    lua["Quat"]["ZERO"] = quat(0.0f, 0.0f, 0.0f, 0.0f);
    lua["Quat"]["X_AXIS_90"] = quat(vec3(1.0f, 0.0f, 0.0f), 90.0f);
    lua["Quat"]["Y_AXIS_90"] = quat(vec3(0.0f, 1.0f, 0.0f), 90.0f);
    lua["Quat"]["Z_AXIS_90"] = quat(vec3(0.0f, 0.0f, 1.0f), 90.0f);
    lua["Quat"]["X_AXIS_180"] = quat(vec3(1.0f, 0.0f, 0.0f), 180.0f);
    lua["Quat"]["Y_AXIS_180"] = quat(vec3(0.0f, 1.0f, 0.0f), 180.0f);
    lua["Quat"]["Z_AXIS_180"] = quat(vec3(0.0f, 0.0f, 1.0f), 180.0f);
    lua["Quat"]["X_AXIS_270"] = quat(vec3(1.0f, 0.0f, 0.0f), 270.0f);
    lua["Quat"]["Y_AXIS_270"] = quat(vec3(0.0f, 1.0f, 0.0f), 270.0f);
    lua["Quat"]["Z_AXIS_270"] = quat(vec3(0.0f, 0.0f, 1.0f), 270.0f);
    lua["Quat"]["X_AXIS_360"] = quat(vec3(1.0f, 0.0f, 0.0f), 360.0f);
    lua["Quat"]["Y_AXIS_360"] = quat(vec3(0.0f, 1.0f, 0.0f), 360.0f);
    lua["Quat"]["Z_AXIS_360"] = quat(vec3(0.0f, 0.0f, 1.0f), 360.0f);
    lua["Quat"]["NEGATIVE_IDENTITY"] = quat(0.0f, 0.0f, 0.0f, -1.0f);

    // Add lowercase alias for convenience
    lua["quat"] = lua["Quat"];

    lua["Quat"][sol::meta_function::call] = sol::overload(
        []() { return quat(); },
        [](float x, float y, float z, float w) { return quat(x, y, z, w); },
        [](const vec3& axis, float angle) { return quat(axis, angle); },
        [](const Vec3& axis, double angle) { return quat(vec3(axis), static_cast<float>(angle)); },
        [](float x, float y, float z) { return quat(x, y, z); }
    );
    
    // Add .new() method for convenience (same as calling Quat())
    lua["Quat"]["new"] = sol::overload(
        []() { return quat(); },
        [](float x, float y, float z, float w) { return quat(x, y, z, w); },
        [](const vec3& axis, float angle) { return quat(axis, angle); },
        [](const Vec3& axis, double angle) { return quat(vec3(axis), static_cast<float>(angle)); },
        [](float x, float y, float z) { return quat(x, y, z); }
    );
#pragma endregion

#pragma region vec4(float) registration
    lua.new_usertype<vec4>("vec4",
        sol::constructors<
        vec4(),
        vec4(float),
        vec4(float, float, float, float)
        > (),

        // Properties
        "x", &vec4::x,
        "y", &vec4::y,
        "z", &vec4::z,
        "w", &vec4::w,

        // Alias for color operations
        "r", &vec4::x,
        "g", &vec4::y,
        "b", &vec4::z,
        "a", &vec4::w,

        // Methods
        "length", &vec4::length,
        "length2", &vec4::length2,
        "normalize", [](vec4& v) { v.normalize(); return v; },
        "normalized", [](const vec4& v) {
            vec4 result = v;
            result.normalize();
            return result;
        },

        // Operators
        sol::meta_function::addition, [](const vec4& a, const vec4& b) { return a + b; },
        sol::meta_function::subtraction, [](const vec4& a, const vec4& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const vec4& v, float s) { return v * s; },
            [](float s, const vec4& v) { return v * s; }
        ),
        sol::meta_function::division, [](const vec4& v, float s) { return v / s; },
        sol::meta_function::unary_minus, [](const vec4& v) { return -v; },
        sol::meta_function::equal_to, [](const vec4& a, const vec4& b) {
            return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
        },

        sol::meta_function::to_string, [](const vec4& v) {
            return "vec4(" + std::to_string(v.x) + ", " +
                std::to_string(v.y) + ", " +
                std::to_string(v.z) + ", " +
                std::to_string(v.w) + ")";
        }
    );

    lua["vec4"][sol::meta_function::call] = sol::overload(
        []() { return vec4(0.0f, 0.0f, 0.0f, 0.0f); },
        [](float value) { return vec4(value); },
        [](float x, float y, float z, float w) { return vec4(x, y, z, w); }
    );

    // Utility functions
    lua["vec4"]["dot"] = [](const vec4& a, const vec4& b) { return dot(a, b); };
    lua["vec4"]["lerp"] = [](const vec4& a, const vec4& b, float t) { return lerp(a, b, t); };
    lua["vec4"]["min"] = [](const vec4& a, const vec4& b) { return min(a, b); };
    lua["vec4"]["max"] = [](const vec4& a, const vec4& b) { return max(a, b); };
    lua["vec4"]["clamp"] = [](const vec4& v, const vec4& minVal, const vec4& maxVal) {
        return clamp(v, minVal, maxVal);
        };

    // Common color constants
    lua["vec4"]["ZERO"] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    lua["vec4"]["ONE"] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    lua["vec4"]["WHITE"] = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    lua["vec4"]["BLACK"] = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    lua["vec4"]["RED"] = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    lua["vec4"]["GREEN"] = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    lua["vec4"]["BLUE"] = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    lua["vec4"]["YELLOW"] = vec4(1.0f, 1.0f, 0.0f, 1.0f);
    lua["vec4"]["CYAN"] = vec4(0.0f, 1.0f, 1.0f, 1.0f);
    lua["vec4"]["MAGENTA"] = vec4(1.0f, 0.0f, 1.0f, 1.0f);
    lua["vec4"]["TRANSPARENT"] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
#pragma endregion

#pragma region Mat4 (4x4 matrix) registration
    // Mat4 is a typedef, we need to bind the actual type
    // Using dmat4 directly since Mat4 = dmat4 in double precision mode
    lua.new_usertype<dmat4>("Mat4",
        sol::no_constructor,

        // Transform setup methods
        "setIdentity", [](dmat4& m) { m.setIdentity(); },
        "setTranslate", [](dmat4& m, const Vec3& v) { m.setTranslate(v); },
        "setRotate", sol::overload(
            [](dmat4& m, const Vec3& axis, double angle) { m.setRotate(axis, angle); },
            [](dmat4& m, const vec3& axis, float angle) { m.setRotate(dvec3(axis), static_cast<double>(angle)); }
        ),
        "setScale", [](dmat4& m, const Vec3& s) { m.setScale(s); },

        // Get translation and axes
        "getTranslate", [](const dmat4& m) { return m.getTranslate(); },
        "getAxisX", [](const dmat4& m) { return m.getAxisX(); },
        "getAxisY", [](const dmat4& m) { return m.getAxisY(); },
        "getAxisZ", [](const dmat4& m) { return m.getAxisZ(); },
        "getRotate", [](const dmat4& m) { return m.getRotate(); },
        "setColumn3", [](dmat4& m, int col, const Vec3& v) { m.setColumn3(col, v); },

        // Operators
        sol::meta_function::multiplication, sol::overload(
            [](const dmat4& a, const dmat4& b) { return a * b; },
            [](const dmat4& a, const Vec3& v) { return a * v; },
            [](const dmat4& a, const vec3& v) { return a * vec3(v); }
        )
    );

    // Add factory function Mat4.new() that returns dmat4
    lua["Mat4"]["new"] = []() {
        dmat4 m;
        m.setIdentity();
        return m;
    };

    // Helper functions for Mat4
    lua["Mat4"]["translate"] = [](const Vec3& v) {
        return Math::translate(v);
    };

    lua["Mat4"]["setTo"] = [](const Vec3& pos, const Vec3& target, const Vec3& up, int axis) {
        // Convert Vec3 (double) to vec3 (float) for setTo function
        return Math::setTo(vec3(pos), vec3(target), vec3(up), axis);
    };
    
    lua["Mat4"]["rotateZ"] = [](double angle) {
        return Math::rotateZ(angle);
    };
    
    lua["Mat4"]["rotate"] = [](const Vec3& axis, double angle) {
        return Math::rotate(dvec3(axis), angle);
    };
    
    // Note: Math.getAngle will be registered in registerMathUtilities where Math table exists

    // Matrix axis constants
    lua["Mat4"]["AXIS_X"] = Math::AXIS_X;
    lua["Mat4"]["AXIS_Y"] = Math::AXIS_Y;
    lua["Mat4"]["AXIS_Z"] = Math::AXIS_Z;
    lua["Mat4"]["AXIS_NX"] = Math::AXIS_NX;
    lua["Mat4"]["AXIS_NY"] = Math::AXIS_NY;
    lua["Mat4"]["AXIS_NZ"] = Math::AXIS_NZ;
#pragma endregion

#pragma region ivec2, ivec3, ivec4 (integer vectors) registration
    lua.new_usertype<ivec2>("ivec2",
        sol::constructors<
        ivec2(),
        ivec2(int),
        ivec2(int, int)
        >(),

        "x", &ivec2::x,
        "y", &ivec2::y,

        sol::meta_function::addition, [](const ivec2& a, const ivec2& b) { return a + b; },
        sol::meta_function::subtraction, [](const ivec2& a, const ivec2& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const ivec2& v, int s) { return v * s; },
            [](int s, const ivec2& v) { return v * s; }
        ),
        sol::meta_function::division, [](const ivec2& v, int s) { return v / s; },
        sol::meta_function::unary_minus, [](const ivec2& v) { return -v; },
        sol::meta_function::equal_to, [](const ivec2& a, const ivec2& b) {
            return a.x == b.x && a.y == b.y;
        },
        sol::meta_function::to_string, [](const ivec2& v) {
            return "ivec2(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
        }
    );

    lua.new_usertype<ivec3>("ivec3",
        sol::constructors<
        ivec3(),
        ivec3(int),
        ivec3(int, int, int)
        >(),

        "x", &ivec3::x,
        "y", &ivec3::y,
        "z", &ivec3::z,

        sol::meta_function::addition, [](const ivec3& a, const ivec3& b) { return a + b; },
        sol::meta_function::subtraction, [](const ivec3& a, const ivec3& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const ivec3& v, int s) { return v * s; },
            [](int s, const ivec3& v) { return v * s; }
        ),
        sol::meta_function::division, [](const ivec3& v, int s) { return v / s; },
        sol::meta_function::unary_minus, [](const ivec3& v) { return -v; },
        sol::meta_function::equal_to, [](const ivec3& a, const ivec3& b) {
            return a.x == b.x && a.y == b.y && a.z == b.z;
        },
        sol::meta_function::to_string, [](const ivec3& v) {
            return "ivec3(" + std::to_string(v.x) + ", " +
                   std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
        }
    );

    lua.new_usertype<ivec4>("ivec4",
        sol::constructors<
        ivec4(),
        ivec4(int),
        ivec4(int, int, int, int)
        >(),

        "x", &ivec4::x,
        "y", &ivec4::y,
        "z", &ivec4::z,
        "w", &ivec4::w,

        sol::meta_function::addition, [](const ivec4& a, const ivec4& b) { return a + b; },
        sol::meta_function::subtraction, [](const ivec4& a, const ivec4& b) { return a - b; },
        sol::meta_function::multiplication, sol::overload(
            [](const ivec4& v, int s) { return v * s; },
            [](int s, const ivec4& v) { return v * s; }
        ),
        sol::meta_function::division, [](const ivec4& v, int s) { return v / s; },
        sol::meta_function::unary_minus, [](const ivec4& v) { return -v; },
        sol::meta_function::equal_to, [](const ivec4& a, const ivec4& b) {
            return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
        },
        sol::meta_function::to_string, [](const ivec4& v) {
            return "ivec4(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " +
                   std::to_string(v.z) + ", " + std::to_string(v.w) + ")";
        }
    );

    lua["ivec2"]["ZERO"] = ivec2(0, 0);
    lua["ivec2"]["ONE"] = ivec2(1, 1);
    lua["ivec3"]["ZERO"] = ivec3(0, 0, 0);
    lua["ivec3"]["ONE"] = ivec3(1, 1, 1);
    lua["ivec4"]["ZERO"] = ivec4(0, 0, 0, 0);
    lua["ivec4"]["ONE"] = ivec4(1, 1, 1, 1);
#pragma endregion
}
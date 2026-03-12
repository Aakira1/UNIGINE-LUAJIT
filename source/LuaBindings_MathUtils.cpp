#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include <UnigineMathLib2d.h>
#include <UnigineMathLibRandom.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

/// <summary>
/// Registers the Game API in the provided Lua state by creating a 'Game' table and binding Game functions (getIFps, getTime, getScale, setScale).
/// </summary>
/// <param name="lua">Reference to the sol::state representing the Lua environment where the Math utilities will be registered. The function modifies this Lua state.</param>
void Unigine::LuaBindings::registerMathUtilities(sol::state& lua)
{
	// Create Math table
	lua["Math"] = lua.create_table();

	// trigonometric functions
	lua["Math"]["sin"] = [](float x) { return sin(x); };
	lua["Math"]["cos"] = [](float x) { return cos(x); };
	lua["Math"]["tan"] = [](float x) { return tan(x); };
	lua["Math"]["asin"] = [](float x) { return asin(x); };
	lua["Math"]["acos"] = [](float x) { return acos(x); };
	lua["Math"]["atan"] = [](float x) { return atan(x); };
	lua["Math"]["atan2"] = [](float y, float x) { return atan2(y, x); };
	lua["Math"]["quatEuler"] = [](float pitch, float roll, float yaw) {
		return Unigine::Math::quat(Unigine::Math::vec3(pitch, roll, yaw));
		};

	// common mathematical functions
	lua["Math"]["abs"] = [](float x) { return abs(x); };
	lua["Math"]["floor"] = [](float x) { return floor(x); };
	lua["Math"]["ceil"] = [](float x) { return ceil(x); };
	lua["Math"]["sqrt"] = [](float x) { return sqrt(x); };
	lua["Math"]["pow"] = [](float base, float exp) { return pow(base, exp); };
	lua["Math"]["exp"] = [](float x) { return exp(x); };
	lua["Math"]["log"] = [](float x) { return log(x); };

	// constants
	lua["Math"]["PI"] = Consts::PI;
	lua["Math"]["PI2"] = Consts::PI2;  // 2*PI
	lua["Math"]["HALF_PI"] = Consts::PI05; // PI/2
	lua["Math"]["INV_PI2"] = Consts::IPI2; // 1/(2*PI)
	lua["Math"]["LOG2"] = Consts::LOG2;
	lua["Math"]["LOG10"] = Consts::LOG10;
	lua["Math"]["SQRT2"] = Consts::SQRT2;
	lua["Math"]["DEG2RAD"] = Consts::DEG2RAD;
	lua["Math"]["RAD2DEG"] = Consts::RAD2DEG;
	lua["Math"]["EPSILON"] = Consts::EPS;
	lua["Math"]["INFINITY"] = Consts::INF;
	lua["Math"]["GOLDEN_RATIO"] = Consts::GOLDEN_RATIO;
	lua["Math"]["GRAVITY"] = Consts::GRAVITY;
	lua["Math"]["EPSILON_D"] = Consts::EPS_D;
	lua["Math"]["DEG2RAD_D"] = Consts::DEG2RAD_D;
	lua["Math"]["RAD2DEG_D"] = Consts::RAD2DEG_D;
	lua["Math"]["INV_PI2_D"] = Consts::IPI2_D;
	lua["Math"]["PI_D"] = Consts::PI_D;
	lua["Math"]["HALF_PI_D"] = Consts::PI05_D;

	// Interpolation functions
	lua["Math"]["clamp"] = [](float x, float minVal, float maxVal) {
		return (x < minVal) ? minVal : (x > maxVal) ? maxVal : x;
		};
	lua["Math"]["lerp"] = [](float a, float b, float t) {
		return a + t * (b - a);
		};
	lua["Math"]["saturate"] = [](float x) { return Math::saturate(x); };

	// min/max functions
	lua["Math"]["min"] = sol::overload(
		[](float a, float b) { return Math::min(a, b); },
		[](int a, int b) { return Math::min(a, b); },
		[](double a, double b) { return Math::min(a, b); }
	);
	lua["Math"]["max"] = sol::overload(
		[](float a, float b) { return Math::max(a, b); },
		[](int a, int b) { return Math::max(a, b); },
		[](double a, double b) { return Math::max(a, b); }
	);

	// Random number generation
	lua["Math"]["random"] = [](float min, float max) {
		return min + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min);
		};
	lua["Math"]["randomInt"] = [](int min, int max) {
		return min + (rand() % (max - min + 1));
		};
	lua["Math"]["randomFloat"] = [](float min, float max) {
		return min + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (max - min);
		};
	lua["Math"]["randomDouble"] = [](double min, double max) {
		return min + (static_cast<double>(rand()) / static_cast<double>(RAND_MAX)) * (max - min);
		};
	lua["Math"]["seedRandom"] = [](unsigned int seed) {
		srand(seed);
		};

	// Angle calculation functions (moved from registerMathTypes)
	lua["Math"]["getAngle"] = sol::overload(
		[](const Vec3& v0, const Vec3& v1) { return Math::getAngle(vec3(v0), vec3(v1)); },
		[](const Vec3& v0, const Vec3& v1, const Vec3& up) { return Math::getAngle(vec3(v0), vec3(v1), vec3(up)); }
	);

	// sign: -1, 0, or 1 (for FPC port)
	lua["Math"]["sign"] = [](float x) { return Math::sign(x); };

	// compare: 0 if equal within epsilon, -1 if a < b, 1 if a > b
	lua["Math"]["compare"] = sol::overload(
		[](float a, float b) { return Math::compare(a, b); },
		[](float a, float b, float epsilon) { return Math::compare(a, b, epsilon); }
	);
}
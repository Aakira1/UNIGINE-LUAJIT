#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include <UnigineVisualizer.h>
#include "LuaSystem.h"

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

void Unigine::LuaBindings::registerVizualizerAPI(sol::state& lua) {
	// create visualizer table
	lua["Visualizer"] = lua.create_table();

	// initialize visualizer
	lua["Visualizer"]["isInitialized"] = []() { return Visualizer::isInitialized(); };

	//Mode Control
	lua["Visualizer"]["setMode"] = [](Visualizer::MODE mode) { Visualizer::setMode(mode); };
	lua["Visualizer"]["getMode"] = []() { return Visualizer::getMode(); };
	lua["Visualizer"]["setEnabled"] = [](bool enabled) { Visualizer::setEnabled(enabled); };
	lua["Visualizer"]["isEnabled"] = []() { return Visualizer::isEnabled(); };

	// size and texture control
	lua["Visualizer"]["setSize"] = [](int size) { Visualizer::setSize(size); };
	lua["Visualizer"]["getSize"] = []() { return Visualizer::getSize(); };
	lua["Visualizer"]["setTextureName"] = [](const char* name) { Visualizer::setTextureName(name); };
	lua["Visualizer"]["getTextureName"] = []() { return Visualizer::getTextureName(); };

	// handler info
	lua["Visualizer"]["getNumHandlers"] = []() { return Visualizer::getNumHandlers(); };
	lua["Visualizer"]["getHandlerNode"] = [](int num) { return Visualizer::getHandlerNode(num); };
	lua["Visualizer"]["getHandlerSize"] = [](int num) { return Visualizer::getHandlerSize(num); };

	// clear visualizer
	lua["Visualizer"]["clear"] = []() { Visualizer::clear(); };

#pragma region 3DPrimitives
	// ============== 3D Primitives ==============

	// Point
	lua["Visualizer"]["renderPoint3D"] = sol::overload(
		[](const Vec3& v, float size, const vec4& color) {
			Visualizer::renderPoint3D(v, size, color);
		},
		[](const Vec3& v, float size, const vec4& color, bool screen_space) {
			Visualizer::renderPoint3D(v, size, color, screen_space);
		},
		[](const Vec3& v, float size, const vec4& color, bool screen_space, float duration) {
			Visualizer::renderPoint3D(v, size, color, screen_space, duration);
		},
		[](const Vec3& v, float size, const vec4& color, bool screen_space, float duration, bool depth_test) {
			Visualizer::renderPoint3D(v, size, color, screen_space, duration, depth_test);
		}
	);

    // Lines
    lua["Visualizer"]["renderLine3D"] = sol::overload(
        // 2 points
        [](const Vec3& v0, const Vec3& v1, const vec4& color) {
            Visualizer::renderLine3D(v0, v1, color);
        },
        [](const Vec3& v0, const Vec3& v1, const vec4& color, float duration) {
            Visualizer::renderLine3D(v0, v1, color, duration);
        },
        [](const Vec3& v0, const Vec3& v1, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderLine3D(v0, v1, color, duration, depth_test);
        },
        // 3 points
        [](const Vec3& v0, const Vec3& v1, const Vec3& v2, const vec4& color) {
            Visualizer::renderLine3D(v0, v1, v2, color);
        },
        [](const Vec3& v0, const Vec3& v1, const Vec3& v2, const vec4& color, float duration) {
            Visualizer::renderLine3D(v0, v1, v2, color, duration);
        },
        [](const Vec3& v0, const Vec3& v1, const Vec3& v2, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderLine3D(v0, v1, v2, color, duration, depth_test);
        }
        // Note: 4-point version exists but omitted for brevity - add if needed
    );

    // Triangle
    lua["Visualizer"]["renderTriangle3D"] = sol::overload(
        [](const Vec3& v0, const Vec3& v1, const Vec3& v2, const vec4& color) {
            Visualizer::renderTriangle3D(v0, v1, v2, color);
        },
        [](const Vec3& v0, const Vec3& v1, const Vec3& v2, const vec4& color, float duration) {
            Visualizer::renderTriangle3D(v0, v1, v2, color, duration);
        },
        [](const Vec3& v0, const Vec3& v1, const Vec3& v2, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderTriangle3D(v0, v1, v2, color, duration, depth_test);
        }
    );

    // Quad
    lua["Visualizer"]["renderQuad3D"] = sol::overload(
        [](const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& v3, const vec4& color) {
            Visualizer::renderQuad3D(v0, v1, v2, v3, color);
        },
        [](const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& v3, const vec4& color, float duration) {
            Visualizer::renderQuad3D(v0, v1, v2, v3, color, duration);
        },
        [](const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& v3, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderQuad3D(v0, v1, v2, v3, color, duration, depth_test);
        }
    );

    // Billboard
    lua["Visualizer"]["renderBillboard3D"] = sol::overload(
        [](const Vec3& v, float size, const vec4& texcoord) {
            Visualizer::renderBillboard3D(v, size, texcoord);
        },
        [](const Vec3& v, float size, const vec4& texcoord, bool screen_space) {
            Visualizer::renderBillboard3D(v, size, texcoord, screen_space);
        },
        [](const Vec3& v, float size, const vec4& texcoord, bool screen_space, float duration) {
            Visualizer::renderBillboard3D(v, size, texcoord, screen_space, duration);
        },
        [](const Vec3& v, float size, const vec4& texcoord, bool screen_space, float duration, bool depth_test) {
            Visualizer::renderBillboard3D(v, size, texcoord, screen_space, duration, depth_test);
        }
    );
#pragma endregion

#pragma region 2DPrimitives
    // ============== 2D Primitives ============

    // Point 2D
    lua["Visualizer"]["renderPoint2D"] = sol::overload(
        [](const vec2& v, float size, const vec4& color) {
            Visualizer::renderPoint2D(v, size, color);
        },
        [](const vec2& v, float size, const vec4& color, float order) {
            Visualizer::renderPoint2D(v, size, color, order);
        },
        [](const vec2& v, float size, const vec4& color, float order, float duration) {
            Visualizer::renderPoint2D(v, size, color, order, duration);
        }
    );

    // Line 2D
    lua["Visualizer"]["renderLine2D"] = sol::overload(
        // 2 points
        [](const vec2& v0, const vec2& v1, const vec4& color) {
            Visualizer::renderLine2D(v0, v1, color);
        },
        [](const vec2& v0, const vec2& v1, const vec4& color, float order) {
            Visualizer::renderLine2D(v0, v1, color, order);
        },
        [](const vec2& v0, const vec2& v1, const vec4& color, float order, float duration) {
            Visualizer::renderLine2D(v0, v1, color, order, duration);
        },
        // 3 points
        [](const vec2& v0, const vec2& v1, const vec2& v2, const vec4& color) {
            Visualizer::renderLine2D(v0, v1, v2, color);
        },
        [](const vec2& v0, const vec2& v1, const vec2& v2, const vec4& color, float order) {
            Visualizer::renderLine2D(v0, v1, v2, color, order);
        },
        [](const vec2& v0, const vec2& v1, const vec2& v2, const vec4& color, float order, float duration) {
            Visualizer::renderLine2D(v0, v1, v2, color, order, duration);
        }
    );

    // Triangle 2D
    lua["Visualizer"]["renderTriangle2D"] = sol::overload(
        [](const vec2& v0, const vec2& v1, const vec2& v2, const vec4& color) {
            Visualizer::renderTriangle2D(v0, v1, v2, color);
        },
        [](const vec2& v0, const vec2& v1, const vec2& v2, const vec4& color, float order) {
            Visualizer::renderTriangle2D(v0, v1, v2, color, order);
        },
        [](const vec2& v0, const vec2& v1, const vec2& v2, const vec4& color, float order, float duration) {
            Visualizer::renderTriangle2D(v0, v1, v2, color, order, duration);
        }
    );

    // Quad 2D
    lua["Visualizer"]["renderQuad2D"] = sol::overload(
        [](const vec2& v0, const vec2& v1, const vec2& v2, const vec2& v3, const vec4& color) {
            Visualizer::renderQuad2D(v0, v1, v2, v3, color);
        },
        [](const vec2& v0, const vec2& v1, const vec2& v2, const vec2& v3, const vec4& color, float order) {
            Visualizer::renderQuad2D(v0, v1, v2, v3, color, order);
        },
        [](const vec2& v0, const vec2& v1, const vec2& v2, const vec2& v3, const vec4& color, float order, float duration) {
            Visualizer::renderQuad2D(v0, v1, v2, v3, color, order, duration);
        }
    );
#pragma endregion

#pragma region DirectionalHelpers
	// ============== Directional Helpers ==============
    lua["Visualizer"]["renderVector"] = sol::overload(
        [](const Vec3& start, const Vec3& end, const vec4& color) {
            Visualizer::renderVector(start, end, color);
        },
        [](const Vec3& start, const Vec3& end, const vec4& color, float arrow_size) {
            Visualizer::renderVector(start, end, color, arrow_size);
        },
        [](const Vec3& start, const Vec3& end, const vec4& color, float arrow_size, bool screen_space) {
            Visualizer::renderVector(start, end, color, arrow_size, screen_space);
        },
        [](const Vec3& start, const Vec3& end, const vec4& color, float arrow_size, bool screen_space, float duration) {
            Visualizer::renderVector(start, end, color, arrow_size, screen_space, duration);
        },
        [](const Vec3& start, const Vec3& end, const vec4& color, float arrow_size, bool screen_space, float duration, bool depth_test) {
            Visualizer::renderVector(start, end, color, arrow_size, screen_space, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderDirection"] = sol::overload(
        [](const Vec3& position, const vec3& direction, const vec4& color) {
            Visualizer::renderDirection(position, direction, color);
        },
        [](const Vec3& position, const vec3& direction, const vec4& color, float arrow_size) {
            Visualizer::renderDirection(position, direction, color, arrow_size);
        },
        [](const Vec3& position, const vec3& direction, const vec4& color, float arrow_size, bool screen_space) {
            Visualizer::renderDirection(position, direction, color, arrow_size, screen_space);
        },
        [](const Vec3& position, const vec3& direction, const vec4& color, float arrow_size, bool screen_space, float duration) {
            Visualizer::renderDirection(position, direction, color, arrow_size, screen_space, duration);
        },
        [](const Vec3& position, const vec3& direction, const vec4& color, float arrow_size, bool screen_space, float duration, bool depth_test) {
            Visualizer::renderDirection(position, direction, color, arrow_size, screen_space, duration, depth_test);
        }
    );
#pragma endregion

#pragma region 3Dshapes_Wireframe
	// =================== 3d Shapes =================== - wireframe

    lua["Visualizer"]["renderBox"] = sol::overload(
        [](const vec3& size, const Mat4& transform, const vec4& color) {
            Visualizer::renderBox(size, transform, color);
        },
        [](const vec3& size, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderBox(size, transform, color, duration);
        },
        [](const vec3& size, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderBox(size, transform, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderSphere"] = sol::overload(
        [](float radius, const Mat4& transform, const vec4& color) {
            Visualizer::renderSphere(radius, transform, color);
        },
        [](float radius, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderSphere(radius, transform, color, duration);
        },
        [](float radius, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderSphere(radius, transform, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderCapsule"] = sol::overload(
        [](float radius, float height, const Mat4& transform, const vec4& color) {
            Visualizer::renderCapsule(radius, height, transform, color);
        },
        [](float radius, float height, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderCapsule(radius, height, transform, color, duration);
        },
        [](float radius, float height, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderCapsule(radius, height, transform, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderCylinder"] = sol::overload(
        [](float radius, float height, const Mat4& transform, const vec4& color) {
            Visualizer::renderCylinder(radius, height, transform, color);
        },
        [](float radius, float height, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderCylinder(radius, height, transform, color, duration);
        },
        [](float radius, float height, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderCylinder(radius, height, transform, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderCircle"] = sol::overload(
        [](float radius, const Mat4& transform, const vec4& color) {
            Visualizer::renderCircle(radius, transform, color);
        },
        [](float radius, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderCircle(radius, transform, color, duration);
        },
        [](float radius, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderCircle(radius, transform, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderCone"] = sol::overload(
        [](float radius, float angle, const Mat4& transform, const vec4& color) {
            Visualizer::renderCone(radius, angle, transform, color);
        },
        [](float radius, float angle, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderCone(radius, angle, transform, color, duration);
        },
        [](float radius, float angle, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderCone(radius, angle, transform, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderEllipse"] = sol::overload(
        [](const vec3& radius, const Mat4& transform, const vec4& color) {
            Visualizer::renderEllipse(radius, transform, color);
        },
        [](const vec3& radius, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderEllipse(radius, transform, color, duration);
        },
        [](const vec3& radius, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderEllipse(radius, transform, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderFrustum"] = sol::overload(
        [](const mat4& projection, const Mat4& transform, const vec4& color) {
            Visualizer::renderFrustum(projection, transform, color);
        },
        [](const mat4& projection, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderFrustum(projection, transform, color, duration);
        },
        [](const mat4& projection, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderFrustum(projection, transform, color, duration, depth_test);
        }
    );
#pragma endregion

#pragma region 3DShapes_Solid
	// =================== 3d Shapes =================== - solid
    lua["Visualizer"]["renderSolidBox"] = sol::overload(
        [](const vec3& size, const Mat4& transform, const vec4& color) {
            Visualizer::renderSolidBox(size, transform, color);
        },
        [](const vec3& size, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderSolidBox(size, transform, color, duration);
        },
        [](const vec3& size, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderSolidBox(size, transform, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderSolidSphere"] = sol::overload(
        [](float radius, const Mat4& transform, const vec4& color) {
            Visualizer::renderSolidSphere(radius, transform, color);
        },
        [](float radius, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderSolidSphere(radius, transform, color, duration);
        },
        [](float radius, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderSolidSphere(radius, transform, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderSolidCapsule"] = sol::overload(
        [](float radius, float height, const Mat4& transform, const vec4& color) {
            Visualizer::renderSolidCapsule(radius, height, transform, color);
        },
        [](float radius, float height, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderSolidCapsule(radius, height, transform, color, duration);
        },
        [](float radius, float height, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderSolidCapsule(radius, height, transform, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderSolidCylinder"] = sol::overload(
        [](float radius, float height, const Mat4& transform, const vec4& color) {
            Visualizer::renderSolidCylinder(radius, height, transform, color);
        },
        [](float radius, float height, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderSolidCylinder(radius, height, transform, color, duration);
        },
        [](float radius, float height, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderSolidCylinder(radius, height, transform, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderSolidEllipse"] = sol::overload(
        [](const vec3& radius, const Mat4& transform, const vec4& color) {
            Visualizer::renderSolidEllipse(radius, transform, color);
        },
        [](const vec3& radius, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderSolidEllipse(radius, transform, color, duration);
        },
        [](const vec3& radius, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderSolidEllipse(radius, transform, color, duration, depth_test);
        }
    );
#pragma endregion

#pragma region BoundsVisualization
	// ================== Bounds Visualization ==================
    lua["Visualizer"]["renderBoundBox"] = sol::overload(
        [](const BoundBox& bb, const Mat4& transform, const vec4& color) {
            Visualizer::renderBoundBox(bb, transform, color);
        },
        [](const BoundBox& bb, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderBoundBox(bb, transform, color, duration);
        },
        [](const BoundBox& bb, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderBoundBox(bb, transform, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderBoundSphere"] = sol::overload(
        [](const BoundSphere& bs, const Mat4& transform, const vec4& color) {
            Visualizer::renderBoundSphere(bs, transform, color);
        },
        [](const BoundSphere& bs, const Mat4& transform, const vec4& color, float duration) {
            Visualizer::renderBoundSphere(bs, transform, color, duration);
        },
        [](const BoundSphere& bs, const Mat4& transform, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderBoundSphere(bs, transform, color, duration, depth_test);
        }
    );

    // Node bounds
    lua["Visualizer"]["renderNodeBoundBox"] = sol::overload(
        [](const Ptr<Node>& node, const vec4& color) {
            Visualizer::renderNodeBoundBox(node, color);
        },
        [](const Ptr<Node>& node, const vec4& color, float duration) {
            Visualizer::renderNodeBoundBox(node, color, duration);
        },
        [](const Ptr<Node>& node, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderNodeBoundBox(node, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderNodeBoundSphere"] = sol::overload(
        [](const Ptr<Node>& node, const vec4& color) {
            if (node) Visualizer::renderNodeBoundSphere(node, color);
        },
        [](const Ptr<Node>& node, const vec4& color, float duration) {
            if (node) Visualizer::renderNodeBoundSphere(node, color, duration);
        },
        [](const Ptr<Node>& node, const vec4& color, float duration, bool depth_test) {
            if (node) Visualizer::renderNodeBoundSphere(node, color, duration, depth_test);
        }
    );

    // Object surface bounds
    lua["Visualizer"]["renderObjectSurfaceBoundBox"] = sol::overload(
        [](const Ptr<Object>& object, int surface, const vec4& color) {
            Visualizer::renderObjectSurfaceBoundBox(object, surface, color);
        },
        [](const Ptr<Object>& object, int surface, const vec4& color, float duration) {
            Visualizer::renderObjectSurfaceBoundBox(object, surface, color, duration);
        },
        [](const Ptr<Object>& object, int surface, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderObjectSurfaceBoundBox(object, surface, color, duration, depth_test);
        }
    );

    lua["Visualizer"]["renderObjectSurfaceBoundSphere"] = sol::overload(
        [](const Ptr<Object>& object, int surface, const vec4& color) {
            Visualizer::renderObjectSurfaceBoundSphere(object, surface, color);
        },
        [](const Ptr<Object>& object, int surface, const vec4& color, float duration) {
            Visualizer::renderObjectSurfaceBoundSphere(object, surface, color, duration);
        },
        [](const Ptr<Object>& object, int surface, const vec4& color, float duration, bool depth_test) {
            Visualizer::renderObjectSurfaceBoundSphere(object, surface, color, duration, depth_test);
        }
    );
#pragma endregion

#pragma region ObjectMeshRendering
	// =================== Object/Mesh Rendering ===================
    lua["Visualizer"]["renderObject"] = sol::overload(
        [](const Ptr<Object>& object, const vec4& color) {
            Visualizer::renderObject(object, color);
        },
        [](const Ptr<Object>& object, const vec4& color, float duration) {
            Visualizer::renderObject(object, color, duration);
        }
    );

    lua["Visualizer"]["renderObjectSurface"] = sol::overload(
        [](const Ptr<Object>& object, int surface, const vec4& color) {
            Visualizer::renderObjectSurface(object, surface, color);
        },
        [](const Ptr<Object>& object, int surface, const vec4& color, float duration) {
            Visualizer::renderObjectSurface(object, surface, color, duration);
        }
    );

    lua["Visualizer"]["renderSolidObject"] = sol::overload(
        [](const Ptr<Object>& object, const vec4& color) {
            Visualizer::renderSolidObject(object, color);
        },
        [](const Ptr<Object>& object, const vec4& color, float duration) {
            Visualizer::renderSolidObject(object, color, duration);
        }
    );

    lua["Visualizer"]["renderSolidObjectSurface"] = sol::overload(
        [](const Ptr<Object>& object, int surface, const vec4& color) {
            Visualizer::renderSolidObjectSurface(object, surface, color);
        },
        [](const Ptr<Object>& object, int surface, const vec4& color, float duration) {
            Visualizer::renderSolidObjectSurface(object, surface, color, duration);
        }
    );
#pragma endregion

#pragma region TextRendering
	// =================== text rendering ===================
    lua["Visualizer"]["renderMessage2D"] = sol::overload(
        [](const vec3& position, const vec3& center, const char* str, const vec4& color) {
            Visualizer::renderMessage2D(position, center, str, color);
        },
        [](const vec3& position, const vec3& center, const char* str, const vec4& color, int outline) {
            Visualizer::renderMessage2D(position, center, str, color, outline);
        },
        [](const vec3& position, const vec3& center, const char* str, const vec4& color, int outline, int font_size) {
            Visualizer::renderMessage2D(position, center, str, color, outline, font_size);
        },
        [](const vec3& position, const vec3& center, const char* str, const vec4& color, int outline, int font_size, float duration) {
            Visualizer::renderMessage2D(position, center, str, color, outline, font_size, duration);
        }
    );

    lua["Visualizer"]["renderMessage3D"] = sol::overload(
        [](const Vec3& position, const vec3& center, const char* str, const vec4& color) {
            Visualizer::renderMessage3D(position, center, str, color);
        },
        [](const Vec3& position, const vec3& center, const char* str, const vec4& color, int outline) {
            Visualizer::renderMessage3D(position, center, str, color, outline);
        },
        [](const Vec3& position, const vec3& center, const char* str, const vec4& color, int outline, int font_size) {
            Visualizer::renderMessage3D(position, center, str, color, outline, font_size);
        },
        [](const Vec3& position, const vec3& center, const char* str, const vec4& color, int outline, int font_size, float duration) {
            Visualizer::renderMessage3D(position, center, str, color, outline, font_size, duration);
        }
    );
#pragma endregion

#pragma region Miscellaneous
    // ============= Miscellaneous ===========

    lua["Visualizer"]["renderNodeHandler"] = sol::overload(
        [](const Ptr<Node>& node) { Visualizer::renderNodeHandler(node); },
        [](const Ptr<Node>& node, float duration) { Visualizer::renderNodeHandler(node, duration); }
    );

    lua["Visualizer"]["renderRectangle"] = sol::overload(
        [](const vec4& rectangle, const vec4& color) {
            Visualizer::renderRectangle(rectangle, color);
        },
        [](const vec4& rectangle, const vec4& color, float duration) {
            Visualizer::renderRectangle(rectangle, color, duration);
        }
    );
#pragma endregion

    // Constants - Modes
    lua["Visualizer"]["MODE_DISABLED"] = Visualizer::MODE_DISABLED;
    lua["Visualizer"]["MODE_ENABLED_DEPTH_TEST_ENABLED"] = Visualizer::MODE_ENABLED_DEPTH_TEST_ENABLED;
    lua["Visualizer"]["MODE_ENABLED_DEPTH_TEST_DISABLED"] = Visualizer::MODE_ENABLED_DEPTH_TEST_DISABLED;

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] Visualizer API registered\n");
}
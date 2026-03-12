#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include <UniginePhysics.h>
#include <UnigineWorld.h>
#include <UnigineWorlds.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

void Unigine::LuaBindings::registerPhysicsAPI(sol::state& lua)
{
    




















    // PhysicsIntersection
    

    lua.new_usertype<PhysicsIntersectionPtr>("PhysicsIntersection",
        sol::no_constructor,

        "create", []() { return PhysicsIntersection::create(); },

        "getType", [](const PhysicsIntersectionPtr& p) { return p->getType(); },
        "getTypeName", [](const PhysicsIntersectionPtr& p) { return p->getTypeName(); },

        "setPoint", [](PhysicsIntersectionPtr& p, const Vec3& v) { p->setPoint(v); },
        "getPoint", [](const PhysicsIntersectionPtr& p) { return p->getPoint(); },

        "setSurface", [](PhysicsIntersectionPtr& p, int s) { p->setSurface(s); },
        "getSurface", [](const PhysicsIntersectionPtr& p) { return p->getSurface(); },

        "setShape", [](PhysicsIntersectionPtr& p, const ShapePtr& s) { p->setShape(s); },
        "getShape", [](const PhysicsIntersectionPtr& p) { return p->getShape(); },

        sol::meta_function::to_string, [](const PhysicsIntersectionPtr& p) {
            return p
                ? "PhysicsIntersection(" + std::string(p->getTypeName()) + ")"
                : "PhysicsIntersection(null)";
        }
    );

    lua.new_usertype<PhysicsIntersectionNormalPtr>("PhysicsIntersectionNormal",
        sol::no_constructor,

        "create", []() { return PhysicsIntersectionNormal::create(); },

        "setNormal", [](PhysicsIntersectionNormalPtr& p, const vec3& n) { p->setNormal(n); },
        "getNormal", [](const PhysicsIntersectionNormalPtr& p) { return p->getNormal(); },

        // PhysicsIntersection base class methods (manually exposed)
        "getType", [](const PhysicsIntersectionNormalPtr& p) { return p->getType(); },
        "getTypeName", [](const PhysicsIntersectionNormalPtr& p) { return p->getTypeName(); },
        "setPoint", [](PhysicsIntersectionNormalPtr& p, const Vec3& v) { p->setPoint(v); },
        "getPoint", [](const PhysicsIntersectionNormalPtr& p) { return p->getPoint(); },
        "setSurface", [](PhysicsIntersectionNormalPtr& p, int s) { p->setSurface(s); },
        "getSurface", [](const PhysicsIntersectionNormalPtr& p) { return p->getSurface(); },
        "setShape", [](PhysicsIntersectionNormalPtr& p, const ShapePtr& s) { p->setShape(s); },
        "getShape", [](const PhysicsIntersectionNormalPtr& p) { return p->getShape(); }
    );

    
    // Shape
    

    lua.new_usertype<ShapePtr>("Shape",
        sol::no_constructor,

        "createShape", sol::overload(
            [](int t) { return Shape::createShape(t); },
            [](const char* n) { return Shape::createShape(n); }
        ),

        "getID", [](const ShapePtr& s) { return s->getID(); },
        "getType", [](const ShapePtr& s) { return s->getType(); },
        "getTypeName", [](const ShapePtr& s) { return s->getTypeName(); },

        "setBody", [](ShapePtr& s, const BodyPtr& b) { s->setBody(b); },
        "getBody", [](const ShapePtr& s) { return s->getBody(); },

        "setEnabled", [](ShapePtr& s, bool e) { s->setEnabled(e); },
        "isEnabled", [](const ShapePtr& s) { return s->isEnabled(); },

        "setTransform", [](ShapePtr& s, const Mat4& m) { s->setTransform(m); },
        "getTransform", [](const ShapePtr& s) { return s->getTransform(); },

        "clone", [](const ShapePtr& s) { return s->clone(); },
        
        // Collision detection - returns a Lua table of contacts
        "getCollision", [](ShapePtr& s, sol::this_state L) {
            Vector<ShapeContactPtr> contacts;
            s->getCollision(contacts);
            sol::table result = sol::state_view(L).create_table();
            for (int i = 0; i < contacts.size(); i++) {
                result[i + 1] = contacts[i];  // Lua uses 1-based indexing
            }
            return result;
        },

        sol::meta_function::to_string, [](const ShapePtr& s) {
            return s
                ? "Shape(" + std::string(s->getTypeName()) + ")"
                : "Shape(null)";
        }
    );

    lua.new_usertype<ShapeSpherePtr>("ShapeSphere",
        sol::no_constructor,

        "create", sol::overload(
            []() { return ShapeSphere::create(); },
            [](float r) { return ShapeSphere::create(r); }
        ),

        "setRadius", [](ShapeSpherePtr& s, float r) { s->setRadius(r); },
        "getRadius", [](const ShapeSpherePtr& s) { return s->getRadius(); }
    );

    lua.new_usertype<ShapeBoxPtr>("ShapeBox",
        sol::no_constructor,

        "create", sol::overload(
            []() { return ShapeBox::create(); },
            [](const vec3& v) { return ShapeBox::create(v); }
        ),

        "setSize", [](ShapeBoxPtr& s, const vec3& v) { s->setSize(v); },
        "getSize", [](const ShapeBoxPtr& s) { return s->getSize(); }
    );

    lua.new_usertype<ShapeCapsulePtr>("ShapeCapsule",
        sol::no_constructor,

        "create", [](const BodyPtr& body, float radius, float height) {
            return body ? ShapeCapsule::create(body, radius, height) : ShapeCapsulePtr();
        },

        // Size methods
        "setHeight", [](ShapeCapsulePtr& s, float height) { if (s) s->setHeight(height); },
        "getHeight", [](const ShapeCapsulePtr& s) { return s ? s->getHeight() : 0.0f; },
        "setRadius", [](ShapeCapsulePtr& s, float radius) { if (s) s->setRadius(radius); },
        "getRadius", [](const ShapeCapsulePtr& s) { return s ? s->getRadius() : 0.0f; },

        // Transform
        "setTransform", [](ShapeCapsulePtr& s, const Mat4& transform) {
            if (s) s->setTransform(transform);
        },
        "getTransform", [](const ShapeCapsulePtr& s) { return s ? s->getTransform() : Mat4(); },

        // Capsule-specific
        "getBottomCap", [](const ShapeCapsulePtr& s) { return s ? s->getBottomCap() : Vec3(0, 0, 0); },
        "getTopCap", [](const ShapeCapsulePtr& s) { return s ? s->getTopCap() : Vec3(0, 0, 0); },

        // Masks (Shape base class)
        "setPhysicsIntersectionMask", [](ShapeCapsulePtr& s, int mask) {
            if (s) s->setPhysicsIntersectionMask(mask);
        },
        "setCollisionMask", [](ShapeCapsulePtr& s, int mask) {
            if (s) s->setCollisionMask(mask);
        },
        "setExclusionMask", [](ShapeCapsulePtr& s, int mask) {
            if (s) s->setExclusionMask(mask);
        },
        
        // Collision detection - returns a Lua table of contacts
        "getCollision", [](ShapeCapsulePtr& s, sol::this_state L) {
            sol::table result = sol::state_view(L).create_table();
            if (!s) return result;
            Vector<ShapeContactPtr> contacts;
            s->getCollision(contacts);
            for (int i = 0; i < static_cast<int>(contacts.size()); i++) {
                result[i + 1] = contacts[i];  // Lua uses 1-based indexing
            }
            return result;
        }
    );

    lua["ShapeCapsule"]["cast"] = [](const ShapePtr& s) -> ShapeCapsulePtr {
        if (!s) return ShapeCapsulePtr();
        return checked_ptr_cast<ShapeCapsule>(s);
    };

    lua.new_usertype<ShapeCylinderPtr>("ShapeCylinder",
        sol::no_constructor,

        "create", []() { return ShapeCylinder::create(); },

        // Size methods (null-safe)
        "setRadius", [](ShapeCylinderPtr& s, float radius) { if (s) s->setRadius(radius); },
        "getRadius", [](const ShapeCylinderPtr& s) { return s ? s->getRadius() : 0.0f; },
        "setHeight", [](ShapeCylinderPtr& s, float height) { if (s) s->setHeight(height); },
        "getHeight", [](const ShapeCylinderPtr& s) { return s ? s->getHeight() : 0.0f; },

        // Transform
        "setTransform", [](ShapeCylinderPtr& s, const Mat4& transform) {
            if (s) s->setTransform(transform);
        },

        // Enable/disable
        "setEnabled", [](ShapeCylinderPtr& s, bool enabled) { if (s) s->setEnabled(enabled); },
        "isEnabled", [](const ShapeCylinderPtr& s) { return s ? s->isEnabled() : false; },

        // Collision (same as ShapeCapsule for object interaction)
        "getCollision", [](ShapeCylinderPtr& s, sol::this_state L) {
            sol::table result = sol::state_view(L).create_table();
            if (!s) return result;
            Vector<ShapeContactPtr> contacts;
            s->getCollision(contacts);
            for (int i = 0; i < static_cast<int>(contacts.size()); i++)
                result[i + 1] = contacts[i];
            return result;
        }
    );

    lua.new_usertype<ShapeContactPtr>("ShapeContact",
        sol::no_constructor,

        // Contact info (null-safe to avoid UniginePtr assertion)
        "getPoint", [](const ShapeContactPtr& c) { return c ? c->getPoint() : Vec3(0, 0, 0); },
        "getNormal", [](const ShapeContactPtr& c) { return c ? c->getNormal() : vec3(0, 0, 1); },
        "getDepth", [](const ShapeContactPtr& c) { return c ? c->getDepth() : 0.0f; },
        "getObject", [](const ShapeContactPtr& c) { return c ? c->getObject() : ObjectPtr(); }
    );

    lua.new_usertype<ShapeConvexPtr>("ShapeConvex",
        sol::no_constructor,

        "create", []() { return ShapeConvex::create(); },

        // Transform
        "setTransform", [](ShapeConvexPtr& s, const Mat4& transform) {
            s->setTransform(transform);
        },
        "getTransform", [](const ShapeConvexPtr& s) { return s->getTransform(); },

        // Enable/disable (Shape base)
        "setEnabled", [](ShapeConvexPtr& s, bool enabled) { s->setEnabled(enabled); },
        "isEnabled", [](const ShapeConvexPtr& s) { return s->isEnabled(); }
    );

    
    // Body
    

    lua.new_usertype<BodyPtr>("Body",
        sol::no_constructor,

        "createBody", sol::overload(
            [](int t) { return Body::createBody(t); },
            [](const char* n) { return Body::createBody(n); }
        ),

        "setObject", [](BodyPtr& b, const ObjectPtr& o) { b->setObject(o); },
        "getObject", [](const BodyPtr& b) { return b->getObject(); },

        "addShape", [](BodyPtr& b, const ShapePtr& s) { b->addShape(s); },
        "removeShape", [](BodyPtr& b, const ShapePtr& s) { b->removeShape(s); },
        "getShape", [](const BodyPtr& b, int i) { return b->getShape(i); },
        "getNumShapes", [](const BodyPtr& b) { return b->getNumShapes(); },

        "setEnabled", [](BodyPtr& b, bool e) { b->setEnabled(e); },
        "isEnabled", [](const BodyPtr& b) { return b->isEnabled(); },

        "renderVisualizer", [](BodyPtr& b) { b->renderVisualizer(); },

        sol::meta_function::to_string, [](const BodyPtr& b) {
            return b
                ? "Body(" + std::string(b->getTypeName()) + ")"
                : "Body(null)";
        }
    );

    lua.new_usertype<BodyRigidPtr>("BodyRigid",
        sol::no_constructor,

        "create", []() { return BodyRigid::create(); },

        "setMass", [](BodyRigidPtr& b, float m) { b->setMass(m); },
        "getMass", [](const BodyRigidPtr& b) { return b->getMass(); },

        "addForce", [](BodyRigidPtr& b, const vec3& f) { b->addForce(f); },
        "addImpulse", sol::overload(
            [](BodyRigidPtr& b, const vec3& r, const vec3& i) { if (b) b->addImpulse(r, i); },
            [](BodyRigidPtr& b, const vec3& r, const Vec3& i) { if (b) b->addImpulse(r, vec3(i)); },
            [](BodyRigidPtr& b, const Vec3& r, const Vec3& i) { if (b) b->addImpulse(vec3(r), vec3(i)); }
        ),

        // Frozen state (for object interaction)
        "setFrozen", [](BodyRigidPtr& b, bool frozen) { if (b) b->setFrozen(frozen); },
        "isFrozen", [](const BodyRigidPtr& b) { return b ? b->isFrozen() : false; },

        // World impulse (for object interaction)
        "addWorldImpulse", sol::overload(
            [](BodyRigidPtr& b, const Vec3& point, const vec3& impulse) {
                if (b) b->addWorldImpulse(point, impulse);
            },
            [](BodyRigidPtr& b, const Vec3& point, const Vec3& impulse) {
                if (b) b->addWorldImpulse(point, vec3(impulse));
            }
        )
    );

    lua["BodyRigid"]["cast"] = [](const BodyPtr& b) -> BodyRigidPtr {
        if (!b) return BodyRigidPtr();
        return checked_ptr_cast<BodyRigid>(b);
    };

    lua.new_usertype<BodyDummyPtr>("BodyDummy",
        sol::no_constructor,

        // Static creation
        "create", [](const ObjectPtr& obj) { return obj ? BodyDummy::create(obj) : BodyDummyPtr(); },

        // Transform methods
        "setTransform", [](BodyDummyPtr& b, const Mat4& t) { if (b) b->setTransform(t); },
        "getTransform", [](const BodyDummyPtr& b) { return b ? b->getTransform() : Mat4(); },

        // Shape methods (Body base class)
        "getNumShapes", [](const BodyDummyPtr& b) { return b ? b->getNumShapes() : 0; },
        "getShape", [](const BodyDummyPtr& b, int index) { return b ? b->getShape(index) : ShapePtr(); },
        "setShapeTransform", [](BodyDummyPtr& b, int num, const Mat4& transform) {
            if (b) b->setShapeTransform(num, mat4(transform));
        }
    );

    
    // Joint
    

    lua.new_usertype<JointPtr>("Joint",
        sol::no_constructor,

        "createJoint", sol::overload(
            [](int t) { return Joint::createJoint(t); },
            [](const char* n) { return Joint::createJoint(n); }
        ),

        "setBody0", [](JointPtr& j, const BodyPtr& b) { j->setBody0(b); },
        "setBody1", [](JointPtr& j, const BodyPtr& b) { j->setBody1(b); },

        "setEnabled", [](JointPtr& j, bool e) { j->setEnabled(e); },
        "isEnabled", [](const JointPtr& j) { return j->isEnabled(); }
    );

    lua.new_usertype<JointHingePtr>("JointHinge",
        sol::no_constructor,

        "create", []() { return JointHinge::create(); },

        "setAngularVelocity", [](JointHingePtr& j, float v) { j->setAngularVelocity(v); },
        "getAngularVelocity", [](const JointHingePtr& j) { return j->getAngularVelocity(); }
    );

    
    // Physics static API
    

    sol::table physics = lua.create_named_table("Physics");

    physics["isInitialized"] = &Physics::isInitialized;
    physics["setEnabled"] = &Physics::setEnabled;
    physics["isEnabled"] = &Physics::isEnabled;

    physics["getIntersection"] = sol::overload(
        [](const Vec3& a, const Vec3& b, int m, const PhysicsIntersectionPtr& i) {
            return Physics::getIntersection(a, b, m, i);
        },
        [](const Vec3& a, const Vec3& b, int m, const PhysicsIntersectionNormalPtr& i) {
            return Physics::getIntersection(a, b, m, i);
        }
    );

    // Physics settings (for character controllers)
    physics["getScale"] = []() { return Physics::getScale(); };
    physics["getGravity"] = []() { return Physics::getGravity(); };
    physics["getFrozenLinearVelocity"] = []() {
        return Physics::getFrozenLinearVelocity();
    };
    physics["getPenetrationTolerance"] = []() {
        return Physics::getPenetrationTolerance();
    };

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] Physics API registered\n");
}

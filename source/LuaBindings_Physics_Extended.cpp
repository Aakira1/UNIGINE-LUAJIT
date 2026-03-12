#include "CommonIncludes.h"
#include "LuaAPIBindings.h"
#include "LuaSystem.h"
#include <UniginePhysics.h>

using namespace sol;
using namespace Unigine;
using namespace Unigine::Math;

void Unigine::LuaBindings::registerPhysicsExtended(sol::state& lua)
{
    // Additional Body Types
    // BodyPath - Path-constrained body
    lua.new_usertype<BodyPathPtr>("BodyPath",
        sol::no_constructor,

        "create", sol::overload(
            []() { return BodyPath::create(); },
            [](const ObjectPtr& obj) { return BodyPath::create(obj); }
        ),

        "setPathName", [](BodyPathPtr& body, const char* name) { body->setPathName(name); },
        "getPathName", [](const BodyPathPtr& body) { return body->getPathName(); },

        "getClosestPosition", [](const BodyPathPtr& body, const Vec3& pos) { return body->getClosestPosition(pos); },
        "getClosestRotation", [](const BodyPathPtr& body, const Vec3& pos) { return body->getClosestRotation(pos); }
    );

    // BodyRope - Rope/cable simulation (inherits from BodyParticles)
    lua.new_usertype<BodyRopePtr>("BodyRope",
        sol::no_constructor,

        "create", sol::overload(
            []() { return BodyRope::create(); },
            [](const ObjectPtr& obj) { return BodyRope::create(obj); }
        )
    );

    // BodyCloth - Cloth simulation (inherits from BodyParticles)
    lua.new_usertype<BodyClothPtr>("BodyCloth",
        sol::no_constructor,

        "create", sol::overload(
            []() { return BodyCloth::create(); },
            [](const ObjectPtr& obj) { return BodyCloth::create(obj); }
        ),

        "setTwoSided", [](BodyClothPtr& body, int sided) { body->setTwoSided(sided); },
        "getTwoSided", [](const BodyClothPtr& body) { return body->getTwoSided(); },

        "getNumIndices", [](const BodyClothPtr& body) { return body->getNumIndices(); },
        "getParticleIndex", [](const BodyClothPtr& body, int num) { return body->getParticleIndex(num); }
    );

    // BodyWater - Water physics
    lua.new_usertype<BodyWaterPtr>("BodyWater",
        sol::no_constructor,

        "create", sol::overload(
            []() { return BodyWater::create(); },
            [](const ObjectPtr& obj) { return BodyWater::create(obj); }
        ),

        "setDensity", [](BodyWaterPtr& body, float density) { body->setDensity(density); },
        "getDensity", [](const BodyWaterPtr& body) { return body->getDensity(); },

        "setDepth", [](BodyWaterPtr& body, float depth) { body->setDepth(depth); },
        "getDepth", [](const BodyWaterPtr& body) { return body->getDepth(); },

        "setLinearDamping", [](BodyWaterPtr& body, float damping) { body->setLinearDamping(damping); },
        "getLinearDamping", [](const BodyWaterPtr& body) { return body->getLinearDamping(); },

        "setAngularDamping", [](BodyWaterPtr& body, float damping) { body->setAngularDamping(damping); },
        "getAngularDamping", [](const BodyWaterPtr& body) { return body->getAngularDamping(); }
    );

    // BodyParticles base class - shared methods for Rope and Cloth
    lua.new_usertype<BodyParticlesPtr>("BodyParticles",
        sol::no_constructor,

        "setNumIterations", [](BodyParticlesPtr& body, int iterations) { body->setNumIterations(iterations); },
        "getNumIterations", [](const BodyParticlesPtr& body) { return body->getNumIterations(); },

        "setLinearDamping", [](BodyParticlesPtr& body, float damping) { body->setLinearDamping(damping); },
        "getLinearDamping", [](const BodyParticlesPtr& body) { return body->getLinearDamping(); },

        "setLinearStretch", [](BodyParticlesPtr& body, float stretch) { body->setLinearStretch(stretch); },
        "getLinearStretch", [](const BodyParticlesPtr& body) { return body->getLinearStretch(); },

        "setRadius", [](BodyParticlesPtr& body, float radius) { body->setRadius(radius); },
        "getRadius", [](const BodyParticlesPtr& body) { return body->getRadius(); },

        "setMass", [](BodyParticlesPtr& body, float mass) { body->setMass(mass); },
        "getMass", [](const BodyParticlesPtr& body) { return body->getMass(); },

        "getNumParticles", [](const BodyParticlesPtr& body) { return body->getNumParticles(); }
    );

    // BodyFracture - Destructible geometry
    lua.new_usertype<BodyFracturePtr>("BodyFracture",
        sol::no_constructor,

        "create", sol::overload(
            []() { return BodyFracture::create(); },
            [](const ObjectPtr& obj) { return BodyFracture::create(obj); }
        ),

        "setDensity", [](BodyFracturePtr& body, float density) { body->setDensity(density); },
        "getDensity", [](const BodyFracturePtr& body) { return body->getDensity(); },

        "setBroken", [](BodyFracturePtr& body, bool broken) { body->setBroken(broken); },
        "isBroken", [](const BodyFracturePtr& body) { return body->isBroken(); },

        "setMass", [](BodyFracturePtr& body, float mass) { body->setMass(mass); },
        "getMass", [](const BodyFracturePtr& body) { return body->getMass(); }
    );

    // BodyRagdoll - Ragdoll physics
    lua.new_usertype<BodyRagdollPtr>("BodyRagdoll",
        sol::no_constructor,

        "create", sol::overload(
            []() { return BodyRagdoll::create(); },
            [](const ObjectPtr& obj) { return BodyRagdoll::create(obj); }
        ),

        "getNumBones", [](const BodyRagdollPtr& body) { return body->getNumBones(); },
        "getBoneName", [](const BodyRagdollPtr& body, int num) { return body->getBoneName(num); },
        "findBone", [](const BodyRagdollPtr& body, const char* name) { return body->findBone(name); },

        "setMass", [](BodyRagdollPtr& body, float mass) { body->setMass(mass); },
        "getMass", [](const BodyRagdollPtr& body) { return body->getMass(); },

        "setRigidity", [](BodyRagdollPtr& body, float rigidity) { body->setRigidity(rigidity); },
        "getRigidity", [](const BodyRagdollPtr& body) { return body->getRigidity(); }
    );

    
    // Additional Joint Types
    

    // JointFixed - Fixed constraint
    lua.new_usertype<JointFixedPtr>("JointFixed",
        sol::no_constructor,

        "create", sol::overload(
            []() { return JointFixed::create(); },
            [](const BodyPtr& body0, const BodyPtr& body1) { return JointFixed::create(body0, body1); },
            [](const BodyPtr& body0, const BodyPtr& body1, const Vec3& anchor) { return JointFixed::create(body0, body1, anchor); }
        ),

        "setWorldAnchor", [](JointFixedPtr& joint, const Vec3& anchor) { joint->setWorldAnchor(anchor); },
        "getWorldAnchor", [](const JointFixedPtr& joint) { return joint->getWorldAnchor(); }
    );

    // JointBall - Ball/spherical joint
    lua.new_usertype<JointBallPtr>("JointBall",
        sol::no_constructor,

        "create", sol::overload(
            []() { return JointBall::create(); },
            [](const BodyPtr& body0, const BodyPtr& body1) { return JointBall::create(body0, body1); },
            [](const BodyPtr& body0, const BodyPtr& body1, const Vec3& anchor) { return JointBall::create(body0, body1, anchor); }
        ),

        "setWorldAnchor", [](JointBallPtr& joint, const Vec3& anchor) { joint->setWorldAnchor(anchor); },
        "getWorldAnchor", [](const JointBallPtr& joint) { return joint->getWorldAnchor(); },

        "setAngularDamping", [](JointBallPtr& joint, float damping) { joint->setAngularDamping(damping); },
        "getAngularDamping", [](const JointBallPtr& joint) { return joint->getAngularDamping(); },

        "setAngularLimitFrom", [](JointBallPtr& joint, float from) { joint->setAngularLimitFrom(from); },
        "getAngularLimitFrom", [](const JointBallPtr& joint) { return joint->getAngularLimitFrom(); },

        "setAngularLimitTo", [](JointBallPtr& joint, float to) { joint->setAngularLimitTo(to); },
        "getAngularLimitTo", [](const JointBallPtr& joint) { return joint->getAngularLimitTo(); },

        "setWorldAxis", [](JointBallPtr& joint, const vec3& axis) { joint->setWorldAxis(axis); },
        "getWorldAxis", [](const JointBallPtr& joint) { return joint->getWorldAxis(); }
    );

    // JointPrismatic - Sliding/prismatic joint
    lua.new_usertype<JointPrismaticPtr>("JointPrismatic",
        sol::no_constructor,

        "create", sol::overload(
            []() { return JointPrismatic::create(); },
            [](const BodyPtr& body0, const BodyPtr& body1) { return JointPrismatic::create(body0, body1); },
            [](const BodyPtr& body0, const BodyPtr& body1, const Vec3& anchor, const vec3& axis) { return JointPrismatic::create(body0, body1, anchor, axis); }
        ),

        "setWorldAxis", [](JointPrismaticPtr& joint, const vec3& axis) { joint->setWorldAxis(axis); },
        "getWorldAxis", [](const JointPrismaticPtr& joint) { return joint->getWorldAxis(); },

        "setLinearDamping", [](JointPrismaticPtr& joint, float damping) { joint->setLinearDamping(damping); },
        "getLinearDamping", [](const JointPrismaticPtr& joint) { return joint->getLinearDamping(); },

        "setLinearLimitFrom", [](JointPrismaticPtr& joint, float from) { joint->setLinearLimitFrom(from); },
        "getLinearLimitFrom", [](const JointPrismaticPtr& joint) { return joint->getLinearLimitFrom(); },

        "setLinearLimitTo", [](JointPrismaticPtr& joint, float to) { joint->setLinearLimitTo(to); },
        "getLinearLimitTo", [](const JointPrismaticPtr& joint) { return joint->getLinearLimitTo(); }
    );

    // JointCylindrical - Cylindrical constraint
    lua.new_usertype<JointCylindricalPtr>("JointCylindrical",
        sol::no_constructor,

        "create", sol::overload(
            []() { return JointCylindrical::create(); },
            [](const BodyPtr& body0, const BodyPtr& body1) { return JointCylindrical::create(body0, body1); },
            [](const BodyPtr& body0, const BodyPtr& body1, const Vec3& anchor, const vec3& axis) { return JointCylindrical::create(body0, body1, anchor, axis); }
        ),

        "setWorldAxis", [](JointCylindricalPtr& joint, const vec3& axis) { joint->setWorldAxis(axis); },
        "getWorldAxis", [](const JointCylindricalPtr& joint) { return joint->getWorldAxis(); },

        "setAngularDamping", [](JointCylindricalPtr& joint, float damping) { joint->setAngularDamping(damping); },
        "getAngularDamping", [](const JointCylindricalPtr& joint) { return joint->getAngularDamping(); },

        "setLinearDamping", [](JointCylindricalPtr& joint, float damping) { joint->setLinearDamping(damping); },
        "getLinearDamping", [](const JointCylindricalPtr& joint) { return joint->getLinearDamping(); }
    );

    // JointWheel - Vehicle wheel simulation
    lua.new_usertype<JointWheelPtr>("JointWheel",
        sol::no_constructor,

        "create", sol::overload(
            []() { return JointWheel::create(); },
            [](const BodyPtr& body0, const BodyPtr& body1) { return JointWheel::create(body0, body1); },
            [](const BodyPtr& body0, const BodyPtr& body1, const Vec3& anchor, const vec3& axis0, const vec3& axis1) {
                return JointWheel::create(body0, body1, anchor, axis0, axis1);
            }
        ),

        "setLinearDamping", [](JointWheelPtr& joint, float damping) { joint->setLinearDamping(damping); },
        "getLinearDamping", [](const JointWheelPtr& joint) { return joint->getLinearDamping(); }
    );

    // JointSuspension - Vehicle suspension
    lua.new_usertype<JointSuspensionPtr>("JointSuspension",
        sol::no_constructor,

        "create", sol::overload(
            []() { return JointSuspension::create(); },
            [](const BodyPtr& body0, const BodyPtr& body1) { return JointSuspension::create(body0, body1); },
            [](const BodyPtr& body0, const BodyPtr& body1, const Vec3& anchor, const vec3& axis0, const vec3& axis1) {
                return JointSuspension::create(body0, body1, anchor, axis0, axis1);
            }
        ),

        "setLinearDamping", [](JointSuspensionPtr& joint, float damping) { joint->setLinearDamping(damping); },
        "getLinearDamping", [](const JointSuspensionPtr& joint) { return joint->getLinearDamping(); },

        "setLinearSpring", [](JointSuspensionPtr& joint, float spring) { joint->setLinearSpring(spring); },
        "getLinearSpring", [](const JointSuspensionPtr& joint) { return joint->getLinearSpring(); },

        "setLinearLimitFrom", [](JointSuspensionPtr& joint, float from) { joint->setLinearLimitFrom(from); },
        "getLinearLimitFrom", [](const JointSuspensionPtr& joint) { return joint->getLinearLimitFrom(); },

        "setLinearLimitTo", [](JointSuspensionPtr& joint, float to) { joint->setLinearLimitTo(to); },
        "getLinearLimitTo", [](const JointSuspensionPtr& joint) { return joint->getLinearLimitTo(); }
    );

    // JointPath - Path-following constraint
    lua.new_usertype<JointPathPtr>("JointPath",
        sol::no_constructor,

        "create", sol::overload(
            []() { return JointPath::create(); },
            [](const BodyPtr& body0, const BodyPtr& body1) { return JointPath::create(body0, body1); }
        ),

        "setLinearVelocity", [](JointPathPtr& joint, float velocity) { joint->setLinearVelocity(velocity); },
        "getLinearVelocity", [](const JointPathPtr& joint) { return joint->getLinearVelocity(); },

        "setLinearDamping", [](JointPathPtr& joint, float damping) { joint->setLinearDamping(damping); },
        "getLinearDamping", [](const JointPathPtr& joint) { return joint->getLinearDamping(); }
    );

    // JointParticles - Particle constraint
    lua.new_usertype<JointParticlesPtr>("JointParticles",
        sol::no_constructor,

        "create", sol::overload(
            []() { return JointParticles::create(); },
            [](const BodyPtr& body0, const BodyPtr& body1) { return JointParticles::create(body0, body1); }
        ),

        "setThreshold", [](JointParticlesPtr& joint, float threshold) { joint->setThreshold(threshold); },
        "getThreshold", [](const JointParticlesPtr& joint) { return joint->getThreshold(); }
    );

    LuaSystem* ls = LuaSystem::get();
    if (ls && ls->getLogInit())
        Log::message("[LuaBindings] Extended Physics types registered (Cloth, Ragdoll, Rope, Wheel, Suspension, etc.)\n");
}

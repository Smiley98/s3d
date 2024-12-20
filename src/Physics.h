#pragma once
#include "Math.h"
#include <vector>

enum ShapeType
{
    CIRCLE,
    BOX,
    PLANE
};

struct PhysicsBody
{
    Vector2 pos = V3_ZERO;
    Vector2 vel = V3_ZERO;

    // Drag must be between 0 and 1. Drag of 1 means no air resistance!
    float drag = 1.0f;

    // 1.0 means mass of 1, 0 means mass of infinity
    float invMass = 1.0f;

    // How collision detection is done (sphere or plane)
    ShapeType type = CIRCLE;

    union
    {
        Vector2 normal{};       // If type is SPHERE, use radius
        Vector2 extents;        // If type is BOX, use extents
        float radius;           // If type is PLANE, use normal
    };

    // Whether the body collided with another last physics update
    bool collision = false;

    // Coefficient of friction (both static & kinetic for simplicity)
    float frictionCoefficient = 1.0f;

    // Coefficient of restitution (1.0 = no energy lost, 0.0 = all energy lost)
    float restitutionCoefficient = 1.0f;

    bool Dynamic()
    {
        return invMass == 0.0f ? false : true;
    }
};

struct HitPair
{
    int a = -1;
    int b = -1;
    Vector2 mtv = V2_ZERO;
};

constexpr Vector2 GRAVITY = { 0.0f, -9.81f };
using PhysicsWorld = std::vector<PhysicsBody>;
using Collisions = std::vector<HitPair>;

// TODO -- If I *actually want* a generic physics system, make a class for it here.
// A *generic physics engine* is PROBABLY NOT a good idea because each object has its own rules.
// Its easier to write unique rules vs generic rules + hacks.
// Its okay to make re-usable functions like Seek & Flee, or UpdateMotion (kinematic movement) and even Rebound (impulse-based collision).
// Just be careful not to back yourself into a corner trying to generalize object-specific physics code!

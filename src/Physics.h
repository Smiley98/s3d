#pragma once
#include "Collision.h"
#include <vector>

enum ShapeType
{
    CIRCLE,
    BOX,
    PLANE
};

struct PhysicsBody
{
    Vector3 pos = V3_ZERO;
    Vector3 vel = V3_ZERO;

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

constexpr Vector2 GRAVITY = { 0.0f, -9.81f };
using PhysicsWorld = std::vector<PhysicsBody>;

// TODO -- If I *actually want* a generic physics system, make a class for it here.
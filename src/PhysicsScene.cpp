#include "PhysicsScene.h"
#include "Render.h"
#include "Camera.h"
#include "Physics.h"
#include "Collision.h"
#include <cassert>

void PhysicsStep(PhysicsWorld& world, float dt);
void UpdateMotion(PhysicsWorld& world, float dt);
Collisions DetectCollisions(PhysicsWorld& world);
void ResolveCollisions(PhysicsWorld& world, Collisions& collisions);

static PhysicsWorld fWorld{};

void PhysicsScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP));

	PhysicsBody ground;
	ground.type = PLANE;
	ground.normal = V2_UP;
	ground.pos = { 0.0f, -100.0f };
	ground.invMass = 0.0f;
	fWorld.push_back(ground);

	PhysicsBody circle1;
	circle1.type = CIRCLE;
	circle1.radius = 10.0f;
	circle1.pos = V2_ZERO;
	circle1.vel = V2_ZERO;
	fWorld.push_back(circle1);
}

void PhysicsScene::OnUnload()
{
    fWorld.clear();
}

void PhysicsScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Ortho(-100.0f * SCREEN_ASPECT, 100.0f * SCREEN_ASPECT, -100.0f, 100.0f, 0.1f, 10.0f);
	PhysicsStep(fWorld, dt);
}

void PhysicsScene::OnDraw()
{
	for (const PhysicsBody& body : fWorld)
	{
		Vector3 color = body.collision ? V3_RIGHT : V3_UP;
		switch (body.type)
		{
		case CIRCLE:
			DrawCircle(body.pos, body.radius, color);
			break;

		case BOX:
			break;

		case PLANE:
            // Screen extents are [-100 * aspect, 100 * aspect], so rect is rendering correctly!
			DrawRectangle(body.pos, 100.0f * SCREEN_ASPECT * 2.0f, 5.0f, { 0.75f, 0.5f, 0.33f });
			break;
		}
	}
}

void PhysicsStep(PhysicsWorld& world, float dt)
{
	UpdateMotion(world, dt);
	Collisions collisions = DetectCollisions(world);
	ResolveCollisions(world, collisions);
}

void UpdateMotion(PhysicsWorld& world, float dt)
{
    for (size_t i = 0; i < world.size(); i++)
    {
        PhysicsBody& body = world[i];
        if (body.Dynamic())
        {
            Vector2 acc = GRAVITY;
            body.vel *= powf(body.drag, dt);
            body.vel += acc * dt;
            body.pos += body.vel * dt;
        }
    }
}

Collisions DetectCollisions(PhysicsWorld& world)
{
    // Reset collision flag before hit-testing
    for (PhysicsBody& body : fWorld)
        body.collision = false;

    // Test all bodies for collision, store pairs of colliding bodies
    Collisions collisions{};
    for (size_t i = 0; i < world.size(); i++)
    {
        for (size_t j = i + 1; j < world.size(); j++)
        {
            PhysicsBody& a = world[i];
            PhysicsBody& b = world[j];

            Vector2 mtv = V2_ZERO;
            bool collision = false;
            if (a.type == CIRCLE && b.type == CIRCLE)
            {
                collision = CircleCircle(a.pos, a.radius, b.pos, b.radius, &mtv);
            }
            else if (a.type == CIRCLE && b.type == PLANE)
            {
                collision = CirclePlane(a.pos, a.radius, b.pos, b.normal, &mtv);
            }
            else if (a.type == PLANE && b.type == CIRCLE)
            {
                collision = CirclePlane(b.pos, b.radius, a.pos, a.normal, &mtv);
            }
            else
            {
                assert(false, "Invalid collision test");
            }
            a.collision |= collision;
            b.collision |= collision;

            if (collision)
            {
                HitPair hitPair;
                hitPair.a = i; hitPair.b = j;
                hitPair.mtv = mtv;
                collisions.push_back(hitPair);
            }
        }
    }
    return collisions;
}

void ResolveCollisions(PhysicsWorld& world, Collisions& collisions)
{
    // Pre-pass to ensure A is *always* dynamic and MTV points *FROM* B *TO* A
    for (HitPair& collision : collisions)
    {
        if (!world[collision.a].Dynamic())
        {
            int temp = collision.a;
            collision.a = collision.b;
            collision.b = temp;

            if (Dot(Normalize(collision.mtv), Normalize(world[collision.a].pos - world[collision.b].pos)) < 0.0f)
            {
                collision.mtv *= -1.0f;
            }
        }
    }

    for (HitPair& collision : collisions)
    {
        PhysicsBody& a = world[collision.a];
        PhysicsBody& b = world[collision.b];

        // No motion to resolve if both bodies are static (should never happen, but still)
        float invMassSum = a.invMass + b.invMass;
        assert(invMassSum > FLT_EPSILON);

        // Velocity of A relative to B
        Vector2 velBA = a.vel - b.vel;
        Vector2 mtvDir = Normalize(collision.mtv);
        float mtvMag = Length(collision.mtv);

        // How similar motion of A relative to B is to the direction we want to move A and/or B
        float t = Dot(velBA, mtvDir);

        // Don't change velocities if object are already moving away from each other
        // (Only change if velocities within are less than 90 degrees of each other)
        if (t > 0.0f)
            continue;

        float restitution = fminf(a.restitutionCoefficient, b.restitutionCoefficient);
        float normalImpulseMagnitude = -(1.0f + restitution) * t / invMassSum;

        // p = mv --> v = p / m
        Vector2 normalImpulse = mtvDir * normalImpulseMagnitude;
        a.vel += normalImpulse * a.invMass;
        b.vel -= normalImpulse * b.invMass;

        Vector2 frictionImpulseDirection = Normalize(velBA - (mtvDir * t));
        float frictionImpulseMagnitude = -Dot(velBA, frictionImpulseDirection) / invMassSum;
        float mu = sqrtf(a.frictionCoefficient * b.frictionCoefficient); // <-- Coulomb's Law (how to combine friction coefficients)
        frictionImpulseMagnitude = Clamp(frictionImpulseMagnitude, -normalImpulseMagnitude * mu, normalImpulseMagnitude * mu);

        // p = mv --> v = p / m
        Vector2 frictionImpulse = frictionImpulseDirection * frictionImpulseMagnitude;
        a.vel += frictionImpulse * a.invMass;
        b.vel -= frictionImpulse * b.invMass;
    }

    for (const HitPair& collision : collisions)
    {
        PhysicsBody& a = world[collision.a];
        PhysicsBody& b = world[collision.b];

        if (!b.Dynamic())
        {
            a.pos += collision.mtv;
        }
        else
        {
            a.pos += collision.mtv * 0.5f;
            b.pos -= collision.mtv * 0.5f;
        }
    }
}

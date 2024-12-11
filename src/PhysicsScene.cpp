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
	circle1.pos = V2_ZERO;//{ 0.0f, -100.0f };
	circle1.vel = V2_ZERO;//V2_UP * 50.0f;
	fWorld.push_back(circle1);
}

void PhysicsScene::OnUnload()
{
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
			// TODO -- Examine if 400 is in fact the correct width (shouldn't it be 200)?
			DrawRectangle(body.pos, 400.0f, 5.0f, { 0.75f, 0.5f, 0.33f });
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
        PhysicsBody& circle1 = world[i];
        if (circle1.Dynamic())
        {
            Vector2 acc = GRAVITY;
            circle1.vel *= powf(circle1.drag, dt);
            circle1.vel += acc * dt;
            circle1.pos += circle1.vel * dt;
        }
    }
}

Collisions DetectCollisions(PhysicsWorld& world)
{
    // Reset collision flag before hit-testing
    for (PhysicsBody& circle1 : fWorld)
        circle1.collision = false;

    Collisions collisions{};
    // Test all bodies for collision, store pairs of colliding objects
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
    // Pre-pass to ensure A is *always* dynamic and MTV points from B to A
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

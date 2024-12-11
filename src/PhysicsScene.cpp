#include "PhysicsScene.h"
#include "Render.h"
#include "Camera.h"
#include "Physics.h"

void PhysicsStep(PhysicsWorld& world, float dt);
void UpdateMotion(PhysicsWorld& world, float dt);

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
	circle1.pos = { 0.0f, -100.0f };
	circle1.vel = V2_UP * 50.0f;
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
	for (PhysicsBody& circle1 : fWorld)
		circle1.collision = false;

	UpdateMotion(world, dt);

	// 2. Detect collisions
	//List<HitPair> collisions = DetectCollisions();

	// 3. Resolve collisions
	//ResolveCollisions(collisions);


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

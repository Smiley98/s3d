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

	PhysicsBody body;
	body.type = CIRCLE;
	body.radius = 10.0f;
	body.pos = { 0.0f, -100.0f };
	body.vel = V2_UP * 50.0f;
	fWorld.push_back(body);
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
			break;
		}
	}
}

void PhysicsStep(PhysicsWorld& world, float dt)
{
	for (PhysicsBody& body : fWorld)
		body.collision = false;

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

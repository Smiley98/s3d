#include "CollisionScene.h"
#include "Collision.h"
#include "Render.h"
#include "Window.h"
#include "Time.h"
#include <imgui/imgui.h>

// Use to change cursor-object & test-object
enum ShapeType : int
{
	CIRCLE,
	CAPSULE,
	RECTANGLE,
	PLANE
};

Vector2 fPosition1 = { 0.0f,  500.0f };
Vector2 fPosition2 = { 0.0f, -500.0f };
Vector3 fColor1 = V3_UP;
Vector3 fColor2 = V3_UP;
ShapeType fShape1 = CIRCLE;
ShapeType fShape2 = CIRCLE;

constexpr float fSize = 1000.0f;
static Matrix fView = LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP);
static Matrix fProj = Ortho(-fSize * SCREEN_ASPECT, fSize * SCREEN_ASPECT, -fSize, fSize, 0.01f, 10.0f);

constexpr float r = 25.0f;
constexpr float hh = 75.0f;
constexpr float w = 150;
constexpr float h = 50;

void DrawShape(ShapeType type, Vector2 pos, float rot, Vector3 color);

void CollisionScene::OnCreate()
{
	SetView(fView);
	SetProj(fProj);
}

void CollisionScene::OnDestroy()
{
}

void CollisionScene::OnUpdate(float dt)
{
}

void CollisionScene::OnDraw()
{
	static float angle = 0.0f;
	static bool rotate = false;
	if (IsKeyPressed(KEY_R)) rotate = !rotate;
	if (rotate)
		angle += 100.0f * DEG2RAD * FrameTime();
	fPosition2 = ScreenToWorld(MousePosition(), fProj, fView);
	Vector2 right2 = Direction(angle);
	Vector2 up2 = Direction(angle + PI * 0.5f);

	Vector2 mtv = V2_ZERO;
	switch (fShape1)
	{
	case CIRCLE:
		switch (fShape2)
		{
		case CIRCLE:
			CircleCircle(fPosition1, r, fPosition2, r, &mtv);
			break;

		case CAPSULE:
			CircleCapsule(fPosition1, r, fPosition2, right2, r, hh, &mtv);
			break;

		case RECTANGLE:
			CircleRectangle(fPosition1, r, fPosition2, { hh, r }, &mtv);
			break;

		case PLANE:
			CirclePlane(fPosition1, r, fPosition2, up2, &mtv);
			break;
		}
		break;

	case CAPSULE:
		switch (fShape2)
		{
		case CIRCLE:
			break;

		case CAPSULE:
			break;

		case RECTANGLE:
			break;

		case PLANE:
			break;
		}
		break;

	case RECTANGLE:
		switch (fShape2)
		{
		case CIRCLE:
			break;

		case CAPSULE:
			break;

		case RECTANGLE:
			break;

		case PLANE:
			break;
		}
		break;
	}
	fPosition1 += mtv;

	DrawShape(fShape1, fPosition1, 0.0f, fColor1);
	DrawShape(fShape2, fPosition2, angle, fColor2);
}

void CollisionScene::OnDrawImGui()
{
	ImGui::RadioButton("Circle 1", (int*)&fShape1, CIRCLE); ImGui::SameLine();
	ImGui::RadioButton("Capsule 1", (int*)&fShape1, CAPSULE); ImGui::SameLine();
	ImGui::RadioButton("Rectangle 1", (int*)&fShape1, RECTANGLE); ImGui::Separator();

	ImGui::RadioButton("Circle 2", (int*)&fShape2, CIRCLE); ImGui::SameLine();
	ImGui::RadioButton("Capsule 2", (int*)&fShape2, CAPSULE); ImGui::SameLine();
	ImGui::RadioButton("Rectangle 2", (int*)&fShape2, RECTANGLE); ImGui::SameLine();
	ImGui::RadioButton("Plane 2", (int*)&fShape2, PLANE); ImGui::Separator();

	float limit = 2000.0f;
	Vector2 min = { -fSize * SCREEN_ASPECT, -fSize };
	Vector2 max = {  fSize * SCREEN_ASPECT,  fSize };
	ImGui::SliderFloat2("Position 1", &fPosition1.x, -limit, limit);
	ImGui::SliderFloat2("Position 2", &fPosition2.x, -limit, limit);
	fPosition1 = Clamp(fPosition1, min, max);
	fPosition2 = Clamp(fPosition2, min, max);
}

void DrawShape(ShapeType type, Vector2 pos, float rot, Vector3 color)
{
	Vector2 right = Direction(rot);
	Vector2 up = Direction(rot + PI * 0.5f);
	switch (type)
	{
	case CIRCLE:
		DrawCircle(pos, r, color);
		break;

	case CAPSULE:
		DrawCapsuleH(pos, r, hh, color, rot);
		break;

	case RECTANGLE:
		DrawRectangle(pos, w, h, color);
		break;

	case PLANE:
		DrawRectangle(pos, 500.0f, 5.0f, color, rot);
		DrawRectangle(pos + up * 50.0f, 100.0f, 5.0f, V3_RIGHT, rot + PI * 0.5f);
		break;
	}
}

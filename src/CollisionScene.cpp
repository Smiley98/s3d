#include "CollisionScene.h"
#include "Collision.h"
#include "Render.h"
#include "Window.h"
#include <imgui/imgui.h>

// Use to change cursor-object & test-object
enum ShapeType : int
{
	CIRCLE,
	CAPSULE,
	RECTANGLE
};

Vector2 fPosition1 = { 0.0f,  500.0f };
Vector2 fPosition2 = { 0.0f, -500.0f };
Vector3 fColor1 = V3_UP;
Vector3 fColor2 = V3_UP;
ShapeType fShape1 = CIRCLE;
ShapeType fShape2 = CIRCLE;

constexpr float fSize = 1000.0f;
static Matrix fView = LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP);
static Matrix fProj = Ortho(-fSize * SCREEN_ASPECT, fSize * SCREEN_ASPECT, -fSize, fSize, -10.0f, 10.0f);

constexpr float r = 25.0f;
constexpr float hh = 75.0f;
constexpr float w = 60;
constexpr float h = 40;

Vector2 ScreenToWorld(Vector2 screen); // ortho shortcut, look into Unproject
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
	Vector2 mouse = MousePosition();
	mouse = ScreenToNDC(mouse);
	mouse = NDCToScreen(mouse);
	printf("%f %f\n", mouse.x, mouse.y);
}

void CollisionScene::OnDraw()
{
	Vector2 mouse = MousePosition();
	DrawShape(fShape1, ScreenToWorld(mouse), 0.0f, fColor1);
	DrawShape(fShape2, fPosition2, 0.0f, fColor2);
}

void CollisionScene::OnDrawImGui()
{
	ImGui::RadioButton("Circle 1", (int*)&fShape1, CIRCLE); ImGui::SameLine();
	ImGui::RadioButton("Capsule 1", (int*)&fShape1, CAPSULE); ImGui::SameLine();
	ImGui::RadioButton("Rectangle 1", (int*)&fShape1, RECTANGLE); ImGui::Separator();

	ImGui::RadioButton("Circle 2", (int*)&fShape2, CIRCLE); ImGui::SameLine();
	ImGui::RadioButton("Capsule 2", (int*)&fShape2, CAPSULE); ImGui::SameLine();
	ImGui::RadioButton("Rectangle 2", (int*)&fShape2, RECTANGLE); ImGui::Separator();

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
	}
}

Vector2 ScreenToWorld(Vector2 screen)
{
	float x = Remap(screen.x, 0.0f, SCREEN_WIDTH,  -fSize * SCREEN_ASPECT, fSize * SCREEN_ASPECT);
	float y = Remap(SCREEN_HEIGHT - screen.y, 0.0f, SCREEN_HEIGHT, -fSize, fSize);
	return { x, y };
}

#include "CollisionScene.h"
#include "Collision.h"
#include "Render.h"
#include <imgui/imgui.h>

// Use to change cursor-object & test-object
enum ShapeType : int
{
	CIRCLE,
	CAPSULE,
	RECTANGLE
};

ShapeType fShape1 = CIRCLE;
ShapeType fShape2 = CIRCLE;

constexpr float fSize = 1000.0f;
static Matrix fView = LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP);
static Matrix fProj = Ortho(-fSize * SCREEN_ASPECT, fSize * SCREEN_ASPECT, -fSize, fSize, -10.0f, 10.0f);

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
	DrawCircle({}, 50.0f, { 1.0f, 0.0f, 0.0f });
}

void CollisionScene::OnDrawImGui()
{
	ImGui::RadioButton("Circle 1", (int*)&fShape1, 0); ImGui::SameLine();
	ImGui::RadioButton("Capsule 1", (int*)&fShape1, 1); ImGui::SameLine();
	ImGui::RadioButton("Rectangle 1", (int*)&fShape1, 2); ImGui::Separator();

	ImGui::RadioButton("Circle 2", (int*)&fShape2, 0); ImGui::SameLine();
	ImGui::RadioButton("Capsule 2", (int*)&fShape2, 1); ImGui::SameLine();
	ImGui::RadioButton("Rectangle 2", (int*)&fShape2, 2); ImGui::Separator();
}

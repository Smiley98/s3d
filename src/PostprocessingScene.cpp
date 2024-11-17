#include "PostprocessingScene.h"
#include "Window.h"
#include "Time.h"
#include "Render.h"
#include "Camera.h"

static Matrix fTransform1 = MatrixIdentity();
static Matrix fTransform2 = MatrixIdentity();
static Matrix fTransform3 = MatrixIdentity();

void PostprocessingScene::OnLoad()
{
	gCamera.position = { 0.0f, 0.0f, 5.0f };
}

void PostprocessingScene::OnUnload()
{
}

void PostprocessingScene::OnUpdate(float dt)
{
	float tt = TotalTime();
	float timeScale = 0.25f;
	float tx = tt * 4.0f * timeScale;
	float ty = tt * 8.0f * timeScale;
	float rot = 100.0f * tt * DEG2RAD;
	Matrix fig8 = Translate({ sinf(tx) * 6.0f, sinf(ty) * 2.0f, 0.0f });

	// Still have to invert this matrix, otherwise rotation is clockwise instead of counter-clockwise (right-handed systems rotate counter-clockwise)
	fTransform1 = Transpose(RotateY(rot));

	// Torus oriented about the xz plane by default, so rotate about x to make the hole point along z.
	fTransform2 = Invert(RotateX(PI * 0.5f) * fig8);

	// Since the hole points along y by default, no need to apply any 90 degree rotations!
	fTransform3 = Invert(Translate(1.5f, 0.0f, 0.0f) * RotateZ(rot) * fig8);

	// Update camera matrix
	UpdateFpsCameraDefault(gCamera, dt);

	// Raymarching uses the camera matrix, so no need to calculate the view matrix!
	//gView = ToView(gCamera);
}

void PostprocessingScene::OnDraw()
{
	float time = TotalTime();
	Vector2 resolution{ SCREEN_WIDTH, SCREEN_HEIGHT };
	Vector2 mouse = GetMousePosition() / resolution;
	mouse.y = 1.0f - mouse.y;
	mouse = mouse * 2.0f - 1.0f;

	Shader* shaders[]{ &gShaderRaymarchBase, &gShaderFractal2D, &gShaderFractal3D, &gShaderFractalOrb };
	static int shader = 0;
	if (IsKeyPressed(KEY_TAB))
		++shader %= 4;

	Matrix rot = FpsRotation(gCamera);
	BindShader(shaders[shader]);
	SendMat4("u_transform1", fTransform1);
	SendMat4("u_transform2", fTransform2);
	SendMat4("u_transform3", fTransform3);
	SendMat3("u_camRot", rot);
	SendVec3("u_camPos", gCamera.position);
	SendFloat("u_fov", tanf(90.0f * 0.5f * DEG2RAD));
	// Ensure the ray FoV of 90 degrees matches the perspective projection's FoV!
	
	SendVec2("u_resolution", resolution);
	SendVec2("u_mouse", mouse);
	SendFloat("u_time", time);
	DrawFsq();
	UnbindShader();
}

void PostprocessingScene::OnDrawImGui()
{
}

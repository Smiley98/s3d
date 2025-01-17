#include "NeonDrive.h"
#include "Time.h"
#include "Render.h"
#include "ImageLoader.h"
#include <vector>

static Texture2D fTextureWhite;
static Texture2D fTextureAlbedo;
static Texture2D fTextureGround;
static Framebuffer fGeometryBuffer;

static Vector3 fLightPosition = V3_ZERO;
static Vector3 fLightColor = V3_ONE;

struct HexagonGrid
{
	int rows = 0;
	int cols = 0;
	std::vector<int> values;
};

static HexagonGrid fGrid;

void GenGrid(HexagonGrid* grid, int rows, int cols)
{
	grid->rows = rows;
	grid->cols = cols;
	grid->values.resize(rows * cols);

	// Vertical borders
	for (int i = 0; i < rows; i += 2)
	{
		int left = i * cols;
		int right = i * cols + (cols - 1);
		grid->values[left] = 1;
		grid->values[right] = 1;
	}

	// Horizontal borders
	for (int i = 0; i < cols; i++)
	{
		grid->values[i] = 1;
		grid->values[(rows - 1) * cols + i] = 1;
	}

	// TODO - flip array, visuals are deceiving us
}

void NeonDriveScene::OnLoad()
{
	GenGrid(&fGrid, 32, 16);
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP));

	// World's most elaborate texture xD xD xD
	{
		uint32_t pixel = 0xFFFFFFFF;
		CreateTexture2D(&fTextureWhite, 1, 1, GL_RGB8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, &pixel);
	}

	// Original format = RGBA
	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/african_head_diffuse.png", &w, &h, &c);
		CreateTexture2D(&fTextureAlbedo, w, h, GL_RGB8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
		UnloadImage(pixels);
	}

	// Original format = RGB
	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/ground.png", &w, &h, &c);
		CreateTexture2D(&fTextureGround, w, h, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
		UnloadImage(pixels);
	}

	// Positions, normals, and albedo are all 3-component. Can convert to RGBA for albedo-specular.
	CreateFramebuffer(&fGeometryBuffer, SCREEN_WIDTH, SCREEN_HEIGHT);
	AddColor(&fGeometryBuffer, GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST);		// Positions
	AddColor(&fGeometryBuffer, GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST);		// Normals
	AddColor(&fGeometryBuffer, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST);	// Albedo
	AddDepth(&fGeometryBuffer);
	CompleteFramebuffer(&fGeometryBuffer);
}

void NeonDriveScene::OnUnload()
{
	DestroyFramebuffer(&fGeometryBuffer);
	DestroyTexture2D(&fTextureGround);
	DestroyTexture2D(&fTextureAlbedo);
	DestroyTexture2D(&fTextureWhite);
}

void NeonDriveScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(PI * 0.5f, SCREEN_ASPECT, 0.1f, 1000.0f);

	float tt = TotalTime();
	fLightPosition = { cosf(tt) * 10.0f, 0.0f, 0.0f };
}

void NeonDriveScene::OnDraw()
{
	//DrawMeshTexture(gMeshGround, Scale(100.0f, 100.0f, 1.0f), fTextureGround, 0);
	//DrawMeshNormals(gMeshTd, MatrixIdentity(), MatrixIdentity());

	//static int grid[3][5]
	//{
	//	{ 1, 1, 1, 1, 1 },
	//	{   1, 0, 0, 0, 1 },
	//	{ 1, 1, 1, 1, 1 }
	//};

	// TODO - Make a pipeline state save and load feature?
	// Pipeline pipeline = SavePipeline();
	// *Insert state changes & draw calls here*
	// LoadPipeline(&pipeline);
	{
		bool depthTest = DepthTest();
		bool depthWrite = DepthWrite();
		SetWireframes(true);
		SetDepthTest(false);
		SetDepthWrite(false);

		for (int i = 0; i < fGrid.rows; i++)
		{
			float y = (sqrtf(3.0f) * 0.5f) * (float)i;
			float xStart = i % 2 == 0 ? 0.0f : 1.5f;
			for (int j = 0; j < fGrid.cols; j++)
			{
				//Vector3 color = grid[i][j] == 0 ? V3_RIGHT : V3_UP;
				int index = i * fGrid.cols + j;
				Vector3 color = fGrid.values[index] == 0 ? V3_RIGHT : V3_UP;
				float x = xStart + 3.0f * (float)j;
				DrawMeshFlat(gMeshParticle, Translate(x, y, 0.0f), color);
			}
		}

		SetDepthWrite(depthWrite);
		SetDepthTest(depthTest);
		SetWireframes(false);
	}
	return;

	Matrix world = MatrixIdentity();//Scale(V3_ONE * 10.0f);
	Matrix mvp = world * gView * gProj;

	BindFramebuffer(fGeometryBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindTexture2D(fTextureWhite, 0);
	BindShader(&gShaderDeferred);

	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	SendInt("u_tex", 0);
	DrawMesh(gMeshTd);

	UnbindShader();
	UnbindTexture2D(fTextureWhite, 0);
	UnbindFramebuffer(fGeometryBuffer);

	BindTexture2D(fGeometryBuffer.colors[0], 0);
	BindTexture2D(fGeometryBuffer.colors[1], 1);
	BindTexture2D(fGeometryBuffer.colors[2], 2);
	BindShader(&gShaderDeferredLighting);

	Matrix S = Scale(V3_ONE * 10.0f);
	Matrix R = LookRotation(fLightPosition, gCamera.position, V3_UP);
	Matrix T = Translate(fLightPosition);
	world = S * R * T;
	mvp = world * gView * gProj;
	SendMat4("u_mvp", mvp);
	SendInt("u_positions", 0);
	SendInt("u_normals", 1);
	SendInt("u_albedo", 2);
	SendVec3("u_lightPosition", fLightPosition);
	SendVec3("u_lightColor", fLightColor);
	SendFloat("u_ambient", 1.0f);
	SendFloat("u_diffuse", 1.0f);
	SendVec2("u_viewportSize", { SCREEN_WIDTH, SCREEN_HEIGHT });
	SendVec2("u_viewportOffset", V2_ZERO);

	bool depthTest = DepthTest();
	bool depthWrite = DepthWrite();
	SetDepthTest(false);
	SetDepthWrite(false);
	DrawMesh(gMeshCircle);
	SetDepthWrite(depthWrite);
	SetDepthTest(depthTest);

	UnbindShader();
	UnbindTexture2D(fGeometryBuffer.colors[2], 2);
	UnbindTexture2D(fGeometryBuffer.colors[1], 1);
	UnbindTexture2D(fGeometryBuffer.colors[0], 0);

	//DrawMeshWireframes(gMeshCircle, world, fLightColor);
}

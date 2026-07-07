#include "fbo_test.h"

#include "CameraController.h"
#include "chunkMash.h"
#include "ControllerManager.h"
#include "GLFWCallbackManager.h"
#include "imgui/imgui.h"
#include "InputManager.h"
#include "l_mesh.h"
#include "load_blockData.h"
#include "m_model.h"
#include "register_block.h"
#include "Renderer.h"
#include "texture_manager.h"
#include "block_best.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// ==================== 构造函数 ====================
Test::FBOTest::FBOTest(GLFWwindow* window)
	: clearColor(0.2f, 0.4f, 0.4f)
	, deltatime(-1.0f)
	, window(window)
{
	// 初始化渲染器设置
	Renderer& renderer = Renderer::Instance();
	renderer.SetCullFace(true);

	// 深度反转设置
	glDepthFunc(GL_GREATER);

	// 加载方块数据
	LoadBlockData::loadBlockDataFromFile("asset/block/block_1.json");



	// 创建摄像机
	auto windowSize = InputManager::GetInstance().GetWindowSize();
	camera = new Camera(
		glm::mat4(0.0f),
		60.0f,
		static_cast<float>(windowSize.x) / windowSize.y,
		10000.0f,
		0.02f
	);

	// 设置摄像机控制器
	ControllerManager& controller_manager = ControllerManager::GetInstance();
	camera_controller = std::make_shared<CameraController>(camera, 10);
	controller_manager.AddController(camera_controller);

	// 设置帧缓冲回调
	GLFWCallbackManager::GetInstance().SetFramebufferCallback(
		[this](GLFWwindow* window, int w, int h) {
			this->framebuffer_size_callback(window, w, h);
		});

	// 创建帧缓冲
	createFramebuffers(windowSize.x, windowSize.y);

	// 创建全屏四边形
	setupQuad();

	// 创建着色器
	createShaders();

	// 创建坐标轴线条网格
	createAxisLines();
	// 创建测试网格
	createTestMeshes();

	// 创建 Uniform Buffers
	createUniformBuffers();

	// 初始化执行一次窗口回调
	framebuffer_size_callback(window, windowSize.x, windowSize.y);

	// 创建世界
	world = new World(0, camera->getPos());
}

// ==================== 析构函数 ====================
Test::FBOTest::~FBOTest() {
	delete mesh_line;
	delete world;
	GLFWCallbackManager::GetInstance().SetFramebufferCallback(nullptr);
}

// ==================== 初始化辅助函数 ====================
void Test::FBOTest::createAxisLines() {
	std::vector<LineVertex> vertices = {
		// X轴(红色)
		{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
		{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
		// Y轴(绿色)
		{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
		{glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
		// Z轴(蓝色)
		{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
		{glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
	};

	std::vector<unsigned int> indices = { 0, 1, 2, 3, 4, 5 };
	mesh_line = new L_Mesh(vertices, indices);
}

void Test::FBOTest::createFramebuffers(int width, int height) {
	// 不透明物体 FBO
	opaqueFBO = new FrameBuffer(width, height);
	opaqueColor = opaqueFBO->AddColorAttachment(GL_RGBA16F, GL_RGBA, GL_FLOAT);
	Texture* opaqueDepth = opaqueFBO->AddDepthTexture();
	opaqueFBO->SetDrawBuffer({ GL_COLOR_ATTACHMENT0 });

	// 透明物体 FBO
	transparentFBO = new FrameBuffer(width, height);
	accumTex = transparentFBO->AddColorAttachment(GL_RGBA16F, GL_RGBA, GL_FLOAT);
	revealTex = transparentFBO->AddColorAttachment(GL_RGBA16F, GL_RGBA, GL_FLOAT);

	// 共享深度纹理
	transparentFBO->AttachExternalDepthTexture(opaqueDepth);
	transparentFBO->SetDrawBuffer({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });
}

void Test::FBOTest::createShaders() {
	lineShader = new Shader(
		"src/shader/Line/V_lineShader.glsl",
		"src/shader/Line/F_lineShader.glsl", "");

	chunkShader = new Shader(
		"src/shader/chunk/VertexShader.glsl",
		"src/shader/chunk/FragmentShader.glsl", "");

	transparentShader_a = new Shader(
		"src/shader/wboit/wboit_accum.vert.glsl",
		"src/shader/wboit/wboit_accum.frag.glsl", "");

	transparentShader_r = new Shader(
		"src/shader/wboit/wboit_accum.vert.glsl",
		"src/shader/wboit/wboit_Revealage.frag.glsl", "");

	compositeShader = new Shader(
		"src/shader/wboit/wboit_composite.vert.glsl",
		"src/shader/wboit/wboit_composite.frag.glsl", "");

	// 设置纹理采样器
	chunkShader->use();
	chunkShader->uniformsetInt("Texture1", 0);

	transparentShader_a->use();
	transparentShader_a->uniformsetInt("texture_base", 0);

	transparentShader_r->use();
	transparentShader_r->uniformsetInt("texture_base", 0);

	compositeShader->use();
	compositeShader->uniformsetInt("opaqueTexture", 0);
	compositeShader->uniformsetInt("accumulationTexture", 1);
	compositeShader->uniformsetInt("revealageTexture", 2);
}

void Test::FBOTest::createTestMeshes() {
	std::vector<Texture*> textures = {
		TextureManager::getInstance()->loadTexture("asset/image/t_t_1.png", TextureType::translucent)
	};
	std::vector<Vertex> vertex = blockVertex;
	std::vector<unsigned int> indices = blockIndices;
	triangleMesh = new M_Mesh(vertex, indices, textures);
}

void Test::FBOTest::createUniformBuffers() {
	UBO_Proj = new UniformBuffer(sizeof(glm::mat4), GL_STATIC_DRAW);
	UBO_View0 = new UniformBuffer(sizeof(glm::mat4), GL_DYNAMIC_DRAW);
	UBO_View1 = new UniformBuffer(sizeof(glm::mat4), GL_DYNAMIC_DRAW);
}

// ==================== 更新循环 ====================
void Test::FBOTest::update(double deltatime) {
	this->deltatime = deltatime;

	// 更新投影矩阵
	if (camera->isUpdatePJ) {
		updateShaderProjection(camera->getProjection());
		camera->isUpdatePJ = false;
	}

	// 处理方块交互
	processBlockRemoval();
	processBlockPlacement();

	// 更新世界
	world->update(deltatime, camera->getPos());
}

void Test::FBOTest::processBlockRemoval() {
	if (!is_set_air) return;

	InputManager& input_manager = InputManager::GetInstance();
	if (!input_manager.IsMouseButtonPressed(MouseButton::RIGHT)) return;

	glm::i64vec4 res = RaycastBlock(camera, world);
	glm::ivec3 chunkPos = worldToChunk(glm::dvec3(res.x, res.y, res.z));
	glm::ivec3 blockPos = worldToChunkLocal(glm::dvec3(res.x, res.y, res.z));

	Chunk* chunk = world->getChunkorCreate(chunkPos);
	if (!chunk) {
		std::cout << "\033[31m [error] \033[0m [mc_test2] : setair chunk is nullptr"
			<< glm::to_string(chunkPos) << std::endl;
		return;
	}

	{
		std::lock_guard<std::mutex> lock(chunk->chunkMutex);
		chunk->setBlock(blockPos.x, blockPos.y, blockPos.z,
			RegisterBlock::getInstance()->getBlockState_NAME("air").ID);
		chunk->buildMeshData();
		chunk->buildMeshVertex();
	}
}
void Test::FBOTest::processBlockPlacement() {
	if (block_cd <= 0.2f) {
		block_cd += deltatime;
		return;
	}

	InputManager& input_manager = InputManager::GetInstance();
	if (!is_set_block || !input_manager.IsMouseButtonPressed(MouseButton::RIGHT)) return;

	glm::i64vec4 res = RaycastBlock(camera, world);
	if (res.w == -1) return;

	// 根据碰撞面偏移坐标
	switch (res.w) {
	case 0: res.z++; break;
	case 1: res.z--; break;
	case 2: res.y--; break;
	case 3: res.y++; break;
	case 4: res.x++; break;
	case 5: res.x--; break;
	}

	glm::dvec3 worldPos(res.x, res.y, res.z);
	glm::ivec3 chunkPos = worldToChunk(worldPos);
	glm::ivec3 blockPos = worldToChunkLocal(worldPos);

	Chunk* chunk = world->getChunkorCreate(chunkPos);
	if (!chunk) {
		std::cout << "\033[31m [error] \033[0m [mc_test2] : setblock chunk is nullptr"
			<< glm::to_string(chunkPos) << std::endl;
		return;
	}

	{
		std::lock_guard<std::mutex> lock(chunk->chunkMutex);
		chunk->setBlock(blockPos.x, blockPos.y, blockPos.z,
			RegisterBlock::getInstance()->getBlockState_NAME("red").ID);
		chunk->buildMeshData();
		chunk->buildMeshVertex();
	}

	world->addRenderChunk(chunk);
	block_cd = 0.0f;
}

// ==================== 渲染 ====================
void Test::FBOTest::Render() {
	Renderer* renderer = &Renderer::Instance();
	renderer->SetPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// 更新视图矩阵 Uniform Buffers
	updateViewUniforms();

	// 渲染不透明阶段
	renderOpaquePass();

	// 渲染透明阶段
	renderTransparentPass();
	renderer->SetPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// 最终合成
	renderCompositePass();
}

void Test::FBOTest::updateViewUniforms() {
	// 更新完整视图矩阵 (绑定点 1)
	UBO_View0->Bind();
	UBO_View0->SetData(0, sizeof(glm::mat4), glm::value_ptr(camera->getView()));
	UBO_View0->BindRangeToBindingPoint(1, 0, sizeof(glm::mat4));

	UBO_View0->ConnectBlockToBindingPoint(lineShader->getID(), "View", 1);

	// 更新仅旋转的视图矩阵 (绑定点 2)
	UBO_View1->Bind();
	UBO_View1->SetData(0, sizeof(glm::mat4), glm::value_ptr(glm::mat4(glm::mat3(camera->getView()))));
	UBO_View1->BindRangeToBindingPoint(2, 0, sizeof(glm::mat4));

	UBO_View1->ConnectBlockToBindingPoint(chunkShader->getID(), "View", 2);
	UBO_View1->ConnectBlockToBindingPoint(transparentShader_a->getID(), "View", 2);
	UBO_View1->ConnectBlockToBindingPoint(transparentShader_r->getID(), "View", 2);
}

void Test::FBOTest::renderOpaquePass() {
	Renderer* renderer = &Renderer::Instance();

	opaqueFBO->Bind();

	// 不透明渲染状态设置
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// 清除缓冲
	renderer->setClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
	renderer->setClearDepth(0.0f);
	renderer->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 绘制坐标轴
	lineShader->use();
	lineShader->uniformsetMat4("model", glm::mat4(1.0f));
	glLineWidth(2.0f);
	mesh_line->Draw(*lineShader);

	// 绘制场景块
	chunkShader->use();
	world->Draw(*chunkShader, camera->getPos());

	// 绘制传统透明物体测试
	renderTranslucentTestObjects();
}

void Test::FBOTest::renderTranslucentTestObjects() {
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	chunkShader->use();

	std::vector<glm::vec3> positions = {
		glm::vec3(0, 1, 0),
		glm::vec3(1, 1, 0),
		glm::vec3(2, 1, 0),
		glm::vec3(3, 1, 0),
		glm::vec3(4, 1, 0)
	};

	for (const auto& pos : positions) {
		glm::vec3 relativePos = pos - glm::vec3(camera->getPos());
		chunkShader->uniformsetVec3("chunkRelativePos", relativePos);
		triangleMesh->Draw(*chunkShader);
	}
}

void Test::FBOTest::renderTransparentPass() {
	transparentFBO->Bind();

	// 设置两个颜色附件
	GLenum allBufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, allBufs);

	// 清空附件
	float color0[] = { 0, 0, 0, 0 };
	glClearBufferfv(GL_COLOR, 0, color0);
	float color1[] = { 1, 1, 1, 1 };
	glClearBufferfv(GL_COLOR, 1, color1);

	// 设置深度测试（不写入）
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glDepthMask(GL_FALSE);

	// 渲染累积通道
	renderAccumulationPass();

	// 渲染揭示度通道
	renderRevealagePass();

	transparentFBO->Unbind();
}

void Test::FBOTest::renderAccumulationPass() {
	GLenum drawBuf0 = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &drawBuf0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	transparentShader_a->use();

	std::vector<glm::vec3> positions = {
		glm::vec3(0, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(2, 0, 0),
		glm::vec3(3, 0, 0),
		glm::vec3(4, 0, 0)
	};

	for (const auto& pos : positions) {
		glm::vec3 relativePos = pos - glm::vec3(camera->getPos());
		transparentShader_a->uniformsetVec3("chunkRelativePos", relativePos);
		triangleMesh->Draw(*transparentShader_a);
	}
}

void Test::FBOTest::renderRevealagePass() {
	GLenum drawBuf1 = GL_COLOR_ATTACHMENT1;
	glDrawBuffers(1, &drawBuf1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

	transparentShader_r->use();

	std::vector<glm::vec3> positions = {
		glm::vec3(0, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(2, 0, 0),
		glm::vec3(3, 0, 0),
		glm::vec3(4, 0, 0)
	};

	for (const auto& pos : positions) {
		glm::vec3 relativePos = pos - glm::vec3(camera->getPos());
		transparentShader_r->uniformsetVec3("chunkRelativePos", relativePos);
		triangleMesh->Draw(*transparentShader_r);
	}
}

void Test::FBOTest::renderCompositePass() {
	// 切换到默认帧缓冲
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(0.0f);
	glClear(GL_COLOR_BUFFER_BIT);



	// 绘制全屏四边形
	quadMesh->Draw(*compositeShader);
}

// ==================== GUI ====================
void Test::FBOTest::GuiRender() {
	renderCrosshair();
	renderSettingsWindow();
	renderDebugWindow();
}

void Test::FBOTest::renderCrosshair() {
	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoFocusOnAppearing;

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::Begin("Crosshair", nullptr, window_flags);

	auto windowSize = InputManager::GetInstance().GetWindowSize();
	ImVec2 center(windowSize.x * 0.5f, windowSize.y * 0.5f);

	const float length = 10.0f;
	const float thickness = 2.0f;

	ImDrawList* draw = ImGui::GetWindowDrawList();
	draw->AddLine(
		ImVec2(center.x - length, center.y),
		ImVec2(center.x + length, center.y),
		IM_COL32_WHITE, thickness);
	draw->AddLine(
		ImVec2(center.x, center.y - length),
		ImVec2(center.x, center.y + length),
		IM_COL32_WHITE, thickness);

	ImGui::End();
}

void Test::FBOTest::renderSettingsWindow() {
	ImGui::Begin("setting");

	ImGui::DragFloat("speed", &camera_controller->getSpeed(), 0.1f);

	if (ImGui::Button("set this block")) {
		setBlockAtCamera("stone");
	}

	if (ImGui::Button("set this air")) {
		setBlockAtCamera("air");
	}

	ImGui::Checkbox("set block_dir", &is_set_block);
	ImGui::Checkbox("set air_dir", &is_set_air);

	ImGui::End();
}

void Test::FBOTest::renderDebugWindow() {
	ImGui::Begin("Debug Window");

	isback = ImGui::Button("<-");
	ImGui::Text("FPS: %.1f", 1.0f / deltatime);
	ImGui::ColorEdit3("选择颜色", (float*)&clearColor);

	glm::vec3 front = camera->getFront();
	if (ImGui::DragFloat3("camera front", (float*)&front, 0.1f)) {
		camera->setFront(front);
	}

	ImGui::DragScalarN(
		"Camera Pos",
		ImGuiDataType_Double,
		&camera->getPosRef(), 3, 0.1,
		nullptr, nullptr, "%.6f", 1.0);

	ImGui::End();
}

void Test::FBOTest::setBlockAtCamera(const std::string& blockName) {
	glm::ivec3 chunkPos = worldToChunk(camera->getPos());
	glm::ivec3 blockPos = worldToChunkLocal(camera->getPos());

	Chunk* chunk = world->getChunkorCreate(chunkPos);
	if (chunk) {
		chunk->setBlock(blockPos.x, blockPos.y, blockPos.z,
			RegisterBlock::getInstance()->getBlockState_NAME(blockName).ID);
		chunk->buildMeshData();
		chunk->buildMeshVertex();
	}
	else {
		std::cout << "\033[31m error: chunk is nullptr at \033[0m"
			<< glm::to_string(chunkPos) << std::endl;
	}
}

// ==================== 回调函数 ====================
void Test::FBOTest::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	camera->setProjection(
		camera->getFov(),
		static_cast<float>(width) / height,
		camera->getNearFar().x,
		camera->getNearFar().y);

	updateShaderProjection(camera->getProjection());

	opaqueFBO->Resize(width, height);
	transparentFBO->Resize(width, height);
}

// ==================== 辅助函数 ====================
void Test::FBOTest::setupQuad() {
	static std::vector<Vertex2D> quadVertices = {
		{{-1.0f, -1.0f}, {0.0f, 0.0f}},
		{{ 1.0f, -1.0f}, {1.0f, 0.0f}},
		{{-1.0f,  1.0f}, {0.0f, 1.0f}},
		{{ 1.0f,  1.0f}, {1.0f, 1.0f}}
	};
	static std::vector<unsigned int> quadIndices = { 0, 1, 2, 1, 3, 2 };

	std::vector<Texture*> quadtextures = { opaqueColor, accumTex, revealTex };
	quadMesh = new Mesh_2D(quadVertices, quadIndices, quadtextures);
}

void Test::FBOTest::updateShaderProjection(const glm::mat4& projection) {
	UBO_Proj->Bind();
	UBO_Proj->SetData(0, sizeof(glm::mat4), glm::value_ptr(projection));
	UBO_Proj->BindToBindingPoint(0);

	UBO_Proj->ConnectBlockToBindingPoint(chunkShader->getID(), "Proj", 0);
	UBO_Proj->ConnectBlockToBindingPoint(lineShader->getID(), "Proj", 0);
	UBO_Proj->ConnectBlockToBindingPoint(transparentShader_a->getID(), "Proj", 0);
	UBO_Proj->ConnectBlockToBindingPoint(transparentShader_r->getID(), "Proj", 0);
}
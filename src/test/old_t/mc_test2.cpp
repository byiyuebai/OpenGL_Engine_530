#include "mc_test2.h"

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

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <iostream>



Test::McTest2::McTest2(GLFWwindow* window)
	: clearColor(0.2f, 0.4f, 0.4f),
	deltatime(-1.0f),
	window(window),
	offsetPos1(0.0f, 0.0f, 0.0f),
	model1(nullptr)
{

	Renderer& renderer = Renderer::Instance();
	renderer.SetDrawMode(DrawMode::Opaque);
	renderer.SetCullFace(true);

	glDepthFunc(GL_GREATER);
	glClearDepth(0.0f);



	LoadBlockData::loadBlockDataFromFile("asset/block/block_1.json");


	std::vector<LineVertex> Z_v = {
		{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
		{glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},

		{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
		{glm::vec3(0.0f, 1.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},

		{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
		{glm::vec3(0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
	};

	std::vector<unsigned int> Z_i = {
		0, 1,
		2, 3,
		4, 5,
	};

	camera = new Camera(glm::mat4(0.0f), 60.0f, (float)InputManager::GetInstance().GetWindowSize().x / InputManager::GetInstance().GetWindowSize().y, 10000.0f, 0.02f);

	shader = new Shader("src/shader/VertexShader.glsl", "src/shader/FragmentShader.glsl", "");
	shader->use();
	shader->uniformsetMat4("projection", camera->getProjection());

	normalshader = new Shader("src/shader/normal/VertexShader.glsl", "src/shader/normal/FragmentShader.glsl", "src/shader/normal/GeometryShader.glsl");
	normalshader->use();
	normalshader->uniformsetMat4("projection", camera->getProjection());

	lineShader = new Shader("src/shader/Line/V_lineShader.glsl", "src/shader/Line/F_lineShader.glsl", "");
	lineShader->use();
	lineShader->uniformsetMat4("projection", camera->getProjection());

	chunkShader = new Shader("src/shader/chunk/VertexShader.glsl", "src/shader/chunk/FragmentShader.glsl", "");
	chunkShader->use();
	chunkShader->uniformsetMat4("projection", camera->getProjection());

	shader_2D = new Shader("src/shader/2Dshader/VertexShader.glsl", "src/shader/2Dshader/FragmentShader.glsl", "");
	shader_2D->use();


	framebuffer_size_callback(window, InputManager::GetInstance().GetWindowSize().x, InputManager::GetInstance().GetWindowSize().y);

	mesh_line = new L_Mesh(Z_v, Z_i);

	model1 = new Model("asset/model/survival_guitar_backpack/scene.gltf");
	model2 = new Model("asset/model/ys/scene.gltf");
	model3 = new Model("asset/model/a2121/ht.gltf");


	world = new World(0, camera->getPos());

	setting1[0] = 0.85;
	setting1[1] = -0.85;
	setting1[2] = 0.05;


	//控制器管理器
	ControllerManager& controller_manager = ControllerManager::GetInstance();
	//摄像机控制器
	camera_controller = std::make_shared<CameraController>(camera, 10);

	controller_manager.AddController(camera_controller);

	GLFWCallbackManager::GetInstance().SetFramebufferCallback(
		[this](GLFWwindow* window, int w, int h) {
			this->framebuffer_size_callback(window, w, h); // 成员函数
		});

	offsetPos1 = glm::vec3(0, 1, 0);

	glm::mat4 transform1 = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 3.0f));
	transform1 = glm::rotate(transform1, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model1->setTransform(transform1);
}


Test::McTest2::~McTest2() {

	delete mesh_line;
	delete model1;
	delete model2;
	delete model3;
	delete world;
	GLFWCallbackManager::GetInstance().SetFramebufferCallback(nullptr);
}

void Test::McTest2::update(double deltatime) {
	this->deltatime = deltatime;

	glm::mat4 transform2 = glm::mat4(1.0f);
	transform2 = glm::translate(transform2, glm::vec3(10.0f, 10.0f, 10.0f));
	transform2 = glm::scale(transform2, glm::vec3(offsetPos1.x));
	transform2 = glm::rotate(transform2, glm::radians(offsetPos1.y), offsetPos1);
	model2->setTransform(transform2);

	glm::mat4 transform3 = glm::mat4(1.0f);
	transform3 = glm::translate(transform3, glm::vec3(0.0f, 0.0f, -5.0f));
	transform3 = glm::scale(transform3, glm::vec3(10.0f));
	transform3 = glm::rotate(transform3, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model3->setTransform(transform3);




	world->update(deltatime, camera->getPos());


	InputManager& input_manager = InputManager::GetInstance();

	{
		if (input_manager.GetMouseScroll() != 0) {
			projection_fov -= input_manager.GetMouseScroll()*5;
			projection_fov = glm::clamp(projection_fov, 10.0f, 180.0f);
			framebuffer_size_callback(window, InputManager::GetInstance().GetWindowSize().x, InputManager::GetInstance().GetWindowSize().y);

		}
	}
	{
		// 鼠标左键点击：设置方块为空气（挖除方块）
		if (is_set_air) {
			if (input_manager.IsMouseButtonPressed(MouseButton::RIGHT)) {
				glm::i64vec4 res = RaycastBlock(camera, world);

				glm::ivec3 chunkPos = worldToChunk(glm::dvec3(res.x, res.y, res.z));
				glm::ivec3 blockPos = worldToChunkLocal(glm::dvec3(res.x, res.y, res.z));

				Chunk* chunk = world->getChunkorCreate(chunkPos);


				if (chunk != nullptr) {
					{
						std::lock_guard<std::mutex> lock(chunk->chunkMutex);
						chunk->setBlock(blockPos.x, blockPos.y, blockPos.z, RegisterBlock::getInstance()->getBlockState_NAME("air").ID);
						chunk->buildMeshData();
						chunk->buildMeshVertex();
					}

				}
				else
				{
					std::cout << "\033[31m [error] \033[0m [mc_test2] : setair chunk is nullptr" << glm::to_string(chunkPos) << std::endl;
				}
			}
		}
	}

	{
		// 冷却计时
		if (block_cd <= 0.2f) {
			block_cd += deltatime;
		}
		else {
			// 冷却完成 + 允许放置 + 右键按下
			if (is_set_block && input_manager.IsMouseButtonPressed(MouseButton::RIGHT)) {
				glm::i64vec4 res = RaycastBlock(camera, world);
				if (res.w == -1) {
					return; // 未命中方块，直接退出
				}

				// 根据碰撞面偏移坐标（核心逻辑简化）
				switch (res.w) {
				case 0: res.z++; break;
				case 1: res.z--; break;
				case 2: res.y--; break;
				case 3: res.y++; break;
				case 4: res.x++; break;
				case 5: res.x--; break;
				}

				// 打印调试信息
				std::cout << "res:" << res.x << ";" << res.y << ";" << res.z << ";    " << res.w << std::endl;

				// 计算区块与方块位置
				glm::dvec3 worldPos(res.x, res.y, res.z);
				glm::ivec3 chunkPos = worldToChunk(worldPos);
				glm::ivec3 blockPos = worldToChunkLocal(worldPos);
				std::cout << "chunkPos: " << glm::to_string(chunkPos) << " blockPos: " << glm::to_string(blockPos) << std::endl;

				// 获取/创建区块
				Chunk* chunk = world->getChunkorCreate(chunkPos);

				if (chunk) {// 设置方块并重建网格
					{
						std::lock_guard<std::mutex> lock(chunk->chunkMutex);
						chunk->setBlock(blockPos.x, blockPos.y, blockPos.z, RegisterBlock::getInstance()->getBlockState_NAME("red").ID);
						chunk->buildMeshData();
						chunk->buildMeshVertex();
					}

					world->addRenderChunk(chunk);

					block_cd = 0.0f;

				}
				else {
					std::cout << "\033[31m [error] \033[0m [mc_test2] : setblock chunk is nullptr" << glm::to_string(chunkPos) << std::endl;
				}
			}
		}


	}


}

void Test::McTest2::Render() {
	shader->use();
	shader->uniformsetMat4("view", camera->getView());


	Renderer* renderer = &Renderer::Instance();
	renderer->setClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
	renderer->setClearDepth(0.0f);
	renderer->Clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_DEPTH_TEST);

	model1->Draw(*shader);


	model2->Draw(*shader);

	model3->Draw(*shader);

	shader->use();
	shader->uniformsetMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));




	normalshader->use();
	normalshader->uniformsetMat4("view", camera->getView());

	normalshader->use();
	normalshader->uniformsetMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));

	model1->Draw(*normalshader);

	lineShader->use();
	lineShader->uniformsetMat4("view", camera->getView());
	lineShader->uniformsetMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));
	glLineWidth(2.0f);

	mesh_line->Draw(*lineShader);


	chunkShader->use();
	chunkShader->uniformsetMat4("view", glm::mat4(glm::mat3(camera->getView())));

	//chunk1->Draw(*chunkShader, camera->getPos());

	world->Draw(*chunkShader, camera->getPos());

	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 modelMat = glm::mat4(1.0f);

	modelMat = glm::translate(modelMat, glm::vec3(setting1[0], setting1[1], setting1[4]));
	modelMat = glm::rotate(modelMat, glm::radians(setting1[3]), glm::vec3(0, 1, 0));
	modelMat = glm::scale(modelMat, glm::vec3(setting1[2]));

	model1->setTransform(modelMat);
	model1->Draw(*shader_2D);
	setting1[3] += 0.1f;
}

void Test::McTest2::GuiRender()
{
	// ====================== 屏幕中间十字准星 ===================================
	// 让 ImGui 画一个全屏透明窗口，只用来显示准星
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoFocusOnAppearing;

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::Begin("Crosshair", nullptr, window_flags);

	// 获取屏幕中心
	ImVec2 center = ImVec2(InputManager::GetInstance().GetWindowSize().x * 0.5f, InputManager::GetInstance().GetWindowSize().y * 0.5f);

	// 准星颜色（白色）、粗细、长度
	ImU32 color = IM_COL32_WHITE;
	float thickness = 2.0f;
	float length = 10.0f;

	// 画横线 + 竖线
	ImDrawList* draw = ImGui::GetWindowDrawList();
	draw->AddLine(ImVec2(center.x - length, center.y),
		ImVec2(center.x + length, center.y),
		color, thickness);
	draw->AddLine(ImVec2(center.x, center.y - length),
		ImVec2(center.x, center.y + length),
		color, thickness);

	ImGui::End();
	//=========================================================================

	ImGui::Begin("setting");
	ImGui::DragFloat("speed", &camera_controller->getSpeed(), 0.1f);
	if (ImGui::Button("set this block"))
	{
		glm::ivec3 chunkPos = worldToChunk(camera->getPos());
		glm::ivec3 blockPos = worldToChunkLocal(camera->getPos());
		std::cout << "res:" << (long long)chunkPos.x * Chunk::ChunkSize + blockPos.x << ";" << (long long)chunkPos.y * Chunk::ChunkSize + blockPos.y << ";" << (long long)chunkPos.z * Chunk::ChunkSize + blockPos.z << std::endl;
		std::cout << "chunkPos: " << glm::to_string(chunkPos) << " blockPos: " << glm::to_string(blockPos) << std::endl;

		Chunk* chunk = world->getChunkorCreate(chunkPos);
		if (chunk != nullptr) {
			chunk->setBlock(blockPos.x, blockPos.y, blockPos.z, RegisterBlock::getInstance()->getBlockState_NAME("stone").ID);
			chunk->buildMeshData();
			chunk->buildMeshVertex();
		}
		else
		{
			std::cout << "\033[31m error: chunk is nullptr at \033[0m" << glm::to_string(chunkPos) << std::endl;
		}
	}
	if (ImGui::Button("set this air"))
	{
		glm::ivec3 chunkPos = worldToChunk(camera->getPos());
		glm::ivec3 blockPos = worldToChunkLocal(camera->getPos());


		Chunk* chunk = world->getChunkorCreate(chunkPos);
		if (chunk != nullptr) {
			chunk->setBlock(blockPos.x, blockPos.y, blockPos.z, RegisterBlock::getInstance()->getBlockState_NAME("air").ID);
			chunk->buildMeshData();
			chunk->buildMeshVertex();
		}
		else
		{
			std::cout << "\033[31m error: chunk is nullptr at \033[0m" << glm::to_string(chunkPos) << std::endl;
		}
	}

	ImGui::Checkbox("set block_dir", &is_set_block);
	// 2. 绘制勾选框
	ImGui::Checkbox("set air_dir", &is_set_air);

	ImGui::DragFloat3("row 0", &setting1[0], 0.01f);
	ImGui::DragFloat3("row 1", &setting1[3], 0.05f);
	ImGui::DragFloat3("row 2", &setting1[6], 0.05f);

	ImGui::End();

	ImGui::Begin("Debug Window");  // 示例窗口，验证流程
	isback = ImGui::Button("<-");
	ImGui::Text("FPS: %.1f", 1.0f / deltatime);


	// 2. RGBA 颜色编辑框（带透明度）
	ImGui::ColorEdit3("选择颜色", (float*)&clearColor);  // 显示透明度条

	ImGui::DragFloat3("1", (float*)&offsetPos1, 0.1f);
	glm::vec3 fronttemp = camera->getFront();
	if (ImGui::DragFloat3("camera front", (float*)&fronttemp, 0.1f)) {
		camera->setFront(fronttemp);
	};

	ImGui::DragScalarN(
		"Camera Pos",          // 标签
		ImGuiDataType_Double,  // 数据类型：双精度
		&camera->getPosRef(),                   // 数据指针（double*）
		3,                     // 分量数：3
		0.1,                   // 拖拽速度（double，不要 f 后缀）
		nullptr, nullptr,      // 最小值、最大值（nullptr=无限制）
		"%.6f",                // 显示格式（保留6位小数）
		1.0                    // 功率曲线（默认1.0线性）
	);



	ImGui::End();


}

void Test::McTest2::framebuffer_size_callback(GLFWwindow* window, int width, int height) {

	camera->setProjection(projection_fov, (float)width / height,10000.0f ,0.02f );
	shader->use();
	shader->uniformsetMat4("projection", camera->getProjection());
	lineShader->use();
	lineShader->uniformsetMat4("projection", camera->getProjection());
	normalshader->use();
	normalshader->uniformsetMat4("projection", camera->getProjection());
	chunkShader->use();
	chunkShader->uniformsetMat4("projection", camera->getProjection());

}


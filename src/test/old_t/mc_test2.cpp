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
#include "block_best.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// ==================== 构造函数 ====================
Test::OLDTest::OLDTest(GLFWwindow* window)
    : clearColor(0.2f, 0.4f, 0.4f)
    , deltatime(-1.0f)
    , window(window){
    // 初始化渲染器设置
    Renderer& renderer = Renderer::Instance();
    renderer.SetCullFace(true);

    // 深度反转设置
    glDepthFunc(GL_GREATER);

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
}

// ==================== 析构函数 ====================
Test::OLDTest::~OLDTest() {
    delete mesh_line;
    GLFWCallbackManager::GetInstance().SetFramebufferCallback(nullptr);
}

// ==================== 初始化辅助函数 ====================
void Test::OLDTest::createAxisLines() {
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

void Test::OLDTest::createShaders() {
    lineShader = new Shader(
        "src/shader/Line/V_lineShader.glsl",
        "src/shader/Line/F_lineShader.glsl", "");

    chunkShader = new Shader(
        "src/shader/chunk/VertexShader.glsl",
        "src/shader/chunk/FragmentShader.glsl", "");

    // 设置纹理采样器
    chunkShader->use();
    chunkShader->uniformsetInt("Texture1", 0);
}

void Test::OLDTest::createTestMeshes() {
    std::vector<Texture*> textures = {
        TextureManager::getInstance()->loadTexture("asset/image/t_t_1.png", TextureType::translucent)
    };
    std::vector<Vertex> vertex = blockVertex;
    std::vector<unsigned int> indices = blockIndices;
    chunkMesh = new C_Mesh(textures);
    chunkMesh->initGPU();
	chunkMesh->addface(glm::ivec4(10, 0, 0, 0), { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f) });
	chunkMesh->buildVertexBuffer();
	chunkMesh->buildIndexBuffer();
    
}

void Test::OLDTest::createUniformBuffers() {
    UBO_Proj = new UniformBuffer(sizeof(glm::mat4), GL_STATIC_DRAW);
    UBO_View0 = new UniformBuffer(sizeof(glm::mat4), GL_DYNAMIC_DRAW);
    UBO_View1 = new UniformBuffer(sizeof(glm::mat4), GL_DYNAMIC_DRAW);
}

// ==================== 更新循环 ====================
void Test::OLDTest::update(double deltatime) {
    this->deltatime = deltatime;

    // 更新投影矩阵
    if (camera->isUpdatePJ) {
        updateShaderProjection(camera->getProjection());
        camera->isUpdatePJ = false;
    }
}

// ==================== 渲染 ====================
void Test::OLDTest::Render() {
    // 更新视图矩阵 Uniform Buffers
    updateViewUniforms();

    // 渲染不透明阶段
    renderOpaquePass();


}

void Test::OLDTest::updateViewUniforms() {
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
}

void Test::OLDTest::renderOpaquePass() {
    Renderer* renderer = &Renderer::Instance();

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

	chunkShader->use();
    chunkShader->uniformsetVec3("chunkRelativePos", -camera->getPos());
	chunkMesh->Draw(*chunkShader);
}

// ==================== GUI ====================
void Test::OLDTest::GuiRender() {
    renderSettingsWindow();
    renderDebugWindow();
}



void Test::OLDTest::renderSettingsWindow() {
    ImGui::Begin("setting");

    ImGui::DragFloat("speed", &camera_controller->getSpeed(), 0.1f);

    ImGui::End();
}

void Test::OLDTest::renderDebugWindow() {
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


void Test::OLDTest::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    camera->setProjection(
        camera->getFov(),
        static_cast<float>(width) / height,
        camera->getNearFar().x,
        camera->getNearFar().y);

    updateShaderProjection(camera->getProjection());
}


void Test::OLDTest::updateShaderProjection(const glm::mat4& projection) {
    UBO_Proj->Bind();
    UBO_Proj->SetData(0, sizeof(glm::mat4), glm::value_ptr(projection));
    UBO_Proj->BindToBindingPoint(0);

    UBO_Proj->ConnectBlockToBindingPoint(chunkShader->getID(), "Proj", 0);
    UBO_Proj->ConnectBlockToBindingPoint(lineShader->getID(), "Proj", 0);
}
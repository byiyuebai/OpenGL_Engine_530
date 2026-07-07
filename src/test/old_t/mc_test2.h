#pragma once

#include "GL/Camera.h"
#include "Chunk.h"
#include "ElementBuffer.h"
#include "l_mesh.h"
#include "m_model.h"
#include "Shader.h"
#include "testmenu.h"
#include "Texture_base.h"
#include "imageTexture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "UniformBuffer.h"
#include "World.h"
#include "mesh_2d.h"
#include "FrameBuffer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

class CameraController;

namespace Test {
    class OLDTest : public Test {
    public:
        OLDTest(GLFWwindow* window);
        ~OLDTest();

        void createAxisLines();

        void createShaders();

        void createTestMeshes();

        void createUniformBuffers();

        void update(double deltatime);
        void Render();
        void updateViewUniforms();
        void GuiRender();


        void renderSettingsWindow();

        void renderDebugWindow();


    private:
        // ==================== 窗口相关 ====================
        GLFWwindow* window;
        double deltatime;

        // ==================== 摄像机 ====================
        Camera* camera;
        std::shared_ptr<CameraController> camera_controller;

        // ==================== Uniform Buffers ====================
        UniformBuffer* UBO_Proj;   // 投影矩阵，绑定点 0
        UniformBuffer* UBO_View0;  // 视图矩阵，绑定点 1
        UniformBuffer* UBO_View1;  // 视图矩阵(仅旋转)，绑定点 2

        // ==================== 着色器 ====================
        Shader* lineShader;
        Shader* chunkShader;

        // ==================== 网格 ====================
        L_Mesh* mesh_line;      // 坐标轴线条
		C_Mesh* chunkMesh;    // 场景块网格
        // ==================== 状态标志 ====================
        glm::vec3 clearColor;

        float block_cd = 0.0f;

        // ==================== 回调函数 ====================
        void framebuffer_size_callback(GLFWwindow* window, int width, int height);

        // ==================== 辅助函数 ====================
        void updateShaderProjection(const glm::mat4& projection); // 更新着色器投影矩阵

        void renderOpaquePass();            // 渲染不透明阶段


    };
}
#pragma once

#include "GL/Camera.h"
#include "Chunk.h"
#include "ElementBuffer.h"
#include "l_mesh.h"
#include "m_model.h"
#include "Shader.h"
#include "testmenu.h"
#include "Texture.h"
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
    class FBOTest : public Test {
    public:
        FBOTest(GLFWwindow* window);
        ~FBOTest();

        void createAxisLines();

        void createFramebuffers(int width, int height);

        void createShaders();

        void createTestMeshes();

        void createUniformBuffers();

        void update(double deltatime);
        void Render();
        void updateViewUniforms();
        void GuiRender();

        void renderCrosshair();

        void renderSettingsWindow();

        void renderDebugWindow();

        void setBlockAtCamera(const std::string& blockName);

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
        Shader* transparentShader_a;   // 透明物体累积
        Shader* transparentShader_r;   // 透明物体揭示度
        Shader* compositeShader;       // 最终合成

        // ==================== 帧缓冲 ====================
        FrameBuffer* opaqueFBO;        // 不透明物体 FBO
        FrameBuffer* transparentFBO;   // 透明物体 FBO

        // ==================== 纹理 ====================
        Texture* opaqueColor;   // 不透明颜色纹理
        Texture* accumTex;      // 累积纹理
        Texture* revealTex;     // 揭示度纹理

        // ==================== 网格 ====================
        Mesh_2D* quadMesh;      // 全屏四边形（用于合成）
        L_Mesh* mesh_line;      // 坐标轴线条
        M_Mesh* triangleMesh;   // 透明三角形（测试用）

        // ==================== 世界 ====================
        World* world;

        // ==================== 状态标志 ====================
        glm::vec3 clearColor;
        bool is_set_air = false;
        bool is_set_block = false;
        float block_cd = 0.0f;

        // ==================== 回调函数 ====================
        void framebuffer_size_callback(GLFWwindow* window, int width, int height);

        // ==================== 辅助函数 ====================
        void setupQuad();   
		void updateShaderProjection(const glm::mat4& projection); // 更新着色器投影矩阵

        void processBlockPlacement();       // 处理方块放置
        void processBlockRemoval();         // 处理方块移除
        void renderOpaquePass();            // 渲染不透明阶段
        void renderTranslucentTestObjects();// 绘制传统透明物体测试
        void renderTransparentPass();       // 渲染透明阶段
        void renderAccumulationPass();      // 渲染累积通道
        void renderRevealagePass();         // 渲染揭示度通道
        void renderCompositePass();         // 最终合成阶段
    };
}
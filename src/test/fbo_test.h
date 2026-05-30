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
#include "World.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include "mesh_2d.h"
#include "FrameBuffer.h"

class CameraController;
namespace Test {
	class FBOTest :public Test {
	public:
		FBOTest(GLFWwindow* window);
		~FBOTest();

		void update(double deltatime);

		void Render();
		void GuiRender();
	private:
		GLFWwindow* window;
		double deltatime;

		Camera* camera;

		Mesh_2D* quadMesh;


		FrameBuffer* baseFBO;

		FrameBuffer* transparentFBO;

		Texture* opaqueColor;
		Texture* accumTex;
		Texture * revealTex;

		Shader* lineShader;
		Shader* chunkShader;

		Shader* transparentShader_a;
		Shader* transparentShader_r;

		Shader* compositeShader;

		L_Mesh* mesh_line;

		World* world;

		M_Mesh* triangleMesh;

		void framebuffer_size_callback(GLFWwindow* window, int width, int height);

		std::shared_ptr<CameraController> camera_controller;

		glm::vec3 clearColor;

		bool is_set_air = false;
		bool is_set_block = false;

		float block_cd = 0.0f;

		void setupQuad();
		void updateShaderProjection(const glm::mat4& projection);
	};

}

glm::vec3 getCameraLookAtCenter(Camera* camera);
/*
深度默认最远为1，最近为0
深度反转后，最远为0，最近为1，这样可以提高深度精度，尤其是在远距离场景中。
*/
#pragma once
#include "Camera.h"
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

class CameraController;
namespace Test {
	class McTest2 :public Test {
	public:
		McTest2(GLFWwindow* window);
		~McTest2();

		void update(double deltatime);

		void Render();
		void GuiRender();
	private:
		GLFWwindow* window;
		double deltatime;

		Camera* camera;
		Shader* shader;
		Shader* normalshader;
		Shader* lineShader;
		Shader* chunkShader;
		Shader* shader_2D;

		L_Mesh* mesh_line;

		Model* model1;
		Model* model2;
		Model* model3;
		
		World* world;

		void framebuffer_size_callback(GLFWwindow* window, int width, int height);

		std::shared_ptr<CameraController> camera_controller;

		glm::vec3 clearColor;

		glm::vec3 offsetPos1;
		//std::array<std::array<float, 3>, 3> setting1;
		std::vector<float> setting1 = std::vector<float>(9, 0.0f);

		bool is_set_air = false;
		bool is_set_block = false;
		float block_cd = 0.0f;

		float projection_fov = 60.0f;
	};

}

glm::vec3 getCameraLookAtCenter(Camera* camera);
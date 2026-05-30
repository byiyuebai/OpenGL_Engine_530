#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "GLFWcallback.h"
#include "InputManager.h"
#include "Camera.h"
#include "Shader.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "GLFWCallbackManager.h"
#include <iostream>
GLFWcallback::GLFWcallback(GLFWwindow* window) {
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

GLFWcallback::~GLFWcallback() {
}

void GLFWcallback::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	KeyCode myKey = GLFWkey_to_mytype(key);
	InputAction myAction = GLFWaction_to_mytype(action);

	if (myKey != KeyCode::UNKNOWN) {
		InputManager::GetInstance().OnKeyEvent(myKey, myAction);
	}
}

void GLFWcallback::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    // 获取 ImGui IO 状态
    ImGuiIO& io = ImGui::GetIO();
    // 若 ImGui 需要捕获键盘（如输入框激活），则不继续传递
    if (io.WantCaptureMouse) {
        return;
    }

	MouseButton myButton = GLFWmouse_button_to_mytype(button);
	InputAction myAction = GLFWaction_to_mytype(action);
	if (myButton != MouseButton::UNKNOWN) {
		InputManager::GetInstance().OnMouseButtonEvent(myButton, myAction);
	}
}

void GLFWcallback::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	if (height == 0) height = 1;
	if (width == 0) width = 1;

	glViewport(0, 0, width, height);
	
	InputManager::GetInstance().OnWindowSizeEvent(width, height);
	GLFWCallbackManager::GetInstance().OnFramebufferSize(window, width, height);
}

void GLFWcallback::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {

	ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
	// 获取 ImGui IO 状态
	ImGuiIO& io = ImGui::GetIO();
	// 若 ImGui 需要捕获键盘（如输入框激活），则不继续传递
	if (io.WantCaptureMouse) {
		return;
	}
	InputManager::GetInstance().OnMouseMoveEvent(xpos, ypos);
}

void GLFWcallback::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	// 获取 ImGui IO 状态
	ImGuiIO& io = ImGui::GetIO();
	// 若 ImGui 需要捕获键盘（如输入框激活），则不继续传递
	if (io.WantCaptureMouse) {
		return;
	}
	InputManager::GetInstance().OnMouseScrollEvent(yoffset);
}




KeyCode GLFWcallback::GLFWkey_to_mytype(int key)
{
	switch (key) {
	case GLFW_KEY_ESCAPE: return KeyCode::ESC;
	case GLFW_KEY_W: return KeyCode::W;
	case GLFW_KEY_A: return KeyCode::A;
	case GLFW_KEY_S: return KeyCode::S;
	case GLFW_KEY_D: return KeyCode::D;
	case GLFW_KEY_SPACE: return KeyCode::SPACE;
	case GLFW_KEY_LEFT_SHIFT: return KeyCode::LEFT_SHIFT;
	case GLFW_KEY_F: return KeyCode::F;
	case GLFW_KEY_E: return KeyCode::E;

	default: return KeyCode::UNKNOWN; // 未知按键
	}
}

MouseButton GLFWcallback::GLFWmouse_button_to_mytype(int button)
{
	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT: return MouseButton::LEFT;
	case GLFW_MOUSE_BUTTON_RIGHT: return MouseButton::RIGHT;
	case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::MIDDLE;
	case GLFW_MOUSE_BUTTON_4: return MouseButton::MOUSE4;
	case GLFW_MOUSE_BUTTON_5: return MouseButton::MOUSE5;

	default: return MouseButton::UNKNOWN; // 未知按键
	}
}
InputAction GLFWcallback::GLFWaction_to_mytype(int action)
{
	switch (action) {
	case GLFW_PRESS: return InputAction::PRESS;
	case GLFW_RELEASE: return InputAction::RELEASE;
	case GLFW_REPEAT: return InputAction::REPEAT;

	default: return InputAction::UNKNOWN; // 未知按键
	}
}

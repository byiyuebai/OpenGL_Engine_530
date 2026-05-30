#ifndef GL_INIT_H
#define GL_INIT_H
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

bool initGL(GLFWwindow*& window, int WDwidth, int WDheight, int WDx, int WDy) {
	// 初始化GLFW
	if (!glfwInit()) {
		std::wcerr << L"\033[31m[ERROR]\033[0m [initGL] 初始化GLFW失败" << std::endl;
		return false;
	}
	// 设置 OpenGL 上下文版本
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 创建窗口
	window = glfwCreateWindow(WDwidth, WDheight, "OpenGL_WD", NULL, NULL);
	glfwSetWindowPos(window, WDx, WDy);
	if (!window) {
		std::wcerr << "\033[31m[ERROR]\033 [initGL] 创建 GLFW 窗口失败" << std::endl;
		// 终止 GLFW 并释放所有 GLFW 分配的资源
		glfwTerminate();
		return false;
	}


	// 设置当前的 OpenGL 上下文
	glfwMakeContextCurrent(window);

	// 初始化 GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "\033[31m[ERROR]\033[0m [initGL] Glad初始化失败！" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return false;
	}

	glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
	glfwSwapInterval(0);
	return true;
}


#endif
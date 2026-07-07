#include "GLmain.h"


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "ControllerManager.h"
#include "GLFWcallback.h"
#include "initGL.h"
#include "InputManager.h"
#include "top_window.h"


#include "mc_test2.h"
#include "fbo_test.h"

#include "FPSLimiter.h"
#include "timer.h"
void GLmain()
{
	//设置帧率
	double deltatime = 0;

	Timer timer;
	FPSLimiter fpsLimiter(6000.0);

	glm::ivec2 window_size = InputManager::GetInstance().GetWindowSize();

	GLFWwindow* window = nullptr;
	if (!initGL(window, window_size.x, window_size.y, 100, 100)) {
		return;
	}
	std::wcout << window_size.x << " " << window_size.y << std::endl;
	const char* versionStr = (const char*)glGetString(GL_VERSION);
	if (versionStr == nullptr) {
		std::wcerr << L"\033[31m[ERROR]\033 获取 OpenGL 版本字符串失败！" << std::endl;
	}
	else {
		std::wcout << L"完整 OpenGL 版本：" << versionStr << std::endl;
	}

	IMGUI_CHECKVERSION(); // 验证版本兼容性
	ImGui::CreateContext(); // 创建 ImGui 上下文


	ImGui::StyleColorsClassic(); // 设置样式（Dark/Light/Classic）

	// 3. 初始化平台（GLFW）和渲染器（OpenGL 3）绑定
	ImGui_ImplGlfw_InitForOpenGL(window, true); // 绑定 GLFW 窗口
	ImGui_ImplOpenGL3_Init("#version 330"); // 绑定 OpenGL 3，传入 GLSL 版本字符串

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/msyh.ttc", 16.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());



	//多线程延迟200ms聚焦窗口
	std::thread topThread(windowTopThread, window); // 传入 window 指针
	topThread.detach();

	//回调函数
	{ GLFWcallback callback(window); }

	//控制器管理器
	ControllerManager& controller_manager = ControllerManager::GetInstance();


	Test::Test* currentTest = nullptr;

	Test::TestMenu* testMenu = new Test::TestMenu(currentTest);
	currentTest = testMenu;

	//注册测试
	testMenu->RegisterTest<Test::FBOTest>("FBO Test", window);

	while (!glfwWindowShouldClose(window)) {

		fpsLimiter.Wait();
		timer.Tick();
		deltatime = timer.DeltaTime();


		glfwPollEvents(); // 轮询窗口、键鼠等事件，响应关闭、按键、鼠标操作。
		controller_manager.UpdateAllController(deltatime); //更新控制器管理器
		InputManager::GetInstance().update(); // 更新输入状态（重置一次性输入，如鼠标位移、滚轮）

		ImGui_ImplOpenGL3_NewFrame(); // 渲染器新帧（绑定 OpenGL 资源）
		ImGui_ImplGlfw_NewFrame();    // 平台新帧（更新输入：鼠标/键盘/窗口大小）
		ImGui::NewFrame();            // 激活 ImGui 帧状态（g.WithinFrameScope = true）

		if (currentTest)
		{
			currentTest->update(deltatime);
			currentTest->Render();

			currentTest->GuiRender();

			if (currentTest != testMenu && currentTest->getIsBack())
			{
				delete currentTest;
				currentTest = testMenu;
			}


		}

		// 5. 生成 ImGui 绘制数据
		ImGui::Render();
		// 6. 渲染 ImGui 到屏幕
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// 交换前后双缓冲
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	// 释放资源

}

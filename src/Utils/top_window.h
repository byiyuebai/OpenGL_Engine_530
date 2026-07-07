#pragma once
#include <windows.h>
#include <GLFW/glfw3.h>


void windowTopThread(GLFWwindow* window) {
    if (!window) {
        return;
    }

    // 延迟（子线程阻塞，不影响主程序）
    Sleep(300);


    glfwFocusWindow(window);
}
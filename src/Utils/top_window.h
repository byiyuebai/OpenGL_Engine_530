#pragma once
#include <windows.h>
#include <mutex>       // 互斥锁（保证线程安全）
#include <GLFW/glfw3.h>

// 全局互斥锁：保护 std::cout，确保同一时间只有一个线程输出（核心线程安全）
static std::mutex g_coutMutex;

// 线程安全的中文输出函数（统一处理编码 + 加锁）
void safeCout(const wchar_t* msg) {
    // 1. 加锁：自动上锁/解锁（离开作用域时 lock_guard 析构解锁）
    std::lock_guard<std::mutex> lock(g_coutMutex);

    // 3. 输出（此时是线程安全且无乱码的）
    std::wcout << msg << std::endl;
}

void windowTopThread(GLFWwindow* window) {
    if (!window) {
        safeCout(L"[windowTopThread]线程错误：窗口指针为空！");
        return;
    }

    // 延迟（子线程阻塞，不影响主程序）
    Sleep(200);


    glfwFocusWindow(window);
    // 线程安全输出中文（无乱码 + 无竞争）
    //safeCout(L"[windowTopThread]置顶完成");
}
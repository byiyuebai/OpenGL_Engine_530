#pragma once
#include <GLFW/glfw3.h>
#include <functional>
#include <mutex>

// 回调管理器
class GLFWCallbackManager {
public:
    static GLFWCallbackManager& GetInstance();

    // 注册/替换回调
    void SetFramebufferCallback(std::function<void(GLFWwindow*, int, int)> cb);

    // 触发回调
    void OnFramebufferSize(GLFWwindow* window, int width, int height);

private:
    // 回调函数
    std::function<void(GLFWwindow*, int, int)> framebufferCallback;
    // 线程安全锁
    std::mutex callbackMutex;

    // 单例禁止拷贝
    GLFWCallbackManager();
    GLFWCallbackManager(const GLFWCallbackManager&) = delete;
    GLFWCallbackManager& operator=(const GLFWCallbackManager&) = delete;
};
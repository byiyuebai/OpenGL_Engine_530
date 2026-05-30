#include "GLFWCallbackManager.h"
#include <cassert>

// 单例实现
GLFWCallbackManager& GLFWCallbackManager::GetInstance() {
    static GLFWCallbackManager instance;
    return instance;
}

GLFWCallbackManager::GLFWCallbackManager(): framebufferCallback([](GLFWwindow*, int, int) {}){}

void GLFWCallbackManager::SetFramebufferCallback(std::function<void(GLFWwindow*, int, int)> cb) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    framebufferCallback = std::move(cb);
}

void GLFWCallbackManager::OnFramebufferSize(GLFWwindow* window, int width, int height) {
    std::lock_guard<std::mutex> lock(callbackMutex);
    if (framebufferCallback) {
        framebufferCallback(window, width, height);
    }
}

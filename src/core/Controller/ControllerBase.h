#pragma once
#include <GLFW/glfw3.h>
#include <string>

// 控制器基类
class ControllerBase {
public:
    ControllerBase(const std::string& name, int priority = 0)
        : cont_name(name), cont_priority(priority), cont_isuse(true) {}

    virtual ~ControllerBase() = default; // 虚析构，保证子类正确析构

    // 纯虚函数：输入处理（必须由子类实现）
    virtual void processInput(float deltaTime) = 0;

    // 虚函数：逻辑更新（可选重写，如需要帧同步的逻辑）
    virtual void update(float deltaTime) {}

    //  getter/setter：通用属性
    const std::string& getName() const { return cont_name; }
    int getPriority() const { return cont_priority; }

    bool getIsuse() const { return cont_isuse; }

    void setIsuse(bool enabled) { cont_isuse = enabled; }
    void setPriority(int priority) { cont_priority = priority; }

private:
    std::string cont_name;    // 控制器名称（用于调试/查找）
    int cont_priority;        // 优先级（数值越大，越先执行）
    bool cont_isuse;        // 是否启用（禁用后不执行processInput和update）
};
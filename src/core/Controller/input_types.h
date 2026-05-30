#pragma once
#include <cstdint>
#include <glm/glm.hpp> // 依赖 glm 处理向量/矩阵（摄像机核心依赖）
#include <GLFW/glfw3.h> // 依赖 GLFW 处理输入事件
// 输入设备枚举
//enum class InputDevice :uint8_t {
//    UNKNOWN,
//    KEYBOARD,
//    MOUSE
//};

// 按键码（与 GLFW 兼容，避免直接暴露 GLFW 宏）
enum class KeyCode :uint16_t {
    UNKNOWN,
    // 常用按键映射
    W,
    A,
    S,
    D,
    SPACE,
    ESC,
    LEFT_SHIFT,

    F,
    E,
};
enum class MouseButton :uint8_t {
    UNKNOWN,
    LEFT,
    RIGHT,
    MIDDLE,
    // 侧键
    MOUSE4,
    MOUSE5,
    
};

enum class InputAction {
    UNKNOWN,
    RELEASE,  // 按键松开（动作结束）
    PRESS,    // 按键按下（动作开始）
    REPEAT   // 按键长按重复（按住不放时持续触发）
};

// 鼠标移动数据
struct MouseMoveData {
    glm::vec2 pos = { 0.0f, 0.0f }; // 鼠标位置;
    glm::vec2 offset = { 0.0f, 0.0f }; // 鼠标移动偏移量;

    glm::vec2 last_pos = { 0.0f, 0.0f };
};


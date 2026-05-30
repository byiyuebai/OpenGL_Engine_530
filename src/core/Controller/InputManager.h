#pragma once
#include "input_types.h"
#include <mutex>
#include <vector>
class InputManager {
public:
	// 单例模式（全局唯一输入管理器）
	static InputManager& GetInstance() {
		static InputManager instance;
		return instance;
	}

	// 禁止拷贝
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;

	// -------------------------- 事件接收接口（供 GLFW 回调调用）--------------------------
	// 按键事件处理
	void OnKeyEvent(KeyCode key, InputAction action);
	// 鼠标按键事件处理
	void OnMouseButtonEvent(MouseButton button, InputAction action);
	// 鼠标移动事件处理
	void OnMouseMoveEvent(double xpos, double ypos);
	// 鼠标滚轮事件处理（可选，用于缩放）
	void OnMouseScrollEvent(double offset);
	// 窗口大小变化事件（可选，用于更新投影矩阵）
	void OnWindowSizeEvent(int width, int height);

	// -------------------------- 输入查询接口（供控制器调用）--------------------------
	// 检查按键是否按下
	bool IsKeyPressed(KeyCode key) const;
	bool IsKeyPressedAsFrame(KeyCode key) const;
	bool IsMouseButtonPressed(MouseButton button) const;
	MouseMoveData GetMouseMoveData() const;

	// 获取鼠标滚轮位移
	float GetMouseScroll() const;
	// 获取窗口大小
	glm::ivec2 GetWindowSize() const;

	// 每帧更新（重置一次性输入状态，如鼠标位移、滚轮）
	void update();

private:
	InputManager() = default;

	// 按键状态映射
	std::vector<bool> key_states_;
	std::vector<bool> key_pressed_this_frame_;

	// 鼠标按键状态映射
	std::vector<bool> mouse_button_states_;

	MouseMoveData mouse_move_data_;

	// 滚轮状态
	float mouse_scroll_ = 0.0;

	// 窗口状态
	glm::ivec2 window_size_ = { 800, 600 };
	// 线程安全锁（GLFW 回调可能在主线程外触发）
	mutable std::mutex mutex_;
};
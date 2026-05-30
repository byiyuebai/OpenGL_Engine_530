#pragma once
#include <memory>
#include <vector>

#include "ControllerBase.h"
class ControllerManager {
public:
	// 单例模式
	static ControllerManager& GetInstance() {
		static ControllerManager instance;
		return instance;
	}
	// 3. 禁止拷贝构造和赋值运算符（防止创建多个实例）
	ControllerManager(const ControllerManager&) = delete;            // 禁用拷贝构造
	ControllerManager& operator=(const ControllerManager&) = delete; // 禁用赋值运算符

	// 添加控制器（按优先级排序）
	void AddController(std::shared_ptr<ControllerBase> controller);

	// 每帧更新所有控制器
	void UpdateAllController(float deltaTime);

private:
	ControllerManager();

	~ControllerManager() = default;
	std::vector<std::shared_ptr<ControllerBase>> controllers_; // 存储所有控制器
};
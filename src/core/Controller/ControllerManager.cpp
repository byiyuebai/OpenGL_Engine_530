#include "ControllerManager.h"
#include <algorithm>
ControllerManager::ControllerManager() {
}

void ControllerManager::AddController(std::shared_ptr<ControllerBase> controller) {
	controllers_.push_back(controller);
	// 按优先级降序排序（优先级高的先执行）
	std::sort(controllers_.begin(), controllers_.end(),
		[](const std::shared_ptr<ControllerBase>& a, const std::shared_ptr<ControllerBase>& b) {
			return a->getPriority() > b->getPriority();
		});
}

void ControllerManager::UpdateAllController(float deltaTime) {


	for (auto& controller : controllers_) {
		if (controller->getIsuse()) {
			controller->processInput(deltaTime);
			controller->update(deltaTime);
		}
	}
}

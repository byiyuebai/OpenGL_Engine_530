#pragma once

#include "Camera.h"
#include "ControllerBase.h"

#include "input_types.h"

struct MouseButtonStates {
	bool left = false;
	bool right = false;
	bool middle = false;
};
struct KeyStates {
	bool forward = false;
	bool backward = false;

	bool left = false;
	bool right = false;

	bool up = false;
	bool down = false;
};
class CameraController : public ControllerBase {
public:
	// 构造函数：初始化基类（名称+优先级），绑定摄像机和窗口
	CameraController(Camera* camera, int priority = 10, const std::string& name = "CameraController");
	~CameraController() = default;

	void processInput(float deltaTime) override;
	void update(float dt) override;

	float& getSpeed() { return speed; }
private:

	float target_yaw = 0.0f;          // 目标偏航角
	float target_pitch = 0.0f;        // 目标俯仰角



	KeyStates key_states; //按键状态

	MouseButtonStates mouse_button_states; //鼠标按钮状态

	MouseMoveData mouse_move_data; //鼠标移动数据

	float mouse_scroll_data = 0.0f; //鼠标滚轮数据

	Camera* camera;

	float speed = 10.0f;

	float rotation_speed = 1.0f;

	float scroll_speed = 6.0f;

	

	void keyInput();
	void mouseInput();
	void mouseScrollInput();

	void cameraMove(float dt);
	void cameraRotate();
	void cameraScroll();
};
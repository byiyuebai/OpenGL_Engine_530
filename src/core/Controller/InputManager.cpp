// InputManager.cpp
#include "InputManager.h"
void InputManager::OnKeyEvent(KeyCode key, InputAction action) {
    const size_t idx = static_cast<size_t>(key);
    if (idx >= key_states_.size()) {
        key_states_.resize(idx + 1, false);
        key_pressed_this_frame_.resize(idx + 1, false);
    }
    switch (action) {
    case InputAction::PRESS:
        key_states_[idx] = true;
        key_pressed_this_frame_[idx] = true; // 标记本帧有按下动作
        break;
    case InputAction::REPEAT:
        key_states_[idx] = true; // 持续按住，不重复标记“按下过”
        break;
    case InputAction::RELEASE:
        key_states_[idx] = false;
        break;
    }
}

void InputManager::OnMouseButtonEvent(MouseButton button, InputAction action){

    const size_t idx = static_cast<size_t>(button);

    if (idx >= mouse_button_states_.size()) {
        mouse_button_states_.resize(idx + 1, false);
    }
    switch (action) {
    case InputAction::PRESS:
    case InputAction::REPEAT:
        mouse_button_states_[idx] = true;
        break;
    case InputAction::RELEASE:
        mouse_button_states_[idx] = false;
        break;
    }
}

void InputManager::OnMouseMoveEvent(double xpos, double ypos) {
    mouse_move_data_.pos = glm::vec2(xpos, ypos);
    mouse_move_data_.offset = mouse_move_data_.pos - mouse_move_data_.last_pos;
    mouse_move_data_.last_pos = mouse_move_data_.pos;
}
//鼠标滚轮
void InputManager::OnMouseScrollEvent(double offset) {
	mouse_scroll_ = offset;
}
//窗口大小
void InputManager::OnWindowSizeEvent(int width, int height) {
    window_size_ = glm::ivec2(width, height);
}

bool InputManager::IsKeyPressed(KeyCode key) const {
    const size_t idx = static_cast<size_t>(key);
    return idx < key_states_.size() && key_states_[idx];
}
bool InputManager::IsKeyPressedAsFrame(KeyCode key) const{
    const size_t idx = static_cast<size_t>(key);
    return idx < key_pressed_this_frame_.size() && key_pressed_this_frame_[idx];
}
bool InputManager::IsMouseButtonPressed(MouseButton button) const
{
    const size_t idx = static_cast<size_t>(button);
    return idx < mouse_button_states_.size() && mouse_button_states_[idx];
}
MouseMoveData InputManager::GetMouseMoveData() const {
    return mouse_move_data_;
}
float InputManager::GetMouseScroll() const {
    return mouse_scroll_;
}

glm::ivec2 InputManager::GetWindowSize() const {
    return window_size_;
}
void InputManager::update() {
    mouse_move_data_.offset = glm::vec2(0,0);
	mouse_scroll_ = 0;
    fill(key_pressed_this_frame_.begin(), key_pressed_this_frame_.end(), false);
}
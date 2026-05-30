#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include "InputManager.h"

#include <iostream>
Camera::Camera(glm::mat4 view, float fov, float aspect, float m_near, float m_far)
{
	if (view == glm::mat4(1.0f)) {
		this->view = glm::lookAt(pos, pos + front, up);
	}
	else {
		this->view = view;
	}

	setProjection(fov, aspect, m_near, m_far);
	setFront(front);
}

void Camera::update()
{
	
	right = glm::normalize(glm::cross(front, up));
	view = glm::lookAt(pos, pos + front, up);
}

void Camera::setPos(glm::dvec3 pos) { this->pos = pos; }

void Camera::setFront(glm::dvec3 front) {
	float yaw = glm::degrees(std::atan2(front.y, front.x));
	float pitch = glm::degrees(std::atan2(front.z, glm::sqrt(front.x * front.x + front.y * front.y)));
	setYawANDPitch(yaw, pitch);
}

void Camera::setYawANDPitch(float yaw, float pitch)
{

	this->yaw = fmod(yaw, 360.0f);
	this->pitch = glm::clamp(pitch, -89.9f, 89.9f);

	float yaw_rad = glm::radians(this->yaw);
	float pitch_rad = glm::radians(this->pitch);

	front.x = cos(yaw_rad) * cos(pitch_rad);
	front.y = sin(yaw_rad) * cos(pitch_rad);
	front.z = sin(pitch_rad);

	front = glm::normalize(front);
}
void Camera::setUp(glm::dvec3 up) { this->up = up; }

void Camera::setView(glm::mat4 view) { this->view = view; }

void Camera::setProjection(float fov, float aspect, float m_near, float m_far) {
	this->fov = fov;
	this->farNear = glm::vec2(m_near, m_far);
	this->projection = glm::perspective(glm::radians(fov), aspect, m_near, m_far);
	isUpdatePJ = true;
}

glm::dvec3 Camera::getPos() const { return pos; }
glm::dvec3 Camera::getFront() const { return front; }
glm::dvec3 Camera::getRight() const { return right; }
glm::dvec3 Camera::getUp() const { return up; }
glm::mat4 Camera::getView() { return view; }
glm::mat4 Camera::getProjection() { return projection; }

float Camera::getYaw() const { return yaw; }
float Camera::getPitch() const { return pitch; }

float Camera::getFov() const
{
	return fov;
}

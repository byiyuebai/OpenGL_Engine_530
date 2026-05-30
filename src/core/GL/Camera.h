#pragma once
#include <memory>
#include <glm/glm.hpp>

class Camera {
public:
	Camera(glm::mat4 view = glm::mat4(1.0f), float fov= 60.0f, float aspect= 1.0f, float m_near= 0.1f, float m_far= 100.0f);

	bool isUpdatePJ = false;

	void update();

	void setPos(glm::dvec3 pos);
	void setFront(glm::dvec3 front);
	void setUp(glm::dvec3 up);
	void setView(glm::mat4 view);
	void setYawANDPitch(float yaw, float pitch);

	void setProjection(float fov, float aspect, float m_near, float m_far);

	glm::dvec3 getPos() const;
	glm::dvec3 getFront() const;
	glm::dvec3 getRight() const;
	glm::dvec3 getUp() const;
	glm::mat4 getView();
	glm::mat4 getProjection();
	glm::dvec3& getPosRef() { return pos; }
	glm::dvec3& getFrontRef() { return front; }
	float getYaw() const;
	float getPitch() const;

	//如果不使用setProjection,下面的值不会更新
	float getFov() const;
	glm::vec2 getNearFar() const { return farNear; }

private:
	glm::dvec3 pos = glm::dvec3(2.0, 2.0, 2.0);
	glm::dvec3 front = glm::dvec3(-1.0, -1.0, -1.0);
	glm::dvec3 up = glm::dvec3(0.0, 0.0, 1.0);
	glm::dvec3 right = glm::normalize(glm::cross(front, up));

	float yaw = 0.0f;
	float pitch = 0.0f;

	glm::mat4 view;
	glm::mat4 projection;

	glm::vec2 farNear = glm::vec2(0.01f, 10000.0f);
	float fov = 60.0f;
};
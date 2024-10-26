#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>

enum CAMERA_DIRECTIONS {
	UP,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

static const glm::vec3 DEFAULT_pos = glm::vec3(0.0, 10.0, 3.0);
static const glm::vec3 DEFAULT_front = glm::vec3(0.0, 0.0, -1.0);
static const glm::vec3 DEFAULT_worldUp = glm::vec3(0.0, 1.0, 0.0);
static const float DEFAULT_yaw = -90.0f;
static const float DEFAULT_pitch = 0.0f;
static const float DEFAULT_movementSpeed = 10.0f;
static const float DEFAULT_sensitivity = 0.16f;
static const float DEFAULT_zoom = 1.0f;

class Camera {
private:
	glm::vec3  m_front = DEFAULT_front,
		m_worldUp = DEFAULT_worldUp,
		m_up,
		m_right;
	float m_yaw = DEFAULT_yaw;
	float m_pitch = DEFAULT_pitch;
	float m_movementSpeed = DEFAULT_movementSpeed;
	float m_sensitivity = DEFAULT_sensitivity;
	float m_zoom = DEFAULT_zoom;
public:
	glm::vec3 m_pos = DEFAULT_pos;
	Camera(glm::vec3 position, glm::vec3 up,
		float yaw, float pitch);
	Camera();
	~Camera();

	glm::mat4 getViewMatrix();
	void move(CAMERA_DIRECTIONS direction, float dt);
	void look(float xoffset, float yoffset, bool constrainPitch = true);
	void handleZoom(float xoffset, float yoffset);
	void updateDirectionVectors();
	float zoom();
	glm::vec3 position();
	void setPosition(glm::vec3 newPos);
};
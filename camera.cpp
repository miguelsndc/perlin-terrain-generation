#include "camera.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up,
	float yaw, float pitch)
	: m_pos(position), m_worldUp(up), m_yaw(yaw),
	m_pitch(pitch)
{
	updateDirectionVectors();
}

Camera::Camera() {
	updateDirectionVectors();
}

void Camera::updateDirectionVectors() {
	glm::vec3 direction = m_front;
	float radYaw = glm::radians(m_yaw);
	float radPitch = glm::radians(m_pitch);

	direction.x = cos(radYaw) * cos(radPitch);
	direction.y = sin(radPitch);
	direction.z = sin(radYaw) * cos(radPitch);

	m_front = glm::normalize(direction);
	m_right = glm::normalize(glm::cross(m_front, m_worldUp));
	m_up = glm::normalize(glm::cross(m_right, m_front));
}

Camera::~Camera() {}

void Camera::look(float xoffset, float yoffset, bool constrainPitch) {
	float xOffScaled = xoffset * m_sensitivity;
	float yOffScaled = yoffset * m_sensitivity;

	m_yaw += xOffScaled;
	m_pitch += yOffScaled;

	if (constrainPitch) {
		m_pitch = std::max(-89.f, std::min(m_pitch, 89.0f)); //clamp pitch to avoid lookat flip
	}

	updateDirectionVectors();
}

void Camera::move(CAMERA_DIRECTIONS direction, float dt) {
	if (direction == UP) {
		m_pos += m_worldUp * m_movementSpeed * dt;
	}
	if (direction == FORWARD) {
		m_pos += glm::vec3(m_front.x, 0.0, m_front.z) * m_movementSpeed * dt;
	}
	if (direction == BACKWARD) {
		m_pos -= glm::vec3(m_front.x, 0.0, m_front.z) * m_movementSpeed * dt;
	}
	if (direction == LEFT) {
		m_pos -= glm::vec3(m_right.x, 0.0, m_right.z) * m_movementSpeed * dt;
	}
	if (direction == RIGHT) {
		m_pos += glm::vec3(m_right.x, 0.0, m_right.z) * m_movementSpeed * dt;
	}
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(m_pos, m_pos + m_front, m_up);
}

void Camera::handleZoom(float xoffset, float yoffset) {
	m_zoom -= yoffset * 0.05;
	if (m_zoom < 0.1f) {
		m_zoom = 0.1f;
	}
	else if (m_zoom > 3.0f) {
		m_zoom = 3.0f;
	}
}

float Camera::zoom() {
	return m_zoom;
}

glm::vec3 Camera::position() {
	return m_pos;
}

void Camera::setPosition(glm::vec3 newPos) {
	m_pos = newPos;
}
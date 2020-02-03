#include "engine/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <functional>

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch)
 : _position(position), _worldUp(up), _yaw(yaw), _pitch(pitch), _fov(k_FOV) {
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : _position(glm::vec3(posX, posY, posZ)), _worldUp(glm::vec3(upX, upY, upZ)), _yaw(yaw), _pitch(pitch), _fov(k_FOV) {
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
	return glm::lookAt(_position, _position + _front, _up);
}

glm::mat4 Camera::personalGetViewMatrix() const {
	glm::vec3 zaxis = glm::normalize(_front );
	glm::vec3 xaxis = glm::normalize(glm::cross(zaxis, _up));
	glm::vec3 yaxis = cross(xaxis, zaxis);

	glm::mat4 viewMatrix = {
	  glm::vec4(xaxis.x, xaxis.y, xaxis.z, -glm::dot(xaxis, _position)),
	  glm::vec4(yaxis.x, yaxis.y, yaxis.z, -glm::dot(yaxis, _position)),
	  glm::vec4(zaxis.x, zaxis.y, zaxis.z, -glm::dot(zaxis, _position)),
	  glm::vec4(_up, 1)
	};

	return viewMatrix;
}

float Camera::getFOV() const {
    return _fov;
}

glm::vec3 Camera::getPosition() const {
    return _position;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(front);

    _right = glm::normalize(glm::cross(_front, _worldUp));
    _up = glm::normalize(glm::cross(_right, _front));
}

void Camera::handleKeyboard(Movement direction, float dt) {
    const float velocity = k_Speed * dt;

    switch (direction) {
        case Movement::Forward: _position += _front * velocity; break;
        case Movement::Backward: _position -= _front * velocity; break;
        case Movement::Left: _position -= _right * velocity; break;
        case Movement::Right: _position += _right * velocity; break;
        default:;
    }
}

void Camera::handleKeyboardFPS(Movement direction, float dt) {
	const float velocity = k_Speed * dt;

	switch (direction) {
	case Movement::Forward:
		_position.z += _front.z * velocity; break;
	case Movement::Backward: _position.z -= _front.z * velocity; break;
	case Movement::Left: _position -= _right * velocity; break;
	case Movement::Right: _position += _right * velocity; break;
	default:;
	}
}

void Camera::handleMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    const float xoff = xoffset * k_Sensitivity;
    const float yoff = yoffset * k_Sensitivity;

    _yaw += xoff;
    _pitch += yoff;

    if (constrainPitch) {
        if (_pitch > 89.0f) _pitch = 89.0f;
        if (_pitch < -89.0f) _pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::handleMouseScroll(float yoffset) {
    if (_fov >= 1.0f && _fov <= 45.0f) _fov -= yoffset;
    if (_fov <= 1.0f) _fov = 1.0f;
    if (_fov >= 45.0f) _fov = 45.0f;
}


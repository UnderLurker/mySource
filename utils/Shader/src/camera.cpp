//
// Created by 常笑男 on 2024/3/5.
//
#include "camera.h"

NAME_SPACE_START(myUtil)

void Camera::moveProcess(uint32_t direction) {
    if (direction & FRONT) _cameraPos += _cameraFront * _cameraSpeed;
    if (direction & BACK) _cameraPos -= _cameraFront * _cameraSpeed;
    if (direction & LEFT) _cameraPos -= glm::normalize(glm::cross(_cameraFront, _cameraUp)) * _cameraSpeed;
    if (direction & RIGHT) _cameraPos += glm::normalize(glm::cross(_cameraFront, _cameraUp)) * _cameraSpeed;
}

void Camera::scrollProcess(float offsetX, float offsetY) {
    _fov += offsetY;
    if (_fov < 1.0f) _fov = 1.0f;
    else if (_fov > 55.0f) _fov = 55.0f;
}

void Camera::cursorProcess(float xPos, float yPos) {
    if (_firstCursor) {
        _lastCursorXPos = xPos;
        _lastCursorYPos = yPos;
        _firstCursor    = false;
    }

    float xOffset   = xPos - _lastCursorXPos;
    float yOffset   = _lastCursorYPos - yPos;
    _lastCursorXPos = xPos;
    _lastCursorYPos = yPos;

    xOffset *= _sensitivity;
    yOffset *= _sensitivity;
    _yaw    += xOffset;
    _pitch  += yOffset;
    if (_pitch < -89.0f) _pitch = -89.0f;
    else if (_pitch > 89.0f) _pitch = 89.0f;
    glm::vec3 front;
    front.x      = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y      = sin(glm::radians(_pitch));
    front.z      = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _cameraFront = glm::normalize(front);
}

glm::mat4 Camera::getViewMat4() const {
    glm::mat4 result(1.0f);
    result = glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
    return result;
}
NAME_SPACE_END()

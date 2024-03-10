//
// Created by 常笑男 on 2024/3/5.
//

#ifndef MAIN_CAMERA_H
#define MAIN_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Util.h"

NAME_SPACE_START(myUtil)

enum Direction {
    FRONT = 0x0001,
    BACK  = 0x0010,
    LEFT  = 0x0100,
    RIGHT = 0x1000
};

class Camera {
public:
    Camera()          = default;
    virtual ~Camera() = default;

    [[nodiscard]] const glm::vec3& getCameraPos() const { return _cameraPos; }
    [[nodiscard]] const glm::vec3& getCameraFront() const { return _cameraFront; }
    [[nodiscard]] const glm::vec3& getCameraUp() const { return _cameraUp; }
    [[nodiscard]] float getCameraSpeed() const { return _cameraSpeed; }
    [[nodiscard]] float getPitch() const { return _pitch; }
    [[nodiscard]] float getYaw() const { return _yaw; }
    [[nodiscard]] float getFov() const { return _fov; }
    [[nodiscard]] float getSensitivity() const { return _sensitivity; }

    void setCameraPos(const glm::vec3& position) { _cameraPos = position; }
    void setCameraFront(const glm::vec3& cameraFront) { _cameraFront = cameraFront; }
    void setCameraUp(const glm::vec3& cameraUp) { _cameraUp = cameraUp; }
    void setCameraSpeed(float speed) { _cameraSpeed = speed; }
    void setPitch(float pitch) { _pitch = pitch; }
    void setYaw(float yaw) { _yaw = yaw; }
    void setFov(float fov) { _fov = fov; }
    void setSensitivity(float sensitivity) { _sensitivity = sensitivity; }

    void moveProcess(uint32_t direction);
    void scrollProcess(float offsetX, float offsetY);
    void cursorProcess(float xPos, float yPos);

    [[nodiscard]] glm::mat4 getViewMat4() const;

private:
    glm::vec3 _cameraPos {.0f, .0f, 3.0f};
    glm::vec3 _cameraFront {.0f, 0, -1.0f};
    glm::vec3 _cameraUp {.0f, 1.0f, .0f};
    float _cameraSpeed {0};
    float _pitch {0};
    float _yaw {-90.0f};
    float _fov {45.0f};
    float _lastCursorXPos {0};
    float _lastCursorYPos {0};
    float _sensitivity {0.1f};
    bool _firstCursor {true};
};

NAME_SPACE_END()

#endif // MAIN_CAMERA_H

//
// Created by hao on 4/22/17.
//

#ifndef CG_PROJECT_CAMERA_H
#define CG_PROJECT_CAMERA_H

#include "Header.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 100.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;

using namespace glm;


class Camera {
public:
    vec3 Position;
    vec3 Front;
    vec3 Up;
    vec3 Right;
    vec3 WorldUp;

    GLfloat Yaw;
    GLfloat Pitch;
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat Zoom;

    Camera(vec3 position = glm::vec3(0, 0, 0),
           vec3 up = vec3(0, 1, 0),
           GLfloat yaw= YAW, GLfloat pitch = PITCH)
            : Front(vec3(0, 0, -1)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        this->updateCameraVectors();
    }
    Camera(GLfloat posX, GLfloat posY, GLfloat posZ,
           GLfloat upX, GLfloat upY, GLfloat upZ,
           GLfloat yaw, GLfloat pitch)
            : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
    {
        this->Position = glm::vec3(posX, posY, posZ);
        this->WorldUp = glm::vec3(upX, upY, upZ);
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }

    mat4 GetViewMatrix();
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(GLfloat yoffset);
private:
    void updateCameraVectors();

};

class CameraHandler {
public:
    static Camera *camera;
    static Camera* getCamera() {
        if (camera == nullptr)
            camera = new Camera();
        return camera;
    }
};

#endif //CG_PROJECT_CAMERA_H

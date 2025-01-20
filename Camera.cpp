#include "Camera.hpp"


namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUp, cameraFrontDirection));
        this->cameraUpDirection = glm::cross(cameraFrontDirection, cameraRightDirection);
        this->pitch = 0.0f;
        this->yaw = -90.0f;
    }

    glm::vec3 Camera::getPosition() {
        return cameraPosition;
    }
    glm::vec3 Camera::getCameraFront() {
        return cameraFrontDirection;
    }
    glm::vec3 Camera::getCameraUp() {
        return cameraUpDirection;
    }


    //automated tour
    void Camera::setPosition(glm::vec3 newPosition)
    {
        cameraPosition = newPosition;
    }

    void Camera::focusOnTarget(glm::vec3 target) {
        // Update cameraTarget
        cameraTarget = target;

        // Recalculate the direction vectors
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cameraFrontDirection));
        cameraUpDirection = glm::cross(cameraFrontDirection, cameraRightDirection);
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
        return view;
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed,float deltaTime, Model3D& ground) {

        //float speed = cameraSpeed * deltaTime; // Frame-rate independent speed
        glm::vec3 movement = glm::vec3(0.0f);

        if (direction == gps::MOVE_FORWARD) {
            movement += speed * cameraFrontDirection;
        }
        if (direction == gps::MOVE_BACKWARD) {
            movement -= speed * cameraFrontDirection;
        }
        if (direction == gps::MOVE_LEFT) {
            movement -= speed * cameraRightDirection;
        }
        if (direction == gps::MOVE_RIGHT) {
            movement += speed * cameraRightDirection;
        }
        cameraPosition += movement;
        cameraTarget = cameraPosition + cameraFrontDirection;


    }
    void Camera::rotate(float pitch_off, float yaw_off) {

        this->pitch += pitch_off;
        this->yaw += yaw_off;

        // Asigurăm că pitch-ul nu depășește 89 grade
        if (this->pitch > 89.0f)
            this->pitch = 89.0f;
        if (this->pitch < -89.0f)
            this->pitch = -89.0f;

        // Actualizăm vectorii camerei
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFrontDirection = glm::normalize(front);

        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }


}




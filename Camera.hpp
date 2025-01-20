#ifndef Camera_hpp
#define Camera_hpp

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "Model3D.hpp"

namespace gps {
    
    enum MOVE_DIRECTION {MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT};
    
    class Camera {

    public:
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        glm::vec3 getPosition();
        glm::vec3 getCameraFront();
        glm::vec3 getCameraUp();
        glm::mat4 getViewMatrix();
        void setPosition(glm::vec3 newPosition);
        void focusOnTarget(glm::vec3 target);
        void move(MOVE_DIRECTION direction, float speed, float deltaTime,Model3D& ground);
     
        void rotate(float pitch, float yaw);
        
    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;
        float yaw;   
        float pitch; 
    };    
}

#endif /* Camera_hpp */

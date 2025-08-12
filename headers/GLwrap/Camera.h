#ifndef GLWRAP_CAMERA_H
#define GLWRAP_CAMERA_H

#include "GLwrap/Window.h"
#include "GLwrap/Shader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <cmath>

namespace GLwrap
{
    class Camera {
    public:
        Camera(glm::vec3 position = glm::vec3(0));
        ~Camera();

        void Update(const Window& window);
        void UploadMatrix(const Shader& shader, std::string uniform);

        glm::vec3 getPosition() const;
    private:
        glm::vec3 position, direction;
        const glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
        glm::mat4 matrix;
        float speed = 25.f, sensitivity = 300.f;
        bool firstClick = true;

        float FOV = 45.f, near = 1.f, far = 1000.f;
    };

} // namespace GLwrap


#endif // GLWRAP_CAMERA_H
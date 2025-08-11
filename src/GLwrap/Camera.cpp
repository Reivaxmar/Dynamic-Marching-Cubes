#include "GLwrap/Camera.h"

namespace GLwrap
{
    Camera::Camera(glm::vec3 position)
        : position(position), direction(0.f, 0.f, 1.f)
    {

    }

    Camera::~Camera() {}

    void Camera::Update(const Window& window, float deltaTime) {
        // --- Movement ---
        glm::vec3 dirHorizontal = glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));

        if (glfwGetKey(window.getWindow(), GLFW_KEY_W) == GLFW_PRESS)
            position += dirHorizontal * speed * deltaTime;
        if (glfwGetKey(window.getWindow(), GLFW_KEY_S) == GLFW_PRESS)
            position -= dirHorizontal * speed * deltaTime;
        if (glfwGetKey(window.getWindow(), GLFW_KEY_A) == GLFW_PRESS)
            position -= glm::normalize(glm::cross(direction, up)) * speed * deltaTime;
        if (glfwGetKey(window.getWindow(), GLFW_KEY_D) == GLFW_PRESS)
            position += glm::normalize(glm::cross(direction, up)) * speed * deltaTime;
        if (glfwGetKey(window.getWindow(), GLFW_KEY_Q) == GLFW_PRESS)
            position -= speed * up * deltaTime;
        if (glfwGetKey(window.getWindow(), GLFW_KEY_E) == GLFW_PRESS)
            position += speed * up * deltaTime;

        // --- Rotation ---
        if(glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            // Hide the mouse
		    glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            // Prevent camera jumping on first click
            if(firstClick) {
                glfwSetCursorPos(window.getWindow(), window.getSize().x / 2.f, window.getSize().y / 2.f);
                firstClick = false;
            }

            // Get deltaMouse
            double mouseX, mouseY;
            glfwGetCursorPos(window.getWindow(), &mouseX, &mouseY);
            
            // Calculate rotation
            float rotX = sensitivity * (float)(mouseY - (window.getSize().y / 2)) / window.getSize().y;
            float rotY = sensitivity * (float)(mouseX - (window.getSize().x / 2)) / window.getSize().x;

		    glm::vec3 newDir = glm::rotate(direction, glm::radians(-rotX), glm::normalize(glm::cross(direction, up)));
            
            // Check if new rotation is valid
            if (abs(glm::angle(newDir, up) - glm::radians(90.0f)) <= glm::radians(89.0f)) {
                direction = newDir;
            }

            // Rotates the Orientation left and right
            direction = glm::rotate(direction, glm::radians(-rotY), up);

            // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
            glfwSetCursorPos(window.getWindow(), (window.getSize().x / 2), (window.getSize().y / 2));

        } else if(glfwGetMouseButton(window.getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            // Show the mouse
		    glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            firstClick = true;
        }

        // --- Update matrix ---
        // Initializes matrices since otherwise they will be the null matrix
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        // Makes camera look in the right direction from the right position
        view = glm::lookAt(position, position + direction, up);
        // Adds perspective to the scene
        projection = glm::perspective(
            glm::radians(FOV),
            (float)window.getSize().x / (float)window.getSize().y,
            near, far
        );

        // Sets new camera matrix
        matrix = projection * view;
    }

    void Camera::UploadMatrix(const Shader& shader, std::string uniform) {
        glUniformMatrix4fv(
            glGetUniformLocation(shader.ID, uniform.c_str()),
            1, GL_FALSE, glm::value_ptr(matrix)
        );
    }

    glm::vec3 Camera::getPosition() const {
        return position;
    }

} // namespace GLwrap


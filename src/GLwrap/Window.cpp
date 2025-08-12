#include "GLwrap/Window.h"

namespace GLwrap
{
    
    Window::Window(int majorVersion, int minorVersion, glm::ivec2 size,
             const std::string& title, bool visible)
             : window(nullptr), size(size), prevTime(0)
    {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorVersion);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, visible ? GLFW_TRUE : GLFW_FALSE);

        window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwDestroyWindow(window);
            glfwTerminate();
            throw std::runtime_error("Failed to initialize GLAD");
        }
    }

    Window::~Window() {
        if (window) {
            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }

    glm::ivec2 Window::getSize() const {
        return size;
    }

    GLFWwindow* Window::getWindow() const {
        return window;
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(window);
    }

    void Window::swapBuffers() const {
        glfwSwapBuffers(window);
    }

    void Window::pollEvents() {
        // Recalculate deltaTime
        double curTime = glfwGetTime();
        deltaTime = curTime - prevTime;
        prevTime = curTime;

        // Poll events
        glfwPollEvents();
    }

} // namespace GLwrap

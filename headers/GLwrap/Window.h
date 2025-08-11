#ifndef GLWRAP_WINDOW_H
#define GLWRAP_WINDOW_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

namespace GLwrap
{
    
    class Window {
    public:
        Window(int majorVersion = 4, int minorVersion = 5,
             glm::ivec2 size = glm::ivec2(800, 600),
             const std::string& title = "OpenGL Window",
             bool visible = true);
        
        ~Window();
        glm::ivec2 getSize() const;
        GLFWwindow* getWindow() const;
        bool shouldClose() const;
        void swapBuffers() const;
        void pollEvents() const;
    private:
        GLFWwindow* window;
        glm::ivec2 size;
    };

} // namespace GLwrap


#endif // GLWRAP_WINDOW_H
#ifndef GLWRAP_WINDOW_H
#define GLWRAP_WINDOW_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace GLwrap
{
    
    class Window {
    public:
        Window(int majorVersion = 4, int minorVersion = 5,
             int width = 800, int height = 600,
             const std::string& title = "OpenGL Window",
             bool visible = true);
        
        ~Window();
        GLFWwindow* getWindow() const;
        bool shouldClose() const;
        void swapBuffers() const;
        void pollEvents() const;
    private:
        GLFWwindow* window;
    };

} // namespace GLwrap


#endif
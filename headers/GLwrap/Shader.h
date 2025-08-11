#ifndef GLWRAP_SHADER_H
#define GLWRAP_SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Utils.h"

namespace GLwrap {
    // Adapted from YoutubeOpenGL: https://github.com/VictorGordan/opengl-tutorials
    class Shader {
    public:
        GLuint ID;
    
        Shader(std::string vertFile, std::string fragFile);
        void Activate();
        void Delete();

        ~Shader();
    
    private:
        void compileErrors(unsigned int shader, const char* type);
    };
} // namespace GLwrap


#endif // GLWRAP_SHADER_H
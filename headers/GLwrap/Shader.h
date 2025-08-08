#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

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
}


#endif // SHADER_CLASS_H
#ifndef COMPUTESHADER_CLASS_H
#define COMPUTESHADER_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "Utils.h"

namespace GLwrap {
    // Adapted from YoutubeOpenGL: https://github.com/VictorGordan/opengl-tutorials
    class ComputeShader {
    public:
        GLuint ID;
    
        ComputeShader(std::string compFile);
        void Activate();
        void Delete();
        void Run(glm::ivec3 dispatches = glm::ivec3(1, 1, 1));

        ~ComputeShader();
    
    private:
        void compileErrors(unsigned int shader, const char* type);
    };
}


#endif // SHADER_CLASS_H
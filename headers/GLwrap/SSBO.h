#ifndef SSBO_H
#define SSBO_H

#include <vector>
#include <glad/glad.h>

namespace GLwrap {
    
    // Class for Shader Storage Buffer Object. It allows storing any arbitrary type of data on the GPU
    template<typename T>
    class SSBO {
    public:
        SSBO(const std::vector<T>& data, GLuint bindingPoint) {
            glGenBuffers(1, &bufferID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, bufferID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        void update(const std::vector<T>& data) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        void bind() const {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, bufferID);
        }

        void unbind() const {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, 0);
        }

        ~SSBO() {
            glDeleteBuffers(1, &bufferID);
        }

        // Disable copy
        SSBO(const SSBO&) = delete;
        SSBO& operator=(const SSBO&) = delete;
        SSBO(SSBO&&) = delete;
        SSBO& operator=(SSBO&&) = delete;


    private:
        GLuint bufferID = 0;
        GLuint bindingPoint = 0;
    };
}

#endif

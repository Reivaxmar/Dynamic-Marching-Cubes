#ifndef GLWRAP_SSBO_H
#define GLWRAP_SSBO_H

#include <vector>
#include <glad/glad.h>

namespace GLwrap {
    
    // Class for Shader Storage Buffer Object. It allows storing any arbitrary type of data on the GPU
    template<typename T>
    class SSBO {
    public:
        SSBO(const std::vector<T>& data, GLuint bindingPoint, GLenum usage = GL_STATIC_DRAW)
            : bindingPoint(bindingPoint), capacity(data.size()), size(data.size()) {
            capacity = data.size();
            glGenBuffers(1, &bufferID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(T), data.data(), usage);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, bufferID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        SSBO(int capacity, GLuint bindingPoint, GLenum usage = GL_STATIC_DRAW)
            : bindingPoint(bindingPoint), capacity(capacity), size(0) {
            glGenBuffers(1, &bufferID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, capacity * sizeof(T), nullptr, usage);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, bufferID);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        void setData(const std::vector<T>& data, bool update_size = true, size_t offset = 0) {
            if(size_enabled && offset + data.size() > capacity) {
                throw std::runtime_error("SSBO write error: requested range exceeds buffer size.");
            }

            // Update the size if requested
            if(update_size || size < offset + data.size())
                size = offset + data.size();

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset * sizeof(T), data.size() * sizeof(T), data.data());
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        std::vector<T> getData(size_t count = 0, size_t offset = 0) const {
            if(count == 0) {
                count = size;
                offset = 0;
            }
            
            if (size_enabled && offset + count > size) {
                throw std::runtime_error("SSBO read error: requested range exceeds buffer size.");
            }


            std::vector<T> result(count);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
            glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset * sizeof(T), count * sizeof(T), result.data());
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

            return result;
        }

        void enableSize(bool enable) {
            size_enabled = enable;
        }

        void bind() const {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, bufferID);
        }

        void unbind() const {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, 0);
        }

        size_t getSize() const {
            return size;
        }
        
        size_t getCapacity() const {
            return capacity;
        }

        GLuint getID() const {
            return bufferID;
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
        size_t capacity = 0, size = 0;
        bool size_enabled = false;
    };
} // namespace GLwrap

#endif // GLWRAP_SSBO_H

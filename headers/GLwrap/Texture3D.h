#ifndef TEXTURE3D_CLASS_H
#define TEXTURE3D_CLASS_H

#include <glad/glad.h>
#include <glm/vec3.hpp>

namespace GLwrap {
    class Texture3D {
    public:
        // Constructor: create an empty texture with given dimensions and format
        Texture3D(glm::ivec3 size = glm::ivec3(0, 0, 0),
                  GLenum internalFormat = GL_RGBA8,
                  GLenum format = GL_RGBA,
                  GLenum type = GL_UNSIGNED_BYTE);
    
        // Non-copyable (manages GPU resource)
        Texture3D(const Texture3D&) = delete;
        Texture3D& operator=(const Texture3D&) = delete;
    
        // Movable
        Texture3D(Texture3D&& other) = delete;
        Texture3D& operator=(Texture3D&& other) = delete;
    
        // Destructor
        ~Texture3D();
    
        // Bind and unbind texture
        void bind(GLuint unit = 0) const;
        static void unbind(GLuint unit = 0);

        // Bind to unit (compute shaders)
        void bindImageUnit(GLuint unit, GLenum access) const;
    
        // Upload data to the texture (replaces entire texture)
        // void setData(const void* data);
        void setData(const void* data, glm::ivec3 offset, glm::ivec3 region);
        void getData(void* outData, glm::ivec3 offset, glm::ivec3 region) const;
    
        // Get texture ID
        GLuint id() const { return textureID; }
    
        // Get dimensions
        glm::ivec3 getSize() const { return size; }
    
    private:
        GLuint textureID = 0;
        glm::ivec3 size = glm::ivec3(0, 0, 0);
        GLenum internalFormat;
        GLenum format;
        GLenum type;
    };
}


#endif
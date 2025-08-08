#include "GLwrap/Texture3D.h"
#include <stdexcept>

namespace GLwrap {

    Texture3D::Texture3D(glm::ivec3 size, GLenum internalFormat, GLenum format, GLenum type)
        : size(size), internalFormat(internalFormat), format(format), type(type)
    {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_3D, textureID);

        // glTexImage3D(GL_TEXTURE_3D, 0, internalFormat,
        //              size.x, size.y, size.z, 0, format, type, nullptr);
        
        glTexStorage3D(GL_TEXTURE_3D, 1, internalFormat, size.x, size.y, size.z);

        // Set default filtering and wrapping
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_3D, 0);
    }

    Texture3D::~Texture3D() {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
        }
    }

    void Texture3D::bind(GLuint unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_3D, textureID);
    }

    void Texture3D::unbind(GLuint unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    void Texture3D::bindImageUnit(GLuint unit, GLenum access) const {
        glBindImageTexture(unit, textureID, 0, GL_TRUE, 0, access, internalFormat);
    }

    // void Texture3D::setData(const void* data) {
    //     glBindTexture(GL_TEXTURE_3D, textureID);
    //     glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0,
    //                     size.x, size.y, size.z, format, type, data);
    //     glBindTexture(GL_TEXTURE_3D, 0);
    // }

    void Texture3D::setData(const void* data, glm::ivec3 offset, glm::ivec3 region) {
        if (region == glm::ivec3(0)) {
            region = size; // Default to full texture
        }

        glBindTexture(GL_TEXTURE_3D, textureID);
        glTexSubImage3D(GL_TEXTURE_3D, 0,
                        offset.x, offset.y, offset.z,
                        region.x, region.y, region.z,
                        format, type, data);
        glBindTexture(GL_TEXTURE_3D, 0);
    }

    void Texture3D::getData(void* outData, glm::ivec3 offset, glm::ivec3 region) const {
        if (region == glm::ivec3(0)) {
            region = size; // Default to full texture
        }

        // Calculate bytes per pixel - assuming 4 bytes here, adjust if needed.
        constexpr int bytesPerPixel = 4;

    #if defined(GL_VERSION_4_5)
        if (offset == glm::ivec3(0) && region == size) {
            // Full texture read: direct call
            glGetTextureImage(textureID, 0, format, type, size.x * size.y * size.z * bytesPerPixel, outData);
        } else {
            // Partial read supported by glGetTextureSubImage in OpenGL 4.5+
            // Note: glGetTextureSubImage API usage here
            glGetTextureSubImage(textureID, 0, offset.x, offset.y, offset.z, region.x, region.y, region.z,
                                format, type, region.x * region.y * region.z * bytesPerPixel, outData);
        }
    #else
        throw std::runtime_error("Use OpenGL 4.5, I'm too lazy to implement without it.");
    #endif
    }



} // namespace GLwrap

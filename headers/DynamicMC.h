#ifndef DYNAMIC_MC_CLASS_H
#define DYNAMIC_MC_CLASS_H

#include "GLwrap/ComputeShader.h"
#include "GLwrap/SSBO.h"
#include "GLwrap/Texture3D.h"

#include "Definitions.h"

#include <glm/glm.hpp>

class DynamicMC {
public:
    DynamicMC();
    ~DynamicMC();

    void update(const std::vector<glm::vec4>& point_cloud);
    GLuint getVAO() const;

// private:
    // BINDING 4: SSBO for point data
    GLwrap::SSBO<glm::vec4> point_buffer;
    // BINDING 6: 3D texture for distances
    GLwrap::Texture3D dist_tex;
    // BINDING 7: SSBO for lookup marching cubes edges
    GLwrap::SSBO<int> edgeTableSSBO;
    // BINDING 8: SSBO for lookup marching cubes triangles
    GLwrap::SSBO<int> triTableSSBO;
    // BINDING 9: SSBO for storing the vertex position
    GLwrap::SSBO<glm::vec4> vertexSSBO;
    // BINDING 10: SSBO for storing the vertex normal
    GLwrap::SSBO<glm::vec4> normalSSBO;
    // BINDING 11: SSBO for keeping track of the current vertex store index
    GLwrap::SSBO<unsigned int> counterSSBO;

    // Compute shader for calculating the distance field
    GLwrap::ComputeShader pointProcess;
    // Compute shader for Marching Cubes
    GLwrap::ComputeShader mcShader;

    // ID for the VAO
    GLuint vaoID;
};

#endif // DYNAMIC_MC_CLASS_H
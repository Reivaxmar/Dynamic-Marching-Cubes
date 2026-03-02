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

    void processPoints(const std::vector<glm::vec4>& point_cloud, const std::vector<glm::vec4>& colors, const glm::vec3& camPos);
    void updateMesh();

    void Draw(bool drawGrid = true);

private:
    // BINDING 4: SSBO for point data
    GLwrap::SSBO<glm::vec4> point_buffer;
    // BINDING 5: 3D texture for TSDF
    GLwrap::Texture3D tsdf_tex;
    // BINDING 6: 3D texture for TSDF weight
    GLwrap::Texture3D weight_tex;
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
    // BINDING 12: 3D texture for TSDF color
    GLwrap::Texture3D color_tex;
    // BINDING 13: SSBO for point color data
    GLwrap::SSBO<glm::vec4> color_buffer;
    // BINDING 14: SSBO for storing the vertex color
    GLwrap::SSBO<glm::vec4> colorSSBO;

    // Compute shader for calculating the distance field
    GLwrap::ComputeShader pointProcess;
    // Compute shader for Marching Cubes
    GLwrap::ComputeShader mcShader;

    // ID for the Marching Cubes VAO
    GLuint MCvaoID;
    // ID for the grid VAO
    GLuint Gridvao;
};

#endif // DYNAMIC_MC_CLASS_H
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

    void processPoints(const std::vector<Point>& point_cloud, const glm::vec3& camPos);
    void updateMesh();

    void Draw(bool drawGrid = true);

private:
    // BINDING 0: SSBO for point data
    GLwrap::SSBO<Point> point_buffer;
    // BINDING 1: 3D texture for TSDF
    GLwrap::Texture3D tsdf_tex;
    // BINDING 2: 3D texture for TSDF weight
    GLwrap::Texture3D weight_tex;
    // BINDING 3: 3D texture for TSDF color
    GLwrap::Texture3D color_tex;
    // BINDING 4: SSBO for lookup marching cubes edges
    GLwrap::SSBO<int> edgeTableSSBO;
    // BINDING 5: SSBO for lookup marching cubes triangles
    GLwrap::SSBO<int> triTableSSBO;
    // BINDING 6: SSBO for storing the vertex position
    GLwrap::SSBO<glm::vec4> vertexSSBO;
    // BINDING 7: SSBO for storing the vertex normal
    GLwrap::SSBO<glm::vec4> normalSSBO;
    // BINDING 8: SSBO for keeping track of the current vertex store index
    GLwrap::SSBO<unsigned int> counterSSBO;

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
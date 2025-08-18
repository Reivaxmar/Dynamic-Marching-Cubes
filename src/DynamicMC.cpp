#include "DynamicMC.h"

DynamicMC::DynamicMC()
    : point_buffer(EXPECTED_CAPACITY, 4)
    , dist_tex(GRIDSIZE, GL_R32F, GL_RED, GL_FLOAT)
    , edgeTableSSBO(std::vector<int>(edgeTable, edgeTable + 256), 7)
    , triTableSSBO(std::vector<int>(triTable[0], triTable[0] + 256 * 16), 8)
    , vertexSSBO(GRIDSIZE.x * GRIDSIZE.y * GRIDSIZE.z * 15, 9, GL_DYNAMIC_DRAW)
    , normalSSBO(GRIDSIZE.x * GRIDSIZE.y * GRIDSIZE.z * 15, 10, GL_DYNAMIC_DRAW)
    , counterSSBO(1, 11, GL_DYNAMIC_DRAW)
    , pointProcess("assets/shaders/processPoints.comp")
    , mcShader("assets/shaders/marchingCubes.comp")
    {
    
    // Bind the texture
    dist_tex.bindImageUnit(6, GL_READ_WRITE);
    
    // Reset the counter
    counterSSBO.setData({0});


    // The vertex SSBO will be set to be the VBO
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    // Bind the vertices to the VAO
    glBindBuffer(GL_ARRAY_BUFFER, vertexSSBO.getID());
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

    // Bind the normals to the VAO
    glBindBuffer(GL_ARRAY_BUFFER, normalSSBO.getID());
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

DynamicMC::~DynamicMC() {}

void DynamicMC::update(const std::vector<glm::vec4>& point_cloud) {

    // Upload to the GPU the data
    point_buffer.setData(point_cloud);

    // Beginning point distance
    pointProcess.Activate();

    // Set the radius on the shader
    glUniform1i(glGetUniformLocation(pointProcess.ID, "radius"), (int)RADIUS_SIZE + 1);
    // Set the number of elements on the shader
    glUniform1i(glGetUniformLocation(pointProcess.ID, "numElements"), (int)point_cloud.size());

    // Run the point distance shader
    pointProcess.Run(glm::ivec3((point_cloud.size() + 7) / 8, 1, 1));


    // Beginning Marching Cubes
    mcShader.Activate();
    glUniform1f(glGetUniformLocation(mcShader.ID, "isoLevel"), 0.5f);
    glUniform3f(glGetUniformLocation(mcShader.ID, "gridScale"), 1.0f, 1.0f, 1.0f);
    glUniform3i(glGetUniformLocation(mcShader.ID, "gridSize"), GRIDSIZE.x, GRIDSIZE.y, GRIDSIZE.z);
    
    // Reset the counter
    counterSSBO.setData({0});
    
    // Run the marching cubes shader
    mcShader.Run(GRIDSIZE / 8);

    // Memory barrier to read SSBOs
    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT |
        GL_BUFFER_UPDATE_BARRIER_BIT |
        GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
    );

}

GLuint DynamicMC::getVAO() const {
    return vaoID;
}
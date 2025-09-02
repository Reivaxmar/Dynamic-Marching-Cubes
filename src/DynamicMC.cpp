#include "DynamicMC.h"

DynamicMC::DynamicMC()
    : point_buffer(EXPECTED_CAPACITY, 0)
    , tsdf_tex(GRIDSIZE, GL_R32F, GL_RED, GL_FLOAT)
    , weight_tex(GRIDSIZE, GL_R32F, GL_RED, GL_FLOAT)
    , edgeTableSSBO(std::vector<int>(edgeTable, edgeTable + 256), 4)
    , triTableSSBO(std::vector<int>(triTable[0], triTable[0] + 256 * 16), 5)
    , vertexSSBO(GRIDSIZE.x * GRIDSIZE.y * GRIDSIZE.z * 15, 6, GL_DYNAMIC_DRAW)
    , normalSSBO(GRIDSIZE.x * GRIDSIZE.y * GRIDSIZE.z * 15, 7, GL_DYNAMIC_DRAW)
    , counterSSBO(1, 8, GL_DYNAMIC_DRAW)
    , pointProcess("assets/shaders/processPoints.comp")
    , mcShader("assets/shaders/marchingCubes.comp")
    {
    
    // Bind the texture
    tsdf_tex.bindImageUnit(1, GL_READ_WRITE);
    weight_tex.bindImageUnit(2, GL_READ_WRITE);
    
    // Reset the counter
    counterSSBO.setData({0});


    // The vertex SSBO will be set to be the VBO
    glGenVertexArrays(1, &MCvaoID);
    glBindVertexArray(MCvaoID);

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

    GLuint gridVbo;
    glGenVertexArrays(1, &Gridvao);
    glGenBuffers(1, &gridVbo);
    glBindVertexArray(Gridvao);
    glBindBuffer(GL_ARRAY_BUFFER, gridVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridCubeVerts), gridCubeVerts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

DynamicMC::~DynamicMC() {}

void DynamicMC::processPoints(const std::vector<Point>& point_cloud, const glm::vec3& camPos) {
    // Upload to the GPU the data
    point_buffer.setData(point_cloud);
    

    // Beginning point distance
    pointProcess.Activate();

    // Set uniforms
    // glUniform1i(glGetUniformLocation(pointProcess.ID, "radius"), (int)RADIUS_SIZE + 1);
    glUniform1i(glGetUniformLocation(pointProcess.ID, "numElements"), (int)point_cloud.size());
    glUniform1i(glGetUniformLocation(pointProcess.ID, "truncDist"), (int)RADIUS_SIZE);
    glUniform3f(glGetUniformLocation(pointProcess.ID, "camPos"), camPos.x, camPos.y, camPos.z);
    glUniform1i(glGetUniformLocation(pointProcess.ID, "maxWeight"), 1);
    glUniform1i(glGetUniformLocation(pointProcess.ID, "useNeighborhood"), GL_TRUE);
    glUniform3i(glGetUniformLocation(pointProcess.ID, "gridResolution"), GRIDSIZE.x, GRIDSIZE.y, GRIDSIZE.z);

    // Run the point distance shader
    pointProcess.Run(glm::ivec3((point_cloud.size() + 7) / 8, 1, 1));

    glMemoryBarrier(
        GL_SHADER_STORAGE_BARRIER_BIT |
        GL_BUFFER_UPDATE_BARRIER_BIT |
        GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT
    );
}

void DynamicMC::updateMesh() {
    // Beginning Marching Cubes
    mcShader.Activate();

    tsdf_tex.bind(1);

    glUniform1i(glGetUniformLocation(mcShader.ID, "dist_sampler"), 5);

    // Set uniforms
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

void DynamicMC::Draw(bool drawGrid) {

    // Draw Marching Cubes mesh
    glBindVertexArray(MCvaoID);
    glDrawArrays(GL_TRIANGLES, 0, counterSSBO.getData(1)[0]);

    if (drawGrid) {
        glBindVertexArray(Gridvao);
        // 24 vertices for 12 edges (lines)
        glDrawArrays(GL_LINES, 0, 24);
        glBindVertexArray(0);
    }
}
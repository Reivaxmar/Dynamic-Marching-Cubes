#include <iostream>
#include <vector>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "Definitions.h"

#include "GLwrap/ComputeShader.h"
#include "GLwrap/SSBO.h"
#include "GLwrap/Texture3D.h"
#include "GLwrap/Window.h"
#include "GLwrap/Shader.h"

int main() {
    // Create the window
    GLwrap::Window window(4, 5, 800, 600, "Dynamic Marching Cubes", false);

    
    // Create a random number generator
    std::random_device rd;
    std::mt19937 rng(rd());

    // Generate an arbitrary point cloud
    int n = 128;
    std::vector<glm::vec4> point_cloud(n);

    for(int i = 0; i < n; i++) {
        // point_cloud[i] = glm::vec4(GLwrap::randomVec3(rng), 0);
        point_cloud[i] = glm::vec4(i, i, i, 0);
    }


    // Create the shader buffer for the point data
    GLwrap::SSBO<glm::vec4> point_buffer(point_cloud, 0);

    // Create the update texture
    GLwrap::Texture3D update_tex(GRIDSIZE, GL_R32F, GL_RED, GL_FLOAT);
    update_tex.bindImageUnit(1, GL_WRITE_ONLY);

    // Create the distance texture
    GLwrap::Texture3D dist_tex(GRIDSIZE, GL_R32F, GL_RED, GL_FLOAT);
    dist_tex.bindImageUnit(2, GL_READ_WRITE);


    // Create the compute shader
    GLwrap::ComputeShader pointProcess("assets/shaders/processPoints.comp");
    pointProcess.Activate();

    // Set the radius on the shader
    glUniform1i(glGetUniformLocation(pointProcess.ID, "radius"), (int)RADIUS_SIZE);
    // Set the number of elements on the shader
    glUniform1i(glGetUniformLocation(pointProcess.ID, "numElements"), (int)point_cloud.size());

    // Run the shader
    pointProcess.Run(glm::ivec3((point_cloud.size() + 7) / 8, 1, 1));

    

    // Create the SSBO for the edge table and triangle table
    GLwrap::SSBO<int> edgeTableSSBO(std::vector<int>(edgeTable, edgeTable + 256), 3);
    GLwrap::SSBO<int> triTableSSBO(std::vector<int>(triTable[0], triTable[0] + 256 * 16), 4);

    
    // Create the SSBO for the generated vertices
    size_t maxVerts = GRIDSIZE.x * GRIDSIZE.y * GRIDSIZE.z * 15; // generous
    std::vector<glm::vec4> emptyVerts(maxVerts);
    GLwrap::SSBO<glm::vec4> vertexSSBO(emptyVerts, 5);

    // Atomic counter for vertex counter (for storing the current vertex)
    // Create a small SSBO to act as the vertex counter (binding = 6)
    std::vector<unsigned int> counterInit(1, 0u);
    GLwrap::SSBO<unsigned int> counterSSBO(counterInit, 6);

    // Reset it to zero just in case (alternate safe path)
    counterInit[0] = 0u;
    counterSSBO.update(counterInit);

    
    // Marching Cubes compute shader
    GLwrap::ComputeShader mcShader("assets/shaders/marchingCubes.comp");
    mcShader.Activate();
    glUniform1f(glGetUniformLocation(mcShader.ID, "isoLevel"), 0.5f);
    glUniform3f(glGetUniformLocation(mcShader.ID, "gridScale"), 1.0f, 1.0f, 1.0f);
    glUniform3i(glGetUniformLocation(mcShader.ID, "gridSize"), GRIDSIZE.x, GRIDSIZE.y, GRIDSIZE.z);

    // Reset counter
    counterInit[0] = 0u;
    counterSSBO.update(counterInit);

    mcShader.Run(GRIDSIZE / 8);

    // ------- memory barrier so SSBO writes are visible to CPU/GPU copies/readbacks -------
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

    // ------- read back the counter from the SSBO (NOT from GL_ATOMIC_COUNTER_BUFFER) -------
    unsigned int vertexCount = 0u;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, counterSSBO.getID());
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(unsigned int), &vertexCount);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    std::cout << "Generated " << vertexCount << " vertices.\n";

    // Read back vertices
    std::vector<glm::vec4> vertices(vertexCount);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexSSBO.getID());
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, vertexCount * sizeof(glm::vec4), vertices.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Write OBJ file
    std::ofstream objFile("output.obj");
    if (!objFile) {
        std::cerr << "Failed to open output.obj for writing\n";
        return 1;
    }

    // Positions
    std::cout << "Writing vertices\n";
    int m = 0;
    for (const auto& v : vertices) {
        objFile << "v " << v.x << " " << v.y << " " << v.z << "\n";
        m++;
        if(m % 10000 == 0) std::cout << "iteration: " << m << std::endl;
    }
    
    // Faces: every 3 vertices = 1 triangle
    std::cout << "Writing faces\n";
    m = 0;
    for (size_t i = 0; i < vertexCount; i += 3) {
        objFile << "f " << i + 1 << " " << i + 2 << " " << i + 3 << "\n";
        m++;
        if(n % 10000 == 0) std::cout << "iteration: " << m << std::endl;
    }

    objFile.close();
    std::cout << "Saved mesh to output.obj\n";

    return 0;


    // ------------------------------------
    // VAO for rendering from SSBO
    // ------------------------------------
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_COPY_READ_BUFFER, vertexSSBO.getID());
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, vertexCount * sizeof(glm::vec4));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);

    // Simple shader to display triangles
    // Default rendering shader
    GLwrap::Shader defaultShader("assets/shaders/default.vert", "assets/shaders/default.frag");
    defaultShader.Activate();

    // ------------------------------------
    // Main render loop
    // ------------------------------------
    while (!window.shouldClose()) {
        window.pollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        defaultShader.Activate();
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        window.swapBuffers();
    }
}

#include <iostream>
#include <vector>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Definitions.h"
#include "DynamicMC.h"
#include "NetReceiver.h"

#include "GLwrap/Window.h"
#include "GLwrap/Shader.h"
#include "GLwrap/Camera.h"

int main() {
    // Network handler for getting data from the scanner
    NetReceiver network(5000);
    
    // Create the window
    GLwrap::Window window(4, 5, {1600, 900}, "Dynamic Marching Cubes");

    // Dynamic Marching Cubes mesh reconstructor
    DynamicMC MarchingCubes;


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    // Create the camera
    GLwrap::Camera camera;
    // Create the rendering shader
    GLwrap::Shader defaultShader("assets/shaders/default.vert", "assets/shaders/default.frag");

    int cur_s = 0;

    while(!window.shouldClose()) {
        // Poll events
        window.pollEvents();

        if((int)glfwGetTime() > cur_s) {
            // Update title to show FPS
            std::string newTitle = "Dynamic Marching Cubes - FPS: " + GLwrap::floatprec(1.f/window.deltaTime, 2);
            glfwSetWindowTitle(window.getWindow(), newTitle.c_str());
            cur_s++;
        }

        // Get latest point cloud data from the device
        std::vector<glm::vec4> upload;
        network.GetPointCloud(upload);

        // If there's something new, update the mesh
        if(!upload.empty()) {
            MarchingCubes.update(upload);
        }


        // Update camera position and matrix
        camera.Update(window);

        // Clear the window
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Activate shader
        defaultShader.Activate();
        // Upload uniforms
        camera.UploadMatrix(defaultShader, "uMVP");
        glUniform3fv(glGetUniformLocation(defaultShader.ID, "lightPos"), 1, glm::value_ptr(camera.getPosition()));
        glUniform3fv(glGetUniformLocation(defaultShader.ID, "viewPos"), 1, glm::value_ptr(camera.getPosition()));
        // Draw the Marching Cubes mesh
        glBindVertexArray(MarchingCubes.getVAO());
        glDrawArrays(GL_TRIANGLES, 0, MarchingCubes.counterSSBO.getData(1)[0]);

        // Display
        window.swapBuffers();

    }

    return 0;
}

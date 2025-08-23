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
    // glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    // Create the camera
    GLwrap::Camera camera;
    // Create the rendering shader
    GLwrap::Shader defaultShader("assets/shaders/default.vert", "assets/shaders/default.frag");

    int cur_s = 0;
    double lastTime = glfwGetTime();
    double updTime = 0.25; // Update MC mesh time interval


#ifdef DEBUG_POINTS

    std::vector<glm::vec4> all_points;

    GLuint particleVAO = 0, particleVBO = 0;
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);
    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, EXPECTED_CAPACITY * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glBindVertexArray(0);

#endif

    while(!window.shouldClose()) {
        // Poll events
        window.pollEvents();

        double curTime = glfwGetTime();

        while((int)curTime > cur_s) {
            // Update title to show FPS
            std::string newTitle = "Dynamic Marching Cubes - FPS: " + GLwrap::floatprec(1.f/window.deltaTime, 2);
            glfwSetWindowTitle(window.getWindow(), newTitle.c_str());
            cur_s++;
        }

        if(!network.IsCalibrating()) {
            // Get latest point cloud data from the device
            std::vector<glm::vec4> upload;
            glm::vec3 camPos;
            network.GetPointCloud(upload, camPos);
    
            // If there's something new, update the mesh
            if(!upload.empty()) {
                #ifdef DEBUG_POINTS
                all_points.insert(all_points.end(), upload.begin(), upload.end());
                #else
                MarchingCubes.processPoints(upload, camPos);
                // Follow the scanner camera
                // camera.setPosition(camPos);
                // TODO: send/receive the camera direction. Or just send the matrix ;)
                #endif
            }
        }

        // Update mesh
        while(curTime - lastTime >= updTime) {
            MarchingCubes.updateMesh();
            lastTime += updTime;
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
        
        #ifdef DEBUG_POINTS

        // Draw points on the data
        glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
        glBufferData(GL_ARRAY_BUFFER, all_points.size() * sizeof(glm::vec4), all_points.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(particleVAO);
        glDrawArrays(GL_POINTS, 0, all_points.size());
        glBindVertexArray(0);

        #else
        
        // Draw the Marching Cubes mesh
        MarchingCubes.Draw();

        #endif

        // Display
        window.swapBuffers();

    }

    return 0;
}

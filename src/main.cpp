#include <iostream>
#include <vector>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Definitions.h"
#include "DynamicMC.h"

#include "GLwrap/Window.h"
#include "GLwrap/Shader.h"
#include "GLwrap/Camera.h"

int main() {
    // Create the window
    GLwrap::Window window(4, 5, {1600, 900}, "Dynamic Marching Cubes");
    glfwSwapInterval(0); // Remove vSync

    
    // Create a random number generator
    std::random_device rd;
    std::mt19937 rng(rd());

    // Generate an arbitrary point cloud
    int n = 25000;
    std::vector<glm::vec4> point_cloud(n);

    for(int i = 0; i < n; i++) {
        point_cloud[i] = glm::vec4(GLwrap::randomVec3(rng), 0);
        // point_cloud[i] = glm::vec4(i, i, (i-32)*(i-32)/16, 0);
    }

    DynamicMC MarchingCubes;


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    GLwrap::Camera camera;


    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    GLwrap::Shader defaultShader("assets/shaders/default.vert", "assets/shaders/default.frag");

    while(!window.shouldClose()) {
        // Time logic for consistent movement speed
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        bool changeTime = false;
        if((int)(currentFrame) > (int)(lastFrame)) {
            std::cout << "FPS: " << 1.f / deltaTime << std::endl;
            changeTime = true;
        }
        lastFrame = currentFrame;


        window.pollEvents();

        glm::vec4 ran_vec = glm::vec4(GLwrap::randomVec3(rng), 0);
        for(int i = 0; i < n; i++) {
            point_cloud[i] = ran_vec;
            // point_cloud[i] = glm::vec4(i, i, (i-32)*(i-32)/16, 0);
        }
        MarchingCubes.update(point_cloud);

        camera.Update(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        defaultShader.Activate();

        camera.UploadMatrix(defaultShader, "uMVP");

        glm::vec3 lightPos(1.0f, 1.0f, 2.0f);
        glUniform3fv(glGetUniformLocation(defaultShader.ID, "lightPos"), 1, glm::value_ptr(camera.getPosition()));
        glUniform3fv(glGetUniformLocation(defaultShader.ID, "viewPos"), 1, glm::value_ptr(camera.getPosition())); // updated

        glBindVertexArray(MarchingCubes.getVAO());
        glDrawArrays(GL_TRIANGLES, 0, MarchingCubes.counterSSBO.getData(1)[0]);

        window.swapBuffers();

    }

    return 0;
}

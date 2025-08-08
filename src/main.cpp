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

int main() {
    // Create the window
    GLwrap::Window window(4, 5, 800, 600, "Dynamic Marching Cubes", false);

    
    // Create a random number generator
    std::random_device rd;
    std::mt19937 rng(rd());

    // Generate an arbitrary point cloud
    int n = 25000;
    std::vector<glm::vec4> point_cloud(n);

    for(int i = 0; i < n; i++) {
        point_cloud[i] = glm::vec4(GLwrap::randomVec3(rng), 0);
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


    // Read back each slice and save as PNG
    std::vector<float> sliceData(GRIDSIZE.x * GRIDSIZE.y); // Only R channel

    // Bind texture for reading
    dist_tex.bind();

    for (int z = 0; z < GRIDSIZE.z; ++z) {
        // Read a 2D slice (layer) from the 3D texture
        dist_tex.getData(sliceData.data(), glm::ivec3(0, 0, z), glm::ivec3(GRIDSIZE.x, GRIDSIZE.y, 1));

        std::vector<unsigned char> imageData(GRIDSIZE.x * GRIDSIZE.y * 4, 0);
        for (int i = 0; i < GRIDSIZE.x * GRIDSIZE.y; ++i) {
            unsigned char val = static_cast<unsigned char>(std::clamp(sliceData[i], 0.0f, 1.0f) * 255.0f);
            imageData[i * 4 + 0] = val; // R
            imageData[i * 4 + 1] = val; // G
            imageData[i * 4 + 2] = val; // B
            imageData[i * 4 + 3] = 255; // A
        }

        // Save PNG (layer_z.png)
        char filename[64];
        snprintf(filename, sizeof(filename), "../images/layer_%03d.png", z);
        stbi_write_png(filename, GRIDSIZE.x, GRIDSIZE.y, 4, imageData.data(), GRIDSIZE.x * 4);
    }

    std::cout << "Done saving " << GRIDSIZE.z << " PNG slices.\n";

    system("cd ../images && ffmpeg -framerate 30 -i layer_%03d.png -c:v libx264 -pix_fmt yuv420p ../output.mp4");
    

    return 0;
}

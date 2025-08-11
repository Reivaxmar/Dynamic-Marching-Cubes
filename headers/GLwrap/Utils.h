#ifndef GLWRAP_UTILS_H
#define GLWRAP_UTILS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace GLwrap {
    // Reads a text file and outputs a string with everything in the text file
    std::string get_file_contents(std::string filename);
    
    // Given an random device, generates a random vec3
    glm::vec3 randomVec3(std::mt19937& rng);

    // Save a list of vertices to an obj
    void saveToFile(
        std::string filename,
        const std::vector<glm::vec4>& points,
        const std::vector<glm::vec4>& normals
    );
} // namespace GLwrap


#endif // GLWRAP_UTILS_H
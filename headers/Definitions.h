#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <glm/vec3.hpp>

// #define DEBUG_POINTS // Show points instead of Marching Cubes

const glm::ivec3 GRIDSIZE = { 128, 128, 128 };
const float ZERO = 0.0f;
const float RADIUS_SIZE = 4.0f;
const int EXPECTED_CAPACITY = 50000; // 50k points per frame

// Marching cubes tables. Source: https://paulbourke.net/geometry/polygonise/
extern const int edgeTable[256];
extern const int triTable[256][16];

#endif
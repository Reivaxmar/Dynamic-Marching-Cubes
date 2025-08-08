# How the algorithm works internally

There will be two compute shaders: one to process the point data, and one to process the Marching Cubes.

## Processing point cloud data

The program will first begin by creating a buffer for storing the given point cloud data (a buffer of size around ~25k), and two 3D textures, one for storing the nearest distance to a point, where `1.0` will be a point that will be more than `r` far away, and `0.0 - 1.0` is a point with distance from `0.0 - r` scaled down by `r`.

Then, the C++ program will receive a vector of `glm::vec3`, of around `v.size() = 25000`. Then, the program will pass that data to the point cloud buffer via a SSBO. It will also pass

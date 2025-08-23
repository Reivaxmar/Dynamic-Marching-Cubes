# Dynamic 3D scanner mesh generation
This project is the server program where it receives data from a 3D scanner and dynamically
updates a 3D mesh with Marching Cubes.

This project is part of my '_Treball de Recerca_', a project that must be done in Catalonia for graduating '_Batxillerat_' (last two high school years)

## Screenshots
_Add screenshots..._

## Usage
### Prerequisites
Just some basic stuff:
- Modern C/C++ compiler
- [CMake](https://cmake.org/download/) installed
- Compatibilty with OpenGL 4.5

### Building the project
Download a zip file or clone the repository:
```bash
> git clone \
    https://github.com/Reivaxmar/TR-Dynamic-Marching-Cubes.git \
    TR-Dynamic-Marching-Cubes
> cd TR-Dynamic-Marching-Cubes
```

Great! Now that the project is downloaded, it must be built:
```bash
> bash ./build.sh
> ./MarchingCubes
```

Now the server should be up and running.


## External libraries
There is no need to have them installed, CMake will do it:
- [GLAD](https://glad.dav1d.de/) for loading OpenGL
- [GLFW](https://www.glfw.org/) for OpenGL window managing
- [GLM](https://github.com/g-truc/glm) for 3D math utils
- [asio](https://github.com/chriskohlhoff/asio) for networking

## TODOs
- [x] Implement distance shader
- [x] Implement Marching Cubes shader
- [x] Fix Marching Cubes shader
- [x] Implement GLwrap library
- [x] Add network receiving
- [x] Fix async and bugs on networking
- [x] Update README.md
- [x] Do iOS app
- [x] Fix iOS app
- [ ] Merge iOS app repository
- [x] Calibrate world position and sphere radius
- [ ] Add screenshots to README.md
- [ ] Explain algorithm in Algorithm.md (or add link to paper)
- [x] Option for smooth shading (use gradients)
- [x] Implement TSDF instead of the weird thing we have
- [ ] Add colors / textures
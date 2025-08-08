cmake_minimum_required(VERSION 3.16)

project(MarchingCubes)

set(CMAKE_CXX_STANDARD 17)

# Get all the files
file(GLOB_RECURSE SOURCES src/*.cpp)
include_directories(headers /usr/include)
add_executable(${PROJECT_NAME} ${SOURCES})


#------------------------------------------------------------------------------------
# Add dependencies here
#------------------------------------------------------------------------------------

find_package(OpenGL REQUIRED)

# Fetch GLFW
include(FetchContent)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# Fetch GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)


target_link_libraries(${PROJECT_NAME} glad OpenGL::GL glfw glm)

#------------------------------------------------------------------------------------

# Create a custom target to copy the assets
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying assets to the build directory"
)

add_dependencies(${PROJECT_NAME} copy_assets)

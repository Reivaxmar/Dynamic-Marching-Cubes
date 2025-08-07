cmake_minimum_required(VERSION 3.16)

# Set project name
project(proj)

set(CMAKE_CXX_STANDARD 17)

# Get all the files
file(GLOB_RECURSE SOURCES src/*.cpp)
include_directories(headers /usr/include)
add_executable(${PROJECT_NAME} ${SOURCES})


#------------------------------------------------------------------------------------
# Add dependencies here
#------------------------------------------------------------------------------------

find_package(SFML 2.5 COMPONENTS graphics QUIET)
if(NOT SFML_FOUND)
    message(">>> Installing SFML because it wasn't found")
    # Install SFML
    include(FetchContent)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
    FetchContent_MakeAvailable(SFML)
endif()
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

#------------------------------------------------------------------------------------

# Create a custom target to copy the assets
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying assets to the build directory"
)

# Make sure the 'copy_assets' target is run before building the main executable
add_dependencies(${PROJECT_NAME} copy_assets)

#version 450 core

layout(location = 0) in vec3 inPos;    // Vertex position from SSBO/VBO
layout(location = 1) in vec3 inNormal; // Vertex normal from SSBO/VBO

uniform mat4 uMVP = mat4(1.0);
uniform mat4 uModel = mat4(1.0); // for transforming normals if needed

out vec3 vNormal; // pass to fragment shader

void main()
{
    // Transform position
    gl_Position = uMVP * vec4(inPos, 1.0);

    // Transform normal (ignore translation, just rotation/scale)
    vNormal = mat3(uModel) * inNormal;
}

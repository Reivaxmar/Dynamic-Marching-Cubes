#version 330 core

out vec4 FragColor;

uniform vec3 meshColor; // Set this from your application
uniform vec3 lightPos;  // Light position in world space
uniform vec3 viewPos;   // Camera position in world space
in vec3 FragPos;        // Fragment position in world space (from vertex shader)
in vec3 Normal;         // Normal vector (from vertex shader)

void main()
{
    // Ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * meshColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * meshColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * vec3(1.0);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
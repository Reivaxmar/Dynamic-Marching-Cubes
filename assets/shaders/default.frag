#version 450 core

in vec3 vNormal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);  // White light diffuse color
    vec3 ambient = 0.2 * vec3(1.0, 1.0, 1.0);

    vec3 color = ambient + diffuse;

    FragColor = vec4(color, 1.0);
}

#version 450 core

in vec3 vNormal;
out vec4 FragColor;

uniform vec3 skyColor    = vec3(0.6, 0.8, 1.0); // light blue sky
uniform vec3 groundColor = vec3(0.3, 0.25, 0.2); // brownish ground

void main()
{
    // Normalize the interpolated normal
    vec3 N = normalize(vNormal);

    // Simple "sky lighting": blend sky color if facing up, ground if facing down
    float upFactor = clamp(N.y * 0.5 + 0.5, 0.0, 1.0); // map [-1,1] → [0,1]
    vec3 color = mix(groundColor, skyColor, upFactor);

    FragColor = vec4(color, 1.0);
}

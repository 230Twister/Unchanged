#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    // ReinhardÉ«µ÷Ó³Éä
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    
    FragColor = vec4(result, 1.0);
}
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform float exposure;
uniform bool dying;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    // ReinhardÉ«µ÷Ó³Éä
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    
    FragColor = vec4(result, 1.0);

    // ±ôËÀ×´Ì¬
    if (dying) {
        float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
        FragColor = vec4(average, average, average, 1.0f);
    }
}
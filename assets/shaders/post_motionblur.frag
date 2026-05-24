#version 430 core
in vec2 vUV;
uniform sampler2D uScene;
uniform vec2 uVelocity;
uniform float uStrength;
out vec4 FragColor;

void main() {
    vec3 col = vec3(0);
    int  samples = 8;
    for (int i = 0; i < samples; ++i) {
        float t = float(i) / float(samples - 1) - 0.5;
        col += texture(uScene, vUV + uVelocity * t * uStrength).rgb;
    }
    FragColor = vec4(col / float(samples), 1.0);
}

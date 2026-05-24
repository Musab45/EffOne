#version 430 core
in vec2 vUV;
uniform sampler2D uScene;
uniform float uStrength;
out vec4 FragColor;

void main() {
    vec2 dir = (vUV - 0.5) * uStrength * 0.012;
    float r = texture(uScene, vUV + dir).r;
    float g = texture(uScene, vUV).g;
    float b = texture(uScene, vUV - dir).b;
    FragColor = vec4(r, g, b, 1.0);
}

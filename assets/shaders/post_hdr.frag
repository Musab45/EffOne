#version 430 core
in vec2 vUV;
uniform sampler2D uScene;
uniform float uExposure;
out vec4 FragColor;

vec3 aces(vec3 x) {
    float a=2.51,b=0.03,c=2.43,d=0.59,e=0.14;
    return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

void main() {
    vec3 col = texture(uScene, vUV).rgb * uExposure;
    col = aces(col);
    col = pow(col, vec3(1.0/2.2)); // gamma
    // Vignette
    vec2 uv = vUV - 0.5;
    float vig = 1.0 - dot(uv,uv)*2.2;
    col *= vig;
    FragColor = vec4(col, 1.0);
}

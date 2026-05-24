#version 430 core
in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;
in vec4 vLightSpacePos;

uniform vec3  uAlbedo;
uniform float uMetallic;
uniform float uRoughness;
uniform vec3  uCamPos;
uniform vec3  uSunDir;
uniform vec3  uSunColor;
uniform sampler2D uShadowMap;

out vec4 FragColor;

const float PI = 3.14159265;

float shadowFactor() {
    vec3 proj = vLightSpacePos.xyz / vLightSpacePos.w;
    proj = proj * 0.5 + 0.5;
    if (proj.z > 1.0) return 1.0;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    for(int x=-1; x<=1; x++) for(int y=-1; y<=1; y++) {
        float pcfDepth = texture(uShadowMap, proj.xy + vec2(x,y)*texelSize).r;
        shadow += (proj.z - 0.002 > pcfDepth) ? 0.3 : 1.0;
    }
    return shadow / 9.0;
}

float D_GGX(float NdH, float rough) {
    float a=rough*rough, a2=a*a, d=NdH*NdH*(a2-1.0)+1.0;
    return a2/(PI*d*d);
}
float G_Smith(float NdV, float NdL, float rough) {
    float k=(rough+1.0)*(rough+1.0)/8.0;
    float gv=NdV/(NdV*(1.0-k)+k), gl=NdL/(NdL*(1.0-k)+k);
    return gv*gl;
}
vec3 F_Schlick(float cosT, vec3 F0) {
    return F0+(1.0-F0)*pow(1.0-cosT,5.0);
}

void main() {
    vec3 N  = normalize(vNormal);
    vec3 V  = normalize(uCamPos - vWorldPos);
    vec3 L  = normalize(-uSunDir);
    vec3 H  = normalize(V+L);
    float NdL=max(dot(N,L),0.0), NdV=max(dot(N,V),0.001), NdH=max(dot(N,H),0.0);

    vec3 F0 = mix(vec3(0.04), uAlbedo, uMetallic);
    vec3 F  = F_Schlick(max(dot(H,V),0.0), F0);
    float D = D_GGX(NdH, uRoughness);
    float G = G_Smith(NdV, NdL, uRoughness);
    vec3 spec   = (D*G*F) / max(4.0*NdV*NdL, 0.001);
    vec3 kd     = (1.0-F)*(1.0-uMetallic);
    vec3 diffuse= kd * uAlbedo / PI;

    vec3 ambient = uAlbedo * 0.08;
    vec3 color   = (diffuse+spec)*uSunColor*NdL*shadowFactor() + ambient;
    FragColor    = vec4(color, 1.0);
}

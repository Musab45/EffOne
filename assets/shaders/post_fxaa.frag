#version 430 core
in vec2 vUV;
uniform sampler2D uScene;
uniform vec2 uTexelSize;
out vec4 FragColor;

void main() {
    vec3 rgbNW = texture(uScene, vUV + vec2(-1,-1)*uTexelSize).rgb;
    vec3 rgbNE = texture(uScene, vUV + vec2( 1,-1)*uTexelSize).rgb;
    vec3 rgbSW = texture(uScene, vUV + vec2(-1, 1)*uTexelSize).rgb;
    vec3 rgbSE = texture(uScene, vUV + vec2( 1, 1)*uTexelSize).rgb;
    vec3 rgbM  = texture(uScene, vUV).rgb;
    vec3 luma  = vec3(0.299,0.587,0.114);
    float lumNW=dot(rgbNW,luma), lumNE=dot(rgbNE,luma);
    float lumSW=dot(rgbSW,luma), lumSE=dot(rgbSE,luma), lumM=dot(rgbM,luma);
    float lumMin=min(lumM,min(min(lumNW,lumNE),min(lumSW,lumSE)));
    float lumMax=max(lumM,max(max(lumNW,lumNE),max(lumSW,lumSE)));
    vec2 dir = vec2(-(lumNW+lumNE)+(lumSW+lumSE), (lumNW+lumSW)-(lumNE+lumSE));
    float dirReduce=max((lumNW+lumNE+lumSW+lumSE)*0.03125, 0.0078125);
    float rcpDirMin=1.0/(min(abs(dir.x),abs(dir.y))+dirReduce);
    dir=clamp(dir*rcpDirMin,vec2(-8),vec2(8))*uTexelSize;
    vec3 rgbA=0.5*(texture(uScene,vUV+dir*(-0.166667)).rgb+texture(uScene,vUV+dir*0.166667).rgb);
    vec3 rgbB=rgbA*0.5+0.25*(texture(uScene,vUV+dir*-0.5).rgb+texture(uScene,vUV+dir*0.5).rgb);
    float lumB=dot(rgbB,luma);
    FragColor=vec4((lumB<lumMin||lumB>lumMax)?rgbA:rgbB,1.0);
}

#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
uniform float iTime;
out vec2 UV;
void main(){
    UV = vUV;
    float displacement = sin(iTime);
    gl_Position = vec4(((vPos.x+displacement)/2),(vPos.yz/2),1.0);
}
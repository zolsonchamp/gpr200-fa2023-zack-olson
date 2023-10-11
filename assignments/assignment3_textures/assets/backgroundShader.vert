#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;

uniform float iTime;
out vec2 UV;

void main() {
    gl_Position = vec4(vPos * iTime, 1.0);
    UV = vUV;
}
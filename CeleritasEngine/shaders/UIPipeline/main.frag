#version 450

layout(location = 0) in vec4 color;
layout(location = 1) in vec3 vPos;

layout(location = 0) out vec4 FragColor;

void main() {
    FragColor = color;
}

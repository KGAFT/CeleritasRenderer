#version 450

layout(location = 0) in vec4 color;
layout(location = 1) in vec3 vPos;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 vertexPos;

void main() {
    vertexPos = vec4(vPos, 1.0);
    FragColor = color;
}

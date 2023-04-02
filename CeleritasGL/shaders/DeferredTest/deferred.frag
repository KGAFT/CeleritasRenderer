#version 330 core

in vec4 readyColor;

layout(location = 0) out vec4 firstColor;
layout(location = 1) out vec4 invertedColor;

void main() {
    firstColor = readyColor;
    vec4 tempInvers = vec4(1.0-readyColor.x, 1.0-readyColor.y, 1.0f-readyColor.z, readyColor.w);
    invertedColor = tempInvers;
}

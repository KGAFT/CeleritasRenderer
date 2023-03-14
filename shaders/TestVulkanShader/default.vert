#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 readyColor;

layout(push_constant) uniform WorldTransformData{
    mat4 viewMatrix;
    mat4 worldMatrix;
    vec3 cameraPosition;
} worldTransformData;

layout(std140, binding = 0) uniform testUBO{
    vec3 data;
} data;

void main(){
    readyColor = color;
    vec3 vector = worldTransformData.cameraPosition;
    vec3 temp = data.data;
    gl_Position = vec4(position, 1.0f);
}
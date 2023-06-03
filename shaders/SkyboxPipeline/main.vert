#version 450

layout(location = 0) in vec3 position;


layout(push_constant) uniform SkyboxConfig{
    mat4 cameraMatrix;
} skyboxConfig;

layout(location = 0) out vec3 texCoords;


void main() {
    texCoords = position;
    vec4 outPosition = skyboxConfig.cameraMatrix*vec4(position, 1.0f);
    position.y*=-1;
    gl_Position = outPosition.xyww;
}

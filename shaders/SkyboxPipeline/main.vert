#version 450

layout(location = 0) in vec3 position;


layout(push_constant) uniform SkyboxConfig{
    mat4 cameraMatrix;
} skyboxConfig;

layout(location = 0) out vec3 texCoords;


void main() {
    texCoords = vec3(position.x,position.y,position.z);
    vec4 outPosition = skyboxConfig.cameraMatrix*vec4(position, 0.0f);
    gl_Position = outPosition.xyzz;
}

#version 450

layout(location = 0) in vec3 texCoords;

layout (set = 0, binding = 6) uniform samplerCube cubemap;

layout(location = 1) out vec4 FragColor;
void main() {
    FragColor = texture(cubemap, texCoords);
}

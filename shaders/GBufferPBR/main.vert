#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec3 normals;

layout(location = 0) out vec3 readyPosition;
layout(location = 1) out vec2 readyTextureCoordinates;
layout(location = 2) out vec3 readyNormals;
layout(push_constant) uniform WorldTransformData{
    mat4 viewMatrix;
    mat4 worldMatrix;
    vec3 cameraPosition;
} worldTransForm;

vec2 fixVectorPositioning(vec2 base){
    base.y*=-1;
    return base;
}

vec3 fixVectorPositioning(vec3 base){
    base.y*=-1;
    return base;
}

vec4 fixVectorPositioning(vec4 base){
    base.y*=-1;
    return base;
}

void main() {
    readyPosition = worldTransformData.worldMatrix*vec4(position, 1.0).rgb;
    mat3 normalMatrix = transpose(inverse(mat3(worldTransform.worldMatrix)));
    readyNormals = normalMatrix * normals;
    gl_Position = fixVectorPositioning(worldTransformData.viewMatrix*worldTransformData.worldMatrix*vec4(position, 1.0));
}

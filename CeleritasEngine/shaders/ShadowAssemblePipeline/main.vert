#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec3 normals;

layout(push_constant) uniform WorldTransformData{
    mat4 lightSpaceMatrix;
    mat4 worldMatrix;
    mat4 viewMatrix;
    vec3 cameraPosition;
} worldTransformData;

layout(location = 0) out vec4 fragmentPositionLightSpace;
layout(location = 1) out vec3 fragmentPosition;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec3 cameraPosition;
layout(location = 4) out vec2 uv;

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
    fragmentPosition = vec3(worldTransformData.worldMatrix * vec4(position, 1.0));
    fragmentPositionLightSpace = worldTransformData.lightSpaceMatrix * vec4(fragmentPosition, 1.0);

    normal = (vec4(normals, 0.0f)*worldTransformData.worldMatrix).xyz;
    uv = textureCoordinates;
    cameraPosition = worldTransformData.cameraPosition;
    gl_Position = fixVectorPositioning(worldTransformData.viewMatrix*worldTransformData.worldMatrix*vec4(position, 1.0));
}

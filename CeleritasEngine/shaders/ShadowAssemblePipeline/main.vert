#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec3 normals;

layout(push_constant) uniform WorldTransformData{
    mat4 lightSpaceMatrix;
    mat4 worldMatrix;
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

vec3 getNormalFromMap(vec2 uvsCoords, vec3 normals, vec3 fragmentPosition)
{
    vec4 baseNormal = texture(normalMap, uvsCoords);
    vec3 tangentNormal = baseNormal.xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fragmentPosition);
    vec3 Q2  = dFdy(fragmentPosition);
    vec2 st1 = dFdx(uvsCoords);
    vec2 st2 = dFdy(uvsCoords);
    vec3 N   = normalize(normals);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    return normalize(TBN * tangentNormal);
}

void main() {
    fragmentPosition = vec3(worldTransformData.worldMatrix * vec4(position, 1.0));
    fragmentPositionLightSpace = worldTransformData.lightSpaceMatrix * vec4(fragmentPosition, 1.0);

    normal = (vec4(normals, 0.0f)*worldTransformData.worldMatrix).xyz;
    uv = textureCoordinates;
    cameraPosition = worldTransformData.cameraPosition;
    gl_Position = fixVectorPositioning(worldTransformData.viewMatrix*worldTransformData.worldMatrix*vec4(position, 1.0));
}

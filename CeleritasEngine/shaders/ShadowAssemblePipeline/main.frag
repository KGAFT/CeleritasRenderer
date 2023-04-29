#version 450

layout(location = 0) in vec4 fragmentPositionLightSpace;
layout(location = 1) in vec3 fragmentPosition;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 cameraPosition;
layout(location = 4) in vec2 uv;
layout(std140, binding = 0) uniform ShadowAssemblerConfig{
    vec3 lightPosition;
    int normalMapEnabled;
    int previousAOEnabled;
} config;

layout(set = 0, binding = 1) uniform sampler2D shadowMap;
layout(set = 0, binding = 2) uniform sampler2D previousAO;
layout(set = 0, binding = 3) uniform sampler2D normalMap;

layout(location = 0) out vec4 result;


float calculateShadowFirstMethod(vec4 fragPosLightSpace, vec3 processedNormal)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    vec3 normal = processedNormal;
    vec3 lightDir = normalize(config.lightPosition - fragmentPosition);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0)
    shadow = 0.0;

    return shadow;
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
    float previousShadow = 1;
    vec3 cNormal = normalize(normal);
    if(config.previousAOEnabled==1){
        previousShadow = texture(previousAO, uv).r;
    }
    if(config.normalMapEnabled==1){
        cNormal = normalize(getNormalFromMap(uv, normal, fragmentPosition));
    }

    previousShadow-=(calculateShadowFirstMethod(fragmentPositionLightSpace, cNormal));
    if(previousShadow<0){
        previousShadow = 0;
    }
    result = vec4(previousShadow, previousShadow, previousShadow, 1.0);
}

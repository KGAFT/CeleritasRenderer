#version 450


layout(location = 0) in vec3 fragmentPosition;
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec3 normals;

layout(set = 0, binding = 1) uniform sampler2D albedoMap;
layout(set = 0, binding = 2) uniform sampler2D normalMap;
layout(set = 0, binding = 3) uniform sampler2D metallicMap;
layout(set = 0, binding = 4) uniform sampler2D roughnessMap;
layout(set = 0, binding = 5) uniform sampler2D aoMap;
layout(set = 0, binding = 6) uniform sampler2D emissiveMap;
layout(set = 0, binding = 7) uniform sampler2D metallicRoughness;
layout(set = 0, binding = 8) uniform sampler2D opacityMap;

layout(location = 0) out vec4 position;
layout(location = 1) out vec4 albedo;
layout(location = 2) out vec4 normal;
layout(location = 3) out vec4 metallicRouhgnessEmissiveINVAO;

layout(std140, binding = 0) uniform GBufferConfig{
    int combinedMetallicRoughness = 0;
    int opacityMapEnabled = 0;
    int emissiveEnabled = 0;
} config;


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

float getEmissivePower(vec2 UvsCoords, vec4 albedoColor){
    vec4 emissive = texture(emissiveMap, UvsCoords);
    float numerator = emissive.r+emissive.g+emissive.b+emissive.a;
    float denominator = albedoColor.r+albedoColor.g+albedoColor.b+albedoColor.a;
    return numerator/denominator;
}

void main() {
    position = vec4(fragmentPosition, 1.0f);
    albedo = texture(albedoMap, textureCoordinates);
    if(config.opacityMapEnabled==1){
        albedo.a = texture(opacityMap, textureCoordinates).r;
    }
    normal = vec4(getNormalFromMap(textureCoordinates, normals, fragmentPosition), 1.0);
    float emissive = 0;
    if (config.emissiveEnabled == 1){
        emissive = getEmissivePower(textureCoordinates, albedo);
    }
    float metallic = 0;
    float roughness = 0;
    if (config.combinedMetallicRoughness==1){
        vec4 mr = texture(metallicRoughness, textureCoordinates);
        metallc = mr.r;
        roughness = mr.g;
    }
    else {
        metallic = texture(metallicMap, textureCoordinates).r;
        roughness  = texture(roughnessMap, textureCoordinates).r;
    }
    metallicRouhgnessEmissiveINVAO.r = metallic;
    metallicRouhgnessEmissiveINVAO.g = roughness;
    metallicRouhgnessEmissiveINVAO.b = emissive;
    metallicRouhgnessEmissiveINVAO.a = 1.0-texture(aoMap, textureCoordinates).r;
}

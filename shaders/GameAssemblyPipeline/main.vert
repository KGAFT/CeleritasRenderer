#version 450


layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 readyUv;
layout(location = 1) out vec3 cameraPosition;

layout(push_constant) uniform CameraPosData{
    vec3 cameraPosition;
} camPosData;

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
    cameraPosition = camPosData.cameraPosition;
    readyUv = uv;
    gl_Position = vec4(fixVectorPositioning(position), 1.0f);
}

#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec3 vPos;

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
    outColor = color;
    vPos = fixVectorPositioning(position);
    gl_Position = vec4(fixVectorPositioning(position), 1.0);
}
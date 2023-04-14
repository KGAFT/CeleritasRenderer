#version 450

layout(location = 0) in vec3 position;

layout(push_constant) uniform LightViewData{
    mat4 lightSpaceMatrix;
    mat4 worldMatrix;
} viewData;

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
    gl_Position = fixVectorPositioning(viewData.lightSpaceMatrix*viewData.worldMatrix*vec4(position, 1.0));
}

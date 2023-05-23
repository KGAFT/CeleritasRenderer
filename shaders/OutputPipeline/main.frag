#version 450

layout(location = 0) in vec2 uvs;

layout(set = 0, binding = 1) uniform sampler2D UiLayer;
layout(set = 0, binding = 2) uniform sampler2D GameLayer;

layout(location = 0) out vec4 FragColor;

layout(push_constant) uniform ColorCorrect{
    int mode;
} correct;

void main() {
    if(correct.mode==1){
        FragColor = texture(GameLayer, uvs);
    }
    else{
        FragColor = texture(UiLayer, uvs);
    }
}

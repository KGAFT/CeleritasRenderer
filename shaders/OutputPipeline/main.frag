#version 450

layout(location = 0) in vec2 uvs;

layout(set = 0, binding = 1) uniform sampler2D UiLayer;
layout(set = 0, binding = 2) uniform sampler2D GameLayer;

layout(location = 0) out vec4 FragColor;

layout(std140, binding = 0) uniform ColorCorrect{
    vec4 uiCorrectAmplifier;
    vec4 gameCorrectAmplifier;

    vec4 uiCorrectAdder;
    vec4 gameCorrectAdder;

    vec4 outCorrectAmplifier;
    vec4 outCorrectAdder;
} correct;

void main() {
    //vec4 gameColor = texture(GameLayer, uvs)*correct.gameCorrectAmplifier+correct.gameCorrectAdder;
    //vec4 uiColor = texture(UiLayer, uvs)*correct.uiCorrectAmplifier+correct.uiCorrectAdder;
    FragColor = texture(GameLayer, uvs)+texture(UiLayer, uvs);
}

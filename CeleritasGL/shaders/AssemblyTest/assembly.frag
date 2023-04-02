#version 330 core

in vec2 readyUv;

uniform sampler2D firstColorSampler;


out vec4 color;
void main() {
   color = texture(firstColorSampler, readyUv);

}

#version 330 core

in vec3 myColor;
in vec2 UV;
out vec4 FragColor;


uniform sampler2D tex_1;
uniform vec3 addColor;

void main() {
    FragColor = texture(tex_1, UV*2) * vec4(myColor + addColor, 1.0);
}
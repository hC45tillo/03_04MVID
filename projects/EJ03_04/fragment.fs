#version 330 core

in vec3 myColor;
out vec4 FragColor;

in vec3 aPosf;

uniform vec3 addColor;

void main() {
    FragColor = vec4(myColor + addColor, 1.0);
    gl_Position = vec4(aPosf, 1.0);
}
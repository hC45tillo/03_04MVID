#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aColor;

out vec3 myColor;
uniform vec3 moveObj;
void main() {
    myColor = aColor;
    gl_Position = vec4(aPos + moveObj, 1.0);
}
#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;

uniform vec3 objectColor;
uniform vec3 lightColor;

uniform float ambientStrength;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform int shininess;
uniform float specularStrength;

uniform mat4 view;

void main() {
    vec3 lightPosVista = vec3(view * vec4(lightPos, 1.0));

    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPosVista - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(-fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = spec * specularStrength * lightColor;

    vec3 phong = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(phong, 1.0f);
}
#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int shininess;
    float alpha;
};
uniform Material material;

struct Light {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

uniform vec3 viewPos;

void main() {
    vec3 ambient = material.ambient * light.ambient;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * light.diffuse;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * light.specular;

    vec3 phong = ambient + diffuse + specular;
    FragColor = vec4(phong, material.alpha);
} 

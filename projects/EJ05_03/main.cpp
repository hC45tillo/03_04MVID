#include <glad/glad.h>

#include "engine/window.hpp"
#include "engine/shader.hpp"

#include "engine/texture.hpp"
#include "engine/geometry/cube.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

void handleInput() {/*Intentionally Left BLank*/ }

void render(const Geometry& geom, const Shader& shader, Texture& tex) {
    glClear(GL_COLOR_BUFFER_BIT);
	// Al jugar con los diferentes valores dentro del rotate y del translate podemos modificar las ubicaciones donde este 
	// estara girando, o el angulo, invertir los valores z de translate provocan una desaparicion en la escena mientras que los valores
	// en x y y haran que se cambie la posicion en la que se encuentra 
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(90.0f), glm::vec3(-1.0f, 1.0f, 3.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(-1.0f, 1.0f, -5.0f));

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    shader.use();

    tex.use(shader, "tex", 0);

    shader.set("model", model);
    shader.set("view", view);
    shader.set("proj", proj);  //TODO const mat4

    geom.render();
}

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    const Shader shader("../projects/EJ05_03/vertex.vs", "../projects/EJ05_03/fragment.fs");
    const Cube cube(1.0f);

    Texture tex("../assets/textures/question.jpg", Texture::Format::RGB);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    while (window->alive()) {
        handleInput();
        render(cube, shader, tex);
        window->frame();
    }

    return 0;
}
#include <glad/glad.h>

#include "engine/window.hpp"
#include "engine/shader.hpp"

#include "engine/texture.hpp"

#include "engine/geometry/cube.hpp"
#include "engine/geometry/quad.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

glm::vec3 cubePositions[] = {
	glm::vec3(1.0f, 0.0f, -3.0f),
	glm::vec3(-1.0f, 0.0f, -3.0f),
	glm::vec3(-1.5f, 1.0f, -2.0f),
	glm::vec3(2.0f, 0.5f, -1.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
};

void handleInput() {/*Intentionally Left BLank*/ }

void render(const Geometry& floor, const Geometry& geom, const Shader& shader, Texture& tex, Texture& cube_tex) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(85.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, -1.0f, -4.0f));

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    shader.use();

    tex.use(shader, "tex", 0);

    shader.set("model", model);
    shader.set("view", view);
    shader.set("proj", proj);  //TODO const mat4

    floor.render();
	for (const auto& i : cubePositions) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, i);
		cube_tex.use(shader, "tex", 1);
		shader.set("model", model);
		geom.render();
	}
}

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    const Shader shader("../projects/EJ05_04/vertex.vs", "../projects/EJ05_04/fragment.fs");
    const Cube cube(0.5f);
	const Quad quad(3.0f);
    Texture floor_tex("../assets/textures/bricks_specular.png", Texture::Format::RGB);
	Texture cube_tex("../assets/textures/question.jpg", Texture::Format::RGB);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    while (window->alive()) {
        handleInput();
        render(quad, cube, shader, floor_tex, cube_tex);
        window->frame();
    }

    return 0;
}
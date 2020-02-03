#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/camera.hpp"
#include "engine/geometry/cube.hpp"
#include "engine/input.hpp"
#include "engine/shader.hpp"
#include "engine/texture.hpp"
#include "engine/window.hpp"
#include "engine/geometry/sphere.hpp"
#include <engine\geometry\quad.hpp>
#include <iostream>

glm::vec3 cubePositions[] = {
	glm::vec3(1.0f, 0.0f, -3.0f),
	glm::vec3(-1.0f, 0.0f, -3.0f),
	glm::vec3(-2.5f, 1.0f, -2.0f),
	glm::vec3(2.0f, 0.5f, -1.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
};

Camera camera(glm::vec3(0.0f, 2.0f, 3.0f));
float lastFrame = 0.0f;
float lastX, lastY;
bool firstMouse = true;

void handleInput(float dt) {
    Input* input = Input::instance();

    if (input->isKeyPressed(GLFW_KEY_W)) {
        camera.handleKeyboardFPS(Camera::Movement::Forward, dt);
    }
    if (input->isKeyPressed(GLFW_KEY_S)) {
        camera.handleKeyboardFPS(Camera::Movement::Backward, dt);
    }
    if (input->isKeyPressed(GLFW_KEY_A)) {
        camera.handleKeyboardFPS(Camera::Movement::Left, dt);
    }
    if (input->isKeyPressed(GLFW_KEY_D)) {
        camera.handleKeyboardFPS(Camera::Movement::Right, dt);
    }
}

void onKeyPress(int key, int action) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        Window::instance()->setCaptureMode(true);
    }

    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        Window::instance()->setCaptureMode(false);
    }
}

void onMouseMoved(float x, float y) {
    if (firstMouse) {
        firstMouse = false;
        lastX = x;
        lastY = y;
    }

    const float xoffset = x - lastX;
    const float yoffset = lastY - y;
    lastX = x;
    lastY = y;

    camera.handleMouseMovement(xoffset, yoffset);
}

void onScrollMoved(float x, float y) {
    camera.handleMouseScroll(y);
}

void render(const Geometry& floor, const Geometry& geom, const Shader& shader, Texture& tex, Texture& cube_tex) {
    glClear(GL_COLOR_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(85.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, -1.0f, -4.0f));
	
    glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 100.0f);

    shader.use();

    tex.use(shader, "tex", 0);

    shader.set("model", model);
    shader.set("view", camera.getViewMatrix());
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

    const Shader shader("../projects/AG06/vertex.vs", "../projects/AG06/fragment.fs");
    
	const Cube cube(1.0f);
	const Quad quad(20.0f);
	
	Texture floor_tex("../assets/textures/bricks_specular.png", Texture::Format::RGB);
    Texture cube_tex("../assets/textures/question.jpg", Texture::Format::RGB);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Input::instance()->setKeyPressedCallback(onKeyPress);
    Input::instance()->setMouseMoveCallback(onMouseMoved);
    Input::instance()->setScrollMoveCallback(onScrollMoved);

    while (window->alive()) {
        const float currentFrame = glfwGetTime();
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
		
        handleInput(deltaTime);
        render(quad, cube, shader, floor_tex, cube_tex);
        window->frame();
    }

    return 0;
}
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
#include "engine/geometry/quad.hpp"
#include <iostream>
#include <engine\model.hpp>
#include <engine\player.hpp>
#include <engine\lavaBall.hpp>

Camera camera(glm::vec3(0.0f, 0.0f, 30.0f));
glm::vec3 lightPos(1.2f, 3.0f, 10.0f);

glm::vec3 playerPos(0.0f, 0.0f, 3.0f);

const uint32_t k_shadow_height = 1024;
const uint32_t k_shadow_width = 1024;
const float k_shadow_near = 1.0f;
const float k_shadow_far = 7.5f;

float lastFrame = 0.0f;
float lastX, lastY;
bool firstMouse = true;

glm::vec3 _front, _right;
glm::vec3 _up, _worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

/*void updatePlayerVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(-90.0f)) * cos(glm::radians(0.0f));
	front.y = sin(glm::radians(0.0f));
	front.z = sin(glm::radians(-90.0f)) * cos(glm::radians(0.0f));
	_front = glm::normalize(front);

	_right = glm::normalize(glm::cross(_front, _worldUp));
	_up = glm::normalize(glm::cross(_right, _front));
}*/

void handleInput(float dt, Player& xwing) {
	Input* input = Input::instance();
	
	camera.handleKeyboard(Camera::Movement::Up, dt);

	if (input->isKeyPressed(GLFW_KEY_W)) {
		//camera.handleKeyboard(Camera::Movement::Up, dt);
		//playerPos.y += dt * 3.5f;
		xwing.up(dt);
	}
	if (input->isKeyPressed(GLFW_KEY_S)) {
		//camera.handleKeyboard(Camera::Movement::Down, dt);
		xwing.down(dt);
	}
	if (input->isKeyPressed(GLFW_KEY_A)) {
		xwing.left(dt);
		
	}
	if (input->isKeyPressed(GLFW_KEY_D)) {
		xwing.right(dt);
	}
}

/*void handleInput(float dt) {
	Input* input = Input::instance();
	//updatePlayerVectors();
	//const float velocity = 2.5f * dt;
	if (input->isKeyPressed(GLFW_KEY_W) || input->isKeyPressed(GLFW_KEY_UP)) {
		camera.handleKeyboard(Camera::Movement::Up, dt);
		//lightPos = camera.getPosition();
		//playerPos += _up * velocity;
	}
	if (input->isKeyPressed(GLFW_KEY_S) || input->isKeyPressed(GLFW_KEY_DOWN)) {
		camera.handleKeyboard(Camera::Movement::Down, dt);
		//playerPos -= _up * velocity;
	}
	if (input->isKeyPressed(GLFW_KEY_A) || input->isKeyPressed(GLFW_KEY_LEFT)) {
		camera.handleKeyboard(Camera::Movement::Left, dt);
		//playerPos -= _right * velocity;
	}
	if (input->isKeyPressed(GLFW_KEY_D) || input->isKeyPressed(GLFW_KEY_RIGHT)) {
		camera.handleKeyboard(Camera::Movement::Right, dt);
		//playerPos += _right * velocity;
	}
}*/

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

std::pair<uint32_t, uint32_t> createFBO() {
	uint32_t fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	uint32_t depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, k_shadow_width, k_shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[]{ 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer Incomplete" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_pair(fbo, depthMap);
}

void renderScene(const Shader& shader, const Model& xwing, const Geometry& quad, const Geometry& cube, const Geometry& sphere,
	const Texture& t_albedo, const Texture& t_specular, const Texture& floor) {
	floor.use(shader, "material.diffuse", 0);
	floor.use(shader, "material.specular", 1);

	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
	//model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	shader.set("model", model);
	glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader.set("normalMat", normalMat);
	quad.render();

	t_albedo.use(shader, "material.diffuse", 0);
	t_specular.use(shader, "material.specular", 1);

	model = glm::mat4(1.0f);
	//model = glm::translate(model, playerPos);
	model = glm::translate(model, playerPos);
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(.5f, .5f, .5f));
	shader.set("model", model);
	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader.set("normalMat", normalMat);
	xwing.render(shader);
	
	/*model = glm::mat4(1.0);
	shader.set("model", model);
	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader.set("normalMat", normalMat);
	cube.render();*/

	/*model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(4.0f, 4.0f, 3.0f));
	shader.set("model", model);
	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader.set("normalMat", normalMat);
	sphere.render();*/

	/*model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(-6.0f, -2.0f, 3.0f));
	shader.set("model", model);
	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader.set("normalMat", normalMat);
	sphere.render();*/
}

void renderScene(Player xwing, LavaBall ball, const Shader& shader, const Geometry& land, const Model& player, const Texture& t_landA, const Texture& t_landB, const Texture& t_landC, const Texture& t_player) {
	t_landA.use(shader, "material.diffuse", 0);
	t_landA.use(shader, "material.specular", 1);

	//Begin Land Rendering
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
	shader.set("model", model);
	glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader.set("normalMat", normalMat);
	land.render();

	t_landB.use(shader, "material.diffuse", 0);
	t_landB.use(shader, "material.specular", 1);
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	shader.set("model", model);
	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader.set("normalMat", normalMat);
	land.render();

	t_landC.use(shader, "material.diffuse", 0);
	t_landC.use(shader, "material.specular", 1);
	model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
	shader.set("model", model);
	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader.set("normalMat", normalMat);
	land.render();
	//End Land Rendering

	//Player Rendering
	t_player.use(shader, "material.diffuse", 0);
	t_player.use(shader, "material.specular", 1);

	model = glm::mat4(1.0f);
	model = glm::translate(model, xwing.position);
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(.5f, .5f, .5f));
	shader.set("model", model);
	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader.set("normalMat", normalMat);
	player.render(shader);
	//End player Rendering

}

void render(Player xwing, LavaBall ball, const Geometry& land, const Model& player, const Shader& s_phong, const Shader& s_depth, const Shader& s_debug, const Shader& s_light,
	const Texture& t_landA, const Texture& t_landB, const Texture& t_landC, const Texture& t_player, const uint32_t fbo, const uint32_t fbo_texture) {
	//FIRST PASS
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, k_shadow_width, k_shadow_height);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	const glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, k_shadow_near, k_shadow_far);
	const glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	s_depth.use();
	s_depth.set("lightSpaceMatrix", lightSpaceMatrix);
	//glCullFace(GL_FRONT);
	renderScene(xwing, ball, s_depth, land, player, t_landA, t_landB, t_landC, t_player);
	
	//glCullFace(GL_BACK);

	//SECOND PASS
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), static_cast<float>(Window::instance()->getWidth()) / Window::instance()->getHeight(), 0.1f, 100.0f);

	s_phong.use();

	s_phong.set("view", view);
	s_phong.set("proj", proj);

	s_phong.set("viewPos", camera.getPosition());

	s_phong.set("light.position", lightPos);
	s_phong.set("light.ambient", 0.1f, 0.1f, 0.1f);
	s_phong.set("light.diffuse", 0.5f, 0.5f, 0.5f);
	s_phong.set("light.specular", 1.0f, 1.0f, 1.0f);

	s_phong.set("material.shininess", 32);

	s_phong.set("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	s_phong.set("depthMap", 2);

	renderScene(xwing, ball, s_phong, land, player, t_landA, t_landB, t_landC, t_player);
}

/*void render(const Model& xwing, const Geometry& quad, const Geometry& cube, const Geometry& sphere,
	const Shader& s_phong, const Shader& s_depth, const Shader& s_debug, const Shader& s_light,
	const Texture& t_albedo, const Texture& t_specular, const uint32_t fbo, const uint32_t fbo_texture, const Texture& floor) {

	//FIRST PASS
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, k_shadow_width, k_shadow_height);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	const glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, k_shadow_near, k_shadow_far);
	const glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	s_depth.use();
	s_depth.set("lightSpaceMatrix", lightSpaceMatrix);
	//glCullFace(GL_FRONT);
	renderScene(s_depth, xwing, quad, cube, sphere, t_albedo, t_specular, floor);
	//glCullFace(GL_BACK);

//SECOND PASS
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), static_cast<float>(Window::instance()->getWidth()) / Window::instance()->getHeight(), 0.1f, 100.0f);

	s_phong.use();

	s_phong.set("view", view);
	s_phong.set("proj", proj);

	s_phong.set("viewPos", camera.getPosition());

	s_phong.set("light.position", lightPos);
	s_phong.set("light.ambient", 0.1f, 0.1f, 0.1f);
	s_phong.set("light.diffuse", 0.5f, 0.5f, 0.5f);
	s_phong.set("light.specular", 1.0f, 1.0f, 1.0f);

	s_phong.set("material.shininess", 32);

	s_phong.set("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	s_phong.set("depthMap", 2);
	
	renderScene(s_phong, xwing, quad, cube, sphere, t_albedo, t_specular, floor);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
	//glClear(GL_COLOR_BUFFER_BIT);
	//glDisable(GL_DEPTH_TEST);

	//s_debug.use();
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, fbo_texture);
	//s_debug.set("depthMap", 0);

	//quad.render();
}
*/
int main(int, char* []) {
	Window* window = Window::instance();
	glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

	// Shaders
	const Shader s_phong("../projects/FINAL/phong.vs", "../projects/FINAL/blinn.fs");
	const Shader s_depth("../projects/FINAL/depth.vs", "../projects/FINAL/depth.fs");
	const Shader s_debug("../projects/FINAL/debug.vs", "../projects/FINAL/debug.fs");
	const Shader s_light("../projects/FINAL/light.vs", "../projects/FINAL/light.fs");

	// Textures
	const Texture t_landA("../assets/textures/mustafar4.png", Texture::Format::RGB);
	const Texture t_landB("../assets/textures/mustafar2.jpg", Texture::Format::RGB);
	const Texture t_landC("../assets/textures/mustafar4.png", Texture::Format::RGB);
	const Texture t_player("../assets/textures/xwing.jpg", Texture::Format::RGB);
	
	//Objects
	const Quad land(5.0f);
	const Model player("../assets/models/xwing18/source/xwing_tiefighter_sketchfab.obj");

	Player xwing = Player();
	xwing.position = glm::vec3(0.0f, 0.0f, 3.0f);

	LavaBall ball = LavaBall();
	ball.position = glm::vec3(5.0f, 10.0f, 3.0f);
	auto fbo = createFBO();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Inpunt handling 
	Input::instance()->setKeyPressedCallback(onKeyPress);

	while (window->alive()) {
		const float currentFrame = glfwGetTime();
		const float deltaTime = currentFrame - lastFrame;
		
		lastFrame = currentFrame;
		//handleInput(deltaTime);
		handleInput(deltaTime, xwing);
		render(xwing, ball, land, player, s_phong, s_depth, s_debug, s_light, t_landA, t_landB, t_landC, t_player, fbo.first, fbo.second);
		window->frame();
	}

	glDeleteFramebuffers(1, &fbo.first);
	glDeleteTextures(1, &fbo.second);

	return 0;
}

/*int main(int, char* []) {
	Window* window = Window::instance();

	glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

	const Shader s_phong("../projects/FINAL/phong.vs", "../projects/FINAL/blinn.fs");
	const Shader s_depth("../projects/FINAL/depth.vs", "../projects/FINAL/depth.fs");
	const Shader s_debug("../projects/FINAL/debug.vs", "../projects/FINAL/debug.fs");
	const Shader s_light("../projects/FINAL/light.vs", "../projects/FINAL/light.fs");
	const Shader s_phong2("../projects/FINAL/simple.vs", "../projects/FINAL/simple.fs");
	
	const Texture t_albedo("../assets/textures/xwing.jpg", Texture::Format::RGB);
	const Texture t_specular("../assets/textures/xwing.jpg", Texture::Format::RGB);
	const Texture floor("../assets/textures/mustafar4.png", Texture::Format::RGB);
	
	const Model xwing("../assets/models/xwing18/source/xwing_tiefighter_sketchfab.obj");
	const Quad quad(3.0f);
	const Cube cube(1.0f);
	const Sphere sphere(1.0f, 25, 25);
	const Cube pj(2.0f);

	auto fbo = createFBO();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	Input::instance()->setKeyPressedCallback(onKeyPress);
	//Input::instance()->setMouseMoveCallback(onMouseMoved);
	//Input::instance()->setScrollMoveCallback(onScrollMoved);

	while (window->alive()) {
		
		const float currentFrame = glfwGetTime();
		const float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		render(xwing, quad, cube, sphere, s_phong, s_depth, s_debug, s_light, t_albedo, t_specular, fbo.first, fbo.second, floor);
		//render(xwing, s_phong);
		window->frame();
	}

	glDeleteFramebuffers(1, &fbo.first);
	glDeleteTextures(1, &fbo.second);

	return 0;
}*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>  //for std::vector
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
#include <engine\enemy.hpp>

Camera camera(glm::vec3(0.0f, 0.0f, 60.0f));
glm::vec3 lightPos(1.2f, 3.0f, 70.0f);

glm::vec3 playerPos(0.0f, 0.0f, 3.0f);

const uint32_t k_shadow_height = 1024;
const uint32_t k_shadow_width = 1024;
const float k_shadow_near = 1.0f;
const float k_shadow_far = 7.5f;

float lastFrame = 0.0f;
float lastX, lastY;
bool firstMouse = true;
int shootCount = 1;

Enemy sinEnemies[18];
Enemy deathstars[6];
std::vector<Enemy> shoots;

void handleInput(float dt, float lf, Player& xwing) {
	Input* input = Input::instance();
	
	camera.handleKeyboard(Camera::Movement::Up, dt);
	if (input->isKeyPressed(GLFW_KEY_W)) {
		//camera.handleKeyboard(Camera::Movement::Up, dt);
		//playerPos.y += dt * 3.5f;

		xwing.up(dt);
		lightPos.y += dt * 4.5f;
	}
	if (input->isKeyPressed(GLFW_KEY_S)) {
		//camera.handleKeyboard(Camera::Movement::Down, dt);
		xwing.down(dt);
		lightPos.y -= dt * 4.5f;
	}
	if (input->isKeyPressed(GLFW_KEY_A)) {
		xwing.left(dt);
		lightPos.x -= dt * 4.5f;
	}
	if (input->isKeyPressed(GLFW_KEY_D)) {
		xwing.right(dt);
		lightPos.x += dt * 4.5f;
	}
	if (input->isKeyPressed(GLFW_KEY_SPACE)) {
		std::cout << "shoot" << std::endl;
		shoots.push_back(Enemy(glm::vec3(xwing.position.x + 1.5, xwing.position.y + 0.5, xwing.position.z), 2.5f));
		shoots.push_back(Enemy(glm::vec3(xwing.position.x - 1.5, xwing.position.y + 0.5, xwing.position.z), 2.5f));
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

void renderScene(Player xwing,const Model& enemyAMod, const Geometry& ball, const Shader& shader, const Geometry& land, const Texture& t_landA, const Texture& t_landB, const Texture& t_landC, const Texture& t_player, const Texture& t_ball, const Texture& t_blast) {
	t_landA.use(shader, "material.diffuse", 0);
	t_landA.use(shader, "material.specular", 1);

	//Begin Land Rendering
	glm::mat4 model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(50.0f, 50.0f, 10.0f));
	shader.set("model", model);
	glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader.set("normalMat", normalMat);
	land.render();

	t_landB.use(shader, "material.diffuse", 0);
	t_landB.use(shader, "material.specular", 1);
	model = glm::translate(model, glm::vec3(0.0f, 100.0f, 0.0f));
	model = glm::scale(model, glm::vec3(50.0f, 50.0f, 10.0f));
	shader.set("model", model);
	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader.set("normalMat", normalMat);
	land.render();

	t_landC.use(shader, "material.diffuse", 0);
	t_landC.use(shader, "material.specular", 1);
	model = glm::translate(model, glm::vec3(0.0f, 150.0f, 0.0f));
	model = glm::scale(model, glm::vec3(50.0f, 50.0f, 10.0f));
	shader.set("model", model);
	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	shader.set("normalMat", normalMat);
	land.render();
	//End Land Rendering

	t_ball.use(shader, "material.diffuse", 0);
	t_ball.use(shader, "material.specular", 1);
	
	for (int i = 0; i < std::size(deathstars); i++) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, deathstars[i].position);
		model = glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(20.0f), deathstars[i].position);
		shader.set("model", model);
		normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
		shader.set("normalMat", normalMat);
		ball.render();
	}
	
	t_blast.use(shader, "material.diffuse", 1);
	t_blast.use(shader, "material.specular", 1);

	for (int i = 0; i < std::size(shoots); i++) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, shoots[i].position);
		model = glm::scale(model, glm::vec3(.08f, 1.0f, 3.0f));
		shader.set("model", model);
		normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
		shader.set("normalMat", normalMat);
		land.render();
	}

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
	xwing.model.render(shader);
	//End player Rendering

	//Enemy Rendering
	for (int i = 0; i < std::size(sinEnemies); i++) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, sinEnemies[i].position);
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(.1f, .1f, .1f));
		shader.set("model", model);
		normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
		shader.set("normalMat", normalMat);
		enemyAMod.render(shader);
	}
	
	//End Enemy Rendering
	


}

void render(Player xwing, const Model& enemyA, const Geometry& ball, const Geometry& land, const Shader& s_phong, const Shader& s_depth, const Shader& s_debug, const Shader& s_light,
	const Texture& t_landA, const Texture& t_landB, const Texture& t_landC, const Texture& t_player, const Texture& t_ball, const Texture& t_blast, const uint32_t fbo, const uint32_t fbo_texture) {
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
	renderScene(xwing, enemyA, ball,  s_depth, land, t_landA, t_landB, t_landC, t_player, t_ball, t_blast);
	
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

	s_phong.set("material.shininess", 128);

	s_phong.set("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, fbo_texture);
	s_phong.set("depthMap", 2);

	renderScene(xwing, enemyA, ball, s_phong, land, t_landA, t_landB, t_landC, t_player, t_ball, t_blast);
}

void updateWorld(float dt) {
	for (int i = 0; i < std::size(sinEnemies); i++) {
		sinEnemies[i].sinFalling(dt);
	}
	for (int i = 0; i < std::size(shoots); i++) {
		shoots[i].up(dt);
	}
}

void initElements() {

	//Enemies with sin movement
	sinEnemies[0] = Enemy(glm::vec3(10.0f, 7.0f, 3.0f));
	sinEnemies[1] = Enemy(glm::vec3(-10.0f, 7.0f, 3.0f));
	sinEnemies[2] = Enemy(glm::vec3(13.0f, 19.0f, 3.0f));
	sinEnemies[3] = Enemy(glm::vec3(-13.0f, 19.0f, 3.0f));
	sinEnemies[4] = Enemy(glm::vec3(10.0f, 31.0f, 3.0f));
	sinEnemies[5] = Enemy(glm::vec3(-10.0f, 31.0f, 3.0f));

	sinEnemies[6] = Enemy(glm::vec3(7.0f, 107.0f, 3.0f));
	sinEnemies[7] = Enemy(glm::vec3(-7.0f, 107.0f, 3.0f));
	sinEnemies[8] = Enemy(glm::vec3(10.0f, 119.0f, 3.0f));
	sinEnemies[9] = Enemy(glm::vec3(-10.0f, 119.0f, 3.0f));
	sinEnemies[10] = Enemy(glm::vec3(7.0f, 131.0f, 3.0f));
	sinEnemies[11] = Enemy(glm::vec3(-7.0f, 131.0f, 3.0f));

	sinEnemies[12] = Enemy(glm::vec3(7.0f, 207.0f, 3.0f));
	sinEnemies[13] = Enemy(glm::vec3(-7.0f, 207.0f, 3.0f));
	sinEnemies[14] = Enemy(glm::vec3(10.0f, 219.0f, 3.0f));
	sinEnemies[15] = Enemy(glm::vec3(-10.0f, 219.0f, 3.0f));
	sinEnemies[16] = Enemy(glm::vec3(7.0f, 231.0f, 3.0f));
	sinEnemies[17] = Enemy(glm::vec3(-7.0f, 231.0f, 3.0f));

	//deathstars
	deathstars[0] = Enemy(glm::vec3(15.0f, 50.0f, 5.0f));
	deathstars[1] = Enemy(glm::vec3(20.0f, 90.0f, 5.0f));
	deathstars[2] = Enemy(glm::vec3(-13.0f, 110.0f, 5.0f));
	deathstars[3] = Enemy(glm::vec3(12.0f, 210.0f, 5.0f));
	deathstars[4] = Enemy(glm::vec3(-25.0f, 60.0f, 5.0f));
	deathstars[5] = Enemy(glm::vec3(30.0f, 300.0f, 5.0f));
}

int main(int, char* []) {
	Window* window = Window::instance();
	glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

	// Shaders
	const Shader s_phong("../projects/FINAL/phong.vs", "../projects/FINAL/blinn.fs");
	const Shader s_depth("../projects/FINAL/depth.vs", "../projects/FINAL/depth.fs");
	const Shader s_debug("../projects/FINAL/debug.vs", "../projects/FINAL/debug.fs");
	const Shader s_light("../projects/FINAL/light.vs", "../projects/FINAL/light.fs");

	// Textures
	const Texture t_landA("../assets/textures/space.jpg", Texture::Format::RGB);
	const Texture t_landB("../assets/textures/space2.jpg", Texture::Format::RGB);
	const Texture t_landC("../assets/textures/space3.jpg", Texture::Format::RGB);
	const Texture t_player("../assets/textures/xwing.jpg", Texture::Format::RGB);
	const Texture t_ball("../assets/textures/deathstar.png", Texture::Format::RGB);
	const Texture t_blast("../assets/textures/green3.jpg", Texture::Format::RGB);
	
	//Objects
	const Quad land(5.0f);
	const Model enemyAMod("../assets/models/tie/TIE_Advanced_X2_MSDOS.obj");
	//Deathstar
	Sphere sphere(4.0f, 50, 50);
	//Player
	Player xwing = Player(glm::vec3(0.0f, 0.0f, 3.0f));
	//Enemy
	initElements();
	
	auto fbo = createFBO();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Inpunt handling 
	Input::instance()->setKeyPressedCallback(onKeyPress);

	while (window->alive()) {
		const float currentFrame = glfwGetTime();
		const float deltaTime = currentFrame - lastFrame;
		
		lastFrame = currentFrame;
		handleInput(deltaTime, lastFrame, xwing);
		updateWorld(deltaTime);
		render(xwing, enemyAMod, sphere, land, s_phong, s_depth, s_debug, s_light, t_landA, t_landB, t_landC, t_player, t_ball, t_blast, fbo.first, fbo.second);
		window->frame();
	}

	glDeleteFramebuffers(1, &fbo.first);
	glDeleteTextures(1, &fbo.second);

	return 0;
}
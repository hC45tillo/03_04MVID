//EJ02.01; - Pintar dos tri�ngulos, uno al lado del otro(sin formar un quad, de esta forma " /\/\ "), usando glDrawArrays, a�adiendo m�s v�rtices a los datos(usando un solo VBO).

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdint>
#include <engine\window.hpp>

void onChangeFrameBufferSize(GLFWwindow*, const int32_t width, const int32_t height) noexcept {
	glViewport(0, 0, width, height);
	std::cout << width << " " << height << std::endl;
}

void handleInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

bool checkShader(uint32_t shader) {
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cout << "Error Compiling Shader " << infoLog << std::endl;
		return false;
	}
	return true;
}

bool checkProgram(uint32_t program) {
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		std::cout << "Error Linking Program " << infoLog << std::endl;
		return false;
	}
	return true;
}

void render(uint32_t VAO, uint32_t program) {
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

uint32_t createProgram() {
	const char* vertexShaderSource = "#version 330 core\n"
		"layout (location=0) in vec3 aPos;\n"
		"void main() {\n"
		"    gl_Position = vec4(aPos, 1.0);\n"
		"}\0";
	const char* fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main() {\n"
		"    FragColor = vec4(0.03, 0.41, 0.04, 1.0);\n"
		"}\0";

	const uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	checkShader(vertexShader);

	const uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);
	checkShader(fragmentShader);

	const uint32_t program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	checkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

uint32_t createVertexData(uint32_t* VBO, uint32_t* EBO) {
	float vertices[] = {
		-0.5f,  0.5f, 0.0f, //top
		-0.9f, -0.5f, 0.0f, //bottom left
		-0.1f, -0.5f, 0.0f, //bottom right

		0.5f,  0.5f, 0.0f, //top
		0.1f, -0.5f, 0.0f, //bottom left
		0.9f, -0.5f, 0.0f, //bottom right
	};
	
	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return VAO;
}

int main(int, char* []) {
	Window* window = Window::instance();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	uint32_t VBO, EBO;
	const uint32_t VAO = createVertexData(&VBO, &EBO);
	const uint32_t program = createProgram();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	while (window->alive()) {
		render(VAO, program);
		window->frame();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteProgram(program);

	return 0;
}
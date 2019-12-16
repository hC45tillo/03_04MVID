//EJ02.06; - Pintar el mismo hexágono, pero generando los vértices de manera procedural (usando algún tipo de algoritmo), mediante coordenadas polares.
//EJ02.05; - Pintar un hexágono centrado en la pantalla, usando un VAO, VBO y EBO, generando los vértices de la manera habitual a mano.

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine/input.hpp"
#include "engine/window.hpp"

#include <iostream>

void handleInput() {
	std::vector<std::pair<int, int>> keys = Input::instance()->getKeys();
	for (auto& key : keys) {
		std::cout << key.first << " - " << key.second << std::endl;
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

uint32_t createVertexData(uint32_t* VBO, uint32_t* EBO, float radious, float p1x, float p1y, float p2x, float p2y) {

	float vertices[] = {
		0.0f, 0.0f, 0.0f,
		 p1x,  p1y, 0.0f,
		 p2x,  p2y, 0.0f,
	};
	//float vertices[] = {
	//	0.0f, 0.0f, 0.0f, //0							0x= radio * 0; y= radio * 0
	//	radious, 0.0f, 0.0f, //1						1x= radio * 1; y = radio * 0
	//	radious / 2, radious, 0.0f, //2					2x= radio * 1/2; y = radio * 1
	//	//radious * -1, 0.0f, 0.0f, //3					3x= radio * -1; y = radio * 0
	//	//radious /2 * -1, radious, 0.0f, //4				4x= radio - 1/2; y = radio * 1
	//	//radious / 2 * -1, radious * -1, 0.0f, //5		5x= radio * -1/2; y = radio * -1
	//	//radious / 2, radious * -1, 0.0f, //6				6x= radio * 1; y = radio * -1
	//};
	uint32_t indices[] = {
		0,1,2,
		/*0,2,4,
		0,4,3,
		0,3,5,
		0,5,6,
		0,6,1*/
	};



	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return VAO;
}


void render(uint32_t VAO, uint32_t program) {
	//glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, nullptr);
}

int main(int, char* []) {
	Window* window = Window::instance();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	uint32_t VBO, EBO, VAO;
	//const uint32_t VAO = createVertexData(&VBO, &EBO, 0.5f);
	const uint32_t program = createProgram();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	int contP1 = 0;
	int contP2 = 1;
	float p1x = 0.0f;
	float p1y = 0.0f;
	float p2x = 0.0f; 
	float p2y = 0.0f;
	float length1 = 1.0f;
	float length2 = 1.0f;
	float y1Const = 1.0f;
	float y2Const = 1.0f;
	float radious = 1.0f;
	while (window->alive()) {
		
		switch (contP1) {
		case 0:
			p1x = radious * length1;
			p1y = 0;
			contP1 = 1;
			break;
		case 1:
		case 2:
			p1x = radious * (length1 / 2);
			p1y = radious * y1Const;
			y1Const = contP1 == 2 ? y1Const * -1: y1Const;
			contP1 = contP1 == 2 ? 0 : 2;
			length1 = contP1 == 2.00f ? length1 * -1.00f : length1;
			break;
		}

		switch (contP2) {
		case 0:
			p2x = radious * length2;
			p2y = 0;
			contP2 = 1;
			break;
		case 1:
		case 2:
			p2x = radious * (length2 / 2);
			p2y = radious * y2Const;
			y2Const = contP2 == 2 ? y2Const * -1 : y2Const;
			contP2 = contP2 == 2 ? 0 : 2;
			length2 = contP2 == 2.00f ? length2 * -1.00f : length2;
			break;
		}
		VAO = createVertexData(&VBO, &EBO, radious, p1x, p1y, p2x, p2y);
		handleInput();
		render(VAO, program);
		window->frame();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteProgram(program);

	return 0;
}
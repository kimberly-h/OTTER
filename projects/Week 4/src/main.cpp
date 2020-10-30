#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream> //03
#include <string> //03
#include <GLM/glm.hpp> //04
#include <GLM/gtc/matrix_transform.hpp> //04

GLFWwindow* window;

bool initGLFW() {
	if (glfwInit() == GLFW_FALSE) {
		std::cout << "Failed to Initialize GLFW" << std::endl;
		return false;
	}

	//Create a new GLFW window
	window = glfwCreateWindow(800, 800, "Kimberly Hansuwan 100752265", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	return true;
}

bool initGLAD() {
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		std::cout << "Failed to initialize Glad" << std::endl;
		return false;
	}
}


GLuint shader_program;

bool loadShaders() {
	// Read Shaders from file
	std::string vert_shader_str;
	std::ifstream vs_stream("vertex_shader.glsl", std::ios::in);
	if (vs_stream.is_open()) {
		std::string Line = "";
		while (getline(vs_stream, Line))
			vert_shader_str += "\n" + Line;
		vs_stream.close();
	}
	else {
		printf("Could not open vertex shader!!\n");
		return false;
	}
	const char* vs_str = vert_shader_str.c_str();

	std::string frag_shader_str;
	std::ifstream fs_stream("frag_shader.glsl", std::ios::in);
	if (fs_stream.is_open()) {
		std::string Line = "";
		while (getline(fs_stream, Line))
			frag_shader_str += "\n" + Line;
		fs_stream.close();
	}
	else {
		printf("Could not open fragment shader!!\n");
		return false;
	}
	const char* fs_str = frag_shader_str.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_str, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_str, NULL);
	glCompileShader(fs);

	shader_program = glCreateProgram();
	glAttachShader(shader_program, fs);
	glAttachShader(shader_program, vs);
	glLinkProgram(shader_program);

	return true;
}

	//LECTURE 04
GLfloat rotY = 0.0f;
void keyboard()
{
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		rotY += 0.5;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rotY -= 0.5;
}


int main() {
	//Initialize GLFW
	if (!initGLFW())
		return 1;

	//Initialize GLAD
	if (!initGLAD())
		return 1;

	//// Lecture 3 starts here

	static const GLfloat points[] = {
		-0.5f, -0.5f, 0.5f,
		 0.5f, -0.5f, 0.5f,
		 0.0f,  0.5f, 0.5f
	};

	static const GLfloat colors[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	//VBO - Vertex buffer object
	GLuint pos_vbo = 0;
	glGenBuffers(1, &pos_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	
	GLuint color_vbo = 1;
	glGenBuffers(1, &color_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
	
	//						index, size, type, normalize?, stride, pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(0);//pos
	glEnableVertexAttribArray(1);//colors

	// Load our shaders

	if (!loadShaders())
		return 1;

	//////LECTURE 4 STARTS HERE
	
	//Projection Matrix: 45 Degrees FoV, ratio, range 0.1 - 100 units
	int width, height;
	glfwGetWindowSize(window, &width, &height); //this is going to be the dimention of the window

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f),
		(float)width / (float)height, 0.1f, 100.0f);

	// Camera
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 3), // Camera Position
		glm::vec3(0, 0, 0),	// Camera looks at the origin
		glm::vec3(0, 1, 0) // Up Vector
		);

	// Model Matrix
	glm::mat4 Model = glm::mat4(1.0f); // Resetting the Matrix

	Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));
	Model = glm::rotate(Model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, 1.0f));

		// T * R * S <-- from the right
	glm::mat4 mvp = Projection * View * Model;

	// Handler for our mvp matrix (uniform variable)
	GLuint MatrixID = glGetUniformLocation(shader_program, "MVP");



	// GL States
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	
	// Face Culling
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);		// Clockwise "CW" // Counter Clock Wise "CCW"
	glCullFace(GL_BACK);		//"Front" //"Back"
	

	// GL states
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	///// Game loop /////
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);
		
		//LECTURE 04
		Model = glm::mat4(1.0f);
		keyboard();
		Model = glm::translate(Model, glm::vec3(0.0f, 0.5f, 0.0f));
		Model = glm::rotate(Model, glm::radians(rotY), glm::vec3(0.0f, 0.0f, 1.0f)); //Radians number change speed
		Model = glm::translate(Model, glm::vec3(0.0f, -0.5f, 0.0f));
		mvp = Projection * View * Model;
		//rotY = 0.0; because we added the beginning portion

		// Send MVP Matrix to GPU
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
	}
	return 0;

}
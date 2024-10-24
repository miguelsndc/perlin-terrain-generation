#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "geometry.hpp"
#include "shader.hpp"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "camera.hpp"
#include "stb_image.hpp"

using glm::vec3, glm::mat4, std::vector;

void processInput(GLFWwindow* window, float dt);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void framebufferSizeCallback(GLFWwindow* window, int w, int h);

static void gatherComputeInfo() {
	int workGroupCount[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCount[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCount[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCount[2]);

	std::cout << "Max work groups per compute shader: " <<
		"(x: " << workGroupCount[0] << ", " <<
		"y: " << workGroupCount[1] << ", " <<
		"z: " << workGroupCount[2] << ")\n";

	int workGroupSize[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);

	std::cout << "Max work groups sizes: " <<
		"(x: " << workGroupSize[0] << ", " <<
		"y: " << workGroupSize[1] << ", " <<
		"z: " << workGroupSize[2] << ")\n";

	int workGroupInv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInv);
	std::cout << "Max invocations count per work group: " << workGroupInv << '\n';
}

int main(void) {
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Terrain", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	Camera camera;
	glfwSetWindowUserPointer(window, &camera);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Could not load GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	Shader base("./shaders/vertex_base.glsl", "./shaders/fragment_base.glsl");

	mat4 projection = mat4(1.0f), view = mat4(1.0f), model = mat4(1.0f);
	model = glm::rotate(model, glm::radians(110.0f), vec3(1.0f, 0.0f, 0.0f));
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.f);

	base.use();
	base.setMat4("projection", projection);
	base.setMat4("view", view);
	base.setMat4("model", model);

	//unsigned int VBO, VAO, EBO;
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);
	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(GLfloat), planeVertices.data(), GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeIndexBuffer.size() * sizeof(GLuint), planeIndexBuffer.data(), GL_STATIC_DRAW);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("./textures/amazon-heightmap.png", &width, &height, &nrChannels, 0);

	if (!data) {
		std::cout << "COULD NOT LOAD THE HEIGHTMAP: " << stbi_failure_reason() << std::endl;
		stbi_image_free(data);
		return -1;
	}

	std::vector<GLfloat> vertices;
	vertices.reserve(width * height * 3);
	float scale = 64.0f / 256.0f, yShift = -8.0f;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			unsigned char* texel = data + (j + width * i) * nrChannels;
			unsigned int y = texel[0];

			vertices.emplace_back(-height / 2.0f + i);
			vertices.emplace_back(y * scale + yShift);
			vertices.emplace_back(-width / 2.0f + j);
		}
	}
	stbi_image_free(data);

	std::vector<GLuint> indices;
	indices.reserve(width * height * 3);
	for (int i = 0; i < height - 1; i++) {
		for (int j = 0; j < width; j++) {
			for (int k = 0; k < 2; k++) {
				indices.emplace_back(j + width * (i + k));
			}
		}
	}

	const unsigned int NUM_STRIPS = height - 1;
	const unsigned int NUM_VERTS_PER_STRIP = width * 2;

	GLuint terrainVAO, terrainVBO, terrainEBO;
	glGenVertexArrays(1, &terrainVAO);
	glGenBuffers(1, &terrainVBO);
	glGenBuffers(1, &terrainEBO);

	glBindVertexArray(terrainVAO);

	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(float),       // size of vertices buffer
		&vertices[0],                          // pointer to first element
		GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int), // size of indices buffer
		&indices[0],                           // pointer to first element
		GL_STATIC_DRAW);

	gatherComputeInfo();

	// -----------

	float last = glfwGetTime();
	float dt = 0.00001;
	while (!glfwWindowShouldClose(window)) {
		float current = glfwGetTime();
		dt = current - last;
		last = current;

		processInput(window, dt);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		base.use();
		base.setMat4("view", camera.getViewMatrix());
		vec3 p = camera.position();
		std::cout << "Camera position: (" << p.x << ", " << p.y << ", " << p.z << ")\n";
		glBindVertexArray(terrainVAO);
		// render the mesh triangle strip by triangle strip - each row at a time
		for (unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
		{
			glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
				NUM_VERTS_PER_STRIP, // number of indices to render
				GL_UNSIGNED_INT,     // index data type
				(void*)(sizeof(unsigned int)
					* NUM_VERTS_PER_STRIP
					* strip)); // offset to starting index
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window, float dt) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		return;
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera->move(FORWARD, dt);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera->move(LEFT, dt);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera->move(BACKWARD, dt);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera->move(RIGHT, dt);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera->move(UP, dt);
	}
}

bool firstMouse = true;
float lastX = 0.0f;
float lastY = 0.0f;
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos; // reversed because y ranges frmo bottom to top
	lastX = xpos;
	lastY = ypos;

	camera->look(xOffset, yOffset, true);
}

void framebufferSizeCallback(GLFWwindow* window, int w, int h) {
	glViewport(0, 0, w, h);
}
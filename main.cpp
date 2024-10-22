#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "geometry.hpp"
#include "shader.hpp"

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

using glm::vec3, glm::mat4, std::vector;

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

int main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(800, 600, "Terrain", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Could not load GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	int div = 10;
	vector<GLfloat> planeVertices = plane(vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f),
		vec3(1.0f, 1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f), div);
	vector<GLuint> planeIndexBuffer = planeIndices(div);

	Shader base("./shaders/vertex_base.glsl", "./shaders/fragment_base.glsl");

	mat4 projection = mat4(1.0f), view = mat4(1.0f), model = mat4(1.0f);
	model = glm::rotate(model, glm::radians(110.0f), vec3(1.0f, 0.0f, 0.0f));
	view = glm::translate(view, vec3(0.0f, 0.0f, -5.0f));
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.f);

	base.use();
	base.setMat4("projection", projection);
	base.setMat4("view", view);
	base.setMat4("model", model);

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, planeVertices.size() * sizeof(GLfloat), planeVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, planeIndexBuffer.size() * sizeof(GLuint), planeIndexBuffer.data(), GL_STATIC_DRAW);

	gatherComputeInfo();

	// -----------
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		base.use();
		glBindVertexArray(VAO);
		//		glDrawArrays(GL_POINTS, 0, planeVertices.size() / 3);
		glDrawElements(GL_TRIANGLES, planeIndexBuffer.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
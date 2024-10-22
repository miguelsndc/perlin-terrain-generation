#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

using glm::vec3, std::vector;

vector<GLfloat> line(vec3 start, vec3 end, int div) {
	vec3 direction = (end - start) / (float)div;
	std::vector<GLfloat> vertices;
	vertices.reserve(div * 3);
	for (int i = 0; i <= div; i++) {
		vec3 current = start + direction * (float)i;

		vertices.emplace_back(current.x);
		vertices.emplace_back(current.y);
		vertices.emplace_back(current.z);
	}
	return vertices;
}

// 

vector<GLfloat> plane(vec3 v0, vec3 v1, vec3 v2, vec3 v3, int div) {
	vector<GLfloat> plane_vertices;
	plane_vertices.reserve(2 * div);// completely arbitrary

	/*  v3 --- v2
		|      |
		|      |
		v0 --- v1
	*/

	vec3 basis_x = (v3 - v0) / (float)div;
	vec3 basis_y = (v2 - v1) / (float)div;

	for (int i = 0; i <= div; i++) {
		vec3 start = v0 + basis_x * (float)i;
		vec3 end = v1 + basis_y * (float)i;
		vector<GLfloat> _line = line(start, end, div);
		for (GLfloat const v : _line) {
			plane_vertices.emplace_back(v);
		}
	}
	return plane_vertices;
}

vector<GLuint> planeIndices(int div) {
	/*  i + div +1 --- i + div + 1 + 1
		|   /        |
		| /          |
		i --------- i+1
	*/
	vector<GLuint> indices;
	indices.reserve(6 * div);

	for (int i = 0; i < div; i++) {
		for (int j = 0; j < div; j++) {
			int index = i * (div + 1) + j;
			// Top triangle
			indices.emplace_back(index);
			indices.emplace_back(index + div + 1 + 1);
			indices.emplace_back(index + div + 1);
			// Bottom Triangle
			indices.emplace_back(index);
			indices.emplace_back(index + 1);
			indices.emplace_back(index + div + 1 + 1);
		}
	}

	return indices;
}


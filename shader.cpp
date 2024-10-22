#include "shader.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

Shader::Shader(const std::string& vertexShaderPath, const std::string& pixelShaderPath)
	: m_program(0)
{
	std::string vertexCode, pixelCode;
	std::ifstream vertexShaderFile, pixelShaderFile;

	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	pixelShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vertexShaderFile.open(vertexShaderPath);
		pixelShaderFile.open(pixelShaderPath);
		std::stringstream vertexShaderStream, pixelShaderStream;
		vertexShaderStream << vertexShaderFile.rdbuf();
		pixelShaderStream << pixelShaderFile.rdbuf();
		vertexShaderFile.close();
		pixelShaderFile.close();
		vertexCode = vertexShaderStream.str();
		pixelCode = pixelShaderStream.str();
	}
	catch (std::ifstream::failure& e) {
		std::cout << "SHADER FILES NO SUCCESSFULLY READ: " << e.what() << std::endl;
	}

	const char* vcode = vertexCode.c_str();
	const char* pcode = pixelCode.c_str();

	unsigned int vertex, pixel;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vcode, NULL);
	glCompileShader(vertex);

	checkShaderCompileErrors(vertex, "VERTEX");

	pixel = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(pixel, 1, &pcode, NULL);
	glCompileShader(pixel);

	checkShaderCompileErrors(pixel, "PIXEL");

	m_program = glCreateProgram();

	glAttachShader(m_program, vertex);
	glAttachShader(m_program, pixel);
	glLinkProgram(m_program);

	checkProgramLinkErrors(m_program);

	glDeleteShader(vertex);
	glDeleteShader(pixel);
};

void Shader::checkShaderCompileErrors(unsigned int target, const char* type) {
	int success;
	char log[512];
	glGetShaderiv(target, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(target, 512, NULL, log);
		std::cout << type << "SHADER COMPILATION FAILED: " << log << std::endl;
	}
}

void Shader::checkProgramLinkErrors(unsigned int target) {
	int success;
	char log[512];
	glGetProgramiv(target, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(target, 512, NULL, log);
		std::cout << "PROGRAM LINKAGE FAILED: " << log << std::endl;
	}
}

void Shader::use() {
	glUseProgram(m_program);
};

Shader::~Shader() {
	glDeleteProgram(m_program);
	m_program = 0;
};

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
};

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
};

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(m_program, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(glGetUniformLocation(m_program, name.c_str()), 1, &value[0]);
};

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(m_program, name.c_str()), x, y, z, w);
}

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
};

void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
};

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
	glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &value[0][0]);
};
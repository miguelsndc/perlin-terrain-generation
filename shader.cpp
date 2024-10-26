#include "shader.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

Shader::Shader(const std::string& vertexShaderPath, const std::string& pixelShaderPath,
	const std::string& tscShaderPath, const std::string& tseShaderPath)
	: m_program(0)
{
	std::string vertexCode, pixelCode, tscCode, tseCode;
	std::ifstream vertexFile, pixelFile, tscFile, tseFile;

	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	pixelFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tscFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	tseFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vertexFile.open(vertexShaderPath);
		pixelFile.open(pixelShaderPath);
		tscFile.open(tscShaderPath);
		tseFile.open(tseShaderPath);

		std::stringstream vertexStream, pixelStream, tscStream, tseStream;
		vertexStream << vertexFile.rdbuf();
		pixelStream << pixelFile.rdbuf();
		tscStream << tscFile.rdbuf();
		tseStream << tseFile.rdbuf();

		vertexFile.close();
		pixelFile.close();
		tscFile.close();
		tseFile.close();

		vertexCode = vertexStream.str();
		pixelCode = pixelStream.str();
		tscCode = tscStream.str();
		tseCode = tseStream.str();
	}
	catch (std::ifstream::failure& e) {
		std::cout << "SHADER FILES NO SUCCESSFULLY READ: " << e.what() << std::endl;
	}

	const char* vertexCodeRaw = vertexCode.c_str();
	const char* pixelCodeRaw = pixelCode.c_str();
	const char* tscCodeRaw = tscCode.c_str();
	const char* tseCodeRaw = tseCode.c_str();

	unsigned int vertex, pixel, tessCtrl, tessEv;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexCodeRaw, NULL);
	glCompileShader(vertex);

	checkShaderCompileErrors(vertex, "VERTEX");

	pixel = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(pixel, 1, &pixelCodeRaw, NULL);
	glCompileShader(pixel);

	checkShaderCompileErrors(pixel, "PIXEL");

	tessCtrl = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tessCtrl, 1, &tscCodeRaw, NULL);
	glCompileShader(tessCtrl);

	checkShaderCompileErrors(tessCtrl, "TESSELLATION CONTROL");

	tessEv = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tessEv, 1, &tseCodeRaw, NULL);
	glCompileShader(tessEv);

	checkShaderCompileErrors(tessEv, "TESSELLATION EVALUATION");

	m_program = glCreateProgram();

	glAttachShader(m_program, vertex);
	glAttachShader(m_program, pixel);
	glAttachShader(m_program, tessCtrl);
	glAttachShader(m_program, tessEv);
	glLinkProgram(m_program);

	checkProgramLinkErrors(m_program);

	glDeleteShader(vertex);
	glDeleteShader(pixel);
	glDeleteShader(tessCtrl);
	glDeleteShader(tessEv);
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
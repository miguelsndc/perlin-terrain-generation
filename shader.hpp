#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

class Shader {
private:
	unsigned int m_program = 0;
	void checkShaderCompileErrors(unsigned int target, const char* type);
	void checkProgramLinkErrors(unsigned int target);
public:
	Shader(const std::string& vertex_shader_path, const std::string& pixel_shader_path,
		const std::string& tsc_shader_path, const std::string& tse_shader_path
	);
	~Shader();
	void use();
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, const glm::mat4& value) const;
};
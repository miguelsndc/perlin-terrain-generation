#version 460 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texCoord;

out vec2 TexCoord;

void main() {
	gl_Position = vec4(a_position, 1.0);
	TexCoord = a_texCoord;
}
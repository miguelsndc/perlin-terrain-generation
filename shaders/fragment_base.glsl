#version 460 core

out vec4 FragColor;
in float height;

void main() {
	float h = (height + 16.0f) / 32.0f;
	FragColor = vec4(h, h, h, 1.0);
}
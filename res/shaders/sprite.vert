#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexcoord0;

layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aOffset;
layout (location = 4) in vec2 aScale;
layout (location = 5) in float aZ;

uniform vs_params {
	mat4 model;
	mat4 view;
	mat4 proj;
};

out vec4 instCol;

void main() {
	instCol = aColor;

	vec2 instPos = aOffset + (aScale * aPos);
	gl_Position = proj * view * model * vec4(instPos, aZ, 1.0);
}

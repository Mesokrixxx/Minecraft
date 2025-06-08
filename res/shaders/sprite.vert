#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexcoord0;

layout (location = 2) in vec4 aColor;
layout (location = 3) in vec2 aOffset;
layout (location = 4) in vec2 aScale;
layout (location = 5) in float aZ;
layout (location = 6) in int aTexindex;
layout (location = 7) in vec2 aUV_min;
layout (location = 8) in vec2 aUV_max;

uniform vs_params {
	mat4 model;
	mat4 view;
	mat4 proj;
};

out vec4 instCol;
out vec2 uv;
flat out int instTexindex;

void main() {
	instCol = aColor;
	instTexindex = aTexindex;
	uv = aUV_min + (aTexcoord0 * (aUV_max - aUV_min));

	vec2 instPos = aOffset + (aScale * aPos);
	gl_Position = proj * view * model * vec4(instPos, aZ, 1.0);
}

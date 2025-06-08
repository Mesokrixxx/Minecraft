#version 330 core

uniform sampler2D uTextures[8];

in vec4 instCol;
in vec2 uv;
flat in int instTexindex;

out vec4 FragColor;

void main() {
	FragColor = texture(uTextures[instTexindex], uv);
}

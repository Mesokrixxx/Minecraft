#version 330 core

uniform sampler2D uTextures[8];

in vec4 instCol;
in vec2 uv;
flat in int instTexindex;

out vec4 FragColor;

void main() {
	vec4 texCol = texture(uTextures[instTexindex], uv);
	if (texCol.a < 0.001)
		discard;
	FragColor = texCol * instCol;
}

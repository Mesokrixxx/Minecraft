#version 330 core

uniform sampler2D BlockAtlas;

in vec2 uv;
out vec4 FragColor;

void main() {
	vec4 texCol = texture(BlockAtlas, uv);
	if (texCol.a < 0.001)
		discard;
	FragColor = texCol;
}

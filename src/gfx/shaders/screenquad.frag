#version 330 core

uniform sampler2D screenquad;

out vec4 fragColor;

in vec2 uv;

void main() {
	fragColor = texture(screenquad, uv);
}

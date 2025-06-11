#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexcoord;
layout (location = 2) in int aFaceid;

layout (location = 3) in vec2 aOffset;
layout (location = 4) in float aZ;
layout (location = 5) in int aBlockid;

uniform vs_params {
	mat4 model;
	mat4 view;	
	mat4 proj;	
};

out vec2 uv;

const vec2 atlasSizePx = vec2(128); // pixels width/height
const vec2 atlasSpriteSize = vec2(16); // Atlas sprite size (16px)
const vec2 atlasSizeSprite = atlasSizePx / atlasSpriteSize; // Atlas size in sprite
const vec2 atlasTxPerPx = 1 / atlasSizePx; // Texels per pixels
const vec2 atlasSizeTx = atlasSpriteSize * atlasTxPerPx;

void main() {
	vec2 index = vec2(aFaceid, atlasSizeSprite.y - aBlockid - 1);
	vec2 uv_min = index * atlasSpriteSize * atlasTxPerPx;
	vec2 uv_max = uv_min + atlasSizeTx;
	uv = uv_min + (aTexcoord * (uv_max - uv_min));

	vec3 instPos = vec3(aOffset, aZ) + aPos;
	gl_Position = proj * view * model * vec4(instPos, 1);
}

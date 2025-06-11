#include "font.h"

#include "util/assert.h"
#include "math/aliases.h"
#include "sprite.h"

#include <ctype.h>

#define FONT_HEIGHT 8
#define FONT_WIDTH 8
#define DIGIT_NEEDED 2
#define _FONT_PALETTE_SIZE 99
static color FONT_PALETTE[_FONT_PALETTE_SIZE] = {
	{ 1, 1, 1, 1 }, // White
	{ 0, 0, 0, 1 }, // Black
	{ 1, 0, 0, 1 }, // Red
	{ 0, 1, 0, 1 }, // Blue
	{ 0, 0, 1, 1 }, // Green
};

static sprite_manager_t *_sprite_manager;
static sprite_atlas_t font_atlas;

static v2i find_char(char c)
{
	c = toupper(c);

	static const char *chars[] = {
		"ABCDEFGHIJKLMNOP",
		"QRSTUVWXYZ ",
		"0123456789",
		"!@#$^&*()_+-=,./",
		"<>?;':\"[]"
	};

	for (int y = 0; chars[y]; y++) {
		for (int x = 0; chars[y][x]; x++) {
			if (c == chars[y][x])
				return (v2i_of(x, 16 - y - 1));
		}
	}
	return (v2i_of(2, 4)); // Pos of ?
}

void	font_init(sprite_manager_t *manager)
{
	_sprite_manager = manager;

	sprite_atlas_create(&font_atlas, 
		(sprite_atlas_desc){
			.path = "res/textures/font.png",
			.sprite_size = v2i_of(8),
			.format = SPRITE_RGBA,
			.internal_format = SPRITE_RGBA,
		});
	sprite_manager_register(manager, font_atlas);
}

void	font_char(char c, font_desc font)
{
	sprite_manager_push(_sprite_manager, 
		(sprite_t){
			.color = font.color,
			.pos = font.pos,
			.scale = font.scale,
			.z = font.z,
			.index = find_char(c),
			.tex_atlas = font_atlas,
		});
}

void	font_str(char *str, font_desc font)
{
	char *istr = str;
	v2 ipos = font.pos;
	color col = font.color;
	v2 pos = ipos;

	int i = 0;
	while (str[i])
	{
		if (str[i] == '$') {
			if (str[i + 1] == '$')
				i++;
			else if (str[i + 1] != 0) {
				int index = 0;
				i++;
				for (int j = 0; j < DIGIT_NEEDED; j++) {
					ASSERT(isdigit(str[i]),
						"$ must be followed by $ or %d digit for '%s' at index %d",
						DIGIT_NEEDED, istr, i);
					index = index * 10 + (str[i] - '0');
					i++;
				}
				col = FONT_PALETTE[index];
			}
		}

		if (str[i] == '\n') {
			pos.x = ipos.x;
			pos.y -= FONT_HEIGHT * font.scale.y;
			goto next_char;
		}

		font_char(str[i], 
			(font_desc){
				.color = col,
				.scale = font.scale,
				.z = font.z,
				.pos = pos
			});
		pos.x += FONT_WIDTH * font.scale.x;

next_char:
		i++;
	}
}

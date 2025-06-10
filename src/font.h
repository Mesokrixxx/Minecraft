#ifndef FONT_H
# define FONT_H

# include "sprite.h"

typedef struct {
	color color;
	v2 pos;
	v2 scale;
	float z;
}	font_desc;

void	font_init(sprite_manager_t *manager);
void	font_char(char c, font_desc font);
void	font_str(char *str, font_desc font);

#endif
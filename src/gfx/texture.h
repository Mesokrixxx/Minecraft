#ifndef GFX_TEXTURE_H
# define GFX_TEXTURE_H

# include "../math/aliases.h"

# include <stdbool.h>
# include <GL/glew.h>

typedef enum {
	TEXTURE_2D,
	TEXTURE_TYPE_COUNT,
}	texture_type_e;

typedef enum {
	TEXTURE_NEAREST = GL_NEAREST,
	TEXTURE_LINEAR = GL_LINEAR,
	TEXTURE_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
	TEXTURE_LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
	TEXTURE_NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
	TEXTURE_LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR,
}	texture_sampler_filter_e;

typedef enum {
	TEXTURE_REPEAT = GL_REPEAT,
	TEXTURE_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
}	texture_sampler_wrap_e;

typedef enum {
	TEXTURE_RGBA = GL_RGBA,
}	texture_format_e;

typedef enum {
	TEXTURE_UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
}	texture_datatype_e;

typedef struct {
	texture_sampler_wrap_e wrap_u, wrap_v;
	texture_sampler_filter_e min_filter, max_filter;
}	texture_sampler_desc;

typedef struct {
	v2i size;
	texture_format_e internal_format, format;
	texture_datatype_e datatype;
	const void *data;
}	texture_data_desc;

typedef struct {
	unsigned int id;
	unsigned int bind_point;
	texture_type_e type;

	bool mipmap;
	v2i	size;
}	texture_t;

void	texture_create(texture_t *tex, texture_type_e type);
void	texture_sampler_set(texture_t *tex, texture_sampler_desc sampler);
void	texture_data_set(texture_t *tex, texture_data_desc data);
void	texture_destroy(texture_t *tex);
void	texture_bind(texture_t tex);
bool	texture_valid(texture_t tex);

#endif
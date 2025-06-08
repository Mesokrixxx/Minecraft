#include "texture.h"

#include "../util/assert.h"

#define MAX_TEXTURE_BINDPOINT 32

static unsigned int texture_type_target[TEXTURE_TYPE_COUNT] = {
	GL_TEXTURE_2D,
};

void	texture_create(texture_t *tex, texture_type_e type)
{
	static unsigned int texture_count;

	ASSERT(MAX_TEXTURE_BINDPOINT > texture_count,
		"Reached max cap for texture bindpoint (%d)", 
		MAX_TEXTURE_BINDPOINT);

	*tex = (texture_t){
		.bind_point = texture_count,
		.type = type,
	};

	unsigned int new_tex;
	glGenTextures(1, &new_tex);

	tex->id = new_tex;
	texture_bind(*tex);
	texture_count++;
}

void	texture_sampler_set(texture_t *tex, texture_sampler_desc sampler)
{
	texture_bind(*tex);

	glTexParameteri(texture_type_target[tex->type], GL_TEXTURE_WRAP_S, sampler.wrap_u);
	glTexParameteri(texture_type_target[tex->type], GL_TEXTURE_WRAP_T, sampler.wrap_v);
	glTexParameteri(texture_type_target[tex->type], GL_TEXTURE_MIN_FILTER, sampler.min_filter);
	glTexParameteri(texture_type_target[tex->type], GL_TEXTURE_MAG_FILTER, sampler.max_filter);

	if (sampler.min_filter != TEXTURE_NEAREST
		&& sampler.min_filter != TEXTURE_LINEAR)
		tex->mipmap = true;
}

void	texture_data_set(texture_t *tex, texture_data_desc data)
{
	texture_bind(*tex);

	switch (tex->type) {
		case (TEXTURE_2D):
			glTexImage2D(
				texture_type_target[tex->type], 0,
				data.internal_format,
				data.size.x, data.size.y, 0,
				data.format, data.datatype, data.data);
			break ;
		default:
			ASSERT(false, "unknow texture type");
	};
	if (tex->mipmap)
		glGenerateMipmap(texture_type_target[tex->type]);

	tex->size = data.size;
}

void	texture_destroy(texture_t *tex)
{
	glDeleteTextures(1, &tex->id);

	*tex = (texture_t){0};
}

void	texture_bind(texture_t tex)
{
	static unsigned int 
		last_texture[TEXTURE_TYPE_COUNT][MAX_TEXTURE_BINDPOINT];

	if (last_texture[tex.type][tex.bind_point] != tex.id)
	{
		last_texture[tex.type][tex.bind_point] = tex.id;
		glActiveTexture(GL_TEXTURE0 + tex.bind_point);
		glBindTexture(texture_type_target[tex.type], tex.id);
	}
}

bool	texture_valid(texture_t tex)
{
	if (glIsTexture(tex.id) != GL_TRUE)
		return (false);

	return (true);
}

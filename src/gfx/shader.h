#ifndef GFX_SHADER_H
# define GFX_SHADER_H

# include <stdbool.h>

typedef struct {
	unsigned int program;
}	shader_t;

// Call shader_bind after creating shader
void	shader_create(shader_t *shader, const char *vertsource_path, const char *fragsource_path);
void	shader_destroy(shader_t *shader);
void	shader_bind(shader_t shader);
bool	shader_valid(shader_t shader);

#endif
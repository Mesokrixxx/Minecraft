#include "shader.h"

#include "../util/file.h"
#include "../util/assert.h"

#include <GL/glew.h>
#include <string.h>

static void shader_compile(unsigned int *shader, const char *data, unsigned int shadertype)
{
	unsigned int new_shader;

	new_shader = glCreateShader(shadertype);
	glShaderSource(new_shader, 1, &data, NULL);
	glCompileShader(new_shader);

	int  success;
	char infoLog[512];
	glGetShaderiv(new_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(new_shader, 512, NULL, infoLog);
		ASSERT(false, "Error log: %s", infoLog);
	}

	*shader = new_shader;
}

void	shader_create(shader_t *shader, const char *vertsource_path, const char *fragsource_path)
{
	*shader = (shader_t){0};

	unsigned int program;
	unsigned int vertex_shader;
	unsigned int fragment_shader;

	char *vertsource = file_get(vertsource_path);
	char *fragsource = file_get(fragsource_path);

	shader_compile(&vertex_shader, vertsource, GL_VERTEX_SHADER);
	shader_compile(&fragment_shader, fragsource, GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		ASSERT(false, "Error log: %s", infoLog);
	}

	free(vertsource);
	free(fragsource);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	shader->program = program;
	shader_bind(*shader);
}

void	shader_destroy(shader_t *shader)
{
	glDeleteProgram(shader->program);

	*shader = (shader_t){0};
}

void	shader_bind(shader_t shader)
{
	static unsigned int last_progam;

	if (last_progam != shader.program)
	{
		last_progam = shader.program;
		glUseProgram(shader.program);
	}
}

bool	shader_valid(shader_t shader)
{
	if (glIsProgram(shader.program) != GL_TRUE)
		return false;

	return true;
}

void	shader_uniform_setbindpoint(shader_t shader, const char *uniformname, unsigned int bindpoint)
{
	unsigned int blockindex;

	blockindex = glGetUniformBlockIndex(shader.program, uniformname);
	ASSERT(blockindex != GL_INVALID_INDEX,
		"Failed to found %s", uniformname);

	glUniformBlockBinding(shader.program, blockindex, bindpoint);
}

void	shader_uniform_loc(shader_t shader, const char *uniformname, unsigned int *loc)
{
	unsigned int uIndex;

	uIndex = glGetUniformLocation(shader.program, uniformname);
	ASSERT(uIndex != GL_INVALID_INDEX,
		"Failed to found %s", uniformname);

	*loc = uIndex;
}

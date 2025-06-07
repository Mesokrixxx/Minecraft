#include "shader.h"

#include "../util/file.h"
#include "../util/assert.h"

#include <GL/glew.h>

static void shader_compile(unsigned int *shader, const char *data, unsigned int shadertype)
{
	*shader = glCreateShader(shadertype);
	glShaderSource(*shader, 1, &data, NULL);
	glCompileShader(*shader);

	int  success;
	char infoLog[512];
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(*shader, 512, NULL, infoLog);
		ASSERT(false, "Error log: %s", infoLog);
	}
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
	shader_compile(&vertex_shader, fragsource, GL_FRAGMENT_SHADER);

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

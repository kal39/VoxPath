#include <stdlib.h>

#include "gl_util.h"
#include "shader.h"
#include "util/log.h"

static GLchar *_shader_compile_error(GLuint shader) {
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		GLint message_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &message_length);
		GLchar *message = malloc(message_length + 1);
		glGetShaderInfoLog(shader, message_length, NULL, message);
		return message;
	}

	return NULL;
}

static GLchar *_shader_link_error(GLuint shader) {
	GLint success;
	glGetProgramiv(shader, GL_LINK_STATUS, &success);

	if (!success) {
		GLint message_length = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &message_length);
		GLchar *message = malloc(message_length + 1);
		glGetProgramInfoLog(shader, message_length, NULL, message);
		return message;
	}

	return NULL;
}

GLuint load_shader(char *path, GLenum type) {
	MESSAGE("compiling shader from %s", path);

	char *code = read_file(path);
	if (code == NULL) {
		ERROR("could not open %s", path);
		return 0;
	}

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const char **)&code, NULL);
	free(code);
	glCompileShader(shader);

	char *message = _shader_compile_error(shader);
	if (message != NULL) {
		ERROR("error compiling vertex shader:\n%s", message);
		free(message);
		return 0;
	}

	return shader;
}

GLuint make_program(GLuint *shaders, int shaderCount) {
	MESSAGE("linking %d shaders", shaderCount);

	GLuint program = glCreateProgram();
	for (int i = 0; i < shaderCount; i++) glAttachShader(program, shaders[i]);
	glLinkProgram(program);

	char *message = _shader_link_error(program);
	if (message != NULL) {
		ERROR("error linking render program:\n%s", message);
		free(message);
		return 0;
	}

	return program;
}

GLuint load_compute_shader(char *path) {
	GLuint shader = load_shader(path, GL_COMPUTE_SHADER);
	if (shader == 0) return 0;
	GLuint program = make_program(&shader, 1);
	return program;
}
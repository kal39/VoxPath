#include <stdio.h>
#include <stdlib.h>

#include "gl_util.h"
#include "util/log.h"

Status gl_error() {
	int hadError = 0;
	for (GLenum err = glGetError(); err != GL_NO_ERROR; err = glGetError()) {
		hadError = 1;
		switch (err) {
			case GL_INVALID_ENUM: {
				ERROR("gl error: GL_INVALID_ENUM");
				break;
			}
			case GL_INVALID_VALUE: {
				ERROR("gl error: GL_INVALID_VALUE");
				break;
			}
			case GL_INVALID_OPERATION: {
				ERROR("gl error: GL_INVALID_OPERATION");
				break;
			}
			case GL_INVALID_FRAMEBUFFER_OPERATION: {
				ERROR("gl error: GL_INVALID_FRAMEBUFFER_OPERATION");
				break;
			}
			case GL_OUT_OF_MEMORY: {
				ERROR("gl error: GL_OUT_OF_MEMORY");
				break;
			}
			case GL_STACK_UNDERFLOW: {
				ERROR("gl error: GL_STACK_UNDERFLOW");
				break;
			}
			case GL_STACK_OVERFLOW: {
				ERROR("gl error: GL_STACK_OVERFLOW");
				break;
			}
			default: {
				ERROR("gl error: UNKNOWN");
				break;
			}
		}
	}

	if (hadError) {
		return STATE_FAILURE;
	} else {
		DEBUG("no opengl errors");
		return STATE_SUCCESS;
	}
}

char *read_file(char *fileName) {
	FILE *fp = fopen(fileName, "rb");
	if (fp == NULL) return NULL;

	fseek(fp, 0L, SEEK_END);
	size_t fileSize = ftell(fp);
	rewind(fp);

	char *buff = malloc(fileSize + 1);
	fread(buff, sizeof(char), fileSize, fp);
	buff[fileSize] = '\0';

	fclose(fp);
	return buff;
}

GLuint make_texture(GLenum unit, int w, int h, GLenum access, GLenum fmt) {
	GLuint texture;

	glGenTextures(1, &texture);
	glActiveTexture(unit);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, fmt, w, h);
	glBindImageTexture(
		unit - GL_TEXTURE0,
		texture,
		0,
		GL_FALSE,
		0,
		access,
		fmt
	);

	return texture;
}

GLuint make_screen_quad() {
	/*
	 * Based on:
	 * - https://learnopengl.com/Getting-started/Hello-Triangle
	 * - https://learnopengl.com/Getting-started/Shaders
	 * - https://learnopengl.com/Getting-started/Textures
	 */

	float vertices[]
		= {1, 1, 0, 1, 0, 1, -1, 0, 1, 1, -1, -1, 0, 0, 1, -1, 1, 0, 0, 0};
	unsigned int indices[] = {0, 1, 3, 1, 2, 3};

	GLuint vbo, ebo, vao;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 20, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int) * 6,
		indices,
		GL_STATIC_DRAW
	);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(float),
		(void *)(0 * sizeof(float))
	);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(float),
		(void *)(3 * sizeof(float))
	);
	glEnableVertexAttribArray(1);

	return vao;
}
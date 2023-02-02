#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLuint load_shader(char *path, GLenum type);
GLuint make_program(GLuint *shaders, int shaderCount);
GLuint load_compute_shader(char *path);
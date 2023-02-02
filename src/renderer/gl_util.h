#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "util/status.h"

Status gl_error();
char *read_file(char *fileName);
GLuint make_texture(GLenum unit, int w, int h, GLenum access, GLenum fmt);
GLuint make_screen_quad();
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "gl_util.h"
#include "renderer.h"
#include "shader.h"
#include "util/log.h"

typedef struct RendererState {
	int width;
	int height;
	GLFWwindow *window;
	GLuint renderProgram;
	GLuint ptProgram;
	GLuint textProgram;
	GLuint clearProgram;
	GLuint vao;
	GLuint renderTexture;
	GLuint ptTexture;
	GLuint fontTexture;
	double prevTime;
} RendererState;

typedef struct TextColor {
	float r;
	float g;
	float b;
	float a;
} TextColor;

static RendererState g_state;

static void _draw_character(int x, int y, int scale, TextColor col, char c) {
	GLint pos = glGetUniformLocation(g_state.textProgram, "charPos");
	glProgramUniform2i(g_state.textProgram, pos, x, y);

	pos = glGetUniformLocation(g_state.textProgram, "charScale");
	glProgramUniform1i(g_state.textProgram, pos, scale);

	pos = glGetUniformLocation(g_state.textProgram, "charColour");
	glProgramUniform4f(g_state.textProgram, pos, col.r, col.g, col.b, col.a);

	pos = glGetUniformLocation(g_state.textProgram, "charOffset");
	glProgramUniform1ui(g_state.textProgram, pos, c - 32);

	glUseProgram(g_state.textProgram);
	glDispatchCompute(8 * scale, 10 * scale, 1);
}

void _draw_text(int x, int y, int scale, TextColor col, char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int len = vsnprintf(NULL, 0, fmt, args) + 1;
	va_end(args);
	char *text = malloc(len);
	va_start(args, fmt);
	vsnprintf(text, len, fmt, args);
	va_end(args);

	for (char *c = text; *c != '\0'; c++) {
		int offset = (c - text) * 8 * scale;
		_draw_character(x + offset, y, scale, col, *c);
	}

	free(text);
}

Status renderer_start(RendererSettings settings) {
	MESSAGE("initialising rendering pipeline");

	g_state.width = settings.width;
	g_state.height = settings.height;

	//========================================================================//
	// initialise OpenGL and create window
	//========================================================================//

	MESSAGE("initialising GLFW");
	glfwInit();

	MESSAGE(
		"opening window, size: %dx%d, title: %s",
		g_state.width,
		g_state.height,
		settings.title
	);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	g_state.window = glfwCreateWindow(
		g_state.width,
		g_state.height,
		settings.title,
		NULL,
		NULL
	);

	glfwMakeContextCurrent(g_state.window);
	glewInit();
	glfwSwapInterval(0);

	//========================================================================//
	// load shaders
	//========================================================================//

	GLuint shaders[5];
	shaders[0] = load_shader(settings.vertexShaderPath, GL_VERTEX_SHADER);
	shaders[1] = load_shader(settings.fragmentShaderPath, GL_FRAGMENT_SHADER);
	shaders[2] = load_shader(settings.ptShaderPath, GL_COMPUTE_SHADER);
	shaders[3] = load_shader(settings.textShaderPath, GL_COMPUTE_SHADER);
	shaders[4] = load_shader(settings.clearShaderPath, GL_COMPUTE_SHADER);

	if ((g_state.renderProgram = make_program(shaders, 2)) == 0)
		return STATE_FAILURE;
	if ((g_state.ptProgram = make_program(&shaders[2], 1)) == 0)
		return STATE_FAILURE;
	if ((g_state.textProgram = make_program(&shaders[3], 1)) == 0)
		return STATE_FAILURE;
	if ((g_state.clearProgram = make_program(&shaders[4], 1)) == 0)
		return STATE_FAILURE;

	//========================================================================//
	// make VAO for a full-screen quad
	//========================================================================//

	g_state.vao = make_screen_quad();

	//========================================================================//
	// make textures
	//========================================================================//

	g_state.renderTexture = make_texture(
		GL_TEXTURE0,
		g_state.width,
		g_state.height,
		GL_READ_WRITE,
		GL_RGBA32F
	);

	g_state.ptTexture = make_texture(
		GL_TEXTURE1,
		g_state.width,
		g_state.height,
		GL_READ_WRITE,
		GL_RGBA32F
	);

	//========================================================================//
	// load font
	//========================================================================//

	int fontDataWidth, fontDataHeight;
	unsigned char *fontData = stbi_load(
		settings.fontFilePath,
		&fontDataWidth,
		&fontDataHeight,
		NULL,
		1
	);

	if (fontData == NULL) {
		ERROR("failed to load font from %s", settings.fontFilePath);
		return STATE_FAILURE;
	}

	g_state.fontTexture = make_texture(
		GL_TEXTURE2,
		fontDataWidth,
		fontDataHeight,
		GL_READ_ONLY,
		GL_R32F
	);

	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		0,
		0,
		fontDataWidth,
		fontDataHeight,
		GL_RED,
		GL_UNSIGNED_BYTE,
		fontData
	);

	free(fontData);

	//========================================================================//
	// misc
	//========================================================================//

	g_state.prevTime = glfwGetTime();

	return gl_error();
}

Status renderer_stop() {
	MESSAGE("terminating OpenGL");
	glfwTerminate();
	return STATE_SUCCESS;
}

Status renderer_step() {
	//========================================================================//
	// update fps
	//========================================================================//

	double currentTime = glfwGetTime();
	double fps = 1.0 / (currentTime - g_state.prevTime);
	g_state.prevTime = currentTime;

	//========================================================================//
	// run path tracing
	//========================================================================//

	glUseProgram(g_state.ptProgram);
	glDispatchCompute(g_state.width, g_state.height, 1);

	//========================================================================//
	// clear stuff
	//========================================================================//

	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(g_state.clearProgram);
	glDispatchCompute(g_state.width, g_state.height, 1);

	//========================================================================//
	// draw ui
	//========================================================================//

	TextColor white = (TextColor){1.0, 1.0, 1.0, 1.0};
	_draw_text(10, 10, 2, white, "%04d fps", (int)fps);

	//========================================================================//
	// render texture to screen
	//========================================================================//

	glUseProgram(g_state.renderProgram);
	glBindVertexArray(g_state.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//========================================================================//
	// misc
	//========================================================================//

	glfwSwapBuffers(g_state.window);
	glfwPollEvents();

	return STATE_SUCCESS;
}

int renderer_should_stop() {
	return glfwWindowShouldClose(g_state.window);
}

#include <stdio.h>

#include "renderer/renderer.h"
#include "util/log.h"
#include "util/status.h"

int main(void) {
	RendererSettings settings = {
		.width = 800,
		.height = 600,
		.title = "VoxPath",
		.fontFilePath = "assets/font.png",
		.vertexShaderPath = "shaders/vertex_shader.glsl",
		.fragmentShaderPath = "shaders/fragment_shader.glsl",
		.ptShaderPath = "shaders/path_trace_shader.glsl",
		.textShaderPath = "shaders/text_shader.glsl",
		.clearShaderPath = "shaders/clear_shader.glsl",
	};

	if (renderer_start(settings) != STATE_SUCCESS) {
		ERROR("failed to start pipeline");
		renderer_stop();
		return -1;
	}

	while (!renderer_should_stop()) renderer_step();

	renderer_stop();
	return 0;
}
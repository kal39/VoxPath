#pragma once

#include "util/status.h"

typedef struct {
	int width;
	int height;
	char *title;
	char *fontFilePath;
	char *vertexShaderPath;
	char *fragmentShaderPath;
	char *ptShaderPath;
	char *textShaderPath;
	char *clearShaderPath;
} RendererSettings;

Status renderer_start(RendererSettings settings);
Status renderer_stop();
Status renderer_step();
int renderer_should_stop();
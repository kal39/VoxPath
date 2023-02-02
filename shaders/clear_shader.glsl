#version 430

layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba32f, binding = 0) uniform image2D renderImage;

void main() {
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	imageStore(renderImage, pos, vec4(0.0, 0.0, 0.0, 0.0));
}
#version 430

layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba32f, binding = 1) uniform image2D computeImage;

void main() {
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = ivec2(gl_NumWorkGroups.xy);

	vec4 pixel = imageLoad(computeImage, pos);
	imageStore(computeImage, pos, vec4(vec2(pos) / vec2(size), 0.0, 1.0));
}
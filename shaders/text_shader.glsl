#version 430

layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba32f, binding = 0) uniform image2D renderImage;
layout(r32f, binding = 2) uniform image2D fontImage;

uniform ivec2 charPos;
uniform vec4 charColor;
uniform int charScale;
uniform uint charOffset;

void main() {
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = ivec2(gl_NumWorkGroups.xy);

	ivec2 fontPos =
		ivec2((charOffset % 16) * 8, (charOffset / 16) * 10) + pos / charScale;
	vec4 color = vec4(1.0, 1.0, 1.0, imageLoad(fontImage, fontPos).r);

	imageStore(renderImage, pos + charPos, color);
}
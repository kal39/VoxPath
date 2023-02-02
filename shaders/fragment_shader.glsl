#version 430

in vec2 texCoord;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D renderImage;
layout(binding = 1) uniform sampler2D computeImage;

void main() {
	vec4 renderColor = texture(renderImage, texCoord);
	vec4 computeColor = texture(computeImage, texCoord);

	if(renderColor.a != 0) {
		fragColor = renderColor;
	} else {
		fragColor = computeColor;
	}
}
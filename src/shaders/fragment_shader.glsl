#version 430

in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D sampler;
uniform float progress;

void main() {
	fragColor = texture(sampler, texCoord)
		* (1 + sin(3.1415926 * 2 * progress)) / 2;
}

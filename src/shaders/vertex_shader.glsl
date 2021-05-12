#version 430

layout (location = 0) in vec2 VertexPosition;
layout (location = 1) in vec3 VertexColor;
out vec3 Color;

uniform float progress;

void main() {
	Color = VertexColor * (1 + sin(3.1415926 * 2 * progress)) / 2;
	gl_Position = vec4(VertexPosition, 0.0, 1.0);
}

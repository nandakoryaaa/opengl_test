#version 430

layout (location = 0) in vec4 Vertex;

out vec2 texCoord;

void main() {
	texCoord = Vertex.zw;
	gl_Position = vec4(Vertex.xy, 0.0, 1.0);
}

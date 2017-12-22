#version 450

layout (location = 0) in vec2 VertexPosition;
out vec2 oPos;

void main() {
     gl_Position = vec4(VertexPosition,1.0,1.0);
     oPos = VertexPosition;
     return;
}
#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
out V{
vec3 tnorm;
vec4 eyeCoords;
}v_out;
uniform mat4 Model;
uniform mat4 View;
uniform mat3 NormalMatrix;
uniform mat4 Projection;

void main()
{
    v_out.tnorm = normalize( NormalMatrix * VertexNormal);
    v_out.eyeCoords = View * Model* vec4(VertexPosition,1.0);
    gl_Position =Projection * View * Model * vec4(VertexPosition,1.0);
}

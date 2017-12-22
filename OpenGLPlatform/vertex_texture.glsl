#version 440

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;
//layout (location = 1) in vec3 VertexNormal;
out V{
//vec3 tnorm;
//vec4 eyeCoords;
vec3 color;
vec2 texCoord;
}v_out;
uniform mat4 Model;
uniform mat4 View;
//uniform mat3 NormalMatrix;
uniform mat4 Projection;

void main()
{
    //v_out.tnorm = normalize( NormalMatrix * VertexNormal);
    //v_out.eyeCoords = View * Model* vec4(VertexPosition,1.0);
    //gl_Position =Projection * View * Model * vec4(VertexPosition,1.0);
    gl_Position =Projection * View * Model * vec4(VertexPosition,1.0);
    //gl_Position = Model * vec4(VertexPosition,1.0);
    v_out.color = color;
    v_out.texCoord = texCoord;
	//gl_Position = 
}

#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 color;
out V{
//vec3 tnorm;
//vec4 eyeCoords;
vec3 color;
}v_out;

void main()
{
    //v_out.tnorm = normalize( NormalMatrix * VertexNormal);
    //v_out.eyeCoords = View * Model* vec4(VertexPosition,1.0);
    //gl_Position =Projection * View * Model * vec4(VertexPosition,1.0);
    gl_Position =vec4(VertexPosition,1.0);
    //gl_Position = Model * vec4(VertexPosition,1.0);
    v_out.color = color;
	//gl_Position = 
}

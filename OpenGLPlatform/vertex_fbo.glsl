#version 410

layout (location = 0) in vec3 VertexPosition;
out V{
vec2 texCoord;
//vec3 color;
}v_out;

void main()
{
    //v_out.tnorm = normalize( NormalMatrix*VertexNormal);
    v_out.texCoord = VertexPosition.xy;
    //gl_Position =Projection * View * Model * vec4(VertexPosition,1.0);
    gl_Position = vec4(VertexPosition,1.0);
    //gl_Position = Model * vec4(VertexPosition,1.0);
    //v_out.color = color;
	//gl_Position = 
}
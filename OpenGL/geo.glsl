#version 430 core
layout(triangles) in;
layout(points, max_vertices = 3) out;
in V{
vec3 tnorm;
vec4 eyeCoords;
}v_in[];
out V{
vec3 tnorm;
vec4 eyeCoords;
}geo_out;
void main(void)
{
	int i;
	for (i = 0; i < gl_in.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		geo_out.tnorm = v_in[0].tnorm;
		geo_out.eyeCoords = v_in[1].eyeCoords;
		EmitVertex();
	}	
	geo_out.tnorm = v_in[0].tnorm;
	geo_out.eyeCoords = v_in[1].eyeCoords;
	EndPrimitive();
}
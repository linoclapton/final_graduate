#version 430 core 
layout(vertices = 3) out; 
in V{
vec3 tnorm;
vec4 eyeCoords;
}v_in[];
out TCS{
vec3 tnorm;
vec4 eyeCoords;
}tcs_out[];

void main(void) 
{
	if (gl_InvocationID == 0)
	{	
		gl_TessLevelInner[0] = 1.0;
		gl_TessLevelOuter[0] = 1.0;
		gl_TessLevelOuter[1] = 1.0;
		gl_TessLevelOuter[2] = 1.0; 
	}
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	tcs_out[gl_InvocationID].tnorm = v_in[gl_InvocationID].tnorm;
	tcs_out[gl_InvocationID].eyeCoords = v_in[gl_InvocationID].eyeCoords;
}
#version 430 core                       
layout(triangles) in;
in TCS{
vec3 tnorm;
vec4 eyeCoords;
}tcs_in[];
out V{
vec3 tnorm;
vec4 eyeCoords;
}tes_out;
void main(void)                         
{                                       
	gl_Position = gl_TessCoord.x * gl_in[0].gl_Position +gl_TessCoord.y * gl_in[1].gl_Position +gl_TessCoord.z * gl_in[2].gl_Position; 
	vec3 tmp = mix(tcs_in[0].tnorm,tcs_in[1].tnorm,gl_TessCoord.y);
	tes_out.tnorm = mix(tcs_in[1].tnorm,tcs_in[2].tnorm,gl_TessCoord.z);
	vec4 eyeTmp = mix(tcs_in[0].eyeCoords,tcs_in[1].eyeCoords,gl_TessCoord.y); 
	tes_out.eyeCoords = mix(tcs_in[1].eyeCoords,tcs_in[2].eyeCoords,gl_TessCoord.z); 
}                                       
#pragma once
#include <GL/glew.h>
class Torus{
private:
	float innerRadius;
	float outerRadius;
	int rings;
	int slices;
	int faces;
	GLuint vertex_array;
	GLuint buffers[3];
	int render_mode;
	void generateModelData(float* vertice,float* normals,unsigned int* elements);
public:
	Torus(float R, float r, int rings, int slices);
	void model();
	void render();
	void del();
	void setData(float R,float r,int rings,int slices);
	void setData(float value,int index);
	void setRenderMode(int mode);
};
#include "Torus.h"
#include <cstdio>
#include <cmath>
#include <iostream>
#include <fstream>
#include <QLabel>
Torus::Torus(float R, float r, int rings, int slices) :rings(rings), slices(slices){
	outerRadius = R;
	innerRadius = r;
}

void Torus::render(){
	glBindVertexArray(vertex_array);
	glDrawElements(render_mode, 3 * faces, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));
}
void Torus::del(){
	glDeleteBuffers(3,buffers);
	glDeleteVertexArrays(1,&vertex_array);
}
void Torus::generateModelData(float* vertice,float* normals,unsigned int* elements)
{
	const float PI = 3.141592657f;
	float ringRadian = 2 * PI / rings;
	float sliceRadian = 2 * PI / slices;
	float R = outerRadius, r = innerRadius;
	for (int i = 0; i < rings+1; i++){
		float u = ringRadian*i;
		float cu = cos(u);
		float su = sin(u);
		for (int j = 0; j < slices; j++){
			float v = sliceRadian*j;
			float cv = cos(v);
			float sv = sin(v);
			int index = (i * slices + j)*3;
			float r_m = R + r*cv;
			vertice[index] = r_m*cu;
			vertice[index + 1] = r_m*su;
			vertice[index + 2] = r*sv;
			normals[index] = r_m*cv*cu;
			normals[index + 1] = r_m*cv*su;
			normals[index + 2] = r_m*sv;
			float length = sqrt(normals[index] * normals[index] + normals[index + 1] * normals[index + 1] + normals[index + 2] * normals[index + 2]);
			normals[index] /= length;
			normals[index + 1] /= length;
			normals[index + 2] /= length;
		}
	}

	for (int i = 0; i < rings; i++){
		for (int j = 0; j < slices; j++){
			int index = (i*slices + j) * 6;
			elements[index] = i*slices + j;
			elements[index + 1] = (i + 1)*slices + j;
			elements[index + 2] = (i + 1)*slices + (j + 1) % slices;
			elements[index + 3] = i*slices + j;
			elements[index + 4] = (i + 1)*slices + (j + 1) % slices;
			elements[index + 5] = i*slices + (j + 1) % slices;
		}
	}

}
void Torus::setData(float value,int index){
	bool hasChanged = false;
	switch(index){

	case 1:if(outerRadius!=value){outerRadius = value;hasChanged=true;}break;
	case 2:if(innerRadius != value){innerRadius = value;hasChanged=true;}break;
	case 3:if(rings!=(int)value){rings = (int)value;hasChanged=true;}break;
	case 4:if(slices!=(int)value){slices = (int)value;hasChanged=true;}break;
	}
	if(hasChanged){
		del();
		model();
		render();
	}
}
void Torus::setData(float R,float r,int rings,int slices)
{
	if(R==outerRadius&&r==innerRadius&&this->rings==rings&&this->slices==slices)
		return;
	else{
		outerRadius = R;
		innerRadius = r;
		this->rings = rings;
		this->slices = slices;
		del();
		model();
		render();
	}
}
void Torus::model(){
	faces = 2 * rings*slices;
	int count = (rings+1)*slices;
	float* vertice = new float[3 * count];
	float* normals = new float[3 * count];
	unsigned int* elements = new unsigned int[3 * faces];
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);
	generateModelData(vertice,normals,elements);
	glGenBuffers(3, buffers);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 3*count*sizeof(float), vertice, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, 3*count*sizeof(float), normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*faces*sizeof(unsigned int), elements, GL_STATIC_DRAW);

	delete[] vertice;
	delete[] normals;
	delete[] elements;


	


}
void Torus::setRenderMode(int mode)
{
	render_mode = mode;
}
#include "AVSTable2D.h"
#include <fstream>
#include <iostream>
#include <cmath>

using namespace std;

#define COLORMAP_WIDTH 4

// black body radiation
unsigned char colormap[COLORMAP_WIDTH][3] = { {  0,   0,   0}, 
                                              {255,   0,   0}, 
                                              {255, 255,   0},
                                              {255, 255, 255} };


AVSTable2D::AVSTable2D(void) {

	m_a = 0.0f;
	m_g = 0.0f;
	m_N = 0;
	m_sigmat_res = 0;
	m_theta_res = 0;
	m_sigmat_min = 0.0f;
	m_sigmat_max = 0.0f;
	m_sphere_r = 0.0f;
	m_L = NULL;
	m_allocated = false;
}


AVSTable2D::AVSTable2D(string filename) {

	m_a = 0.0f;
	m_g = 0.0f;
	m_N = 0;
	m_sigmat_res = 0;
	m_theta_res = 0;
	m_sigmat_min = 0.0f;
	m_sigmat_max = 0.0f;
	m_sphere_r = 0.0f;
	m_L = NULL;
	m_allocated = false;

	loadFromFile(filename);
}


AVSTable2D::~AVSTable2D(void) {

	free();
}


bool AVSTable2D::saveToFile(string filename) {

	FILE* fp;
	fp = fopen(filename.c_str(), "wb");

	if(fp == NULL) {

		cout << "Could not save to file: " << filename << endl;
		return false;
	}

	// table parameters
	fwrite(&m_a, sizeof(float), 1, fp);
	fwrite(&m_g, sizeof(float), 1, fp);
	fwrite(&m_N, sizeof(int), 1, fp);
	fwrite(&m_sigmat_res, sizeof(int), 1, fp);
	fwrite(&m_theta_res, sizeof(int), 1, fp);
	fwrite(&m_sigmat_min, sizeof(float), 1, fp);
	fwrite(&m_sigmat_max, sizeof(float), 1, fp);
	fwrite(&m_sphere_r, sizeof(float), 1, fp);

	// radiance data
	fwrite(m_L, sizeof(float), m_sigmat_res * m_theta_res, fp);

	fclose(fp);

	return true;
}


bool AVSTable2D::loadFromFile(string filename) {

	FILE* fp;
 	fp = fopen(filename.c_str(), "rb");

	if(fp == NULL) {

		cout << "Could not load from file: " << filename << endl;
		return false;
	}

	// table parameters
	fread(&m_a, sizeof(float), 1, fp);
	fread(&m_g, sizeof(float), 1, fp);
	fread(&m_N, sizeof(int), 1, fp);
	fread(&m_sigmat_res, sizeof(int), 1, fp);
	fread(&m_theta_res, sizeof(int), 1, fp);
	fread(&m_sigmat_min, sizeof(float), 1, fp);
	fread(&m_sigmat_max, sizeof(float), 1, fp);
	fread(&m_sphere_r, sizeof(float), 1, fp);

	// radiance data
	allocate(m_theta_res, m_sigmat_res);
	fread(m_L, sizeof(float), m_sigmat_res * m_theta_res, fp);

	fclose(fp);

	return true;
}


void AVSTable2D::plotRadianceToPPM(string filename) {

	if(!m_allocated) {

		cout << "Could not create radiance plot." << endl;
		return;
	}

	cout << "Creating radiance plot of a=" << m_a << " g=" << m_g << "..." << endl;

	// min and max exponentials for the logarithmic range of the plot
	float L_max = 0.0f;			// = 1
	float L_min = -4.0f;		// = 1e-4

	ofstream ppm(filename.c_str(), std::ios::out);

	// write the ppm header
	ppm.put('P');				// magic number
	ppm.put('3');
	ppm.put('\n');
	ppm << m_theta_res;			// width
	ppm.put(' ');
	ppm << m_sigmat_res;		// height
	ppm.put('\n');
	ppm << 255;					// maximum value
	ppm.put('\n');

	// write image data
	for(int sigmat_i = m_sigmat_res-1; sigmat_i >= 0; --sigmat_i) {
		for(int theta_i = 0; theta_i < m_theta_res; ++theta_i) {
		
			float L = evalRadiance(sigmat_i, theta_i);
		
			if(L > 0.0f)
				L = log10(L);
			else
				L = L_min;

			if(L < L_min)
				L = L_min;

			float f = (L - L_min) / (L_max - L_min);
			if(f < 0.0f) f = 0.0f;
			if(f > 1.0f) f = 1.0f;
			float fc = f * (COLORMAP_WIDTH-1);
			int ia = (int)floor(fc);
			int ib = (int)ceil(fc);
			
			unsigned char ra = colormap[ia][0];
			unsigned char ga = colormap[ia][1];
			unsigned char ba = colormap[ia][2];

			unsigned char rb = colormap[ib][0];
			unsigned char gb = colormap[ib][1];
			unsigned char bb = colormap[ib][2];

			float fn = fc - ia;
			if(fn < 0.0f) fn = 0.0f;
			if(fn > 1.0f) fn = 1.0f;
			
			ppm << (int)((1.0 - fn)*ra + fn*rb);
			ppm.put(' ');

			ppm << (int)((1.0 - fn)*ga + fn*gb);
			ppm.put(' ');

			ppm << (int)((1.0 - fn)*ba + fn*bb);
			ppm.put('\n');
		}
	}

	ppm.close();
}


void AVSTable2D::allocate(int theta_res, int sigmat_res) {

	if(m_allocated)
		free();

	m_L = new float[theta_res * sigmat_res];
	m_allocated = true;
}


void AVSTable2D::free(void) {

	if(m_L != NULL)
		delete [] m_L;

	m_L = NULL;
	m_allocated = false;
}
#pragma once

#include <string>
using std::string;


/*
 * Ambient Volume Scattering Table. This class implements 
 * one 2D slice of the preintegration table with constant
 * anisotropy value. Radiance varies with polar angle theta
 * and ambient extinction sigmat.
 */

class AVSTable2D {

public:
	AVSTable2D(void);
	AVSTable2D(string filename);
	~AVSTable2D(void);
 
	// I/O
	bool		saveToFile(string filename);
	bool		loadFromFile(string filename);

	// plot radiance to ppm image
	void		plotRadianceToPPM(string filename);

	// evaluate radiance
	float		evalRadiance(int sigmat_i, int theta_i) { return m_L[getIndex2D(sigmat_i, theta_i)]; }

	// data access
	float       getAlbedo(void)							{ return m_a; }
	float		getAnisotropy(void)						{ return m_g; }
	int			getNumPathSamples(void)					{ return m_N; }
	int			getSigmaTResolution(void)				{ return m_sigmat_res; }
	int			getThetaResolution(void)				{ return m_theta_res; }
	float		getSigmaTMin(void)						{ return m_sigmat_min; }
	float		getSigmaTMax(void)						{ return m_sigmat_max; }
	float		getSphereRadius(void)					{ return m_sphere_r; }
	float*		getRadianceDataPtr(void)				{ return m_L; }

	void		setAlbedo(float a)						{ m_a = a; }
	void		setAnisotropy(float g)					{ m_g = g; }
	void		setNumPathSamples(int N)				{ m_N = N; }
	void		setSigmaTResolution(int sigmat_res)		{ m_sigmat_res = sigmat_res; }
	void		setThetaResolution(int theta_res)		{ m_theta_res = theta_res; }
	void		setSigmaTMin(float sigmat_min)			{ m_sigmat_min = sigmat_min; }
	void		setSigmaTMax(float sigmat_max)			{ m_sigmat_max = sigmat_max; }
	void		setSphereRadius(float sphere_r)			{ m_sphere_r = sphere_r; }
	void		setRadiancePtr(float* L)				{ m_L = L; }

protected:
	// memory management
	void		allocate(int theta_res, int sigmat_res);
	void		free(void);

	// index for direct data access
	int			getIndex2D(int sigmat_i, int theta_i) { return sigmat_i * m_theta_res + theta_i; }

protected:
	float		m_a;					// albedo
	float		m_g;					// anisotropy parameter for Henyey-Greenstein phase function
	int			m_N;					// number of path samples
	int			m_sigmat_res;			// resolution of ambient extinction coefficient 
	int			m_theta_res;			// resolution of angular bins
	float		m_sigmat_min;			// minimum value of ambient extinction coefficient
	float		m_sigmat_max;			// maximum value of ambient extinction coefficient
	float		m_sphere_r;				// radius of sphere 
	float*		m_L;					// radiance data (theta, sigmat)
	bool		m_allocated;			// indicates if memory is allocated
};

/*
 * Copyright 1993-2015 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */

/*
 * This sample implements a conjugate gradient solver on GPU
 * using CUBLAS and CUSPARSE
 *
 */

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

/* Using updated (v2) interfaces to cublas */
#include <cuda_runtime.h>
#include <cusparse.h>
#include <cublas_v2.h>
#include <cuda.h>

// Utilities and system includes
#include <helper_functions.h>  // helper for shared functions common to CUDA Samples
#include <helper_cuda.h>       // helper function CUDA error checking and initialization

#include <string>

const char *sSDKname     = "conjugateGradient";
extern "C"
float* gc( int X, int Y, int Z, float* i_udata, int *dims, int dim, float* i_opacity, float *i_min, float *i_max);


class DataBlock{
public:
	int *dimension;
	int *idx;
	int *DIM;
	float* opacity;
	float* udata;
	float *min;
	float *max;
	int *Nd;
	float *ux;
};


class Clock {
private:
	static const int N = 50;
	int index;
	LARGE_INTEGER startCounter[N], frequency, stopCounter[N];
	std::string text;
public:
	Clock() {
		QueryPerformanceFrequency(&frequency);
		text = "";
		index = 0;
	}
	Clock(std::string text) :text(text) {
		QueryPerformanceFrequency(&frequency);
		index = 0;
	}
	void start() {
		QueryPerformanceCounter(&startCounter[++index]);
	}
	void end(std::string text) {
		QueryPerformanceCounter(&stopCounter[index]);
		printf("%s %f\n", text.c_str(), (double)(stopCounter[index].QuadPart - startCounter[index].QuadPart) / frequency.QuadPart);
		index--;
	}
	void end() {
		QueryPerformanceCounter(&stopCounter[index]);
		printf("%s %f\n", text.c_str(), (double)(stopCounter[index].QuadPart - startCounter[index].QuadPart) / frequency.QuadPart);
		index--;
	}
};
// I[i] 第I行第一个非0，在val中的index
/* genTridiag: generate a random tridiagonal symmetric matrix */
void gen(int *I, int *J, float *val, int N, int nz,int X,int Y,int Z)
{
	Clock clock;
	clock.start();
    I[0] = 0, J[0] = 0, J[1] = 1, J[2] = Y, J[3] = Y*Z;
	val[0] = 6;
	val[1] = -1;
	val[2] = -1;
	val[3] = -1;
    int start;
	int k = 4;
	int count = X*Y*Z;
    for (int i = 1; i < N; i++)
    {
		I[i] = k;
		if (i - Y*Z + 1>0) {
			J[k] = i - Y*Z;
			val[k] = -1;
			k++;
		}
		if (i - Y + 1 > 0) {
			J[k] = i - Y;
			val[k] = -1;
			k++;
		}
		if (i > 0) {
			J[k] = i - 1;
			val[k] = -1;
			k++;
		}
		J[k] = i;
		val[k] = 6;
		k++;
		if (i + 1 < count) {
			J[k] = i + 1;
			val[k] = -1;
			k++;
		}
		if (i + Y < count) {
			J[k] = i + Y;
			val[k] = -1;
			k++;
		}
		if (i + Y*Z < count) {
			J[k] = i + Y*Z;
			val[k] = -1;
			k++;
		}
    }

    I[N] = k;
	clock.end("prepare CSR A");
}
// dim 1 dimension 3 windowMin&Max 2 opacity 1 udata 1 Nx,Ny,Nz 
__device__ float getOpacity(float* opacity,float g) {
	return  opacity[int(g * 255)];
}
__device__ float index(DataBlock block,int x, int y, int z) {
	float* udata = block.udata;
	int DIM = block.DIM[0];
	int* dimension = block.dimension;
	float mx = block.max[0];
	float mn = block.min[0];
	float d = udata[DIM*x*dimension[1] * dimension[0] + DIM*y*dimension[0] + DIM*z];
	if (d < mn ) return 0.0;
	if (d > mx) return 1.0;

	return (d - mn) / (mx - mn);
}

__global__ void convection(float *p,DataBlock block) {
	int Nx = block.Nd[0];
	int Ny = block.Nd[1];
	int Nz = block.Nd[2];
	int x = threadIdx.x + blockIdx.x * blockDim.x;
	int y = threadIdx.y + blockIdx.y * blockDim.y;
	int z = threadIdx.z + blockIdx.z * blockDim.z;
	int offset = z + y*Nz + x*Ny*Nz;
	if (offset >= Nx*Ny*Nz) return;
	//float err = 1.0;
	//while(err>0.1) {
		/*if (p[offset] < 0.0) {
			p[offset] = 1.0 - getOpacity(block.opacity, index(block, x, y, z));
			return;
		}*/
	float op = getOpacity(block.opacity, index(block, x, y, z));
	for (int i = 0; i < 25; i++) {
		int right = offset + 1;
		int left = offset - 1;
		int down = offset + Nz; 
		int up = offset - Nz;
		int top = offset + Ny*Nz;
		int bottom = offset - Ny*Nz;
		float ux[] = { 0.577,0.577,0.577 };
		if (x == 0) {
			bottom += Ny*Nz;
		}
		if (x == (Nx - 1)) {
			top -= Ny*Nz;
		}
		if (y == 0) { up += Nz; }
		if (y == (Ny - 1)) { down -= Nz; }
		if (z == 0) { left++; }
		if (z == (Nz - 1)) { right--; }

		float flux_right = 0.0;
		float flux_left = 0.0;
		if (ux[0] < 0.) { flux_right = (p[right] - p[offset]) * ux[0]; }
		else { flux_left = (p[offset] - p[left])* ux[0]; }

		float flux_down = 0.0;
		float flux_up = 0.0;
		if (ux[1] < 0.) { flux_down = (p[down] - p[offset]) * ux[1]; }
		else { flux_up = (p[offset] - p[up])* ux[1]; }

		float flux_top;
		float flux_bottom;
		if (ux[2] < 0.) { flux_top = (p[top] - p[offset]) * ux[2]; }
		else { flux_bottom = (p[offset] - p[bottom])* ux[1]; }
		p[offset] = (1 - op)*(p[offset] - 0.5*(
			flux_right + flux_left + flux_up + flux_down + flux_top + flux_bottom));
		if (p[offset] > 1.0) p[offset] = 1.0;
		if (p[offset] < 0.001) p[offset] = 0.0;
	}
	//	err = -c*(ux[0] * (flux_right + flux_left) + ux[1] * (flux_down + flux_up) + ux[2] * (flux_top + flux_bottom)) - (1 - op)*p[offset];
	//}
}

static bool flag = true;
extern "C"
float* gc(int X, int Y, int Z, float* i_udata, int *dims, int dim, float* i_opacity, float *i_min, float *i_max)
{
	Clock clock;
	static int M = 0, N = 0, nz = 0, *I = NULL, *J = NULL;
	static float *init_bound = NULL;
    float *val = NULL;
    const float tol = 1e-5f;
    const int max_iter = 1;
    float *x;
    float *rhs;
    float a, b, na, r0, r1;
    static int *d_col, *d_row;
	static float *d_val;
	float *d_x, dot;
	static float *d_r;
	float *d_p, *d_Ax;
    int k;
    float alpha, beta, alpham1;
	int Nd[] = { X,Y,Z };
	int count = X*Y*Z;
	static DataBlock block;
	static int *dimension;
	static float *opacity;
	static float *udata;
    /* Generate a random tridiagonal symmetric matrix in CSR format */
    M = N = count;
	clock.start();
	clock.start();
	if (flag) {
		checkCudaErrors(cudaMalloc(&(block.udata), sizeof(float)*dims[0] * dims[1] * dims[2]));
		checkCudaErrors(cudaMalloc(&d_r, sizeof(float)*count));
		checkCudaErrors(cudaMalloc(&(block.dimension), sizeof(int)*3));
		checkCudaErrors(cudaMalloc(&(block.Nd), sizeof(int)*3));
		checkCudaErrors(cudaMalloc(&(block.max), sizeof(float)*4));
		checkCudaErrors(cudaMalloc(&(block.min), sizeof(float)*4));
		checkCudaErrors(cudaMalloc(&(block.DIM), sizeof(int)));
		checkCudaErrors(cudaMalloc(&(block.opacity), sizeof(float)*256));
		checkCudaErrors(cudaMemcpy(block.udata, i_udata, sizeof(float)*dims[0] * dims[1] * dims[2],cudaMemcpyHostToDevice));
		checkCudaErrors(cudaMemcpy(block.dimension,dims, sizeof(int)*3,cudaMemcpyHostToDevice));
		checkCudaErrors(cudaMemcpy(block.Nd,Nd, sizeof(int)*3,cudaMemcpyHostToDevice));
		checkCudaErrors(cudaMemcpy(block.DIM,&dim, sizeof(int),cudaMemcpyHostToDevice));
		//checkCudaErrors(cudaMemcpyToSymbol(&DIM, &dim, sizeof(int)));
		for (int i = 0; i < N; i++)
		{
			if (i - Y*Z + 1 > 0) {
				nz++;
			}
			if (i - Y + 1 > 0) {
				nz++;
			}
			if (i > 0) {
				nz++;
			}
			nz++;
			if (i + 1 < count) {
				nz++;
			}
			if (i + Y < count) {
				nz++;
			}
			if (i + Y*Z < count) {
				nz++;
			}
		}
		init_bound = (float*)malloc(sizeof(float)*N);
		for (int i = 0; i < X; i++)
			for (int j = 0; j < Y; j++)
				for (int k = 0; k < Z; k++)
					if (i == 0 || j == 0 || k == 0)
						init_bound[i*Y*Z + j*Z + k] = 1.0;
					else if (i == X - 1 || j == Y - 1 || k == Z - 1)
						init_bound[i*Y*Z + j*Z + k] = 0.0;
					else
						init_bound[i*Y*Z + j*Z + k] = 0.0;// 333 * (init_bound[(i - 1)*Y*Z + j*Z + k] + init_bound[i*Y*Z + (j - 1)*Z + k] + init_bound[i*Y*Z + j*Z + k - 1]);
		clock.end("count total");
		I = (int *)malloc(sizeof(int)*(N + 1));
		J = (int *)malloc(sizeof(int)*nz);
		val = (float *)malloc(sizeof(float)*nz);
		gen(I, J, val, N, nz, X, Y, Z);
	}
	checkCudaErrors(cudaMemcpy(block.opacity,i_opacity, sizeof(float)*256,cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(d_r, init_bound,sizeof(float)*count,cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(block.min, i_min, sizeof(int),cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(block.max, i_max, sizeof(int),cudaMemcpyHostToDevice));
	//checkCudaErrors(cudaMemcpy(block.idx, &i, sizeof(int),cudaMemcpyHostToDevice));
	int i;
	int tb[] = { 4,8,8 };
	dim3 grid((X+tb[0]-1)/tb[0],(Y+tb[1]-1)/tb[1],(Z+tb[2]-1)/tb[2]);
	dim3 thread(tb[0],tb[1],tb[2]);
	for (i = 0; i < 2 ; i++) {
		convection << <grid, thread>> > ( d_r, block );
		cudaDeviceSynchronize();
	}
	//rhs = (float*)malloc(sizeof(float)*N);
	//cudaMemcpy(rhs, d_r, sizeof(float)*count, cudaMemcpyDeviceToHost);
    x = (float *)malloc(sizeof(float)*N);
	clock.start();
    for (int i = 0; i < N; i++)
    {
        //rhs[i] = data[i];
        x[i] = 0.0;
    }

    /* Get handle to the CUBLAS context */
    cublasHandle_t cublasHandle = 0;
    cublasStatus_t cublasStatus;
    cublasStatus = cublasCreate(&cublasHandle);

    checkCudaErrors(cublasStatus);

    /* Get handle to the CUSPARSE context */
    cusparseHandle_t cusparseHandle = 0;
    cusparseStatus_t cusparseStatus;
    cusparseStatus = cusparseCreate(&cusparseHandle);

    checkCudaErrors(cusparseStatus);

    cusparseMatDescr_t descr = 0;
    cusparseStatus = cusparseCreateMatDescr(&descr);

    checkCudaErrors(cusparseStatus);

    cusparseSetMatType(descr,CUSPARSE_MATRIX_TYPE_GENERAL);
    cusparseSetMatIndexBase(descr,CUSPARSE_INDEX_BASE_ZERO);

	if (flag) {
		checkCudaErrors(cudaMalloc((void **)&d_col, nz * sizeof(int)));
		checkCudaErrors(cudaMalloc((void **)&d_row, (N + 1) * sizeof(int)));
		checkCudaErrors(cudaMalloc((void **)&d_val, nz * sizeof(float)));
		//checkCudaErrors(cudaMalloc((void **)&d_r, N * sizeof(float)));
	}
	checkCudaErrors(cudaMalloc((void **)&d_x, N * sizeof(float)));
    checkCudaErrors(cudaMalloc((void **)&d_p, N*sizeof(float)));
    checkCudaErrors(cudaMalloc((void **)&d_Ax, N*sizeof(float)));

    cudaMemcpy(d_col, J, nz*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_row, I, (N+1)*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_val, val, nz*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_x, x, N*sizeof(float), cudaMemcpyHostToDevice);
    //cudaMemcpy(d_r, rhs, N*sizeof(float), cudaMemcpyHostToDevice);

    alpha = 1.0;
    alpham1 = -1.0;
    beta = 0.0;
    r0 = 0.;
    cusparseScsrmv(cusparseHandle,CUSPARSE_OPERATION_NON_TRANSPOSE, N, N, nz, &alpha, descr, d_val, d_row, d_col, d_x, &beta, d_Ax);
    cublasSaxpy(cublasHandle, N, &alpham1, d_Ax, 1, d_r, 1);
    cublasStatus = cublasSdot(cublasHandle, N, d_r, 1, d_r, 1, &r1);

    k = 1;

    while (r1 > tol*tol && k <= max_iter)
    {
        if (k > 1)
        {
            b = r1 / r0;
            cublasStatus = cublasSscal(cublasHandle, N, &b, d_p, 1);
            cublasStatus = cublasSaxpy(cublasHandle, N, &alpha, d_r, 1, d_p, 1);
        }
        else
        {
            cublasStatus = cublasScopy(cublasHandle, N, d_r, 1, d_p, 1);
        }

        cusparseScsrmv(cusparseHandle, CUSPARSE_OPERATION_NON_TRANSPOSE, N, N, nz, &alpha, descr, d_val, d_row, d_col, d_p, &beta, d_Ax);
        cublasStatus = cublasSdot(cublasHandle, N, d_p, 1, d_Ax, 1, &dot);
        a = r1 / dot;

        cublasStatus = cublasSaxpy(cublasHandle, N, &a, d_p, 1, d_x, 1);
        na = -a;
        cublasStatus = cublasSaxpy(cublasHandle, N, &na, d_Ax, 1, d_r, 1);

        r0 = r1;
        cublasStatus = cublasSdot(cublasHandle, N, d_r, 1, d_r, 1, &r1);
        cudaDeviceSynchronize();
        //printf("iteration = %3d, residual = %e\n", k, sqrt(r1));
        k++;
    }

    cudaMemcpy(x, d_x, N*sizeof(float), cudaMemcpyDeviceToHost);
	clock.end("cal:");
	//error estimate
    /*
	clock.start();
	float rsum, diff, err = 0.0;

    for (int i = 0; i < N; i++)
    {
        rsum = 0.0;

        for (int j = I[i]; j < I[i+1]; j++)
        {
            rsum += val[j]*x[J[j]];
        }

        diff = fabs(rsum - rhs[i]);

        if (diff > err)
        {
            err = diff;
        }
    }
	clock.end("error");
	*/

    cusparseDestroy(cusparseHandle);
    cublasDestroy(cublasHandle);

    //free(I);
    //free(J);
    //free(val);
    //free(rhs);
    //cudaFree(d_col);
    //cudaFree(d_row);
    //cudaFree(d_val);
    cudaFree(d_x);
    //cudaFree(d_r);
    cudaFree(d_p);
    cudaFree(d_Ax);
	clock.end("total time:");
	if (flag) flag = false;
	// printf("Test Summary:  Error amount = %f\n", err);
    //exit((k <= max_iter) ? 0 : 1);
	return x;
}

/*int main() {
	float *udata = (float*)malloc(512 * 512 * 100 * sizeof(float));
	int dim[3] = { 512,512,100 };
	float *o= (float*)malloc(256* sizeof(float));
	gc(256, 256, 50, udata, dim,2,o, 0.0f, 1.0f);
}*/

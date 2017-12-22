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

// Utilities and system includes
#include <helper_functions.h>  // helper for shared functions common to CUDA Samples
#include <helper_cuda.h>       // helper function CUDA error checking and initialization

#include <string>

const char *sSDKname     = "conjugateGradient";
extern "C"
float* gc( float*,int,int,int);
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

extern "C"
float* gc(float* data,int X,int Y,int Z )
{
	Clock clock;
	static bool flag = true;
    static int M = 0, N = 0, nz = 0, *I = NULL, *J = NULL;
    float *val = NULL;
    const float tol = 1e-5f;
    const int max_iter = 1;
    float *x;
    float *rhs;
    float a, b, na, r0, r1;
    int *d_col, *d_row;
    float *d_val, *d_x, dot;
    float *d_r, *d_p, *d_Ax;
    int k;
    float alpha, beta, alpham1;
	int count = X*Y*Z;
    /* Generate a random tridiagonal symmetric matrix in CSR format */
    M = N = count;
	clock.start();
	clock.start();
	if (flag) {
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
		clock.end("count total");
		I = (int *)malloc(sizeof(int)*(N + 1));
		J = (int *)malloc(sizeof(int)*nz);
		val = (float *)malloc(sizeof(float)*nz);
		gen(I, J, val, N, nz, X, Y, Z);
	}

    x = (float *)malloc(sizeof(float)*N);
    rhs = (float *)malloc(sizeof(float)*N);
	clock.start();
    for (int i = 0; i < N; i++)
    {
        rhs[i] = data[i];
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

    checkCudaErrors(cudaMalloc((void **)&d_col, nz*sizeof(int)));
    checkCudaErrors(cudaMalloc((void **)&d_row, (N+1)*sizeof(int)));
    checkCudaErrors(cudaMalloc((void **)&d_val, nz*sizeof(float)));
    checkCudaErrors(cudaMalloc((void **)&d_x, N*sizeof(float)));
    checkCudaErrors(cudaMalloc((void **)&d_r, N*sizeof(float)));
    checkCudaErrors(cudaMalloc((void **)&d_p, N*sizeof(float)));
    checkCudaErrors(cudaMalloc((void **)&d_Ax, N*sizeof(float)));

    cudaMemcpy(d_col, J, nz*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_row, I, (N+1)*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_val, val, nz*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_x, x, N*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_r, rhs, N*sizeof(float), cudaMemcpyHostToDevice);

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
        printf("iteration = %3d, residual = %e\n", k, sqrt(r1));
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
    free(rhs);
    cudaFree(d_col);
    cudaFree(d_row);
    cudaFree(d_val);
    cudaFree(d_x);
    cudaFree(d_r);
    cudaFree(d_p);
    cudaFree(d_Ax);
	clock.end("total time:");
	// printf("Test Summary:  Error amount = %f\n", err);
    //exit((k <= max_iter) ? 0 : 1);
	return x;
}

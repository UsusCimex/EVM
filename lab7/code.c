#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <math.h>
#include <time.h>
#include <immintrin.h>
#define N 1024
#define M 10 //count iteration

//default Inverse
void multiplyMatrix(const float* A, const float* B, float* RES)
{
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++)
        {
            RES[i*N + j] = 0;
            for(int k = 0; k < N; k++)
                RES[i*N + j] += A[i*N + k] * B[k*N + j];
        }
}

void sumMatrix(float* A, float* B, float* RES, char sign)
{
    if (sign)
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                RES[i*N + j] = A[i*N + j] + B[i*N + j];
    else
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                RES[i*N + j] = A[i*N + j] - B[i*N + j];
}

void copyMatrix(float* A, float* B)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            A[i*N + j] = B[i*N + j];
}

float defaultMatrixInverse(float* matrix, float* result)
{
    clock_t c_start = clock();
    float* I = malloc(sizeof(float) * N * N);
    float* buff = malloc(sizeof(float) * N * N);
    float* B = malloc(sizeof(float) * N * N);
    float* BA = malloc(sizeof(float) * N * N);
    float* tmp = malloc(sizeof(float) * N * N);
    float* R = malloc(sizeof(float) * N * N);
    
    float maxRowSum = 0;
    float maxColumnSum = 0;
    float rowSum = 0;
    float columnSum = 0;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            rowSum += fabs(matrix[N * i + j]);
            columnSum += fabs(matrix[j * N + i]);

            I[N * i + j] = (i == j);
            buff[N * i + j] = (i == j);
        }
        if (columnSum > maxColumnSum)
        {
            maxColumnSum = columnSum;
        }
        if (rowSum > maxRowSum)
        {
            maxRowSum = rowSum;
        }
        rowSum = 0;
        columnSum = 0;
    }
    float max = maxColumnSum * maxRowSum;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            B[N * i + j] = matrix[j * N + i] / max;
        }
    }
    
    multiplyMatrix(B, matrix, BA); //BA = B * MATRIX
    sumMatrix(I, BA, R, 0); //R = I - BA
    for (size_t i = 0; i < M-1; i++)
    {
        multiplyMatrix(I, R, tmp); // tmp = I * R
        copyMatrix(I, tmp); // I = tmp
        sumMatrix(buff, I, buff, 1); //buff = buff + I
    }
    multiplyMatrix(buff, B, result); //result = buff * B
    clock_t c_end = clock();
    float totalTime = 1000.0 * ((float)(c_end - c_start) / CLOCKS_PER_SEC);
    free(I);
    free(B);
    free(BA);
    free(tmp);
    free(R);
    free(buff);
    return totalTime;
}

//SSE Inverse | Streaming SIMD Extensions
void mulSSE(const float* A, const float* B, float* RES)
{
    __m256 line; //8 float vector
    __m256 row;
    __m256 temp;
    __m256 res;
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; j += 8)
        {
            res = _mm256_setzero_ps(); //8 float zero vector
            for (int k = 0; k < N; ++k)
            {
                row = _mm256_set1_ps(A[i * N + k]); //8 float =1 vector
                line = _mm256_load_ps(B + (k * N) + j); //Load 8 float bytes from parameters
                temp = _mm256_mul_ps(row, line); //multiply 2 vector
                res = _mm256_add_ps(res, temp); //Adds 2 vector
            }
            _mm256_store_ps(RES + i * N + j, res); //Store 8 float bytes from 2 param to 1 param
        }
    }
}

void sumSSE(float* A, float* B, float* RES, char sign)
{
    __m256 p;
    __m256* xx;
    __m256* yy;
    for (int i = 0; i < N; i++)
        {
        xx = (__m256*)(A + i*N);
        yy = (__m256*)(B + i*N);
        for (int k = 0; k < N / 8; k++)
        {
            if (sign)
                p = _mm256_add_ps(xx[k], yy[k]);
            else
                p = _mm256_sub_ps(xx[k], yy[k]);
            _mm256_store_ps(RES + i*N + k*8, p);
        }
    }
}

float SSEMatrixInverse(float* matrix, float* result)
{
    clock_t c_start = clock();
    float* I = _mm_malloc(sizeof(float) * N * N, 64);
    float* buff = _mm_malloc(sizeof(float) * N * N, 64);
    float* B = _mm_malloc(sizeof(float) * N * N, 64);
    float* BA = _mm_malloc(sizeof(float) * N * N, 64);
    float* tmp = _mm_malloc(sizeof(float) * N * N, 64);
    float* R = _mm_malloc(sizeof(float) * N * N, 64);

    float maxRowSum = 0;
    float maxColumnSum = 0;
    float rowSum = 0;
    float columnSum = 0;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            rowSum += fabs(matrix[N * i + j]);
            columnSum += fabs(matrix[j * N + i]);
            I[N * i + j] = (i == j);
            buff[N * i + j] = (i == j);
        }
        if (columnSum > maxColumnSum)
        {
            maxColumnSum = columnSum;
        }
        if (rowSum > maxRowSum)
        {
            maxRowSum = rowSum;
        }
        rowSum = 0;
        columnSum = 0;
    }
    float max = maxColumnSum * maxRowSum;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            B[N * i + j] = matrix[j * N + i] / max;
        }
    }

    mulSSE(B, matrix, BA);
    sumSSE(I, BA, R, 0);
    for (size_t i = 0; i < M - 1; i++)
    {
        mulSSE(I, R, tmp);
        copyMatrix(I, tmp);
        sumSSE(buff, I, buff, 1);
    }
    mulSSE(buff, B, result);
    clock_t c_end = clock();
    float totalTime = 1000.0 * ((float)(c_end - c_start) / CLOCKS_PER_SEC);
    _mm_free(I);
    _mm_free(B);
    _mm_free(BA);
    _mm_free(tmp);
    _mm_free(R);
    _mm_free(buff);
    return totalTime;
}

//BlasInverse | Basic Linear Algebra Subroutines
float BLASMatrixInverse(float* matrix, float* result)
{
    clock_t c_start = clock();
    float* I = malloc(sizeof(float) * N * N);
    float* buff = malloc(sizeof(float) * N * N);
    float* B = malloc(sizeof(float) * N * N);
    float* BA = malloc(sizeof(float) * N * N);
    float* tmp = malloc(sizeof(float) * N * N);
    float* R;

    float maxRowSum = 0;
    float maxColumnSum = 0;
    float rowSum = 0;
    float columnSum = 0;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            rowSum += fabs(matrix[N * i + j]);
            columnSum += fabs(matrix[j * N + i]);

            I[N * i + j] = (i == j);
            buff[N * i + j] = (i == j);
        }
        if (columnSum > maxColumnSum)
        {
            maxColumnSum = columnSum;
        }
        if (rowSum > maxRowSum)
        {
            maxRowSum = rowSum;
        }
        rowSum = 0;
        columnSum = 0;
    }
    float max = maxColumnSum * maxRowSum;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            B[N * i + j] = matrix[j * N + i] / max;
        }
    }

    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0, B, N, matrix, N, 0.0, BA, N); //Multiplies two matrices
    cblas_saxpy(N * N, -1.0, I, 1.0, BA, 1.0); //Computes a constant times a vector plus a vector (single-precision).
    cblas_sscal(N * N, -1.0, BA, 1.0); //Multiplies each element of a vector by a constant (single-precision).
    R = BA;
    for (size_t i = 0; i < M - 1; i++)
    {
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0, I, N, R, N, 0.0, tmp, N);
        copyMatrix(I, tmp);
        cblas_saxpy(N * N, 1.0, I, 1.0, buff, 1.0);
    }
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0, buff, N, B, N, 0.0, result, N);
    clock_t c_end = clock();
    float totalTime = 1000.0 * ((float)(c_end - c_start) / CLOCKS_PER_SEC);
    free(I);
    free(B);
    free(BA);
    free(tmp);
    free(buff);
    return totalTime;
}

int main()
{
    float* matrix = malloc(sizeof(float) * N * N);
    float* inversedMatrix = malloc(sizeof(float) * N * N);
    
    for (int k = 0; k < N; ++k)
    {
        for (int i = 0; i < N; ++i)
        {
            matrix[k * N + i] = rand() % 10;
            inversedMatrix[k * N + i] = 0;
        }
    }
    
    float res = defaultMatrixInverse(matrix, inversedMatrix);
    printf("Default Inversion: %f ms\n", res);

    float* dMatrix = _mm_malloc(sizeof(float) * N * N, 64);
    float* inversedDMatrix = _mm_malloc(sizeof(float) * N * N, 64);
    res = SSEMatrixInverse(dMatrix, inversedDMatrix);
    printf("SSE Inversion: %f ms\n", res);

    res = BLASMatrixInverse(matrix, inversedMatrix);
    printf("BLAS Inversion: %f ms\n", res);
    free(matrix);
    free(inversedMatrix);
    return 0;
}

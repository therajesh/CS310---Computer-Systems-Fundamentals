/*
    Rajesh Narayan
    narayar@whitman.edu
*//* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void transpose32(int M, int N, int A[N][M], int B[M][N]);
void transpose64(int M, int N, int A[N][M], int B[M][N]);
void transpose61(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    switch(M) {
        case 32:
            transpose32(M,N,A,B);
            break;
        case 64:
            transpose64(M,N,A,B);
            break;
        case 61:
            transpose61(M,N,A,B);
            break;
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

void transpose32(int M, int N, int A[N][M], int B[M][N]) {

    int blocksize = 8;
    int i, j, k, l;

    int diagval, diagind; // diagonal value and diagonal index

    for (i = 0; i < N; i += blocksize) {
        for (j = 0; j < M; j += blocksize) {
            // transpose the block beginning at [i,j]
            for (k = i; k < i + blocksize; ++k) {
                for (l = j; l < j + blocksize; ++l) {
                    if (k != l) {
                        B[l][k] = A[k][l];
                    }
                    else {
                        diagval = A[k][l];
                        diagind = k;
                    }
                }
                if(i == j) {
                    B[diagind][diagind] = diagval;
                }
            }
        }
    }
}

void transpose64(int M, int N, int A[N][M], int B[M][N]) {
    // temporarily moving the upper and lower 4x8 blocks to unused blocks in b, and these shouldn't have any conflicts.
    //going on. we can transpose the two blocks to the target block in b, and since they're in the cache they shouldn't have any misses. we can transpose q1 and q4 to the upper and lower 4x8 blocks respectively
    //the target matrix also has sequential moves within itself, so when used there can be fewer conflicts, and we can then work on the temporary data.
    int blocksize = 8;
    int gap = 4;
    int i,j;
    // iterating over y axis then x axis
    for (j = 0; j < M; j += blocksize) {
        for (i = 0; i < N; i += blocksize){
            int xval = j, nx = j;
            int yval = i, ny = i;

            do{ 
                ny += blocksize;
                if (ny >= N){
                    nx += blocksize;
                    ny -= N;
                }
            } while (j == ny);

            int nx2 = nx;
            int ny2 = ny;

            do{ 
                ny2 += blocksize;
                if (ny2 >= N){
                    nx2 += blocksize;
                    ny2 -= N;				
                }
            } while (j == ny2);

            if (nx >= M){
                for (int k = 0; k < blocksize; k++)
                    for (int l = 0; l < blocksize; l++)
                        B[xval + l][yval + k] = A[i + k][j + l];	
            } else {
                for (int k = 0; k < gap; k++)
                    for (int l = 0; l < blocksize; l++)
                        B[nx + k][ny + l] = A[i + k][j + l];			
                for (int k = 0; k < gap; k++)
                    for (int l = 0; l < blocksize; l++)
                        B[nx2 + k][ny2 + l] = A[i + gap + k][j + l];

                for (int k = 0; k < gap; k++)
                    for (int l = 0; l < gap; l++){
                        B[xval + l][yval + k] = B[nx + k][ny + l];
                        B[xval + l][yval + gap + k] = B[nx2 + k][ny2 + l];	
                    }

                for (int k = 0; k < gap; k++)
                    for (int l = 0; l < gap; l++){
                        B[xval + gap + l][yval + k] = B[nx + k][ny + gap + l];
                        B[xval + gap + l][yval + gap + k] = B[nx2 + k][ny2 + gap + l];	
                    }			
            }
        }
    }
}

/*
    int i, j, k, l;
    int loop = 0;
    int blocksize = 8;
    int diagind, diagval;
    while (loop < 4)
    {
        switch (loop)
        {
            case 0:
                N = M = 32;
                break;
            case 1:
                //N = M = 64;
                i += 32;
                break;
            case 2:
                N = M = 32;
                i -= 32;
                j += 32;
                break;
            case 3:
                //N = M = 64;
                i += 32;
                break;
        }

        for (i = 0; i < N; i += blocksize)
        {
            for (j = 0; j < M; j += blocksize)
            {
                // transpose the block beginning at [i,j]
                for (k = i; k < i + blocksize; ++k)
                {
                    for (l = j; l < j + blocksize; ++l)
                    {
                        if (k != l)
                        {
                            B[l][k] = A[k][l];
                        }
                        else
                        {
                            diagval = A[k][l];
                            diagind = k;
                        }
                    }
                    if (i == j)
                    {
                        B[diagind][diagind] = diagval;
                    }
                }
            }
        }
        loop ++;
    }
*/
/*
    int blocksize = 8;

    // loop variables to pass through rows and columns 
    int i, j; 

    // loop variables to pass through rows and columns of blocks
    int k, l; 

    // if a block's diagonal is part of the matrix's diagonal, we don't have to alter it. assign the number to a variable so that we have quick access

    int diagval, diagind; // diagonal element and diagonal index

    int tmp;
    for (i = 0; i < N; i += blocksize) {
        for (j = 0; j < M; j += blocksize) {
            // transpose the block beginning at [i,j]
            for (k = i; k < i + blocksize; ++k) {
                for (l = j; l < j + blocksize; ++l) {
                    if (k != l) {
                        B[k][l] = A[k][l];
                    }
                    else {
                        diagval = A[k][l];
                        diagind = k;
                    }
                }
                if(i == j) {
                    B[diagind][diagind] = diagval;
                }
            }
        }
    }

    for (i = 0; i < N; i += blocksize) {
        for (j = i+1; j < M; j+= blocksize) {
            for (k = i; k < i + blocksize; ++k) {
                    for (l = j; l < j + blocksize; ++l) {
                        if (k != l) {
                            tmp = B[k][l];
                            B[k][l] = B[l][k];
                            B[l][k] = tmp;
                        }
                        else {
                            diagval = B[k][l];
                            diagind = k;
                        }
                    }
                    if ( i == j ) B[diagind][diagind] = diagval;
                }
        } 
    } */

void transpose61(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, k, l;
    int tmp;
    int blocksize = 17;
    for (i = 0; i < N; i += blocksize) {
        for (j = 0;  j < M; j += blocksize) {
            for (k = i; k < i + blocksize; k++) {
                if (k < N) {
                    for (l = j; l < j + blocksize; l++) {
                        if (l < M) {
                            tmp = A[k][l];
                            B[l][k] = tmp;
                        }
                    }
                }
            }
        }
    }
}


/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}


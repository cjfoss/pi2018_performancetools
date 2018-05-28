# include <stdlib.h>
# include <stdio.h>
# include <math.h>
#include <sys/time.h>
// Global variables
double **matX, **matY, **matZ;
struct timeval start_time, stop_time, elapsed_time;  // timers

// Functions Declaration
// allocMem wrapper to allocate memory for each matrix
void allocMem();
// freeMem wrapper to take back allocated memory
void freeMem();

//matrix multiply naive version
double matrix_mult_naive(double** matX, double** matY, double** matZ);

//matrix multiply tiling version
double matrix_mult_tiling(double** matX, double** matY, double** matZ);

//print test 
void printMat();

#define NUM_ROW 1500   //Number of rows in each matrix
#define NUM_COL 1500   //Number of column in each matrix

int main (int argc, char **argv){
    
    matX=matY=matZ=NULL;

    allocMem();
    
    int matType;
    //double start_t, end_t, compute_t = 0.0;
    printf ("Compute matrix product Z = X * Y.\n" );
    printf("  How do you want to compute the matrix\n"
            "  enter [1] for Naive, or [2] for tiling\n");
    scanf("%d", &matType);
    switch(matType)
    {
        case 1:
            matrix_mult_naive(matX, matY, matZ);
            break;
        case 2:
            matrix_mult_tiling(matX, matY, matZ);
            break;
        default:
            printf("Please enter either 'n' or 't' \n");
            exit(1);
    }
    printMat();
    //Call function to free memory allocated for each matrix
    freeMem();
    return 0;
} //END: main()

//Naieve way of matrix multiply
double matrix_mult_naive(double** matX, double** matY, double** matZ){
    int i, j, k;
    printf("|---This is naive matrix multiply---|\n");
    //Initialize matA basically each element is the sum of index i+j.
// Directive inserted by Cray Reveal.  May be incomplete.
#pragma omp parallel for default(none)                                   \
        private (i,j)                                                    \
        shared  (matX)
    for ( i = 0; i < NUM_ROW; i++ ){
        for ( j = 0; j < NUM_COL; j++ ){
            matX[i][j] = 1;
        }
    } //END: outerloop
    //Initialize matB basically to product of indicies for each element.
    for ( i = 0; i < NUM_ROW; i++ ){
        for ( j = 0; j < NUM_COL; j++ )
        {
            matY[i][j] = 2;
        }
    } //END: outerloop
    
    //start timer here
    gettimeofday(&start_time,NULL);         //start time
    // Compute matSum = matA * matB.
    for ( i = 0; i < NUM_ROW; i++ ){
        for ( j = 0; j < NUM_COL; j++ ){
            matZ[i][j] = 0.0;
            for ( k = 0; k < NUM_ROW; k++ ){
                //Actuall multiplication here.
                matZ[i][j] = matZ[i][j] + matX[i][k] * matY[k][j];
            }
        }
    } //END: outerloop
    
    //stop timer and calc time taken
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time);
    printf("||==Total time was %f seconds.==||\n", 
            elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);

    return (0);
} //END: matrix_mult_naive()

//matrix multiply with tiling
double matrix_mult_tiling(double** matX, double** matY, double** matZ){
    int row, col, prod;
    printf("|--This is matrix Multiply by tiling--|\n");
    int t_r, t_c, t_prod;
    int total_bytes;
    int element_per_tile, tile_bytes;
    int GB = 1024 * 1024 * 1024;
    int MB = 1024 * 1024;
    int KB = 1024;
    int block_size = 362;//682;
    //total_bytes = (row_start + row_end) * (col_start + col_end);
    total_bytes = NUM_ROW * NUM_COL * sizeof(double);
    //total_bytes = total_bytes * sizeof(double);

    tile_bytes = block_size * sizeof(double);
  
    printf("\ttotal_bytes = %d\n", total_bytes);
    printf("\ttile_bytes = %d \n", tile_bytes);
    
    gettimeofday(&start_time,NULL);         //start time
    //Initialize matA basically each element is the sum of index i+j.
    for ( row = 0; row < NUM_ROW; row++ ){
// Directive inserted by Cray Reveal.  May be incomplete.
        for ( col = 0; col < NUM_COL; col++ ){
            matX[row][col] = 1;
        }
    } //END: outerloop
    //stop timer
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time);
    printf("|--Total time for column major is: %f seconds.--|\n", 
            elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    
    gettimeofday(&start_time,NULL);
    //Initialize matB basically to product of indicies for each element.
    for ( row = 0; row < NUM_ROW; row++ ){
        for ( col = 0; col < NUM_COL; col++ )
        {
            matY[row][col] = 2;
        }
    } //END: outerloop
    //stop timer
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time);
    printf("|--Total time for row major: %f seconds.--|\n", 
            elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    
    // Start timer
    gettimeofday(&start_time,NULL);         //start time
    // Compute matSum = matA * matB.
    //tiling loops
    for (t_r = 0; t_r< NUM_ROW; t_r = t_r + block_size){
        for (t_c =0; t_c< NUM_COL; t_c = t_c + block_size){
            for (t_prod = 0; t_prod<NUM_COL; t_prod = t_prod + block_size){
                for (row = t_r; row < fmin(NUM_COL, t_r+block_size); row++){
                    for (col = t_c; col < fmin(NUM_COL, t_c+block_size); col++){
                        for (prod = t_prod; prod < fmin(NUM_COL, t_prod+block_size); prod++){
                            matZ[row][col] = matZ[row][col] + matX[row][prod] * matY[prod][col];
                        } // end of inner loopp
                    }   // end of fifth loop
                }   // end of fourth loop
            } // end of thread loop
        } // end of second loop
    } //end of first outer loop
    
    //stop timer
    gettimeofday(&stop_time,NULL);
    timersub(&stop_time, &start_time, &elapsed_time);
    printf("||==Total time was %f seconds.==||\n", 
            elapsed_time.tv_sec+elapsed_time.tv_usec/1000000.0);
    
    return (0);

} //END: matrix_mult_tiling()

//Allocate Memory to each matrix
void allocMem(){
    int i;
    matX = (double **)malloc(NUM_ROW*sizeof(double *));
    for (i=0;i<NUM_ROW; i++){
        matX[i]=(double *)malloc(NUM_COL*sizeof(double));
    }

    matY = (double **)malloc(NUM_ROW*sizeof(double *));
    for (i=0;i<NUM_ROW; i++){
        matY[i]=(double *)malloc(NUM_COL*sizeof(double));
    }
    matZ = (double **)malloc(NUM_ROW*sizeof(double *));
    for (i=0;i<NUM_ROW; i++){
        matZ[i]=(double *)malloc(NUM_COL*sizeof(double));
    }
    //return 0;

} //END: allocMem() 

void printMat(){
    int i, j;
    printf("Computed first 6 rows are:\n");
    //printf("matSum(i,j)  = %8.5f\n", matSum[10][10] );
    for(i=0; i < 10; i++){
        printf(" \n");
        for(j=0; j< 6; j++){
            printf("  %.1f ", matZ[i][j] );
        }
    }
    printf(" \n");
}
//Free allocated memory to two dimentional arrays
void freeMem(){
    int i;
    for (i=NUM_ROW-1; i>=0; i--){
        free(matX[i]);
    }
    free(matX);
    for (i=NUM_ROW-1; i>=0; i--){
        free(matY[i]);
    }
    free(matY);
    for (i=NUM_ROW-1; i>=0; i--){
        free(matZ[i]);
    }
    free(matZ);
    //return 0;
} //END: freeMem()

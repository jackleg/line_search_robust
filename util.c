/******************************************************
 *                                                    *
 * btutil.c : Bradley-Terry Model utility.c           *
 *                                                    *
 * Sangho Lee                                         *
 *                                                    *
 ******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include "Smemory.h"

//////////////////////////////////////////////////////////////

// sort matrix by the first column

void sort_matrix(double **in_matrix, int height, int width) 
{
	int      i , j , idx ; 
	double  **workmatrix ;
    double  *workvector  ; // for sorting target vector
	int     *workvectIdx ;
	extern void merge_sortidx() ;

	workmatrix  = (double **) SmatrixDouble(height, width)    ;
	workvectIdx = (int *)     malloc(sizeof(int) * height)    ;
	workvector  = (double *)  malloc(sizeof(double) * height) ;

	// copy the matrix to workmatrix
	for (i = 0 ; i < height ; ++i) {
	    for (j = 0 ; j < width ; ++j)
		workmatrix[i][j] = in_matrix[i][j]  ;
	    workvector[i]    = workmatrix[i][0] ; // List Set Index
	    workvectIdx[i]   = i ;
	}

	// quick sorting workvectIdx
    merge_sortidx(workvector, workvectIdx, 0, height - 1) ;

	// copy workmatrix to in_matrix
	for (i = 0 ; i < height ; ++i) {
	    idx = workvectIdx[i] ;
	    for (j = 0 ; j < width ; ++j)
            in_matrix[i][j] = workmatrix[idx][j] ; 
	}

	// free memory
	free(workmatrix)  ;
	free(workvectIdx) ;
	free(workvector)  ;

#ifdef DEBUG
    for (i = 0 ; i < height ; ++i)  {
        for (j = 0 ; j < width ; ++j)
            printf("%f ", in_matrix[i][j]) ;
        printf("\n") ;
    }
#endif

}

//////////////////////////////////////////////////////////////

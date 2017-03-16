#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "Smemory.h"
#ifndef __STDC__
#define __DATE__ "1996. 12. 25"
#define __TIME__ "00:00:00"
#endif


void HowTo()
{
	printf("HowTo Page.\n") ;
	printf("the first column should have an index to list.\n") ;
	printf("the first feature begins at the second column.\n") ;
}


int GetUniqueList( double **matrix, int *qstart, int nRecord, int nlist, int nvars)
{
	int    idx , cnt ;
	double predidx ;
	int		max_len;
	int		len;


	//int	stidx, endidx, idx2;

	// Fill qstart[]
	max_len = 0;
	len = 0;
	cnt       = 0 ;
	qstart[0] = 0 ;
	predidx   = matrix[0][0] ;
	for (idx = 1 ; idx < nRecord ; ++idx) {
		len++;
		if (matrix[idx][0] != predidx) {
			qstart[++cnt] = idx ;
			predidx = matrix[idx][0] ;

			if( max_len < len ) max_len = len;
			len = 0;
		}
	}
	qstart[++cnt] = idx ;
   	if( max_len < len ) max_len = len;

	if (cnt != nlist) {
		fprintf(stderr,"Error in GetUniqueList\n") ;
		fprintf(stderr,"cnt : %d, nlist : %d\n", cnt, nlist) ;
		exit(1) ;
	}

	return max_len;

}


int GetNList(double **matrix, int nRecord)
{
    int    idx, cnt ;
    double predidx ;

    cnt = 0 ;
    predidx = -1000 ;
    for (idx = 0 ; idx < nRecord ; ++idx) {
        if (matrix[idx][0] != predidx) {
            ++cnt ;
            predidx = matrix[idx][0] ;
        }
    }

    return cnt ;
}


void BT_ReadMatrix(double **matrix, char *inputfile, int nRecord, int nFeats)
{
    FILE *fptr ; 
    int idx, idx2 ;
	double	target;

    if ((fptr = fopen(inputfile,"rt")) == NULL) {
        fprintf(stderr,"Error in opening %s\n", inputfile) ;
        exit(1) ;
    }

    for (idx = 0 ; idx < nRecord ; ++idx) {
        for (idx2 = 0 ; idx2 < nFeats ; ++idx2)
            fscanf(fptr,"%lf", &matrix[idx][idx2]) ;
        fscanf(fptr,"%lf", &target) ;
        matrix[idx][idx2] = target ;
    }

    fclose(fptr) ;
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

void cut_tail(char *str)
{
	int idx ; 
	for (idx = strlen(str) - 1 ; idx >= 0 ; --idx) {
		if (str[idx] == '\n' || str[idx] == '\r' || 
                str[idx] == ' ') str[idx] = '\0' ;
		else break ;
	}
}

// Given the name of a file of numeric data, this routine counts
// the numbers of rows and columns. It's assumed that the number
// of entries is the same in each row, and an error is flagged if this
// is not the case.
//

void file_size (char *file_name, int *width, int *height)
{
    FILE *f;
    int buf_size = 600, _width = 0;
    char *buffer, *ptr;

    *width = *height = 0;

    buffer = (char*) malloc (buf_size * sizeof (char));
    if (buffer == NULL) {
        fprintf(stderr,"Malloc Error %s %d\n",__FILE__, __LINE__) ;
        exit(1) ;
    }

    /* Open price file - abort if filename invalid */
    f = fopen(file_name, "r");
    if (f == NULL) {
        fprintf(stderr,"\n File not found : %s\n", file_name) ;
        fprintf(stderr,"Error : %s %d\n", __FILE__, __LINE__) ;
		return ;
    }

    /* Get number of entries in first row */
    if (fgets(buffer, buf_size, f) != NULL) {
        cut_tail(buffer) ;

        ++*height;
        ptr = strtok (buffer, "\t ,");
        while (ptr != NULL) {
            ++*width;
            ptr = strtok (NULL, "\t ,");
        }
    }

    /* Count numbers of subsequent rows */
    while (!feof(f)) {
        if (fgets(buffer, buf_size, f) != NULL) {
            cut_tail(buffer) ;
            if (strlen(buffer) > strlen("\n")) { /* if line is more than a NL char */
                if (buffer[0] == '#')
                    continue;
            
                ++*height;
                _width = 0;
                ptr = strtok (buffer, "\t ,");
                while (ptr != NULL) {
                    ++_width;
                    ptr = strtok (NULL, "\t ,");
                }

                if (*width != _width) {
                    printf("\n Number of entries in file %s did not agree", file_name);
                    fprintf(stderr,"\n Error : %s %d\n", __FILE__, __LINE__) ;
                    exit(1) ;
                }
            }
        }
    }
    fclose(f) ; 
    free (buffer);
}

void WriteBeta(double beta[], int nFeats, char *parafile)
{
    FILE *fptr ;
    int idx ;

    if ((fptr = fopen(parafile,"wt")) == NULL) {
        fprintf(stderr,"Can't write %s\n", parafile) ;
        exit(1) ;
    }

    for (idx = 0 ; idx < nFeats ; ++idx) 
        fprintf(fptr,"%.10f\n", beta[idx]) ;

    fclose(fptr) ;
}

void Write_Candidate_Model( char *parafile, int *ncls, double **cls_measure, double **mean, int nDim, int NUM_VQ, int npos)
{
	FILE	*fptr;
	char	fname[256];
	int		t_idx;
	int		idx;
	int		idx3;
	double	tmpf;

	sprintf(fname,"%s_cand",parafile);

    if ((fptr = fopen(fname,"wt")) == NULL) {
        fprintf(stderr,"Can't write %s\n", fname) ;
        exit(1) ;
    }

	fprintf(fptr,"Idx\tnum_element\t");
	fprintf(fptr,"ndcg%-6d\t",npos+1 );
	fprintf(fptr,"cg%-6d\t",npos+1 );
	fprintf(fptr,"prec%-6d\t",npos+1 );

	for(t_idx = 0; t_idx < nDim; t_idx++){
		fprintf(fptr,"w%-10d\t",t_idx+1);
	}
	fprintf(fptr,"\n");
	for(t_idx = 0; t_idx < NUM_VQ; t_idx++){
		fprintf(fptr,"c%d\t%8d\t",t_idx, ncls[t_idx] );
		for(idx = 0; idx < 3; idx++){
			fprintf(fptr,"%-6f\t",cls_measure[ t_idx ][idx] );
 		}
		tmpf = fabs(mean[t_idx][ 0 ]);
		for( idx3 = 0; idx3 < nDim; idx3++){
			fprintf(fptr,"%f\t", mean[t_idx][idx3] / tmpf);
		}
		fprintf(fptr,"\n");
	}

	fclose(fptr);
}

void WriteBeta_tmp(double beta[], int nFeats, char *parafile, int num)
{
    FILE *fptr ;
    int idx ;

	char fname[256];

	sprintf(fname,"%s_%d",parafile, num);

    if ((fptr = fopen(fname,"wt")) == NULL) {
        fprintf(stderr,"Can't write %s\n", fname) ;
        exit(1) ;
    }

    for (idx = 0 ; idx < nFeats ; ++idx) 
        fprintf(fptr,"%.10f\n", beta[idx]) ;

    fclose(fptr) ;
}
    
////////////////////////////////////////////////////////////////////////

void ReadBeta(double beta[], int nFeats, char parafile[])
{
    FILE *fptr ;
    int idx ;

    if ((fptr = fopen(parafile,"rt")) == NULL) {
        fprintf(stderr,"Error in opening %s\n", parafile) ;
        exit(1) ;
    }

    for (idx = 0 ; idx < nFeats ; ++idx) 
        fscanf(fptr,"%lf", &beta[idx]) ;

    fclose(fptr) ;
}

void EstiLNStrength(int ngroups, double **matrix, int nfeats, double *beta, char *outputfile) {
    int    idx, idx2    ;
    double expsum ;

	FILE	*fptr;


	fptr = Fopen( outputfile, "wt");

    for (idx = 0 ; idx < ngroups ; ++idx) {
        expsum = 1.0 ;
        for (idx2 = 1 ; idx2 < nfeats ; ++idx2)
              expsum += beta[idx2]*matrix[idx][idx2] ;

		fprintf(fptr,"%d %f %.10f\n", (int) matrix[idx][0], matrix[idx][nfeats], expsum) ;
    }

	fclose(fptr);
}




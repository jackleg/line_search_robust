#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include "Smemory.h"
#include "types.h"

#ifndef __STDC__
#define __DATE__ "1996. 12. 25"
#define __TIME__ "00:00:00"
#endif


static int nCutNum;
static int *in_idx;
static int *out_idx;
static double   *output_value;
static double   *toScore;
static int		g_idx, child, parent;
static double	item;
static int		itemwid;
static int		g_nCnt;

#define DEMOTE(heap, xtra, size, parent)\
	item    = heap[parent] ;\
	itemwid = xtra[parent] ;\
	while ((child = (parent << 1) + 1) < size) {\
		if (child + 1 < size && heap[child] < heap[child+1]) ++child ;\
		if (item < heap[child]) {\
			heap[parent] = heap[child] ;\
			xtra[parent] = xtra[child] ;\
			parent       = child       ;\
		} else break ;\
	}\
	heap[parent] = item    ;\
	xtra[parent] = itemwid ;\

#define  HeapSort(score, toscr, wordid, topwid, nTotalList,  nTopList) \
	if ( nTotalList < 2 ) {       \
		topwid[0] = wordid[0] ; \
		toscr[0] = score[0]; \
	}\
	else{\
		for (g_idx = (nTotalList >> 1) - 1 ; g_idx >= 0 ; --g_idx) {\
			parent = g_idx ;\
			DEMOTE(score, wordid, nTotalList, parent)\
		}\
		for (g_nCnt = 0, g_idx = nTotalList - 1 ; g_idx > nTotalList - nTopList - 1 ; --g_idx) {\
			\
			toscr[g_nCnt] = score[0] ; \
			topwid[g_nCnt++] = wordid[0] ; \
			\
			score[0]  = score[g_idx]  ; \
			wordid[0] = wordid[g_idx] ; \
			\
			parent = 0 ;\
			DEMOTE(score, wordid, g_idx, parent) \
		}\
	}

static double calc_disto(double **x, double **codeword, int *x_class_idx, int nsample, int dim_vec, int ncode);
static void mk_codeword(double **x, double **codeword, int *x_class_idx, int total_nframe, int dim_vec, int current_ncodeword);
static int calc_centroid(double *center, double **x, int *class, int class_idx, int nsample, int dim);

static int	vq_lbg( double **mean, int jmax, int ntotal , int norder, double **vector);
static double	variance(double *xi,double *yi,int norder);

int vector_quantize( srch_point_s **sr_p, int nData , double **mean, int *ncls)
{
	int i, l, m;
	int ntotal;
	int ndim;
	double **vector;
	double dist;
	double	min_dist;
	int	min_idx;

	ndim = sr_p[0]->nDim;

	vector = Dmat( nData, ndim );

	ntotal = 0;
	for( i = 0; i < nData; i++){
		if( sr_p[i]->flag ){
			for( m = 0; m < ndim; m++){
				vector[ ntotal ][ m ] = sr_p[i]->wgt[ m ];
			}
			ntotal++;
		}
	}

	vq_lbg( mean, NUM_VQ, ntotal , ndim , vector); 
	
	for( l = 0; l < NUM_VQ; l++){
		ncls[ l ] = 0;
	}
	for( i = 0, ntotal = 0; i < nData; i++){
		if( sr_p[i]->flag){
			min_dist = INFINITY;
			min_idx = 0;
			for( l = 0; l < NUM_VQ; l++){
				dist = variance( vector[ntotal], mean[l], ndim );
				if( dist < min_dist ) {
					min_dist = dist;
					min_idx = l;
				}
			}
			ncls[ min_idx ]++;
			ntotal++;
		}
	}


	Dmatfree( vector, nData );

	return ntotal;
}
int outlier_detect( srch_point_s **sr_p, int *ntop_measure_idx , int nData )
{

#if 1
	float	dist;
	int		j, k, m, l;
	int		nDim;
	int		idx;


	int		*N_idx;
	int		**cls_idx;
	double	**cls_dist;
	double	*lof;
	double	*lrd;
	double	**vec;
	double	*x_vec, *y_vec;
	double	*k_dist;
	int		MinPts;
	double	max_measure;

	int		nTotal;
	double		out_th;

	nDim = sr_p[0]->nDim;

	output_value = SvectorDouble( nData );
	in_idx = SvectorInt( nData );
	toScore = SvectorDouble( nData );
	out_idx = SvectorInt( nData );

	N_idx = SvectorInt( nData );
	lof = SvectorDouble( nData );
	lrd = SvectorDouble( nData );
	vec = SmatrixDouble( nData, nDim );
	k_dist = SvectorDouble( nData );


	nTotal = 0;
	max_measure = sr_p[ ntop_measure_idx[ 0 ] ]->measure;
	for( k = 0; k < nData; k++){
		if( sr_p[ k ]->measure >= (0.95 * max_measure) ){
			for( l = 0; l < nDim; l++){
				vec[ nTotal ][ l ] = sr_p[ k ]->wgt[ l ];
			}
			nTotal++;
			sr_p[ k ]->flag = 1;
		}
		else {
			sr_p[ k ]->flag = 0;
		}
	}


	MinPts = MIN(MIN_POINT, nTotal * 0.2 );

	cls_idx = (int **)calloc( nData, sizeof(int *));
	cls_dist = (double **)calloc( nData, sizeof(double *));

	for( k = 0; k < nTotal ; k++) {
		x_vec = vec[ k ];
		for( m = 0, j = 0; m < nTotal; m++){
			if( k != m ){
				y_vec = vec[ m ];
				dist = 0.0;
				for ( l = 0; l < nDim; l++ ){
					dist -= SQ(x_vec[ l ] - y_vec[ l ] );
				}
				in_idx[ j ] =  m;
				output_value[ j++ ] = dist;
			}
		}
		nCutNum = MIN( 4*MinPts , j );
		HeapSort( output_value, toScore, in_idx, out_idx, j, nCutNum)

		l = MinPts - 1;
		do{
			l++;	
		}while( toScore[l] == toScore[ MinPts - 1] && l <= nCutNum - 1);
		N_idx[ k ] = l ;

		cls_idx[ k ] = (int *)calloc( N_idx[ k ], sizeof(int));
		cls_dist[ k ] = (double *)calloc( N_idx[ k ], sizeof(double));

		for( l = 0; l < N_idx[ k ] ; l++){
			cls_idx[ k ][ l ] = out_idx[ l ];
			cls_dist[ k ][ l ] = -toScore[ l ];
		}
		k_dist[ k ] = cls_dist[ k ][ N_idx[k]-1 ];
	}

	for( k = 0; k < nTotal; k++){
		dist = 0.0;
		for( m = 0; m < N_idx[k]; m++){
			idx = cls_idx[ k ][ m ];
			dist += MAX( k_dist[ idx ], cls_dist[ k ][ m ] );
		}
		if( dist >= EPSILON ){
			lrd[ k ] = 1.0 / (dist / N_idx[k]);
		}
		else{
			lrd[ k ] = INFINITY;
		}
	}

	for( k = 0; k < nTotal; k++){
		dist = 0.0;
		for( m = 0; m < N_idx[k]; m++){
			idx = cls_idx[ k ][ m ];
			dist += lrd[ idx ];
		}
		lof[ k ] = dist / (lrd[ k ] * N_idx[k]);
		in_idx[ k ] = k;
		output_value[ k ] = lof[ k ] ;
	}

	nCutNum = MAX(nTotal / 5, 1);

	HeapSort( output_value, toScore, in_idx, out_idx, nTotal, nCutNum)
	//out_th = toScore[ nCutNum - 1 ];
	out_th = MAX(toScore[ nCutNum - 1 ], 1.0);

	for( k = 0; k < nData; k++){
		if( sr_p[k]->flag ){
		   	if( lof[ k ] <= out_th ){
			   	sr_p[ k ]->flag = 1; }
		   	else {
			   	sr_p[ k ]->flag = 0;
		   	}
		}
	}

	for( k = 0; k < nTotal; k++){
		free( cls_dist[ k ] );
		free( cls_idx[ k ] );
	}
	free( cls_dist );
	free( cls_idx );

	free( vec );
	free( N_idx );
	free( k_dist );
	free( lof );
	free( lrd );

	free( output_value );
	free( toScore );
	free( in_idx );
	free( out_idx );
#endif


	return 0;

}

static double calc_disto(double **x, double **codeword, int *x_class_idx, int nsample, int dim_vec, int ncode)
{
	int	i, j, k;
	double	a, min_val, total;

	total = 0.;
	for(i=0; i<nsample; i++) {
		min_val = BIG_NUM;
		for(j=0; j<ncode; j++) {
			for(k=0, a=0.; k<dim_vec; k++)
				a += SQR( x[i][k] - codeword[j][k] );
			a = sqrt(a);
			if( a < min_val ) {
				min_val = a;
				x_class_idx[i] = j;
			}
		}
		total += min_val;
	}
	return( total/(double)nsample );
}

static void mk_codeword(double **x, double **codeword, int *x_class_idx, int total_nframe, int dim_vec, int current_ncodeword)
{
	register int i, j;
	int	count, class_mode, desired_ncodeword;
	int	*tmp_class_idx;
	double	disto, disto_prev;
	double	criterion;

	int	count_empirical;
	double	**org_codeword;

	tmp_class_idx = Ivec(total_nframe);
	org_codeword = Dmat(current_ncodeword, dim_vec);
	desired_ncodeword = current_ncodeword * 2;
	disto_prev = BIG_NUM;


	count_empirical = 0;
	for(i=current_ncodeword-1; i>=0; i--)
		for(j=0; j<dim_vec; j++)
			org_codeword[i][j] = codeword[i][j];

	/***** Splitting *****/
split:	count_empirical ++;
	for(i=current_ncodeword-1; i>=0; i--) {
		for(j=0; j<dim_vec; j++) {
			codeword[2*i+1][j] = org_codeword[i][j] + PERT/(double)(count_empirical);
			codeword[2*i][j] = org_codeword[i][j] - PERT/(double)(count_empirical);
		}
	}

	/***** Distortion over all current codebook *****/
	disto = calc_disto(x, codeword, tmp_class_idx, total_nframe, dim_vec, desired_ncodeword);
	criterion = fabs((disto_prev - disto)/disto);

	fprintf(stderr,"Init distortion = %e, Crit = %e\n", disto, criterion);
	fprintf(stderr,"-----------------------------------------------------\n");

	/***** *****/
	class_mode = 1;
	count = 0;
	fprintf(stderr,"count   distortion   criterion    class mode\n");
	while(count < ITRBND && class_mode && criterion > CRIT_BOUND) {

		for(i=0; i<total_nframe; i++) x_class_idx[i] = tmp_class_idx[i];
		disto_prev = disto;

		/*--- Update Reproduction Vector ---*/
		for(i=0; i<desired_ncodeword; i++) 
			if((!calc_centroid(codeword[i], x, x_class_idx, i, total_nframe, dim_vec ))&& (count_empirical < MAX_SPLIT_NUM ) ) {
				fprintf(stderr,"count_empirical = %d\n", count_empirical+1);
				goto split;
			}

		/*--- ---*/
		disto = calc_disto(x, codeword, tmp_class_idx, total_nframe, dim_vec, desired_ncodeword);
		criterion = fabs((disto_prev - disto)/disto);
		for(i=0, class_mode=0; i<total_nframe; i++) {
			if( tmp_class_idx[i] != x_class_idx[i] ) {
				class_mode = 1;
				break;
			}
		}
		count ++;
		fprintf(stderr,"%d	%e	%e	%d\n", count, disto, criterion, class_mode);
	}

	Ifree(tmp_class_idx);
	Dmatfree(org_codeword, current_ncodeword);
}

static int calc_centroid(double *center, double **x, int *class, int class_idx, int nsample, int dim)
{
	register int	i, j;
	int	count;

	count = 0;
	for(j=0; j<dim; j++) center[j] = 0.;
	for(i=0; i<nsample; i++) {
		if( class[i] == class_idx ) {
			for(j=0; j<dim; j++) center[j] += x[i][j];
			count ++;
		}
	}
	if(count == 0) {
		fprintf(stderr,"# of samples classified to class %d is 0\n", class_idx);
		return(0);
	}
	for(j=0; j<dim; j++) center[j] /= (double)count;
	return(1);
}


static int	vq_lbg( double **mean, int jmax, int ntotal , int norder , double **vector)
{
	int rr;
	int	r, i;
	int *x_class_idx;
	//double	**codeword;
	int	current_ncodeword ;

	x_class_idx = Ivec( ntotal );

	rr = (int)(log( (double)jmax ) / log(2.0));
	for( i = 0; i < ntotal ; i++) x_class_idx[ i ] = 0;
	calc_centroid( mean[0], vector, x_class_idx, 0, ntotal, norder );
	for( r = 0; r < rr; r++){
		current_ncodeword = (int)pow( 2.0 , (double)r );
		fprintf(stderr,"=======================================================\n");
		fprintf(stderr,"          LEVEL %d : # of codewords = %d\n", r+1, current_ncodeword*2);
		fprintf(stderr,"=======================================================\n");
		mk_codeword(vector, mean, x_class_idx, ntotal, norder, current_ncodeword);
	}

	Ifree( x_class_idx );

	return 0;

}

static double	variance(double *xi,double *yi,int norder)
{
	double	dist;
	int	i;

	dist = 0.;
	for ( i=0; i < norder; i++) dist += SQ(xi[i]-yi[i]);

	return(dist);
}

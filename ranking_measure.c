#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "Smemory.h"
#include "listutil.h"
#include "ranking_measure.h"
#include "log_add.h"

#define       ILOG2   1.442695041
//#define       LOG2   0.6931471805


#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

static int		*qstart;
static double	**Idcg;
static int		**prec_idx;
static double	**relevance;
static int		*in_idx;
static int		*out_idx;

static double	*dcg;
static double	*dcg_sum;

static double	*cg;
static double	*cg_sum;

static double	*ndcg;
static double	*ndcg_sum;

static double	*prec;
static double	*prec_sum;

static double	*map;
static double	*map_sum;

static double	*mrr_sum;

static int		num_querry;
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

/*
static int cmp_output ( const void *i, const void *j)
{
	double  f1;
	double  f2;

	f1 = output_value[ *(int *)i ];
	f2 = output_value[ *(int *)j ];

	if( f2 >= f1 ) return 1;
	else return -1;

}

*/

double get_ranking_measure( int nType , int nPos)
{
	double	f_measure = 0;

	switch ( nType ){
		case NDCG: 
			f_measure = ndcg_sum[nPos] / (double)num_querry ;
			break;
		case CG: 
			f_measure = cg_sum[nPos] / (double)num_querry;
			break;
		case PREC: 
			f_measure = prec_sum[nPos] / (double)num_querry;
			break;
		case MAP:
			f_measure = map_sum[nPos] / (double)num_querry;
			break;
		case MRR:
			f_measure = mrr_sum[nPos] / (double)num_querry;
			break;
		case DCG:
			f_measure = dcg_sum[nPos] / (double)num_querry;
			break;
		default: 
			f_measure = prec_sum[nPos] / (double)num_querry;
	}

	return f_measure;
}

int free_ranking_measure( feat_data_s *tr_feat)
{

	int		idx4;
	int		nlist;



	free(cg );
	free(cg_sum );

	free(dcg );
	free(dcg_sum);

	free(ndcg );
	free(ndcg_sum );

	free(map);
	free(map_sum);

	free(mrr_sum);

	free(prec );
	free(prec_sum );

	free(in_idx );
	free(out_idx );
	free(output_value );
	free(toScore );

	nlist = tr_feat->nlist;
	for (idx4 = 0 ; idx4 < nlist ; ++idx4) {
		free( Idcg[ idx4] );
		free( relevance[ idx4 ]);
		free( prec_idx[ idx4 ]);
	}
	free(Idcg);
	free(relevance);
	free(prec_idx);



	return 0;
}

int check_querry( feat_data_s *tr_feat )
{
	int		idx2, idx4;
	double	**matrix;
	int		nlist;
	int		nRecord;

	int		stidx, endidx;
	int		tr_idx;
	int		nFeats;

	int		nEvalNum;
	int		nCnt;
	int		npos;
	//int		low_bound;

	float	prec_th = 2.0;
	float	ratio;
	

	matrix = tr_feat->matrix;
	qstart = tr_feat->qstart;
	nlist = tr_feat->nlist;
	nRecord = tr_feat->nRecord;
	nFeats = tr_feat->nFeats;
	prec_th = tr_feat->prec_th;
	npos = tr_feat->npos;
	

	srand(time(NULL));
	for (idx4 = 0 ; idx4 < nlist ; ++idx4) {
		//tr_feat->flag[ idx4 ] = 0;
		tr_feat->flag[idx4] = 0;
		tr_idx = idx4;
		stidx  = qstart[tr_idx]   ;
		endidx = qstart[tr_idx+1] ; 
		nEvalNum = endidx - stidx;

		nCnt = 0;
		for( idx2 = 0; idx2 < nEvalNum; idx2++){
			if( matrix[ idx2 + stidx ][ nFeats ] >= prec_th ) {
				nCnt++;
			}
		} 

		ratio = (float)nCnt / (float)nEvalNum;
		if( ratio >= 0.8 ){
			tr_feat->flag[idx4] = 1;
		}
		else tr_feat->flag[idx4] = 0;

#if 0
		if( rand()% 5 == 0 ){
			tr_feat->flag[ idx4 ] = 1;
		}
		else{ 
			tr_feat->flag[idx4] = 0;
			tr_idx = idx4;
			stidx  = qstart[tr_idx]   ;
			endidx = qstart[tr_idx+1] ; 
			nEvalNum = endidx - stidx;

			nCnt = 0;
			for( idx2 = 0; idx2 < nEvalNum; idx2++){
				if( matrix[ idx2 + stidx ][ nFeats ] >= prec_th ) {
					nCnt++;
				}
			} 

			ratio = (float)nCnt / (float)nEvalNum;
			if( ratio >= 0.8 ){
				tr_feat->flag[idx4] = 1;
			}
			else tr_feat->flag[idx4] = 0;
		}
#endif
	}

	return 0;
}

int init_ranking_measure( feat_data_s *tr_feat)
{

	int		idx2, idx4;
	double	**matrix;
	int		nlist;
	int		nRecord;

	int		stidx, endidx;
	int		tr_idx;
	int		nFeats;



	double	ftmp;
	int		max_len;
	int		nEvalNum;

	float	prec_th = 2.0;
	

	//////////////////////////////////////
	/// Train wgt by ListMLE Algorithm ///
	////////////////////////////////////// 

	// initial setting

	matrix = tr_feat->matrix;
	qstart = tr_feat->qstart;
	nlist = tr_feat->nlist;
	nRecord = tr_feat->nRecord;
	nFeats = tr_feat->nFeats;
	max_len = tr_feat->max_len; 
	prec_th = tr_feat->prec_th;
	

	Idcg = (double **)calloc( nlist, sizeof(double *));
	relevance = (double **)calloc( nlist, sizeof(double *));
	prec_idx = (int **)calloc( nlist, sizeof(int *));
	
	for (idx4 = 0 ; idx4 < nlist ; ++idx4) {
		tr_idx = idx4;
		stidx  = qstart[tr_idx]   ;
		endidx = qstart[tr_idx+1] ; 
		nEvalNum = endidx - stidx;

		Idcg[ idx4 ] = (double *)calloc( NUM_CUTOFF , sizeof(double));
		relevance[ idx4 ] = (double *)calloc( nEvalNum , sizeof(double));
		prec_idx[ idx4 ] = (int *)calloc( nEvalNum , sizeof(int));


		for( idx2 = 0; idx2 < nEvalNum; idx2++){
			relevance[ idx4 ][ idx2 ] = matrix[ idx2 + stidx ][ nFeats ] ;

			if( matrix[ idx2 + stidx ][ nFeats ] >= prec_th ) {
				prec_idx[ idx4 ][ idx2 ] = 1;
			}
			else{
				prec_idx[ idx4 ][ idx2 ] = 0;
			}

		} 
		nEvalNum = MIN( NUM_CUTOFF, endidx - stidx );
		// Idcg
		for( idx2 = 0; idx2 < nEvalNum; idx2++){
			ftmp = pow( 2.0, relevance[ idx4 ][idx2] ) - 1.0;
			if( idx2  ){
				Idcg[ idx4 ][ idx2 ] = Idcg[ idx4 ][ idx2 - 1] + ftmp * LOG2 /LOG((double)(idx2+2));
			} 
			else{
				Idcg[ idx4 ][ idx2 ] = ftmp ;
			}
		}
	}


	map = SvectorDouble( NUM_CUTOFF );
	map_sum = SvectorDouble( NUM_CUTOFF );

	mrr_sum = SvectorDouble( NUM_CUTOFF );
	cg = SvectorDouble( NUM_CUTOFF );
	cg_sum = SvectorDouble( NUM_CUTOFF );

	dcg = SvectorDouble( NUM_CUTOFF );
	dcg_sum = SvectorDouble( NUM_CUTOFF );

	ndcg = SvectorDouble( NUM_CUTOFF );
	ndcg_sum = SvectorDouble( NUM_CUTOFF );

	prec = SvectorDouble( NUM_CUTOFF );
	prec_sum = SvectorDouble( NUM_CUTOFF );

	in_idx = SvectorInt( max_len );
	output_value = SvectorDouble( max_len )  ;
	out_idx = SvectorInt( NUM_CUTOFF );
	toScore = SvectorDouble( NUM_CUTOFF )  ;



	return 0;


}
void Recog_Test( feat_data_s *tr_feat, double *wgt, char *outfile)
{
	double	**matrix;
	int		*trans_type;
	int		nFeats;
	int		w_idx;

	int		ii, jj;
	double	expsum;

	int		nRecord;
	double	*feature;
	double	ftmp;


	FILE	*fptr;

	

	//////////////////////////////////////
	/// Train wgt by ListMLE Algorithm ///
	////////////////////////////////////// 

	// initial setting

	matrix = tr_feat->matrix;
	nRecord = tr_feat->nRecord;
	nFeats = tr_feat->nFeats;
	trans_type = tr_feat->trans_type;


	fptr = Fopen(outfile,"wt");

	for( ii = 0; ii < nRecord; ii++){
		expsum = 0 ;
		feature = matrix[ ii ];
		for (jj = 1 , w_idx = 0 ; jj < nFeats ; jj++){
			if( trans_type[ jj - 1] == T_RAW ){
				expsum += wgt[ w_idx ] * feature[ jj ];
				w_idx++;
			}
			else if( trans_type[ jj- 1 ] == T_LOG ){
				expsum += wgt[ w_idx ] * LOG(feature[ jj ]);
				w_idx++;
			}
			else if( trans_type[ jj- 1 ] == T_SIGMOID ){
				ftmp = wgt[ w_idx ] + wgt[ w_idx + 1 ] * feature[ jj ] ;
				expsum += ( wgt[ w_idx + 2] / ( 1.0 + exp( ftmp )));
				w_idx+=3;
			}
		}
		fprintf(fptr,"%d\t%f\t%f\n", (int) feature[0], feature[nFeats], expsum) ;
	}

	fclose(fptr);

}

int calc_ranking_measure( feat_data_s *tr_feat, double *wgt)
{
	int		idx2, idx4;
	double	**matrix;
	int		*trans_type;
	int		*qstart;
	int		nlist;
	int		nRecord;
	int		nFeats;
	int		w_idx;
	int		max_len;
	int		num_wgt;

	int		ii, jj;
	int		stidx, endidx;
	int		tr_idx;
	double	expsum;

	int		nEvalNum;
	int		nCutNum;
	double	ftmp;
	double	*feature;

	int		numRelevant;
	double	avgPrecision;
	int		nFirstPos ;

	

	//////////////////////////////////////
	/// Train wgt by ListMLE Algorithm ///
	////////////////////////////////////// 

	// initial setting

	matrix = tr_feat->matrix;
	qstart = tr_feat->qstart;
	nlist = tr_feat->nlist;
	nRecord = tr_feat->nRecord;
	nFeats = tr_feat->nFeats;
	max_len = tr_feat->max_len; 
	trans_type = tr_feat->trans_type;
	num_wgt = tr_feat->num_wgt;

	for( idx2 = 0; idx2 < NUM_CUTOFF; idx2++){
		out_idx[ idx2 ] = 0 ;
		prec_sum[ idx2 ] = 0.0;
		ndcg_sum[ idx2 ] = 0.0;
		dcg_sum[ idx2 ] = 0.0;
		cg_sum[ idx2 ] = 0.0;
		map_sum[ idx2 ] = 0.0;
		mrr_sum[ idx2 ] = 0.0;
	}
	for( idx2 = 0; idx2 < max_len; idx2++){
		output_value[ idx2 ] = 0.0;
	}

	num_querry = 0;
	for (idx4 = 0 ; idx4 < nlist ; ++idx4) {
		tr_idx = idx4;
		stidx  = qstart[tr_idx]   ;
		endidx = qstart[tr_idx+1] ; 
		nEvalNum = endidx - stidx;

		if( tr_feat->flag[idx4]) continue;

		for( idx2 = 0; idx2 < nEvalNum; idx2++){
			in_idx[ idx2 ] = idx2 ;
		}

		for( ii = 0; ii < nEvalNum; ii++){
			expsum = 0 ;
			feature = matrix[ ii + stidx ];
			for (jj = 1 , w_idx = 0 ; jj < nFeats ; jj++){
				if( trans_type[ jj - 1] == T_RAW ){
					expsum += wgt[ w_idx ] * feature[ jj ];
					w_idx++;
				}
				else if( trans_type[ jj - 1] == T_LOG ){
					expsum += wgt[ w_idx ] * LOG(feature[ jj ]);
					w_idx++;
				}
				else if( trans_type[ jj - 1] == T_SIGMOID ){
					ftmp = wgt[ w_idx ] + wgt[ w_idx + 1 ] * feature[ jj ] ;
					expsum += ( wgt[ w_idx + 2] / ( 1.0 + exp( ftmp )));
					w_idx+=3;
				}
			}
			output_value[ ii ] = expsum;
		}
		// sort descendig order
		//qsort( out_idx, nEvalNum , sizeof(int), cmp_output);
		nCutNum = MIN( NUM_CUTOFF, nEvalNum );
		HeapSort( output_value, toScore, in_idx, out_idx, nEvalNum , nCutNum)
		// end

		numRelevant = 0;
		avgPrecision = 0.0;
		nFirstPos = 0;
		for( jj = 0; jj < nCutNum ; jj++){
			// dcg
			ftmp = relevance[ idx4 ][ out_idx[ jj ] ] ;
			if( jj  ){
				dcg[ jj ] = dcg[ jj - 1] + (pow(2.0,ftmp) - 1.0 ) * LOG2 /LOG((double)(jj+2));
				cg[ jj ] = cg[jj-1] + ftmp;

			}
			else{
				dcg[ jj ] = pow(2.0,ftmp) - 1.0 ;
				cg[ jj ] = ftmp;
			}
			cg_sum[ jj ] += cg[ jj ];
			dcg_sum[ jj ] += dcg[ jj ];

			// ndcg
			if( Idcg[idx4][jj] >= 0.001){

				ndcg[ jj ] = dcg[jj]/Idcg[ idx4 ][jj];
				ndcg_sum[ jj ] += ndcg[ jj ];

			}
			else{
				ndcg[ jj ] = 0.0;
				ndcg_sum[ jj ] += ndcg[ jj ];
			}

			//printf("%f %f\n", cg_sum[ jj ], ndcg_sum[ jj ]);


			// precision
			prec[ jj ] = 0.0;
			if( prec_idx[ idx4 ][ out_idx[jj]] ) {
				if( !numRelevant ) {
					nFirstPos = jj + 1;
				}
				numRelevant++;
				avgPrecision += ( (double)numRelevant / (double)(jj + 1));
			}
			if( numRelevant ){
				map[ jj ] = avgPrecision / (double)numRelevant;
				map_sum[ jj ] += map[ jj ];
			}

			prec[ jj ] = (double)numRelevant / (double)(jj+1);
			prec_sum[ jj ] += prec[ jj ];

		}

		for( jj = nCutNum; jj < NUM_CUTOFF; jj++){
			dcg[ jj ] = dcg[ nCutNum - 1 ];
			dcg_sum[ jj ] += dcg[ jj ];

			ndcg[ jj ] = ndcg[ nCutNum - 1 ];
			ndcg_sum[ jj ] += ndcg[ jj ];

			prec[ jj ] = (double)numRelevant / (double)(jj+1);
			prec_sum[ jj ] += prec[ jj ];

			cg[ jj ] = cg[ nCutNum - 1 ];
			cg_sum[ jj ] += cg[ jj ];

			if( numRelevant ){
				map[ jj ] = avgPrecision / (double)numRelevant;
				map_sum[ jj ] += map[ jj ];
			}


		}
		

		if( nFirstPos ){
			for( jj = NUM_CUTOFF -1 ; jj >= (nFirstPos-1); jj--){
				mrr_sum[ jj ] += (1.0 / (double)(nFirstPos));
			}
		}
		num_querry++;

	}

	return 0;


}


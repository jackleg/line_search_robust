#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include "Smemory.h"
#include "listutil.h"
#include "ranking_measure.h"
#include "linesearch.h"
#include "find_max_point.h"
#include "log_add.h"
#include "time_check.h"
#include "types.h"
#include "data_processing.h"

#ifndef __STDC__
#define __DATE__ "1996. 12. 25"
#define __TIME__ "00:00:00"
#endif


static int insert_ntop_array( int *ntop_measure_idx , int nData , srch_point_s **srch_point, double cur_measure, double *wgt);

static int insert_ntop_array( int *ntop_measure_idx , int nData , srch_point_s **srch_point, double cur_measure, double *wgt)
{

	int	ii, jj;
	int	idx;
	int left, mid, right;
	int	real_idx;
	double	ntop_measure;
	int	flag;

	int save_idx ;


	save_idx = nData;
	if( nData ){
		left = 0 ;
		right = nData - 1;
		do{

			mid = ( left + right ) >> 1;
			flag = 0;

			real_idx = ntop_measure_idx[ mid ];
			ntop_measure = srch_point[real_idx]->measure;
			if( ntop_measure > cur_measure ){
				left = mid + 1;
			}
			else if( ntop_measure < cur_measure){
				right = mid - 1; 
			}
			else {
				flag = 1;
				break;
			}

		}while( right >= left ); 


		if( flag ){
			for( jj = 0; jj < nData ; jj++){
			   	idx = ntop_measure_idx[ jj ];
				if( srch_point[ ntop_measure_idx[ jj ] ]->measure == cur_measure ){
					for( ii = 0; ii < srch_point[ idx ]->nDim; ii++){
						if( (int)(10.0 * srch_point[ idx ]->wgt[ ii ]) != (int)(10.0*wgt[ ii ])) {
							break;
						}
					}
					if( ii == srch_point[ idx ]->nDim ){
						flag = 1;
						break;
					}
					else {
						flag = 0;
					}
				}
			}

		}
		
		

		if( nData == NUM_TOP ){

			save_idx = ntop_measure_idx[ nData - 1 ];

			if( (mid != NUM_TOP -1) && !flag){


				srch_point[ save_idx ]->measure = cur_measure;
				for( ii = 0; ii < srch_point[ save_idx ]->nDim; ii++){
					srch_point[ save_idx ]->wgt[ ii ] = wgt[ ii ];
				}

				if( left == mid + 1   ){
					for( idx = nData - 1; idx >= (mid + 1); idx--){
						ntop_measure_idx[ idx + 1] = ntop_measure_idx[ idx ];
					}
					ntop_measure_idx[ mid + 1] = save_idx;
				}
				else {
					for( idx = nData - 1; idx >= mid ; idx--){
						ntop_measure_idx[ idx + 1] = ntop_measure_idx[ idx ];
					}
					ntop_measure_idx[ mid ] = save_idx;
				}

			}
		}
		else{
			if( !flag ){
				srch_point[ save_idx ]->measure = cur_measure;
				for( ii = 0; ii < srch_point[ save_idx ]->nDim; ii++){
					srch_point[ save_idx ]->wgt[ ii ] = wgt[ ii ];
				}

				if( left == mid + 1   ){
					for( idx = nData - 1; idx >= (mid + 1); idx--){
						ntop_measure_idx[ idx + 1] = ntop_measure_idx[ idx ];
					}
					ntop_measure_idx[ mid + 1] = save_idx;
				}
				else {
					for( idx = nData - 1; idx >= mid ; idx--){
						ntop_measure_idx[ idx + 1] = ntop_measure_idx[ idx ];
					}
					ntop_measure_idx[ mid ] = save_idx;
				}
				nData++;
			}
		}
	}
	else{
		// save cur_wgt --> save_idx_array;
		srch_point[ save_idx ]->measure = cur_measure;
		for( ii = 0; ii < srch_point[ save_idx ]->nDim; ii++){
			srch_point[ save_idx ]->wgt[ ii ] = wgt[ ii ];
		}
		//
		nData++;
		ntop_measure_idx[ 0 ] = 0;
	}

	return nData;

}


double Indep_LineSearch( feat_data_s *tr_feat, int maxiter, double *beta, char *parafile)
{

	int iter;
	int	jj, kk;


	int		idx, idx2, idx3;
	int		nlist;
	double	fMeasure;
	double	fMeasure_prev;
	double	fCnvg;


	double	*cur_pos;
	double	*min_pos;
	double	*max_pos;
	double	*opt_pos;
	double	f_min, f_max;

	double	x0[3];
	double	fx0[3];
	int		nFlag;

	int		nEndFlag;

	double	*fstep;


	int		nDim;
	int		mtype;
	int		npos;

	int		nData;
	int		nTop;
	int		t_idx;

	srch_point_s	**sr_p;
	int	*ntop_measure_idx;


	double	tmpf;
	double	f_ranking_measure;
	double	norm;

	double	**mean;
	int		*ncls;
	double	**cls_measure;
	

	// initial setting

	nlist = tr_feat->nlist;
	nDim = tr_feat->num_wgt;
	mtype = tr_feat->mtype;
	npos = tr_feat->npos;
	


	ntop_measure_idx = (int *)calloc( NUM_TOP + 1, sizeof(int));
	sr_p = (srch_point_s **)calloc( NUM_TOP, sizeof( srch_point_s *));
	for( idx = 0; idx < NUM_TOP; idx++){
		sr_p[ idx ] = (srch_point_s *)calloc( 1, sizeof(srch_point_s ));
		sr_p[idx]->wgt = (double *)calloc( nDim, sizeof(double));
		sr_p[idx]->nDim = nDim;
	}



	fprintf(stderr,"Num Querry = %d Feature: %d\n", nlist, tr_feat->nFeats - 1);


	cur_pos = SvectorDouble( nDim );
	min_pos = SvectorDouble( nDim );
	max_pos = SvectorDouble( nDim );
	opt_pos = SvectorDouble( nDim );

	fstep = SvectorDouble( nDim );


	sr_p[0]->wgt[ 0 ] = 1.0;
	for( idx = 1; idx < nDim; idx++){
		sr_p[0]->wgt[ idx ] = 1.0;
	}
	nData = 0;
	nTop = 1;

	for( kk = 0; kk < NUM_TRAIN; kk++){
	   	init_ranking_measure( tr_feat ); 
		check_querry( tr_feat ); 
		
		iter = 0; 
		for( jj = 0; jj < NUM_INIT; jj++){

			nDim = 0;
			for( idx2 = 0; idx2 < tr_feat->nFeats - 1 ; idx2++){
				if( tr_feat->trans_type[idx2] == 0 || tr_feat->trans_type[ idx2 ] ==  1 ){
					min_pos[ nDim ] = -4.0;
					max_pos[ nDim ] = 4.0;
					fstep[ nDim ] = (max_pos[ nDim ] - min_pos[ nDim ] ) / GRID_SIZE;
					tmpf = (double)(2 * rand() - RAND_MAX ) / RAND_MAX;
					opt_pos[ nDim ] = tmpf  * 4.0;
					nDim++;
				}
				else if ( tr_feat->trans_type[idx2] == 2 ){

					min_pos[ nDim ] = -4.0;
					max_pos[ nDim ] = 4.0;
					fstep[ nDim ] = (max_pos[ nDim ] - min_pos[ nDim ] ) / GRID_SIZE;
					tmpf = (double)(2 * rand() - RAND_MAX ) / RAND_MAX;
					opt_pos[ nDim ] = tmpf  * 4.0;
					nDim++;

					min_pos[ nDim ] = -4.0;
					max_pos[ nDim ] = 4.0;
					fstep[ nDim ] = (max_pos[ nDim ] - min_pos[ nDim ] ) / GRID_SIZE;
					tmpf = (double)(2 * rand() - RAND_MAX ) / RAND_MAX;
					opt_pos[ nDim ] = tmpf  * 4.0;
					nDim++;

					min_pos[ nDim ] = -4.0;
					max_pos[ nDim ] = 4.0;
					fstep[ nDim ] = (max_pos[ nDim ] - min_pos[ nDim ] ) / GRID_SIZE;
					tmpf = (double)(2 * rand() - RAND_MAX ) / RAND_MAX;
					opt_pos[ nDim ] = tmpf  * 4.0;
					nDim++;
				}
			} 
			
			fMeasure = -1000.0;
			fMeasure_prev = -1000.0;

			nEndFlag = FALSE;
			do{
				
				for( idx =0; idx < nDim; idx++){
					fprintf(stderr,"iter = %d Dim = %d\n", iter, idx ); 
					
					for( idx3 = 0; idx3 < nDim; idx3++){
						cur_pos[ idx3 ] = opt_pos[ idx3 ];
					} 
					
					f_min = min_pos[ idx ] ;
					for( idx3 = 0; idx3 < GRID_SIZE-1; idx3++){
						f_max = f_min + fstep[ idx ];

						nFlag = 0;
						x0[0] = (f_max + f_min)/ 2.0 - 0.2 * fstep[idx];
						x0[2] = (f_max + f_min)/ 2.0 + 0.2 * fstep[idx];
						nFlag = max_search_parabolic_extrapolation( tr_feat, cur_pos , idx, x0, fx0, MIN_BOUND, MAX_BOUND);
						f_ranking_measure = fx0[0]; 
						if( nFlag == 0){ 
							nFlag = max_search_parabolic_interpolation( tr_feat, cur_pos , idx, x0, fx0);
							
							if( nFlag == 0){
								f_ranking_measure = fx0[1];
								cur_pos[idx] = x0[1];
							}
						   
							nData = insert_ntop_array( ntop_measure_idx, nData, sr_p, f_ranking_measure , cur_pos);

							if( f_ranking_measure > fMeasure ){
								fMeasure = f_ranking_measure;
								fprintf(stderr,"**** Train[%d] : Iter[%2d], Dim[%2d] Max_Measure:%e\n", kk, iter, idx, f_ranking_measure );
								for( idx2 = 0; idx2 < nDim; idx2++){
									beta[ idx2 ] = cur_pos[ idx2 ];

									if( idx2 % 5 == 0 ){
										fprintf(stderr,"[%2d - %2d] ",idx2 , idx2 + 4);
									}
									opt_pos[ idx2 ] = cur_pos[idx2];
									fprintf(stderr,"%-10.6f\t", cur_pos[idx2]);
									if( (idx2+1) % 5 == 0 ) {
										fprintf(stderr,"\n");
									}
								}
								fprintf(stderr,"\n\n");
								WriteBeta(beta, nDim , parafile) ; // 1 .. nDim - 1
							}
						}
						f_min = f_max;

					}

				}
				nTop = nData;

				fCnvg = fabs( 1.0 - fMeasure_prev/fMeasure );
				fprintf(stderr," Convg = %e \n", fCnvg);
				if( fCnvg < fCnvgTh ) nEndFlag = TRUE;
				fMeasure_prev = fMeasure;

				iter++;

				if( iter > maxiter ) nEndFlag = TRUE;

			}while( !nEndFlag  );
		}
   	}


#if 1
	//fprintf(stdout,"=== Final Srch Point ======= \n");
	for( t_idx = 0; t_idx < nTop; t_idx++){
		idx2 = ntop_measure_idx[ t_idx ];
		//fprintf(stdout,"%d\t%d\t%e\t", t_idx, idx2, sr_p[idx2]->measure);
		norm = 0.0;
		for( idx3 = 0; idx3 < nDim; idx3++){
			tmpf = sr_p[ idx2 ]->wgt[ idx3 ];
			norm += ( tmpf * tmpf );
		}
		norm = sqrt( norm );
		for( idx3 = 0; idx3 < nDim; idx3++){
			tmpf = sr_p[ idx2 ]->wgt[ idx3 ] / norm;
			sr_p[ idx2 ]->wgt[ idx3 ] = tmpf;
			//fprintf(stdout,"%f\t", tmpf);
		}
		//fprintf(stdout,"\n");
	}
//	fprintf(stdout,"\n"); 
#endif
	
	
	outlier_detect( sr_p, ntop_measure_idx, nTop);

#if 0
	fprintf(stdout,"=== Final Srch Point After LOF ======= \n");
	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[ idx3 ] = 0.0;
	}
	for( t_idx = 0; t_idx < nTop; t_idx++){
		idx2 = ntop_measure_idx[ t_idx ];
		if( sr_p[ idx2 ]->flag ){
			fprintf(stdout,"%d\t%d\t%e\t", t_idx, idx2, sr_p[idx2]->measure);
			for( idx3 = 0; idx3 < nDim; idx3++){
				tmpf = sr_p[ idx2 ]->wgt[ idx3 ] ;
				fprintf(stdout,"%f\t", tmpf);
			}
			fprintf(stdout,"\n");
		}
	}
	fprintf(stdout,"\n"); 
#endif

	mean = Dmat( NUM_VQ, nDim );
	ncls = Ivec( NUM_VQ );
	nTop = vector_quantize( sr_p , nTop, mean, ncls);

#if 0

	// 0 Max Point 
	t_idx = ntop_measure_idx[ 0 ]; 
	
	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[ idx3 ] = sr_p[ t_idx ]->wgt[ idx3 ];
   	}
	tmpf = fabs(beta[0]);
	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[ idx3 ] /= tmpf;
	}
   	WriteBeta_tmp(beta, nDim , parafile, 0) ; // 1 .. nDim - 1
	// 1 Average

   	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[idx3] = 0.0;
	}
	for(t_idx = 0; t_idx < NUM_VQ; t_idx++){
		for( idx3 = 0; idx3 < nDim; idx3++){
			beta[ idx3 ] += mean[ t_idx][ idx3 ] ;
		}
	}
	tmpf = fabs(beta[0]);
	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[ idx3 ] /= tmpf;
	}
   	WriteBeta_tmp(beta, nDim , parafile, 1) ; // 1 .. nDim - 1
	// 2 weighted Average
   	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[idx3] = 0.0;
	}
	for(t_idx = 0; t_idx < NUM_VQ; t_idx++){
		for( idx3 = 0; idx3 < nDim; idx3++){
			beta[ idx3 ] += (mean[ t_idx][ idx3 ] * ncls[ t_idx ]);
		}
	}
	tmpf = fabs(beta[0]);
	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[ idx3 ] /= tmpf;
	}
   	WriteBeta_tmp(beta, nDim , parafile, 2) ; // 1 .. nDim - 1
	// 3 max num cluster
   	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[idx3] = 0.0;
	}
	idx = 0;
	idx2 = 0;
	for(t_idx = 0; t_idx < NUM_VQ; t_idx++){
		if( ncls[ t_idx ] > idx2 ){
			idx2 = ncls[t_idx];
			idx = t_idx;
		}
	}
	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[ idx3 ] += (mean[ idx ][ idx3 ]);
	}
	tmpf = fabs(beta[0]);
	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[ idx3 ] /= tmpf;
	}
   	WriteBeta_tmp(beta, nDim , parafile, 3) ; // 1 .. nDim - 1

	// 4 max measure cluster
   	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[idx3] = 0.0;
	}
	idx = 0;
	tmpf = 0.0;
	for(t_idx = 0; t_idx < NUM_VQ; t_idx++){
		if( cls_measure[ t_idx ] > tmpf ){
			tmpf = cls_measure[t_idx];
			idx = t_idx;
		}
	}
	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[ idx3 ] += (mean[ idx ][ idx3 ]);
	}
	tmpf = fabs(beta[0]);
	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[ idx3 ] /= tmpf;
	}
   	WriteBeta_tmp(beta, nDim , parafile, 4) ; // 1 .. nDim - 1
#endif

	for( idx = 0; idx < tr_feat->nlist; idx++){
		tr_feat->flag[ idx ] = 0;
	}
	cls_measure = Dmat( NUM_VQ , 3);
	for( idx = 0; idx < NUM_VQ; idx++){
		calc_ranking_measure( tr_feat, mean[idx] );
	   	cls_measure[idx][0] = get_ranking_measure( NDCG , npos);
	   	cls_measure[idx][1] = get_ranking_measure( CG , npos);
	   	cls_measure[idx][2] = get_ranking_measure( PREC , npos);
	}

	Write_Candidate_Model( parafile, ncls, cls_measure, mean, nDim, NUM_VQ, npos );

   	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[idx3] = 0.0;
	}
	for(t_idx = 0; t_idx < NUM_VQ; t_idx++){
		tmpf = fabs(mean[t_idx][ 0 ]);
		for( idx3 = 0; idx3 < nDim; idx3++){
			beta[ idx3 ] += (mean[ t_idx][ idx3 ] * ncls[t_idx]);
		}
	}
	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[ idx3 ] /= NUM_VQ;
	}
	tmpf = fabs(beta[0]);
	for( idx3 = 0; idx3 < nDim; idx3++){
		beta[ idx3 ] /= tmpf;
	}
   	WriteBeta(beta, nDim , parafile) ; // 1 .. nDim - 1


	Ifree(ncls);
	Dmatfree( cls_measure, NUM_VQ );
	Dmatfree( mean, NUM_VQ);

	free( fstep );
	free( min_pos );
	free( max_pos );
	free( opt_pos );
	free( cur_pos );

	for( idx = 0; idx < NUM_TOP; idx++){
		free(sr_p[idx]->wgt);
		free(sr_p[idx]);
	}
	free(sr_p);
	free( ntop_measure_idx );


	free_ranking_measure( tr_feat);


	return 0.0;


}



double Indep_LineSearch_s( feat_data_s *tr_feat, int maxiter, double *beta, char *parafile)
{

	int iter;


	int		idx, idx2, idx3;
	int		nIndex;
	int		nlist;
	double	fMeasure;
	double	fMeasure_prev;
	double	fCnvg;


	double	*cur_pos;
	double	*min_pos;
	double	*opt_pos;

	int		nEndFlag;

	double	*fstep;


	int		nDim;
	int		mtype;
	int		npos;


	double	f_ranking_measure;
	

	// initial setting

	nlist = tr_feat->nlist;
	nDim = tr_feat->num_wgt;
	mtype = tr_feat->mtype;
	npos = tr_feat->npos;
	

	init_ranking_measure( tr_feat );

	fMeasure = -1000.0;
	fMeasure_prev = -1000.0;


	iter = 0;

	fprintf(stderr,"Num Querry = %d Feature: %d\n", nlist, tr_feat->nFeats - 1);



	cur_pos = SvectorDouble( nDim );
	min_pos = SvectorDouble( nDim );
	opt_pos = SvectorDouble( nDim );

	fstep = SvectorDouble( nDim );

	nDim = 0;
	for( idx2 = 0; idx2 < tr_feat->nFeats - 1 ; idx2++){
		if( tr_feat->trans_type[idx2] == 0 || tr_feat->trans_type[ idx2 ] ==  1 ){
			fstep[ nDim ] = 50.0 / GRID_SIZE_S;
			opt_pos[ nDim ] = 0.0;
			min_pos[ nDim ] = opt_pos[ nDim ] - fstep[nDim] * GRID_SIZE_S * 0.5;
			nDim++;
		}
		else if ( tr_feat->trans_type[idx2] == 2 ){

			fstep[ nDim ] = 6.0 / GRID_SIZE_S;
			opt_pos[ nDim ] = 0.0;
			min_pos[ nDim ] = opt_pos[ nDim ] - fstep[nDim] * GRID_SIZE_S * 0.5;
			//opt_pos[ nDim ] = min_pos[nDim];
			nDim++;

			fstep[ nDim ] = 6.0 / GRID_SIZE_S;
			opt_pos[ nDim ] = 0.0;
			min_pos[ nDim ] = opt_pos[ nDim ] - fstep[nDim] * GRID_SIZE_S * 0.5;
			//opt_pos[ nDim ] = min_pos[nDim];
			nDim++;

			fstep[ nDim ] = 20.0 / GRID_SIZE_S;
			opt_pos[ nDim ] = 0.0;
			min_pos[ nDim ] = opt_pos[ nDim ] - fstep[nDim] * GRID_SIZE_S * 0.5;
			opt_pos[ nDim ] = 1.0;
			nDim++;
		}
	}
	
	// set initial point
	//opt_pos[ 0 ] = 1.0;
	for( idx = 0; idx < nDim; idx++) opt_pos[ idx ] = 1.0;

	
	timing_init();
	timing_start();

	iter = 0;
	nEndFlag = FALSE;
	do{
		nIndex = GRID_SIZE_S; 
		
		for( idx = 1; idx < nDim; idx++){
			fprintf(stderr,"iter = %d Dim = %d\n", iter, idx ); 
			
			for( idx3 = 0; idx3 < nDim; idx3++){
				cur_pos[ idx3 ] = opt_pos[ idx3 ];

			}

			for( idx3 = 0; idx3 < nIndex; idx3++){

				cur_pos[ idx ] = min_pos[ idx ] + fstep[idx] * idx3;


				calc_ranking_measure( tr_feat, cur_pos );
				f_ranking_measure = get_ranking_measure( mtype , npos);


				if( f_ranking_measure > fMeasure ){

					for( idx2 = 0; idx2 < nDim; idx2++){
						beta[ idx2 ] = cur_pos[ idx2 ];
					}
					WriteBeta(beta, nDim , parafile) ; // 1 .. nDim - 1
					fMeasure = f_ranking_measure; 

					fprintf(stderr,"**** %d\t%d\t%d\t", iter, idx, idx3 );
					for( idx2 = 0; idx2 < nDim; idx2++){
						opt_pos[ idx2 ] = cur_pos[idx2];
						fprintf(stderr,"%f\t", cur_pos[idx2]);
					}
					fprintf(stderr,"%e\n", fMeasure);
				}

				if( (idx3 + 1 ) % 4000 == 0){
					fprintf(stderr,"%d\t%d\t", iter, idx3 );
					for( idx2 = 0; idx2 < nDim; idx2++){
						fprintf(stderr,"%f\t", opt_pos[idx2]);
					}
					fprintf(stderr,"%e\n", fMeasure);
					
					
					timing_stop();
					fprintf(stderr,"%6.2f sec elpased\n", user_elapsed_time());
					fprintf(stderr,"%6.2f sec cpu elpased\n", cpu_elapsed_time());
				}

			}
		}

		fCnvg = fabs( 1.0 - fMeasure_prev/fMeasure );
		fprintf(stderr," Convg = %e \n", fCnvg);
		if( fCnvg < fCnvgTh ) nEndFlag = TRUE;
		fMeasure_prev = fMeasure;

		iter++;

		if( iter > maxiter ) nEndFlag = TRUE;

	}while( !nEndFlag  );


	free( fstep );
	free( min_pos );
	free( opt_pos );
	free( cur_pos );


	free_ranking_measure( tr_feat);


	return 0.0;


}


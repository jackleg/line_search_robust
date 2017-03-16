#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include "Smemory.h"
#include "log_add.h"
#include "listutil.h"
#include "ranking_measure.h"
#include "linesearch.h"

#ifndef __STDC__
#define __DATE__ "1996. 12. 25"
#define __TIME__ "00:00:00"
#endif


extern int  Sgetargs  () ;  
extern void sort_matrix(double **in_matrix, int height, int width) ;

#define MAX_FIELD 100

static int parse_trans_type (char *trans_type , feat_data_s *tr_feat)
{
	int	idx;
	int fid;
	int tid;
	const	char delptr[] = ",";
	char	*str_elm[ MAX_FIELD ];
	char	tmp_str[ 256 ];
	int	nField;
	char	*pbuf;

	strcpy( tmp_str, trans_type );

	pbuf = tmp_str;
	nField = 0;
	while( (str_elm[ nField ] = strsep( &pbuf, delptr ) ) != NULL ) nField++;


	for( idx = 0; idx < nField ; idx++){
		if( sscanf(str_elm[idx], "%d:%d",&fid, &tid) == 2 ){
			tr_feat->trans_type[ fid ] = tid;
		}
		if( tid == 0 ){
			fprintf(stderr,"Feature[%d] : Raw \n", idx );
		}
		else if( tid == 1 ){
			fprintf(stderr,"Feature[%d] : Log \n", idx );
		}
		else if( tid == 2 ){
			fprintf(stderr,"Feature[%d] : Sigmoid \n", idx );
		}
	}
	fprintf(stderr,"\n");

	return nField;

}

static int parse_measure_type (char *mtype , feat_data_s *tr_feat)
{
	int fid;
	int tid;


	fid = 2;
	tid = 10;


	if( sscanf(mtype, "%d:%d",&fid, &tid) == 2 ){
		tr_feat->mtype = fid;
		tr_feat->npos = tid - 1;
	}

	switch ( fid ){
		case NDCG: 
			fprintf(stderr,"Measure : NDCG@%d\n",tid);
			break;
		case CG: 
			fprintf(stderr,"Measure : CG@%d\n",tid);
			break;
		case PREC: 
			fprintf(stderr,"Measure : PREC@%d\n",tid);
			break;
		case MAP:
			fprintf(stderr,"Measure : MAP@%d\n",tid);
			break;
		case MRR:
			fprintf(stderr,"Measure : MRR@%d\n",tid);
			break;
		case DCG:
			fprintf(stderr,"Measure : DCG@%d\n",tid);
			break;
		default: 
			fprintf(stderr,"Measure : PREC@%d\n",tid);
	}
	fprintf(stderr,"\n");

	return 0;


}


////////////////////////////////////////////////////////////////////////

int main (int argc, char *argv[])
{
    double *beta ;
    int    howto = 0 ;
    char   *parafile   = "beta.lrm"  ; // list rank model
    char   *inputfile  = "datafname" ;
    char   *outputfile = "outfile"   ;
    char   *logfile    = "logfile"   ;
	char	*transtype	= "0:0,1:0";
    int     nRecord  = 1        ;
    int     nFeats   = 1        ;
    int     crossval = 1        ;
    int	    maxiter  = 200    ;
    float   rel_th   = 2.0     ;
    float   rratio   = 1.0e-7 ;
	int		topk	= 10;
    double  finalentropy        ;
    int     n_vars, n_samples, do_sort = 0 ;
    int     trainflag  = 1   ; 
	int	max_len;
	char	*validation = "valid.dat";
	char	*mtype = "1:10";
	int		idx;
	int		num_wgt;
	int		smode_flag = 1;		
	

	feat_data_s	*tr_feat;



    Sgetargs(argc, argv, "-h", "-help", NULL,
		"-train",   NULL, &trainflag,   "if set, train a listrank, if not test it",
        "-crv",     NULL, &crossval,    "if set, 3-fold cross-validation",
	    "-mdl",     "%s", &parafile ,   "output listrank model .lrm file name (human-readable)",
	    "-input",   "%s", &inputfile,   "input data file name",
        "-output",  "%s", &outputfile,  "output file name, when testing on input file",
        "-trans",  "%s",  &transtype,  "feature transformation type",
        "-how",     NULL, &howto,       "How-To page",
        "-sort",    NULL, &do_sort,     "do sort the data by the first column",
	    "-maxi",    "%d", &maxiter,     "max iteration",
		"-vali",	"%s", &validation, "validation file",
		"-mtype",	"%s", &mtype, "measure type and position",
        "-rel_th",  "%f", &rel_th,      "relevance_th",
        "-topk",  "%d", &topk,      "topk prob",
        "-smode",  "%d", &smode_flag,      "search in speed mode",
        "-rratio",  "%f", &rratio,      "iterate condition: (pre - curr)/pre > rratio",
	    "-log",     "%s", &logfile,     "log file which has coeffs on every epoch",
	         NULL,
	         NULL);

    if (howto) { HowTo() ; exit(1) ; } ;

    if (trainflag==1) {
		// Read Train Size
		file_size (inputfile, &n_vars, &n_samples) ;
		nRecord = n_samples  ;
		nFeats  = n_vars - 1 ;
		fprintf(stderr,"# of records  : %d\n", nRecord)  ;
		fprintf(stderr,"# of features : %d\n", nFeats+1) ; // ex) n_vars : 4 , nFeats : 3
		fprintf(stderr,"Program runs on %s mode.\n", (trainflag) ? "TRAIN" : "TEST") ;
		tr_feat = (feat_data_s *)calloc( 1, sizeof( feat_data_s ));
		tr_feat->prec_th = rel_th;
		tr_feat->nRecord = nRecord;
		tr_feat->nFeats = nFeats;
		tr_feat->matrix   = SmatrixDouble(nRecord, nFeats+1) ; // 0 .. nFeats : 4
		BT_ReadMatrix(tr_feat->matrix, inputfile, nRecord, nFeats) ;
		if (do_sort) sort_matrix(tr_feat->matrix, nRecord, nFeats+1) ;

        tr_feat->nlist    = GetNList(tr_feat->matrix, nRecord) ; // # of unique lists
        tr_feat->qstart   = SvectorInt(tr_feat->nlist+1)       ;
		max_len = GetUniqueList(tr_feat->matrix, tr_feat->qstart, nRecord, tr_feat->nlist, nFeats+1) ; 
		tr_feat->max_len = max_len + 1; 
		
		tr_feat->trans_type = SvectorInt( nFeats - 1);
		for( idx = 0; idx < nFeats - 1; idx++){
			tr_feat->trans_type[ idx ] = 0;
		}
		parse_trans_type( transtype  , tr_feat );
		parse_measure_type( mtype, tr_feat );

		num_wgt = 0;
		for( idx = 0; idx < nFeats - 1; idx++){
			if( tr_feat->trans_type[ idx ] == T_RAW || tr_feat->trans_type[ idx ] == T_LOG ) num_wgt+=1;
			else if( tr_feat->trans_type[ idx ] == T_SIGMOID ) num_wgt += 3;
		}
		tr_feat->num_wgt = num_wgt;

		srand(time(NULL));
		tr_feat->flag = SvectorInt( tr_feat->nlist );
		check_querry( tr_feat );

		beta     = SvectorDouble(num_wgt)          ; // 4 

		if( smode_flag ){
		   	finalentropy = Indep_LineSearch( tr_feat, maxiter, beta, parafile) ;
		}
		else {
		   	finalentropy = Indep_LineSearch_s( tr_feat, maxiter, beta, parafile) ;
		}
		if( !nRecord  ) WriteBeta(beta, num_wgt, parafile) ; // 1 .. nFeats - 1
		Recog_Test( tr_feat, beta, outputfile );
        //EstiLNStrength(tr_feat->nRecord, tr_feat->matrix, nFeats, beta, outputfile) ; // 1 .. nFeats -1

		free(beta) ;

		free( tr_feat->trans_type );
		free( tr_feat->matrix );
		free( tr_feat->flag );
		free(tr_feat);

    } else {
		// Read File Size
		file_size (inputfile, &n_vars, &n_samples) ;
		nRecord = n_samples  ;
		nFeats  = n_vars - 1 ;
		fprintf(stderr,"# of records  : %d\n", nRecord)  ;
		fprintf(stderr,"# of features : %d\n", nFeats+1) ; // ex) n_vars : 4 , nFeats : 3
		fprintf(stderr,"Program runs on %s mode.\n", (trainflag) ? "TRAIN" : "TEST") ;
	   	tr_feat = (feat_data_s *)calloc( 1, sizeof( feat_data_s ));
		tr_feat->nRecord = nRecord;
		tr_feat->nFeats = nFeats;
		tr_feat->matrix   = SmatrixDouble(nRecord, nFeats+1) ; // 0 .. nFeats : 4
		BT_ReadMatrix(tr_feat->matrix, inputfile, nRecord, nFeats) ;
		if (do_sort) sort_matrix(tr_feat->matrix, nRecord, nFeats+1) ;


		tr_feat->trans_type = SvectorInt( nFeats - 1);
		for( idx = 0; idx < nFeats - 1; idx++){
			tr_feat->trans_type[ idx ] = 0;
		}
		parse_trans_type( transtype  , tr_feat );

		num_wgt = 0;
		for( idx = 0; idx < nFeats - 1; idx++){
			if( tr_feat->trans_type[ idx ] == T_RAW || tr_feat->trans_type[ idx ] == T_LOG ) num_wgt+=1;
			else if( tr_feat->trans_type[ idx ] == T_SIGMOID ) num_wgt += 3;
		}
		tr_feat->num_wgt = num_wgt;

		beta     = SvectorDouble(num_wgt)          ; // 4 
        ReadBeta(beta, num_wgt, parafile) ;

        //EstiLNStrength(tr_feat->nRecord, tr_feat->matrix, nFeats, beta, outputfile) ;
		Recog_Test( tr_feat, beta, outputfile );
	   
		free(beta) ;
		free( tr_feat->trans_type);
		free( tr_feat->matrix );
		free(tr_feat);
    }


    return 0 ;
}

/*-------------------------------------------------------------------*/




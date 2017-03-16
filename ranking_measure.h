#ifndef __RANKING_MEASURE_H__
#define	__RANKING_MEASURE_H__

#define NUM_CUTOFF  30
								 //#define CUTOFF  {0, 1, 2, 3, 4, 9, 14, 19, 24, 29}
#define CUTOFF1  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
#define CUTOFF2  {4, 9, 14, 19, 24, 29, 34, 39, 44, 49}

#define nMeasureType	10


typedef enum tag_transform_type {
	T_RAW=0,
	T_LOG,
	T_SIGMOID
} transform_type ;

typedef enum tag_measure_type {
	NDCG=0,
	CG,
	PREC,
	MRR,
	MAP,
	DCG
} measure_type ;

typedef struct tag_feat_data {
	int	max_len;
	int	nRecord;
	int	nFeats;
	int	*qstart;
	int	nlist;
	float	prec_th;
	int	*trans_type;
	int	num_wgt;
	int	*wgt;
	int mtype;
	int npos;
	int	*flag;
	double	**matrix;
}feat_data_s;

typedef struct tag_check_data {
	double	measure[ nMeasureType ] ;
}check_data_s;

extern int init_ranking_measure( feat_data_s *val_feat);
extern int calc_ranking_measure( feat_data_s *val_feat, double *wgt);
extern double get_ranking_measure( int nType , int nPos);
extern int free_ranking_measure( feat_data_s *tr_feat);
extern void Recog_Test( feat_data_s *tr_feat, double *wgt, char *outfile);
extern int check_querry( feat_data_s *tr_feat );


#endif

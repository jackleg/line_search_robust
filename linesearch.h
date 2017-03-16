#ifndef __LINESEARCH__
#define __LINESEARCH__

extern double Indep_LineSearch( feat_data_s *tr_feat, int maxiter, double *beta, char *parafile);
extern double Indep_LineSearch_s( feat_data_s *tr_feat, int maxiter, double *beta, char *parafile);



enum {
	TRAIN_CG = 0,
	TRAIN_BFGS
};


#endif


#ifndef __FIND_MAX_POINT_H__
#define __FIND_MAX_POINT_H__

#define sqrt5 2.236067977499789696

extern double max_search_golden_section( feat_data_s *tr_feat, double *wgt, int n_curr_pos, double a, double b );
extern int max_search_parabolic_interpolation( feat_data_s *tr_feat, double *wgt, int n_curr_pos, double *x0, double *fx0);
extern int max_search_parabolic_extrapolation( feat_data_s *tr_feat, double *wgt, int n_curr_pos, double *x, double *fx , double min, double max);


#endif


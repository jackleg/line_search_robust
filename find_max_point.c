#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include "ranking_measure.h"
#include "find_max_point.h"

#ifndef __STDC__
#define __DATE__ "1996. 12. 25"
#define __TIME__ "00:00:00"
#endif


#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))


static double	lambda = 0.5 * ( sqrt5 - 1.0 );
static double	mu = 0.5 * (3.0 - sqrt5 );

static const double alpha = 0.5 * (3.0 - sqrt5);

static int Stopping_Rule(double x0, double x1, double tolerance);
static int Stopping_Rule_Parabolic(double x[], double tolerance);

static int Parabolic_Maximum( double x[], double y[], double *xmin);
static void Check_New_Test_Point( double x[], double *xmin, double min_displacement, double epsilon );
static void Left_Interior_Point( feat_data_s *tr_feat, double *wgt, int n_curr_pos,  double x[], double y[] );
static void Right_Interior_Point( feat_data_s *tr_feat, double *wgt , int n_curr_pos, double x[], double y[] );

static double Parabolic_Maximum_Ex( double x[], double y[], double cut_off_scale_factor ) ;
static double New_Test_Point_Ex( double x[], double y[], double a, double b, double min_displacement, double cut_off_scale_factor ) ;



static int Stopping_Rule(double x0, double x1, double tolerance)
{
	double xm = 0.5 * fabs( x1 + x0 );

	if ( xm <= 1.0 ) return ( fabs( x1 - x0 ) < tolerance ) ? 1 : 0;
	return ( fabs( x1 - x0 ) < tolerance * xm ) ? 1 : 0;
}


double max_search_golden_section( feat_data_s *tr_feat, double *wgt, int n_curr_pos, double a, double b )
{

	double	x1;
	double	x2;
	double	fx1;
	double	fx2;
	double	tolerance ;

	double	fa, fb;
	
	x1 = b - lambda * ( b - a );
	x2 = a + lambda * ( b - a );

	wgt[ n_curr_pos ] = x1; 
	calc_ranking_measure( tr_feat, wgt );
	fx1 = get_ranking_measure( tr_feat->mtype , tr_feat->npos);

	wgt[ n_curr_pos ] = x2; 
	calc_ranking_measure( tr_feat, wgt );
	fx2 = get_ranking_measure( tr_feat->mtype , tr_feat->npos);

	tolerance = sqrt( DBL_EPSILON) * ( b - a );


	fa = 0.0;
	fb = 0.0;

	while( ! Stopping_Rule( a, b, tolerance )) {
		if( fx1 < fx2 ){
			a = x1;
			fa = fx1;
			if( Stopping_Rule( a, b, tolerance )) break;
			x1 = x2;
			fx1 = fx2;
			x2 = b - mu * ( b - a); 
			
			wgt[ n_curr_pos ] = x2; 
			calc_ranking_measure( tr_feat, wgt );
			fx2 = get_ranking_measure( tr_feat->mtype , tr_feat->npos);

			//fprintf(stderr,"1 %d %f %f\n", n_curr_pos, x1, x2 );

		}
		else{
			b = x2;
			fb = fx2;
			if( Stopping_Rule( a, b, tolerance )) break;
			x2 = x1;
			fx2 = fx1;
			x1 = a + mu * ( b - a );

			wgt[ n_curr_pos ] = x1; 
			calc_ranking_measure( tr_feat, wgt );
			fx1 = get_ranking_measure( tr_feat->mtype , tr_feat->npos);

			//fprintf(stderr,"2 %d %f %f\n", n_curr_pos, x1, x2 );
		}
	}

	if( fa > fb ) {
		wgt[ n_curr_pos ] = a; 
	}
	else{
		wgt[ n_curr_pos ] = b; 
	}
	calc_ranking_measure( tr_feat, wgt );

	fx1 = get_ranking_measure( tr_feat->mtype , tr_feat->npos);


	return fx1;


}



int max_search_parabolic_interpolation( feat_data_s *tr_feat, double *wgt, int n_curr_pos, double *x0, double *fx0)
{


	double	tolerance ;
	double x[5] ;
	double y[5] ;
	double min_displacement ;
	double epsilon ;
	double xmax, ymax;
	int err = 0;
	int		idx;


	//tolerance = sqrt(DBL_EPSILON) * ( x0[2] - x0[0] );
	tolerance = 1.0e-8;
	min_displacement = 0.01 * tolerance;
	epsilon = 0.333333 * tolerance;
	err = 0;

	for( idx = 0; idx < 3; idx++){
		wgt[ n_curr_pos ] = x0[idx]; 
		calc_ranking_measure( tr_feat, wgt );
		fx0[idx] = get_ranking_measure( tr_feat->mtype , tr_feat->npos);
	}

	x[0] = x0[0];
	x[1] = x0[0];
	x[2] = x0[1];
	x[3] = x0[2];
	x[4] = x0[2];

	y[0] = fx0[0];
	y[1] = fx0[0];
	y[2] = fx0[1];
	y[3] = fx0[2];
	y[4] = fx0[2];


	// Verify that the input data satisfy the conditions:
	// x1 < x2 < x3 and fx1 <= fx2 >= fx3 but not fx1 = fx2 = fx3.

	if ( (x[0] > x[2]) || (x[4] < x[2]) ) return -2;
	if ( (y[0] > y[2]) || (y[2] < y[4]) ) return -2;
	if ( (y[0] == y[2]) && (y[2] == y[4]) ) return -2;

	// If the input tolerance is nonpositive, set it to the default.


	// Find the three interior points.

	Left_Interior_Point( tr_feat, wgt, n_curr_pos, x, y );
	if ( Stopping_Rule_Parabolic(x, tolerance) ) return 0;
	Right_Interior_Point( tr_feat, wgt , n_curr_pos, x, y );

	// Iterate until the location of the maximum
	// is found within the specified tolerance.

	while (!Stopping_Rule_Parabolic(x, tolerance ) ) {
		err = Parabolic_Maximum( x, y, &xmax);
		if (err) break;
		Check_New_Test_Point( x, &xmax, min_displacement, epsilon);
		//ymax = f(xmax);
		wgt[ n_curr_pos ] = xmax;
		calc_ranking_measure( tr_feat, wgt );
		ymax = get_ranking_measure( tr_feat->mtype , tr_feat->npos);

		if ( xmax < x[2] ) {
			if ( ymax <= y[1] ) {
				x[0] = xmax; y[0] = ymax; Left_Interior_Point( tr_feat,wgt, n_curr_pos, x, y );
			}
			else if ( ymax <= y[2] ) {
				x[0] = x[1]; y[0] = y[1]; x[1] = xmax; y[1] = ymax;
			}
			else {
				x[4] = x[3]; y[4] = y[3]; x[3] = x[2]; y[3] = y[2];
				x[2] = xmax; y[2] = ymax;
			}
		}
		else {
			if ( ymax < y[3] ) {
				x[4] = xmax; y[4] = ymax; Right_Interior_Point( tr_feat, wgt , n_curr_pos, x, y );
			}
			else if ( ymax <= y[2] ) {
				x[4] = x[3]; y[4] = y[3]; x[3] = xmax; y[3] = ymax;
			}
			else {
				x[0] = x[1]; y[0] = y[1]; x[1] = x[2]; y[1] = y[2];
				x[2] = xmax; y[2] = ymax;
			}
		}
	}

	// If no errs were detected, return the final 
	// interval and an internal point together with
	// the value of the function evaluated there.

	if (err == 0) {
		x0[0] = x[1];
		x0[1] = x[2];
		x0[2] = x[3];
		fx0[0] = y[1];
		fx0[1] = y[2];
		fx0[2] = y[3];
	}

	return err;
}


////////////////////////////////////////////////////////////////////////////////
//  static int Stopping_Rule_Parabolic(double x[], double tolerance)                    //
//                                                                            //
//  Description:                                                              //
//     Given an ordered array x[0] < x[1] < ... < x[4], this routine returns  //
//     TRUE (1) if  x[3] - x[1] < epsilon * (|xm| + eta) or FALSE (0)         //
//     otherwise, where f(x[2]) is the maximum among f(x[i]), i = 0,...,4,    //
//     epsilon = tolerance, and eta = 1.0 if xm <= 0.0 otherwise              //
//     eta = 0.0.                                                             //
//                                                                            //
//     For C compilers which support the 'inline' type qualifier, this routine//
//     could be declared with the 'inline' qualifier.                         //
//                                                                            //
//  Return Values:                                                            //
//     0  Bound is not within the specified tolerance.                        //
//     1  Bound is within the specified tolerance.                            //
////////////////////////////////////////////////////////////////////////////////
static int Stopping_Rule_Parabolic(double x[], double tolerance)
{
	double xmid = 0.5 * fabs( x[3] + x[1] );
	double eta = (xmid <= 1.0) ? 1.0 : fabs(xmid);

	return ( ( x[3] - x[1] ) <= tolerance * eta ) ? 1 : 0;
}


////////////////////////////////////////////////////////////////////////////////
//  static int Parabolic_Maximum( double x[], double y[], double *xmax)       //
//                                                                            //
//  Description:                                                              //
//     Given three noncollinear points (x[1],y[1]), (x[2],y[2]), and          //
//     (x[3],y[3]) such that x[1] < x[2] < x[3] and y[1] < y[2] > y[3] find   //
//     the location of the maximum, xmax, of the parabola through these       //
//     points.                                                                //
//                                                                            //
//     For C compilers which support the 'inline' type qualifier, this routine//
//     could be declared with the 'inline' qualifier.                         //
//                                                                            //
//  Return Values:                                                            //
//     0  Success                                                             //
//    -1  The three points are either collinear or the extremum of the        //
//        parabola through the three points is a minimum.                     //
////////////////////////////////////////////////////////////////////////////////
static int Parabolic_Maximum( double x[], double y[], double *xmax) 
{
	double d1 = (x[3] - x[2]) * (y[1] - y[2]);
	double d2 = (x[2] - x[1]) * (y[3] - y[2]);
	double denominator = d1 + d2;
	double numerator = (x[3] + x[2]) * d1 + (x[2] + x[1]) * d2;

	if (denominator >= 0.0) { *xmax = 0.0; return -1; }
	*xmax = 0.5 * numerator / denominator;
	return 0;
}


static void Check_New_Test_Point( double x[], double *xmax, double min_displacement, double epsilon )
{
	double eta;
	double d1, d2;

	// Compare the location of the minimum of the
	// parabolic fit to the center node.

	eta = ( x[2] == 0.0 ) ? 1.0 : fabs(x[2]);
	if ( fabs(*xmax - x[2]) < (min_displacement * eta) ) {
		if ( *xmax == x[2] ) 
			if ( (x[3] - x[2]) > ( x[2] - x[1] ) )
				*xmax = x[2] + alpha * (x[3] - x[2]);
			else *xmax = x[2] - alpha * (x[2] - x[1]);
		else if ( *xmax < x[2] ) 
			if ( x[2] - x[1] < epsilon * eta )
				*xmax = x[2] + (x[2] - x[1]);
			else {
				d1 = epsilon * eta;
				d2 = alpha * (x[2] - x[1]);
				*xmax = x[2] - ( (d1 < d2) ? d1 : d2 );
			}
		else 
			if ( x[3] - x[2] < epsilon * eta )
				*xmax = x[2] - (x[3] - x[2]);
			else {
				d1 = epsilon * eta;
				d2 = alpha * (x[3] - x[2]);
				*xmax = x[2] + ( (d1 < d2) ? d1 : d2 );
			}
		return;
	}

	// Compare the location of the maximum of the
	// parabolic fit to the left-most node.

	eta = ( x[1] == 0.0 ) ? 1.0 : fabs(x[1]);
	if ( (*xmax - x[1]) < (min_displacement * eta)) {
		d1 = epsilon * eta;
		d2 = alpha * (x[2] - x[1]);
		*xmax = x[1] + ( (d1 < d2) ? d1 : d2 );
		return;
	}

	// Compare the location of the maximum of the
	// parabolic fit to the right-most node.

	eta = ( x[3] == 0.0 ) ? 1.0 : fabs(x[3]);
	if ( (x[3] - *xmax) < (min_displacement * eta)) {
		d1 = epsilon * eta;
		d2 = alpha * (x[3] - x[2]);
		*xmax = x[3] - ( (d1 < d2) ? d1 : d2 );
		return;
	}

	return;
}


////////////////////////////////////////////////////////////////////////////////
//  static void Left_Interior_Point( double (*f)(double), double x[],         //
//                                                              double y[] )  //
//                                                                            //
//  Description:                                                              //
//     Select the new test point using the points (x[0],y[0]), (x[2],y[2]),   //
//     and (x[4],y[4]),  where x[0] < x[2] < x[4] and y[0] < y[2] > y[4].     //
//     Upon return the points (x[0],y[0]), (x[1],y[1]), (x[2],y[2]),          //
//     (x[4],y[4]) are defined with x[0] < x[1] < x[2] < x[4] and             //
//     y[0] < y[1] < y[2] > y[4], where y[i] = f(x[i]).                       //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
static void Left_Interior_Point( feat_data_s *tr_feat, double *wgt , int n_curr_pos, double x[], double y[] )
{
	int not_found = 1;
	int	nCnt = 0;

	//while (not_found && nCnt <= 1000 ) { 
	while (not_found ) { 
		x[1] = x[2] - alpha * (x[2] - x[0]);
		//y[1] = f(x[1]);
		wgt[ n_curr_pos ] = x[1];
		calc_ranking_measure( tr_feat, wgt );
		y[1] = get_ranking_measure( tr_feat->mtype , tr_feat->npos);

		if ( y[1] < y[0] ) { 
			x[0] = x[1]; y[0] = y[1]; 
		}
		else if ( y[1] <= y[2] ) not_found = 0;
		else {
			x[4] = x[3]; y[4] = y[3];
			x[3] = x[2]; y[3] = y[2];
			x[2] = x[1]; y[2] = y[1];
		}
		nCnt++;
	}
}


////////////////////////////////////////////////////////////////////////////////
//  static void Right_Interior_Point( double (*f)(double), double x[],        //
//                                                              double y[] )  //
//                                                                            //
//  Description:                                                              //
//     Given four points (x[0],y[0]), (x[1],y[1]), (x[2],y[2]), and           //
//     (x[4],y[4]), such that x[0] < x[1] < x[2] < x[4] and y[0] < y[1] <     //
//     y[2] > y[4] find points (x[0],y[0]), (x[1],y[1]), (x[2],y[2]),         //
//     (x[3],y[3]) and (x[4],y[4]), such that x[0] < x[1] < x[2] < x[3] < x[4]//
//     y[0] < y[1] < y[2] > y[3] > y[4], where y[i] = f(x[i]).                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
static void Right_Interior_Point( feat_data_s *tr_feat, double *wgt , int n_curr_pos, double x[], double y[] )
{
	int not_found = 1;

	int	nCnt = 0;


	//while (not_found && nCnt <= 10) { 
	while (not_found ) { 
		x[3] = x[2] + alpha * (x[4] - x[2]);
		//y[3] = f(x[3]);
		wgt[ n_curr_pos ] = x[3];
		calc_ranking_measure( tr_feat, wgt );
		y[3] = get_ranking_measure( tr_feat->mtype , tr_feat->npos);

		if ( y[3] < y[4] ) { x[4] = x[3]; y[4] = y[3]; }
		else if ( y[3] <= y[2] ) not_found = 0;
		else {
			x[0] = x[1]; y[0] = y[1];
			x[1] = x[2]; y[1] = y[2];
			x[2] = x[3]; y[2] = y[3];
		}

		nCnt++;
	}

}




int max_search_parabolic_extrapolation( feat_data_s *tr_feat, double *wgt, int n_curr_pos, double *x, double *fx , double bound_min, double bound_max)
{
	int return_code = 0;
	double xmax, ymax;
	double	min_displacement ;


	double	a;
	double	b;
	double	cut_off_scale_factor = 10.0;

	int		nCnt = 0;

	//min_displacement = sqrt(DBL_EPSILON);
	min_displacement = 1.0e-10;



	//a = -DBL_MAX;
	//b = DBL_MAX;

	a = bound_min;
	b = bound_max;

	wgt[ n_curr_pos ] = x[0]; 
	calc_ranking_measure( tr_feat, wgt );
	fx[0] = get_ranking_measure( tr_feat->mtype , tr_feat->npos);

	wgt[ n_curr_pos ] = x[2]; 
	calc_ranking_measure( tr_feat, wgt );
	fx[2] = get_ranking_measure( tr_feat->mtype , tr_feat->npos);


	if( x[0] == x[2] ){
		return -2;
	}
	// Set x[1] such that x[0] < x[1] < x[2]
	if ( fx[0] > fx[2] ) x[1] = x[0] + alpha * (x[2] - x[0]);
	x[1] = x[2] - alpha * (x[2] - x[0]);
	wgt[ n_curr_pos ] = x[1]; 
	calc_ranking_measure( tr_feat, wgt );
	fx[1] = get_ranking_measure( tr_feat->mtype , tr_feat->npos);

	// Iterate until either f(x[0]) < f(x[1]) > f(x[2])
	// or x[0] == a or x[2] == b.

	while ( ( fx[0] >= fx[1] ) || ( fx[2] >= fx[1] ) ) {
		xmax = New_Test_Point_Ex( x, fx, a, b, min_displacement,  cut_off_scale_factor );
		if ( (xmax == a) || (xmax == b) ) { return_code = -1; break; }
		if ( fx[0] == fx[1] && fx[1] == fx[2]  ) { return_code = 0; break; }

		if( nCnt++ >= 50 ) {
			return_code = -1;
			break;
		}

		//ymax = f(xmax);
		wgt[ n_curr_pos ] = xmax; 
		calc_ranking_measure( tr_feat, wgt );
		ymax = get_ranking_measure( tr_feat->mtype , tr_feat->npos);

		if ( xmax < x[0] ) {
			x[2] = x[1]; fx[2] = fx[1];
			x[1] = x[0]; fx[1] = fx[0];
			x[0] = xmax; fx[0] = ymax;
		} 
		else if ( xmax > x[2] ) {
			x[0] = x[1]; fx[0] = fx[1];
			x[1] = x[2]; fx[1] = fx[2];
			x[2] = xmax; fx[2] = ymax;
		}
		else if ( xmax < x[1] )
			if  (( ymax <= fx[0] ) && ( ymax >= fx[1] )) {
				x[2] = x[1]; fx[2] = fx[1];
				x[1] = xmax; fx[1] = ymax;
			}
			else if ( ymax < fx[1] ) { x[0] = xmax; fx[0] = ymax; break; }
			else { x[2] = x[1]; fx[2] = fx[1]; x[1] = xmax; fx[1] = ymax; break; }
		else 
			if  (( ymax <= fx[2] ) && ( ymax >= fx[1] )) {
				x[0] = x[1]; fx[0] = fx[1];
				x[1] = xmax; fx[1] = ymax;
			}
			else if ( ymax < fx[1] ) { x[2] = xmax; fx[2] = ymax; break; }
			else if ( ymax > fx[2] ) { x[1] = xmax; fx[1] = ymax; break; }
	} 

	
	return return_code;
}


////////////////////////////////////////////////////////////////////////////////
//  static double New_Test_Point( double x[], double y[], double a, double b, //
//                     double min_displacement, double cut_off_scale_factor ) //
//                                                                            //
//  Description:                                                              //
//     Given three points (x[0],y[0]), (x[1],y[1]), and (x[2],y[2]) such that //
//     x[0] < x[1] < x[2] find the location of the next test point, normally  //
//     the maximum, xmax, of the parabola through these points.               //
//                                                                            //
//     For C compilers which support the 'inline' type qualifier, this routine//
//     could be declared with the 'inline' qualifier.                         //
//                                                                            //
//  Return Values:                                                            //
//     The next test point.                                                   //
////////////////////////////////////////////////////////////////////////////////
static double New_Test_Point_Ex( double x[], double y[], double a, double b, double min_displacement, double cut_off_scale_factor ) 
{
	double xmax;
	double eta;

	xmax = Parabolic_Maximum_Ex( x, y, cut_off_scale_factor );

	// Compare the location of the minimum of the
	// parabolic fit to the left-most node.

	eta = ( x[0] == 0.0 ) ? 1.0 : fabs(x[0]);
	if ( fabs(xmax - x[0]) < (min_displacement * eta) ) 
		xmax -= 100.0 * min_displacement * eta;
	else {
		// Compare the location of the minimum of the
		// parabolic fit to the center node.

		eta = ( x[1] == 0.0 ) ? 1.0 : fabs(x[1]);
		if ( fabs(xmax - x[1]) < (min_displacement * eta) ) 
			if ( y[2] > y[0] )  
				xmax = x[1] + alpha * (x[2] - x[1]);
			else 
				xmax = x[1] - alpha * (x[1] - x[0]);
		else {
			// Compare the location of the minimum of the
			// parabolic fit to the right-most node.

			eta = ( x[2] == 0.0 ) ? 1.0 : fabs(x[2]);
			if ( fabs(x[2] - xmax) < (min_displacement * eta) ) 
				xmax += 100.0 * min_displacement * eta;
		}
	}

	if (xmax < a) xmax = a;
	if (xmax > b) xmax = b;

	return xmax;
}


static double Parabolic_Maximum_Ex( double x[], double y[], double cut_off_scale_factor ) 
{
	double d1 = (x[2] - x[1]) * (y[1] - y[0]);
	double d2 = (x[1] - x[0]) * (y[2] - y[1]);
	double denominator = 2.0 * (d1 - d2);
	double numerator = (x[2] + x[1]) * d1 - (x[1] + x[0]) * d2;
	double x_lb = x[0] - cut_off_scale_factor * (x[1] - x[0]);
	double x_ub = x[2] + cut_off_scale_factor * (x[2] - x[1]);
	double ymin =( y[2] - y[0]) * (x[1] - x[0]) / (x[2] - x[0]) + y[0];
	double xmax;

	if (y[1] <= ymin) xmax = ( y[0] > y[2] ) ? x_lb : x_ub;
	else {
		if ( x_lb * denominator >= numerator ) xmax = x_lb;
		else if ( x_ub * denominator <= numerator ) xmax = x_ub;
		else  xmax = numerator / denominator;
	}
	return xmax;
}

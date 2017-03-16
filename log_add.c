#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "log_add.h"


//static double	log_tbl[MAX_LOG_TABLE+1];
static double	*log_tbl = NULL;

int 	free_log_table(void)
{
	if( log_tbl )
	   	free( log_tbl );

	return 0;
}

int     make_log_table(void)
{
	int     j;
	double  init_value, fvalue, step;
	double  ftmp;

	init_value = 0.0;
	step = LOG_STEP;

	log_tbl = (double *)calloc( MAX_LOG_TABLE + 1, sizeof(double));

	for(j=0,fvalue=init_value;j<MAX_LOG_TABLE;j++,fvalue-=step)
	{
		ftmp = exp(fvalue);
		log_tbl[j] = LOG(1.0+ftmp);
	}
	log_tbl[j] = 0.0;

	return(0);
}

double  log_addition( double x, double y )
{
	int	idx;
	double	max_p, diff;
	double	fvalue;

	diff = ABS(x-y);
	max_p = MAX1(x,y);

	if ( diff > LOG_DIFF ){
		return(max_p);
	}
	else
	{
		idx = LOG_IDX(diff);
		fvalue = max_p + (double)log_tbl[idx];
		return(fvalue);
	}

}


#ifdef LOG_DEBUG
int main(int argc, char **argv)
{

	double bx;
	double ii, kk;

	make_log_table();

	bx = LZERO;


	bx = log_addition( bx, atof(argv[1]));
	fprintf(stdout,"%f %f\n", bx, atof(argv[1]));

	bx = log_addition( bx, atof(argv[2]));
	fprintf(stdout,"%f %f\n", bx, atof(argv[2]));

	ii = exp(atof(argv[1]));
	kk = exp(atof(argv[2]));

	fprintf(stdout,"%f %f %f %f \n", kk - ii, atof(argv[1]), atof(argv[2]), log( ii + kk ));
}

#endif

#ifndef __TYEPS_H__
#define __TYEPS_H__

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#define		SQ(x)		((x) * (x))
#define		INFINITY	1E30	
#define		EPSILON         1E-30
#define     SQR(x)       ((x) * (x))
#define		ITRBND             100	
#define		CRIT_BOUND         1.0e-8    /* distortion convergence bound */
#define		PERT               0.05     /* perturbation quantity */
#define		BIG_NUM            1e10
#define		MAX_SPLIT_NUM		200


#define GRID_SIZE	10
#define GRID_SIZE_S	10000
#define NUM_INIT	20
#define NUM_TRAIN	1
#define MIN_BOUND	-10
#define MAX_BOUND	10
#define NUM_VQ	16
#define MIN_POINT	25


#define NUM_TOP	500

#define	TRUE	1
#define	FALSE	0
#define	fCnvgTh 1.0e-6

typedef struct tag_srch_point {
	int		flag;
	int		nDim;
	double	*wgt;
	double	measure;
} srch_point_s;

#endif

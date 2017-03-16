#ifndef __LOG_ADD_H__
#define __LOG_ADD_H__

#define	LZERO	(-1.0E10)
#define	LOG_DIFF	50.0	
#define	LOG_STEP	0.0001
#define	LOG_PROD	(int)(1.0 / LOG_STEP)
#define	MAX_LOG_TABLE (int)(LOG_DIFF / LOG_STEP)	
#define	LOG_IDX(x)	((int)( (x)*LOG_PROD ))

#define	LOG2	0.693147180
#define	ILOG2	1.442695041

#define	MINUSINFINITY	(-1E31)
#define	ABS(x)		(((x) >= 0) ? (x) : -(x))
#define	MIN1(x,y)	(((x) < (y)) ? (x) : (y))
#define	MAX1(x,y)	(((x) > (y)) ? (x) : (y))
#define	LOG(x)	(((x)>0.) ? (double)log((double)(x)) : MINUSINFINITY)

extern double  log_addition( double x, double y );
extern int make_log_table(void);
extern int free_log_table(void);

#endif


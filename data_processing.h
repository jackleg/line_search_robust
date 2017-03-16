#ifndef __DATA_PROCESSING_H__
#define __DATA_PROCESSING_H__

#include "types.h"

extern int outlier_detect( srch_point_s **sr_p, int *ntop_measure_idx , int nData );
extern int vector_quantize( srch_point_s **sr_p, int nData, double **mean, int *ncls );

#endif

#ifndef __TIME_CHECK_H__
#define __TIME_CHECK_H__

void timing_init ( void );
void timing_start ( void );
void timing_stop (void);
float cpu_elapsed_time(void);
float user_elapsed_time(void);

#endif

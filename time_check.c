#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>



static struct rusage start, stop;
static struct timeval e_start, e_stop;

static float TotalCPUTime, TotalElapsedTime;


double MakeSeconds (struct timeval const *s, struct timeval const *e)
{
	return ((e->tv_sec - s->tv_sec) + ((e->tv_usec - s->tv_usec) / 1000000.0));
}

void timing_init ( void )
{ 
	TotalCPUTime = TotalElapsedTime = 0.0;
}

void timing_start ( void )
{

	    getrusage (RUSAGE_SELF, &start);
		gettimeofday (&e_start, 0);
}


void timing_stop (void)
{

	//TotalSpeechTime += searchFrame()*0.01f;

	getrusage (RUSAGE_SELF, &stop);
	gettimeofday (&e_stop, 0);


	TotalCPUTime = MakeSeconds (&start.ru_utime, &stop.ru_utime);
	TotalElapsedTime = MakeSeconds (&e_start, &e_stop);

	//fprintf(stderr,"%6.2f sec elapsed", TotalElapsedTime);
	//fprintf(stderr,"%6.2f sec CPU", TotalCPUTime);
	//fprintf(stderr,"\n\n");


}

float cpu_elapsed_time(void)
{
	return TotalCPUTime;
}

float user_elapsed_time(void)
{
	return TotalElapsedTime;
}





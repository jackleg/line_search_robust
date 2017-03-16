CC     = gcc
CFLAGS = -O3 -Wall

#CC=/opt/gcc-3.4.6-bc/bin/gcc
#CFLAGS = -g -Wall -fbounds-checking


OBJS = linesearch_drvr.o \
	   util.o \
	   listmergesort.o \
	   data_processing.o \
	   listsort.o \
	   lnlist.o \
	   Smemory.o \
	   getargs.o\
	   linesearch.o\
	   listutil.o\
	   ranking_measure.o\
	   time_check.o\
	   find_max_point.o\
	   nrutil.o\
	   log_add.o

SRCS = $(OBJS:.o=.c)

HEADERS=Smemory.h lnlist.h log_add.h linesearch.h listutil.h ranking_measure.h find_max_point.h nrutil.h  types.h
		


LRDRVR = linesrch

all : $(LRDRVR)

$(LRDRVR) : $(OBJS) $(HEADERS)
	$(CC) -o $@ $(CFLAGS) $(OBJS) -lm

touch :
	touch *.c

clean : 
	rm -f *.o $(LRDRVR)

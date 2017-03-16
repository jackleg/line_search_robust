
///////////////////////
//                   //
// list-merge-sort.c //
//                   //
///////////////////////

#include <stdio.h>
#include <stdlib.h>

typedef double Data_T ;

#include "lnlist.h"

void merge_sort(Data_T data[], int first, int last) 
{
	LnList *listHead   ;
	LnList *list       ;
	int     idx        ; 
	extern void Sort() ;

	InitList (&listHead) ;

	for (idx = first ; idx <= last ; ++idx) 
		AddItem(data[idx], idx, &listHead) ;	

	// Now, Perform List-Merge-Sort
	//Sort((void **)&listHead, NodeComp) ;
	Sort((void *)(&listHead), NodeComp) ;

	idx  = first - 1 ;
	list = listHead  ;
        while(list) {
	    data[++idx] = list->data_val ;
            list = list->next ;
        }

	if (idx != last) {
	    fprintf(stderr,"Error in %s:%d\n",__FILE__,__LINE__) ;
	    exit(1) ;
	}

	FreeLnList(listHead) ;
	return ;

}

/////////////////////////////////////////////////////////

void merge_sortidx(Data_T data[], int dataidx[], int first, int last) 
{
	LnList *listHead   ;
	LnList *list       ;
	int     idx        ; 
	extern void Sort() ;

	InitList (&listHead) ;

	for (idx = first ; idx <= last ; ++idx) 
		AddItem(data[idx], dataidx[idx], &listHead) ;	

	// Now, Perform List-Merge-Sort
	//Sort((void **)&listHead, NodeComp) ;
	Sort((void *)(&listHead), NodeComp) ;

	idx  = first - 1 ;
	list = listHead  ;
        while(list) {
	    data[++idx]  = list->data_val ;
	    dataidx[idx] = list->idx      ;
            list = list->next ;
        }

	if (idx != last) {
	    fprintf(stderr,"Error in %s:%d\n",__FILE__,__LINE__) ;
	    exit(1) ;
	}

	FreeLnList(listHead) ;
	return ;

}


#ifdef DRIVER

Data_T x[10] = { 4, 3, 2, 1, 1, 1, 2, 3, 4, 3 } ;
Data_T y[10] = { 4, 3, 2, 1, 1, 1, 2, 3, 4, 3 } ;
int    k[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } ;

main() {
	int i ;

	merge_sort(x, 0, 9) ;

	for (i = 0 ; i < 10 ; ++i)
	   printf("%f\n",x[i]) ;

	merge_sortidx(y,k,2,7) ;

	for (i = 0 ; i < 10 ; ++i)
	   printf("%d %f\n",k[i], y[i]) ;

}

#endif


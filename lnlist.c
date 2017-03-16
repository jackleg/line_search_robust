
/******************************
 *                            *
 * lnlist.c                   *
 *                            *
 * by Sangho Lee              *
 *                            *
 ******************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef double Data_T ;

typedef struct _lnlist_ {
	struct _lnlist_ *next     ;
	Data_T           data_val ;
	int              idx      ;
} LnList ;

//#define NULL '\0' 

//////////////////////////////////////////////////////////////////////

//////////////
// InitList //
//////////////
void InitList (LnList **listHead)
{
	*listHead = NULL ;
}

//////////////////////////////////////////////////////////////////////

/////////////
// AddItem //
/////////////
void AddItem(Data_T data_val, int idx, LnList **listHead)
{
	LnList *newItem = (LnList *) malloc(sizeof(LnList)) ;
	LnList **rpItem = listHead  ; 
	LnList *item    = *listHead ;

	newItem->data_val = data_val ;
	newItem->idx      = idx      ; 
	newItem->next     = NULL     ;

	*rpItem = newItem    ; 
	newItem->next = item ; 
}

//////////////////////////////////////////////////////////////////////

//////////////////
// AddNSortItem //
//////////////////
void AddNSortItem(Data_T data_val, int idx, LnList **listHead)
{
	LnList *newItem = (LnList *) malloc(sizeof(LnList)) ;
	LnList **rpItem = listHead  ; 
	LnList *item    = *listHead ;

	newItem->data_val = data_val ;
	newItem->idx      = idx      ; 
	newItem->next     = NULL     ;

	while (item && (newItem->data_val > item->data_val)) {
	  rpItem = &item->next ; 
	  item = item->next ; 
	}

	*rpItem = newItem    ; 
	newItem->next = item ; 
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////
// DeleteItemFromSortedList //
//////////////////////////////

void DeleteItemFromSortedList (Data_T data_val, LnList **listHead)
{
	LnList **rpItem = listHead ; 
	LnList *item    = *listHead ;

	while (item && (data_val > item->data_val)) {
	  rpItem = &item->next ; 
	  item = item->next ; 
	}

	if (item && (data_val == item->data_val)) {
	  *rpItem = item->next ; 
	  free (item) ; 
	}

}

//////////////////////////////////////////////////////////////////////

/////////////
// ListLen //
/////////////
int ListLen (LnList *listHead)
{
	LnList *nextptr = listHead ;
	int retval = 0 ; 

        while(nextptr) {
	    ++retval ;
	    nextptr = nextptr->next ;
	}

	return retval ;
}

//////////////////////////////////////////////////////////////////////

////////////////
// FreeLnList //
////////////////

void FreeLnList(LnList *ptr)
{
      LnList *nextptr ;

      while(ptr) {
          nextptr = ptr->next ; 
          free(ptr) ;
          ptr = nextptr ;
      }

}

//////////////////////////////////////////////////////////////////////

int NodeComp(void *a, void *b)
{
        return ( ((LnList *) a)->data_val < ((LnList *) b)->data_val ) ? 1 : 0 ; 
}

//////////////////////////////////////////////////////////////////////

int InverseNodeComp(void *a, void *b)
{
        return ( ((LnList *) a)->data_val <= ((LnList *) b)->data_val ) ? 0 : 1 ; 
}

//////////////////////////////////////////////////////////////////////

#ifdef LNLISTDRIVER

extern void Sort() ;

main()
{
	LnList *listHead ; 
	LnList *list     ; 

        InitList(&listHead) ;

	AddItem(1.0, 3, &listHead) ; 
	AddItem(2.0, 2, &listHead) ; 
	AddItem(3.0, 4, &listHead) ; 
	AddItem(4.0, 1, &listHead) ; 
	AddItem(5.0, 5, &listHead) ; 

        ///////////////////
	// scanning list //
        ///////////////////

	list = listHead ;
	while(list) {
	    printf("%f %d, ", list->data_val, list->idx) ;
	    list = list->next ;
	}

        /////////////////////////
        // Sorting Linked List //
        /////////////////////////
        printf("Sorting the Linked List.\n") ;
        Sort((void **)&listHead, NodeComp) ; 
        // Sort((void **)&listHead, InverseNodeComp) ; 


	list = listHead ;
	while(list) {
	    printf("%f %d, ", list->data_val, list->idx) ;
	    list = list->next ;
	}



	printf("length : %d\n",ListLen (listHead)) ;

	FreeLnList(listHead) ; 
	
}

#endif



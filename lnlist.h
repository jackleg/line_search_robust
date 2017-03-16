
/*****************
 *               *
 * lnlist.h      *
 *               *
 * by Sangho Lee *
 *               *
 *****************/

typedef struct _lnlist_ {
	struct _lnlist_ *next     ;
	Data_T           data_val ;
	int              idx      ;
} LnList ;


#ifndef NULL
#define NULL '\0' 
#endif

extern void InitList (LnList **listHead)                                  ;
extern void AddItem(Data_T data_val, int idx, LnList **listHead)          ;
extern void AddNSortItem(Data_T data_val, int idx, LnList **listHead)     ;
extern void DeleteItemFromSortedList (Data_T data_val, LnList **listHead) ;
extern int  ListLen (LnList *listHead)                                    ;
extern void FreeLnList(LnList *ptr)                                       ;
extern int  NodeComp(void *a, void *b)                                    ;
extern int  InverseNodeComp(void *a, void *b)                             ;



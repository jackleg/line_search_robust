
/*
 *
 * Linked List Sort
 *
 * Adopted from Dr. Dobb's Journal, June 1999 pp. 125-128
 *
 * by Sangholee
 *
 */

typedef struct GenericNode GenericNode;
struct GenericNode { GenericNode *next; };
void Sort(void ** pList, int (*comp) (void *, void *))
{
  int outindex;                 /* current output list (0 or 1) */
  GenericNode *p;               /* Scratch variable */
  GenericNode *in[2], *out[2];  /* Input/Output lists */
  GenericNode **outTail[2];     /* Track last items in output lists */
  GenericNode *lastOut;         /* Last node output */

  if(!*pList) return;           /* Empty list is already sorted */
  out[0] = *pList;              /* point out[0] to the list to be sorted */
  out[1] = 0;

  do {
    in[0] = out[0];             /* Move output lists to input lists */
    in[1] = out[1];

    if (!in[1]) {        /* Only one list? Grab first item from other list */
      p = in[0]; if(p) in[0] = in[0]->next;
    } else {             /* There are two lists, get the smaller item */
      int smallList = comp(in[0],in[1])  ? 0 : 1;
      p = in[smallList]; if(p) in[smallList] = in[smallList]->next;
    }
    /* Initialize out[0] to first item, clear out[1] */
    out[0] = p; outTail[0] = &(p->next); lastOut=out[0];
    p->next = (GenericNode *)0;
    outindex = 0;
    out[1] = (GenericNode *)0; outTail[1] = &(out[1]);

    while (in[0] || in[1]) {        /* while either list is not empty */
      if (!in[1]) {                 /* Second list empty, choose first */
        p = in[0]; if(p) in[0] = in[0]->next;
        if(comp(p,lastOut))         /* p < lastOut */
          outindex = 1-outindex;    /* switch lists */
      } else if (!in[0]) {          /* First list empty, choose second */
        p = in[1]; in[1] = in[1]->next;
        if(comp(p,lastOut))         /* p < lastOut */
          outindex = 1-outindex;    /* switch lists */
      } else if (comp(in[0],lastOut)) { /* in[0] < lastOut */
        if(!comp(in[1],lastOut)) {  /* lastOut <= in[1] */
          p = in[1]; in[1] = in[1]->next;
        } else {                    /* in[1] < lastOut */
          if(comp(in[0],in[1])) {   /* in[0] < in[1] */
            p = in[0]; in[0] = in[0]->next;
          } else {
            p = in[1]; in[1] = in[1]->next;
          }
          outindex = 1-outindex;    /* Switch lists */
        }
      } else {                     /* lastOut <= in[0] */
        if(comp(in[1],lastOut)) {  /* in[1] < lastOut */
          p = in[0]; in[0] = in[0]->next;
        } else {                   /* lastOut <= in[1] */
          if(comp(in[0],in[1])) {  /* in[0] < in[1] */
            p = in[0]; in[0] = in[0]->next;
          } else {
            p = in[1]; in[1] = in[1]->next;
          }
        }
      }
      *outTail[outindex] = p;
      outTail[outindex] = &(p->next);
      p->next = (GenericNode *)0;
      lastOut = p;
    }
  } while (out[1]);
  *pList = out[0];
}
///////////////////////////////////////////////////////////////////////


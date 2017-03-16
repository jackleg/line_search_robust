/******************************************************************************
*                                                                             *
*       Copyright (C) 1993,1994 Tony Robinson				      *
*                                                                             *
*       See the file SLICENSE for conditions on distribution and usage	      *
*                                                                             *
******************************************************************************/

# include <stdio.h>
# include <stdlib.h>
# include <stddef.h>


#undef  uchar
#define uchar   unsigned char
#undef  ushort
#define ushort  unsigned short
#undef  uint
#define uint    unsigned int
#undef  ulong
#define ulong   unsigned long


#define Fopen(name,mode) _Fopen((name),(mode),__FILE__,__LINE__)

#ifdef __STDC__
extern void *Svector(size_t nitem, size_t size)  ; 
	
extern FILE * _Fopen (char const *file, char const *mode, char const *srcfile, int srcline);


/* this routine relies on sizeof(char) == 1 */

extern void **Smatrix(size_t nitem0, size_t nitem1, size_t size)  ; 
extern void **alloc2d(size_t dim1, size_t dim2, size_t size);
extern int 	free2d(void **p);
extern void P2free(void **ptr);

/* Here are some useful derived functions */

extern char* SvectorChar(size_t nitem)  ;

extern uchar* SvectorUchar(size_t nitem) ;

extern short* SvectorShort(size_t nitem) ;

extern ushort* SvectorUshort(size_t nitem)  ;

extern int* SvectorInt(size_t nitem) ;

extern uint* SvectorUint(size_t nitem) ;

extern float* SvectorFloat(size_t nitem) ;

extern double* SvectorDouble(size_t nitem) ;

extern char** SmatrixChar(size_t nitem0, size_t nitem1) ;

extern uchar** SmatrixUchar(size_t nitem0, size_t nitem1) ;

extern short** SmatrixShort(size_t nitem0, size_t nitem1) ;

extern ushort** SmatrixUshort(size_t nitem0, size_t nitem1) ;

extern int** SmatrixInt(size_t nitem0, size_t nitem1) ;

extern uint** SmatrixUint(size_t nitem0, size_t nitem1) ;

extern float** SmatrixFloat(size_t nitem0, size_t nitem1) ;

extern double** SmatrixDouble(size_t nitem0, size_t nitem1) ;

extern char*** ScubicChar(size_t nitem0, size_t nitem1, size_t nitem2) ;

extern uchar*** ScubicUchar(size_t nitem0, size_t nitem1, size_t nitem2) ;

extern short*** ScubicShort(size_t nitem0, size_t nitem1, size_t nitem2) ;

extern ushort*** ScubicUshort(size_t nitem0, size_t nitem1, size_t nitem2) ;

extern int*** ScubicInt(size_t nitem0, size_t nitem1, size_t nitem2) ;

extern uint*** ScubicUint(size_t nitem0, size_t nitem1, size_t nitem2) ;

extern float*** ScubicFloat(size_t nitem0, size_t nitem1, size_t nitem2) ;

extern double*** ScubicDouble(size_t nitem0, size_t nitem1, size_t nitem2) ;


extern void eprint(char *text);
extern char *Cvec(int nx);
extern void Cfree(char *cptr);
extern short *Svec(int nx);
extern void Sfree(short *sptr);
extern int *Ivec(int nx);
extern void Ifree(int *iptr);
extern float *Fvec(int nx);
extern void Ffree(float *fptr);
extern double *Dvec(int nx);
extern void Dfree(double *dptr);
extern int *Lvec(int nx);
extern void Lfree(int *lptr);
extern char **Pvec(int nx);
extern void Pfree(char **ptr);
extern char ***P3vec(int nx);
extern void P3free(char ***ptr);
extern char ****P4vec(int nx);
extern void P4free(char ****ptr);
extern char **Cmat(int ny,int nx);
extern void Cmatfree(char **cptr,int ny);
extern short **Smat(int ny,int nx);
extern void Smatfree(short **sptr,int ny);
extern int **Imat(int ny,int nx);
extern void Imatfree(int **iptr,int ny);
extern float **Fmat(int ny,int nx);
extern void Fmatfree(float **fptr,int ny);
extern double **Dmat(int ny,int nx);
extern void Dmatfree(double **dptr,int ny);
extern int **Lmat(int ny,int nx);
extern void Lmatfree(int **lptr,int ny);
//extern FILE *Copen(char *file_name,char *mode);
//extern FILE *Fopen(char *file_name,char *mode);

extern void *m_alloc(int size);
extern void	*c_alloc(int num_of_elts,int size);
extern void	*re_alloc(void *block,int size);
extern int 	little_endian(void);
extern char	*copy_string( char *dest_str, char *src_str );


#else

extern void *Svector()  ; 

/* this routine relies on sizeof(char) == 1 */

extern void **Smatrix()  ; 

/* Here are some useful derived functions */

extern char* SvectorChar()  ;

extern uchar* SvectorUchar() ;

extern short* SvectorShort() ;

extern ushort* SvectorUshort()  ;

extern int* SvectorInt() ;

extern uint* SvectorUint() ;

extern float* SvectorFloat() ;

extern double* SvectorDouble() ;

extern char** SmatrixChar() ;

extern uchar** SmatrixUchar() ;

extern short** SmatrixShort() ;

extern ushort** SmatrixUshort() ;

extern int** SmatrixInt() ;

extern uint** SmatrixUint() ;

extern float** SmatrixFloat() ;

extern double** SmatrixDouble() ;

extern char*** ScubicChar() ;

extern uchar*** ScubicUchar() ;

extern short*** ScubicShort() ;

extern ushort*** ScubicUshort() ;

extern int*** ScubicInt() ;

extern uint*** ScubicUint() ;

extern float*** ScubicFloat() ;

extern double*** ScubicDouble() ;


extern void **alloc2d();
extern int 	free2d();
extern void P2free();


#endif


/*
 *
 * Originally Tony's code.
 *
 * S.H.Lee modified it.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#undef  uchar
#define uchar   unsigned char
#undef  ushort
#define ushort  unsigned short
#undef  uint
#define uint    unsigned int
#undef  ulong
#define ulong   unsigned long


/* vector gives a one dimensional array, matrix a two dimensional array
   and tensorx a x dimensional array. */

void *Svector(size_t nitem, size_t size)
{
  int	nbyte = nitem * size;
  char *address;

  if(nbyte == 0) nbyte = 1;

  address = malloc(nbyte);

  if(address == NULL) {
    printf("Svector(%d, %d)", (int)nitem, (int)size);
    exit(1) ;
  }

  return(address);
}

/* this routine relies on sizeof(char) == 1 */
void **Smatrix(size_t nitem0, size_t nitem1, size_t size)
{
  size_t tabsize, nbyte;
  void **array0;
  char  *array1;
  int    i;

  /* raw table size for pointers */
  tabsize = nitem0 * sizeof(void*);

  /* align to nearest item boundary */
  if(tabsize % size != 0)
    tabsize += size - tabsize % size;
  
  /* compute the total space needed */
  nbyte = tabsize + nitem0 * nitem1 * size;
  if(nbyte == 0) nbyte = 1;

  /* grab all the space */
  array0 = malloc(nbyte);

  /* exit if problems */
  if(array0 == NULL) {
    printf("Smatrix(%d, %d, %d)", (int)nitem0, (int)nitem1, (int)size);
    exit(1) ;
  }

  /* compute data area */
  array1 = (char*) array0 + tabsize;

  /* make table entries point into data */
  for(i = 0; i < nitem0; i++)
    array0[i] = array1 + i * nitem1 * size;

  return(array0);
}

/* cubic */

void ***Scubic(size_t nitem0, size_t nitem1, size_t nitem2, size_t size)
{
  size_t  nbyte1   ;
  size_t  nbyte    ;
  size_t  tabsize  ;
  size_t  tabsize1 ;
  void ***array0   ;
  char   *array1   ;
  char   *array2   ;
  int     i ,j     ;

  tabsize1 = nitem1 * sizeof(void*);

  if(tabsize1 % size != 0)
    tabsize1 += size - tabsize1 % size ;
  
  nbyte1 = tabsize1 + nitem1 * nitem2 * size;
  if(nbyte1 == 0) nbyte1 = 1;

  tabsize  = nitem0 * sizeof(void **) ; 

  if(tabsize % size != 0)
    tabsize += size - tabsize % size ;

  nbyte = tabsize + nbyte1 * nitem0 ; 
  if(nbyte == 0) nbyte = 1;

  array0 = (void ***) malloc(nbyte);

  /* exit if problems */
  if(array0 == NULL) {
    printf("Scubic(%d, %d, %d, %d)", (int)nitem0, (int)nitem1, (int)nitem2, (int)size);
    exit(1) ;
  }

  /* compute data area */
  array1 = (char*) array0 + tabsize;

  for(i = 0; i < nitem0; i++)
    array0[i] = (void **) (array1 + i * nbyte1) ;

  for (i = 0 ; i < nitem0 ; i++) {
    array2 = array1 + i * nbyte1 + tabsize1 ;

    for (j = 0 ; j < nitem1 ; j++) {
     *((void **)(array1 + i * nbyte1 + j * sizeof(void *))) = 
       array2 + j * nitem2 * size ;
    }
  }

  return(array0);
}


/* Here are some useful derived functions */
char* SvectorChar(size_t nitem)
{
  return( (char *) Svector(nitem, sizeof(char)));
}

uchar* SvectorUchar(size_t nitem)
{
  return( (uchar *) Svector(nitem, sizeof(unsigned char)));
}

short* SvectorShort(size_t nitem)
{
  return( (short *) Svector(nitem, sizeof(short)));
}

ushort* SvectorUshort(size_t nitem)
{
  return( (ushort *) Svector(nitem, sizeof(ushort)));
}

int* SvectorInt(size_t nitem)
{
  return( (int *) Svector(nitem, sizeof(int)));
}

uint* SvectorUint(size_t nitem)
{
  return( (uint *) Svector(nitem, sizeof(uint)));
}

float* SvectorFloat(size_t nitem)
{
  return( (float *) Svector(nitem, sizeof(float)));
}

double* SvectorDouble(size_t nitem)
{
  return( (double *) Svector(nitem, sizeof(double)));
}

char** SmatrixChar(size_t nitem0, size_t nitem1)
{
  return((char**) Smatrix(nitem0, nitem1, sizeof(char)));
}

uchar** SmatrixUchar(size_t nitem0, size_t nitem1)
{
  return((uchar**) Smatrix(nitem0, nitem1, sizeof(uchar)));
}

short** SmatrixShort(size_t nitem0, size_t nitem1)
{
  return((short**) Smatrix(nitem0, nitem1, sizeof(short)));
}

ushort** SmatrixUshort(size_t nitem0, size_t nitem1)
{
  return((ushort**) Smatrix(nitem0, nitem1, sizeof(ushort)));
}

int** SmatrixInt(size_t nitem0, size_t nitem1)
{
  return((int**) Smatrix(nitem0, nitem1, sizeof(int)));
}

uint** SmatrixUint(size_t nitem0, size_t nitem1)
{
  return((uint**) Smatrix(nitem0, nitem1, sizeof(uint)));
}

float** SmatrixFloat(size_t nitem0, size_t nitem1)
{
  return((float**) Smatrix(nitem0, nitem1, sizeof(float)));
}

double** SmatrixDouble(size_t nitem0, size_t nitem1)
{
  return((double**) Smatrix(nitem0, nitem1, sizeof(double)));
}

char*** ScubicChar(size_t nitem0, size_t nitem1, size_t nitem2)
{
  return((char***) Scubic(nitem0, nitem1, nitem2, sizeof(char)));
}

uchar*** ScubicUchar(size_t nitem0, size_t nitem1, size_t nitem2)
{
  return((uchar***) Scubic(nitem0, nitem1, nitem2, sizeof(uchar)));
}

short*** ScubicShort(size_t nitem0, size_t nitem1, size_t nitem2)
{
  return((short***) Scubic(nitem0, nitem1, nitem2, sizeof(short)));
}

ushort*** ScubicUshort(size_t nitem0, size_t nitem1, size_t nitem2)
{
  return((ushort***) Scubic(nitem0, nitem1, nitem2, sizeof(ushort)));
}

int*** ScubicInt(size_t nitem0, size_t nitem1, size_t nitem2)
{
  return((int***) Scubic(nitem0, nitem1, nitem2, sizeof(int)));
}

uint*** ScubicUint(size_t nitem0, size_t nitem1, size_t nitem2)
{
  return((uint***) Scubic(nitem0, nitem1, nitem2, sizeof(uint)));
}

float*** ScubicFloat(size_t nitem0, size_t nitem1, size_t nitem2)
{
  return((float***) Scubic(nitem0, nitem1, nitem2, sizeof(float)));
}

double*** ScubicDouble(size_t nitem0, size_t nitem1, size_t nitem2)
{
  return((double***) Scubic(nitem0, nitem1, nitem2, sizeof(double)));
}


void P2free (char **dptr)
{
	if(dptr==NULL) return;
	free((char *)dptr);
}


void	**alloc2d(size_t dim1, size_t dim2, size_t size)
{
	int		i;		
	size_t	nelem;	

	char	*p;	
	void	**pp;

	nelem = dim1 * dim2;

	p = (char *)calloc(nelem, size);

	pp = (void **) calloc(dim1, sizeof(char *));

	for (i = 0; i < dim1; i++)
		pp[i] = p + (i * dim2 * size);

	return (pp);
}


int 	free2d(void **p)
{

	if ( p != NULL && *p != NULL) 
		free( (void *) *p);
	if ( p != NULL)
		free( (void *) p);
	return(1);
}


FILE * _Fopen (char const *file, char const *mode, char const *srcfile, int srcline)
{
	FILE *fs = fopen (file, mode);

	if (fs == NULL) {
		fprintf (stdout, "%s(%d): fopen(%s,%s) failed\n",
				srcfile, srcline, file, mode);
		perror ("fopen");
		exit (-1);
	}

	return (fs);
}

void eprint(char *text)
{
	fprintf(stderr, "%s\n", text);
	exit(1);
}

char *Cvec(int nx)
{
	char *v;

	v=(char *)calloc(nx,sizeof(char));
	if(!v) eprint("Can't allocate memory");
	return(v);
}

void Cfree(char *cptr)
{
	if(cptr==NULL) return;
	free((char *)cptr);
}

short *Svec(int nx)
{
	short *v;

	v=(short *)calloc(nx,sizeof(short));
	if(!v) eprint("Can't allocate memory at Svec");
	return(v);
}

void Sfree(short *sptr)
{
	if(sptr==NULL) return;
	free((char *)sptr);
}

int *Ivec(int nx)
{
	int *v;

	v=(int *)calloc(nx,sizeof(int));
	if(!v) eprint("Can't allocate memory");
	return(v);
}

void Ifree(int *iptr)
{
	if(iptr==NULL) return;
	free((char *)iptr);
}

float *Fvec(int nx)
{
	float *v;

	v=(float *)calloc(nx,sizeof(float));
	if(!v) eprint("Can't allocate memory");
	return(v);
}

void Ffree(float *fptr)
{
	if(fptr==NULL) {
		return;
	}
	free((char *) fptr);
}

double *Dvec(int nx)
{
	double *v;

	v=(double *)calloc(nx,sizeof(double));
	if(!v) eprint("Can't allocate memory");
	return(v);
}

void Dfree(double *dptr)
{
	if(dptr==NULL) return;
	free((char *)dptr);
}

int *Lvec(int nx)
{
	int *v;

	v=(int *)calloc(nx,sizeof(int));
	if(!v) eprint("Can't allocate memory");
	return(v);
}

void Lfree(int *lptr)
{
	if(lptr==NULL) return;
	free((char *)lptr);
}

char **Pvec(int nx)
{
	int i;
	char **v;

	v=(char **)calloc(nx,sizeof(char *));
	if(v==NULL) printf("Error!! %d\n", nx);
	if(!v) eprint("Can't allocate memory at Pvec");
	for(i=0;i<nx;i++) v[i]=NULL;
	return(v);
}

void Pfree(char **ptr)
{
	if(ptr==NULL) return;
	free((char *)ptr);
}

char ***P3vec(int nx)
{
	int i;
	char ***v;

	v=(char ***)calloc(nx,sizeof(char **));
	if(!v) eprint("Can't allocate memory");
	for(i=0;i<nx;i++) v[i]=NULL;
	return(v);
}

void P3free(char ***ptr)
{
	if(ptr==NULL) return;
	free((char *)ptr);
}

char ****P4vec(int nx)
{
	int i;
	char ****v;

	v=(char ****)calloc(nx,sizeof(char ***));
	if(!v) eprint("Can't allocate memory");
	for(i=0;i<nx;i++) v[i]=NULL;
	return(v);
}

void P4free(char ****ptr)
{
	if(ptr==NULL) return;
	free((char *)ptr);
}

char **Cmat(int ny,int nx)
{
	int j;
	char **v;

	v=Pvec(ny);
	for(j=0;j<ny;j++) v[j]=Cvec(nx);
	return(v);
}

void Cmatfree(char **cptr,int ny)
{
	int j;

	if(cptr==NULL) return;
	for(j=0;j<ny;j++) if(cptr[j]!=NULL) Cfree(cptr[j]);	
	free((char *)cptr);
}

short **Smat(int ny,int nx)
{
	int j;
	short **v;

	v=(short **)Pvec(ny);
	for(j=0;j<ny;j++) v[j]=Svec(nx);
	return(v);
}

void Smatfree(short **sptr,int ny)
{
	int j;

	if(sptr==NULL) return;
	for(j=0;j<ny;j++) if(sptr[j]!=NULL) Sfree(sptr[j]);	
	free((char *)sptr);
}

int **Imat(int ny,int nx)
{
	int j;
	int **v;

	v=(int **)Pvec(ny);
	for(j=0;j<ny;j++) v[j]=Ivec(nx);
	return(v);
}

void Imatfree(int **iptr,int ny)
{
	int j;

	if(iptr==NULL) return;
	for(j=0;j<ny;j++) if(iptr[j]!=NULL) Ifree(iptr[j]);	
	free((char *)iptr);
}

float **Fmat(int ny,int nx)
{
	int j;
	float **v;

	v=(float **)Pvec(ny);
	for(j=0;j<ny;j++) v[j]=Fvec(nx);
	return(v);
}

void Fmatfree(float **fptr,int ny)
{
	int j;

	if(fptr==NULL) return;
	for(j=0;j<ny;j++) if(fptr[j]!=NULL) Ffree(fptr[j]);	
	free((char *)fptr);
}

double **Dmat(int ny,int nx)
{
	int j;
	double **v;

	v=(double **)Pvec(ny);
	for(j=0;j<ny;j++) v[j]=Dvec(nx);
	return(v);
}

void Dmatfree(double **dptr,int ny)
{
	int j;

	if(dptr==NULL) return;
	for(j=0;j<ny;j++) if(dptr[j]!=NULL) Dfree(dptr[j]);	
	free((char *)dptr);
}

int **Lmat(int ny,int nx)
{
	int j;
	int **v;

	v=(int **)Pvec(ny);
	for(j=0;j<ny;j++) v[j]=Lvec(nx);
	return(v);
}

void Lmatfree(int **lptr,int ny)
{
	int j;

	if(lptr==NULL) return;
	for(j=0;j<ny;j++) if(lptr[j]!=NULL) Lfree(lptr[j]);	
	free((char *)lptr);
}

FILE *Copen(char *file_name,char *mode)
{
	FILE *fp;	

	if((fp=fopen(file_name,mode))==NULL) {
		printf("Can't open file(%s)\n",file_name);
		exit(0);
	}
	return(fp);
}
FILE *Fopen(char *file_name,char *mode)
{
            FILE *fp;
	    
            if((fp=fopen(file_name,mode))==NULL) {
	            printf("Can't open file(%s)\n",file_name);
       		    exit(0);
           }
           return(fp);
}

void 	*m_alloc(int size)
{
	void 	*temp;

	if(( temp = (void *) malloc(size)) == NULL){
		printf("Memory insufficient\n");
		exit(1);
	}
	return(temp);
}

void	*c_alloc(int num_of_elts,int size)
{
	void	*temp;

	if(( temp = (void *) calloc(num_of_elts,size)) == NULL){
		printf("Memory insufficient\n");
		exit(1);
	}
	return(temp);
}

void	*re_alloc(void *block,int size)
{
	void 	*temp;

	if(( temp = (void *) realloc(block,size)) == NULL){
		printf("Memory insufficient\n");
		exit(1);
	}
	return(temp);
}


/******************************************************** 
 * 	returns non-zero, if the h/w is little endian	*
 ********************************************************/
int 	little_endian(void)
{
	char 	b[4];
	register long *l = (long *) b;

	*l = 1;
	return ((int) b[0]);

}

/* end of file */

#ifndef __LISTUTIL_H__
#define __LISTUTIL_H__

extern int GetUniqueList( double **matrix, int *qstart, int nRecord, int nlist, int nvars);
extern int GetNList(double **matrix, int nRecord);
extern void BT_ReadMatrix(double **matrix, char *inputfile, int nRecord, int nFeats);
extern void cut_tail(char *str);
extern void file_size (char *file_name, int *width, int *height);
extern void HowTo(void);

extern void WriteBeta(double beta[], int nFeats, char *parafile);
extern void WriteBeta_tmp(double beta[], int nFeats, char *parafile, int num);
extern void ReadBeta(double beta[], int nFeats, char parafile[]);
extern void EstiLNStrength(int ngroups, double **matrix, int nfeats, double *beta, char *outputfile);
extern void Write_Candidate_Model( char *parafile, int *ncls, double **cls_measure, double **mean, int nDim, int NUM_VQ, int npos);
#endif

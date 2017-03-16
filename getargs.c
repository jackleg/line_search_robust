/******************************************************************************
*                                                                             *
*       Copyright (C) 1993-1995 Tony Robinson				      *
*                                                                             *
*       See the file SLICENSE for conditions on distribution and usage	      *
*                                                                             *
******************************************************************************/

/*
  This is Sgetargs().

  The aim of this function is to provide hassle free command line
  parsing for most user applications.

  If you need tradition, use the standard getarg();
  If you need bells and whistles, use the GNU getargs();
  If you need to use non-standard syntax, write it yourself.

  This parser accepts the following syntax in argv, argc:

    argv[0] is assumed to contain the program name.
    The remainer of the line contains options then files.
    Options can either be single word boolean flags or two word
      (name, value) pairs.
    Files are either valid pathnames or '-', in which case a default
      is used.  Defaults are overwritten left to right.

  See test/argtest.c for an example of use.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>

#if defined(__STDC__)
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#define ARGFIELDWIDTH 20
#define MAXLINELENGTH 1024

/* a linked list for the help flags */
typedef struct helpl_struct {
  char* flg;	/* a help flag */
  struct helpl_struct* nxt;
} helpl_type;

/* a linked list for the command line options */
typedef struct optl_struct {
  char* flg;	/* the optional command line argument */
  char* fmt;	/* the printf format of the value of the option */
  char  typ;    /* the scanf format of the value of the option */
  void* val;	/* a pointer to the storage location of the default value */
  char* msg;	/* a message explaining the usage of this option */
  struct optl_struct* nxt;
} optl_type;

/* a linked list for the command line files */
typedef struct filel_struct {
  char*  flg;	/* the generic file type */
  char*  mod;	/* the mode in which the file is to be opened */
  FILE** fil;	/* the pointer to the default file and returned file */
  char** nam;	/* the pointer to the name of the returned file */
  char*  msg;	/* a message explaining the usage of this file */
  struct filel_struct* nxt;
} filel_type;

/* The routine Spanic exits with status 1 and a printf style error message */

#if defined(__STDC__)
void
Spanic(char *fmt,...)
#else
void
Spanic(fmt, va_alist)
char *fmt ;
va_dcl
#endif
{
  va_list args;

#if defined(__STDC__)
  va_start(args, fmt);
#else
  va_start(args);
#endif

  (void) vfprintf(stderr, fmt, args);
  va_end(args);
  exit(1);
}

/* The routine Sperror provdives a printf style error message then calls
   perror */

#if defined(__STDC__)
void
Sperror(char *fmt,...)
#else
void
Sperror(fmt, va_alist)
char *fmt ;
va_dcl
#endif
{
  va_list args;

#if defined(__STDC__)
  va_start(args, fmt);
#else
  va_start(args);
#endif

  (void) vfprintf(stderr, fmt, args);
  va_end(args);

  (void) fprintf(stderr, ": ");
  perror(NULL);
  exit(errno);
}


/* checks a flag with a list of help flags and returns the match */
int checkHelp(char *flag, helpl_type *helpl) 
{
  int match = 0;
  for(match = 0; helpl != NULL && !match; helpl = helpl->nxt)
    match = (strcmp(flag, helpl->flg) == 0);
  return(match);
}

/* print the data at *val with type typ according to format fmt */
void sprintfptr(char *string, char *fmt, char typ, void *val) 
{
  if(typ == 'c')
    sprintf(string, fmt, *((char*) val));
  else if(typ == 's')
    if(*(char**) val != NULL)
      sprintf(string, fmt, *((char**) val));
    else
      sprintf(string, "(null)");
  else if(typ == 'd')
    sprintf(string, fmt, *((int*) val));
  else if(typ == 'f')
    sprintf(string, fmt, *((float*) val));
  else {
    fprintf(stderr, "\nSgetargs: format not recognised: %s\n", fmt);
    fprintf(stderr, "Sgetargs: please write me!\n");
    exit(1);
  }
}

/* print the data at *val with type typ according to format fmt */
void fprintfptr(FILE *stream, char *fmt, char typ, void *val) 
{
  char line[MAXLINELENGTH];

  sprintfptr(line, fmt, typ, val);
  fprintf(stream, "%s", line);
}

/* scan a string into *val with type typ according to format fmt */
void sscanfptr(char *string, char *fmt, char typ, void *val) 
{
  if(typ == 'c')
    sscanf(string, "%c", ((char*) val));
  else if(typ == 's')
    *(char**) val = string;
  else if(typ == 'd')
    sscanf(string, "%d", ((int*) val));
  else if(typ == 'f')
    sscanf(string, "%f", ((float*) val));
  else {
    fprintf(stderr, "\nSgetargs: format not recognised: %s\n", fmt);
    fprintf(stderr, "Sgetargs: please write me!\n");
    exit(1);
  }
}

/* parse the command line argument in argv */

#if defined(__STDC__)
int
Sgetargs(int argc, char **argv,...)
#else
int
Sgetargs(argc, argv, va_alist)
int    argc;
char **argv;
va_dcl
#endif
{
  va_list ap;
  char *flg;
  int nhelp, nopt, nfile, j, argfound = 1;
  helpl_type *helpl = NULL, **phelpl, *helpli;
  optl_type  *optl  = NULL, **poptl,  *optli;
  filel_type *filel = NULL, **pfilel, *fileli;

#if defined(__STDC__)
  va_start(ap, argv);
#else
  va_start(ap);
#endif

  /* read the list of help flags into a linked list */
  phelpl = &helpl;
  for(nhelp = 0; (flg = va_arg(ap, char*)) != NULL; nhelp++) {
    helpli = *phelpl = malloc(sizeof(helpl_type));
    helpli->flg = flg;
    helpli->nxt = NULL;
    phelpl = &(helpli->nxt);
  }

  /* read the options into a linked list */
  poptl = &optl;
  for(nopt = 0; (flg = va_arg(ap, char*)) != NULL; nopt++) {
    optli = *poptl = malloc(sizeof(optl_type));
    optli->flg = flg;
    optli->fmt = va_arg(ap, char*);

    /* crudely parse the printf format for a simple type */
    /* this must exist in the last two characters, and be l? or ? */
    if(optli->fmt != NULL) {
      optli->typ = *(optli->fmt + strlen(optli->fmt) - 1);
    }

    optli->val = va_arg(ap, void*);

    /* if a flag, then set the default to 0 */
    if(optli->fmt == NULL)
      *(int*) optli->val = 0;

    optli->msg = va_arg(ap, char*);
    optli->nxt = NULL;

    poptl = &(optli->nxt);
  }

  pfilel = &filel;
  for(nfile = 0; (flg = va_arg(ap, char*)) != NULL; nfile++) {
    fileli = *pfilel = malloc(sizeof(filel_type));
    fileli->flg = flg;
    fileli->mod = va_arg(ap, char*);
    fileli->fil = va_arg(ap, FILE**);
    fileli->nam = va_arg(ap, char**);
    fileli->msg = va_arg(ap, char*);
    fileli->nxt = NULL;

    /* check the syntax */
    if(fileli->flg == NULL)
      Spanic("%s: Sgetargs: file identifier is NULL\n", argv[0]);
    if(fileli->mod == NULL)
      Spanic("%s: Sgetargs: file mode is NULL\n", argv[0]);
    if(fileli->msg == NULL)
      Spanic("%s: Sgetargs: file message is NULL\n", argv[0]);

    pfilel = &(fileli->nxt);
  }
  va_end(ap);

  /* check for a help flag and print out the options and defaults if found */
  for(j = 1; j < argc; j++) {
    if(checkHelp(argv[j], helpl)) {
      int helpFlagLength;
      int maxFlagLength, maxArgLength, maxTotalLength, maxArgFlagLength;
      
      /* first find the length of the help flag */
      helpFlagLength = -2;
      for(helpli = helpl; helpli != NULL; helpli = helpli->nxt)
	helpFlagLength += strlen(helpli->flg) + 2;

      maxFlagLength = helpFlagLength;

      /* now see if there is another flag that is longer */
      for(optli = optl; optli != NULL; optli = optli->nxt)
	if(strlen(optli->flg) > maxFlagLength)
	  maxFlagLength = strlen(optli->flg);

      /* finally, see if any of the file specifier are longer */
      for(fileli = filel; fileli != NULL; fileli = fileli->nxt) {
	if(fileli->fil != NULL && *(fileli->fil) == NULL) {
	  if(strlen(fileli->flg) > maxFlagLength)
	    maxFlagLength = strlen(fileli->flg);
	}
	else {
	  if(strlen(fileli->flg) + 2 > maxFlagLength)
	    maxFlagLength = strlen(fileli->flg) + 2;
	}
      }

      /* now do the same to find the longest arg length */
      maxArgLength = 0;
      maxArgFlagLength = 0;
      maxTotalLength = maxFlagLength;
      for(optli = optl; optli != NULL; optli = optli->nxt)
	if(optli->fmt != NULL) {
	  char argString[MAXLINELENGTH];
	  sprintfptr(argString, optli->fmt, optli->typ, optli->val);
	  if(strlen(argString) > maxArgLength)
	    maxArgLength = strlen(argString);
	  if(strlen(optli->flg) > maxArgFlagLength)
	    maxArgFlagLength = strlen(optli->flg);
	  if(strlen(optli->flg) + 1 + strlen(argString) > maxTotalLength)
	    maxTotalLength = strlen(optli->flg) + 1 + strlen(argString);
	}
      
      printf("syntax: %s {options}", argv[0]);
      for(fileli = filel; fileli != NULL; fileli = fileli->nxt) {
	if(fileli->fil != NULL && *(fileli->fil) == NULL)
	  printf(" %s", fileli->flg);
	else
	  printf(" [%s]", fileli->flg);
      }
      printf("\n");

      printf("    ");
      for(helpli = helpl; helpli != NULL; helpli = helpli->nxt) {
	if(helpli->nxt != NULL)
	  printf("%s, ", helpli->flg);
	else
	  printf("%s", helpli->flg);
      }
      printf("%*sthis message\n", maxTotalLength - helpFlagLength + 2, "");

      for(optli = optl; optli != NULL; optli = optli->nxt) {
	if(optli->fmt != NULL) {
	  char argString[MAXLINELENGTH];
      	  sprintfptr(argString, optli->fmt, optli->typ, optli->val);
	  printf("    %-*s %-*s%s\n", maxArgFlagLength, optli->flg,
		 maxTotalLength - maxArgFlagLength + 1, argString, optli->msg);
	}
	else
	  printf("    %-*s%s\n", maxTotalLength + 2, optli->flg, optli->msg);
      }

      for(fileli = filel; fileli != NULL; fileli = fileli->nxt) {
	if(fileli->fil != NULL && *(fileli->fil) == NULL)
	  printf("    %-*s%s\n", maxTotalLength + 2, fileli->flg, fileli->msg);
	else
	  printf("    [%s%-*s%s\n", fileli->flg,
		 (int)(maxTotalLength - strlen(fileli->flg) + 1), "]", fileli->msg);
#ifdef OLDCODE
	  printf("    [%-*s]\t%s\n", maxFlagLength, fileli->flg,fileli->msg);
#endif
      }
      exit(0);
    }
  }

  /* check for a match */
  for(j = 1; j < argc && argfound; j++) {
    argfound = 0;

    for(optli = optl; optli != NULL; optli = optli->nxt) {
      if(strcmp(argv[j], optli->flg) == 0) {
        argfound = 1;
        if(optli->fmt == NULL) /* found a flag, so set it to true */
          *(int*) optli->val = 1;
        else { /* scan in the next argument */
          j++;
	  if(j == argc) {
	    fprintf(stderr, "Sgetargs: truncated arg list\n");
	    fprintf(stderr, "%s: use %s for more information\n", argv[0],
		    helpl->flg);
	    exit(-1);
	  }
	  sscanfptr(argv[j], optli->fmt, optli->typ, optli->val);
        }
      }
    }
  }
  if(!argfound) j--;

  /* parse the remaining arguments in the command line */
  for(fileli = filel; fileli != NULL; fileli = fileli->nxt, j++) {

    /* is there a command line word for this file argument? */
    if(j < argc) {
      /* test to see if the defaults should be used */
      if(strcmp(argv[j], "-") == 0) {
	/* test to see if a default stream has been specified */
	if(fileli->fil == NULL) {
	  fprintf(stderr, "%s: no default file specified\n", argv[0]);
	  fprintf(stderr, "%s: use \"%s\" for more information\n", argv[0],
		  helpl->flg);
	  exit(-1);
	}
      }
      else
	if(fileli->fil != NULL) {
	  *(fileli->fil) = fopen(argv[j], fileli->mod);
	  if(*(fileli->fil) == NULL)
	    Sperror("%s: Sgetargs: fopen(\"%s\", \"%s\")", argv[0],
		    argv[j], fileli->mod);
	}

      /* fill in the name of the file if required */
      if(fileli->nam != NULL)
	*(fileli->nam) = argv[j];
    }
    else {
      /* no more command line options - use defaults or give error message */
      if(*(fileli->fil) != NULL) {
	/* give "-" as the name of the file as we are using the default */
	/* ASSUMES all compilers treat "-" as static */
	if(fileli->nam != NULL)
	  *(fileli->nam) = "-";
      }
      else {
	fprintf(stderr, "%s: file argument not specified: %s\n", argv[0],
		fileli->flg);
	fprintf(stderr, "%s: use \"%s\" for more information\n", argv[0],
		helpl->flg);
	exit(-1);
      }
    }      

  }

  return(nopt);
}

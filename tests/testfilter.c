/* ***************************************************** */
/* testfilter Test hanning filter implementation.        */
/* testfilter.c                                          */
/* ***************************************************** */
/* Author: Christian Page, CERFACS, Toulouse, France.    */
/* ***************************************************** */
/*! \file testfilter.c
    \brief Test hanning filter implementation.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* C standard includes */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_LIBGEN_H
#  include <libgen.h>
#endif

#define _GNU_SOURCE

/* Local C includes */
#include <utils.h>
#include <filter.h>

/** C prototypes. */
void show_usage(char *pgm);

/** Main program. */
int main(int argc, char **argv)
{
  /**
     @param[in]  argc  Number of command-line arguments.
     @param[in]  argv  Vector of command-line argument strings.

     \return           Status.
   */

  int i;

  /* Command-line arguments variables */
  char filein[500]; /* Input filename */
  char fileout[500]; /* Output filename */
  FILE *inptr;
  FILE *outptr;

  short int end;
  int numval;
  int istat;
  
  double *invect;
  double *outvect;
  double value;
  int width = 60;

  /* Print BEGIN banner */
  (void) banner(basename(argv[0]), "1.0", "BEGIN");

  /* Get command-line arguments and set appropriate variables */
  if (argc <= 1) {
    (void) show_usage(basename(argv[0]));
    (void) banner(basename(argv[0]), "ABORT", "END");
    return 1;
  }
  else
    for (i=1; i<argc; i++) {
      if ( !strcmp(argv[i], "-i") )
        (void) strcpy(filein, argv[++i]);
      else if ( !strcmp(argv[i], "-o") )
        (void) strcpy(fileout, argv[++i]);
      else if ( !strcmp(argv[i], "-w") )
        (void) sscanf(argv[++i], "%d", &width);
      else {
        (void) fprintf(stderr, "%s:: Wrong arg %s.\n\n", basename(argv[0]), argv[i]);
        (void) show_usage(basename(argv[0]));
        (void) banner(basename(argv[0]), "ABORT", "END");
        return 1;
      }
    }

  inptr = fopen(filein, "r");
  if (inptr == NULL) {
    (void) fprintf(stderr, "Cannot open input file : %s.\n", filein);
    exit(1);
  }
  outptr = fopen(fileout, "w");
  if (outptr == NULL) {
    (void) fprintf(stderr, "Cannot open output file : %s.\n", fileout);
    exit(1);
  }
  (void) fprintf(stdout, "Filter width=%d\n", width);

  end = FALSE;
  numval = 0;
  invect = NULL;
  while (end == FALSE) {
    istat = fscanf(inptr, "%lf", &value);
    if ( istat != 1 )
      /* EOF encountered or bad data */
      end = TRUE;
    else {
      numval++;
      invect = (double *) realloc(invect, numval * sizeof(double));
      if (invect == NULL) alloc_error();
      invect[numval-1] = value;
    }
  }

  outvect = (double *) calloc(numval, sizeof(double));
  if (outvect == NULL) alloc_error();
  
  filter(outvect, invect, width, numval);

  for (i=0; i<numval; i++)
    (void) fprintf(outptr, "%d %lf %lf\n", i, invect[i], outvect[i]);

  (void) fclose(inptr);
  (void) fclose(outptr);
  
  /* Print END banner */
  (void) banner(basename(argv[0]), "OK", "END");

  return 0;
}


/** Local Subroutines **/

void show_usage(char *pgm) {

  (void) fprintf(stderr, "%s: usage:\n", pgm);
  (void) fprintf(stderr, "-i: input file\n");
  (void) fprintf(stderr, "-o: output file\n");
  (void) fprintf(stderr, "-w: filter width\n");

}

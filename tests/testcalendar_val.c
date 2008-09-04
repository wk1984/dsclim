/* ********************************************************* */
/* testcalendar_val Test calendar conversion functions.      */
/* testcalendar_val.c                                        */
/* ********************************************************* */
/* Author: Christian Page, CERFACS, Toulouse, France.        */
/* ********************************************************* */
/*! \file testcalendar_val.c
    \brief Test calendar conversion functions on one value.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/** GNU extensions */
#define _GNU_SOURCE

/* C standard includes */
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNSTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#endif

#include <utils.h>

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

  int istat;
  double val;
  int year, month, day, hour, min;
  float sec;
  
  utUnit dataunits;
  char time_units[1000];

  /* Print BEGIN banner */
  (void) banner(basename(argv[0]), "1.0", "BEGIN");

  /* Get command-line arguments and set appropriate variables */
  for (i=1; i<argc; i++) {
    if ( !strcmp(argv[i], "-h") ) {
      (void) show_usage(basename(argv[0]));
      (void) banner(basename(argv[0]), "OK", "END");
      return 0;
    }
    else {
      (void) fprintf(stderr, "%s:: Wrong arg %s.\n\n", basename(argv[0]), argv[i]);
      (void) show_usage(basename(argv[0]));
      (void) banner(basename(argv[0]), "ABORT", "END");
      (void) abort();
    }
  }

  val = 146000;
  (void) strcpy(time_units, "days since 0001-01-01");
  
  istat = utInit("/usr/local/etc/udunits.dat");

  istat = utScan(time_units, &dataunits);
  printf("dataunits.origin=%lf dataunits.factor=%lf dataunits.hasorigin=%d\n",dataunits.origin, dataunits.factor,dataunits.hasorigin);

  // istat = utCalendar(val, &dataunits, &year, &month, &day, &hour, &min, &sec);
  istat = utCalendar_cal(val, &dataunits, &year, &month, &day, &hour, &min, &sec, "noleap");
  printf("Value=%.0f YYYYMMDD HHmmss %04d %02d %02d %02d:%02d:%02.0f\n", val, year, month, day, hour, min, sec);

  /* Print END banner */
  (void) banner(basename(argv[0]), "OK", "END");

  return 0;
}


/** Local Subroutines **/

/** Show usage for program command-line arguments. */
void show_usage(char *pgm) {
  /**
     @param[in]  pgm  Program name.
  */

  (void) fprintf(stderr, "%s: usage:\n", pgm);
  (void) fprintf(stderr, "-h: help\n");

}

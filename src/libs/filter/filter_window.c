/* ***************************************************** */
/* Filter window subroutine.                             */
/* filter_window.c                                       */
/* ***************************************************** */
/* Author: Christian Page, CERFACS, Toulouse, France.    */
/* ***************************************************** */
/*! \file filter_window.c
    \brief Filter window subroutine. Uses hanning.
*/

/* LICENSE BEGIN

Copyright Cerfacs (Christian Page) (2015)

christian.page@cerfacs.fr

This software is a computer program whose purpose is to downscale climate
scenarios using a statistical methodology based on weather regimes.

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software. You can use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty and the software's author, the holder of the
economic rights, and the successive licensors have only limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading, using, modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean that it is complicated to manipulate, and that also
therefore means that it is reserved for developers and experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and, more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

LICENSE END */







#include <filter.h>

/** Filter window subroutine. Uses hanning. */
void
filter_window(double **filter_window, char *type, int width) {
  /**
     @param[out]     filter_window     Output filter window vector.
     @param[in]      type              Type of filter. Possible values: hanning.
     @param[in]      width             Width of filter.
  */

  double scale_factor; /* Hanning filter scale factor. */
  double alpha = 0.5; /* alpha value for hanning filter. */
  double sum; /* Sum for normalizing filter window. */
  int i; /* Loop counter. */

  /* Check if number is odd. If it is, make it even by adding one. */
  if (width % 2 != 0) width++;

  /* Allocate memory */
  (*filter_window) = (double *) calloc(width, sizeof(double));
  if ((*filter_window) == NULL) alloc_error(__FILE__, __LINE__);
  
  if ( !strcmp(type, "hanning") ) {
    /** We are using a hanning filter. **/
    
    /* Scale factor */
    scale_factor = 2.0 * M_PI / (double) width;
    
    /* Compute filter window. */
    sum = 0.0;
    for (i=0; i<width; i++) {
      /* Hanning definition */
      (*filter_window)[i] = (alpha - 1.0) * cos( ((double) i) * scale_factor) + alpha;
      sum += (*filter_window)[i];
    }
    
    /* Normalizing to 1.0 */
    for (i=0; i<width; i++)
      (*filter_window)[i] /= sum;
  }
  else {
    /* Unknown filter type */
    (void) fprintf(stderr, "%s: ABORT: Unknown filtering type: %s\n", __FILE__, type);
    (void) abort();
  }
}

/* ***************************************************** */
/* Normalize a 3D variable by 2D mean and variance.      */
/* normalize_field_2d.c                                  */
/* ***************************************************** */
/* Author: Christian Page, CERFACS, Toulouse, France.    */
/* ***************************************************** */
/*! \file normalize_field_2d.c
    \brief Normalize a 3D variable by 2D mean and variance.
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







#include <utils.h>

/** Normalize a 3D variable by 2D mean and variance. */
void
normalize_field_2d(double *nbuf, double *buf, double *mean, double *var, int ndima, int ndimb, int ntime) {

  /**
     @param[out]  nbuf    Normalized 3D buffer
     @param[in]   buf     Input 3D buffer
     @param[in]   mean    Mean 2D
     @param[in]   var     Variance 2D
     @param[in]   ndima   First dimension
     @param[in]   ndimb   Second dimension
     @param[in]   ntime   Time dimension
   */

  int nt; /* Time loop counter */
  int dima; /* First dimension counter */
  int dimb; /* Second dimension counter */

  /* Loop over all elements and normalize */
  for (nt=0; nt<ntime; nt++)
    for (dimb=0; dimb<ndimb; dimb++)
      for (dima=0; dima<ndima; dima++)
        nbuf[dima+dimb*ndima+nt*ndima*ndimb] = (buf[dima+dimb*ndima+nt*ndima*ndimb] - mean[dima+dimb*ndima]) / sqrt(var[dima+dimb*ndima]);
}

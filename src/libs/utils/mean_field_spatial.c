/* ***************************************************** */
/* Compute the spatial mean of a field.                  */
/* mean_field_spatial.c                                  */
/* ***************************************************** */
/* Author: Christian Page, CERFACS, Toulouse, France.    */
/* ***************************************************** */
/*! \file mean_field_spatial.c
    \brief Compute the spatial mean of a field.
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

/** Compute the spatial mean of a field. */
void
mean_field_spatial(double *buf_mean, double *buf, short int *mask, int ni, int nj, int ntime) {

  /** 
      @param[out]  buf_mean      Vector (over time) of spatially averaged data
      @param[in]   buf           Input 3D buffer
      @param[in]   mask          Input 2D mask
      @param[in]   ni            First dimension
      @param[in]   nj            Second dimension
      @param[in]   ntime         Time dimension
   */

  double sum; /* Sum used to calculate the mean */
  
  int t; /* Time loop counter */
  int i; /* Loop counter */
  int j; /* Loop counter */
  int pts = 0; /* Points counter */

  /* Loop over all time and average spatially, optionally using a mask */
  if (mask == NULL)
    for (t=0; t<ntime; t++) {
      sum = 0.0;
      for (j=0; j<nj; j++)
        for (i=0; i<ni; i++)
          sum += buf[i+j*ni+t*ni*nj];
      buf_mean[t] = sum / (double) (ni*nj);
    }
  else {
    for (t=0; t<ntime; t++) {
      sum = 0.0;
      pts = 0;
      for (j=0; j<nj; j++)
        for (i=0; i<ni; i++)
          if (mask[i+j*ni] == 1) {
            sum += buf[i+j*ni+t*ni*nj];
            pts++;
          }
      buf_mean[t] = sum / (double) pts;
    }
  }
}

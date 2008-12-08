/* ***************************************************** */
/* read_netcdf_latlon Read NetCDF lat and lon dimensions.*/
/* read_netcdf_latlon.c                                  */
/* ***************************************************** */
/* Author: Christian Page, CERFACS, Toulouse, France.    */
/* ***************************************************** */
/* Date of creation: nov 2008                            */
/* Last date of modification: nov 2008                   */
/* ***************************************************** */
/* Original version: 1.0                                 */
/* Current revision:                                     */
/* ***************************************************** */
/* Revisions                                             */
/* ***************************************************** */
/*! \file read_netcdf_latlon.c
    \brief Read NetCDF latitude and longitude dimensions.
*/

#include <io.h>

/** Read latitude and longitude dimensions in a NetCDF file. */
int read_netcdf_latlon(double **lon, double **lat, int *nlon, int *nlat, char *dimcoords, char *coords, char *gridname,
                       char *lonname, char *latname, char *dimxname, char *dimyname, char *filename) {
  /**
     @param[out]  lon        Longitude field
     @param[out]  lat        Latitude field
     @param[out]  nlon       Longitude dimension
     @param[out]  nlat       Latitude dimension
     @param[in]   dimcoords  Coordinates arrangement for X and Y dimensions: either 1D or 2D
     @param[in]   coords     Coordinates arrangement of latitude and longitude data: either 1D or 2D
     @param[in]   gridname   Projection grid name
     @param[in]   lonname    Longitude variable name
     @param[in]   latname    Latitude variable name
     @param[in]   dimxname   X dimension name
     @param[in]   dimyname   Y dimension name
     @param[in]   filename   Input NetCDF filename
     
     \return           Status.
  */

  int istat; /* Diagnostic status */

  size_t dimval; /* Variable used to retrieve dimension length */

  int ncinid; /* NetCDF input file handle ID */
  int latinid; /* Latitude variable ID */
  int loninid; /* Longitude variable ID */
  nc_type vartype; /* Type of the variable (NC_FLOAT, NC_DOUBLE, etc.) */
  int varndims; /* Number of dimensions of variable */
  int vardimids[NC_MAX_VAR_DIMS]; /* Variable dimension ids */
  int londiminid; /* Longitude dimension ID */
  int latdiminid; /* Latitude dimension ID */

  size_t start[3]; /* Start position to read */
  size_t count[3]; /* Number of elements to read */

  double *tmpd = NULL; /* Temporary buffer to read variable from NetCDF file */

  int i; /* Loop counter */
  int j; /* Loop counter */
  int ndims; /* Number of dimensions of latitude and longitude variables, 1 or 2 for 1D and 2D respectively */
  int ndims_xy; /* Number of dimensions of X and Y dimensions, 1 or 2 for 1D and 2D respectively */

  /* Read data in NetCDF file */

  /* Open NetCDF file for reading */
  printf("%s: Reading info from NetCDF input file %s.\n", __FILE__, filename);
  istat = nc_open(filename, NC_NOWRITE, &ncinid);  /* open for reading */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);

  if ( !strcmp(coords, "1D") )
    /** 1D dimensions lat and lon dimensions **/
    ndims = 1;
  else if ( !strcmp(coords, "2D") )
    /** 2D dimensions lat and lon dimensions **/
    ndims = 2;
  else
    /** 2D dimensions lat and lon dimensions **/
    ndims = 2;
  
  if ( !strcmp(dimcoords, "1D") ) {
    /** 1D dimensions X & Y **/
    ndims_xy = 1;

    /* Get dimensions length */
    istat = nc_inq_dimid(ncinid, dimyname, &latdiminid);  /* get ID for Y dimension */
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    istat = nc_inq_dimlen(ncinid, latdiminid, &dimval); /* get lat length */
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    *nlat = (int) dimval;
    
    istat = nc_inq_dimid(ncinid, dimxname, &londiminid);  /* get ID for X dimension */
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    istat = nc_inq_dimlen(ncinid, londiminid, &dimval); /* get lon length */
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    *nlon = (int) dimval;
  }
  else if ( !strcmp(dimcoords, "2D") && !strcmp(gridname, "Latitude_Longitude") ) {
    /** 1D dimensions x and y with 2D lat & lon related variables **/
    ndims_xy = 2;

    /* Get dimensions length */
    istat = nc_inq_dimid(ncinid, dimyname, &latdiminid);  /* get ID for Y dimension */
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    istat = nc_inq_dimlen(ncinid, latdiminid, &dimval); /* get lat length */
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    *nlat = (int) dimval;
    
    istat = nc_inq_dimid(ncinid, dimxname, &londiminid);  /* get ID for X dimension */
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    istat = nc_inq_dimlen(ncinid, londiminid, &dimval); /* get lon length */
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    *nlon = (int) dimval;
  }
  else {
    /** 1D dimensions x and y with 2D lat & lon related variables **/
    ndims_xy = 2;

    /* Get dimensions length */
    istat = nc_inq_dimid(ncinid, "y", &latdiminid);  /* get ID for lat dimension */
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    istat = nc_inq_dimlen(ncinid, latdiminid, &dimval); /* get lat length */
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    *nlat = (int) dimval;
    
    istat = nc_inq_dimid(ncinid, "x", &londiminid);  /* get ID for lon dimension */
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    istat = nc_inq_dimlen(ncinid, londiminid, &dimval); /* get lon length */
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    *nlon = (int) dimval;
  }
  
  /* Get dimension ID */
  istat = nc_inq_varid(ncinid, latname, &latinid);  /* get ID for lat variable */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
  /* Get lat dimensions and type */
  istat = nc_inq_var(ncinid, latinid, (char *) NULL, &vartype, &varndims, vardimids, (int *) NULL); /* get variable information */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);  
  if (varndims != ndims) {
    (void) fprintf(stderr, "Error NetCDF type and/or dimensions %d != %d.\n", varndims, ndims);
    istat = ncclose(ncinid);
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    return -1;
  }

  /* Get dimension ID */
  istat = nc_inq_varid(ncinid, lonname, &loninid);  /* get ID for lon variable */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
  /* Get lat dimensions and type */
  istat = nc_inq_var(ncinid, loninid, (char *) NULL, &vartype, &varndims, vardimids, (int *) NULL); /* get variable information */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);  
  if (varndims != ndims) {
    (void) fprintf(stderr, "Error NetCDF type and/or dimensions %d != %d.\n", varndims, ndims);
    istat = ncclose(ncinid);
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    return -1;
  }

  /** Read dimensions variables **/
  if ( !strcmp(coords, "1D") ) {
    /* Allocate memory and set start and count */
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;
    count[0] = (size_t) (*nlat);
    count[1] = 0;
    count[2] = 0;
    (*lat) = (double *) malloc((*nlat) * (*nlon) * sizeof(double));
    if ((*lat) == NULL) alloc_error(__FILE__, __LINE__);
    tmpd = (double *) malloc((*nlat) * sizeof(double));
    if (tmpd == NULL) alloc_error(__FILE__, __LINE__);
    
    /* Read values from netCDF variable */
    istat = nc_get_vara_double(ncinid, latinid, start, count, tmpd);
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    for (j=0; j<(*nlat); j++)
      for (i=0; i<(*nlon); i++)
        (*lat)[i+j*(*nlon)] = tmpd[j];
    
    /* Allocate memory and set start and count */
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;
    count[0] = (size_t) (*nlon);
    count[1] = 0;
    count[2] = 0;
    (*lon) = (double *) malloc((*nlat) * (*nlon) * sizeof(double));
    if ((*lon) == NULL) alloc_error(__FILE__, __LINE__);
    tmpd = (double *) realloc(tmpd, (*nlon) * sizeof(double));
    if (tmpd == NULL) alloc_error(__FILE__, __LINE__);
    
    /* Read values from netCDF variable */
    istat = nc_get_vara_double(ncinid, loninid, start, count, tmpd);
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    for (j=0; j<(*nlat); j++)
      for (i=0; i<(*nlon); i++)
        (*lon)[i+j*(*nlon)] = tmpd[i];

    (void) free(tmpd);
    
  }
  else {  
    /* Allocate memory and set start and count */
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;
    count[0] = (size_t) (*nlat);
    count[1] = (size_t) (*nlon);
    count[2] = 0;
    (*lat) = (double *) malloc((*nlat) * (*nlon) * sizeof(double));
    if ((*lat) == NULL) alloc_error(__FILE__, __LINE__);
    
    /* Read values from netCDF variable */
    istat = nc_get_vara_double(ncinid, latinid, start, count, (*lat));
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    
    /* Allocate memory and set start and count */
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;
    count[0] = (size_t) (*nlat);
    count[1] = (size_t) (*nlon);
    count[2] = 0;
    (*lon) = (double *) malloc((*nlat) * (*nlon) * sizeof(double));
    if ((*lon) == NULL) alloc_error(__FILE__, __LINE__);
    
    /* Read values from netCDF variable */
    istat = nc_get_vara_double(ncinid, loninid, start, count, (*lon));
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
  }

  istat = ncclose(ncinid);
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);

  /* Success status */
  return 0;
}
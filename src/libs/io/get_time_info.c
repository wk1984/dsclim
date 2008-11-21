/* ***************************************************** */
/* get_time_info Get time NetCDF info.                   */
/* get_time_info.c                                       */
/* ***************************************************** */
/* Author: Christian Page, CERFACS, Toulouse, France.    */
/* ***************************************************** */
/* Date of creation: oct 2008                            */
/* Last date of modification: oct 2008                   */
/* ***************************************************** */
/* Original version: 1.0                                 */
/* Current revision:                                     */
/* ***************************************************** */
/* Revisions                                             */
/* ***************************************************** */
/*! \file get_time_info.c
    \brief Get time NetCDF info.
*/

#include <io.h>

/** Get time information in a NetCDF file. */
int get_time_info(time_struct *time_s, double **timeval, char **time_units, char **cal_type, int *ntime, char *filename,
                  char *varname) {
  /**
     @param[out]  time_s        Time information in a time structure
     @param[out]  timeval       Time field
     @param[out]  time_units    Time units (udunits)
     @param[out]  cal_type      Calendar-type (udunits)
     @param[out]  ntime         Time dimension
     @param[in]   filename      NetCDF input filename
     @param[in]   varname       Variable name
     
     \return           Status.
  */

  int istat; /* Diagnostic status */

  size_t dimval; /* Variable used to retrieve dimension length */

  int ncinid; /* NetCDF input file handle ID */
  int timediminid; /* Time dimension ID */
  int timeinid; /* Time variable ID */
  nc_type vartype_time; /* Type of the time variable (NC_FLOAT, NC_DOUBLE, etc.) */

  int varndims; /* Number of dimensions of variable */
  int vardimids[NC_MAX_VAR_DIMS]; /* Variable dimension ids */

  size_t start[3]; /* Start position to read */
  size_t count[3]; /* Number of elements to read */

  size_t t_len; /* Length of time units attribute string */
  utUnit dataunits; /* Time data units (udunits) */

  int t; /* Time loop counter */

  /* Read data in NetCDF file */

  /* Open NetCDF file for reading */
  printf("%s: Opening for reading NetCDF input file %s.\n", __FILE__, filename);
  istat = nc_open(filename, NC_NOWRITE, &ncinid);  /* open for reading */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);

  /* Get dimensions length and ID */
  istat = nc_inq_dimid(ncinid, varname, &timediminid);  /* get ID for time dimension */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
  istat = nc_inq_dimlen(ncinid, timediminid, &dimval); /* get time length */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
  *ntime = (int) dimval;

  istat = nc_inq_varid(ncinid, varname, &timeinid);  /* get ID for time variable */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);

  /* Get time dimensions and type */
  istat = nc_inq_var(ncinid, timeinid, (char *) NULL, &vartype_time, &varndims, vardimids, (int *) NULL); /* get variable information */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
  
  if (varndims != 1) {
    (void) fprintf(stderr, "Error NetCDF type and/or dimensions Line %d.\n", __LINE__);
    return -1;
  }

  /* Allocate memory and set start and count */
  start[0] = 0;
  count[0] = (size_t) (*ntime);
  (*timeval) = malloc((*ntime) * sizeof(double));
  if ((*timeval) == NULL) alloc_error(__FILE__, __LINE__);

  /* Read values from netCDF variable */
  istat = nc_get_vara_double(ncinid, timeinid, start, count, *timeval);
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);

  /* Check values of time variable because many times they are all zero. In that case assume a 1 increment and a start at zero. */
  for (t=0; t<(*ntime); t++)
    if ((*timeval)[t] != 0.0)
      break;
  if (t == (*ntime)) {
    fprintf(stderr, "WARNING: Time variable values all zero!!! Fixing time variable to index value...\n");
    for (t=0; t<(*ntime); t++)
      (*timeval)[t] = (double) t;
  }

  /* Get time units attribute length */
  istat = nc_inq_attlen(ncinid, timeinid, "units", &t_len);
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
  /* Allocate required space before retrieving values */
  (*time_units) = (char *) malloc((t_len+1) * sizeof(char));
  if ((*time_units) == NULL) alloc_error(__FILE__, __LINE__);
  /* Get time units attribute value */
  istat = nc_get_att_text(ncinid, timeinid, "units", *time_units);
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
  /* Correct if time units ends incorrectly with Z */
  if ((*time_units)[t_len-2] == 'Z')
    (*time_units)[t_len-2] = '\0'; /* null terminate */
  else if ((*time_units)[t_len-1] == 'Z')
    (*time_units)[t_len-1] = '\0'; /* null terminate */
  else
    (*time_units)[t_len] = '\0';

  /* Get calendar type attribute length */
  istat = nc_inq_attlen(ncinid, timeinid, "calendar", &t_len);
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
  /* Allocate required space before retrieving values */
  (*cal_type) = (char *) malloc(t_len + 1);
  if ((*cal_type) == NULL) alloc_error(__FILE__, __LINE__);
  /* Get calendar type attribute value */
  istat = nc_get_att_text(ncinid, timeinid, "calendar", *cal_type);
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
  (*cal_type)[t_len] = '\0'; /* null terminate */

  /* Compute time info and store into easy time structure */
  time_s->year = (int *) malloc((*ntime) * sizeof(int));
  if (time_s->year == NULL) alloc_error(__FILE__, __LINE__);
  time_s->month = (int *) malloc((*ntime) * sizeof(int));
  if (time_s->month == NULL) alloc_error(__FILE__, __LINE__);
  time_s->day = (int *) malloc((*ntime) * sizeof(int));
  if (time_s->day == NULL) alloc_error(__FILE__, __LINE__);
  time_s->hour = (int *) malloc((*ntime) * sizeof(int));
  if (time_s->hour == NULL) alloc_error(__FILE__, __LINE__);
  time_s->minutes = (int *) malloc((*ntime) * sizeof(int));
  if (time_s->minutes == NULL) alloc_error(__FILE__, __LINE__);
  time_s->seconds = (float *) malloc((*ntime) * sizeof(float));
  if (time_s->seconds == NULL) alloc_error(__FILE__, __LINE__);

  istat = utInit("");

  istat = utScan((*time_units), &dataunits);
  for (t=0; t<(*ntime); t++)
    istat = utCalendar_cal((*timeval)[t], &dataunits, &(time_s->year[t]), &(time_s->month[t]), &(time_s->day[t]),
                           &(time_s->hour[t]), &(time_s->minutes[t]), &(time_s->seconds[t]), *cal_type);

  (void) utTerm();

  /** Close NetCDF file **/
  istat = ncclose(ncinid);
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);

  /* Success status */
  return 0;
}
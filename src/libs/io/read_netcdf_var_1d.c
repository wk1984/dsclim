/* ***************************************************** */
/* read_netcdf_var_1d Read a 1D NetCDF variable.         */
/* read_netcdf_var_1d.c                                  */
/* ***************************************************** */
/* Author: Christian Page, CERFACS, Toulouse, France.    */
/* ***************************************************** */
/* Date of creation: sep 2008                            */
/* Last date of modification: sep 2008                   */
/* ***************************************************** */
/* Original version: 1.0                                 */
/* Current revision:                                     */
/* ***************************************************** */
/* Revisions                                             */
/* ***************************************************** */
/*! \file read_netcdf_var_1d.c
    \brief Read a NetCDF variable.
*/

#include <io.h>

/** Read a 1D variable in a NetCDF file, and return information in info_field_struct structure. */
int read_netcdf_var_1d(double **buf, info_field_struct *info_field, char *filename, char *varname,
                       char *dimname, int *ndim) {
  /**
     @param[out]  buf        1D variable
     @param[out]  info_field Information about the output variable
     @param[in]   filename   NetCDF input filename
     @param[in]   varname    NetCDF variable name
     @param[in]   dimname    Dimension name
     @param[out]  ndim       Dimension length
     
     \return           Status.
  */

  int istat; /* Diagnostic status */

  size_t dimval; /* Variable used to retrieve dimension length */

  int ncinid; /* NetCDF input file handle ID */
  int varinid; /* NetCDF variable ID */
  int diminid; /* NetCDF dimension ID */
  nc_type vartype_main; /* Type of the variable (NC_FLOAT, NC_DOUBLE, etc.) */
  int varndims; /* Number of dimensions of variable */
  int vardimids[NC_MAX_VAR_DIMS]; /* Variable dimension ids */

  size_t start[3]; /* Start position to read */
  size_t count[3]; /* Number of elements to read */

  float valf; /* Variable used to retrieve fillvalue */
  char *tmpstr = NULL; /* Temporary string */
  size_t t_len; /* Length of string attribute */

  /* Allocate memory */
  tmpstr = (char *) malloc(5000 * sizeof(char));
  if (tmpstr == NULL) alloc_error(__FILE__, __LINE__);

  /* Read data in NetCDF file */

  /* Open NetCDF file for reading */
  printf("%s: Opening for reading NetCDF input file %s.\n", __FILE__, filename);
  istat = nc_open(filename, NC_NOWRITE, &ncinid);  /* open for reading */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);

  printf("%s: READ %s %s.\n", __FILE__, varname, filename);

  /* Get dimension length */
  istat = nc_inq_dimid(ncinid, dimname, &diminid);  /* get ID for dimension */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
  istat = nc_inq_dimlen(ncinid, diminid, &dimval); /* get dimension length */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
  *ndim = (int) dimval;

  /* Get main variable ID */
  istat = nc_inq_varid(ncinid, varname, &varinid); /* get main variable ID */
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);

  /** Read data variable **/
  
  /* Get variable information */
  istat = nc_inq_var(ncinid, varinid, (char *) NULL, &vartype_main, &varndims, vardimids, (int *) NULL);
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);

  /* Verify that variable is really 1D */
  if (varndims != 1) {
    (void) fprintf(stderr, "%s: Error NetCDF type and/or dimensions.\n", __FILE__);
    (void) free(tmpstr);
    istat = ncclose(ncinid);
    if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);
    return -1;
  }

  /* If info_field si not NULL, get some information about the read variable */
  if (info_field != NULL) {
    /* Get missing value */
    if (vartype_main == NC_FLOAT) {
      istat = nc_get_att_float(ncinid, varinid, "missing_value", &valf);
      if (istat != NC_NOERR)
        info_field->fillvalue = -9999.0;
      else
        info_field->fillvalue = (double) valf;
    }
    else if (vartype_main == NC_DOUBLE) {
      istat = nc_get_att_double(ncinid, varinid, "missing_value", &(info_field->fillvalue));
      if (istat != NC_NOERR)
        info_field->fillvalue = -9999.0;
    }

    /* Get units */
    istat = nc_inq_attlen(ncinid, varinid, "units", &t_len);
    if (istat == NC_NOERR) {
      handle_netcdf_error(istat, __FILE__, __LINE__);
      istat = nc_get_att_text(ncinid, varinid, "units", tmpstr);
      if (istat == NC_NOERR) {
        if (tmpstr[t_len-1] != '\0')
          tmpstr[t_len] = '\0';
        info_field->units = strdup(tmpstr);
      }
      else
        info_field->units = strdup("unknown");
    }
    else
      info_field->units = strdup("unknown");

    /* Get long name */
    istat = nc_inq_attlen(ncinid, varinid, "long_name", &t_len);
    if (istat == NC_NOERR) {
      handle_netcdf_error(istat, __FILE__, __LINE__);
      istat = nc_get_att_text(ncinid, varinid, "long_name", tmpstr);
      if (istat == NC_NOERR) {
        if (tmpstr[t_len-1] != '\0')
          tmpstr[t_len] = '\0';
        info_field->long_name = strdup(tmpstr);
      }
      else
        info_field->long_name = strdup(varname);
    }
    else
      info_field->long_name = strdup(varname);
  }

  /* Allocate memory and set start and count */
  start[0] = 0;
  start[1] = 0;
  start[2] = 0;
  count[0] = (size_t) *ndim;
  count[1] = 0;
  count[2] = 0;
  /* Allocate memory */
  (*buf) = (double *) malloc((*ndim) * sizeof(double));
  if ((*buf) == NULL) alloc_error(__FILE__, __LINE__);

  /* Read values from netCDF variable */
  istat = nc_get_vara_double(ncinid, varinid, start, count, *buf);
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);

  /* Close the input netCDF file. */
  istat = ncclose(ncinid);
  if (istat != NC_NOERR) handle_netcdf_error(istat, __FILE__, __LINE__);

  /* Free memory */
  (void) free(tmpstr);

  /* Success status */
  return 0;
}
/* ***************************************************** */
/* read_learning_fields Read learning fields.            */
/* read_learning_fields.c                                */
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
/*! \file read_learning_fields.c
    \brief Read Learning data from input files. Currently only NetCDF is implemented.
*/

#include <dsclim.h>

/** Read Learning data from input files. Currently only NetCDF is implemented. */
int read_learning_fields(data_struct *data) {
  /**
     @param[in]  data  MASTER data structure.
     
     \return           Status.
  */

  int istat; /* Diagnostic status */
  int i; /* Loop counter */
  int t; /* Loop counter */
  int ii; /* Loop counter */
  char *nomvar = NULL; /* Variable name in NetCDF file */
  char *nomvar_time = NULL; /* Time variable name in NetCDF file */
  char *nomvar_season = NULL; /* Season variable name in NetCDF file */
  char *name = NULL; /* Dimension name in NetCDF file */
  char *cal_type = NULL; /* Calendar type (udunits) */
  char *time_units = NULL; /* Time units (udunits) */
  double *bufd = NULL; /* Temporary buffer */
  double *time_sort = NULL; /* Temporary time info used for time merging */
  size_t *time_index = NULL; /* Temporary time index used for time merging */
  int total_t; /* Total number of times used for time merging */
  int neof; /* EOF dimension */
  int npts; /* Points dimension */
  int nclusters; /* Clusters dimension */
  int neof_file; /* EOF dimension in input file */

  /* Allocate memory for temporary strings */
  nomvar = (char *) malloc(500 * sizeof(char));
  if (nomvar == NULL) alloc_error(__FILE__, __LINE__);
  nomvar_time = (char *) malloc(500 * sizeof(char));
  if (nomvar_time == NULL) alloc_error(__FILE__, __LINE__);
  nomvar_season = (char *) malloc(500 * sizeof(char));
  if (nomvar_season == NULL) alloc_error(__FILE__, __LINE__);
  name = (char *) malloc(500 * sizeof(char));
  if (name == NULL) alloc_error(__FILE__, __LINE__);

  /* Initialize season string */
  (void) strcpy(nomvar_season, "season");

  /* Loop over all the seasons */
  for (i=0; i<data->conf->nseasons; i++) {

    /* Read time data and info */
    (void) sprintf(nomvar, "%s_%d", data->learning->nomvar_time, i+1);
    (void) sprintf(nomvar_time, "%s_%d", data->learning->nomvar_time, i+1);
    istat = get_time_info(data->learning->data[i].time_s, &(data->learning->data[i].time), &time_units, &cal_type,
                          &(data->learning->data[i].ntime),
                          data->learning->filename_learn, nomvar_time, TRUE);
    (void) free(cal_type);
    (void) free(time_units);
    if (istat != 0) {
      /* In case of failure */
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return istat;
    }

    /* Read weight data */
    (void) sprintf(nomvar, "%s_%d", data->learning->nomvar_weight, i+1);
    (void) sprintf(name, "%s_%d", data->conf->clustname, i+1);
    istat = read_netcdf_var_2d(&(data->learning->data[i].weight), (info_field_struct *) NULL, (proj_struct *) NULL,
                               data->learning->filename_weight, nomvar, data->conf->eofname, name,
                               &neof_file, &nclusters, TRUE);
    /* Save EOF dimension for control and model large-scale fields */
    if (data->field[0].n_ls > 0)
      data->field[0].data[0].eof_info->neof_ls = neof_file;
    if (data->field[1].n_ls > 0)
      data->field[1].data[0].eof_info->neof_ls = neof_file;

    /* If clusters dimension is not initialized, use retrieved info from input file */
    if (data->conf->season[i].nclusters == -1)
      data->conf->season[i].nclusters = nclusters;
    /* Else verify that they match */
    else if (data->conf->season[i].nclusters != nclusters) {
      (void) fprintf(stderr, "%s: ERROR: Incorrect number of clusters in NetCDF file. Season %d, nclusters=%d vs configuration file %d.\n",
                     __FILE__, i, nclusters, data->conf->season[i].nclusters);
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return -1;
    }
    if (istat != 0) {
      /* In case of failure */
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return istat;
    }

    /* Read precip_reg data (precipitation regression coefficients) */
    (void) sprintf(nomvar, "%s_%d", data->learning->nomvar_precip_reg, i+1);
    (void) sprintf(name, "%s_%d", data->conf->clustname, i+1);
    istat = read_netcdf_var_2d(&(data->learning->data[i].precip_reg), (info_field_struct *) NULL, (proj_struct *) NULL,
                               data->learning->filename_learn,
                               nomvar, data->conf->ptsname, name,
                               &npts, &(data->conf->season[i].nreg), TRUE);
    /* Verify that points dimension match configuration value */
    if (npts != data->reg->npts) {
      (void) fprintf(stderr, "%s: ERROR: Incorrect number of points in NetCDF file %d vs configuration file %d.\n",
                     __FILE__, npts, data->reg->npts);
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return -1;
    }
    if (istat != 0) {
      /* In case of failure */
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return istat;
    }

    /* Read precip_reg_cst data (precipitation regression constant) */
    (void) sprintf(nomvar, "%s_%d", data->learning->nomvar_precip_reg_cst, i+1);
    istat = read_netcdf_var_1d(&(data->learning->data[i].precip_reg_cst), (info_field_struct *) NULL,
                               data->learning->filename_learn, nomvar, data->conf->ptsname, &npts, TRUE);
    /* Verify that points dimension match configuration value */
    if (npts != data->reg->npts) {
      (void) fprintf(stderr, "%s: ERROR: Incorrect number of points in NetCDF file %d vs configuration file %d.\n",
                     __FILE__, npts, data->reg->npts);
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return -1;
    }
    if (istat != 0) {
      /* In case of failure */
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return istat;
    }

    /* Read precip_index data (precipitation index for learning period over all regression points) */
    (void) sprintf(nomvar, "%s_%d", data->learning->nomvar_precip_index, i+1);
    istat = read_netcdf_var_2d(&(data->learning->data[i].precip_index), (info_field_struct *) NULL, (proj_struct *) NULL,
                               data->learning->filename_learn,
                               nomvar, data->conf->ptsname, nomvar_time,
                               &npts, &(data->learning->data[i].ntime), TRUE);
    /* Verify that points dimension match configuration value */
    if (npts != data->reg->npts) {
      (void) fprintf(stderr, "%s: ERROR: Incorrect number of points in NetCDF file %d vs configuration file %d.\n",
                     __FILE__, npts, data->reg->npts);
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return -1;
    }
    if (istat != 0) {
      /* In case of failure */
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return istat;
    }

    /* Read cluster distances data */
    bufd = NULL;
    (void) sprintf(nomvar, "%s_%d", data->learning->nomvar_class_clusters, i+1);
    istat = read_netcdf_var_1d(&bufd, (info_field_struct *) NULL,
                               data->learning->filename_clust_learn, nomvar, nomvar_time,
                               &(data->learning->data[i].ntime), TRUE);
    if (istat != 0) {
      /* In case of failure */
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return istat;
    }
    /* Transfer data into proper data structure */
    data->learning->data[i].class_clusters = malloc(data->learning->data[i].ntime * sizeof(int));
    if (data->learning->data[i].class_clusters == NULL) alloc_error(__FILE__, __LINE__);
    for (ii=0; ii<data->learning->data[i].ntime; ii++)
      data->learning->data[i].class_clusters[ii] = bufd[ii];
    (void) free(bufd);

    /* Read sup_index data (secondary large-scale field index for learning period) */
    (void) sprintf(nomvar, "%s_%d", data->learning->nomvar_sup_index, i+1);
    istat = read_netcdf_var_1d(&(data->learning->data[i].sup_index), (info_field_struct *) NULL,
                               data->learning->filename_learn, nomvar, nomvar_time,
                               &(data->learning->data[i].ntime), TRUE);
    if (istat != 0) {
      /* In case of failure */
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return istat;
    }

    /* Read sup_index_mean data (secondary large-scale field index spatial mean for learning period) */
    istat = read_netcdf_var_generic_val(&(data->learning->data[i].sup_index_mean), (info_field_struct *) NULL,
                                        data->learning->filename_learn, data->learning->nomvar_sup_index_mean, i);
    if (istat != 0) {
      /* In case of failure */
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return istat;
    }
  
    /* Read sup_index_var data (secondary large-scale field index spatial variance for learning period) */
    istat = read_netcdf_var_generic_val(&(data->learning->data[i].sup_index_var), (info_field_struct *) NULL,
                                        data->learning->filename_learn, data->learning->nomvar_sup_index_var, i);
    if (istat != 0) {
      /* In case of failure */
      (void) free(nomvar);
      (void) free(nomvar_time);
      (void) free(nomvar_season);
      (void) free(name);
      return istat;
    }
  }

  /** Create whole period time info from separate season info merging **/

  /* Allocate memory and set pointers to NULL for realloc use */
  time_index = (size_t *) malloc(data->conf->nseasons * sizeof(size_t));
  if (time_index == NULL) alloc_error(__FILE__, __LINE__);
  data->learning->time_s->year = NULL;
  data->learning->time_s->month = NULL;
  data->learning->time_s->day = NULL;
  data->learning->time_s->hour = NULL;
  data->learning->time_s->minutes = NULL;
  data->learning->time_s->seconds = NULL;

  /* Sort the vector, get the sorted vector indexes */
  time_sort = (double *) malloc(data->conf->nseasons * sizeof(double));
  if (time_sort == NULL) alloc_error(__FILE__, __LINE__);
  /* Loop over seasons */
  for (i=0; i<data->conf->nseasons; i++)
    time_sort[i] = data->learning->data[i].time[0];
  /* Sorting */
  (void) gsl_sort_index(time_index, time_sort, 1, (size_t) data->conf->nseasons);
  (void) free(time_sort);

  /* Merge time info */
  total_t = 0;
  for (i=0; i<data->conf->nseasons; i++) {
    for (t=0; t<data->learning->data[time_index[i]].ntime; t++) {
      data->learning->time_s->year = (int *) realloc(data->learning->time_s->year, (total_t+1) * sizeof(int));
      if (data->learning->time_s->year == NULL) alloc_error(__FILE__, __LINE__);
      data->learning->time_s->month = (int *) realloc(data->learning->time_s->month, (total_t+1) * sizeof(int));
      if (data->learning->time_s->month == NULL) alloc_error(__FILE__, __LINE__);
      data->learning->time_s->day = (int *) realloc(data->learning->time_s->day, (total_t+1) * sizeof(int));
      if (data->learning->time_s->day == NULL) alloc_error(__FILE__, __LINE__);
      data->learning->time_s->hour = (int *) realloc(data->learning->time_s->hour, (total_t+1) * sizeof(int));
      if (data->learning->time_s->hour == NULL) alloc_error(__FILE__, __LINE__);
      data->learning->time_s->minutes = (int *) realloc(data->learning->time_s->minutes, (total_t+1) * sizeof(int));
      if (data->learning->time_s->minutes == NULL) alloc_error(__FILE__, __LINE__);
      data->learning->time_s->seconds = (float *) realloc(data->learning->time_s->seconds, (total_t+1) * sizeof(float));
      if (data->learning->time_s->seconds == NULL) alloc_error(__FILE__, __LINE__);
      data->learning->time_s->year[total_t] = data->learning->data[time_index[i]].time_s->year[t];
      data->learning->time_s->month[total_t] = data->learning->data[time_index[i]].time_s->month[t];
      data->learning->time_s->day[total_t] = data->learning->data[time_index[i]].time_s->day[t];
      data->learning->time_s->hour[total_t] = data->learning->data[time_index[i]].time_s->hour[t];
      data->learning->time_s->minutes[total_t] = data->learning->data[time_index[i]].time_s->minutes[t];
      data->learning->time_s->seconds[total_t] = data->learning->data[time_index[i]].time_s->seconds[t];
      total_t++;
    }
  }
  /* Save total number of times */
  data->learning->ntime = total_t;

  /* Free temporary vector */
  (void) free(time_index);

  /* Read pc_normalized_var data (normalized EOF-projected large-scale field variance for learning period) */
  istat = read_netcdf_var_1d(&(data->learning->pc_normalized_var), (info_field_struct *) NULL,
                             data->learning->filename_learn, data->learning->nomvar_pc_normalized_var, data->conf->eofname,
                             &neof, TRUE);
  if (neof != neof_file) {
    /* Verify that EOF dimension match configuration value */
    (void) fprintf(stderr, "%s: ERROR: Incorrect number of EOFs in NetCDF file %d vs configuration file %d.\n",
                   __FILE__, neof, neof_file);
    (void) free(nomvar);
    (void) free(nomvar_time);
    (void) free(nomvar_season);
    (void) free(name);
    return -1;
  }
  if (istat != 0) {
    /* In case of failure */
    (void) free(nomvar);
    (void) free(nomvar_time);
    (void) free(nomvar_season);
    (void) free(name);
    return istat;
  }

  /* Free memory */
  (void) free(nomvar);
  (void) free(nomvar_time);
  (void) free(nomvar_season);
  (void) free(name);

  /* Success status */
  return 0;
}

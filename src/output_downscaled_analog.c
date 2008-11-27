/* ***************************************************** */
/* Read analog day data and write it for downscaled      */
/* period.                                               */
/* output_downscaled_analog.c                            */
/* ***************************************************** */
/* Author: Christian Page, CERFACS, Toulouse, France.    */
/* ***************************************************** */
/*! \file output_downscaled_analog.c
    \brief Read analog day data and write it for downscaled period.
*/

#include <dsclim.h>

/** Read analog day data and write it for downscaled period. */
int output_downscaled_analog(analog_day_struct analog_days, data_struct *data, double *time_ls, int ntime) {
  /**
     @param[in]   analog_days           Analog days time indexes and dates with corresponding dates being downscaled.
     @param[in]   data                  MASTER data structure.
     @param[in]   time_ls               Time values
     @param[in]   ntime                 Number of times dimension
  */
  
  char *infile = NULL; /* Input filename */
  char *outfile = NULL; /* Output filename */
  char **outfiles = NULL; /* Output filelist */
  int year1 = 0; /* First year of data input file */
  int year2 = 0; /* End year of data input file */
  double *buf = NULL; /* Temporary data buffer */
  double *timeval = NULL; /* Temporary time information buffer */
  double *lat = NULL; /* Temporary latitude buffer */
  double *lon = NULL; /* Temporary longitude buffer */
  char *cal_type = NULL; /* Calendar type (udunits) */
  char *time_units = NULL; /* Time units (udunits) */
  double ctimeval[1]; /* Dummy time info */
  int ntime_file; /* Number of times dimension */
  int ntime_obs; /* Number of times dimension in observation database */
  int nlon; /* Longitude dimension */
  int nlat; /* Latitude dimension */
  int nlon_file; /* Longitude dimension in input file */
  int nlat_file; /* Latitude dimension in input file */
  int noutf; /* Number of files in filelist */
  int found = FALSE; /* Use to tag if we found a specific filename in the filelist */
  int output_month_end; /* Ending month for observation database */
  time_struct *time_s = NULL; /* Time structure for observation database */

  info_field_struct *info = NULL; /* Temporary field information structure */
  proj_struct *proj = NULL; /* Temporary projection information structure */

  int tmpi; /* Temporay integer value */

  int t; /* Time loop counter */
  int tl; /* Time loop counter */
  int var; /* Variable counter */
  int istat; /* Diagnostic status */
  int f; /* Loop counter for files */

  /*                                         J   F   M   A   M   J   J   A   S   O   N   D    */
  static int days_per_month_reg_year[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  infile = (char *) malloc(5000 * sizeof(char));
  if (infile == NULL) alloc_error(__FILE__, __LINE__);
  outfile = (char *) malloc(5000 * sizeof(char));
  if (outfile == NULL) alloc_error(__FILE__, __LINE__);

  if (data->conf->output_month_begin == 1)
    output_month_end = 12;
  else
    output_month_end = data->conf->output_month_begin - 1;

  /* Process each variable */
  for (var=0; var<data->conf->obs_var->nobs_var; var++) {
    
    /** Retrieve dimensions for this variable if needed, using the first timestep **/

    /* Create input filename for retrieving dimensions */
    if (data->conf->obs_var->month_begin != 1) {
      /* Months in observation files *does not* begin in January: must have 2 years in filename */
      printf("%d %d %d\n",analog_days.month[0] , data->conf->obs_var->month_begin,analog_days.year[0]);
      if (analog_days.month[0] < data->conf->obs_var->month_begin)
        year1 = analog_days.year[0] - 1;
      else
        year1 = analog_days.year[0];
      year2 = year1 + 1;
      if (data->conf->obs_var->year_digits == 4)
        (void) sprintf(infile, "%s/%s/%s_%d%d.nc", data->conf->obs_var->path, data->conf->obs_var->frequency, data->conf->obs_var->acronym[var], year1, year2);
      else {
        tmpi = year1 / 100;
        year1 = year1 - (tmpi*100);
        tmpi = year2 / 100;
        year2 = year2 - (tmpi*100);
        (void) sprintf(infile, "%s/%s/%s_%02d%02d.nc", data->conf->obs_var->path, data->conf->obs_var->frequency,
                       data->conf->obs_var->acronym[var], year1, year2);
      }
    }
    else {
      /* Months in observation files begins in January: must have 1 year in filename */
      if (data->conf->obs_var->year_digits == 4)
        (void) sprintf(infile, "%s/%s/%s_%d.nc", data->conf->obs_var->path, data->conf->obs_var->frequency,
                       data->conf->obs_var->acronym[var], year1);
      else {
        tmpi = year1 / 100;
        year1 = year1 - (tmpi*100);
        (void) sprintf(infile, "%s/%s/%s_%02d.nc", data->conf->obs_var->path, data->conf->obs_var->frequency,
                       data->conf->obs_var->acronym[var], year1);
      }
    }
    istat = read_netcdf_dims_3d(&lon, &lat, &timeval, &cal_type, &time_units, &nlon, &nlat, &ntime_obs,
                                data->info, data->conf->proj->coords, data->conf->proj->name,
                                data->conf->lonname, data->conf->latname, data->conf->timename,
                                infile);
    (void) free(timeval);
    (void) free(cal_type);
    (void) free(time_units);

    /* Get time information */
    time_s = (time_struct *) malloc(sizeof(time_struct));
    if (time_s == NULL) alloc_error(__FILE__, __LINE__);
    istat = get_time_info(time_s, &timeval, &time_units, &cal_type, &ntime_obs, infile, data->conf->timename);

    /* Process each downscaled day */
    noutf = 0;
    outfiles = NULL;
    for (t=0; t<ntime; t++) {

      /* Create output filename for writing data */
      if (analog_days.month_s[t] < data->conf->output_month_begin)
        year1 = analog_days.year_s[t] - 1;
      else
        year1 = analog_days.year_s[t];
      if (data->conf->output_month_begin != 1) {
        year2 = year1 + 1;
      }
      else
        year2 = year1;
      /* Example: evapn_1d_19790801_19800731.nc */
      (void) sprintf(outfile, "%s/%s_1d_%04d%02d%02d_%04d%02d%02d.nc", data->conf->output_path, data->conf->obs_var->netcdfname[var],
                     year1, data->conf->output_month_begin, 1,
                     year2, output_month_end, days_per_month_reg_year[output_month_end-1]);
      
      /* Check if output file has already been created */
      found = FALSE;
      f = 0;
      while (f<noutf) {
        if ( !strcmp(outfiles[noutf], outfile) ) {
          found = TRUE;
          break;
        }
        f++;
      }
      if (found == FALSE) {
        /* File was not created already */
        noutf++;
        outfiles = (char **) realloc(outfiles, (noutf+1) * sizeof(char *));
        if (outfiles == NULL) alloc_error(__FILE__, __LINE__);
        outfiles[noutf-1] = strdup(outfile);

        /* Create output file if needed */
        istat = create_netcdf("Downscaling data from CERFACS", "Donnees de desagregation produites par le CERFACS",
                              "Downscaling data from CERFACS", "Donnees de desagregation produites par le CERFACS",
                              "climat,scenarios,desagregation,downscaling,CERFACS", "C language", "Downscaling data from CERFACS",
                              data->info->institution,
                              data->info->creator_email, data->info->creator_url, data->info->creator_name,
                              data->info->version, data->info->scenario, data->info->scenario_co2, data->info->model,
                              data->info->institution_model, data->info->country, data->info->member,
                              data->info->downscaling_forcing, data->info->contact_email, data->info->contact_name,
                              data->info->other_contact_email, data->info->other_contact_name,
                              outfile);
        if (istat != 0) {
          /* In case of failure */
          (void) free(infile);
          (void) free(outfile);
          for (f=0; f<noutf; f++)
            (void) free(outfiles[f]);
          (void) free(outfiles);
          return istat;
        }
        /* Write dimensions of field in newly-created NetCDF output file */
        ctimeval[0] = time_ls[0];
        istat = write_netcdf_dims_3d(lon, lat, ctimeval, data->conf->cal_type,
                                     data->conf->time_units, nlon, nlat, 1,
                                     data->info->timestep, data->conf->proj->name, data->conf->proj->coords,
                                     data->conf->proj->grid_mapping_name, data->conf->proj->latin1,
                                     data->conf->proj->latin2, data->conf->proj->lonc, data->conf->proj->lat0,
                                     data->conf->proj->false_easting, data->conf->proj->false_northing,
                                     data->conf->lonname, data->conf->latname, data->conf->timename,
                                     outfile);
        if (istat != 0) {
          /* In case of failure */
          (void) free(infile);
          (void) free(outfile);
          for (f=0; f<noutf; f++)
            (void) free(outfiles[f]);
          (void) free(outfiles);
          return istat;
        }
      }

      /* Create input filename for reading data */
      if (data->conf->obs_var->month_begin != 1) {
        /* Months in observation files *does not* begin in January: must have 2 years in filename */
        if (analog_days.month[t] < data->conf->obs_var->month_begin)
          year1 = analog_days.year[t] - 1;
        else
          year1 = analog_days.year[t];
        year2 = year1 + 1;
        if (data->conf->obs_var->year_digits == 4)
          (void) sprintf(infile, "%s/%s/%s_%d%d.nc", data->conf->obs_var->path, data->conf->obs_var->frequency,
                         data->conf->obs_var->acronym[var], year1, year2);
        else {
          tmpi = year1 / 100;
          year1 = year1 - (tmpi*100);
          tmpi = year2 / 100;
          year2 = year2 - (tmpi*100);
          (void) sprintf(infile, "%s/%s/%s_%02d%02d.nc", data->conf->obs_var->path, data->conf->obs_var->frequency,
                         data->conf->obs_var->acronym[var], year1, year2);
        }
      }
      else {
        /* Months in observation files begins in January: must have 1 year in filename */
        if (data->conf->obs_var->year_digits == 4)
          (void) sprintf(infile, "%s/%s/%s_%d.nc", data->conf->obs_var->path, data->conf->obs_var->frequency,
                         data->conf->obs_var->acronym[var], year1);
        else {
          tmpi = year1 / 100;
          year1 = year1 - (tmpi*100);
          (void) sprintf(infile, "%s/%s/%s_%02d.nc", data->conf->obs_var->path, data->conf->obs_var->frequency,
                         data->conf->obs_var->acronym[var], year1);
        }
      }

      /* Find date in observation database */
      found = FALSE;
      tl = 0;
      while (tl<(ntime_obs-1)) {
        if (analog_days.year[t] == time_s->year[tl] && analog_days.month[t] == time_s->month[tl] &&
            analog_days.day[t] == time_s->day[tl]) {
          found = TRUE;
          break;
        }
        tl++;
      }

      /* Read data */
      info = (info_field_struct *) malloc(sizeof(info_field_struct));
      if (info == NULL) alloc_error(__FILE__, __LINE__);
      proj = (proj_struct *) malloc(sizeof(proj_struct));
      if (proj == NULL) alloc_error(__FILE__, __LINE__);
      istat = read_netcdf_var_3d_2d(&buf, info, proj, infile, data->conf->obs_var->acronym[var],
                                    data->conf->lonname, data->conf->latname, data->conf->timename,
                                    tl, &nlon_file, &nlat_file, &ntime_file);

      /* Apply modifications to data if needed */

      /* Write data */
      istat = write_netcdf_var_3d_2d(buf, time_ls, info->fillvalue, outfile, data->conf->obs_var->netcdfname[var],
                                     proj->grid_mapping_name,
                                     data->conf->lonname, data->conf->latname, data->conf->timename,
                                     t, nlon_file, nlat_file, ntime_file);
    
      (void) free(timeval);
      (void) free(time_s);
      (void) free(cal_type);
      (void) free(time_units);

      (void) free(buf);

      (void) free(info);
      (void) free(proj);
      (void) free(lat);
      (void) free(lon);
    }
    for (f=0; f<noutf; f++)
      (void) free(outfiles[f]);
    (void) free(outfiles);
  }

  (void) free(infile);
  (void) free(outfile);

  /* Success diagnostic */
  return 0;
}

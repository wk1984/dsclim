# Copyright (C) 2008 Christian Page <christian.page@cerfacs.fr>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

noinst_LTLIBRARIES = libio.la
libio_la_SOURCES = io.h read_netcdf_dims_3d.c read_netcdf_latlon.c read_netcdf_xy.c read_netcdf_var_3d.c read_netcdf_var_3d_2d.c read_netcdf_var_2d.c read_netcdf_var_1d.c read_netcdf_var_generic_val.c handle_netcdf_error.c create_netcdf.c write_netcdf_dims_3d.c write_netcdf_var_3d.c write_netcdf_var_3d_2d.c get_attribute_str.c get_time_attributes.c get_time_info.c compute_time_info.c read_netcdf_dims_eof.c
libio_la_CPPFLAGS = -I${top_srcdir}/src/libs/misc -I${top_srcdir}/src -I${top_srcdir}/src/libs/utils $(NCDF_CPPFLAGS)
libio_la_LIBADD = ../misc/libmisc.la ../utils/libutils.la $(NCDF_LIBS) $(GSL_LIBS) -ludunits2 -lexpat -lm

# Copyright (C) 2008 Christian Page <christian.page@cerfacs.fr>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

noinst_LTLIBRARIES = libpceof.la
libpceof_la_SOURCES = pceof.h normalize_pc.c project_field_eof.c
libpceof_la_CPPFLAGS = -I${top_srcdir}/src/libs/misc $(GSL_CFLAGS)
libpceof_la_LIBADD = ../misc/libmisc.la $(GSL_LIBS) -lm

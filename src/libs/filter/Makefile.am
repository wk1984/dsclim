# Copyright (C) 2008 Christian Page <christian.page@cerfacs.fr>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

noinst_LTLIBRARIES = libfilter.la
libfilter_la_SOURCES = filter.h filter.c filter_window.c
libfilter_la_CPPFLAGS = -I${top_srcdir}/src/libs/misc
libfilter_la_LIBADD = ../misc/libmisc.la -lm

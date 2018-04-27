# LT Utility libraries
noinst_LTLIBRARIES		= libdarwin.la

libdarwin_la_CPPFLAGS	=  -D_GNU_SOURCE -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libdarwin_la_CXXFLAGS	= $(LIBHPX_CXXFLAGS)
libdarwin_la_SOURCES	= time.cpp cpu.cpp mmap.cpp usleep.cpp barrier.cpp get_program_name.cpp

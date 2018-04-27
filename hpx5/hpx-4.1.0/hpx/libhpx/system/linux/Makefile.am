# LT Utility libraries
noinst_LTLIBRARIES		= liblinux.la

liblinux_la_CPPFLAGS	= -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
liblinux_la_CXXFLAGS	= $(LIBHPX_CXXFLAGS)
liblinux_la_SOURCES		= time.cpp cpu.cpp mmap.cpp usleep.cpp get_program_name.cpp
liblinux_la_LIBADD		= -lrt

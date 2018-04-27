bin_PROGRAMS = hawaii
CASE    = RMHD
INTEG   = RK4 #FORWARD_EULER
OUTPUT  = #-DSILO -DRNPL #-DNO_SCALED_EPS #-DCILK

hawaii_SOURCES = hawaii-integr.c hawaii-main.c hawaii-prob.c hawaii-rmhd-param.c \
		  hawaii-rmhd.c hawaii-storage.c hawaii-utils.c  hawaii-wavelet.c 

hawaii_CFLAGS = -O3 -g -std=c99 -I. -D$(CASE) -D$(INTEG) $(OUTPUT)


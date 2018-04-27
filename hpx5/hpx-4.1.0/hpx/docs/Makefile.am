hpxdocdir=$(top_builddir)/docs/man/man3

dist_man3_MANS = $(hpxdocdir)/hpx_parcel.3
$(hpxdocdir)/hpx_parcel.3: doxyfile.stamp

doxyfile.stamp:
	$(DOXYGEN) doxygen
	echo Timestamp > doxyfile.stamp

CLEANFILES += doxyfile.stamp

all-local: doxyfile.stamp
dist-clean:
clean-local:
	rm -rf $(top_builddir)/docs/man/

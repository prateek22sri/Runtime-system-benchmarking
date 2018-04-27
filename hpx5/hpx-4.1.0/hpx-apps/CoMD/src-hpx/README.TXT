This is CoMD version 1.1
========================


This implements the port HPX-5 port for CoMD version 1.1. 

Obtaining Documentation
=======================

CoMD documentation is produced by doxygen (www.doxygen.org).  
In order to generate the call graphs, you will also need graphviz 
installed. Alternatively, you can set 

HAVE_DOT = NO 

in the file 'Doxyfile' in the source directory. 

To build the documentation, navigate to the source directory 
(src-hpx) and run the command:

$ doxygen

To read the documenation, point a browser at 'html/index.html'.  
If you are unable to run doxygen on your local system you can download a
pre-built html directory from

XXXXXXXXXXXXXXXXXXXXXXX

Or, point your browser at

XXXXXXXXXXXXXXXXXXXXXXX


Building CoMD-hpx
=================

CoMD is written with portability in mind and should compile using
practically any compiler that implements the C99 standard.  You will
need to modify the Makefile provided to suit your platform.

to compile, type

$ make

The sample Makefile will compile the code on many platforms and will produce
the executale in ../bin (the default). See comments in Makefile for 
information about specifying the name of the C compiler and/or additional 
compiler switches that might be necessary for your platform.


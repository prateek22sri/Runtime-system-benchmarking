# HPX–5

HPX-5 is the High Performance ParalleX runtime library developed at CREST at
Indiana University. This open source, portable, and performance-oriented
implementation is guided by the ParalleX execution model. It provides a
programming model for parallel and distributed applications, allowing programs
to run unmodified on systems from a single SMP to large clusters or
supercomputers with thousands of nodes. HPX-5 supports Linux running on Intel
x86, Intel Knights Landing, IBM Power8 and various ARM platforms. We use HPX-5 
for a broad range of scientific applications enabling scientists write code 
that performs and scales better than contemporary parallel runtimes.

HPX–5 supports both SMP and networked configurations using the Photon networking
library. MPI support is included for legacy applications and portability.

In order to get started with HPX~5, you first need to know some basic
information and how to go about using it. This document discusses how to get up
and running quickly with the HPX–5 Runtime System. Everything from unpacking and
compilation of the distribution to execution of some tools.

# Platform Support

HPX-5 is tested on Linux on x86_64, Intel Knights Landing, ARMv7, AArch64 (ARMv8) 
and IBM Power8. Experimental support is available for SMP execution on Darwin on
x86_64.

# Tool Support

The following tools have been verified to work with HPX-5.

* GCC 4.8.4 or newer (Tested with 4.8.4, 4.9.1, 4.9.2, 5.1.0, 5.2.0, 5.3.0, 
                      6.2.0).
* clang Tested with 3.8.1 (3.5 and earlier are known to be broken).
* icc (Tested with 17.1).
* GNU Make 3.81+

# Configuration and Building

HPX-5 relies on standard autotools infrastructure for configuration and
building. Basic details can be found in the INSTALL file, while advanced
instructions are available on the website.

```
$ ./configure --prefix=/path-to-install --enable-mpi --enable-photon
$ make
$ make install
$ export PKG_CONFIG_PATH=/path-to-install/lib/pkgconfig:$PKG_CONFIG_PATH
$ export LD_LIBRARY_PATH=/path-to-install/lib:$LD_LIBRARY_PATH
```

# Running

HPX–5 provides runtime options that can be specified on the command line or in
the environment. The list of options can be obtained from any HPX–5 program by
adding --hpx-help option.

HPX–5 programs can be run using any of the MPI or PMI launchers such as mpirun
or mpiexec.  E.g. to run the pingpong example,

```
$ mpirun -np 2 examples/pingpong 10 -m -v
```

Detailed build instructions can be found in the User's Guide to HPX–5 found at
https://hpx.crest.iu.edu/documentation and FAQs in
(https://hpx.crest.iu.edu/faqs_and_tutorials).

CoMD
====

This is a port of CoMD for HPX-5. It is developed and maintained at the 
Center for Research in Extreme Scale Technologies (CREST), 
Indiana University Bloomington.

CoMD is a reference implementation of typical classical molecular dynamics 
algorithms and workloads. It is created and maintained by ExMatEx: 
Exascale Co-Design Center for Materials in Extreme Environments 
(<a href="http://exmatex.org">exmatex.org</a>).  

The code is intended to serve as a vehicle for co-design by allowing
others to extend and/or reimplement it as needed to test performance of 
new architectures, programming models, etc.

The directory "src-hpx" contains source files for CoMD ported to HPX-5. 
Please refer to "README.TXT" there on how to compile for HPX-5.

How to Run
==========

The directory "examples" contains templates on how to run and do strong and
weak scaling. 

How to Check for Correctness
============================

The simulation prints validation data at the end. There are two validation tests
Total Energy conservation and number of atoms be same.

Known Limitations
=================

1 - Currently the HPX port only works for Lennard-Jones Potential which is the default.
The other Embedded Atom Method Potential has some bugs. This will be fixed, 
hopefully, in the next release.

Therefore, running CoMD-hpx with option "-e" will throw errors.

2 - The performance measurements for CoMD-hpx does not make sense as they 
come from the original code that is implemented using MPI and uses the 
Bulk Synchronous Model, whereas HPX defies that.

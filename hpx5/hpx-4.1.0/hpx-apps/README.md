HPX-5 Example Applications
                        
What is it?
===========

HPX-5 is a new and exciting exascale runtime system, developed at Indiana 
University (IU) Center for Research in Extreme Scale Technologies (CREST). 
More informatin about the runtime system can be found at: 
http://hpx.crest.iu.edu

This is a separate but closely related package of example HPX-5 applications. 
The included applications expose and showcase HPX-5, and are not meant to be
production-scale software artifacts. Currently, this release contains four 
applications:

* Lulesh, a physics code, and
* libPXGL, a simple graph library containing various SSSP algorithms.
* Wavelet-amr, solves the Euler equations for a blast wave in 2-D.
* HPCG, this version of HPCG is based off of the HPCG reference 
  implementation.
* CoMD,  The CoMD proxy application is a classical molecular dynamics 
proxy application with OpenCL, OpenMP, and MPI reference implementations 
for comparison.

More information about the applications can be found at:

http://hpx.crest.iu.edu/applications

Lulesh
------

LULESH (Livermore Unstructured Lagrangian Explicit Shock Hydrodynamics) is 
a proxy application representing a kernel of scientific computation. It
has been created by Lawrence Livermore National Lab Co-design center, in part, 
to directly compare performance characteristics and to explore and quantify the
benefits of different approaches to computation.

LibPXGL (SSSP)
---------------

Following the philosophy of design of Standard Template Library(STL) and and 
the trail of evolution of Parallel Boost graph library(PBGL), libPXGL is the 
beginning of an endeavor to develop a next-generation graph library based on 
HPX-5. Our First effort is invested in implementing graph algorithms for solving
the Single Source Shortest Path(SSSP) problem as SSSP is a good representative 
of a class of irregular graph problems. Given a graph and a source, the SSSP 
problem asks to find out the shortest possible distances from the source to all 
other vertices in the graph.

Wavelet AMR
---------------
This wavelet example solves the Euler equations for a blast wave in 2-D.
The wavelet example comes in three flavors: serial, cilk, and hpx. Its 
communication patterns are irregular, dynamic, and nonuniform. For the 
cilk and hpx flavors, the serial version is also present in the source code
to aid comparison.

HPCG
------
HPCG — The HPX-5 HPCG is a comprehensive port of the MPI-OpenMP HPCG reference 
implementation. It closely follows the MPI-OpenMP implementation but using 
active messages and HPX-5 semantics. High Performance Conjugate Gradients 
(HPCG) is a benchmark project aimed to create a more relevant metric for 
ranking HPC systems. Alterations to sparse matrix-vector multiply include RDMA 
based put and get approaches have been added. Also added is a new demonstration 
of an HPX+MPI legacy support modality where HPX and MPI coexist. In this 
modality, some kernels use HPX whilst others use MPI in order to demonstrate a 
path forward for porting large legacy applications.

CoMD
-----------

The CoMD proxy application is a classical molecular dynamics proxy application 
with OpenCL, OpenMP, and MPI reference implementations for comparison. It was 
developed as part of the ExMatEx Codesign center. Information on CoMD can be 
found on the CoMD Proxy Application website: http://www.exmatex.org/comd.html. 
The beta release of CoMD-HPX is feature complete though not yet performance 
oriented.  

Tutorials
-----------

In addition, hpx-apps has two tutorials 1dtree and advection equation. Detailed 
information about the tutorial can be found at http://hpx.crest.iu.edu/tutorial

These programs implement the [LULESH](https://codesign.llnl.gov/lulesh.php)
mini-app.

An MPI reference implementation with which to compare may be downloaded from
[(https://codesign.llnl.gov/lulesh.php)]((https://codesign.llnl.gov/lulesh.php)).

In the MPI implementation a cubic number of MPI processes work together to
calculate the final result.

The `parcels` folder contains a version of LULESH that is implemented similarly
to how the MPI LULESH is implemented. Each HPX–5 locality runs a number of
threads equal to the number of operating system threads/cores the the HPX–5
runtime is set to run with. Thus, on a system with 16 cores, 8 nodes will run
efficiently up to 125 LULESH “domains” since there are 128 cores available. Each
thread maintains local data and must always copy data to other threads, even
when they are located at the same locality.

To compare the parcels LULESH with the MPI LULESH, run both with the same number
of domains, points, and iterations.

The `omp_parcels` folder contains a version of LULESH that refactors the parcels
implementation slightly. Each HPX–5 locality has one domain, and parallelized
loops are used to obtain parallelism instead. It works similarly to how the MPI
with OpenMP version of LULESH works. The version of LULESH must be run with a
cubic number of localities.
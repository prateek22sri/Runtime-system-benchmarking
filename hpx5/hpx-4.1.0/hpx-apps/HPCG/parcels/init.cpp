
//@HEADER
// ***************************************************
//
// HPCG: High Performance Conjugate Gradient Benchmark
//
// Contact:
// Michael A. Heroux ( maherou@sandia.gov)
// Jack Dongarra     (dongarra@eecs.utk.edu)
// Piotr Luszczek    (luszczek@eecs.utk.edu)
//
// ***************************************************
//@HEADER

#include <ctime>
#include <cstdio>
#include <cstring>

#include <fstream>
#include <iostream>

#include "hpcg-main.h"

#include "ReadHpcgDat.hpp"

std::ofstream HPCG_fout; //!< output file stream for logging activities during HPCG run

static int
startswith(const char * s, const char * prefix) {
  size_t n = strlen( prefix );
  if (strncmp( s, prefix, n ))
    return 0;
  return 1;
}

/*!
  Initializes an HPCG run by obtaining problem parameters (from a file on
  command line) and then broadcasts them to all nodes. It also initializes
  loggin I/O streams that are used throughout the HPCG run. Only MPI rank 0
  performs I/O operations.

  The function assumes that MPI has already been initialized for MPI runs.

  @param[in] argc_p the pointer to the "argc" parameter passed to the main() function
  @param[in] argv_p the pointer to the "argv" parameter passed to the main() function
  @param[out] params the reference to the data structures that is filled the basic parameters of the run

  @return returns 0 upon success and non-zero otherwise

  @see HPCG_Finalize
*/

extern HPCG_Params gparams;

int HPCG_Init(int * argc_p, char ** *argv_p);

int
HPCG_Init(int * argc_p, char ** *argv_p) {
  int argc = *argc_p;
  char ** argv = *argv_p;
  char fname[80];
  int i, j, iparams[4];
  char cparams[3][6] = {"--nx=", "--ny=", "--nz="};
  time_t rawtime;
  tm * ptm;

  /* for sequential and some MPI implementations it's OK to read first three args */
  for (i = 0; i < 3; ++i)
    if (argc <= i+1 || sscanf(argv[i+1], "%d", iparams+i) != 1 || iparams[i] < 10) iparams[i] = 0;

  /* for some MPI environments, command line arguments may get complicated so we need a prefix */
  for (i = 1; i <= argc && argv[i]; ++i)
    for (j = 0; j < 3; ++j)
      if (startswith(argv[i], cparams[j]))
        if (sscanf(argv[i]+strlen(cparams[j]), "%d", iparams+j) != 1 || iparams[j] < 10) iparams[j] = 0;

  if (! iparams[0] && ! iparams[1] && ! iparams[2]) { /* no geometry arguments on the command line */
    ReadHpcgDat(iparams, iparams+3);
  }

  for (i = 0; i < 3; ++i) {
    if (iparams[i] < 16)
      for (j = 1; j <= 2; ++j)
        if (iparams[(i+j)%3] > iparams[i])
          iparams[i] = iparams[(i+j)%3];
    if (iparams[i] < 16)
      iparams[i] = 16;
  }

  //gparams.nx = iparams[0];
  //gparams.ny = iparams[1];
  //gparams.nz = iparams[2];
  gparams.nx = 64;
  gparams.ny = 64;
  gparams.nz = 64;

  //gparams.runningTime = iparams[3];
  gparams.runningTime = 10;

  time ( &rawtime );
  ptm = localtime(&rawtime);
  sprintf( fname, "hpcg_log_%04d.%02d.%02d.%02d.%02d.%02d.txt",
      1900 + ptm->tm_year, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec );

  HPCG_fout.open(fname);

  return 0;
}

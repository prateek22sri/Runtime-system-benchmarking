// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2016-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

/*
Copyright (c) 2013, Los Alamos National Security, LLC
All rights reserved.

Copyright 2013. Los Alamos National Security, LLC. This software was produced 
under U.S. Government contract DE-AC52-06NA25396 for Los Alamos National 
Laboratory (LANL), which is operated by Los Alamos National Security, LLC for 
the U.S. Department of Energy. The U.S. Government has rights to use, reproduce, 
and distribute this software.  NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL 
SECURITY, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY 
FOR THE USE OF THIS SOFTWARE.  If software is modified to produce derivative 
works, such modified software should be clearly marked, so as not to confuse 
it with the version available from LANL.

Additionally, redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

* Neither the name of Los Alamos National Security, LLC, Los Alamos National 
Laboratory, LANL, the U.S. Government, nor the names of its contributors may be
 used to endorse or promote products derived from this software without specific
 prior written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL SECURITY, LLC OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

/// \file
/// Write simulation information in YAML format.
///
/// Information regarding platform, run parameters, performance, etc.,
/// are written to a file whose name is generated from the CoMDVariant
/// and the time of the run.  This provides a simple mechanism to track
/// and compare performance etc.
///
/// There are much more sophisticated libraries and routines available
/// to handle YAML, but this simple implemenation handles everything we
/// really need.

#include "yamlOutput.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "CoMD_info.h"
#include "mytype.h"
#include "parallel_hpx.h"

FILE* yamlFile = NULL;

static const char* CoMDVersion = "1.1";
static const char* CoMDVariant = CoMD_VARIANT;

static void getTimeString(char* timestring) {
  time_t rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);

  sprintf(timestring, "%4d-%02i-%02d, %02d:%02d:%02d", timeinfo->tm_year + 1900,
      timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour,
      timeinfo->tm_min, timeinfo->tm_sec);
}

void yamlBegin(void) {
  if (!printRank())
    return;

  char filename[64];
  time_t rawtime;
  time(&rawtime);
  struct tm* ptm = localtime(&rawtime);
  char sdate[25];
  //use tm_mon+1 because tm_mon is 0 .. 11 instead of 1 .. 12
  sprintf(sdate, "%04d:%02d:%02d-%02d:%02d:%02d", ptm->tm_year + 1900,
      ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  sprintf(filename, "%s.%s.yaml", CoMDVariant, sdate);
  yamlFile = fopen(filename, "w");
}

void yamlAppInfo(FILE* file) {
  if (!printRank())
    return;
  printSeparator(file);
  fprintf(file, "Mini-Application Name    : %s\n", CoMDVariant);
  fprintf(file, "Mini-Application Version : %s\n", CoMDVersion);
  fprintf(file, "Platform:\n");
  fprintf(file, "  hostname: %s\n", CoMD_HOSTNAME);
  fprintf(file, "  kernel name: %s\n", CoMD_KERNEL_NAME);
  fprintf(file, "  kernel release: %s\n", CoMD_KERNEL_RELEASE);
  fprintf(file, "  processor: %s\n", CoMD_PROCESSOR);
  fprintf(file, "Build:\n");
  fprintf(file, "  CC: %s\n", CoMD_COMPILER);
  fprintf(file, "  compiler version: %s\n", CoMD_COMPILER_VERSION);
  fprintf(file, "  CFLAGS: %s\n", CoMD_CFLAGS);
  fprintf(file, "  LDFLAGS: %s\n", CoMD_LDFLAGS);
  //fprintf(file,"  using MPI: %s\n",        builtWithMpi() ? "true":"false");
  fprintf(file, "  Threading: none\n");
  fprintf(file, "  Double Precision: %s\n",
      (sizeof(real_t) == sizeof(double) ? "true" : "false"));
  char timestring[32];
  getTimeString(timestring);
  fprintf(file, "Run Date/Time: %s\n", timestring);
  fprintf(file, "\n");
  fflush(file);
}

void yamlEnd(void) {
  if (!printRank())
    return;

  fclose(yamlFile);
}

void printSeparator(FILE* file) {
  //fprintf(file,"=========================================================================\n");
  fprintf(file, "\n");
}


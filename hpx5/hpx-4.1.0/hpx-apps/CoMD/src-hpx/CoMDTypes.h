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
/// CoMD data structures.

#ifndef __COMDTYPES_H_
#define __COMDTYPES_H_

#include <stdio.h>
#include "mytype.h"
#include "haloExchange.h"
#include "linkCells.h"
#include "decomposition.h"
#include "initAtoms.h"

#include <hpx/hpx.h>

struct SimFlatSt;

/// The base struct from which all potentials derive.  Think of this as an
/// abstract base class.
///
/// CoMD uses the following units:
///  - distance is in Angstroms
///  - energy is in eV
///  - time is in fs
///  - force in in eV/Angstrom
///
///  The choice of distance, energy, and time units means that the unit
///  of mass is eV*fs^2/Angstrom^2.  Hence, we must convert masses that
///  are input in AMU (atomic mass units) into internal mass units.
typedef struct BasePotentialSt {
  real_t cutoff;          //!< potential cutoff distance in Angstroms
  real_t mass;            //!< mass of atoms in intenal units
  real_t lat;             //!< lattice spacing (angs) of unit cell
  char latticeType[8];    //!< lattice type, e.g. FCC, BCC, etc.
  char name[3];	   //!< element name
  int atomicNo;	   //!< atomic number
  int (*force)(struct SimFlatSt* s); //!< function pointer to force routine
  void (*print)(FILE* file, struct BasePotentialSt* pot);
  void (*destroy)(struct BasePotentialSt** pot); //!< destruction of the potential
} BasePotential;

/// species data: chosen to match the data found in the setfl/funcfl files
typedef struct SpeciesDataSt {
  char name[3];   //!< element name
  int atomicNo;  //!< atomic number
  real_t mass;     //!< mass in internal units
} SpeciesData;

/// Simple struct to store the initial energy and number of atoms. 
/// Used to check energy conservation and atom conservation. 
typedef struct ValidateSt {
  double eTot0; //<! Initial total energy
  int nAtoms0;  //<! Initial global number of atoms
} Validate;

/// 
/// The fundamental simulation data structure with MAXATOMS in every
/// link cell.
/// 
typedef struct SimFlatSt {
  hpx_addr_t base;		  //<! base pointer to the global sim

  int nSteps;            //<! number of time steps to run
  int printRate;         //<! number of steps between output
  double dt;             //<! time step

  Domain* domain;        //<! domain decomposition data
  LinkCell* boxes;       //<! link-cell data
  Atoms* atoms;          //<! atom data (positions, momenta, ...)
  SpeciesData* species;  //<! species data (per species, not per atom)

  real_t ePotential;     //!< the total potential energy of the system
  real_t eKinetic;       //!< the total kinetic energy of the system

  BasePotential *pot;	  //!< the potential
  HaloExchange* atomExchange;

  hpx_addr_t exchange_recv[2];	  // Future LCO to sync between generations
  hpx_addr_t exchange_and[2]; // LCO for recieving msg from a single generation

} SimFlat;

typedef struct Reductions {
  hpx_addr_t allreduce;
  hpx_addr_t reduction_sum;
  int32_t reduceid;

  hpx_addr_t dallreduce;
  hpx_addr_t dreduction_sum;
  int32_t dreduceid;

  hpx_addr_t allreduce_max;
  hpx_addr_t reduction_max;
  int32_t reducemaxid;

  hpx_addr_t allreduce_minRank;
  hpx_addr_t reduction_minRank;
  int32_t reduceminRankid;

  hpx_addr_t allreduce_maxRank;
  hpx_addr_t reduction_maxRank;
  int32_t reducemaxRankid;

} Reductions;

typedef struct {
  hpx_addr_t reduction_sum;
  hpx_addr_t dreduction_sum;

  hpx_addr_t reduction_max;
  hpx_addr_t reduction_minRank;
  hpx_addr_t reduction_maxRank;

} Communication;

typedef int local_int_t;
typedef long long global_int_t;

//Note: this code only works with double not float
typedef double local_real_t;
typedef double global_real_t;

#endif

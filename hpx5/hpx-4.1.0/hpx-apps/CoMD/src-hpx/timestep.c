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
/// Leapfrog time integrator

#include "timestep.h"

#include "CoMDTypes.h"
#include "linkCells.h"
#include "parallel_hpx.h"
#include "performanceTimers.h"

static void advanceVelocity(SimFlat* s, int nBoxes, real_t dt);
static void advancePosition(SimFlat* s, int nBoxes, real_t dt);

/// Advance the simulation time to t+dt using a leap frog method
/// (equivalent to velocity verlet).
///
/// Forces must be computed before calling the integrator the first time.
///
///  - Advance velocities half time step using forces
///  - Advance positions full time step using velocities
///  - Update link cells and exchange remote particles
///  - Compute forces
///  - Update velocities half time step using forces
///
/// This leaves positions, velocities, and forces at t+dt, with the
/// forces ready to perform the half step velocity update at the top of
/// the next call.
///
/// After nSteps the kinetic energy is computed for diagnostic output.
double timestep(SimFlat* s, int nSteps, real_t dt) {
  for (int ii = 0; ii < nSteps; ++ii) {
    startTimer(velocityTimer);
    advanceVelocity(s, s->boxes->nLocalBoxes, 0.5 * dt);
    stopTimer(velocityTimer);

    startTimer(positionTimer);
    advancePosition(s, s->boxes->nLocalBoxes, dt);
    stopTimer(positionTimer);

    startTimer(redistributeTimer);
    redistributeAtoms(s);
    stopTimer(redistributeTimer);

    startTimer(computeForceTimer);
    computeForce(s);
    stopTimer(computeForceTimer);

    startTimer(velocityTimer);
    advanceVelocity(s, s->boxes->nLocalBoxes, 0.5 * dt);
    stopTimer(velocityTimer);
  }

  kineticEnergy(s);

  return s->ePotential;
}

void computeForce(SimFlat* s) {
  s->pot->force(s);
}

void advanceVelocity(SimFlat* s, int nBoxes, real_t dt) {
  for (int iBox = 0; iBox < nBoxes; iBox++) {
    for (int iOff = MAXATOMS * iBox, ii = 0; ii < s->boxes->nAtoms[iBox];
        ii++, iOff++) {
      s->atoms->p[iOff][0] += dt * s->atoms->f[iOff][0];
      s->atoms->p[iOff][1] += dt * s->atoms->f[iOff][1];
      s->atoms->p[iOff][2] += dt * s->atoms->f[iOff][2];
    }
  }
}

void advancePosition(SimFlat* s, int nBoxes, real_t dt) {
  for (int iBox = 0; iBox < nBoxes; iBox++) {
    for (int iOff = MAXATOMS * iBox, ii = 0; ii < s->boxes->nAtoms[iBox];
        ii++, iOff++) {
      int iSpecies = s->atoms->iSpecies[iOff];
      real_t invMass = 1.0 / s->species[iSpecies].mass;
      s->atoms->r[iOff][0] += dt * s->atoms->p[iOff][0] * invMass;
      s->atoms->r[iOff][1] += dt * s->atoms->p[iOff][1] * invMass;
      s->atoms->r[iOff][2] += dt * s->atoms->p[iOff][2] * invMass;
    }
  }
}

/// Calculates total kinetic and potential energy across all tasks.  The
/// local potential energy is a by-product of the force routine.
void kineticEnergy(SimFlat* s) {
  real_t eLocal[2];
  eLocal[0] = s->ePotential;
  eLocal[1] = 0;
  for (int iBox = 0; iBox < s->boxes->nLocalBoxes; iBox++) {
    for (int iOff = MAXATOMS * iBox, ii = 0; ii < s->boxes->nAtoms[iBox];
        ii++, iOff++) {
      int iSpecies = s->atoms->iSpecies[iOff];
      real_t invMass = 0.5 / s->species[iSpecies].mass;
      eLocal[1] += (s->atoms->p[iOff][0] * s->atoms->p[iOff][0]
          + s->atoms->p[iOff][1] * s->atoms->p[iOff][1]
          + s->atoms->p[iOff][2] * s->atoms->p[iOff][2]) * invMass;
    }
  }

  real_t eSum[2];
  startTimer(commReduceTimer);
  addRealParallel(eLocal, eSum, 2);
  stopTimer(commReduceTimer);

  s->ePotential = eSum[0];
  s->eKinetic = eSum[1];
}

/// \details
/// This function provides one-stop shopping for the sequence of events
/// that must occur for a proper exchange of halo atoms after the atom
/// positions have been updated by the integrator.
///
/// - updateLinkCells: Since atoms have moved, some may be in the wrong
///   link cells.
/// - haloExchange (atom version): Sends atom data to remote tasks. 
/// - sort: Sort the atoms.
///
/// \see updateLinkCells
/// \see initAtomHaloExchange
/// \see sortAtomsInCell
void redistributeAtoms(SimFlat* sim) {

  updateLinkCells(sim->boxes, sim->atoms);

  startTimer(atomHaloTimer);
  haloExchange(sim->atomExchange, sim, sim);
  stopTimer(atomHaloTimer);

  for (int ii = 0; ii < sim->boxes->nTotalBoxes; ++ii)
    sortAtomsInCell(sim->atoms, sim->boxes, ii);
}

// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

#ifndef LULESH_TRACE_H
#define LULESH_TRACE_H

#ifndef ENABLE_TRACING
# define TRACE_INIT(d)
# define TRACE_FINI(d)
# define TRACE(d, i)
#else
struct Domain;


static const int START_ALLREDUCE            = 0;
static const int START_CALC_FORCE           = 1;
static const int START_SBN3_SENDS           = 2;
static const int START_SBN3_RECVS           = 3;
static const int END_SBN3_RECVS             = 4;
static const int START_ALLREDUCE_GET        = 5;
static const int END_ALLREDUCE_GET          = 6;
static const int START_CALC_VELOCITY        = 7;
static const int START_CALC_POSITION        = 8;
static const int START_POSVEL_SENDS         = 9;
static const int START_POSVEL_RECVS         = 10;
static const int END_POSVEL_RECVS           = 11;
static const int START_LAGRANGE_ELEMENTS    = 12;
static const int START_MONOQ_SENDS          = 13;
static const int START_MONOQ_RECVS          = 14;
static const int END_MONOQ_RECVS            = 15;
static const int START_CALC_TIMECONSTRAINTS = 16;
static const int END_CALC_TIMECONSTRAINTS   = 17;

static const char * TRACE_EVENTS[] = {
  "set allreduce",                              // 0
  "start CalcForceForNodes",                    // 1
  "start SBN3 sends",                           // 2
  "start SBN3 recvs",                           // 3
  "end SBN3",                                   // 4
  "start allreduce get",                        // 5
  "end allreduce get",                          // 6
  "start CalcVelocityForNodes",                 // 7
  "start CalcPositionForNodes",                 // 8
  "start PosVel sends",                         // 9
  "start PosVel recvs",                         // 10
  "end PosVel",                                 // 11
  "start LagrangeElements",                     // 12
  "start MonoQ sends",                          // 13
  "start MonoQ recvs",                          // 14
  "MonoQ recvs",                                // 15
  "start CalcTimeConstraintsForElems",          // 16
  "end CalcTimeConstraintsForElems"             // 17
};

void TRACE_INIT(struct Domain *);
void TRACE_FINI(struct Domain *);
void TRACE(struct Domain *, int);
#endif // ENABLE_TRACING

#endif // LULESH_TRACE_H

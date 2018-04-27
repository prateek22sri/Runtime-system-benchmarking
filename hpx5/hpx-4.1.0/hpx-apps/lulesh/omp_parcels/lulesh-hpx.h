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

#ifndef LULESH_HPX_H
#define LULESH_HPX_H

#include <limits.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "hpx/hpx.h"

#define VOLUME_ERROR -1
#define QSTOP_ERROR -2

#define XI_M        0x00007
#define XI_M_SYMM   0x00001
#define XI_M_FREE   0x00002
#define XI_M_COMM   0x00004

#define XI_P        0x00038
#define XI_P_SYMM   0x00008
#define XI_P_FREE   0x00010
#define XI_P_COMM   0x00020

#define ETA_M       0x001c0
#define ETA_M_SYMM  0x00040
#define ETA_M_FREE  0x00080
#define ETA_M_COMM  0x00100

#define ETA_P       0x00e00
#define ETA_P_SYMM  0x00200
#define ETA_P_FREE  0x00400
#define ETA_P_COMM  0x00800

#define ZETA_M      0x07000
#define ZETA_M_SYMM 0x01000
#define ZETA_M_FREE 0x02000
#define ZETA_M_COMM 0x04000

#define ZETA_P      0x38000
#define ZETA_P_SYMM 0x08000
#define ZETA_P_FREE 0x10000
#define ZETA_P_COMM 0x20000

#define SEDOV_SYNC_POS_VEL_EARLY 1

#define MAX_FIELDS_PER_MPI_COMM 6

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define CACHE_COHERENCE_PAD_REAL (128/sizeof(double))
#define CACHE_ALIGN_REAL(n) \
  (((n) + (CACHE_COHERENCE_PAD_REAL - 1)) & ~(CACHE_COHERENCE_PAD_REAL-1))

typedef void (*send_t)(int nx, int ny, int nz, double *src, double *dest);
typedef void (*recv_t)(int nx, int ny, int nz, double *src, double *dest, int type);

typedef struct {
  hpx_addr_t     base;
  int           index;
  int           nDoms;
  int              nx;
  int       maxcycles;
  hpx_addr_t complete;
  hpx_addr_t newdt;
  hpx_addr_t elapsed_ar;
} InitArgs;

typedef struct {
  unsigned long epoch;
  int     srcLocalIdx;
  double        buf[];                         // inline, variable length buffer
} NodalArgs;

typedef struct Domain {
  hpx_addr_t base;
  hpx_addr_t newdt;
  hpx_addr_t elapsed_ar;
  hpx_addr_t complete;
  hpx_addr_t sem_sbn1;
  hpx_addr_t sem_sbn3;
  hpx_addr_t sem_posvel;
  hpx_addr_t sem_monoq;

  int *nodeElemCount;
  int *nodeElemStart;
  int *nodeElemCornerList;

  // Elem-centered
  int *matElemlist;  // material indexset
  int *nodelist;     // elemToNode connectivity

  int *lxim;         // elem connectivity through face
  int *lxip;
  int *letam;
  int *letap;
  int *lzetam;
  int *lzetap;

  int *elemBC;           // elem face symm/free-surface flag
  double *e;             // energy
  double *p;             // pressure
  double *q;             // q
  double *ql;            // linear term for q
  double *qq;            // quadratic term for q
  double *v;             // volume
  double *volo;          // reference volume
  double *delv;          // m_vnew - n_v
  double *vdov;          // volume derivative over volume

  double *arealg;        // elem characteristic length
  double *ss;            // sound speed
  double *elemMass;      // mass

  // Elem temporaries
  double *vnew;          // new relative volume
  double *delv_xi;       // velocity gradient
  double *delv_eta;
  double *delv_zeta;

  double *delx_xi;       // position gradient
  double *delx_eta;
  double *delx_zeta;

  double *dxx;           // principal strains
  double *dyy;
  double *dzz;

  // Node-centered
  double *x;             // coordinates
  double *y;
  double *z;

  double *xd;            // derivatives
  double *yd;
  double *zd;

  double *xdd;           // accelerations
  double *ydd;
  double *zdd;

  double *fx;            // forces
  double *fy;
  double *fz;

  double *nodalMass;     // mass

  // Boundary nodesets
  int *symmX;        // nodes on X symmetry plane
  int *symmY;        // nodes on Y symmetry plane
  int *symmZ;        // nodes on Z symmetry plane

  // Parameters
  double dtfixed;        // fixed time increment
  double time;           // current time
  double deltatime;      // variable time increment
  double deltatimemultlb;
  double deltatimemultub;
  double stoptime;       // end time for simulation

  double u_cut;          // velocity tolerance
  double hgcoef;         // hourglass control
  double qstop;          // excessive q indicator
  double monoq_max_slope;
  double monoq_limiter_mult;
  double e_cut;          // energy tolerance
  double p_cut;          // pressure tolerance
  double ss4o3;
  double q_cut;          // q tolerance
  double v_cut;          // relative volume tolerance
  double qlc_monoq;      // linear term coef for q
  double qqc_monoq;      // quadratic term coef for q
  double qqc;
  double eosvmax;
  double eosvmin;
  double pmin;           // pressure floor
  double emin;           // energy floor
  double dvovmax;        // maximum allowable volume change
  double refdens;        // reference density
  double dtcourant;      // courant constraint
  double dthydro;        // volume change constraint
  double dtmax;          // maximum allowable time increment
  int cycle;             // iteration count for simulation
  int maxcycles;         // user-specified max iteration counts

  int colLoc;
  int rowLoc;
  int planeLoc;
  int tp;
  int rank;
  int nDomains;

  int sizeX;
  int sizeY;
  int sizeZ;
  int numElem;
  int numNode;
  int affinity;

  // hpx_future_t *dataSendTT[26];
  // hpx_future_t *dataSendTF[26];
  // hpx_future_t *dataSendFF[26];
  // hpx_future_t SBN1[26];

  double *dataRecvTT[26];
  double *dataRecvTF[26];
  double *dataRecvFF[26];

  int sendTT[7];
  int sendTF[27];
  int sendFF[14];

  int recvTT[7];
  int recvTF[27];
  int recvFF[14];
  int reverse_recvTT[27];

  hpx_addr_t epoch;                             // an epoch generation counter
  hpx_addr_t sbn1_and;                          // local completion reduction
  hpx_addr_t sbn3_and[2];                       // local completion reduction
  hpx_addr_t posvel_and[2];
  hpx_addr_t monoq_and[2];
} Domain;

typedef struct {
  Domain *domain;
  int destLocalIdx;
  unsigned long epoch;
} pSBN;

int OFFSET[26], BUFSZ[26], XFERCNT[26], MAXEDGESIZE, MAXPLANESIZE;
send_t SENDER[26];
recv_t RECEIVER[26];

//hpx_future_t *fut_deltaTime;

#ifdef DIAGNOSTICS
extern uint64_t remote_comms;
extern uint64_t total_comms;
#endif

typedef struct {
  bool core_major;
  int num_domains;
} global_config_t;

extern global_config_t global_config;
extern bool core_major;
extern int num_domains;
hpx_addr_t get_domain_addr(hpx_addr_t base, int index);

void Init(int tp, int nx);

void SetDomain(int rank, int colLoc, int rowLoc, int planeLoc, int nx, int tp,
           int nDomains, int maxcycles,Domain *domain);

void DestroyDomain(Domain *domain);

void AdvanceDomain(void *data);

void CalcForceForNodes(hpx_addr_t local,Domain *domain,int rank,unsigned long epoch);

void CalcVolumeForceForElems(Domain *domain,int rank);

void CalcQForElems(hpx_addr_t local,Domain *domain,unsigned long epoch);

void InitStressTermsForElems(double *p, double *q, double *sigxx, double *sigyy,
                 double *sigzz, int numElem);

void IntegrateStressForElems(int *nodelist, double *x, double *y, double *z,
                 double *fx, double *fy, double *fz,
                 int *nodeElemCount,
                 int *nodeElemStart,
                 int *nodeElemCornerList,
                 double *sigxx, double *sigyy, double *sigzz,
                 double *determ, int numElem,int numNode);

void CalcHourglassControlForElems(Domain *domain, double determ[], double hgcoef);

void CollectDomainNodesToElemNodes(double *x, double *y, double *z, const int *elemToNode,
                   double elemX[8], double elemY[8], double elemZ[8]);

void CalcElemVolumeDerivative(double dvdx[8], double dvdy[8], double dvdz[8],
                  const double x[8], const double y[8], const double z[8]);

void VoluDer(const double x0, const double x1, const double x2,
         const double x3, const double x4, const double x5,
         const double y0, const double y1, const double y2,
         const double y3, const double y4, const double y5,
         const double z0, const double z1, const double z2,
         const double z3, const double z4, const double z5,
         double *dvdx, double *dvdy, double *dvdz);

void CalcFBHourglassForceForElems(int *nodelist, double *ss, double *elemMass,
                  double *xd, double *yd, double *zd,
                  double *fx, double *fy, double *fz, 
                  int *nodeElemCount,
                  int *nodeElemStart,
                  int *nodeElemCornerList,
                  double *determ,
                  double *x8n, double *y8n, double *z8n,
                  double *dvdx, double *dvdy, double *dvdz,
                  double hourg, int numElem,int numNode);

void CalcElemFBHourglassForce(double *xd, double *yd, double *zd, double *hourgam0,
                  double *hourgam1, double *hourgam2, double *hourgam3,
                  double *hourgam4, double *hourgam5, double *hourgam6,
                  double *hourgam7, double coefficient,
                  double *hgfx, double *hgfy, double *hgfz);

void CalcElemShapeFunctionDerivatives(double const x[], double const y[], double const z[],
                      double b[][8], double *const volume);

void CalcElemNodeNormals(double pfx[8], double pfy[8], double pfz[8],
             const double x[8], const double y[8], const double z[8]);

void SumElemFaceNormal(double *normalX0, double *normalY0, double *normalZ0,
               double *normalX1, double *normalY1, double *normalZ1,
               double *normalX2, double *normalY2, double *normalZ2,
               double *normalX3, double *normalY3, double *normalZ3,
               const double x0, const double y0, const double z0,
               const double x1, const double y1, const double z1,
               const double x2, const double y2, const double z2,
               const double x3, const double y3, const double z3);

void SumElemStressesToNodeForces(double B[][8], const double stress_xx,
                 const double stress_yy, const double stress_zz,
                 double fx[], double fy[], double fz[]);

void CalcAccelerationForNodes(double *xdd, double *ydd, double *zdd,
                  double *fx, double *fy, double *fz,
                  double *nodalMass, int numNode);

void ApplyAccelerationBoundaryConditionsForNodes(double *xdd, double *ydd, double *zdd,
                         int *symmX, int *symmY, int *symmZ, int size);

void CalcVelocityForNodes(double *xd, double *yd, double *zd,
              double *xdd, double *ydd, double *zdd,
              const double dt, const double u_cut, int numNode);

void CalcPositionForNodes(double *x, double *y, double *z,
              double *xd, double *yd, double *zd,
              const double dt, int numNode);

void LagrangeElements(hpx_addr_t local,Domain *domain,unsigned long epoch);

void CalcLagrangeElements(Domain *domain);

void CalcKinematicsForElems(int *nodelist, double *x, double *y, double *z,
                double *xd, double *yd, double *zd,
                double *dxx, double *dyy, double *dzz,
                double *v, double *volo, double *vnew, double *delv,
                double *arealg, double deltaTime, int numElem);

double CalcElemCharacteristicLength(const double x[8], const double y[8],
                    const double z[8], const double volume);

double AreaFace(const double x0, const double x1, const double x2, const double x3,
        const double y0, const double y1, const double y2, const double y3,
        const double z0, const double z1, const double z2, const double z3);

void CalcElemVelocityGradient(const double *const xvel, const double *const yvel,
                  const double *const zvel, double b[][8],
                  const double detJ, double *const d);

void CalcMonotonicQGradientsForElems(double *x, double *y, double *z,
                     double *xd, double *yd, double *zd,
                     double *volo, double *vnew, double *delv_xi,
                     double *delv_eta, double *delv_zeta,
                     double *delx_xi, double *delx_eta, double *delx_zeta,
                     int *nodelist, int numElem);

void CalcMonotonicQRegionForElems(int *matElemlist, int *elemBC, int *lxim, int *lxip,
                  int *letam, int *letap, int *lzetam, int *lzetap,
                  double *delv_xi, double *delv_eta, double *delv_zeta,
                  double *delx_xi, double *delx_eta, double *delx_zeta,
                  double *vdov, double *volo, double *vnew,
                  double *elemMass, double *qq, double *ql,
                  double qlc_monoq, double qqc_monoq, double monoq_limiter_mult,
                  double monoq_max_slope, double ptiny, int numElem);

void CalcMonotonicQForElems(Domain *domain);

void CalcPressureForElems(double *p_new, double *bvc, double *pbvc, double *e_old,
              double *compression, double *vnewc, double pmin, double p_cut,
              double eosvmax, int length);

void CalcEnergyForElems(double *p_new, double *e_new, double *q_new, double *bvc, double *pbvc,
            double *p_old, double *e_old, double *q_old, double *compression,
            double *compHalfStep, double *vnewc, double *work, double *delvc,
            double pmin, double p_cut, double e_cut, double q_cut, double emin,
            double *qq_old, double *ql_old, double rho0, double eosvmax, int length);

void CalcSoundSpeedForElems(int *matElemlist, double *ss, double *vnewc, double rho0,
                double *enewc, double *pnewc, double *pbvc, double *bvc,
                double ss4o3, int nz);

void EvalEOSForElems(Domain *domain, double *vnewc, int numElem);

void ApplyMaterialPropertiesForElems(Domain *domain);

void UpdateVolumesForElems(double *vnew, double *v, double v_cut, int length);

void CalcTimeConstraintsForElems(Domain *domain);

void CalcCourantConstraintForElems(int *matElemlist, double *ss, double *vdov, double *arealg,
                   double qqc, int length, double *dtcourant);

void CalcHydroConstraintForElems(int *matElemlist, double *vdov, double dvovmax,
                 int length, double *dthydro);

double CalcElemVolume(const double x[8], const double y[8], const double z[8]);

int _SBN1_result_action(NodalArgs *nodal, size_t size);
extern hpx_action_t _SBN1_result;
int _SBN1_sends_action(pSBN *psbn, size_t size);
extern hpx_action_t _SBN1_sends;
//void SBN1(hpx_addr_t address,Domain *domain, int index);

void SBN1(hpx_addr_t addr, Domain *domain, unsigned long epoch);

int _SBN3_sends_action(pSBN *psbn, size_t size);
extern hpx_action_t _SBN3_sends;
int _SBN3_result_action(NodalArgs *nodal, size_t size);
extern hpx_action_t _SBN3_result;
void SBN3(hpx_addr_t address,Domain *domain, unsigned long epoch);

int _PosVel_sends_action(pSBN *psbn, size_t size);
extern hpx_action_t _PosVel_sends;
int _PosVel_result_action(NodalArgs *nodal, size_t size);
extern hpx_action_t _PosVel_result;
void PosVel(hpx_addr_t address,Domain *domain, unsigned long epoch);


int _MonoQ_sends_action(pSBN *psbn, size_t size);
extern hpx_action_t _MonoQ_sends;
int _MonoQ_result_action(NodalArgs *nodal, size_t size);
extern hpx_action_t _MonoQ_result;
void MonoQ(hpx_addr_t address,Domain *domain, unsigned long epoch);

void send1(int nx, int ny, int nz, double *src, double *dest);

void send2(int nx, int ny, int nz, double *src, double *dest);

void send3(int nx, int ny, int nz, double *src, double *dest);

void send4(int nx, int ny, int nz, double *src, double *dest);

void send5(int nx, int ny, int nz, double *src, double *dest);

void send6(int nx, int ny, int nz, double *src, double *dest);

void send7(int nx, int ny, int nz, double *src, double *dest);

void send8(int nx, int ny, int nz, double *src, double *dest);

void send9(int nx, int ny, int nz, double *src, double *dest);

void send10(int nx, int ny, int nz, double *src, double *dest);

void send11(int nx, int ny, int nz, double *src, double *dest);

void send12(int nx, int ny, int nz, double *src, double *dest);

void send13(int nx, int ny, int nz, double *src, double *dest);

void send14(int nx, int ny, int nz, double *src, double *dest);

void send15(int nx, int ny, int nz, double *src, double *dest);

void send16(int nx, int ny, int nz, double *src, double *dest);

void send17(int nx, int ny, int nz, double *src, double *dest);

void send18(int nx, int ny, int nz, double *src, double *dest);

void send19(int nx, int ny, int nz, double *src, double *dest);

void send20(int nx, int ny, int nz, double *src, double *dest);

void send21(int nx, int ny, int nz, double *src, double *dest);

void send22(int nx, int ny, int nz, double *src, double *dest);

void send23(int nx, int ny, int nz, double *src, double *dest);

void send24(int nx, int ny, int nz, double *src, double *dest);

void send25(int nx, int ny, int nz, double *src, double *dest);

void send26(int nx, int ny, int nz, double *src, double *dest);

void recv1(int nx, int ny, int nz, double *src, double *dest, int type);

void recv2(int nx, int ny, int nz, double *src, double *dest, int type);

void recv3(int nx, int ny, int nz, double *src, double *dest, int type);

void recv4(int nx, int ny, int nz, double *src, double *dest, int type);

void recv5(int nx, int ny, int nz, double *src, double *dest, int type);

void recv6(int nx, int ny, int nz, double *src, double *dest, int type);

void recv7(int nx, int ny, int nz, double *src, double *dest, int type);

void recv8(int nx, int ny, int nz, double *src, double *dest, int type);

void recv9(int nx, int ny, int nz, double *src, double *dest, int type);

void recv10(int nx, int ny, int nz, double *src, double *dest, int type);

void recv11(int nx, int ny, int nz, double *src, double *dest, int type);

void recv12(int nx, int ny, int nz, double *src, double *dest, int type);

void recv13(int nx, int ny, int nz, double *src, double *dest, int type);

void recv14(int nx, int ny, int nz, double *src, double *dest, int type);

void recv15(int nx, int ny, int nz, double *src, double *dest, int type);

void recv16(int nx, int ny, int nz, double *src, double *dest, int type);

void recv17(int nx, int ny, int nz, double *src, double *dest, int type);

void recv18(int nx, int ny, int nz, double *src, double *dest, int type);

void recv19(int nx, int ny, int nz, double *src, double *dest, int type);

void recv20(int nx, int ny, int nz, double *src, double *dest, int type);

void recv21(int nx, int ny, int nz, double *src, double *dest, int type);

void recv22(int nx, int ny, int nz, double *src, double *dest, int type);

void recv23(int nx, int ny, int nz, double *src, double *dest, int type);

void recv24(int nx, int ny, int nz, double *src, double *dest, int type);

void recv25(int nx, int ny, int nz, double *src, double *dest, int type);

void recv26(int nx, int ny, int nz, double *src, double *dest, int type);


#endif

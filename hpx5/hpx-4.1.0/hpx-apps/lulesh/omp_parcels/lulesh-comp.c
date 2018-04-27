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

#include "lulesh-hpx.h"

#define OMP_HPX 1

typedef struct {
  double min;
  int idx;
} reduce_buf_t;

static void _init_handler(reduce_buf_t *id, const size_t bytes) {
  id->min = 1.0e+20;
  id->idx = -1;
}

static HPX_ACTION(HPX_FUNCTION, 0, _init, _init_handler);

static void _min_handler(reduce_buf_t *lhs, const reduce_buf_t *rhs, size_t UNUSED) {
  if (rhs->min < lhs->min) {
      lhs->min = rhs->min;
      lhs->idx = rhs->idx;
  }
}
static HPX_ACTION(HPX_FUNCTION, 0, _min, _min_handler);


#ifdef OMP_HPX
struct CalcForceForNodes_helper_args1 {
  double *fx;
  double *fy;
  double *fz;
};

static int CalcForceForNodes_par1(int i, void *args) {
  struct CalcForceForNodes_helper_args1 *args1 =
                         (struct CalcForceForNodes_helper_args1 *)args;

  args1->fx[i] = 0.0;
  args1->fy[i] = 0.0;
  args1->fz[i] = 0.0;

  return 0;
}
#endif

void CalcForceForNodes(hpx_addr_t local,Domain *domain,int rank,unsigned long epoch)
{
  int numNode = domain->numNode;

#ifdef OMP_HPX
  struct CalcForceForNodes_helper_args1 args1;
  args1.fx = domain->fx;
  args1.fy = domain->fy;
  args1.fz = domain->fz;
  hpx_par_for_sync(CalcForceForNodes_par1,0,numNode,&args1);
#else
  int i;
  for (i = 0; i < numNode; i++) {
    domain->fx[i] = 0.0;
    domain->fy[i] = 0.0;
    domain->fz[i] = 0.0;
  }
#endif

  CalcVolumeForceForElems(domain,rank);

  SBN3(local,domain,epoch);
}

#ifdef OMP_HPX
struct CalcVolumeForceForElems_helper_args1 {
  double *determ;
  int rank;
  int cycle;
};

static int CalcVolumeForceForElems_par1(int k, void *args) {
  struct CalcVolumeForceForElems_helper_args1 *args1 =
                         (struct CalcVolumeForceForElems_helper_args1 *)args;

  if (args1->determ[k] <= 0.0) {
    // need an abort here, temporarily using exit function
    printf("rank %d: CalcVolumeForceForElems exit(-1) at cycle %d\n",
       args1->rank, args1->cycle);
    exit(-1);
  }

  return 0;
}
#endif

void CalcVolumeForceForElems(Domain *domain,int rank)
{
  int numElem = domain->numElem;

  if (numElem != 0) {
    double hgcoef = domain->hgcoef;
    double *sigxx = malloc(sizeof(double)*numElem);
    double *sigyy = malloc(sizeof(double)*numElem);
    double *sigzz = malloc(sizeof(double)*numElem);
    double *determ = malloc(sizeof(double)*numElem);

    InitStressTermsForElems(domain->p, domain->q, sigxx, sigyy, sigzz, numElem);

    IntegrateStressForElems(domain->nodelist, domain->x, domain->y, domain->z,
                domain->fx, domain->fy, domain->fz,
                domain->nodeElemCount,
                domain->nodeElemStart,
                domain->nodeElemCornerList,
                sigxx, sigyy, sigzz,
                determ, numElem,domain->numNode);
#ifdef OMP_HPX
    struct CalcVolumeForceForElems_helper_args1 args1;
    args1.determ = determ;
    args1.rank = rank;
    args1.cycle = domain->cycle;
    hpx_par_for_sync(CalcVolumeForceForElems_par1,0,numElem,&args1);
#else
    int k;
    for (k = 0; k < numElem; k++) {
      if (determ[k] <= 0.0) {
        // need an abort here, temporarily using exit function
        printf("rank %d: CalcVolumeForceForElems exit(-1) at cycle %d\n",
           rank, domain->cycle);
        exit(-1);
      }
    }
#endif

    CalcHourglassControlForElems(domain, determ, hgcoef);

    free(sigxx);
    free(sigyy);
    free(sigzz);
    free(determ);
  }
}

#ifdef OMP_HPX
struct InitStressTermsForElems_helper_args1 {
  double *p;
  double *q;
  double *sigxx;
  double *sigyy;
  double *sigzz;
};

static int InitStressTermsForElems_par1(int i, void *args) {
  struct InitStressTermsForElems_helper_args1 *args1 =
                         (struct InitStressTermsForElems_helper_args1 *)args;

  args1->sigxx[i] = args1->sigyy[i] = args1->sigzz[i] = -args1->p[i] - args1->q[i];

  return 0;
}

#endif

void InitStressTermsForElems(double *p, double *q, double *sigxx, double *sigyy,
                 double *sigzz, int numElem)
{
#ifdef OMP_HPX
  struct InitStressTermsForElems_helper_args1 args1;
  args1.sigxx = sigxx;
  args1.sigyy = sigyy;
  args1.sigzz = sigzz;
  args1.p = p;
  args1.q = q;
  hpx_par_for_sync(InitStressTermsForElems_par1,0,numElem,&args1);
#else
  int i;
  for (i = 0; i < numElem; i++) {
    sigxx[i] = sigyy[i] = sigzz[i] = -p[i] - q[i];
  }
#endif
}

#ifdef OMP_HPX
struct IntegrateStressForElems_helper_args1 {
  int *nodelist;
  double *x;
  double *y;
  double *z;
  double *sigxx;
  double *sigyy;
  double *sigzz;
  double *determ;
  double *fx_elem;
  double *fy_elem;
  double *fz_elem;
};

struct IntegrateStressForElems_helper_args2 {
  int *nodeElemCount;
  int *nodeElemStart;
  int *nodeElemCornerList;
  double *fx;
  double *fy;
  double *fz;
  double *fx_elem;
  double *fy_elem;
  double *fz_elem;
};

static int IntegrateStressForElems_par1(int k, void *args) {
  struct IntegrateStressForElems_helper_args1 *args1 =
                         (struct IntegrateStressForElems_helper_args1 *)args;

  double B[3][8];
  double x_local[8];
  double y_local[8];
  double z_local[8];

  const int * const elemNodes = &(args1->nodelist[8*k]);

  int lnode;
  for (lnode = 0; lnode < 8; lnode++) {
    int gnode = elemNodes[lnode];
    x_local[lnode] = args1->x[gnode];
    y_local[lnode] = args1->y[gnode];
    z_local[lnode] = args1->z[gnode];
  }

  CalcElemShapeFunctionDerivatives(x_local, y_local, z_local, B, &(args1->determ[k]));

  CalcElemNodeNormals(B[0], B[1], B[2], x_local, y_local, z_local);

  SumElemStressesToNodeForces(B, args1->sigxx[k], args1->sigyy[k], args1->sigzz[k],
              &(args1->fx_elem[k*8]), &(args1->fy_elem[k*8]), &(args1->fz_elem[k*8]));

  return 0;
}

static int IntegrateStressForElems_par2(int gnode, void *args) {
  struct IntegrateStressForElems_helper_args2 *args1 =
                         (struct IntegrateStressForElems_helper_args2 *)args;
  int count = args1->nodeElemCount[gnode] ;
  int start = args1->nodeElemStart[gnode] ;
  double fx_tmp = 0.0;
  double fy_tmp = 0.0 ;
  double fz_tmp = 0.0 ;
  int i;
  for (i=0 ; i < count ; ++i) {
    int elem = args1->nodeElemCornerList[start+i] ;
    fx_tmp += args1->fx_elem[elem] ;
    fy_tmp += args1->fy_elem[elem] ;
    fz_tmp += args1->fz_elem[elem] ;
  }
  args1->fx[gnode] = fx_tmp ;
  args1->fy[gnode] = fy_tmp ;
  args1->fz[gnode] = fz_tmp ;

  return 0;
}
#endif

void IntegrateStressForElems(int *nodelist, double *x, double *y, double *z,
                 double *fx, double *fy, double *fz,
                 int *nodeElemCount,
                 int *nodeElemStart,
                 int *nodeElemCornerList,
                 double *sigxx, double *sigyy, double *sigzz,
                 double *determ, int numElem,int numNode)
{

#ifdef OMP_HPX
  int numElem8 = numElem * 8;
  double *fx_elem;
  double *fy_elem;
  double *fz_elem;
  fx_elem = calloc(1, sizeof(double)*numElem8);
  fy_elem = calloc(1, sizeof(double)*numElem8);
  fz_elem = calloc(1, sizeof(double)*numElem8);

  struct IntegrateStressForElems_helper_args1 args1;
  args1.nodelist = nodelist;
  args1.x = x;
  args1.y = y;
  args1.z = z;
  args1.sigxx = sigxx;
  args1.sigyy = sigyy;
  args1.sigzz = sigzz;
  args1.determ = determ;
  args1.fx_elem = fx_elem;
  args1.fy_elem = fy_elem;
  args1.fz_elem = fz_elem;
  hpx_par_for_sync(IntegrateStressForElems_par1,0,numElem,&args1);

  struct IntegrateStressForElems_helper_args2 args2;
  args2.nodeElemCount = nodeElemCount;
  args2.nodeElemStart = nodeElemStart;
  args2.nodeElemCornerList = nodeElemCornerList;
  args2.fx = fx;
  args2.fy = fy;
  args2.fz = fz;
  args2.fx_elem = fx_elem;
  args2.fy_elem = fy_elem;
  args2.fz_elem = fz_elem;
  hpx_par_for_sync(IntegrateStressForElems_par2,0,numNode,&args2);

  free(fx_elem);
  free(fy_elem);
  free(fz_elem);
#else
  double B[3][8];
  double x_local[8];
  double y_local[8];
  double z_local[8];
  double fx_local[8];
  double fy_local[8];
  double fz_local[8];
  int k;
  for (k = 0; k < numElem; k++) {
    const int * const elemNodes = &nodelist[8*k];

    int lnode;
    for (lnode = 0; lnode < 8; lnode++) {
      int gnode = elemNodes[lnode];
      x_local[lnode] = x[gnode];
      y_local[lnode] = y[gnode];
      z_local[lnode] = z[gnode];
    }

    CalcElemShapeFunctionDerivatives(x_local, y_local, z_local, B, &determ[k]);

    CalcElemNodeNormals(B[0], B[1], B[2], x_local, y_local, z_local);

    SumElemStressesToNodeForces(B, sigxx[k], sigyy[k], sigzz[k],
                fx_local, fy_local, fz_local);

    for (lnode = 0; lnode < 8; lnode++) {
      int gnode = elemNodes[lnode];
      fx[gnode] += fx_local[lnode];
      fy[gnode] += fy_local[lnode];
      fz[gnode] += fz_local[lnode];
    }
  }
#endif
}

#ifdef OMP_HPX
struct CalcHourglassControlForElems_helper_args1 {
   double *x;
   double *y;
   double *z;
   int *nodelist;
   double *dvdx;
   double *dvdy;
   double *dvdz;
   double *x8n;
   double *y8n;
   double *z8n;
   double *determ;
   double *volo;
   double *v;
   int rank;
   int cycle;
};

static int CalcHourglassControlForElems_par1(int i, void *args) {
  struct CalcHourglassControlForElems_helper_args1 *args1 =
                         (struct CalcHourglassControlForElems_helper_args1 *)args;

  int ii,jj;
  double x1[8], y1[8], z1[8];
  double pfx[8], pfy[8], pfz[8];

  int *elemToNode = &(args1->nodelist[8*i]);

  CollectDomainNodesToElemNodes(args1->x, args1->y, args1->z, elemToNode, x1, y1, z1);

  CalcElemVolumeDerivative(pfx, pfy, pfz, x1, y1, z1);

  for (ii = 0; ii < 8; ii++) {
    jj = 8*i + ii;

    args1->dvdx[jj] = pfx[ii];
    args1->dvdy[jj] = pfy[ii];
    args1->dvdz[jj] = pfz[ii];
    args1->x8n[jj] = x1[ii];
    args1->y8n[jj] = y1[ii];
    args1->z8n[jj] = z1[ii];
  }

  args1->determ[i] = args1->volo[i]*args1->v[i];

  if (args1->v[i] <= 0.0) {
    // need an abort function here, temporarily using exit function
    printf("rank %d: CalcHourglassControlForElems exit(-1) at cycle %d\n",
       args1->rank, args1->cycle);
    exit(-1);
  }

  return 0;
}
#endif

void CalcHourglassControlForElems(Domain *domain, double determ[], double hgcoef)
{
  int numElem = domain->numElem;
  int numElem8 = numElem*8;
  double *dvdx = malloc(sizeof(double)*numElem8);
  double *dvdy = malloc(sizeof(double)*numElem8);
  double *dvdz = malloc(sizeof(double)*numElem8);
  double *x8n = malloc(sizeof(double)*numElem8);
  double *y8n = malloc(sizeof(double)*numElem8);
  double *z8n = malloc(sizeof(double)*numElem8);

#ifdef OMP_HPX
  struct CalcHourglassControlForElems_helper_args1 args1;
  args1.x = domain->x;
  args1.y = domain->y;
  args1.z = domain->z;
  args1.nodelist = domain->nodelist;
  args1.dvdx = dvdx;
  args1.dvdy = dvdy;
  args1.dvdz = dvdz;
  args1.x8n = x8n;
  args1.y8n = y8n;
  args1.z8n = z8n;
  args1.determ = determ;
  args1.volo = domain->volo;
  args1.v = domain->v;
  args1.rank = domain->rank;
  args1.cycle = domain->cycle;
  hpx_par_for_sync(CalcHourglassControlForElems_par1,0,numElem,&args1);
#else
  int i, ii, jj;
  double x1[8], y1[8], z1[8];
  double pfx[8], pfy[8], pfz[8];
  for (i = 0; i < numElem; i++) {
    int *elemToNode = &domain->nodelist[8*i];

    CollectDomainNodesToElemNodes(domain->x, domain->y, domain->z, elemToNode, x1, y1, z1);

    CalcElemVolumeDerivative(pfx, pfy, pfz, x1, y1, z1);

    for (ii = 0; ii < 8; ii++) {
      jj = 8*i + ii;

      dvdx[jj] = pfx[ii];
      dvdy[jj] = pfy[ii];
      dvdz[jj] = pfz[ii];
      x8n[jj] = x1[ii];
      y8n[jj] = y1[ii];
      z8n[jj] = z1[ii];
    }

    determ[i] = domain->volo[i]*domain->v[i];

    if (domain->v[i] <= 0.0) {
      // need an abort function here, temporarily using exit function
      printf("rank %d: CalcHourglassControlForElems exit(-1) at cycle %d\n",
         domain->rank, domain->cycle);
      exit(-1);
    }
  }
#endif

  if (hgcoef > 0.0) {
    CalcFBHourglassForceForElems(domain->nodelist, domain->ss, domain->elemMass,
                 domain->xd, domain->yd, domain->zd,
                 domain->fx, domain->fy, domain->fz,
                 domain->nodeElemCount,
                 domain->nodeElemStart,
                 domain->nodeElemCornerList,
                 determ, x8n, y8n, z8n, dvdx, dvdy, dvdz,
                 hgcoef, numElem,domain->numNode);
  }

  free(z8n);
  free(y8n);
  free(x8n);
  free(dvdz);
  free(dvdy);
  free(dvdx);
}

void CollectDomainNodesToElemNodes(double *x, double *y, double *z, const int *elemToNode,
                   double elemX[8], double elemY[8], double elemZ[8])
{
   int nd0i = elemToNode[0];
   int nd1i = elemToNode[1];
   int nd2i = elemToNode[2];
   int nd3i = elemToNode[3];
   int nd4i = elemToNode[4];
   int nd5i = elemToNode[5];
   int nd6i = elemToNode[6];
   int nd7i = elemToNode[7];

   elemX[0] = x[nd0i];
   elemX[1] = x[nd1i];
   elemX[2] = x[nd2i];
   elemX[3] = x[nd3i];
   elemX[4] = x[nd4i];
   elemX[5] = x[nd5i];
   elemX[6] = x[nd6i];
   elemX[7] = x[nd7i];

   elemY[0] = y[nd0i];
   elemY[1] = y[nd1i];
   elemY[2] = y[nd2i];
   elemY[3] = y[nd3i];
   elemY[4] = y[nd4i];
   elemY[5] = y[nd5i];
   elemY[6] = y[nd6i];
   elemY[7] = y[nd7i];

   elemZ[0] = z[nd0i];
   elemZ[1] = z[nd1i];
   elemZ[2] = z[nd2i];
   elemZ[3] = z[nd3i];
   elemZ[4] = z[nd4i];
   elemZ[5] = z[nd5i];
   elemZ[6] = z[nd6i];
   elemZ[7] = z[nd7i];
}

void CalcElemVolumeDerivative(double dvdx[8], double dvdy[8], double dvdz[8],
                  const double x[8], const double y[8], const double z[8])
{
  VoluDer(x[1], x[2], x[3], x[4], x[5], x[7],
      y[1], y[2], y[3], y[4], y[5], y[7],
      z[1], z[2], z[3], z[4], z[5], z[7],
      &dvdx[0], &dvdy[0], &dvdz[0]);

  VoluDer(x[0], x[1], x[2], x[7], x[4], x[6],
      y[0], y[1], y[2], y[7], y[4], y[6],
      z[0], z[1], z[2], z[7], z[4], z[6],
      &dvdx[3], &dvdy[3], &dvdz[3]);

  VoluDer(x[3], x[0], x[1], x[6], x[7], x[5],
      y[3], y[0], y[1], y[6], y[7], y[5],
      z[3], z[0], z[1], z[6], z[7], z[5],
      &dvdx[2], &dvdy[2], &dvdz[2]);

  VoluDer(x[2], x[3], x[0], x[5], x[6], x[4],
      y[2], y[3], y[0], y[5], y[6], y[4],
      z[2], z[3], z[0], z[5], z[6], z[4],
      &dvdx[1], &dvdy[1], &dvdz[1]);

  VoluDer(x[7], x[6], x[5], x[0], x[3], x[1],
      y[7], y[6], y[5], y[0], y[3], y[1],
      z[7], z[6], z[5], z[0], z[3], z[1],
      &dvdx[4], &dvdy[4], &dvdz[4]);

  VoluDer(x[4], x[7], x[6], x[1], x[0], x[2],
      y[4], y[7], y[6], y[1], y[0], y[2],
      z[4], z[7], z[6], z[1], z[0], z[2],
      &dvdx[5], &dvdy[5], &dvdz[5]);

  VoluDer(x[5], x[4], x[7], x[2], x[1], x[3],
      y[5], y[4], y[7], y[2], y[1], y[3],
      z[5], z[4], z[7], z[2], z[1], z[3],
      &dvdx[6], &dvdy[6], &dvdz[6]);

  VoluDer(x[6], x[5], x[4], x[3], x[2], x[0],
      y[6], y[5], y[4], y[3], y[2], y[0],
      z[6], z[5], z[4], z[3], z[2], z[0],
      &dvdx[7], &dvdy[7], &dvdz[7]);
}

void VoluDer(const double x0, const double x1, const double x2,
         const double x3, const double x4, const double x5,
         const double y0, const double y1, const double y2,
         const double y3, const double y4, const double y5,
         const double z0, const double z1, const double z2,
         const double z3, const double z4, const double z5,
         double *dvdx, double *dvdy, double *dvdz)
{
  const double twelfth = 1.0/12.0;

  *dvdx =
    (y1 + y2)*(z0 + z1) - (y0 + y1)*(z1 + z2) +
    (y0 + y4)*(z3 + z4) - (y3 + y4)*(z0 + z4) -
    (y2 + y5)*(z3 + z5) + (y3 + y5)*(z2 + z5);

  *dvdy =
    -(x1 + x2)*(z0 + z1) + (x0 + x1)*(z1 + z2) -
    (x0 + x4)*(z3 + z4) + (x3 + x4)*(z0 + z4) +
    (x2 + x5)*(z3 + z5) - (x3 + x5)*(z2 + z5);

  *dvdz =
    -(y1 + y2)*(x0 + x1) + (y0 + y1)*(x1 + x2) -
    (y0 + y4)*(x3 + x4) + (y3 + y4)*(x0 + x4) +
    (y2 + y5)*(x3 + x5) - (y3 + y5)*(x2 + x5);

  *dvdx *= twelfth;
  *dvdy *= twelfth;
  *dvdz *= twelfth;
}

#ifdef OMP_HPX
struct CalcFBHourglassForceForElems_helper_args1 {
  int *nodelist;
  double *ss;
  double *elemMass;
  double *xd;
  double *yd;
  double *zd;
  int *nodeElemCount;
  int *nodeElemStart;
  int *nodeElemCornerList;
  double *determ;
  double *x8n;
  double *y8n;
  double *z8n;
  double *dvdx;
  double *dvdy;
  double *dvdz;
  double hourg;
  double *fx_elem;
  double *fy_elem;
  double *fz_elem;
};

struct CalcFBHourglassForceForElems_helper_args2 {
  int *nodeElemCount;
  int *nodeElemStart;
  int *nodeElemCornerList;
  double *fx_elem;
  double *fy_elem;
  double *fz_elem;
  double *fx;
  double *fy;
  double *fz;
};

static int CalcFBHourglassForceForElems_par1(int i2, void *args) {
  struct CalcFBHourglassForceForElems_helper_args1 *args1 =
                         (struct CalcFBHourglassForceForElems_helper_args1 *)args;

  double gamma[4][8];

  gamma[0][0] = 1.0;
  gamma[0][1] = 1.0;
  gamma[0][2] = -1.0;
  gamma[0][3] = -1.0;
  gamma[0][4] = -1.0;
  gamma[0][5] = -1.0;
  gamma[0][6] = 1.0;
  gamma[0][7] = 1.0;
  gamma[1][0] = 1.0;
  gamma[1][1] = -1.0;
  gamma[1][2] = -1.0;
  gamma[1][3] = 1.0;
  gamma[1][4] = -1.0;
  gamma[1][5] = 1.0;
  gamma[1][6] = 1.0;
  gamma[1][7] = -1.0;
  gamma[2][0] = 1.0;
  gamma[2][1] = -1.0;
  gamma[2][2] = 1.0;
  gamma[2][3] = -1.0;
  gamma[2][4] = 1.0;
  gamma[2][5] = -1.0;
  gamma[2][6] = 1.0;
  gamma[2][7] = -1.0;
  gamma[3][0] = -1.0;
  gamma[3][1] = 1.0;
  gamma[3][2] = -1.0;
  gamma[3][3] = 1.0;
  gamma[3][4] = 1.0;
  gamma[3][5] = -1.0;
  gamma[3][6] = 1.0;
  gamma[3][7] = -1.0;

  double *fx_local, *fy_local, *fz_local ;
  double hgfx[8], hgfy[8], hgfz[8] ;

  double coefficient;

  double hourgam0[4], hourgam1[4], hourgam2[4], hourgam3[4] ;
  double hourgam4[4], hourgam5[4], hourgam6[4], hourgam7[4];
  double xd1[8], yd1[8], zd1[8] ;

  const int *elemToNode = &args1->nodelist[8*i2];
  int i3=8*i2;
  double volinv= 1.0/args1->determ[i2];
  double ss1, mass1, volume13 ;
  int i1;
  for(i1=0;i1<4;++i1){
     double hourmodx =
        args1->x8n[i3] * gamma[i1][0] + args1->x8n[i3+1] * gamma[i1][1] +
        args1->x8n[i3+2] * gamma[i1][2] + args1->x8n[i3+3] * gamma[i1][3] +
        args1->x8n[i3+4] * gamma[i1][4] + args1->x8n[i3+5] * gamma[i1][5] +
        args1->x8n[i3+6] * gamma[i1][6] + args1->x8n[i3+7] * gamma[i1][7];

     double hourmody =
        args1->y8n[i3] * gamma[i1][0] + args1->y8n[i3+1] * gamma[i1][1] +
        args1->y8n[i3+2] * gamma[i1][2] + args1->y8n[i3+3] * gamma[i1][3] +
        args1->y8n[i3+4] * gamma[i1][4] + args1->y8n[i3+5] * gamma[i1][5] +
        args1->y8n[i3+6] * gamma[i1][6] + args1->y8n[i3+7] * gamma[i1][7];

     double hourmodz =
        args1->z8n[i3] * gamma[i1][0] + args1->z8n[i3+1] * gamma[i1][1] +
        args1->z8n[i3+2] * gamma[i1][2] + args1->z8n[i3+3] * gamma[i1][3] +
        args1->z8n[i3+4] * gamma[i1][4] + args1->z8n[i3+5] * gamma[i1][5] +
        args1->z8n[i3+6] * gamma[i1][6] + args1->z8n[i3+7] * gamma[i1][7];

     hourgam0[i1] = gamma[i1][0] -  volinv*(args1->dvdx[i3  ] * hourmodx +
                                              args1->dvdy[i3  ] * hourmody +
                                              args1->dvdz[i3  ] * hourmodz );

     hourgam1[i1] = gamma[i1][1] -  volinv*(args1->dvdx[i3+1] * hourmodx +
                                              args1->dvdy[i3+1] * hourmody +
                                              args1->dvdz[i3+1] * hourmodz );

     hourgam2[i1] = gamma[i1][2] -  volinv*(args1->dvdx[i3+2] * hourmodx +
                                              args1->dvdy[i3+2] * hourmody +
                                              args1->dvdz[i3+2] * hourmodz );

     hourgam3[i1] = gamma[i1][3] -  volinv*(args1->dvdx[i3+3] * hourmodx +
                                              args1->dvdy[i3+3] * hourmody +
                                              args1->dvdz[i3+3] * hourmodz );

     hourgam4[i1] = gamma[i1][4] -  volinv*(args1->dvdx[i3+4] * hourmodx +
                                              args1->dvdy[i3+4] * hourmody +
                                              args1->dvdz[i3+4] * hourmodz );

     hourgam5[i1] = gamma[i1][5] -  volinv*(args1->dvdx[i3+5] * hourmodx +
                                              args1->dvdy[i3+5] * hourmody +
                                              args1->dvdz[i3+5] * hourmodz );

     hourgam6[i1] = gamma[i1][6] -  volinv*(args1->dvdx[i3+6] * hourmodx +
                                              args1->dvdy[i3+6] * hourmody +
                                              args1->dvdz[i3+6] * hourmodz );

     hourgam7[i1] = gamma[i1][7] -  volinv*(args1->dvdx[i3+7] * hourmodx +
                                              args1->dvdy[i3+7] * hourmody +
                                              args1->dvdz[i3+7] * hourmodz );

  }
       /* compute forces */
  /* store forces into h arrays (force arrays) */

  ss1=args1->ss[i2];
  mass1=args1->elemMass[i2];
  volume13=cbrt(args1->determ[i2]);

  int n0si2 = elemToNode[0];
  int n1si2 = elemToNode[1];
  int n2si2 = elemToNode[2];
  int n3si2 = elemToNode[3];
  int n4si2 = elemToNode[4];
  int n5si2 = elemToNode[5];
  int n6si2 = elemToNode[6];
  int n7si2 = elemToNode[7];

  xd1[0] = args1->xd[n0si2];
  xd1[1] = args1->xd[n1si2];
  xd1[2] = args1->xd[n2si2];
  xd1[3] = args1->xd[n3si2];
  xd1[4] = args1->xd[n4si2];
  xd1[5] = args1->xd[n5si2];
  xd1[6] = args1->xd[n6si2];
  xd1[7] = args1->xd[n7si2];

  yd1[0] = args1->yd[n0si2];
  yd1[1] = args1->yd[n1si2];
  yd1[2] = args1->yd[n2si2];
  yd1[3] = args1->yd[n3si2];
  yd1[4] = args1->yd[n4si2];
  yd1[5] = args1->yd[n5si2];
  yd1[6] = args1->yd[n6si2];
  yd1[7] = args1->yd[n7si2];

  zd1[0] = args1->zd[n0si2];
  zd1[1] = args1->zd[n1si2];
  zd1[2] = args1->zd[n2si2];
  zd1[3] = args1->zd[n3si2];
  zd1[4] = args1->zd[n4si2];
  zd1[5] = args1->zd[n5si2];
  zd1[6] = args1->zd[n6si2];
  zd1[7] = args1->zd[n7si2];

  coefficient = - args1->hourg * 0.01 * ss1 * mass1 / volume13;

  CalcElemFBHourglassForce(xd1,yd1,zd1,
                  hourgam0,hourgam1,hourgam2,hourgam3,
                  hourgam4,hourgam5,hourgam6,hourgam7,
                  coefficient, hgfx, hgfy, hgfz);

  fx_local = &args1->fx_elem[i3] ;
  fx_local[0] = hgfx[0];
  fx_local[1] = hgfx[1];
  fx_local[2] = hgfx[2];
  fx_local[3] = hgfx[3];
  fx_local[4] = hgfx[4];
  fx_local[5] = hgfx[5];
  fx_local[6] = hgfx[6];
  fx_local[7] = hgfx[7];

  fy_local = &args1->fy_elem[i3] ;
  fy_local[0] = hgfy[0];
  fy_local[1] = hgfy[1];
  fy_local[2] = hgfy[2];
  fy_local[3] = hgfy[3];
  fy_local[4] = hgfy[4];
  fy_local[5] = hgfy[5];
  fy_local[6] = hgfy[6];
  fy_local[7] = hgfy[7];

  fz_local = &args1->fz_elem[i3] ;
  fz_local[0] = hgfz[0];
  fz_local[1] = hgfz[1];
  fz_local[2] = hgfz[2];
  fz_local[3] = hgfz[3];
  fz_local[4] = hgfz[4];
  fz_local[5] = hgfz[5];
  fz_local[6] = hgfz[6];
  fz_local[7] = hgfz[7];

  return 0;
}

static int CalcFBHourglassForceForElems_par2(int gnode, void *args) {
  struct CalcFBHourglassForceForElems_helper_args2 *args1 =
                         (struct CalcFBHourglassForceForElems_helper_args2 *)args;

  int count = args1->nodeElemCount[gnode] ;
  int start = args1->nodeElemStart[gnode] ;
  double fx_tmp = 0.0 ;
  double fy_tmp = 0.0 ;
  double fz_tmp = 0.0 ;
  int i;
  for (i=0 ; i < count ; ++i) {
    int elem = args1->nodeElemCornerList[start+i] ;
    fx_tmp += args1->fx_elem[elem] ;
    fy_tmp += args1->fy_elem[elem] ;
    fz_tmp += args1->fz_elem[elem] ;
  }
  args1->fx[gnode] += fx_tmp ;
  args1->fy[gnode] += fy_tmp ;
  args1->fz[gnode] += fz_tmp ;

  return 0;
}
#endif

void CalcFBHourglassForceForElems(int *nodelist, double *ss, double *elemMass,
                  double *xd, double *yd, double *zd,
                  double *fx, double *fy, double *fz,
                  int *nodeElemCount,
                  int *nodeElemStart,
                  int *nodeElemCornerList,
                  double *determ,
                  double *x8n, double *y8n, double *z8n,
                  double *dvdx, double *dvdy, double *dvdz,
                  double hourg, int numElem,int numNode)
{
#ifdef OMP_HPX
  int numElem8 = numElem * 8;
  double *fx_elem;
  double *fy_elem;
  double *fz_elem;
  fx_elem = calloc(1, sizeof(double)*numElem8);
  fy_elem = calloc(1, sizeof(double)*numElem8);
  fz_elem = calloc(1, sizeof(double)*numElem8);

  struct CalcFBHourglassForceForElems_helper_args1 args1;
  args1.nodelist = nodelist;
  args1.ss = ss;
  args1.elemMass = elemMass;
  args1.xd = xd;
  args1.yd = yd;
  args1.zd = zd;
  args1.nodeElemCount = nodeElemCount;
  args1.nodeElemStart = nodeElemStart;
  args1.nodeElemCornerList = nodeElemCornerList;
  args1.determ = determ;
  args1.x8n = x8n;
  args1.y8n = y8n;
  args1.z8n = z8n;
  args1.dvdx = dvdx;
  args1.dvdy = dvdy;
  args1.dvdz = dvdz;
  args1.hourg = hourg;
  args1.fx_elem = fx_elem;
  args1.fy_elem = fy_elem;
  args1.fz_elem = fz_elem;
  hpx_par_for_sync(CalcFBHourglassForceForElems_par1,0,numElem,&args1);

  struct CalcFBHourglassForceForElems_helper_args2 args2;
  args2.nodeElemCount = nodeElemCount;
  args2.nodeElemStart = nodeElemStart;
  args2.nodeElemCornerList = nodeElemCornerList;
  args2.fx_elem = fx_elem;
  args2.fy_elem = fy_elem;
  args2.fz_elem = fz_elem;
  args2.fx = fx;
  args2.fy = fy;
  args2.fz = fz;
  hpx_par_for_sync(CalcFBHourglassForceForElems_par2,0,numNode,&args2);

  free(fx_elem);
  free(fy_elem);
  free(fz_elem);
#else
  double hgfx[8], hgfy[8], hgfz[8];
  double coefficient;
  double gamma[4][8];

  gamma[0][0] = 1.0;
  gamma[0][1] = 1.0;
  gamma[0][2] = -1.0;
  gamma[0][3] = -1.0;
  gamma[0][4] = -1.0;
  gamma[0][5] = -1.0;
  gamma[0][6] = 1.0;
  gamma[0][7] = 1.0;
  gamma[1][0] = 1.0;
  gamma[1][1] = -1.0;
  gamma[1][2] = -1.0;
  gamma[1][3] = 1.0;
  gamma[1][4] = -1.0;
  gamma[1][5] = 1.0;
  gamma[1][6] = 1.0;
  gamma[1][7] = -1.0;
  gamma[2][0] = 1.0;
  gamma[2][1] = -1.0;
  gamma[2][2] = 1.0;
  gamma[2][3] = -1.0;
  gamma[2][4] = 1.0;
  gamma[2][5] = -1.0;
  gamma[2][6] = 1.0;
  gamma[2][7] = -1.0;
  gamma[3][0] = -1.0;
  gamma[3][1] = 1.0;
  gamma[3][2] = -1.0;
  gamma[3][3] = 1.0;
  gamma[3][4] = 1.0;
  gamma[3][5] = -1.0;
  gamma[3][6] = 1.0;
  gamma[3][7] = -1.0;
  double hourgam0[4], hourgam1[4], hourgam2[4], hourgam3[4];
  double hourgam4[4], hourgam5[4], hourgam6[4], hourgam7[4];
  double xd1[8], yd1[8], zd1[8] ;
  int i2, i1;
  for (i2 = 0; i2 < numElem; i2++) {
    const int *elemToNode = &nodelist[8*i2];
    int i3 = 8*i2;
    double volinv = 1.0/determ[i2];
    double ss1, mass1, volume13;
    for (i1 = 0; i1 < 4; i1++) {

      double hourmodx =
    x8n[i3]*gamma[i1][0] + x8n[i3 + 1]*gamma[i1][1] +
    x8n[i3 + 2]*gamma[i1][2] + x8n[i3 + 3]*gamma[i1][3] +
    x8n[i3 + 4]*gamma[i1][4] + x8n[i3 + 5]*gamma[i1][5] +
    x8n[i3 + 6]*gamma[i1][6] + x8n[i3 + 7]*gamma[i1][7];

      double hourmody =
    y8n[i3]*gamma[i1][0] + y8n[i3 + 1]*gamma[i1][1] +
    y8n[i3 + 2]*gamma[i1][2] + y8n[i3 + 3]*gamma[i1][3] +
    y8n[i3 + 4]*gamma[i1][4] + y8n[i3 + 5]*gamma[i1][5] +
    y8n[i3 + 6]*gamma[i1][6] + y8n[i3 + 7]*gamma[i1][7];


      double hourmodz =
    z8n[i3]*gamma[i1][0] + z8n[i3 + 1]*gamma[i1][1] +
    z8n[i3 + 2]*gamma[i1][2] + z8n[i3 + 3]*gamma[i1][3] +
    z8n[i3 + 4]*gamma[i1][4] + z8n[i3 + 5]*gamma[i1][5] +
    z8n[i3 + 6]*gamma[i1][6] + z8n[i3 + 7]*gamma[i1][7];


      hourgam0[i1] = gamma[i1][0] -
    volinv*(dvdx[i3]*hourmodx  + dvdy[i3]*hourmody + dvdz[i3]*hourmodz);

      hourgam1[i1] = gamma[i1][1] -
    volinv*(dvdx[i3 + 1]*hourmodx  + dvdy[i3 + 1]*hourmody + dvdz[i3 + 1]*hourmodz);

      hourgam2[i1] = gamma[i1][2] -
    volinv*(dvdx[i3 + 2]*hourmodx + dvdy[i3 + 2]*hourmody + dvdz[i3 + 2]*hourmodz);

      hourgam3[i1] = gamma[i1][3] -
    volinv*(dvdx[i3 + 3]*hourmodx + dvdy[i3 + 3]*hourmody + dvdz[i3 + 3]*hourmodz);

      hourgam4[i1] = gamma[i1][4] -
    volinv*(dvdx[i3 + 4]*hourmodx + dvdy[i3 + 4]*hourmody + dvdz[i3 + 4]*hourmodz);

      hourgam5[i1] = gamma[i1][5] -
    volinv*(dvdx[i3 + 5]*hourmodx + dvdy[i3 + 5]*hourmody + dvdz[i3 + 5]*hourmodz);

      hourgam6[i1] = gamma[i1][6] -
    volinv*(dvdx[i3 + 6]*hourmodx + dvdy[i3 + 6]*hourmody + dvdz[i3 + 6]*hourmodz);

      hourgam7[i1] = gamma[i1][7] -
    volinv*(dvdx[i3 + 7]*hourmodx + dvdy[i3 + 7]*hourmody + dvdz[i3 + 7]*hourmodz);
    }


    ss1 = ss[i2];
    mass1 = elemMass[i2];
    volume13 = cbrt(determ[i2]);

    int n0si2 = elemToNode[0];
    int n1si2 = elemToNode[1];
    int n2si2 = elemToNode[2];
    int n3si2 = elemToNode[3];
    int n4si2 = elemToNode[4];
    int n5si2 = elemToNode[5];
    int n6si2 = elemToNode[6];
    int n7si2 = elemToNode[7];

    xd1[0] = xd[n0si2];
    xd1[1] = xd[n1si2];
    xd1[2] = xd[n2si2];
    xd1[3] = xd[n3si2];
    xd1[4] = xd[n4si2];
    xd1[5] = xd[n5si2];
    xd1[6] = xd[n6si2];
    xd1[7] = xd[n7si2];

    yd1[0] = yd[n0si2];
    yd1[1] = yd[n1si2];
    yd1[2] = yd[n2si2];
    yd1[3] = yd[n3si2];
    yd1[4] = yd[n4si2];
    yd1[5] = yd[n5si2];
    yd1[6] = yd[n6si2];
    yd1[7] = yd[n7si2];

    zd1[0] = zd[n0si2];
    zd1[1] = zd[n1si2];
    zd1[2] = zd[n2si2];
    zd1[3] = zd[n3si2];
    zd1[4] = zd[n4si2];
    zd1[5] = zd[n5si2];
    zd1[6] = zd[n6si2];
    zd1[7] = zd[n7si2];

    coefficient = -hourg*0.01*ss1*mass1/volume13;

    CalcElemFBHourglassForce(xd1, yd1, zd1, hourgam0, hourgam1, hourgam2, hourgam3,
                 hourgam4, hourgam5, hourgam6, hourgam7,
                 coefficient, hgfx, hgfy, hgfz);

    fx[n0si2] += hgfx[0];
    fy[n0si2] += hgfy[0];
    fz[n0si2] += hgfz[0];

    fx[n1si2] += hgfx[1];
    fy[n1si2] += hgfy[1];
    fz[n1si2] += hgfz[1];

    fx[n2si2] += hgfx[2];
    fy[n2si2] += hgfy[2];
    fz[n2si2] += hgfz[2];

    fx[n3si2] += hgfx[3];
    fy[n3si2] += hgfy[3];
    fz[n3si2] += hgfz[3];

    fx[n4si2] += hgfx[4];
    fy[n4si2] += hgfy[4];
    fz[n4si2] += hgfz[4];

    fx[n5si2] += hgfx[5];
    fy[n5si2] += hgfy[5];
    fz[n5si2] += hgfz[5];

    fx[n6si2] += hgfx[6];
    fy[n6si2] += hgfy[6];
    fz[n6si2] += hgfz[6];

    fx[n7si2] += hgfx[7];
    fy[n7si2] += hgfy[7];
    fz[n7si2] += hgfz[7];
  }
#endif
}

void CalcElemFBHourglassForce(double *xd, double *yd, double *zd, double *hourgam0,
                  double *hourgam1, double *hourgam2, double *hourgam3,
                  double *hourgam4, double *hourgam5, double *hourgam6,
                  double *hourgam7, double coefficient,
                  double *hgfx, double *hgfy, double *hgfz)
{
  int i00 = 0;
  int i01 = 1;
  int i02 = 2;
  int i03 = 3;

  double h00 =
    hourgam0[i00]*xd[0] + hourgam1[i00]*xd[1] +
    hourgam2[i00]*xd[2] + hourgam3[i00]*xd[3] +
    hourgam4[i00]*xd[4] + hourgam5[i00]*xd[5] +
    hourgam6[i00]*xd[6] + hourgam7[i00]*xd[7];

  double h01 =
    hourgam0[i01]*xd[0] + hourgam1[i01]*xd[1] +
    hourgam2[i01]*xd[2] + hourgam3[i01]*xd[3] +
    hourgam4[i01]*xd[4] + hourgam5[i01]*xd[5] +
    hourgam6[i01]*xd[6] + hourgam7[i01]*xd[7];

  double h02 =
    hourgam0[i02]*xd[0] + hourgam1[i02]*xd[1]+
    hourgam2[i02]*xd[2] + hourgam3[i02]*xd[3]+
    hourgam4[i02]*xd[4] + hourgam5[i02]*xd[5]+
    hourgam6[i02]*xd[6] + hourgam7[i02]*xd[7];

  double h03 =
    hourgam0[i03]*xd[0] + hourgam1[i03]*xd[1] +
    hourgam2[i03]*xd[2] + hourgam3[i03]*xd[3] +
    hourgam4[i03]*xd[4] + hourgam5[i03]*xd[5] +
    hourgam6[i03]*xd[6] + hourgam7[i03]*xd[7];

  hgfx[0] = coefficient*
    (hourgam0[i00]*h00 + hourgam0[i01]*h01 +
     hourgam0[i02]*h02 + hourgam0[i03]*h03);

  hgfx[1] = coefficient*
    (hourgam1[i00]*h00 + hourgam1[i01]*h01 +
     hourgam1[i02]*h02 + hourgam1[i03]*h03);

  hgfx[2] = coefficient*
    (hourgam2[i00]*h00 + hourgam2[i01]*h01 +
     hourgam2[i02]*h02 + hourgam2[i03]*h03);

  hgfx[3] = coefficient*
    (hourgam3[i00]*h00 + hourgam3[i01]*h01 +
     hourgam3[i02]*h02 + hourgam3[i03]*h03);

  hgfx[4] = coefficient*
    (hourgam4[i00]*h00 + hourgam4[i01]*h01 +
     hourgam4[i02]*h02 + hourgam4[i03]*h03);

  hgfx[5] = coefficient*
    (hourgam5[i00]*h00 + hourgam5[i01]*h01 +
     hourgam5[i02]*h02 + hourgam5[i03]*h03);

  hgfx[6] = coefficient*
    (hourgam6[i00]*h00 + hourgam6[i01]*h01 +
     hourgam6[i02]*h02 + hourgam6[i03]*h03);

  hgfx[7] = coefficient*
    (hourgam7[i00]*h00 + hourgam7[i01]*h01 +
     hourgam7[i02]*h02 + hourgam7[i03]*h03);

  h00 =
    hourgam0[i00]*yd[0] + hourgam1[i00]*yd[1] +
    hourgam2[i00]*yd[2] + hourgam3[i00]*yd[3] +
    hourgam4[i00]*yd[4] + hourgam5[i00]*yd[5] +
    hourgam6[i00]*yd[6] + hourgam7[i00]*yd[7];

  h01 =
    hourgam0[i01]*yd[0] + hourgam1[i01]*yd[1] +
    hourgam2[i01]*yd[2] + hourgam3[i01]*yd[3] +
    hourgam4[i01]*yd[4] + hourgam5[i01]*yd[5] +
    hourgam6[i01]*yd[6] + hourgam7[i01]*yd[7];

  h02 =
    hourgam0[i02]*yd[0] + hourgam1[i02]*yd[1]+
    hourgam2[i02]*yd[2] + hourgam3[i02]*yd[3]+
    hourgam4[i02]*yd[4] + hourgam5[i02]*yd[5]+
    hourgam6[i02]*yd[6] + hourgam7[i02]*yd[7];

  h03 =
    hourgam0[i03]*yd[0] + hourgam1[i03]*yd[1] +
    hourgam2[i03]*yd[2] + hourgam3[i03]*yd[3] +
    hourgam4[i03]*yd[4] + hourgam5[i03]*yd[5] +
    hourgam6[i03]*yd[6] + hourgam7[i03]*yd[7];


  hgfy[0] = coefficient*
    (hourgam0[i00]*h00 + hourgam0[i01]*h01 +
     hourgam0[i02]*h02 + hourgam0[i03]*h03);

  hgfy[1] = coefficient*
    (hourgam1[i00]*h00 + hourgam1[i01]*h01 +
     hourgam1[i02]*h02 + hourgam1[i03]*h03);

  hgfy[2] = coefficient*
    (hourgam2[i00]*h00 + hourgam2[i01]*h01 +
     hourgam2[i02]*h02 + hourgam2[i03]*h03);

  hgfy[3] = coefficient*
    (hourgam3[i00]*h00 + hourgam3[i01]*h01 +
     hourgam3[i02]*h02 + hourgam3[i03]*h03);

  hgfy[4] = coefficient*
    (hourgam4[i00]*h00 + hourgam4[i01]*h01 +
     hourgam4[i02]*h02 + hourgam4[i03]*h03);

  hgfy[5] = coefficient*
    (hourgam5[i00]*h00 + hourgam5[i01]*h01 +
     hourgam5[i02]*h02 + hourgam5[i03]*h03);

  hgfy[6] = coefficient*
    (hourgam6[i00]*h00 + hourgam6[i01]*h01 +
     hourgam6[i02]*h02 + hourgam6[i03]*h03);

  hgfy[7] = coefficient*
    (hourgam7[i00]*h00 + hourgam7[i01]*h01 +
     hourgam7[i02]*h02 + hourgam7[i03]*h03);

  h00 =
    hourgam0[i00]*zd[0] + hourgam1[i00]*zd[1] +
    hourgam2[i00]*zd[2] + hourgam3[i00]*zd[3] +
    hourgam4[i00]*zd[4] + hourgam5[i00]*zd[5] +
    hourgam6[i00]*zd[6] + hourgam7[i00]*zd[7];

  h01 =
    hourgam0[i01]*zd[0] + hourgam1[i01]*zd[1] +
    hourgam2[i01]*zd[2] + hourgam3[i01]*zd[3] +
    hourgam4[i01]*zd[4] + hourgam5[i01]*zd[5] +
    hourgam6[i01]*zd[6] + hourgam7[i01]*zd[7];

  h02 =
    hourgam0[i02]*zd[0] + hourgam1[i02]*zd[1]+
    hourgam2[i02]*zd[2] + hourgam3[i02]*zd[3]+
    hourgam4[i02]*zd[4] + hourgam5[i02]*zd[5]+
    hourgam6[i02]*zd[6] + hourgam7[i02]*zd[7];

  h03 =
    hourgam0[i03]*zd[0] + hourgam1[i03]*zd[1] +
    hourgam2[i03]*zd[2] + hourgam3[i03]*zd[3] +
    hourgam4[i03]*zd[4] + hourgam5[i03]*zd[5] +
    hourgam6[i03]*zd[6] + hourgam7[i03]*zd[7];


  hgfz[0] = coefficient*
    (hourgam0[i00]*h00 + hourgam0[i01]*h01 +
     hourgam0[i02]*h02 + hourgam0[i03]*h03);

  hgfz[1] = coefficient*
    (hourgam1[i00]*h00 + hourgam1[i01]*h01 +
     hourgam1[i02]*h02 + hourgam1[i03]*h03);

  hgfz[2] = coefficient*
    (hourgam2[i00]*h00 + hourgam2[i01]*h01 +
     hourgam2[i02]*h02 + hourgam2[i03]*h03);

  hgfz[3] = coefficient*
    (hourgam3[i00]*h00 + hourgam3[i01]*h01 +
     hourgam3[i02]*h02 + hourgam3[i03]*h03);

  hgfz[4] = coefficient*
    (hourgam4[i00]*h00 + hourgam4[i01]*h01 +
     hourgam4[i02]*h02 + hourgam4[i03]*h03);

  hgfz[5] = coefficient*
    (hourgam5[i00]*h00 + hourgam5[i01]*h01 +
     hourgam5[i02]*h02 + hourgam5[i03]*h03);

  hgfz[6] = coefficient*
    (hourgam6[i00]*h00 + hourgam6[i01]*h01 +
     hourgam6[i02]*h02 + hourgam6[i03]*h03);

  hgfz[7] = coefficient*
    (hourgam7[i00]*h00 + hourgam7[i01]*h01 +
     hourgam7[i02]*h02 + hourgam7[i03]*h03);
}

void CalcElemShapeFunctionDerivatives(double const x[], double const y[], double const z[],
                      double b[][8], double *const volume)
{
  const double x0 = x[0];   const double x1 = x[1];
  const double x2 = x[2];   const double x3 = x[3];
  const double x4 = x[4];   const double x5 = x[5];
  const double x6 = x[6];   const double x7 = x[7];

  const double y0 = y[0];   const double y1 = y[1];
  const double y2 = y[2];   const double y3 = y[3];
  const double y4 = y[4];   const double y5 = y[5];
  const double y6 = y[6];   const double y7 = y[7];

  const double z0 = z[0];   const double z1 = z[1];
  const double z2 = z[2];   const double z3 = z[3];
  const double z4 = z[4];   const double z5 = z[5];
  const double z6 = z[6];   const double z7 = z[7];

  double fjxxi, fjxet, fjxze;
  double fjyxi, fjyet, fjyze;
  double fjzxi, fjzet, fjzze;
  double cjxxi, cjxet, cjxze;
  double cjyxi, cjyet, cjyze;
  double cjzxi, cjzet, cjzze;

  fjxxi = .125*((x6 - x0) + (x5 - x3) - (x7 - x1) - (x4 - x2));
  fjxet = .125*((x6 - x0) - (x5 - x3) + (x7 - x1) - (x4 - x2));
  fjxze = .125*((x6 - x0) + (x5 - x3) + (x7 - x1) + (x4 - x2));

  fjyxi = .125*((y6 - y0) + (y5 - y3) - (y7 - y1) - (y4 - y2));
  fjyet = .125*((y6 - y0) - (y5 - y3) + (y7 - y1) - (y4 - y2));
  fjyze = .125*((y6 - y0) + (y5 - y3) + (y7 - y1) + (y4 - y2));

  fjzxi = .125*((z6 - z0) + (z5 - z3) - (z7 - z1) - (z4 - z2));
  fjzet = .125*((z6 - z0) - (z5 - z3) + (z7 - z1) - (z4 - z2));
  fjzze = .125*((z6 - z0) + (z5 - z3) + (z7 - z1) + (z4 - z2));

  // compute cofactors
  cjxxi =    (fjyet*fjzze) - (fjzet*fjyze);
  cjxet =  - (fjyxi*fjzze) + (fjzxi*fjyze);
  cjxze =    (fjyxi*fjzet) - (fjzxi*fjyet);

  cjyxi =  - (fjxet*fjzze) + (fjzet*fjxze);
  cjyet =    (fjxxi*fjzze) - (fjzxi*fjxze);
  cjyze =  - (fjxxi*fjzet) + (fjzxi*fjxet);

  cjzxi =    (fjxet*fjyze) - (fjyet*fjxze);
  cjzet =  - (fjxxi*fjyze) + (fjyxi*fjxze);
  cjzze =    (fjxxi*fjyet) - (fjyxi*fjxet);

  // calculate partials :
  //   this need only be done for l = 0,1,2,3   since , by symmetry ,
  //   (6,7,4,5) = - (0,1,2,3) .

  b[0][0] =   -  cjxxi  -  cjxet  -  cjxze;
  b[0][1] =      cjxxi  -  cjxet  -  cjxze;
  b[0][2] =      cjxxi  +  cjxet  -  cjxze;
  b[0][3] =   -  cjxxi  +  cjxet  -  cjxze;
  b[0][4] = -b[0][2];
  b[0][5] = -b[0][3];
  b[0][6] = -b[0][0];
  b[0][7] = -b[0][1];

  b[1][0] =   -  cjyxi  -  cjyet  -  cjyze;
  b[1][1] =      cjyxi  -  cjyet  -  cjyze;
  b[1][2] =      cjyxi  +  cjyet  -  cjyze;
  b[1][3] =   -  cjyxi  +  cjyet  -  cjyze;
  b[1][4] = -b[1][2];
  b[1][5] = -b[1][3];
  b[1][6] = -b[1][0];
  b[1][7] = -b[1][1];

  b[2][0] =   -  cjzxi  -  cjzet  -  cjzze;
  b[2][1] =      cjzxi  -  cjzet  -  cjzze;
  b[2][2] =      cjzxi  +  cjzet  -  cjzze;
  b[2][3] =   -  cjzxi  +  cjzet  -  cjzze;
  b[2][4] = -b[2][2];
  b[2][5] = -b[2][3];
  b[2][6] = -b[2][0];
  b[2][7] = -b[2][1];

  // calculate jacobian determinant (volume)
  *volume = 8.0*(fjxet*cjxet + fjyet*cjyet + fjzet*cjzet);
}

void CalcElemNodeNormals(double pfx[8], double pfy[8], double pfz[8],
             const double x[8], const double y[8], const double z[8])
{
  int i = 0;
  for (i = 0; i < 8; i++) {
    pfx[i] = 0.0;
    pfy[i] = 0.0;
    pfz[i] = 0.0;
  }

  // evaluate face one: nodes 0, 1, 2, 3
  SumElemFaceNormal(&pfx[0], &pfy[0], &pfz[0],
            &pfx[1], &pfy[1], &pfz[1],
            &pfx[2], &pfy[2], &pfz[2],
            &pfx[3], &pfy[3], &pfz[3],
            x[0], y[0], z[0], x[1], y[1], z[1],
            x[2], y[2], z[2], x[3], y[3], z[3]);

  // evaluate face two: nodes 0, 4, 5, 1
  SumElemFaceNormal(&pfx[0], &pfy[0], &pfz[0],
            &pfx[4], &pfy[4], &pfz[4],
            &pfx[5], &pfy[5], &pfz[5],
            &pfx[1], &pfy[1], &pfz[1],
            x[0], y[0], z[0], x[4], y[4], z[4],
            x[5], y[5], z[5], x[1], y[1], z[1]);

  // evaluate face three: nodes 1, 5, 6, 2
  SumElemFaceNormal(&pfx[1], &pfy[1], &pfz[1],
            &pfx[5], &pfy[5], &pfz[5],
            &pfx[6], &pfy[6], &pfz[6],
            &pfx[2], &pfy[2], &pfz[2],
            x[1], y[1], z[1], x[5], y[5], z[5],
            x[6], y[6], z[6], x[2], y[2], z[2]);

  // evaluate face four: nodes 2, 6, 7, 3
  SumElemFaceNormal(&pfx[2], &pfy[2], &pfz[2],
            &pfx[6], &pfy[6], &pfz[6],
            &pfx[7], &pfy[7], &pfz[7],
            &pfx[3], &pfy[3], &pfz[3],
            x[2], y[2], z[2], x[6], y[6], z[6],
            x[7], y[7], z[7], x[3], y[3], z[3]);

  // evaluate face five: nodes 3, 7, 4, 0
  SumElemFaceNormal(&pfx[3], &pfy[3], &pfz[3],
            &pfx[7], &pfy[7], &pfz[7],
            &pfx[4], &pfy[4], &pfz[4],
            &pfx[0], &pfy[0], &pfz[0],
            x[3], y[3], z[3], x[7], y[7], z[7],
            x[4], y[4], z[4], x[0], y[0], z[0]);

  // evaluate face six: nodes 4, 7, 6, 5
  SumElemFaceNormal(&pfx[4], &pfy[4], &pfz[4],
            &pfx[7], &pfy[7], &pfz[7],
            &pfx[6], &pfy[6], &pfz[6],
            &pfx[5], &pfy[5], &pfz[5],
            x[4], y[4], z[4], x[7], y[7], z[7],
            x[6], y[6], z[6], x[5], y[5], z[5]);
}

void SumElemFaceNormal(double *normalX0, double *normalY0, double *normalZ0,
               double *normalX1, double *normalY1, double *normalZ1,
               double *normalX2, double *normalY2, double *normalZ2,
               double *normalX3, double *normalY3, double *normalZ3,
               const double x0, const double y0, const double z0,
               const double x1, const double y1, const double z1,
               const double x2, const double y2, const double z2,
               const double x3, const double y3, const double z3)
{
  double bisectX0 = 0.5*(x3 + x2 - x1 - x0);
  double bisectY0 = 0.5*(y3 + y2 - y1 - y0);
  double bisectZ0 = 0.5*(z3 + z2 - z1 - z0);
  double bisectX1 = 0.5*(x2 + x1 - x3 - x0);
  double bisectY1 = 0.5*(y2 + y1 - y3 - y0);
  double bisectZ1 = 0.5*(z2 + z1 - z3 - z0);

  double areaX = 0.25*(bisectY0*bisectZ1 - bisectZ0*bisectY1);
  double areaY = 0.25*(bisectZ0*bisectX1 - bisectX0*bisectZ1);
  double areaZ = 0.25*(bisectX0*bisectY1 - bisectY0*bisectX1);

  *normalX0 += areaX;
  *normalX1 += areaX;
  *normalX2 += areaX;
  *normalX3 += areaX;

  *normalY0 += areaY;
  *normalY1 += areaY;
  *normalY2 += areaY;
  *normalY3 += areaY;

  *normalZ0 += areaZ;
  *normalZ1 += areaZ;
  *normalZ2 += areaZ;
  *normalZ3 += areaZ;
}

void SumElemStressesToNodeForces(double B[][8], const double stress_xx,
                 const double stress_yy, const double stress_zz,
                 double fx[], double fy[], double fz[])
{
  double pfx0 = B[0][0];   double pfx1 = B[0][1];
  double pfx2 = B[0][2];   double pfx3 = B[0][3];
  double pfx4 = B[0][4];   double pfx5 = B[0][5];
  double pfx6 = B[0][6];   double pfx7 = B[0][7];

  double pfy0 = B[1][0];   double pfy1 = B[1][1];
  double pfy2 = B[1][2];   double pfy3 = B[1][3];
  double pfy4 = B[1][4];   double pfy5 = B[1][5];
  double pfy6 = B[1][6];   double pfy7 = B[1][7];

  double pfz0 = B[2][0];   double pfz1 = B[2][1];
  double pfz2 = B[2][2];   double pfz3 = B[2][3];
  double pfz4 = B[2][4];   double pfz5 = B[2][5];
  double pfz6 = B[2][6];   double pfz7 = B[2][7];

  fx[0] = -stress_xx*pfx0;
  fx[1] = -stress_xx*pfx1;
  fx[2] = -stress_xx*pfx2;
  fx[3] = -stress_xx*pfx3;
  fx[4] = -stress_xx*pfx4;
  fx[5] = -stress_xx*pfx5;
  fx[6] = -stress_xx*pfx6;
  fx[7] = -stress_xx*pfx7;

  fy[0] = -stress_yy*pfy0;
  fy[1] = -stress_yy*pfy1;
  fy[2] = -stress_yy*pfy2;
  fy[3] = -stress_yy*pfy3;
  fy[4] = -stress_yy*pfy4;
  fy[5] = -stress_yy*pfy5;
  fy[6] = -stress_yy*pfy6;
  fy[7] = -stress_yy*pfy7;

  fz[0] = -stress_zz*pfz0;
  fz[1] = -stress_zz*pfz1;
  fz[2] = -stress_zz*pfz2;
  fz[3] = -stress_zz*pfz3;
  fz[4] = -stress_zz*pfz4;
  fz[5] = -stress_zz*pfz5;
  fz[6] = -stress_zz*pfz6;
  fz[7] = -stress_zz*pfz7;
}

#ifdef OMP_HPX
struct CalcAccelerationForNodes_helper_args1 {
  double *xdd;
  double *ydd;
  double *zdd;
  double *fx;
  double *fy;
  double *fz;
  double *nodalMass;
};

static int CalcAccelerationForNodes_par1(int i, void *args) {
  struct CalcAccelerationForNodes_helper_args1 *args1 =
                         (struct CalcAccelerationForNodes_helper_args1 *)args;

  args1->xdd[i] = args1->fx[i]/args1->nodalMass[i];
  args1->ydd[i] = args1->fy[i]/args1->nodalMass[i];
  args1->zdd[i] = args1->fz[i]/args1->nodalMass[i];

  return 0;
}
#endif

void CalcAccelerationForNodes(double *xdd, double *ydd, double *zdd,
                  double *fx, double *fy, double *fz,
                  double *nodalMass, int numNode)
{
#ifdef OMP_HPX
  struct CalcAccelerationForNodes_helper_args1 args1;
  args1.xdd = xdd;
  args1.ydd = ydd;
  args1.zdd = zdd;
  args1.fx = fx;
  args1.fy = fy;
  args1.fz = fz;
  args1.nodalMass = nodalMass;
  hpx_par_for_sync(CalcAccelerationForNodes_par1,0,numNode,&args1);
#else
  int i;
  for (i = 0; i < numNode; i++) {
    xdd[i] = fx[i]/nodalMass[i];
    ydd[i] = fy[i]/nodalMass[i];
    zdd[i] = fz[i]/nodalMass[i];
  }
#endif
}

#ifdef OMP_HPX
struct ApplyAccelerationBoundaryConditionsForNodes_helper_args1 {
  double *xdd;
  int *symmX;
};

struct ApplyAccelerationBoundaryConditionsForNodes_helper_args2 {
  double *ydd;
  int *symmY;
};

struct ApplyAccelerationBoundaryConditionsForNodes_helper_args3 {
  double *zdd;
  int *symmZ;
};

static int ApplyAccelerationBoundaryConditionsForNodes_par1(int i, void *args) {
  struct ApplyAccelerationBoundaryConditionsForNodes_helper_args1 *args1 =
                         (struct ApplyAccelerationBoundaryConditionsForNodes_helper_args1 *)args;

  args1->xdd[args1->symmX[i]] = 0.0;

  return 0;
}

static int ApplyAccelerationBoundaryConditionsForNodes_par2(int i, void *args) {
  struct ApplyAccelerationBoundaryConditionsForNodes_helper_args2 *args1 =
                         (struct ApplyAccelerationBoundaryConditionsForNodes_helper_args2 *)args;

  args1->ydd[args1->symmY[i]] = 0.0;

  return 0;
}

static int ApplyAccelerationBoundaryConditionsForNodes_par3(int i, void *args) {
  struct ApplyAccelerationBoundaryConditionsForNodes_helper_args3 *args1 =
                         (struct ApplyAccelerationBoundaryConditionsForNodes_helper_args3 *)args;

  args1->zdd[args1->symmZ[i]] = 0.0;

  return 0;
}
#endif

void ApplyAccelerationBoundaryConditionsForNodes(double *xdd, double *ydd, double *zdd,
                         int *symmX, int *symmY, int *symmZ, int size)
{
  int numNodeBC = (size + 1)*(size + 1);

  if (symmX != 0) {
#ifdef OMP_HPX
    struct ApplyAccelerationBoundaryConditionsForNodes_helper_args1 args1;
    args1.xdd = xdd;
    args1.symmX = symmX;
    hpx_par_for_sync(ApplyAccelerationBoundaryConditionsForNodes_par1,0,numNodeBC,&args1);
#else
  int i;
    for (i = 0; i < numNodeBC; i++)
      xdd[symmX[i]] = 0.0;
#endif
  }

  if (symmY != 0) {
#ifdef OMP_HPX
    struct ApplyAccelerationBoundaryConditionsForNodes_helper_args2 args2;
    args2.ydd = ydd;
    args2.symmY = symmY;
    hpx_par_for_sync(ApplyAccelerationBoundaryConditionsForNodes_par2,0,numNodeBC,&args2);
#else
  int i;
    for (i = 0; i < numNodeBC; i++)
      ydd[symmY[i]] = 0.0;
#endif
  }

  if (symmZ != 0) {
#ifdef OMP_HPX
    struct ApplyAccelerationBoundaryConditionsForNodes_helper_args3 args3;
    args3.zdd = zdd;
    args3.symmZ = symmZ;
    hpx_par_for_sync(ApplyAccelerationBoundaryConditionsForNodes_par3,0,numNodeBC,&args3);
#else
  int i;
    for (i = 0; i < numNodeBC; i++)
      zdd[symmZ[i]] = 0.0;
#endif
  }
}

#ifdef OMP_HPX
struct CalcVelocityForNodes_helper_args1 {
  double *xd; double *yd; double *zd;
  double *xdd; double *ydd; double *zdd;
  double dt; double u_cut;
};

static int CalcVelocityForNodes_par1(int i, void *args) {
  struct CalcVelocityForNodes_helper_args1 *args1 =
                         (struct CalcVelocityForNodes_helper_args1 *)args;

  double xdtmp, ydtmp, zdtmp;

  xdtmp = args1->xd[i] + args1->xdd[i]*args1->dt;
  if (fabs(xdtmp) < args1->u_cut)
    xdtmp = 0.0;
  args1->xd[i] = xdtmp;

  ydtmp = args1->yd[i] + args1->ydd[i]*args1->dt;
  if (fabs(ydtmp) < args1->u_cut)
    ydtmp = 0.0;
  args1->yd[i] = ydtmp;

  zdtmp = args1->zd[i] + args1->zdd[i]*args1->dt;
  if (fabs(zdtmp) < args1->u_cut)
    zdtmp = 0.0;
  args1->zd[i] = zdtmp;

  return 0;
}
#endif

void CalcVelocityForNodes(double *xd, double *yd, double *zd,
              double *xdd, double *ydd, double *zdd,
              const double dt, const double u_cut, int numNode)
{
#ifdef OMP_HPX
  struct CalcVelocityForNodes_helper_args1 args1;
  args1.xd = xd;
  args1.yd = yd;
  args1.zd = zd;
  args1.xdd = xdd;
  args1.ydd = ydd;
  args1.zdd = zdd;
  args1.dt = dt;
  args1.u_cut = u_cut;
  hpx_par_for_sync(CalcVelocityForNodes_par1,0,numNode,&args1);
#else
  int i;
  for (i = 0; i < numNode; i++) {
    double xdtmp, ydtmp, zdtmp;

    xdtmp = xd[i] + xdd[i]*dt;
    if (fabs(xdtmp) < u_cut)
      xdtmp = 0.0;
    xd[i] = xdtmp;

    ydtmp = yd[i] + ydd[i]*dt;
    if (fabs(ydtmp) < u_cut)
      ydtmp = 0.0;
    yd[i] = ydtmp;

    zdtmp = zd[i] + zdd[i]*dt;
    if (fabs(zdtmp) < u_cut)
      zdtmp = 0.0;
    zd[i] = zdtmp;
  }
#endif
}

#ifdef OMP_HPX
struct CalcPositionForNodes_helper_args1 {
  double *x;
  double *y;
  double *z;
  double *xd;
  double *yd;
  double *zd;
  double dt;
};

static int CalcPositionForNodes_par1(int i, void *args) {
  struct CalcPositionForNodes_helper_args1 *args1 =
                         (struct CalcPositionForNodes_helper_args1 *)args;

  args1->x[i] += args1->xd[i]*args1->dt;
  args1->y[i] += args1->yd[i]*args1->dt;
  args1->z[i] += args1->zd[i]*args1->dt;

  return 0;
}
#endif

void CalcPositionForNodes(double *x, double *y, double *z,
              double *xd, double *yd, double *zd,
              const double dt, int numNode)
{
#ifdef OMP_HPX
  struct CalcPositionForNodes_helper_args1 args1;
  args1.x = x;
  args1.y = y;
  args1.z = z;
  args1.xd = xd;
  args1.yd = yd;
  args1.zd = zd;
  args1.dt = dt;
  hpx_par_for_sync(CalcPositionForNodes_par1,0,numNode,&args1);
#else
  int i;
  for (i = 0; i < numNode; i++) {
    x[i] += xd[i]*dt;
    y[i] += yd[i]*dt;
    z[i] += zd[i]*dt;
  }
#endif
}

void LagrangeElements(hpx_addr_t local,Domain *domain,unsigned long epoch)
{
  int numElem = domain->numElem;

  domain->vnew = malloc(sizeof(double)*numElem);

  CalcLagrangeElements(domain);

  CalcQForElems(local,domain,epoch);

  ApplyMaterialPropertiesForElems(domain);

  UpdateVolumesForElems(domain->vnew, domain->v, domain->v_cut, numElem);

  free(domain->vnew);
}

#ifdef OMP_HPX
struct CalcLagrangeElements_helper_args1 {
  double *dxx;
  double *dyy;
  double *dzz;
  double *vdov;
  double *vnew;
  int rank;
  int cycle;
};

static int CalcLagrangeElements_par1(int k, void *args) {
  struct CalcLagrangeElements_helper_args1 *args1 =
                         (struct CalcLagrangeElements_helper_args1 *)args;

  double vdov = args1->dxx[k] + args1->dyy[k] + args1->dzz[k];
  double vdovthird = vdov/3.0;

  args1->vdov[k] = vdov;
  args1->dxx[k] -= vdovthird;
  args1->dyy[k] -= vdovthird;
  args1->dzz[k] -= vdovthird;

  if (args1->vnew[k] <= 0.0) {
    // need an abort function here, temporarily using exit function
    printf("rank %d: CalcLagrangeElements exit(-1) at cycle %d\n",
         args1->rank, args1->cycle);
    exit(-1);
  }

  return 0;
}
#endif

void CalcLagrangeElements(Domain *domain)
{
  int numElem = domain->numElem;

  if (numElem > 0) {
    const double deltatime = domain->deltatime;

    domain->dxx = malloc(sizeof(double)*numElem);
    domain->dyy = malloc(sizeof(double)*numElem);
    domain->dzz = malloc(sizeof(double)*numElem);

    CalcKinematicsForElems(domain->nodelist, domain->x, domain->y, domain->z,
               domain->xd, domain->yd, domain->zd,
               domain->dxx, domain->dyy, domain->dzz, domain->v,
               domain->volo, domain->vnew, domain->delv,
               domain->arealg, deltatime, numElem);

#ifdef OMP_HPX
    struct CalcLagrangeElements_helper_args1 args1;
    args1.dxx = domain->dxx;
    args1.dyy = domain->dyy;
    args1.dzz = domain->dzz;
    args1.vdov = domain->vdov;
    args1.vnew = domain->vnew;
    args1.rank = domain->rank;
    args1.cycle = domain->cycle;
    hpx_par_for_sync(CalcLagrangeElements_par1,0,numElem,&args1);
#else
    int k;
    for (k = 0; k < numElem; k++) {
      double vdov = domain->dxx[k] + domain->dyy[k] + domain->dzz[k];
      double vdovthird = vdov/3.0;

      domain->vdov[k] = vdov;
      domain->dxx[k] -= vdovthird;
      domain->dyy[k] -= vdovthird;
      domain->dzz[k] -= vdovthird;

      if (domain->vnew[k] <= 0.0) {
         // need an abort function here, temporarily using exit function
         printf("rank %d: CalcLagrangeElements exit(-1) at cycle %d\n",
         domain->rank, domain->cycle);
         exit(-1);
      }
    }
#endif

    free(domain->dzz);
    free(domain->dyy);
    free(domain->dxx);
  }
}

#ifdef OMP_HPX
struct CalcKinematicsForElems_helper_args1 {
  int *nodelist;
  double *x; double *y; double *z;
  double *xd; double *yd; double *zd;
  double *dxx; double *dyy; double *dzz;
  double *v; double *volo;
  double *vnew; double *delv; double *arealg;
  double deltaTime;
};

static int CalcKinematicsForElems_par1(int k, void *args) {
  struct CalcKinematicsForElems_helper_args1 *args1 =
                         (struct CalcKinematicsForElems_helper_args1 *)args;

  double volume;
  double relativeVolume;
  const int * const elemToNode = &(args1->nodelist[8*k]);

  double B[3][8];
  double D[6];
  double x_local[8];
  double y_local[8];
  double z_local[8];
  double xd_local[8];
  double yd_local[8];
  double zd_local[8];
  double detJ = 0.0;

  int lnode;
  for (lnode = 0; lnode < 8; lnode++) {
    int gnode = elemToNode[lnode];
    x_local[lnode] = args1->x[gnode];
    y_local[lnode] = args1->y[gnode];
    z_local[lnode] = args1->z[gnode];
  }

  volume = CalcElemVolume(x_local, y_local, z_local);
  relativeVolume = volume/args1->volo[k];
  args1->vnew[k] = relativeVolume;
  args1->delv[k] = relativeVolume - args1->v[k];

  args1->arealg[k] = CalcElemCharacteristicLength(x_local, y_local, z_local, volume);

  for (lnode = 0; lnode < 8; lnode++) {
    int gnode = elemToNode[lnode];
    xd_local[lnode] = args1->xd[gnode];
    yd_local[lnode] = args1->yd[gnode];
    zd_local[lnode] = args1->zd[gnode];
  }

  double dt2 = 0.5*args1->deltaTime;

  int j;
  for (j = 0; j < 8; j++) {
    x_local[j] -= dt2*xd_local[j];
    y_local[j] -= dt2*yd_local[j];
    z_local[j] -= dt2*zd_local[j];
  }

  CalcElemShapeFunctionDerivatives(x_local, y_local, z_local, B, &detJ);

  CalcElemVelocityGradient(xd_local, yd_local, zd_local, B, detJ, D);

  args1->dxx[k] = D[0];
  args1->dyy[k] = D[1];
  args1->dzz[k] = D[2];

  return 0;
}
#endif

void CalcKinematicsForElems(int *nodelist, double *x, double *y, double *z,
                double *xd, double *yd, double *zd,
                double *dxx, double *dyy, double *dzz,
                double *v, double *volo, double *vnew, double *delv,
                double *arealg, double deltaTime, int numElem)
{

#ifdef OMP_HPX
  struct CalcKinematicsForElems_helper_args1 args1;
  args1.nodelist = nodelist;
  args1.x = x;
  args1.y = y;
  args1.z = z;
  args1.xd = xd;
  args1.yd = yd;
  args1.zd = zd;
  args1.dxx = dxx;
  args1.dyy = dyy;
  args1.dzz = dzz;
  args1.v = v;
  args1.volo = volo;
  args1.vnew = vnew;
  args1.delv = delv;
  args1.arealg = arealg;
  args1.deltaTime = deltaTime;
  hpx_par_for_sync(CalcKinematicsForElems_par1,0,numElem,&args1);
#else
  double B[3][8];
  double D[6];
  double x_local[8];
  double y_local[8];
  double z_local[8];
  double xd_local[8];
  double yd_local[8];
  double zd_local[8];
  double detJ = 0.0;
  int k;
  for (k = 0; k < numElem; k++) {
    double volume;
    double relativeVolume;
    const int * const elemToNode = &nodelist[8*k];

    int lnode;
    for (lnode = 0; lnode < 8; lnode++) {
      int gnode = elemToNode[lnode];
      x_local[lnode] = x[gnode];
      y_local[lnode] = y[gnode];
      z_local[lnode] = z[gnode];
    }

    volume = CalcElemVolume(x_local, y_local, z_local);
    relativeVolume = volume/volo[k];
    vnew[k] = relativeVolume;
    delv[k] = relativeVolume - v[k];

    arealg[k] = CalcElemCharacteristicLength(x_local, y_local, z_local, volume);

    for (lnode = 0; lnode < 8; lnode++) {
      int gnode = elemToNode[lnode];
      xd_local[lnode] = xd[gnode];
      yd_local[lnode] = yd[gnode];
      zd_local[lnode] = zd[gnode];
    }

    double dt2 = 0.5*deltaTime;

    int j;
    for (j = 0; j < 8; j++) {
      x_local[j] -= dt2*xd_local[j];
      y_local[j] -= dt2*yd_local[j];
      z_local[j] -= dt2*zd_local[j];
    }

    CalcElemShapeFunctionDerivatives(x_local, y_local, z_local, B, &detJ);

    CalcElemVelocityGradient(xd_local, yd_local, zd_local, B, detJ, D);

    dxx[k] = D[0];
    dyy[k] = D[1];
    dzz[k] = D[2];
  }
#endif
}

double CalcElemCharacteristicLength(const double x[8], const double y[8],
                    const double z[8], const double volume)
{
  double a, charLength = 0.0;

  a = AreaFace(x[0], x[1], x[2], x[3],
           y[0], y[1], y[2], y[3],
           z[0], z[1], z[2], z[3]);
  charLength = (a > charLength ? a : charLength);

  a = AreaFace(x[4], x[5], x[6], x[7],
           y[4], y[5], y[6], y[7],
           z[4], z[5], z[6], z[7]);
  charLength = (a > charLength ? a : charLength);

  a = AreaFace(x[0], x[1], x[5], x[4],
           y[0], y[1], y[5], y[4],
           z[0], z[1], z[5], z[4]);
  charLength = (a > charLength ? a : charLength);

  a = AreaFace(x[1], x[2], x[6], x[5],
           y[1], y[2], y[6], y[5],
           z[1], z[2], z[6], z[5]);
  charLength = (a > charLength ? a : charLength);

  a = AreaFace(x[2], x[3], x[7], x[6],
           y[2], y[3], y[7], y[6],
           z[2], z[3], z[7], z[6]);
  charLength = (a > charLength ? a : charLength);

  a = AreaFace(x[3], x[0], x[4], x[7],
           y[3], y[0], y[4], y[7],
           z[3], z[0], z[4], z[7]);
  charLength = (a > charLength ? a : charLength);

  charLength = 4.0*volume/sqrt(charLength);

  return charLength;
}

double AreaFace(const double x0, const double x1, const double x2, const double x3,
        const double y0, const double y1, const double y2, const double y3,
        const double z0, const double z1, const double z2, const double z3)
{
  double fx = (x2 - x0) - (x3 - x1);
  double fy = (y2 - y0) - (y3 - y1);
  double fz = (z2 - z0) - (z3 - z1);
  double gx = (x2 - x0) + (x3 - x1);
  double gy = (y2 - y0) + (y3 - y1);
  double gz = (z2 - z0) + (z3 - z1);
  double area = (fx*fx + fy*fy + fz*fz)*(gx*gx + gy*gy + gz*gz) -
    (fx*gx + fy*gy + fz*gz)*(fx*gx + fy*gy + fz*gz);
  return area;
}

void CalcElemVelocityGradient(const double *const xvel, const double *const yvel,
                  const double *const zvel, double b[][8],
                  const double detJ, double *const d)
{
  const double inv_detJ = 1.0/detJ;
  double dyddx, dxddy, dzddx, dxddz, dzddy, dyddz;
  const double *const pfx = b[0];
  const double *const pfy = b[1];
  const double *const pfz = b[2];


  d[0] = inv_detJ*(pfx[0]*(xvel[0] - xvel[6])
           + pfx[1]*(xvel[1] - xvel[7])
           + pfx[2]*(xvel[2] - xvel[4])
           + pfx[3]*(xvel[3] - xvel[5]));

  d[1] = inv_detJ*(pfy[0]*(yvel[0] - yvel[6])
           + pfy[1]*(yvel[1] - yvel[7])
           + pfy[2]*(yvel[2] - yvel[4])
           + pfy[3]*(yvel[3] - yvel[5]));

  d[2] = inv_detJ*(pfz[0]*(zvel[0] - zvel[6])
           + pfz[1]*(zvel[1] - zvel[7])
           + pfz[2]*(zvel[2] - zvel[4])
           + pfz[3]*(zvel[3] - zvel[5]));

  dyddx = inv_detJ*(pfx[0]*(yvel[0] - yvel[6])
            + pfx[1]*(yvel[1] - yvel[7])
            + pfx[2]*(yvel[2] - yvel[4])
            + pfx[3]*(yvel[3] - yvel[5]));

  dxddy = inv_detJ*(pfy[0]*(xvel[0] - xvel[6])
            + pfy[1]*(xvel[1] - xvel[7])
            + pfy[2]*(xvel[2] - xvel[4])
            + pfy[3]*(xvel[3] - xvel[5]));

  dzddx = inv_detJ*(pfx[0]*(zvel[0] - zvel[6])
            + pfx[1]*(zvel[1] - zvel[7])
            + pfx[2]*(zvel[2] - zvel[4])
            + pfx[3]*(zvel[3] - zvel[5]));

  dxddz = inv_detJ*(pfz[0]*(xvel[0]-xvel[6])
            + pfz[1]*(xvel[1]-xvel[7])
            + pfz[2]*(xvel[2]-xvel[4])
            + pfz[3]*(xvel[3]-xvel[5]));

  dzddy = inv_detJ*(pfy[0]*(zvel[0]-zvel[6])
            + pfy[1]*(zvel[1]-zvel[7])
            + pfy[2]*(zvel[2]-zvel[4])
            + pfy[3]*(zvel[3]-zvel[5]));

  dyddz = inv_detJ*(pfz[0]*(yvel[0] - yvel[6])
            + pfz[1]*(yvel[1] - yvel[7])
            + pfz[2]*(yvel[2] - yvel[4])
            + pfz[3]*(yvel[3] - yvel[5]));

  d[5] = 0.5*(dxddy + dyddx);
  d[4] = 0.5*(dxddz + dzddx);
  d[3] = 0.5*(dzddy + dyddz);
}

#ifdef OMP_HPX
struct CalcMonotonicQGradientsForElems_helper_args1 {
  double *x; double *y; double *z;
  double *xd; double *yd; double *zd;
  double *volo; double *vnew; double *delv_xi;
  double *delv_eta; double *delv_zeta;
  double *delx_xi; double *delx_eta; double *delx_zeta;
  int *nodelist; int numElem;
  double ptiny;
};

static int CalcMonotonicQGradientsForElems_par1(int i, void *args) {
  struct CalcMonotonicQGradientsForElems_helper_args1 *args1 =
                         (struct CalcMonotonicQGradientsForElems_helper_args1 *)args;

#define SUM4(a,b,c,d) (a + b + c + d)
  double ax, ay, az;
  double dxv, dyv, dzv;

  const int *elemToNode = &(args1->nodelist[8*i]);
  int n0 = elemToNode[0];
  int n1 = elemToNode[1];
  int n2 = elemToNode[2];
  int n3 = elemToNode[3];
  int n4 = elemToNode[4];
  int n5 = elemToNode[5];
  int n6 = elemToNode[6];
  int n7 = elemToNode[7];

  double x0 = args1->x[n0];
  double x1 = args1->x[n1];
  double x2 = args1->x[n2];
  double x3 = args1->x[n3];
  double x4 = args1->x[n4];
  double x5 = args1->x[n5];
  double x6 = args1->x[n6];
  double x7 = args1->x[n7];

  double y0 = args1->y[n0];
  double y1 = args1->y[n1];
  double y2 = args1->y[n2];
  double y3 = args1->y[n3];
  double y4 = args1->y[n4];
  double y5 = args1->y[n5];
  double y6 = args1->y[n6];
  double y7 = args1->y[n7];

  double z0 = args1->z[n0];
  double z1 = args1->z[n1];
  double z2 = args1->z[n2];
  double z3 = args1->z[n3];
  double z4 = args1->z[n4];
  double z5 = args1->z[n5];
  double z6 = args1->z[n6];
  double z7 = args1->z[n7];

  double xv0 = args1->xd[n0];
  double xv1 = args1->xd[n1];
  double xv2 = args1->xd[n2];
  double xv3 = args1->xd[n3];
  double xv4 = args1->xd[n4];
  double xv5 = args1->xd[n5];
  double xv6 = args1->xd[n6];
  double xv7 = args1->xd[n7];

  double yv0 = args1->yd[n0];
  double yv1 = args1->yd[n1];
  double yv2 = args1->yd[n2];
  double yv3 = args1->yd[n3];
  double yv4 = args1->yd[n4];
  double yv5 = args1->yd[n5];
  double yv6 = args1->yd[n6];
  double yv7 = args1->yd[n7];

  double zv0 = args1->zd[n0];
  double zv1 = args1->zd[n1];
  double zv2 = args1->zd[n2];
  double zv3 = args1->zd[n3];
  double zv4 = args1->zd[n4];
  double zv5 = args1->zd[n5];
  double zv6 = args1->zd[n6];
  double zv7 = args1->zd[n7];

  double vol = args1->volo[i]*args1->vnew[i];
  double norm = 1.0/(vol + args1->ptiny);

  double dxj = -0.25*(SUM4(x0,x1,x5,x4) - SUM4(x3,x2,x6,x7));
  double dyj = -0.25*(SUM4(y0,y1,y5,y4) - SUM4(y3,y2,y6,y7));
  double dzj = -0.25*(SUM4(z0,z1,z5,z4) - SUM4(z3,z2,z6,z7));

  double dxi = 0.25*(SUM4(x1,x2,x6,x5) - SUM4(x0,x3,x7,x4));
  double dyi = 0.25*(SUM4(y1,y2,y6,y5) - SUM4(y0,y3,y7,y4));
  double dzi = 0.25*(SUM4(z1,z2,z6,z5) - SUM4(z0,z3,z7,z4));

  double dxk = 0.25*(SUM4(x4,x5,x6,x7) - SUM4(x0,x1,x2,x3));
  double dyk = 0.25*(SUM4(y4,y5,y6,y7) - SUM4(y0,y1,y2,y3));
  double dzk = 0.25*(SUM4(z4,z5,z6,z7) - SUM4(z0,z1,z2,z3));

  // find delvk and delxk ( i cross j )

  ax = dyi*dzj - dzi*dyj;
  ay = dzi*dxj - dxi*dzj;
  az = dxi*dyj - dyi*dxj;

  args1->delx_zeta[i] = vol/sqrt(ax*ax + ay*ay + az*az + args1->ptiny);

  ax *= norm;
  ay *= norm;
  az *= norm;

  dxv = 0.25*(SUM4(xv4,xv5,xv6,xv7) - SUM4(xv0,xv1,xv2,xv3));
  dyv = 0.25*(SUM4(yv4,yv5,yv6,yv7) - SUM4(yv0,yv1,yv2,yv3));
  dzv = 0.25*(SUM4(zv4,zv5,zv6,zv7) - SUM4(zv0,zv1,zv2,zv3));

  args1->delv_zeta[i] = ax*dxv + ay*dyv + az*dzv;

  // find delxi and delvi ( j cross k )

  ax = dyj*dzk - dzj*dyk;
  ay = dzj*dxk - dxj*dzk;
  az = dxj*dyk - dyj*dxk;

  args1->delx_xi[i] = vol/sqrt(ax*ax + ay*ay + az*az + args1->ptiny);

  ax *= norm;
  ay *= norm;
  az *= norm;

  dxv = 0.25*(SUM4(xv1,xv2,xv6,xv5) - SUM4(xv0,xv3,xv7,xv4));
  dyv = 0.25*(SUM4(yv1,yv2,yv6,yv5) - SUM4(yv0,yv3,yv7,yv4));
  dzv = 0.25*(SUM4(zv1,zv2,zv6,zv5) - SUM4(zv0,zv3,zv7,zv4));

  args1->delv_xi[i] = ax*dxv + ay*dyv + az*dzv;

  // find delxj and delvj ( k cross i )

  ax = dyk*dzi - dzk*dyi;
  ay = dzk*dxi - dxk*dzi;
  az = dxk*dyi - dyk*dxi;

  args1->delx_eta[i] = vol/sqrt(ax*ax + ay*ay + az*az + args1->ptiny);

  ax *= norm;
  ay *= norm;
  az *= norm;

  dxv = -0.25*(SUM4(xv0,xv1,xv5,xv4) - SUM4(xv3,xv2,xv6,xv7));
  dyv = -0.25*(SUM4(yv0,yv1,yv5,yv4) - SUM4(yv3,yv2,yv6,yv7));
  dzv = -0.25*(SUM4(zv0,zv1,zv5,zv4) - SUM4(zv3,zv2,zv6,zv7));

  args1->delv_eta[i] = ax*dxv + ay*dyv + az*dzv;

#undef SUM4
  return 0;
}
#endif

void CalcMonotonicQGradientsForElems(double *x, double *y, double *z,
                     double *xd, double *yd, double *zd,
                     double *volo, double *vnew, double *delv_xi,
                     double *delv_eta, double *delv_zeta,
                     double *delx_xi, double *delx_eta, double *delx_zeta,
                     int *nodelist, int numElem)
{
#define SUM4(a,b,c,d) (a + b + c + d)
  const double ptiny = 1.e-36;
#ifdef OMP_HPX
  struct CalcMonotonicQGradientsForElems_helper_args1 args1;
  args1.x = x;
  args1.y = y;
  args1.z = z;
  args1.xd = xd;
  args1.yd = yd;
  args1.zd = zd;
  args1.volo = volo;
  args1.vnew = vnew;
  args1.delv_xi = delv_xi;
  args1.delv_eta = delv_eta;
  args1.delv_zeta = delv_zeta;
  args1.delx_xi = delx_xi;
  args1.delx_eta = delx_eta;
  args1.delx_zeta = delx_zeta;
  args1.nodelist = nodelist;
  args1.numElem = numElem;
  args1.ptiny = ptiny;
  hpx_par_for_sync(CalcMonotonicQGradientsForElems_par1,0,numElem,&args1);
#else
  int i;
  for (i = 0; i < numElem; i++) {
    double ax, ay, az;
    double dxv, dyv, dzv;

    const int *elemToNode = &nodelist[8*i];
    int n0 = elemToNode[0];
    int n1 = elemToNode[1];
    int n2 = elemToNode[2];
    int n3 = elemToNode[3];
    int n4 = elemToNode[4];
    int n5 = elemToNode[5];
    int n6 = elemToNode[6];
    int n7 = elemToNode[7];

    double x0 = x[n0];
    double x1 = x[n1];
    double x2 = x[n2];
    double x3 = x[n3];
    double x4 = x[n4];
    double x5 = x[n5];
    double x6 = x[n6];
    double x7 = x[n7];

    double y0 = y[n0];
    double y1 = y[n1];
    double y2 = y[n2];
    double y3 = y[n3];
    double y4 = y[n4];
    double y5 = y[n5];
    double y6 = y[n6];
    double y7 = y[n7];

    double z0 = z[n0];
    double z1 = z[n1];
    double z2 = z[n2];
    double z3 = z[n3];
    double z4 = z[n4];
    double z5 = z[n5];
    double z6 = z[n6];
    double z7 = z[n7];

    double xv0 = xd[n0];
    double xv1 = xd[n1];
    double xv2 = xd[n2];
    double xv3 = xd[n3];
    double xv4 = xd[n4];
    double xv5 = xd[n5];
    double xv6 = xd[n6];
    double xv7 = xd[n7];

    double yv0 = yd[n0];
    double yv1 = yd[n1];
    double yv2 = yd[n2];
    double yv3 = yd[n3];
    double yv4 = yd[n4];
    double yv5 = yd[n5];
    double yv6 = yd[n6];
    double yv7 = yd[n7];

    double zv0 = zd[n0];
    double zv1 = zd[n1];
    double zv2 = zd[n2];
    double zv3 = zd[n3];
    double zv4 = zd[n4];
    double zv5 = zd[n5];
    double zv6 = zd[n6];
    double zv7 = zd[n7];

    double vol = volo[i]*vnew[i];
    double norm = 1.0/(vol + ptiny);

    double dxj = -0.25*(SUM4(x0,x1,x5,x4) - SUM4(x3,x2,x6,x7));
    double dyj = -0.25*(SUM4(y0,y1,y5,y4) - SUM4(y3,y2,y6,y7));
    double dzj = -0.25*(SUM4(z0,z1,z5,z4) - SUM4(z3,z2,z6,z7));

    double dxi = 0.25*(SUM4(x1,x2,x6,x5) - SUM4(x0,x3,x7,x4));
    double dyi = 0.25*(SUM4(y1,y2,y6,y5) - SUM4(y0,y3,y7,y4));
    double dzi = 0.25*(SUM4(z1,z2,z6,z5) - SUM4(z0,z3,z7,z4));

    double dxk = 0.25*(SUM4(x4,x5,x6,x7) - SUM4(x0,x1,x2,x3));
    double dyk = 0.25*(SUM4(y4,y5,y6,y7) - SUM4(y0,y1,y2,y3));
    double dzk = 0.25*(SUM4(z4,z5,z6,z7) - SUM4(z0,z1,z2,z3));

    // find delvk and delxk ( i cross j )

    ax = dyi*dzj - dzi*dyj;
    ay = dzi*dxj - dxi*dzj;
    az = dxi*dyj - dyi*dxj;

    delx_zeta[i] = vol/sqrt(ax*ax + ay*ay + az*az + ptiny);

    ax *= norm;
    ay *= norm;
    az *= norm;

    dxv = 0.25*(SUM4(xv4,xv5,xv6,xv7) - SUM4(xv0,xv1,xv2,xv3));
    dyv = 0.25*(SUM4(yv4,yv5,yv6,yv7) - SUM4(yv0,yv1,yv2,yv3));
    dzv = 0.25*(SUM4(zv4,zv5,zv6,zv7) - SUM4(zv0,zv1,zv2,zv3));

    delv_zeta[i] = ax*dxv + ay*dyv + az*dzv;

    // find delxi and delvi ( j cross k )

    ax = dyj*dzk - dzj*dyk;
    ay = dzj*dxk - dxj*dzk;
    az = dxj*dyk - dyj*dxk;

    delx_xi[i] = vol/sqrt(ax*ax + ay*ay + az*az + ptiny);

    ax *= norm;
    ay *= norm;
    az *= norm;

    dxv = 0.25*(SUM4(xv1,xv2,xv6,xv5) - SUM4(xv0,xv3,xv7,xv4));
    dyv = 0.25*(SUM4(yv1,yv2,yv6,yv5) - SUM4(yv0,yv3,yv7,yv4));
    dzv = 0.25*(SUM4(zv1,zv2,zv6,zv5) - SUM4(zv0,zv3,zv7,zv4));

    delv_xi[i] = ax*dxv + ay*dyv + az*dzv;

    // find delxj and delvj ( k cross i )

    ax = dyk*dzi - dzk*dyi;
    ay = dzk*dxi - dxk*dzi;
    az = dxk*dyi - dyk*dxi;

    delx_eta[i] = vol/sqrt(ax*ax + ay*ay + az*az + ptiny);

    ax *= norm;
    ay *= norm;
    az *= norm;

    dxv = -0.25*(SUM4(xv0,xv1,xv5,xv4) - SUM4(xv3,xv2,xv6,xv7));
    dyv = -0.25*(SUM4(yv0,yv1,yv5,yv4) - SUM4(yv3,yv2,yv6,yv7));
    dzv = -0.25*(SUM4(zv0,zv1,zv5,zv4) - SUM4(zv3,zv2,zv6,zv7));

    delv_eta[i] = ax*dxv + ay*dyv + az*dzv;
  }
#endif
#undef SUM4
}

#ifdef OMP_HPX
struct CalcMonotonicQRegionForElems_helper_args1 {
  int *matElemlist;
  int *elemBC;
  int *lxim;
  int *lxip;
  int *letam;
  int *letap;
  int *lzetam;
  int *lzetap;
  double *delv_xi;
  double *delv_eta;
  double *delv_zeta;
  double *delx_xi; double *delx_eta; double *delx_zeta;
  double *vdov; double *volo; double *vnew;
  double *elemMass; double *qq; double *ql;
  double qlc_monoq; double qqc_monoq; double monoq_limiter_mult;
  double monoq_max_slope; double ptiny;
};

static int CalcMonotonicQRegionForElems_par1(int ielem, void *args) {
  struct CalcMonotonicQRegionForElems_helper_args1 *args1 =
                         (struct CalcMonotonicQRegionForElems_helper_args1 *)args;

  double qlin, qquad;
  double phixi, phieta, phizeta;
  int i = args1->matElemlist[ielem];
  int bcMask = args1->elemBC[i];
  double delvm = 0.0, delvp = 0.0;

  // phixi
  double norm = 1.0/(args1->delv_xi[i] + args1->ptiny);

  switch (bcMask & XI_M) {
  case XI_M_COMM: /* needs comm data */
  case 0:         delvm = args1->delv_xi[args1->lxim[i]]; break;
  case XI_M_SYMM: delvm = args1->delv_xi[i];       break;
  case XI_M_FREE: delvm = 0.0;              break;
  default:        /* ERROR */;              break;
  }

  switch (bcMask & XI_P) {
  case XI_P_COMM: /* needs comm data */
  case 0:         delvp = args1->delv_xi[args1->lxip[i]]; break;
  case XI_P_SYMM: delvp = args1->delv_xi[i];       break;
  case XI_P_FREE: delvp = 0.0;              break;
  default:        /* ERROR */;              break;
  }

  delvm = delvm*norm;
  delvp = delvp*norm;

  phixi = 0.5*(delvm + delvp);

  delvm *= args1->monoq_limiter_mult;
  delvp *= args1->monoq_limiter_mult;

  if (delvm < phixi) phixi = delvm;
  if (delvp < phixi) phixi = delvp;
  if (phixi < 0.0) phixi = 0.0;
  if (phixi > args1->monoq_max_slope) phixi = args1->monoq_max_slope;

  // phieta
  norm = 1.0/(args1->delv_eta[i] + args1->ptiny);

  switch (bcMask & ETA_M) {
  case ETA_M_COMM: /* needs comm data */
  case 0:          delvm = args1->delv_eta[args1->letam[i]]; break;
  case ETA_M_SYMM: delvm = args1->delv_eta[i];        break;
  case ETA_M_FREE: delvm = 0.0;                break;
  default:         /* ERROR */;                break;
  }

  switch (bcMask & ETA_P) {
  case ETA_P_COMM: /* needs comm data */
  case 0:          delvp = args1->delv_eta[args1->letap[i]]; break;
  case ETA_P_SYMM: delvp = args1->delv_eta[i];        break;
  case ETA_P_FREE: delvp = 0.0;                break;
  default:         /* ERROR */;                break;
  }

  delvm = delvm*norm;
  delvp = delvp*norm;

  phieta = 0.5*(delvm + delvp);

  delvm *= args1->monoq_limiter_mult;
  delvp *= args1->monoq_limiter_mult;

  if (delvm  < phieta) phieta = delvm;
  if (delvp  < phieta ) phieta = delvp;
  if (phieta < 0.0) phieta = 0.0;
  if (phieta > args1->monoq_max_slope)  phieta = args1->monoq_max_slope;

  //  phizeta
  norm = 1.0/(args1->delv_zeta[i] + args1->ptiny);

  switch (bcMask & ZETA_M) {
  case ZETA_M_COMM: /* needs comm data */
  case 0:           delvm = args1->delv_zeta[args1->lzetam[i]]; break;
  case ZETA_M_SYMM: delvm = args1->delv_zeta[i];         break;
  case ZETA_M_FREE: delvm = 0.0;                  break;
  default:          /* ERROR */;                  break;
  }

  switch (bcMask & ZETA_P) {
  case ZETA_P_COMM: /* needs comm data */
  case 0:           delvp = args1->delv_zeta[args1->lzetap[i]]; break;
  case ZETA_P_SYMM: delvp = args1->delv_zeta[i];         break;
  case ZETA_P_FREE: delvp = 0.0;                  break;
  default:          /* ERROR */;                  break;
  }

  delvm = delvm*norm;
  delvp = delvp*norm;

  phizeta = 0.5*(delvm + delvp);

  delvm *= args1->monoq_limiter_mult;
  delvp *= args1->monoq_limiter_mult;

  if (delvm < phizeta) phizeta = delvm;
  if (delvp < phizeta) phizeta = delvp;
  if (phizeta < 0.0) phizeta = 0.0;
  if (phizeta > args1->monoq_max_slope) phizeta = args1->monoq_max_slope;

  // Remove length scale
  if (args1->vdov[i] > 0.0)  {
    qlin = 0.0;
    qquad = 0.0;
  } else {
    double delvxxi = args1->delv_xi[i]*args1->delx_xi[i];
    double delvxeta = args1->delv_eta[i]*args1->delx_eta[i];
    double delvxzeta = args1->delv_zeta[i]*args1->delx_zeta[i];

    if (delvxxi   > 0.0) delvxxi   = 0.0;
    if (delvxeta  > 0.0) delvxeta  = 0.0;
    if (delvxzeta > 0.0) delvxzeta = 0.0;

    double rho = args1->elemMass[i]/(args1->volo[i]*args1->vnew[i]);

    qlin = -args1->qlc_monoq*rho*
  (delvxxi*(1.0 - phixi) + delvxeta*(1.0 - phieta) + delvxzeta*(1.0 - phizeta));

    qquad = args1->qqc_monoq*rho*
  (delvxxi*delvxxi*(1.0 - phixi*phixi) +
   delvxeta*delvxeta*(1.0 - phieta*phieta) +
   delvxzeta*delvxzeta*(1.0 - phizeta*phizeta));
  }

  args1->qq[i] = qquad;
  args1->ql[i] = qlin;

  return 0;
}
#endif

void CalcMonotonicQRegionForElems(int *matElemlist, int *elemBC, int *lxim, int *lxip,
                  int *letam, int *letap, int *lzetam, int *lzetap,
                  double *delv_xi, double *delv_eta, double *delv_zeta,
                  double *delx_xi, double *delx_eta, double *delx_zeta,
                  double *vdov, double *volo, double *vnew,
                  double *elemMass, double *qq, double *ql,
                  double qlc_monoq, double qqc_monoq, double monoq_limiter_mult,
                  double monoq_max_slope, double ptiny, int numElem)
{
#ifdef OMP_HPX
  struct CalcMonotonicQRegionForElems_helper_args1 args1;
  args1.matElemlist = matElemlist;
  args1.elemBC = elemBC;
  args1.lxim = lxim;
  args1.lxip = lxip;
  args1.letam = letam;
  args1.letap = letap;
  args1.lzetam = lzetam;
  args1.lzetap = lzetap;
  args1.delv_xi = delv_xi;
  args1.delv_eta = delv_eta;
  args1.delv_zeta = delv_zeta;
  args1.delx_xi = delx_xi;
  args1.delx_eta = delx_eta;
  args1.delx_zeta = delx_zeta;
  args1.vdov = vdov;
  args1.volo = volo;
  args1.vnew = vnew;
  args1.elemMass = elemMass;
  args1.qq = qq;
  args1.ql = ql;
  args1.qlc_monoq = qlc_monoq;
  args1.qqc_monoq = qqc_monoq;
  args1.monoq_limiter_mult = monoq_limiter_mult;
  args1.monoq_max_slope = monoq_max_slope;
  args1.ptiny = ptiny;
  hpx_par_for_sync(CalcMonotonicQRegionForElems_par1,0,numElem,&args1);
#else
  int ielem;
  for (ielem = 0; ielem < numElem; ielem++) {
    double qlin, qquad;
    double phixi, phieta, phizeta;
    int i = matElemlist[ielem];
    int bcMask = elemBC[i];
    double delvm = 0.0, delvp = 0.0;

    // phixi
    double norm = 1.0/(delv_xi[i] + ptiny);

    switch (bcMask & XI_M) {
    case XI_M_COMM: /* needs comm data */
    case 0:         delvm = delv_xi[lxim[i]]; break;
    case XI_M_SYMM: delvm = delv_xi[i];       break;
    case XI_M_FREE: delvm = 0.0;              break;
    default:        /* ERROR */;              break;
    }

    switch (bcMask & XI_P) {
    case XI_P_COMM: /* needs comm data */
    case 0:         delvp = delv_xi[lxip[i]]; break;
    case XI_P_SYMM: delvp = delv_xi[i];       break;
    case XI_P_FREE: delvp = 0.0;              break;
    default:        /* ERROR */;              break;
    }

    delvm = delvm*norm;
    delvp = delvp*norm;

    phixi = 0.5*(delvm + delvp);

    delvm *= monoq_limiter_mult;
    delvp *= monoq_limiter_mult;

    if (delvm < phixi) phixi = delvm;
    if (delvp < phixi) phixi = delvp;
    if (phixi < 0.0) phixi = 0.0;
    if (phixi > monoq_max_slope) phixi = monoq_max_slope;

    // phieta
    norm = 1.0/(delv_eta[i] + ptiny);

    switch (bcMask & ETA_M) {
    case ETA_M_COMM: /* needs comm data */
    case 0:          delvm = delv_eta[letam[i]]; break;
    case ETA_M_SYMM: delvm = delv_eta[i];        break;
    case ETA_M_FREE: delvm = 0.0;                break;
    default:         /* ERROR */;                break;
    }

    switch (bcMask & ETA_P) {
    case ETA_P_COMM: /* needs comm data */
    case 0:          delvp = delv_eta[letap[i]]; break;
    case ETA_P_SYMM: delvp = delv_eta[i];        break;
    case ETA_P_FREE: delvp = 0.0;                break;
    default:         /* ERROR */;                break;
    }

    delvm = delvm*norm;
    delvp = delvp*norm;

    phieta = 0.5*(delvm + delvp);

    delvm *= monoq_limiter_mult;
    delvp *= monoq_limiter_mult;

    if (delvm  < phieta) phieta = delvm;
    if (delvp  < phieta ) phieta = delvp;
    if (phieta < 0.0) phieta = 0.0;
    if (phieta > monoq_max_slope)  phieta = monoq_max_slope;

    //  phizeta
    norm = 1.0/(delv_zeta[i] + ptiny);

    switch (bcMask & ZETA_M) {
    case ZETA_M_COMM: /* needs comm data */
    case 0:           delvm = delv_zeta[lzetam[i]]; break;
    case ZETA_M_SYMM: delvm = delv_zeta[i];         break;
    case ZETA_M_FREE: delvm = 0.0;                  break;
    default:          /* ERROR */;                  break;
    }

    switch (bcMask & ZETA_P) {
    case ZETA_P_COMM: /* needs comm data */
    case 0:           delvp = delv_zeta[lzetap[i]]; break;
    case ZETA_P_SYMM: delvp = delv_zeta[i];         break;
    case ZETA_P_FREE: delvp = 0.0;                  break;
    default:          /* ERROR */;                  break;
    }

    delvm = delvm*norm;
    delvp = delvp*norm;

    phizeta = 0.5*(delvm + delvp);

    delvm *= monoq_limiter_mult;
    delvp *= monoq_limiter_mult;

    if (delvm < phizeta) phizeta = delvm;
    if (delvp < phizeta) phizeta = delvp;
    if (phizeta < 0.0) phizeta = 0.0;
    if (phizeta > monoq_max_slope) phizeta = monoq_max_slope;

    // Remove length scale
    if (vdov[i] > 0.0)  {
      qlin = 0.0;
      qquad = 0.0;
    } else {
      double delvxxi = delv_xi[i]*delx_xi[i];
      double delvxeta = delv_eta[i]*delx_eta[i];
      double delvxzeta = delv_zeta[i]*delx_zeta[i];

      if (delvxxi   > 0.0) delvxxi   = 0.0;
      if (delvxeta  > 0.0) delvxeta  = 0.0;
      if (delvxzeta > 0.0) delvxzeta = 0.0;

      double rho = elemMass[i]/(volo[i]*vnew[i]);

      qlin = -qlc_monoq*rho*
    (delvxxi*(1.0 - phixi) + delvxeta*(1.0 - phieta) + delvxzeta*(1.0 - phizeta));

      qquad = qqc_monoq*rho*
    (delvxxi*delvxxi*(1.0 - phixi*phixi) +
     delvxeta*delvxeta*(1.0 - phieta*phieta) +
     delvxzeta*delvxzeta*(1.0 - phizeta*phizeta));
    }

    qq[i] = qquad;
    ql[i] = qlin;
  }
#endif
}

void CalcMonotonicQForElems(Domain *domain)
{
  int numElem = domain->numElem;
  if (numElem > 0) {
    const double ptiny = 1.0e-36;

    CalcMonotonicQRegionForElems(domain->matElemlist, domain->elemBC, domain->lxim,
                 domain->lxip, domain->letam, domain->letap,
                 domain->lzetam, domain->lzetap, domain->delv_xi,
                 domain->delv_eta, domain->delv_zeta, domain->delx_xi,
                 domain->delx_eta, domain->delx_zeta, domain->vdov,
                 domain->volo, domain->vnew, domain->elemMass,
                 domain->qq, domain->ql, domain->qlc_monoq, domain->qqc_monoq,
                 domain->monoq_limiter_mult, domain->monoq_max_slope,
                 ptiny, numElem);
  }
}

double CalcElemVolume(const double x[8], const double y[8], const double z[8])
{
  double x0 = x[0], x1 = x[1], x2 = x[2], x3 = x[3], x4 = x[4], x5 = x[5], x6 = x[6], x7 = x[7],
    y0 = y[0], y1 = y[1], y2 = y[2], y3 = y[3], y4 = y[4], y5 = y[5], y6 = y[6], y7 = y[7],
    z0 = z[0], z1 = z[1], z2 = z[2], z3 = z[3], z4 = z[4], z5 = z[5], z6 = z[6], z7 = z[7];

  double twelveth = 1.0/12.0;

  double dx61 = x6 - x1;
  double dy61 = y6 - y1;
  double dz61 = z6 - z1;

  double dx70 = x7 - x0;
  double dy70 = y7 - y0;
  double dz70 = z7 - z0;

  double dx63 = x6 - x3;
  double dy63 = y6 - y3;
  double dz63 = z6 - z3;

  double dx20 = x2 - x0;
  double dy20 = y2 - y0;
  double dz20 = z2 - z0;

  double dx50 = x5 - x0;
  double dy50 = y5 - y0;
  double dz50 = z5 - z0;

  double dx64 = x6 - x4;
  double dy64 = y6 - y4;
  double dz64 = z6 - z4;

  double dx31 = x3 - x1;
  double dy31 = y3 - y1;
  double dz31 = z3 - z1;

  double dx72 = x7 - x2;
  double dy72 = y7 - y2;
  double dz72 = z7 - z2;

  double dx43 = x4 - x3;
  double dy43 = y4 - y3;
  double dz43 = z4 - z3;

  double dx57 = x5 - x7;
  double dy57 = y5 - y7;
  double dz57 = z5 - z7;

  double dx14 = x1 - x4;
  double dy14 = y1 - y4;
  double dz14 = z1 - z4;

  double dx25 = x2 - x5;
  double dy25 = y2 - y5;
  double dz25 = z2 - z5;

#define TRIPLE_PRODUCT(x1, y1, z1, x2, y2, z2, x3, y3, z3) \
   ((x1)*((y2)*(z3) - (z2)*(y3)) + (x2)*((z1)*(y3) - (y1)*(z3)) + (x3)*((y1)*(z2) - (z1)*(y2)))

  double volume =
    TRIPLE_PRODUCT(dx31 + dx72, dx63, dx20,
       dy31 + dy72, dy63, dy20,
       dz31 + dz72, dz63, dz20) +
    TRIPLE_PRODUCT(dx43 + dx57, dx64, dx70,
       dy43 + dy57, dy64, dy70,
       dz43 + dz57, dz64, dz70) +
    TRIPLE_PRODUCT(dx14 + dx25, dx61, dx50,
       dy14 + dy25, dy61, dy50,
       dz14 + dz25, dz61, dz50);

#undef TRIPLE_PRODUCT

  volume *= twelveth;
  return volume;
}

void CalcQForElems(hpx_addr_t local,Domain *domain,unsigned long epoch)
{
  int numElem = domain->numElem;

  if (numElem != 0) {

    //domain->delv_xi = malloc(sizeof(double)*allElem);
    //domain->delv_eta = malloc(sizeof(double)*allElem);
    //domain->delv_zeta = malloc(sizeof(double)*allElem);

    //domain->delx_xi = malloc(sizeof(double)*numElem);
    //domain->delx_eta = malloc(sizeof(double)*numElem);
    //domain->delx_zeta = malloc(sizeof(double)*numElem);

    CalcMonotonicQGradientsForElems(domain->x, domain->y, domain->z,
                    domain->xd, domain->yd, domain->zd,
                    domain->volo, domain->vnew, domain->delv_xi,
                    domain->delv_eta, domain->delv_zeta,
                    domain->delx_xi, domain->delx_eta, domain->delx_zeta,
                    domain->nodelist, domain->numElem);

    MonoQ(local,domain,epoch);
    hpx_lco_sema_v_sync(domain->sem_monoq);
    hpx_lco_wait(domain->monoq_and[epoch % 2]);
    hpx_lco_sema_p(domain->sem_monoq);
    hpx_lco_delete(domain->monoq_and[epoch % 2], HPX_NULL);


    CalcMonotonicQForElems(domain);

    //free(domain->delx_zeta);
    //free(domain->delx_eta);
    //free(domain->delx_xi);

    //free(domain->delv_zeta);
    //free(domain->delv_eta);
    //free(domain->delv_xi);

    double qstop = domain->qstop;
    int i, idx = -1;
    for (i = 0; i < numElem; i++) {
      if (domain->q[i] > qstop) {
    idx = i;
    break;
      }
    }

    if (idx >= 0) {
      // need an abort here, temporarily using exit function
      printf("rank %d: CalcQForElems exit(-1) at cycle %d\n",
         domain->rank, domain->cycle);
      exit(-1);
    }
  }
}

#ifdef OMP_HPX
struct CalcPressureForElems_helper_args1 {
  double *bvc;
  double c1s;
  double *compression;
  double *pbvc;
};

struct CalcPressureForElems_helper_args2 {
  double *p_new;
  double *bvc;
  double *e_old;
  double p_cut;
  double *vnewc;
  double eosvmax;
  double pmin;
};

static int CalcPressureForElems_par1(int i, void *args) {
  struct CalcPressureForElems_helper_args1 *args1 =
                         (struct CalcPressureForElems_helper_args1 *)args;

  args1->bvc[i] = args1->c1s*(args1->compression[i] + 1.0);
  args1->pbvc[i] = args1->c1s;

  return 0;
}

static int CalcPressureForElems_par2(int i, void *args) {
  struct CalcPressureForElems_helper_args2 *args1 =
                         (struct CalcPressureForElems_helper_args2 *)args;

  args1->p_new[i] = args1->bvc[i]*args1->e_old[i];

  if (fabs(args1->p_new[i]) < args1->p_cut)
    args1->p_new[i] = 0.0;

  if (args1->vnewc[i] >= args1->eosvmax)
    args1->p_new[i] = 0.0;

  if (args1->p_new[i] < args1->pmin)
    args1->p_new[i] = args1->pmin;

  return 0;
}
#endif

void CalcPressureForElems(double *p_new, double *bvc, double *pbvc, double *e_old,
              double *compression, double *vnewc, double pmin, double p_cut,
              double eosvmax, int length)
{
  double c1s = 2.0/3.0;
#ifdef OMP_HPX
  struct CalcPressureForElems_helper_args1 args1;
  args1.bvc = bvc;
  args1.c1s = c1s;
  args1.compression = compression;
  args1.pbvc = pbvc;
  hpx_par_for_sync(CalcPressureForElems_par1,0,length,&args1);
#else
  int i;
  for (i = 0; i < length; i++) {
    bvc[i] = c1s*(compression[i] + 1.0);
    pbvc[i] = c1s;
  }
#endif

#ifdef OMP_HPX
  struct CalcPressureForElems_helper_args2 args2;
  args2.p_new = p_new;
  args2.bvc = bvc;
  args2.e_old = e_old;
  args2.p_cut = p_cut;
  args2.vnewc = vnewc;
  args2.eosvmax = eosvmax;
  args2.pmin = pmin;
  hpx_par_for_sync(CalcPressureForElems_par2,0,length,&args2);
#else
  for (i = 0; i < length; i++) {
    p_new[i] = bvc[i]*e_old[i];

    if (fabs(p_new[i]) < p_cut)
      p_new[i] = 0.0;

    if (vnewc[i] >= eosvmax)
      p_new[i] = 0.0;

    if (p_new[i] < pmin)
      p_new[i] = pmin;
  }
#endif
}

#ifdef OMP_HPX
struct CalcEnergyForElems_helper_args1 {
  double *e_new;
  double *e_old;
  double *delvc;
  double *p_old;
  double *q_old;
  double *work;
  double emin;
};

struct CalcEnergyForElems_helper_args2 {
  double *compHalfStep;
  double *delvc;
  double *q_new;
  double *pbvc;
  double *e_new;
  double *bvc;
  double *pHalfStep;
  double rho0;
  double *ql_old;
  double *qq_old;
  double *p_old;
  double *q_old;
};

struct CalcEnergyForElems_helper_args3 {
  double *e_new;
  double *work;
  double e_cut;
  double emin;
};

struct CalcEnergyForElems_helper_args4 {
  double *delvc;
  double *pbvc;
  double *e_new;
  double *vnewc;
  double *bvc;
  double *p_new;
  double rho0;
  double *ql_old;
  double *qq_old;
  double sixth;
  double *p_old;
  double *q_old;
  double *pHalfStep;
  double *q_new;
  double e_cut;
  double emin;
};

struct CalcEnergyForElems_helper_args5 {
  double *delvc;
  double *pbvc;
  double *e_new;
  double *vnewc;
  double *bvc;
  double *p_new;
  double rho0;
  double *q_new;
  double *ql_old;
  double *qq_old;
  double q_cut;
};

static int CalcEnergyForElems_par1(int i, void *args) {
  struct CalcEnergyForElems_helper_args1 *args1 =
                         (struct CalcEnergyForElems_helper_args1 *)args;

  args1->e_new[i] = args1->e_old[i] - 0.5*args1->delvc[i]*(args1->p_old[i] + args1->q_old[i]) + 0.5*args1->work[i];

  if (args1->e_new[i] < args1->emin)
    args1->e_new[i] = args1->emin;

  return 0;
}

static int CalcEnergyForElems_par2(int i, void *args) {
  struct CalcEnergyForElems_helper_args2 *args1 =
                         (struct CalcEnergyForElems_helper_args2 *)args;

  double vhalf = 1.0/(1.0 + args1->compHalfStep[i]);

  if (args1->delvc[i] > 0.0) {
    args1->q_new[i] = 0.0;
  } else {
    double ssc = (args1->pbvc[i]*args1->e_new[i] + vhalf*vhalf*args1->bvc[i]*args1->pHalfStep[i])/args1->rho0;

    if (ssc <= 1.111111e-36) {
      ssc = .333333e-18;
    } else {
      ssc = sqrt(ssc);
    }

    args1->q_new[i] = (ssc*args1->ql_old[i] + args1->qq_old[i]);
  }

  args1->e_new[i] = args1->e_new[i] + 0.5*args1->delvc[i]*(3.0*(args1->p_old[i] + args1->q_old[i])
                             - 4.0*(args1->pHalfStep[i] + args1->q_new[i]));

  return 0;
}

static int CalcEnergyForElems_par3(int i, void *args) {
  struct CalcEnergyForElems_helper_args3 *args1 =
                         (struct CalcEnergyForElems_helper_args3 *)args;

  args1->e_new[i] += 0.5*args1->work[i];

  if (fabs(args1->e_new[i]) < args1->e_cut)
    args1->e_new[i] = 0.0;

  if (args1->e_new[i] < args1->emin)
    args1->e_new[i] = args1->emin;

  return 0;
}

static int CalcEnergyForElems_par4(int i, void *args) {
  struct CalcEnergyForElems_helper_args4 *args1 =
                         (struct CalcEnergyForElems_helper_args4 *)args;

  double q_tilde;

  if (args1->delvc[i] > 0.0) {
    q_tilde = 0.0;
  } else {
    double ssc = (args1->pbvc[i]*args1->e_new[i] + args1->vnewc[i]*args1->vnewc[i]*args1->bvc[i]*args1->p_new[i])/args1->rho0;

    if (ssc <= 1.111111e-36) {
      ssc = .333333e-18;
    } else {
      ssc = sqrt(ssc);
    }

    q_tilde = (ssc*args1->ql_old[i] + args1->qq_old[i]);
  }

  args1->e_new[i] = args1->e_new[i] - args1->delvc[i]*args1->sixth*
    (7.0*(args1->p_old[i] + args1->q_old[i]) - 8.0*(args1->pHalfStep[i] + args1->q_new[i]) + (args1->p_new[i] + q_tilde));

  if (fabs(args1->e_new[i]) < args1->e_cut)
    args1->e_new[i] = 0.0;

  if (args1->e_new[i] < args1->emin)
    args1->e_new[i] = args1->emin;

  return 0;
}

static int CalcEnergyForElems_par5(int i,  void *args) {
  struct CalcEnergyForElems_helper_args5 *args1 =
                         (struct CalcEnergyForElems_helper_args5 *)args;

  if (args1->delvc[i] <= 0.0) {
    double ssc = (args1->pbvc[i]*args1->e_new[i] + args1->vnewc[i]*args1->vnewc[i]*args1->bvc[i]*args1->p_new[i])/args1->rho0;

    if (ssc <= 1.111111e-36) {
      ssc = .333333e-18;
    } else {
      ssc = sqrt(ssc);
    }

    args1->q_new[i] = (ssc*args1->ql_old[i] + args1->qq_old[i]);

    if (fabs(args1->q_new[i]) < args1->q_cut)
      args1->q_new[i] = 0.0;
  }

  return 0;
}
#endif

void CalcEnergyForElems(double *p_new, double *e_new, double *q_new, double *bvc, double *pbvc,
            double *p_old, double *e_old, double *q_old, double *compression,
            double *compHalfStep, double *vnewc, double *work, double *delvc,
            double pmin, double p_cut, double e_cut, double q_cut, double emin,
            double *qq_old, double *ql_old, double rho0, double eosvmax, int length)
{
  const double sixth = 1.0/6.0;
  double *pHalfStep = malloc(sizeof(double)*length);

#ifdef OMP_HPX
  struct CalcEnergyForElems_helper_args1 args1;
  args1.e_new = e_new;
  args1.e_old = e_old;
  args1.delvc = delvc;
  args1.p_old = p_old;
  args1.q_old = q_old;
  args1.work = work;
  args1.emin = emin;
  hpx_par_for_sync(CalcEnergyForElems_par1,0,length,&args1);
#else
  int i;
  for (i = 0; i < length; i++) {
    e_new[i] = e_old[i] - 0.5*delvc[i]*(p_old[i] + q_old[i]) + 0.5*work[i];

    if (e_new[i] < emin)
      e_new[i] = emin;
  }
#endif

  CalcPressureForElems(pHalfStep, bvc, pbvc, e_new, compHalfStep, vnewc, pmin,
               p_cut, eosvmax, length);

#ifdef OMP_HPX
  struct CalcEnergyForElems_helper_args2 args2;
  args2.compHalfStep = compHalfStep;
  args2.delvc = delvc;
  args2.q_new = q_new;
  args2.pbvc = pbvc;
  args2.e_new = e_new;
  args2.bvc = bvc;
  args2.pHalfStep = pHalfStep;
  args2.rho0 = rho0;
  args2.ql_old = ql_old;
  args2.qq_old = qq_old;
  args2.p_old = p_old;
  args2.q_old = q_old;
  hpx_par_for_sync(CalcEnergyForElems_par2,0,length,&args2);
#else
  for (i = 0; i < length; i++) {
    double vhalf = 1.0/(1.0 + compHalfStep[i]);

    if (delvc[i] > 0.0) {
      q_new[i] = 0.0;
    } else {
      double ssc = (pbvc[i]*e_new[i] + vhalf*vhalf*bvc[i]*pHalfStep[i])/rho0;

      if (ssc <= 1.111111e-36) {
    ssc = .333333e-18;
      } else {
    ssc = sqrt(ssc);
      }

      q_new[i] = (ssc*ql_old[i] + qq_old[i]);
    }

    e_new[i] = e_new[i] + 0.5*delvc[i]*(3.0*(p_old[i] + q_old[i]) - 4.0*(pHalfStep[i] + q_new[i]));
  }
#endif

#ifdef OMP_HPX
  struct CalcEnergyForElems_helper_args3 args3;
  args3.e_new = e_new;
  args3.work = work;
  args3.e_cut = e_cut;
  args3.emin = emin;
  hpx_par_for_sync(CalcEnergyForElems_par3,0,length,&args3);
#else
  for (i = 0; i < length; i++) {
    e_new[i] += 0.5*work[i];

    if (fabs(e_new[i]) < e_cut)
      e_new[i] = 0.0;

    if (e_new[i] < emin)
      e_new[i] = emin;
  }
#endif

  CalcPressureForElems(p_new, bvc, pbvc, e_new, compression, vnewc, pmin,
               p_cut, eosvmax, length);

#ifdef OMP_HPX
  struct CalcEnergyForElems_helper_args4 args4;
  args4.delvc = delvc;
  args4.pbvc = pbvc;
  args4.e_new = e_new;
  args4.vnewc = vnewc;
  args4.bvc = bvc;
  args4.p_new = p_new;
  args4.rho0 = rho0;
  args4.ql_old = ql_old;
  args4.qq_old = qq_old;
  args4.sixth = sixth;
  args4.p_old = p_old;
  args4.q_old = q_old;
  args4.pHalfStep = pHalfStep;
  args4.q_new = q_new;
  args4.p_new = p_new;
  args4.e_cut = e_cut;
  args4.emin = emin;
  hpx_par_for_sync(CalcEnergyForElems_par4,0,length,&args4);
#else
  for (i = 0; i < length; i++) {
    double q_tilde;

    if (delvc[i] > 0.0) {
      q_tilde = 0.0;
    } else {
      double ssc = (pbvc[i]*e_new[i] + vnewc[i]*vnewc[i]*bvc[i]*p_new[i])/rho0;

      if (ssc <= 1.111111e-36) {
        ssc = .333333e-18;
      } else {
        ssc = sqrt(ssc);
      }

      q_tilde = (ssc*ql_old[i] + qq_old[i]);
    }

    e_new[i] = e_new[i] - delvc[i]*sixth*
      (7.0*(p_old[i] + q_old[i]) - 8.0*(pHalfStep[i] + q_new[i]) + (p_new[i] + q_tilde));

    if (fabs(e_new[i]) < e_cut)
      e_new[i] = 0.0;

    if (e_new[i] < emin)
      e_new[i] = emin;
  }
#endif

  CalcPressureForElems(p_new, bvc, pbvc, e_new, compression, vnewc, pmin,
               p_cut, eosvmax, length);

#ifdef OMP_HPX
  struct CalcEnergyForElems_helper_args5 args5;
  args5.delvc = delvc;
  args5.pbvc = pbvc;
  args5.e_new = e_new;
  args5.vnewc = vnewc;
  args5.bvc = bvc;
  args5.p_new = p_new;
  args5.rho0 = rho0;
  args5.q_new = q_new;
  args5.ql_old = ql_old;
  args5.qq_old = qq_old;
  args5.q_cut = q_cut;
  hpx_par_for_sync(CalcEnergyForElems_par5,0,length,&args5);
#else
  for (i = 0; i < length; i++) {
    if (delvc[i] <= 0.0) {
      double ssc = (pbvc[i]*e_new[i] + vnewc[i]*vnewc[i]*bvc[i]*p_new[i])/rho0;

      if (ssc <= 1.111111e-36) {
        ssc = .333333e-18;
      } else {
        ssc = sqrt(ssc);
      }

      q_new[i] = (ssc*ql_old[i] + qq_old[i]);

      if (fabs(q_new[i]) < q_cut)
        q_new[i] = 0.0;
    }
  }
#endif

  free(pHalfStep);
}

#ifdef OMP_HPX
struct CalcSoundSpeedForElems_helper_args1 {
  int *matElemlist;
  double *ss;
  double *pbvc;
  double *enewc;
  double *vnewc;
  double *bvc;
  double *pnewc;
  double rho0;
};

static int CalcSoundSpeedForElems_par1(int i, void *args) {
  struct CalcSoundSpeedForElems_helper_args1 *args1 =
                         (struct CalcSoundSpeedForElems_helper_args1 *)args;

  int iz = args1->matElemlist[i];
  double ssTmp = (args1->pbvc[i]*args1->enewc[i] + args1->vnewc[i]*args1->vnewc[i]*args1->bvc[i]*args1->pnewc[i])/args1->rho0;

  if (ssTmp <= 1.111111e-36) {
    ssTmp = .333333e-18;
  } else {
    ssTmp = sqrt(ssTmp);
  }
  args1->ss[iz] = ssTmp;

  return 0;
}
#endif

void CalcSoundSpeedForElems(int *matElemlist, double *ss, double *vnewc, double rho0,
                double *enewc, double *pnewc, double *pbvc, double *bvc,
                double ss4o3, int nz)
{
#ifdef OMP_HPX
  struct CalcSoundSpeedForElems_helper_args1 args1;
  args1.matElemlist = matElemlist;
  args1.ss = ss;
  args1.pbvc = pbvc;
  args1.enewc = enewc;
  args1.vnewc = vnewc;
  args1.bvc = bvc;
  args1.pnewc = pnewc;
  args1.rho0 = rho0;
  hpx_par_for_sync(CalcSoundSpeedForElems_par1,0,nz,&args1);
#else
  int i;
  for (i = 0; i < nz; i++) {
    int iz = matElemlist[i];
    double ssTmp = (pbvc[i]*enewc[i] + vnewc[i]*vnewc[i]*bvc[i]*pnewc[i])/rho0;

    if (ssTmp <= 1.111111e-36) {
      ssTmp = .333333e-18;
    } else {
      ssTmp = sqrt(ssTmp);
    }
    ss[iz] = ssTmp;
  }
#endif
}

#ifdef OMP_HPX
struct EvalEOSForElems_helper_args1 {
  int *matElemlist;
  double *e_old;
  double *delvc;
  double *p_old;
  double *q_old;
  double *qq_old;
  double *ql_old;
  double *e;
  double *delv;
  double *p;
  double *q;
  double *qq;
  double *ql;
};

struct EvalEOSForElems_helper_args2 {
  double *compression;
  double *vnewc;
  double *delvc;
  double *compHalfStep;
};

struct EvalEOSForElems_helper_args3 {
  double *vnewc;
  double *compHalfStep;
  double eosvmin;
  double *compression;

};

struct EvalEOSForElems_helper_args4 {
  double *vnewc;
  double eosvmax;
  double *p_old;
  double *compression;
  double *compHalfStep;
};

struct EvalEOSForElems_helper_args5 {
  double *work;
};

struct EvalEOSForElems_helper_args6 {
  int *matElemlist;
  double *p_new;
  double *e_new;
  double *q_new;
  double *p;
  double *e;
  double *q;
};

static int EvalEOSForElems_par1(int i, void *args) {
  struct EvalEOSForElems_helper_args1 *args1 =
                         (struct EvalEOSForElems_helper_args1 *)args;

  int zidx = args1->matElemlist[i];
  args1->e_old[i] = args1->e[zidx];
  args1->delvc[i] = args1->delv[zidx];
  args1->p_old[i] = args1->p[zidx];
  args1->q_old[i] = args1->q[zidx];
  args1->qq_old[i] = args1->qq[zidx];
  args1->ql_old[i] = args1->ql[zidx];

  return 0;
}

static int EvalEOSForElems_par2(int i, void *args) {
  struct EvalEOSForElems_helper_args2 *args1 =
                         (struct EvalEOSForElems_helper_args2 *)args;

  double vchalf;
  args1->compression[i] = 1.0/args1->vnewc[i] - 1.0;
  vchalf = args1->vnewc[i] - args1->delvc[i]*0.5;
  args1->compHalfStep[i] = 1.0/vchalf - 1.0;

  return 0;
}

static int EvalEOSForElems_par3(int i, void *args) {
  struct EvalEOSForElems_helper_args3 *args1 =
                         (struct EvalEOSForElems_helper_args3 *)args;

  if (args1->vnewc[i] <= args1->eosvmin) {
    args1->compHalfStep[i] = args1->compression[i];
  }

  return 0;
}

static int EvalEOSForElems_par4(int i, void *args) {
  struct EvalEOSForElems_helper_args4 *args1 =
                         (struct EvalEOSForElems_helper_args4 *)args;

  if (args1->vnewc[i] >= args1->eosvmax) {
    args1->p_old[i] = 0.0;
    args1->compression[i] = 0.0;
    args1->compHalfStep[i] = 0.0;
  }

  return 0;
}

static int EvalEOSForElems_par5(int i, void *args) {
  struct EvalEOSForElems_helper_args5 *args1 =
                         (struct EvalEOSForElems_helper_args5 *)args;

  args1->work[i] = 0.0;

  return 0;
}

static int EvalEOSForElems_par6(int i, void *args) {
  struct EvalEOSForElems_helper_args6 *args1 =
                         (struct EvalEOSForElems_helper_args6 *)args;

  int zidx = args1->matElemlist[i];
  args1->p[zidx] = args1->p_new[i];
  args1->e[zidx] = args1->e_new[i];
  args1->q[zidx] = args1->q_new[i];

  return 0;
}
#endif

void EvalEOSForElems(Domain *domain, double *vnewc, int numElem)
{
  double e_cut = domain->e_cut;
  double p_cut = domain->p_cut;
  double ss4o3 = domain->ss4o3;
  double q_cut = domain->q_cut;

  double eosvmax = domain->eosvmax;
  double eosvmin = domain->eosvmin;
  double pmin    = domain->pmin;
  double emin    = domain->emin;
  double rho0    = domain->refdens;

  double *e_old = malloc(sizeof(double)*numElem);
  double *delvc = malloc(sizeof(double)*numElem);
  double *p_old = malloc(sizeof(double)*numElem);
  double *q_old = malloc(sizeof(double)*numElem);
  double *compression = malloc(sizeof(double)*numElem);
  double *compHalfStep = malloc(sizeof(double)*numElem);
  double *qq_old = malloc(sizeof(double)*numElem);
  double *ql_old = malloc(sizeof(double)*numElem);
  double *work = malloc(sizeof(double)*numElem);
  double *p_new = malloc(sizeof(double)*numElem);
  double *e_new = malloc(sizeof(double)*numElem);
  double *q_new = malloc(sizeof(double)*numElem);
  double *bvc = malloc(sizeof(double)*numElem);
  double *pbvc = malloc(sizeof(double)*numElem);

#ifdef OMP_HPX
  struct EvalEOSForElems_helper_args1 args1;
  args1.matElemlist = domain->matElemlist;
  args1.e_old = e_old;
  args1.delvc = delvc;
  args1.p_old = p_old;
  args1.q_old = q_old;
  args1.qq_old = qq_old;
  args1.ql_old = ql_old;
  args1.e = domain->e;
  args1.delv = domain->delv;
  args1.p = domain->p;
  args1.q = domain->q;
  args1.qq = domain->qq;
  args1.ql = domain->ql;
  hpx_par_for_sync(EvalEOSForElems_par1,0,numElem,&args1);
#else
  int i;
  for (i = 0; i < numElem; i++) {
    int zidx = domain->matElemlist[i];
    e_old[i] = domain->e[zidx];
    delvc[i] = domain->delv[zidx];
    p_old[i] = domain->p[zidx];
    q_old[i] = domain->q[zidx];
    qq_old[i] = domain->qq[zidx];
    ql_old[i] = domain->ql[zidx];
  }
#endif

#ifdef OMP_HPX
  struct EvalEOSForElems_helper_args2 args2;
  args2.compression = compression;
  args2.vnewc = vnewc;
  args2.delvc = delvc;
  args2.compHalfStep = compHalfStep;
  hpx_par_for_sync(EvalEOSForElems_par2,0,numElem,&args2);
#else
  for (i = 0; i < numElem; i++) {
    double vchalf;
    compression[i] = 1.0/vnewc[i] - 1.0;
    vchalf = vnewc[i] - delvc[i]*0.5;
    compHalfStep[i] = 1.0/vchalf - 1.0;
  }
#endif

  if (eosvmin != 0.0) {
#ifdef OMP_HPX
    struct EvalEOSForElems_helper_args3 args3;
    args3.vnewc = vnewc;
    args3.compHalfStep = compHalfStep;
    args3.eosvmin = eosvmin;
    args3.compression = compression;
    hpx_par_for_sync(EvalEOSForElems_par3,0,numElem,&args3);
#else
    for (i = 0; i < numElem; i++) {
      if (vnewc[i] <= eosvmin) {
        compHalfStep[i] = compression[i];
      }
    }
#endif
  }

  if (eosvmax != 0.0) {
#ifdef OMP_HPX
    struct EvalEOSForElems_helper_args4 args4;
    args4.vnewc = vnewc;
    args4.eosvmax = eosvmax;
    args4.p_old = p_old;
    args4.compression = compression;
    args4.compHalfStep = compHalfStep;
    hpx_par_for_sync(EvalEOSForElems_par4,0,numElem,&args4);
#else
    for (i = 0; i < numElem; i++) {
      if (vnewc[i] >= eosvmax) {
        p_old[i] = 0.0;
        compression[i] = 0.0;
        compHalfStep[i] = 0.0;
      }
    }
#endif
  }

#ifdef OMP_HPX
  struct EvalEOSForElems_helper_args5 args5;
  args5.work = work;
  hpx_par_for_sync(EvalEOSForElems_par5,0,numElem,&args5);
#else
  for (i = 0; i < numElem; i++)
    work[i] = 0.0;
#endif

  CalcEnergyForElems(p_new, e_new, q_new, bvc, pbvc, p_old, e_old, q_old,
             compression, compHalfStep, vnewc, work, delvc, pmin,
             p_cut, e_cut, q_cut, emin, qq_old, ql_old, rho0,
             eosvmax, numElem);

#ifdef OMP_HPX
  struct EvalEOSForElems_helper_args6 args6;
  args6.matElemlist = domain->matElemlist;
  args6.p_new = p_new;
  args6.e_new = e_new;
  args6.q_new = q_new;
  args6.p = domain->p;
  args6.e = domain->e;
  args6.q = domain->q;
  hpx_par_for_sync(EvalEOSForElems_par6,0,numElem,&args6);
#else
  for (i = 0; i < numElem; i++) {
    int zidx = domain->matElemlist[i];
    domain->p[zidx] = p_new[i];
    domain->e[zidx] = e_new[i];
    domain->q[zidx] = q_new[i];
  }
#endif

  CalcSoundSpeedForElems(domain->matElemlist, domain->ss, vnewc, rho0,
             e_new, p_new, pbvc, bvc, ss4o3, numElem);

  free(pbvc);
  free(bvc);
  free(q_new);
  free(e_new);
  free(p_new);
  free(work);
  free(ql_old);
  free(qq_old);
  free(compHalfStep);
  free(compression);
  free(q_old);
  free(p_old);
  free(delvc);
  free(e_old);
}

#ifdef OMP_HPX
struct ApplyMaterialPropertiesForElems_helper_args1 {
  int *matElemlist;
  double *vnew;
  double *vnewc;
};

struct ApplyMaterialPropertiesForElems_helper_args2 {
  double eosvmin;
  double *vnewc;
};

struct ApplyMaterialPropertiesForElems_helper_args3 {
  double eosvmax;
  double *vnewc;
};

struct ApplyMaterialPropertiesForElems_helper_args4 {
  int *matElemlist;
  double *v;
  double eosvmin;
  double eosvmax;
  int rank;
  int cycle;
};

static int ApplyMaterialPropertiesForElems_par1(int i, void *args) {
  struct ApplyMaterialPropertiesForElems_helper_args1 *args1 =
                         (struct ApplyMaterialPropertiesForElems_helper_args1 *)args;

  int zn = args1->matElemlist[i];
  args1->vnewc[i] = args1->vnew[zn];

  return 0;
}

static int ApplyMaterialPropertiesForElems_par2(int i, void *args) {
  struct ApplyMaterialPropertiesForElems_helper_args2 *args1 =
                         (struct ApplyMaterialPropertiesForElems_helper_args2 *)args;


  if (args1->vnewc[i] < args1->eosvmin)
    args1->vnewc[i] = args1->eosvmin;

  return 0;
}

static int ApplyMaterialPropertiesForElems_par3(int i, void *args) {
  struct ApplyMaterialPropertiesForElems_helper_args3 *args1 =
                         (struct ApplyMaterialPropertiesForElems_helper_args3 *)args;

  if (args1->vnewc[i] > args1->eosvmax)
    args1->vnewc[i] = args1->eosvmax;

  return 0;
}

static int ApplyMaterialPropertiesForElems_par4(int i, void *args) {
  struct ApplyMaterialPropertiesForElems_helper_args4 *args1 =
                         (struct ApplyMaterialPropertiesForElems_helper_args4 *)args;

  int zn = args1->matElemlist[i];
  double vc = args1->v[zn];

  if (args1->eosvmin != 0.0) {
    if (vc < args1->eosvmin)
      vc = args1->eosvmin;
  }

  if (args1->eosvmax != 0.0) {
    if (vc > args1->eosvmax)
      vc = args1->eosvmax;
  }

  if (vc <= 0.0) {
    // need an abort here, temporarily using exit function
    printf("rank %d: ApplyMaterialProperties exit(-1) at cycle %d\n",
       args1->rank, args1->cycle);
    exit(-1);
  }

  return 0;
}
#endif

void ApplyMaterialPropertiesForElems(Domain *domain)
{
  int numElem = domain->numElem;

  if (numElem != 0) {
    double eosvmin = domain->eosvmin;
    double eosvmax = domain->eosvmax;
    double *vnewc = malloc(sizeof(double)*numElem);

#ifdef OMP_HPX
    struct ApplyMaterialPropertiesForElems_helper_args1 args1;
    args1.vnew = domain->vnew;
    args1.vnewc = vnewc;
    args1.matElemlist = domain->matElemlist;
    hpx_par_for_sync(ApplyMaterialPropertiesForElems_par1,0,numElem,&args1);
#else
    int i;
    for (i = 0; i < numElem; i++) {
      int zn = domain->matElemlist[i];
      vnewc[i] = domain->vnew[zn];
    }
#endif

    if (eosvmin != 0.0) {
#ifdef OMP_HPX
      struct ApplyMaterialPropertiesForElems_helper_args2 args2;
      args2.eosvmin = eosvmin;
      args2.vnewc = vnewc;
      hpx_par_for_sync(ApplyMaterialPropertiesForElems_par2,0,numElem,&args2);
#else
      for (i = 0; i < numElem; i++) {
        if (vnewc[i] < eosvmin)
          vnewc[i] = eosvmin;
      }
#endif
    }

    if (eosvmax != 0.0) {
#ifdef OMP_HPX
      struct ApplyMaterialPropertiesForElems_helper_args3 args3;
      args3.eosvmax = eosvmax;
      args3.vnewc = vnewc;
      hpx_par_for_sync(ApplyMaterialPropertiesForElems_par3,0,numElem,&args3);
#else
      for (i = 0; i < numElem; i++) {
        if (vnewc[i] > eosvmax)
          vnewc[i] = eosvmax;
      }
#endif
    }

#ifdef OMP_HPX
      struct ApplyMaterialPropertiesForElems_helper_args4 args4;
      args4.matElemlist = domain->matElemlist;
      args4.v = domain->v;
      args4.eosvmin = eosvmin;
      args4.eosvmax = eosvmax;
      args4.rank = domain->rank;
      args4.cycle = domain->cycle;
      hpx_par_for_sync(ApplyMaterialPropertiesForElems_par4,0,numElem,&args4);
#else
    for (i = 0; i < numElem; i++) {
      int zn = domain->matElemlist[i];
      double vc = domain->v[zn];

      if (eosvmin != 0.0) {
        if (vc < eosvmin)
          vc = eosvmin;
      }

      if (eosvmax != 0.0) {
        if (vc > eosvmax)
          vc = eosvmax;
      }

      if (vc <= 0.0) {
        // need an abort here, temporarily using exit function
        printf("rank %d: ApplyMaterialProperties exit(-1) at cycle %d\n",
           domain->rank, domain->cycle);
        exit(-1);
      }
    }
#endif

    EvalEOSForElems(domain, vnewc, numElem);

    free(vnewc);
  }
}

#ifdef OMP_HPX
struct UpdateVolumesForElems_helper_args {
  double *vnew;
  double *v;
  double v_cut;
};

int UpdateVolumesForElems_par(int i, void *args) {
  struct UpdateVolumesForElems_helper_args *args1 =
                         (struct UpdateVolumesForElems_helper_args *)args;
  double *vnew = args1->vnew;
  double *v = args1->v;
  double v_cut = args1->v_cut;
  double tmpV = vnew[i];

  if (fabs(tmpV - 1.0) < v_cut)
    tmpV = 1.0;

  v[i] = tmpV;
  return 0;
}
#endif

void UpdateVolumesForElems(double *vnew, double *v, double v_cut, int length)
{
  if (length != 0) {
#ifdef OMP_HPX
    struct UpdateVolumesForElems_helper_args args;
    args.vnew = vnew;
    args.v = v;
    args.v_cut = v_cut;
    hpx_par_for_sync(UpdateVolumesForElems_par,0,length,&args);
#else
    int i;
    for (i = 0; i < length; i++) {
      double tmpV = vnew[i];

      if (fabs(tmpV - 1.0) < v_cut)
        tmpV = 1.0;

      v[i] = tmpV;
    }
#endif
  }
}


void CalcTimeConstraintsForElems(Domain *domain)
{
  CalcCourantConstraintForElems(domain->matElemlist, domain->ss,
                domain->vdov, domain->arealg, domain->qqc,
                domain->numElem, &domain->dtcourant);

  CalcHydroConstraintForElems(domain->matElemlist, domain->vdov,
                  domain->dvovmax, domain->numElem, &domain->dthydro);
}

#ifdef OMP_HPX
struct CalcCourantConstraintForElems_helper_args {
  hpx_addr_t rlco;
  int *matElemlist;
  double *ss;
  double *vdov;
  double *arealg;
  double qqc;
};

int CalcCourantConstraintForElems_par(int i, void *args) {
  struct CalcCourantConstraintForElems_helper_args *args1 =
                         (struct CalcCourantConstraintForElems_helper_args *)args;

  double qqc2 = 64.0*args1->qqc*args1->qqc;
  int indx = args1->matElemlist[i];
  double dtf = args1->ss[indx]*args1->ss[indx];
  if (args1->vdov[indx] < 0.0) {
    dtf = dtf + qqc2*args1->arealg[indx]*args1->arealg[indx]*args1->vdov[indx]*args1->vdov[indx];
  }

  dtf = sqrt(dtf);
  dtf = args1->arealg[indx]/dtf;

  if (args1->vdov[indx] != 0.0) {
    reduce_buf_t rbt;
    rbt.min = dtf;
    rbt.idx = indx;
    hpx_lco_set(args1->rlco, sizeof(reduce_buf_t), &rbt, HPX_NULL, HPX_NULL);
  } else {
    // The LCO is expecting length elements in the reduction -- put this in to avoid a potential hang
    reduce_buf_t rbt;
    rbt.min = 1.0e+20;
    rbt.idx = indx;
    hpx_lco_set(args1->rlco, sizeof(reduce_buf_t), &rbt, HPX_NULL, HPX_NULL);
  }

  return 0;
}
#endif

void CalcCourantConstraintForElems(int *matElemlist, double *ss, double *vdov, double *arealg,
                   double qqc, int length, double *dtcourant)
{
  double dtcourant_tmp = 1.0e+20;
  int courant_elem = -1;

#ifdef OMP_HPX
  reduce_buf_t result;
  hpx_addr_t rlco = hpx_lco_reduce_new(length, sizeof(reduce_buf_t), _init,
                                       _min);
  struct CalcCourantConstraintForElems_helper_args args;
  args.rlco = rlco;
  args.matElemlist = matElemlist;
  args.ss = ss;
  args.vdov = vdov;
  args.arealg = arealg;
  args.qqc = qqc;
  hpx_par_for_sync(CalcCourantConstraintForElems_par,0,length,&args);
  hpx_lco_get(rlco,sizeof(reduce_buf_t),&result);
  hpx_lco_delete(rlco,HPX_NULL);

  dtcourant_tmp = result.min;
  courant_elem = result.idx;
#else
  double qqc2 = 64.0*qqc*qqc;
  int i;
  for (i = 0; i < length; i++) {
    int indx = matElemlist[i];
    double dtf = ss[indx]*ss[indx];
    if (vdov[indx] < 0.0) {
      dtf = dtf + qqc2*arealg[indx]*arealg[indx]*vdov[indx]*vdov[indx];
    }

    dtf = sqrt(dtf);
    dtf = arealg[indx]/dtf;

    if (vdov[indx] != 0.0) {
      if (dtf < dtcourant_tmp) {
        dtcourant_tmp = dtf;
        courant_elem = indx;
      }
    }
  }
#endif

  if (courant_elem != -1) {
    *dtcourant = dtcourant_tmp;
  }
}

#ifdef OMP_HPX
struct CalcHydroConstraintForElems_helper_args {
  hpx_addr_t rlco;
  int *matElemlist;
  double *vdov;
  double dvovmax;
};

int CalcHydroConstraintForElems_par(int i, void *args) {
  struct CalcHydroConstraintForElems_helper_args *args1 =
                         (struct CalcHydroConstraintForElems_helper_args *)args;

  int indx = args1->matElemlist[i];

  if (args1->vdov[indx] != 0.0) {
    reduce_buf_t rbt;
    double dtdvov = args1->dvovmax/(fabs(args1->vdov[indx]) + 1.0e-20);
    rbt.min = dtdvov;
    rbt.idx = indx;
    hpx_lco_set(args1->rlco, sizeof(reduce_buf_t), &rbt, HPX_NULL, HPX_NULL);
  } else {
    // The LCO is expecting length elements in the reduction -- put this in to avoid a potential hang
    reduce_buf_t rbt;
    rbt.min = 1.0e+20;
    rbt.idx = indx;
    hpx_lco_set(args1->rlco, sizeof(reduce_buf_t), &rbt, HPX_NULL, HPX_NULL);
  }

  return 0;
}
#endif

void CalcHydroConstraintForElems(int *matElemlist, double *vdov, double dvovmax,
                 int length, double *dthydro)
{
  double dthydro_tmp = 1.0e+20;
  int hydro_elem = -1;

#ifdef OMP_HPX
  reduce_buf_t result;
  hpx_addr_t rlco = hpx_lco_reduce_new(length, sizeof(reduce_buf_t), _init,
                                       _min);
  struct CalcHydroConstraintForElems_helper_args args;
  args.rlco = rlco;
  args.matElemlist = matElemlist;
  args.vdov = vdov;
  args.dvovmax = dvovmax;
  hpx_par_for_sync(CalcHydroConstraintForElems_par,0,length,&args);
  hpx_lco_get(rlco,sizeof(reduce_buf_t),&result);
  hpx_lco_delete(rlco,HPX_NULL);

  dthydro_tmp = result.min;
  hydro_elem = result.idx;
#else
  int i;
  for (i = 0; i < length; i++) {
    int indx = matElemlist[i];

    if (vdov[indx] != 0.0) {
      double dtdvov = dvovmax/(fabs(vdov[indx]) + 1.0e-20);
      if (dthydro_tmp > dtdvov) {
        dthydro_tmp = dtdvov;
        hydro_elem = indx;
      }
    }
  }
#endif

  if (hydro_elem != -1) {
    *dthydro = dthydro_tmp;
  }
}


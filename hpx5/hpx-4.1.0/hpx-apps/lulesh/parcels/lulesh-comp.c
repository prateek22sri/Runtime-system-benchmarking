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

#include "trace.h"
#include "lulesh-hpx.h"

void CalcForceForNodes(Domain *domain,int rank)
{
  int numNode = domain->numNode;
  int i;

  for (i = 0; i < numNode; i++) {
    domain->fx[i] = 0.0;
    domain->fy[i] = 0.0;
    domain->fz[i] = 0.0;
  }

  CalcVolumeForceForElems(domain,rank);
}

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
                domain->fx, domain->fy, domain->fz, sigxx, sigyy, sigzz,
                determ, numElem);
    int k;
    for (k = 0; k < numElem; k++) {
      if (determ[k] <= 0.0) {
    // need an abort here, temporarily using exit function
    printf("rank %d: CalcVolumeForceForElems exit(-1) at cycle %d\n",
           rank, domain->cycle);
    exit(-1);
      }
    }

    CalcHourglassControlForElems(domain, determ, hgcoef);

    free(sigxx);
    free(sigyy);
    free(sigzz);
    free(determ);
  }
}

void InitStressTermsForElems(double *p, double *q, double *sigxx, double *sigyy,
                 double *sigzz, int numElem)
{
  int i;
  for (i = 0; i < numElem; i++) {
    sigxx[i] = sigyy[i] = sigzz[i] = -p[i] - q[i];
  }
}

void IntegrateStressForElems(int *nodelist, double *x, double *y, double *z,
                 double *fx, double *fy, double *fz,
                 double *sigxx, double *sigyy, double *sigzz,
                 double *determ, int numElem)
{
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
}

void CalcHourglassControlForElems(Domain *domain, double determ[], double hgcoef)
{
  int i, ii, jj;
  double x1[8], y1[8], z1[8];
  double pfx[8], pfy[8], pfz[8];
  int numElem = domain->numElem;
  int numElem8 = numElem*8;
  double *dvdx = malloc(sizeof(double)*numElem8);
  double *dvdy = malloc(sizeof(double)*numElem8);
  double *dvdz = malloc(sizeof(double)*numElem8);
  double *x8n = malloc(sizeof(double)*numElem8);
  double *y8n = malloc(sizeof(double)*numElem8);
  double *z8n = malloc(sizeof(double)*numElem8);

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

  if (hgcoef > 0.0) {
    CalcFBHourglassForceForElems(domain->nodelist, domain->ss, domain->elemMass,
                 domain->xd, domain->yd, domain->zd,
                 domain->fx, domain->fy, domain->fz,
                 determ, x8n, y8n, z8n, dvdx, dvdy, dvdz,
                 hgcoef, numElem);
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

void CalcFBHourglassForceForElems(int *nodelist, double *ss, double *elemMass,
                  double *xd, double *yd, double *zd,
                  double *fx, double *fy, double *fz, double *determ,
                  double *x8n, double *y8n, double *z8n,
                  double *dvdx, double *dvdy, double *dvdz,
                  double hourg, int numElem)
{
  double hgfx[8], hgfy[8], hgfz[8];
  double coefficient;

  double gamma[4][8];
  double hourgam0[4], hourgam1[4], hourgam2[4], hourgam3[4];
  double hourgam4[4], hourgam5[4], hourgam6[4], hourgam7[4];
  double xd1[8], yd1[8], zd1[8];

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

void CalcAccelerationForNodes(double *xdd, double *ydd, double *zdd,
                  double *fx, double *fy, double *fz,
                  double *nodalMass, int numNode)
{
  int i;
  for (i = 0; i < numNode; i++) {
    xdd[i] = fx[i]/nodalMass[i];
    ydd[i] = fy[i]/nodalMass[i];
    zdd[i] = fz[i]/nodalMass[i];
  }
}

void ApplyAccelerationBoundaryConditionsForNodes(double *xdd, double *ydd, double *zdd,
                         int *symmX, int *symmY, int *symmZ, int size)
{
  int numNodeBC = (size + 1)*(size + 1);
  int i;

  if (symmX != 0) {
    for (i = 0; i < numNodeBC; i++)
      xdd[symmX[i]] = 0.0;
  }

  if (symmY != 0) {
    for (i = 0; i < numNodeBC; i++)
      ydd[symmY[i]] = 0.0;
  }

  if (symmZ != 0) {
    for (i = 0; i < numNodeBC; i++)
      zdd[symmZ[i]] = 0.0;
  }
}

void CalcVelocityForNodes(double *xd, double *yd, double *zd,
              double *xdd, double *ydd, double *zdd,
              const double dt, const double u_cut, int numNode)
{
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
}

void CalcPositionForNodes(double *x, double *y, double *z,
              double *xd, double *yd, double *zd,
              const double dt, int numNode)
{
  int i;
  for (i = 0; i < numNode; i++) {
    x[i] += xd[i]*dt;
    y[i] += yd[i]*dt;
    z[i] += zd[i]*dt;
  }
}

void LagrangeElements(Domain *domain)
{
  int numElem = domain->numElem;

  domain->vnew = malloc(sizeof(double)*numElem);

  CalcLagrangeElements(domain);

  CalcQForElems(domain);

  ApplyMaterialPropertiesForElems(domain);

  UpdateVolumesForElems(domain->vnew, domain->v, domain->v_cut, numElem);

  free(domain->vnew);
}

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

    free(domain->dzz);
    free(domain->dyy);
    free(domain->dxx);
  }
}

void CalcKinematicsForElems(int *nodelist, double *x, double *y, double *z,
                double *xd, double *yd, double *zd,
                double *dxx, double *dyy, double *dzz,
                double *v, double *volo, double *vnew, double *delv,
                double *arealg, double deltaTime, int numElem)
{
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

void CalcMonotonicQGradientsForElems(double *x, double *y, double *z,
                     double *xd, double *yd, double *zd,
                     double *volo, double *vnew, double *delv_xi,
                     double *delv_eta, double *delv_zeta,
                     double *delx_xi, double *delx_eta, double *delx_zeta,
                     int *nodelist, int numElem)
{
#define SUM4(a,b,c,d) (a + b + c + d)
  const double ptiny = 1.e-36;
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
#undef SUM4
}

void CalcMonotonicQRegionForElems(int *matElemlist, int *elemBC, int *lxim, int *lxip,
                  int *letam, int *letap, int *lzetam, int *lzetap,
                  double *delv_xi, double *delv_eta, double *delv_zeta,
                  double *delx_xi, double *delx_eta, double *delx_zeta,
                  double *vdov, double *volo, double *vnew,
                  double *elemMass, double *qq, double *ql,
                  double qlc_monoq, double qqc_monoq, double monoq_limiter_mult,
                  double monoq_max_slope, double ptiny, int numElem)
{
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

void CalcQForElems(Domain *domain)
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
    TRACE(domain, START_MONOQ_SENDS);
    MonoQ(domain);
    TRACE(domain, START_MONOQ_RECVS);
    hpx_lco_set(domain->monoq_recv, 0, NULL, HPX_NULL, HPX_NULL);
    hpx_lco_wait_reset(domain->monoq_and);
    hpx_lco_reset_sync(domain->monoq_recv);
    CHECK_AFFINITY(domain);
    TRACE(domain, END_MONOQ_RECVS);
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

void CalcPressureForElems(double *p_new, double *bvc, double *pbvc, double *e_old,
              double *compression, double *vnewc, double pmin, double p_cut,
              double eosvmax, int length)
{
  double c1s = 2.0/3.0;
  int i;
  for (i = 0; i < length; i++) {
    bvc[i] = c1s*(compression[i] + 1.0);
    pbvc[i] = c1s;
  }

  for (i = 0; i < length; i++) {
    p_new[i] = bvc[i]*e_old[i];

    if (fabs(p_new[i]) < p_cut)
      p_new[i] = 0.0;

    if (vnewc[i] >= eosvmax)
      p_new[i] = 0.0;

    if (p_new[i] < pmin)
      p_new[i] = pmin;
  }
}

void CalcEnergyForElems(double *p_new, double *e_new, double *q_new, double *bvc, double *pbvc,
            double *p_old, double *e_old, double *q_old, double *compression,
            double *compHalfStep, double *vnewc, double *work, double *delvc,
            double pmin, double p_cut, double e_cut, double q_cut, double emin,
            double *qq_old, double *ql_old, double rho0, double eosvmax, int length)
{
  const double sixth = 1.0/6.0;
  double *pHalfStep = malloc(sizeof(double)*length);

  int i;
  for (i = 0; i < length; i++) {
    e_new[i] = e_old[i] - 0.5*delvc[i]*(p_old[i] + q_old[i]) + 0.5*work[i];

    if (e_new[i] < emin)
      e_new[i] = emin;
  }

  CalcPressureForElems(pHalfStep, bvc, pbvc, e_new, compHalfStep, vnewc, pmin,
               p_cut, eosvmax, length);

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

  for (i = 0; i < length; i++) {
    e_new[i] += 0.5*work[i];

    if (fabs(e_new[i]) < e_cut)
      e_new[i] = 0.0;

    if (e_new[i] < emin)
      e_new[i] = emin;
  }

  CalcPressureForElems(p_new, bvc, pbvc, e_new, compression, vnewc, pmin,
               p_cut, eosvmax, length);

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

  CalcPressureForElems(p_new, bvc, pbvc, e_new, compression, vnewc, pmin,
               p_cut, eosvmax, length);

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

  free(pHalfStep);
}

void CalcSoundSpeedForElems(int *matElemlist, double *ss, double *vnewc, double rho0,
                double *enewc, double *pnewc, double *pbvc, double *bvc,
                double ss4o3, int nz)
{
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
}

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

  for (i = 0; i < numElem; i++) {
    double vchalf;
    compression[i] = 1.0/vnewc[i] - 1.0;
    vchalf = vnewc[i] - delvc[i]*0.5;
    compHalfStep[i] = 1.0/vchalf - 1.0;
  }

  if (eosvmin != 0.0) {
    for (i = 0; i < numElem; i++) {
      if (vnewc[i] <= eosvmin) {
    compHalfStep[i] = compression[i];
      }
    }
  }

  if (eosvmax != 0.0) {
    for (i = 0; i < numElem; i++) {
      if (vnewc[i] >= eosvmax) {
    p_old[i] = 0.0;
    compression[i] = 0.0;
    compHalfStep[i] = 0.0;
      }
    }
  }

  for (i = 0; i < numElem; i++)
    work[i] = 0.0;

  CalcEnergyForElems(p_new, e_new, q_new, bvc, pbvc, p_old, e_old, q_old,
             compression, compHalfStep, vnewc, work, delvc, pmin,
             p_cut, e_cut, q_cut, emin, qq_old, ql_old, rho0,
             eosvmax, numElem);

  for (i = 0; i < numElem; i++) {
    int zidx = domain->matElemlist[i];
    domain->p[zidx] = p_new[i];
    domain->e[zidx] = e_new[i];
    domain->q[zidx] = q_new[i];
  }

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

void ApplyMaterialPropertiesForElems(Domain *domain)
{
  int numElem = domain->numElem;

  if (numElem != 0) {
    double eosvmin = domain->eosvmin;
    double eosvmax = domain->eosvmax;
    double *vnewc = malloc(sizeof(double)*numElem);

    int i;
    for (i = 0; i < numElem; i++) {
      int zn = domain->matElemlist[i];
      vnewc[i] = domain->vnew[zn];
    }

    if (eosvmin != 0.0) {
      for (i = 0; i < numElem; i++) {
    if (vnewc[i] < eosvmin)
      vnewc[i] = eosvmin;
      }
    }

    if (eosvmax != 0.0) {
      for (i = 0; i < numElem; i++) {
    if (vnewc[i] > eosvmax)
      vnewc[i] = eosvmax;
      }
    }

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

    EvalEOSForElems(domain, vnewc, numElem);

    free(vnewc);
  }
}

void UpdateVolumesForElems(double *vnew, double *v, double v_cut, int length)
{
  if (length != 0) {
    int i;
    for (i = 0; i < length; i++) {
      double tmpV = vnew[i];

      if (fabs(tmpV - 1.0) < v_cut)
    tmpV = 1.0;

      v[i] = tmpV;
    }
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


void CalcCourantConstraintForElems(int *matElemlist, double *ss, double *vdov, double *arealg,
                   double qqc, int length, double *dtcourant)
{
  double dtcourant_tmp = 1.0e+20;
  int courant_elem = -1;
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

  if (courant_elem != -1) {
    *dtcourant = dtcourant_tmp;
  }
}

void CalcHydroConstraintForElems(int *matElemlist, double *vdov, double dvovmax,
                 int length, double *dthydro)
{
  double dthydro_tmp = 1.0e+20;
  int hydro_elem = -1;

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

  if (hydro_elem != -1) {
    *dthydro = dthydro_tmp;
  }
}


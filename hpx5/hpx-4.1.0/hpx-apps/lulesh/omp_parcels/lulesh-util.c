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

hpx_addr_t get_domain_addr(hpx_addr_t base, int index) {
  return hpx_addr_add(base, sizeof(Domain) * index, sizeof(Domain));
}

void Init(int tp, int nx)
{
  // setup OFFSET
  OFFSET[0] = -tp*tp - tp - 1;
  OFFSET[1] = -tp*tp - tp;
  OFFSET[2] = -tp*tp - tp + 1;
  OFFSET[3] = -tp*tp - 1;
  OFFSET[4] = -tp*tp;
  OFFSET[5] = -tp*tp + 1;
  OFFSET[6] = -tp*tp + tp - 1;
  OFFSET[7] = -tp*tp + tp;
  OFFSET[8] = -tp*tp + tp + 1;

  OFFSET[9]  = -tp - 1;
  OFFSET[10] = -tp;
  OFFSET[11] = -tp + 1;
  OFFSET[12] = -1;
  OFFSET[13] = 1;
  OFFSET[14] = tp - 1;
  OFFSET[15] = tp;
  OFFSET[16] = tp + 1;

  OFFSET[17] = tp*tp - tp - 1;
  OFFSET[18] = tp*tp - tp;
  OFFSET[19] = tp*tp - tp + 1;
  OFFSET[20] = tp*tp - 1;
  OFFSET[21] = tp*tp;
  OFFSET[22] = tp*tp + 1;
  OFFSET[23] = tp*tp + tp - 1;
  OFFSET[24] = tp*tp + tp;
  OFFSET[25] = tp*tp + tp + 1;

  // setup SENDER and receiver
  SENDER[0] = send1;
  SENDER[1] = send2;
  SENDER[2] = send3;
  SENDER[3] = send4;
  SENDER[4] = send5;
  SENDER[5] = send6;
  SENDER[6] = send7;
  SENDER[7] = send8;
  SENDER[8] = send9;
  SENDER[9] = send10;

  SENDER[10] = send11;
  SENDER[11] = send12;
  SENDER[12] = send13;
  SENDER[13] = send14;
  SENDER[14] = send15;
  SENDER[15] = send16;
  SENDER[16] = send17;
  SENDER[17] = send18;
  SENDER[18] = send19;
  SENDER[19] = send20;

  SENDER[20] = send21;
  SENDER[21] = send22;
  SENDER[22] = send23;
  SENDER[23] = send24;
  SENDER[24] = send25;
  SENDER[25] = send26;

  RECEIVER[0] = recv1;
  RECEIVER[1] = recv2;
  RECEIVER[2] = recv3;
  RECEIVER[3] = recv4;
  RECEIVER[4] = recv5;
  RECEIVER[5] = recv6;
  RECEIVER[6] = recv7;
  RECEIVER[7] = recv8;
  RECEIVER[8] = recv9;
  RECEIVER[9] = recv10;

  RECEIVER[10] = recv11;
  RECEIVER[11] = recv12;
  RECEIVER[12] = recv13;
  RECEIVER[13] = recv14;
  RECEIVER[14] = recv15;
  RECEIVER[15] = recv16;
  RECEIVER[16] = recv17;
  RECEIVER[17] = recv18;
  RECEIVER[18] = recv19;
  RECEIVER[19] = recv20;

  RECEIVER[20] = recv21;
  RECEIVER[21] = recv22;
  RECEIVER[22] = recv23;
  RECEIVER[23] = recv24;
  RECEIVER[24] = recv25;
  RECEIVER[25] = recv26;

  // setup buffer size
  MAXPLANESIZE = CACHE_ALIGN_REAL((nx + 1)*(nx + 1));
  MAXEDGESIZE = CACHE_ALIGN_REAL(nx + 1);

  int allocsz = MAXPLANESIZE*MAX_FIELDS_PER_MPI_COMM*sizeof(double);
  BUFSZ[4] = BUFSZ[21] = BUFSZ[10] = allocsz;
  BUFSZ[15] = BUFSZ[12] = BUFSZ[13] = allocsz;

  allocsz = MAXEDGESIZE*MAX_FIELDS_PER_MPI_COMM*sizeof(double);
  BUFSZ[9] = BUFSZ[1] = BUFSZ[3] = BUFSZ[16] = allocsz;
  BUFSZ[24] = BUFSZ[22] = BUFSZ[14] = BUFSZ[18] = allocsz;
  BUFSZ[20] = BUFSZ[11] = BUFSZ[7] = BUFSZ[5] = allocsz;

  allocsz = CACHE_COHERENCE_PAD_REAL*sizeof(double);
  BUFSZ[0] = BUFSZ[17] = BUFSZ[2] = BUFSZ[19] = allocsz;
  BUFSZ[6] = BUFSZ[23] = BUFSZ[8] = BUFSZ[25] = allocsz;

  // setup XFERCNT
  XFERCNT[4] = XFERCNT[21] = XFERCNT[10] = (nx + 1)*(nx + 1);
  XFERCNT[15] = XFERCNT[12] = XFERCNT[13] = (nx + 1)*(nx + 1);;

  XFERCNT[9] = XFERCNT[1] = XFERCNT[3] = XFERCNT[16] = nx + 1;
  XFERCNT[24] = XFERCNT[22] = XFERCNT[14] = XFERCNT[18] = nx + 1;
  XFERCNT[20] = XFERCNT[11] = XFERCNT[7] = XFERCNT[5] = nx + 1;

  XFERCNT[0] = XFERCNT[17] = XFERCNT[2] = XFERCNT[19] = 1;
  XFERCNT[6] = XFERCNT[23] = XFERCNT[8] = XFERCNT[25] = 1;
}

void SetDomain(int rank, int colLoc, int rowLoc, int planeLoc, int nx, int tp,
           int nDomains, int maxcycles,Domain *domain)
{
  double tx, ty, tz;
  int nidx, zidx, pidx;
  int ghostIdx[6];
  int i, j, plane, row, col, lnode;

  int edgeElems = nx;
  int edgeNodes = edgeElems + 1;
  int meshEdgeElems = tp*nx;

  domain->colLoc = colLoc;
  domain->rowLoc = rowLoc;
  domain->planeLoc = planeLoc;
  domain->tp = tp;
  domain->rank = rank;
  domain->nDomains = nDomains;
  domain->maxcycles = maxcycles;

  domain->sizeX = edgeElems;
  domain->sizeY = edgeElems;
  domain->sizeZ = edgeElems;
  domain->numElem = edgeElems*edgeElems*edgeElems;
  domain->numNode = edgeNodes*edgeNodes*edgeNodes;
  int domElems = domain->numElem;
  int domNodes = domain->numNode;

  int allElem = domain->numElem +
       2*domain->sizeX*domain->sizeY +
       2*domain->sizeX*domain->sizeZ +
       2*domain->sizeY*domain->sizeZ;

  domain->delv_xi = calloc(1, sizeof(double)*allElem);
  domain->delv_eta = calloc(1, sizeof(double)*allElem);
  domain->delv_zeta = calloc(1, sizeof(double)*allElem);

  domain->delx_xi = calloc(1, sizeof(double)*domain->numElem);
  domain->delx_eta = calloc(1, sizeof(double)*domain->numElem);
  domain->delx_zeta = calloc(1, sizeof(double)*domain->numElem);

  // Elem-centered
  domain->matElemlist = calloc(1, sizeof(int)*domElems);
  domain->nodelist = calloc(1, sizeof(int)*8*domElems);

  domain->lxim   = calloc(1, sizeof(int)*domElems);
  domain->lxip   = calloc(1, sizeof(int)*domElems);
  domain->letam  = calloc(1, sizeof(int)*domElems);
  domain->letap  = calloc(1, sizeof(int)*domElems);
  domain->lzetam = calloc(1, sizeof(int)*domElems);
  domain->lzetap = calloc(1, sizeof(int)*domElems);

  domain->elemBC = calloc(1, sizeof(int)*domElems);

  domain->e        = calloc(1, sizeof(double)*domElems);
  domain->p        = calloc(1, sizeof(double)*domElems);
  domain->q        = calloc(1, sizeof(double)*domElems);
  domain->ql       = calloc(1, sizeof(double)*domElems);
  domain->qq       = calloc(1, sizeof(double)*domElems);
  domain->v        = calloc(1, sizeof(double)*domElems);
  domain->volo     = calloc(1, sizeof(double)*domElems);
  domain->delv     = calloc(1, sizeof(double)*domElems);
  domain->vdov     = calloc(1, sizeof(double)*domElems);
  domain->arealg   = calloc(1, sizeof(double)*domElems);
  domain->ss       = calloc(1, sizeof(double)*domElems);
  domain->elemMass = calloc(1, sizeof(double)*domElems);

  // Node-centered
  domain->x         = calloc(1, sizeof(double)*domNodes);
  domain->y         = calloc(1, sizeof(double)*domNodes);
  domain->z         = calloc(1, sizeof(double)*domNodes);
  domain->xd        = calloc(1, sizeof(double)*domNodes);
  domain->yd        = calloc(1, sizeof(double)*domNodes);
  domain->zd        = calloc(1, sizeof(double)*domNodes);
  domain->xdd       = calloc(1, sizeof(double)*domNodes);
  domain->ydd       = calloc(1, sizeof(double)*domNodes);
  domain->zdd       = calloc(1, sizeof(double)*domNodes);
  domain->fx        = calloc(1, sizeof(double)*domNodes);
  domain->fy        = calloc(1, sizeof(double)*domNodes);
  domain->fz        = calloc(1, sizeof(double)*domNodes);
  domain->nodalMass = calloc(1, sizeof(double)*domNodes);

  int rowMin, rowMax, colMin, colMax, planeMin, planeMax;
  rowMin = rowMax = colMin = colMax = planeMin = planeMax = 1;

  if (rowLoc == 0)
    rowMin = 0;

  if (rowLoc == tp - 1)
    rowMax = 0;

  if (colLoc == 0)
    colMin = 0;

  if (colLoc == tp - 1)
    colMax = 0;

  if (planeLoc == 0)
    planeMin = 0;

  if (planeLoc == tp - 1)
    planeMax = 0;

  // boundary nodesets
  domain->symmX = (colLoc == 0) ? calloc(1, sizeof(int)*edgeNodes*edgeNodes) : NULL;
  domain->symmY = (rowLoc == 0) ? calloc(1, sizeof(int)*edgeNodes*edgeNodes) : NULL;
  domain->symmZ = (planeLoc == 0) ? calloc(1, sizeof(int)*edgeNodes*edgeNodes) : NULL;

  // basic field initialization
  for (i = 0; i < domElems; i++) {
    domain->e[i] = 0.0;
    domain->p[i] = 0.0;
    domain->v[i] = 1.0;
  }

  for (i = 0; i < domNodes; i++) {
    domain->xd[i] = 0.0;
    domain->yd[i] = 0.0;
    domain->zd[i] = 0.0;

    domain->xdd[i] = 0.0;
    domain->ydd[i] = 0.0;
    domain->zdd[i] = 0.0;
  }

  // initialize nodal coordinates
  nidx = 0;
  tz = 1.125*planeLoc*nx/meshEdgeElems;
  for (plane = 0; plane < edgeNodes; ++plane) {
    ty = 1.125*rowLoc*nx/meshEdgeElems;
    for (row = 0; row < edgeNodes; ++row) {
      tx = 1.125*colLoc*nx/meshEdgeElems;
      for (col = 0; col < edgeNodes; ++col) {
    domain->x[nidx] = tx;
    domain->y[nidx] = ty;
    domain->z[nidx] = tz;
    ++nidx;
    tx = 1.125*(colLoc*nx + col + 1)/meshEdgeElems;
      }
      ty = 1.125*(rowLoc*nx + row + 1)/meshEdgeElems;
    }
    tz = 1.125*(planeLoc*nx + plane + 1)/meshEdgeElems;
  }

  // embed hexehedral elements in nodal point lattice
  nidx = 0;
  zidx = 0;
  for (plane = 0; plane < edgeElems; ++plane) {
    for (row = 0; row < edgeElems; ++row) {
      for (col = 0; col < edgeElems; ++col) {
    int *localNode = &domain->nodelist[8*zidx];
    localNode[0] = nidx;
    localNode[1] = nidx + 1;
    localNode[2] = nidx + edgeNodes + 1;
    localNode[3] = nidx + edgeNodes;
    localNode[4] = nidx + edgeNodes*edgeNodes;
    localNode[5] = nidx + edgeNodes*edgeNodes + 1;
    localNode[6] = nidx + edgeNodes*edgeNodes + edgeNodes + 1;
    localNode[7] = nidx + edgeNodes*edgeNodes + edgeNodes;
    ++zidx;
    ++nidx;
      }
      ++nidx;
    }
    nidx += edgeNodes;
  }

  // OMP_HPX fix
  {
    domain->nodeElemCount = calloc(1, sizeof(int)*domNodes);
    for (i=0;i<domNodes;i++) {
      domain->nodeElemCount[i] = 0;
    }

    for (i=0;i<domElems;i++) {
      int *nl = &domain->nodelist[8*i] ;
      for (j=0; j < 8; ++j) {
         ++(domain->nodeElemCount[nl[j]] );
      }
    }
    domain->nodeElemStart = calloc(1, sizeof(int)*domNodes);

    domain->nodeElemStart[0] = 0;

    for (i=1; i < domNodes; ++i) {
      domain->nodeElemStart[i] =
             domain->nodeElemStart[i-1] + domain->nodeElemCount[i-1] ;
    }

    domain->nodeElemCornerList = calloc(1,sizeof(int)*(domain->nodeElemStart[domNodes-1] +
                                                       domain->nodeElemCount[domNodes-1]));

    for (i=0; i < domNodes; ++i) {
      domain->nodeElemCount[i] = 0;
    }

    for (i=0; i < domElems; ++i) {
      int *nl = &domain->nodelist[i*8] ;
      for (j=0; j < 8; ++j) {
        int m = nl[j];
        int k = i*8 + j ;
        int offset = domain->nodeElemStart[m] +
                              domain->nodeElemCount[m] ;
        domain->nodeElemCornerList[offset] = k;
        ++(domain->nodeElemCount[m]) ;
      }
    }

    int clSize = domain->nodeElemStart[domNodes-1] +
                        domain->nodeElemCount[domNodes-1] ;
    for (i=0; i < clSize; ++i) {
      int clv = domain->nodeElemCornerList[i] ;
      if ((clv < 0) || (clv > domElems*8)) {
               fprintf(stderr,
        "AllocateNodeElemIndexes(): nodeElemCornerList entry out of range!\n");
               exit(1);
      }
    }
  }

  // create a material indexset
  for (i = 0; i < domElems; ++i) {
    domain->matElemlist[i] = i;
  }

  // initialize material parameters
  domain->dtfixed = -1.0e-7;
  domain->deltatime = 1.0e-7;
  domain->deltatimemultlb = 1.1;
  domain->deltatimemultub = 1.2;
  domain->stoptime = 1.0e-2*edgeElems*tp/45.0;
  domain->dtcourant = 1.0e+20;
  domain->dthydro = 1.0e+20;
  domain->dtmax = 1.0e-2;
  domain->time = 0.0;
  domain->cycle = 0;

  domain->e_cut = 1.0e-7;
  domain->p_cut = 1.0e-7;
  domain->q_cut = 1.0e-7;
  domain->u_cut = 1.0e-7;
  domain->v_cut = 1.0e-7;

  domain->hgcoef = 3.0;
  domain->ss4o3 = 4.0/3.0;

  domain->qstop = 1.0e+12;
  domain->monoq_max_slope = 1.0;
  domain->monoq_limiter_mult = 2.0;
  domain->qlc_monoq = 0.5;
  domain->qqc_monoq = 2.0/3.0;
  domain->qqc = 2.0;

  domain->pmin = 0.0;
  domain->emin = -1.0e+15;

  domain->dvovmax = 0.1;

  domain->eosvmax = 1.0e+9;
  domain->eosvmin = 1.0e-9;

  domain->refdens = 1.0;

  // initialize field data
  for (i = 0; i < domNodes; i++) {
    domain->nodalMass[i] = 0.0;
  }

  for (i = 0; i < domElems; i++) {
    double x_local[8], y_local[8], z_local[8];
    int *elemToNode = &domain->nodelist[8*i];
    for (lnode = 0; lnode < 8; lnode++) {
      int gnode = elemToNode[lnode];
      x_local[lnode] = domain->x[gnode];
      y_local[lnode] = domain->y[gnode];
      z_local[lnode] = domain->z[gnode];
    }

    double volume = CalcElemVolume(x_local, y_local, z_local);
    domain->volo[i] = volume;
    domain->elemMass[i] = volume;

    for (j = 0; j < 8; j++) {
      int idx = elemToNode[j];
      domain->nodalMass[idx] += volume / (double) 8.0;
    }
  }

  // deposit energy
  if (rowLoc + colLoc + planeLoc == 0) {
    domain->e[0] = (double) 3.948746e+7;
  }

  // setup symmetry nodesets
  nidx = 0;
  for (i = 0; i < edgeNodes; i++) {
    int planeInc = i*edgeNodes*edgeNodes;
    int rowInc = i*edgeNodes;
    for (j = 0; j < edgeNodes; j++) {
      if (planeLoc == 0) {
    domain->symmZ[nidx] = rowInc + j;
      }

      if (rowLoc == 0) {
    domain->symmY[nidx] = planeInc + j;
      }

      if (colLoc == 0) {
    domain->symmX[nidx] = planeInc + j*edgeNodes;
      }

      ++nidx;
    }
  }

  // setup element connectivity information
  domain->lxim[0] = 0;
  for (i = 1; i < domElems; i++) {
    domain->lxim[i] = i - 1;
    domain->lxip[i - 1] = i;
  }
  domain->lxip[domElems - 1] = domElems - 1;

  for (i = 0; i < edgeElems; i++) {
    domain->letam[i] = i;
    domain->letap[domElems - edgeElems + i] = domElems - edgeElems + i;
  }

  for (i = edgeElems; i < domElems; i++) {
    domain->letam[i] = i - edgeElems;
    domain->letap[i - edgeElems] = i;
  }

  for (i = 0; i < edgeElems*edgeElems; i++) {
    domain->lzetam[i] = i;
    domain->lzetap[domElems - edgeElems*edgeElems + i] = domElems - edgeElems*edgeElems + i;
  }

  for (i = edgeElems*edgeElems; i < domElems; i++) {
    domain->lzetam[i] = i - edgeElems*edgeElems;
    domain->lzetap[i - edgeElems*edgeElems] = i;
  }

  // setup boundary condition information
  for (i = 0; i < domElems; i++) {
    domain->elemBC[i] = 0;
  }

  for (i = 0; i < 6; i++) {
    ghostIdx[i] = INT_MIN;
  }

  pidx = domElems;
  if (planeMin != 0) {
    ghostIdx[0] = pidx;
    pidx += domain->sizeX*domain->sizeY;
  }

  if (planeMax != 0) {
    ghostIdx[1] = pidx;
    pidx += domain->sizeX*domain->sizeY;
  }

  if (rowMin != 0) {
    ghostIdx[2] = pidx;
    pidx += domain->sizeX*domain->sizeZ;
  }

  if (rowMax != 0 ) {
    ghostIdx[3] = pidx;
    pidx += domain->sizeX*domain->sizeZ;
  }

  if (colMin != 0) {
    ghostIdx[4] = pidx;
    pidx += domain->sizeY*domain->sizeZ;
  }

  if (colMax != 0) {
    ghostIdx[5] = pidx;
  }

  // symmetry plane or free surface BCs
  for (i = 0; i < edgeElems; i++) {
    int planeInc = i*edgeElems*edgeElems;
    int rowInc = i*edgeElems;
    for (j = 0; j < edgeElems; j++) {
      if (planeLoc == 0) {
    domain->elemBC[rowInc + j] |= ZETA_M_SYMM;
      } else {
    domain->elemBC[rowInc + j] |= ZETA_M_COMM;
    domain->lzetam[rowInc + j] = ghostIdx[0] + rowInc + j;
      }

      if (planeLoc == tp - 1) {
    domain->elemBC[rowInc + j + domElems - edgeElems*edgeElems] |= ZETA_P_FREE;
      } else {
    domain->elemBC[rowInc + j + domElems - edgeElems*edgeElems] |= ZETA_P_COMM;
    domain->lzetap[rowInc + j + domElems - edgeElems*edgeElems] = ghostIdx[1] + rowInc + j;
      }

      if (rowLoc == 0) {
    domain->elemBC[planeInc + j] |= ETA_M_SYMM;
      } else {
    domain->elemBC[planeInc + j] |= ETA_M_COMM;
    domain->letam[planeInc + j] = ghostIdx[2] + rowInc + j;
      }

      if (rowLoc == tp - 1) {
    domain->elemBC[planeInc + j + edgeElems*edgeElems - edgeElems] |= ETA_P_FREE;
      } else {
    domain->elemBC[planeInc + j + edgeElems*edgeElems - edgeElems] |= ETA_P_COMM;
    domain->letap[planeInc + j + edgeElems*edgeElems - edgeElems] = ghostIdx[3] + rowInc + j;
      }

      if (colLoc == 0) {
    domain->elemBC[planeInc + j*edgeElems] |= XI_M_SYMM;
      } else {
    domain->elemBC[planeInc + j*edgeElems] |= XI_M_COMM;
    domain->lxim[planeInc + j*edgeElems] = ghostIdx[4] + rowInc + j;
      }

      if (colLoc == tp - 1) {
    domain->elemBC[planeInc + j*edgeElems + edgeElems - 1] |= XI_P_FREE;
      } else {
    domain->elemBC[planeInc + j*edgeElems + edgeElems - 1] |= XI_P_COMM;
    domain->lxip[planeInc + j*edgeElems + edgeElems - 1] = ghostIdx[5] + rowInc + j;
      }
    }
  }

  // there are three communication patterns in the lulesh code.
  // 1) communicate with all 26 surrounding domains and do aggregate operation (sbn)
  // 2) one-way communication, send information to the domains that are adjacent to the
  // three faces of the domain that are joined at the lower left corner, recv information
  // from the domains that are adjacent to the three faces of the domain that are joined
  // at the upper right corner.
  // 3) communicate with all the face-adjacent domains.

  // setup communications between domains
  int nsTF, nsTT, nsFF, nrTF, nrTT, nrFF;
  nsTF = nsTT = nsFF = nrTF = nrTT = nrFF = 0;

  int *sendTF = &domain->sendTF[1];
  int *recvTF = &domain->recvTF[1];
  int *sendTT = &domain->sendTT[1];
  int *recvTT = &domain->recvTT[1];
  int *sendFF = &domain->sendFF[1];
  int *recvFF = &domain->recvFF[1];

  int PLANEBUFFERSZ = MAXPLANESIZE*MAX_FIELDS_PER_MPI_COMM*sizeof(double);
  int EDGEBUFFERSZ = MAXEDGESIZE*MAX_FIELDS_PER_MPI_COMM*sizeof(double);
  int CORNERBUFFERSZ = CACHE_COHERENCE_PAD_REAL;
  //int FUTURESZ = maxcycles*sizeof(hpx_future_t);

  if (planeMin) {
    // #5
    domain->dataRecvTF[4] = calloc(1, PLANEBUFFERSZ);
    domain->dataRecvTT[4] = calloc(1, PLANEBUFFERSZ);
    recvTF[nrTF++] = 4;
    recvTT[nrTT++] = 4;

   // domain->dataSendTF[4] = calloc(1, FUTURESZ);
   // domain->dataSendTT[4] = calloc(1, FUTURESZ);
   // domain->dataSendFF[4] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 4;
    sendTT[nsTT++] = 4;
    sendFF[nsFF++] = 4;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[4][i]);
    //  hpx_lco_future_init(&domain->dataSendTT[4][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[4][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[4]);
  }

  if (planeMax) {
    // #22
    domain->dataRecvTF[21] = calloc(1, PLANEBUFFERSZ);
    domain->dataRecvTT[21] = calloc(1, PLANEBUFFERSZ);
    domain->dataRecvFF[21] = calloc(1, PLANEBUFFERSZ);
    recvTF[nrTF++] = 21;
    recvTT[nrTT++] = 21;
    recvFF[nrFF++] = 21;

    //domain->dataSendTF[21] = calloc(1, FUTURESZ);
    //domain->dataSendTT[21] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 21;
    sendTT[nsTT++] = 21;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[21][i]);
    //  hpx_lco_future_init(&domain->dataSendTT[21][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[21]);
  }

  if (rowMin) {
    // #11
    domain->dataRecvTF[10] = calloc(1, PLANEBUFFERSZ);
    domain->dataRecvTT[10] = calloc(1, PLANEBUFFERSZ);
    recvTF[nrTF++] = 10;
    recvTT[nrTT++] = 10;

    //domain->dataSendTF[10] = calloc(1, FUTURESZ);
    //domain->dataSendTT[10] = calloc(1, FUTURESZ);
    //domain->dataSendFF[10] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 10;
    sendTT[nsTT++] = 10;
    sendFF[nsFF++] = 10;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[10][i]);
    //  hpx_lco_future_init(&domain->dataSendTT[10][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[10][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[10]);
  }

  if (rowMax) {
    // #16
    domain->dataRecvTF[15] = calloc(1, PLANEBUFFERSZ);
    domain->dataRecvTT[15] = calloc(1, PLANEBUFFERSZ);
    domain->dataRecvFF[15] = calloc(1, PLANEBUFFERSZ);
    recvTF[nrTF++] = 15;
    recvTT[nrTT++] = 15;
    recvFF[nrFF++] = 15;

    //domain->dataSendTF[15] = calloc(1, FUTURESZ);
    //domain->dataSendTT[15] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 15;
    sendTT[nsTT++] = 15;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[15][i]);
    //  hpx_lco_future_init(&domain->dataSendTT[15][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[15]);
  }

  if (colMin) {
    // #13
    domain->dataRecvTF[12] = calloc(1, PLANEBUFFERSZ);
    domain->dataRecvTT[12] = calloc(1, PLANEBUFFERSZ);
    recvTF[nrTF++] = 12;
    recvTT[nrTT++] = 12;

    //domain->dataSendTF[12] = calloc(1, FUTURESZ);
    //domain->dataSendTT[12] = calloc(1, FUTURESZ);
    //domain->dataSendFF[12] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 12;
    sendTT[nsTT++] = 12;
    sendFF[nsFF++] = 12;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[12][i]);
    //  hpx_lco_future_init(&domain->dataSendTT[12][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[12][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[12]);
  }

  if (colMax) {
    // #14
    domain->dataRecvTF[13] = calloc(1, PLANEBUFFERSZ);
    domain->dataRecvTT[13] = calloc(1, PLANEBUFFERSZ);
    domain->dataRecvFF[13] = calloc(1, PLANEBUFFERSZ);
    recvTF[nrTF++] = 13;
    recvTT[nrTT++] = 13;
    recvFF[nrFF++] = 13;

    //domain->dataSendTF[13] = calloc(1, FUTURESZ);
    //domain->dataSendTT[13] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 13;
    sendTT[nsTT++] = 13;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[13][i]);
    //  hpx_lco_future_init(&domain->dataSendTT[13][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[13]);
  }

  if (rowMin & colMin) {
    // #10
    domain->dataRecvTF[9] = calloc(1, EDGEBUFFERSZ);
    recvTF[nrTF++] = 9;

    //domain->dataSendTF[9] = calloc(1, FUTURESZ);
    //domain->dataSendFF[9] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 9;
    sendFF[nsFF++] = 9;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[9][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[9][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[9]);
  }


  if (rowMin & planeMin) {
    // #2
    domain->dataRecvTF[1] = calloc(1, EDGEBUFFERSZ);
    recvTF[nrTF++] = 1;

    //domain->dataSendTF[1] = calloc(1, FUTURESZ);
    //domain->dataSendFF[1] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 1;
    sendFF[nsFF++] = 1;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[1][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[1][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[1]);
  }

  if (colMin & planeMin) {
    // #4
    domain->dataRecvTF[3] = calloc(1, EDGEBUFFERSZ);
    recvTF[nrTF++] = 3;

    //domain->dataSendTF[3] = calloc(1, FUTURESZ);
    //domain->dataSendFF[3] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 3;
    sendFF[nsFF++] = 3;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[3][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[3][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[3]);
  }

  if (rowMax & colMax) {
    // #17
    domain->dataRecvTF[16] = calloc(1, EDGEBUFFERSZ);
    domain->dataRecvFF[16] = calloc(1, EDGEBUFFERSZ);
    recvTF[nrTF++] = 16;
    recvFF[nrFF++] = 16;

    //domain->dataSendTF[16] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 16;

    //for (i = 0; i < maxcycles; i++)
    //  hpx_lco_future_init(&domain->dataSendTF[16][i]);

    //hpx_lco_future_init(&domain->SBN1[16]);
  }

  if (rowMax & planeMax) {
    // #25
    domain->dataRecvTF[24] = calloc(1, EDGEBUFFERSZ);
    domain->dataRecvFF[24] = calloc(1, EDGEBUFFERSZ);
    recvTF[nrTF++] = 24;
    recvFF[nrFF++] = 24;

    //domain->dataSendTF[24] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 24;

    //for (i = 0; i < maxcycles; i++)
    //  hpx_lco_future_init(&domain->dataSendTF[24][i]);

    //hpx_lco_future_init(&domain->SBN1[24]);
  }


  if (colMax & planeMax) {
    // #23
    domain->dataRecvTF[22] = calloc(1, EDGEBUFFERSZ);
    domain->dataRecvFF[22] = calloc(1, EDGEBUFFERSZ);
    recvTF[nrTF++] = 22;
    recvFF[nrFF++] = 22;

    //domain->dataSendTF[22] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 22;

    //for (i = 0; i < maxcycles; i++)
    //  hpx_lco_future_init(&domain->dataSendTF[22][i]);

    //hpx_lco_future_init(&domain->SBN1[22]);
  }

  if (rowMax & colMin) {
    // #15
    domain->dataRecvTF[14] = calloc(1, EDGEBUFFERSZ);
    domain->dataRecvFF[14] = calloc(1, EDGEBUFFERSZ);
    recvTF[nrTF++] = 14;
    recvFF[nrFF++] = 14;

    //domain->dataSendTF[14] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 14;

    //for (i = 0; i < maxcycles; i++)
    //  hpx_lco_future_init(&domain->dataSendTF[14][i]);

    //hpx_lco_future_init(&domain->SBN1[14]);
  }

  if (rowMin & planeMax) {
    // #19
    domain->dataRecvTF[18] = calloc(1, EDGEBUFFERSZ);
    domain->dataRecvFF[18] = calloc(1, EDGEBUFFERSZ);
    recvTF[nrTF++] = 18;
    recvFF[nrFF++] = 18;

    //domain->dataSendTF[18] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 18;

    //for (i = 0; i < maxcycles; i++)
    //  hpx_lco_future_init(&domain->dataSendTF[18][i]);

    //hpx_lco_future_init(&domain->SBN1[18]);
  }

  if (colMin & planeMax) {
    // #21
    domain->dataRecvTF[20] = calloc(1, EDGEBUFFERSZ);
    domain->dataRecvFF[20] = calloc(1, EDGEBUFFERSZ);
    recvTF[nrTF++] = 20;
    recvFF[nrFF++] = 20;

    //domain->dataSendTF[20] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 20;

    //for (i = 0; i < maxcycles; i++)
    //  hpx_lco_future_init(&domain->dataSendTF[20][i]);

    //hpx_lco_future_init(&domain->SBN1[20]);
  }

  if (rowMin & colMax) {
    // #12
    domain->dataRecvTF[11] = calloc(1, EDGEBUFFERSZ);
    recvTF[nrTF++] = 11;

    //domain->dataSendTF[11] = calloc(1, FUTURESZ);
    //domain->dataSendFF[11] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 11;
    sendFF[nsFF++] = 11;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[11][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[11][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[11]);
  }

  if (rowMax & planeMin) {
    // #8
    domain->dataRecvTF[7] = calloc(1, EDGEBUFFERSZ);
    recvTF[nrTF++] = 7;

    //domain->dataSendTF[7] = calloc(1, FUTURESZ);
    //domain->dataSendFF[7] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 7;
    sendFF[nsFF++] = 7;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[7][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[7][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[7]);
  }

  if (colMax & planeMin) {
    // #6
    domain->dataRecvTF[5] = calloc(1, EDGEBUFFERSZ);
    recvTF[nrTF++] = 5;

    //domain->dataSendTF[5] = calloc(1, FUTURESZ);
    //domain->dataSendFF[5] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 5;
    sendFF[nsFF++] = 5;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[5][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[5][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[5]);
  }

  if (rowMin & colMin & planeMin) {
    // #1
    domain->dataRecvTF[0] = calloc(1, CORNERBUFFERSZ);
    recvTF[nrTF++] = 0;

    //domain->dataSendTF[0] = calloc(1, FUTURESZ);
    //domain->dataSendFF[0] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 0;
    sendFF[nsFF++] = 0;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[0][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[0][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[0]);
  }

  if (rowMin & colMin & planeMax) {
    // #18
    domain->dataRecvTF[17] = calloc(1, CORNERBUFFERSZ);
    domain->dataRecvFF[17] = calloc(1, CORNERBUFFERSZ);
    recvTF[nrTF++] = 17;
    recvFF[nrFF++] = 17;

    //domain->dataSendTF[17] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 17;

    //for (i = 0; i < maxcycles; i++)
    //  hpx_lco_future_init(&domain->dataSendTF[17][i]);

    //hpx_lco_future_init(&domain->SBN1[17]);
  }

  if (rowMin & colMax & planeMin) {
    // #3
    domain->dataRecvTF[2] = calloc(1, CORNERBUFFERSZ);
    recvTF[nrTF++] = 2;

    //domain->dataSendTF[2] = calloc(1, FUTURESZ);
    //domain->dataSendFF[2] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 2;
    sendFF[nsFF++] = 2;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[2][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[2][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[2]);
  }

  if (rowMin & colMax & planeMax) {
    // #20
    domain->dataRecvTF[19] = calloc(1, CORNERBUFFERSZ);
    domain->dataRecvFF[19] = calloc(1, CORNERBUFFERSZ);
    recvTF[nrTF++] = 19;
    recvFF[nrFF++] = 19;

    //domain->dataSendTF[19] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 19;

    //for (i = 0; i < maxcycles; i++)
    //  hpx_lco_future_init(&domain->dataSendTF[19][i]);

    //hpx_lco_future_init(&domain->SBN1[19]);
  }

  if (rowMax & colMin & planeMin) {
    // #7
    domain->dataRecvTF[6] = calloc(1, CORNERBUFFERSZ);
    recvTF[nrTF++] = 6;

    //domain->dataSendTF[6] = calloc(1, FUTURESZ);
    //domain->dataSendFF[6] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 6;
    sendFF[nsFF++] = 6;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[6][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[6][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[6]);
  }

  if (rowMax & colMin & planeMax) {
    // #24
    domain->dataRecvTF[23] = calloc(1, CORNERBUFFERSZ);
    domain->dataRecvFF[23] = calloc(1, CORNERBUFFERSZ);
    recvTF[nrTF++] = 23;
    recvFF[nrFF++] = 23;

    //domain->dataSendTF[23] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 23;

    //for (i = 0; i < maxcycles; i++)
    //  hpx_lco_future_init(&domain->dataSendTF[23][i]);

    //hpx_lco_future_init(&domain->SBN1[23]);
  }

  if (rowMax & colMax & planeMin) {
    // #9
    domain->dataRecvTF[8] = calloc(1, CORNERBUFFERSZ);
    recvTF[nrTF++] = 8;

    //domain->dataSendTF[8] = calloc(1, FUTURESZ);
    //domain->dataSendFF[8] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 8;
    sendFF[nsFF++] = 8;

    //for (i = 0; i < maxcycles; i++) {
    //  hpx_lco_future_init(&domain->dataSendTF[8][i]);
    //  hpx_lco_future_init(&domain->dataSendFF[8][i]);
    //}

    //hpx_lco_future_init(&domain->SBN1[8]);
  }

  if (rowMax & colMax & planeMax) {
    // #26
    domain->dataRecvTF[25] = calloc(1, CORNERBUFFERSZ);
    domain->dataRecvFF[25] = calloc(1, CORNERBUFFERSZ);
    recvTF[nrTF++] = 25;
    recvFF[nrFF++] = 25;

    //domain->dataSendTF[25] = calloc(1, FUTURESZ);
    sendTF[nsTF++] = 25;

    //for (i = 0; i < maxcycles; i++)
    //  hpx_lco_future_init(&domain->dataSendTF[25][i]);

    //hpx_lco_future_init(&domain->SBN1[25]);
  }

  domain->sendTT[0] = nsTT;
  domain->sendTF[0] = nsTF;
  domain->sendFF[0] = nsFF;

  domain->recvTT[0] = nrTT;
  domain->recvTF[0] = nrTF;
  domain->recvFF[0] = nrFF;

  // Compute the reverse of recvTT
  for (i=0;i<nrTT;i++) {
    int srcLocalIdx = recvTT[i];
    domain->reverse_recvTT[srcLocalIdx] = i;
  }
}

void DestroyDomain(Domain *domain)
{
  free(domain->delx_zeta);
  free(domain->delx_eta);
  free(domain->delx_xi);

  free(domain->delv_zeta);
  free(domain->delv_eta);
  free(domain->delv_xi);

  free(domain->matElemlist);
  free(domain->nodelist);
  free(domain->lxim);
  free(domain->lxip);
  free(domain->letam);
  free(domain->letap);
  free(domain->lzetam);
  free(domain->lzetap);
  free(domain->elemBC);
  free(domain->e);
  free(domain->p);
  free(domain->q);
  free(domain->ql);
  free(domain->qq);
  free(domain->v);
  free(domain->volo);
  free(domain->delv);
  free(domain->arealg);
  free(domain->ss);
  free(domain->elemMass);
  free(domain->x);
  free(domain->y);
  free(domain->z);
  free(domain->xd);
  free(domain->yd);
  free(domain->zd);
  free(domain->xdd);
  free(domain->ydd);
  free(domain->zdd);
  free(domain->fx);
  free(domain->fy);
  free(domain->fz);
  free(domain->nodalMass);

  if (domain->colLoc)
    free(domain->symmX);

  if (domain->rowLoc)
    free(domain->symmY);

  if (domain->planeLoc)
    free(domain->symmZ);

  int i;
  for (i = 0; i < 26; i++) {
    free(domain->dataRecvTF[i]);
    free(domain->dataRecvTT[i]);
    free(domain->dataRecvFF[i]);

    //free(domain->dataSendTF[i]);
    //free(domain->dataSendTT[i]);
    //free(domain->dataSendFF[i]);
  }
}

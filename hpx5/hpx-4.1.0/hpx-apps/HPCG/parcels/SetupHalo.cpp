
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

/*!
 @file SetupHalo.cpp

 HPCG routine
 */

#include <map>
#include <set>
#include <iostream>


#ifdef HPCG_DETAILED_DEBUG
#include <fstream>
using std::endl;
#include <cassert>
#endif

#include "SetupHalo.hpp"
#include "mytimer.hpp"
#include "hpcg-main.h"

/*!
  Prepares system matrix data structure and creates data necessary necessary
  for communication of boundary values of this process.

  @param[in]    geom The description of the problem's geometry.
  @param[inout] A    The known system matrix

  @see ExchangeHalo
*/
void SetupHalo(SparseMatrix & A,Domain *domain) {

  // Extract Matrix pieces

  local_int_t localNumberOfRows = A.localNumberOfRows;
  char  * nonzerosInRow = A.nonzerosInRow;
  global_int_t ** mtxIndG = A.mtxIndG;
  local_int_t ** mtxIndL = A.mtxIndL;

  // Scan global IDs of the nonzeros in the matrix.  Determine if the column ID matches a row ID.  If not:
  // 1) We call the ComputeRankOfMatrixRow function, which tells us the rank of the processor owning the row ID.
  //  We need to receive this value of the x vector during the halo exchange.
  // 2) We record our row ID since we know that the other processor will need this value from us, due to symmetry.

  std::map< int, std::set< global_int_t> > sendList, receiveList;
  typedef std::map< int, std::set< global_int_t> >::iterator map_iter;
  typedef std::set<global_int_t>::iterator set_iter;
  std::map< local_int_t, local_int_t > externalToLocalMap;

  // TODO: With proper critical and atomic regions, this loop could be threaded, but not attempting it at this time
  for (local_int_t i=0; i< localNumberOfRows; i++) {
    global_int_t currentGlobalRow = A.localToGlobalMap[i];
    for (int j=0; j<nonzerosInRow[i]; j++) {
      global_int_t curIndex = mtxIndG[i][j];
      int rankIdOfColumnEntry = ComputeRankOfMatrixRow(*(A.geom), curIndex);
#ifdef HPCG_DETAILED_DEBUG
      HPCG_fout << "rank, row , col, globalToLocalMap[col] = " << A.geom->rank << " " << currentGlobalRow << " "
          << curIndex << " " << A.globalToLocalMap[curIndex] << endl;
#endif
      if (A.geom->rank!=rankIdOfColumnEntry) {// If column index is not a row index, then it comes from another processor
        receiveList[rankIdOfColumnEntry].insert(curIndex);
        sendList[rankIdOfColumnEntry].insert(currentGlobalRow); // Matrix symmetry means we know the neighbor process wants my value
      }
    }
  }

  // Count number of matrix entries to send and receive
  local_int_t totalToBeSent = 0;
  for (map_iter curNeighbor = sendList.begin(); curNeighbor != sendList.end(); ++curNeighbor) {
    totalToBeSent += (curNeighbor->second).size();
  }
  local_int_t totalToBeReceived = 0;
  for (map_iter curNeighbor = receiveList.begin(); curNeighbor != receiveList.end(); ++curNeighbor) {
    totalToBeReceived += (curNeighbor->second).size();
  }

#ifdef HPCG_DETAILED_DEBUG
  // These are all attributes that should be true, due to symmetry
  HPCG_fout << "totalToBeSent = " << totalToBeSent << " totalToBeReceived = " << totalToBeReceived << endl;
  assert(totalToBeSent==totalToBeReceived); // Number of sent entry should equal number of received
  assert(sendList.size()==receiveList.size()); // Number of send-to neighbors should equal number of receive-from
  // Each receive-from neighbor should be a send-to neighbor, and send the same number of entries
  for (map_iter curNeighbor = receiveList.begin(); curNeighbor != receiveList.end(); ++curNeighbor) {
    assert(sendList.find(curNeighbor->first)!=sendList.end());
    assert(sendList[curNeighbor->first].size()==receiveList[curNeighbor->first].size());
  }
#endif

  // Build the arrays and lists needed by the ExchangeHalo function.
  double * sendBuffer = new double[totalToBeSent];
  local_int_t * elementsToSend = new local_int_t[totalToBeSent];
  int * neighbors = new int[sendList.size()];
  local_int_t * receiveLength = new local_int_t[receiveList.size()];
  local_int_t * sendLength = new local_int_t[sendList.size()];
  int neighborCount = 0;
  local_int_t receiveEntryCount = 0;
  local_int_t sendEntryCount = 0;
  for (map_iter curNeighbor = receiveList.begin(); curNeighbor != receiveList.end(); ++curNeighbor, ++neighborCount) {
    int neighborId = curNeighbor->first; // rank of current neighbor we are processing
    neighbors[neighborCount] = neighborId; // store rank ID of current neighbor
    receiveLength[neighborCount] = receiveList[neighborId].size();
    sendLength[neighborCount] = sendList[neighborId].size(); // Get count if sends/receives
    for (set_iter i = receiveList[neighborId].begin(); i != receiveList[neighborId].end(); ++i, ++receiveEntryCount) {
      externalToLocalMap[*i] = localNumberOfRows + receiveEntryCount; // The remote columns are indexed at end of internals
    }
    for (set_iter i = sendList[neighborId].begin(); i != sendList[neighborId].end(); ++i, ++sendEntryCount) {
      //if (geom.rank==1) HPCG_fout << "*i, globalToLocalMap[*i], sendEntryCount = " << *i << " " << A.globalToLocalMap[*i] << " " << sendEntryCount << endl;
      elementsToSend[sendEntryCount] = A.globalToLocalMap[*i]; // store local ids of entry to send
    }
  }

  // Convert matrix indices to local IDs
  for (local_int_t i=0; i< localNumberOfRows; i++) {
    for (int j=0; j<nonzerosInRow[i]; j++) {
      global_int_t curIndex = mtxIndG[i][j];
      int rankIdOfColumnEntry = ComputeRankOfMatrixRow(*(A.geom), curIndex);
      if (A.geom->rank==rankIdOfColumnEntry) { // My column index, so convert to local index
        mtxIndL[i][j] = A.globalToLocalMap[curIndex];
      } else { // If column index is not a row index, then it comes from another processor
        mtxIndL[i][j] = externalToLocalMap[curIndex];
      }
    }
  }

  // Store contents in our matrix struct
  A.numberOfExternalValues = externalToLocalMap.size();
  A.localNumberOfColumns = A.localNumberOfRows + A.numberOfExternalValues;
  A.numberOfSendNeighbors = sendList.size();
  A.totalToBeSent = totalToBeSent;
  A.elementsToSend = elementsToSend;
  A.neighbors = neighbors;
  A.receiveLength = receiveLength;
  A.sendLength = sendLength;
  A.sendBuffer = sendBuffer;

  if ( domain->params.use_memput == 1 || domain->params.use_memput_copyless == 1 ) {
    A.grecv_length = new int[domain->params.comm_size*domain->params.comm_size];
    A.rn_order = new int[domain->params.comm_size];
    set_rn(neighbors,A.numberOfSendNeighbors,receiveLength,A.grecv_length,A.rn_order,domain);
  }

#ifdef HPCG_DETAILED_DEBUG
  std::cout << " For rank " << A.geom->rank << " of " << A.geom->size << ", number of neighbors = " << A.numberOfSendNeighbors << std::endl;
  for (int i = 0; i < A.numberOfSendNeighbors; i++) {
    std::cout << "     rank " << A.geom->rank << " neighbor " << neighbors[i] << " send/recv length = " << sendLength[i] << "/" << receiveLength[i] << std::endl;
  //  for (local_int_t j = 0; j<sendLength[i]; ++j)
  //    std::cout << "       rank " << A.geom->rank << " elementsToSend[" << j << "] = " << elementsToSend[j] << std::endl;
  }
#endif

  return;
}

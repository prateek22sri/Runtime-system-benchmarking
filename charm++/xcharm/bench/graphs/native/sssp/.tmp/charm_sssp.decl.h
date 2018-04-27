#ifndef _DECL_charm_sssp_H_
#define _DECL_charm_sssp_H_
#include "charm++.h"
#include "envelope.h"
#include <memory>
#include "sdag.h"
#include "GraphGenerator.decl.h"

/* DECLS: readonly int K;
 */

/* DECLS: readonly CmiUInt8 N;
 */

/* DECLS: readonly CmiUInt8 M;
 */

/* DECLS: readonly CProxy_TestDriver driverProxy;
 */

/* DECLS: mainchare TestDriver: Chare{
TestDriver(CkArgMsg* impl_msg);
void start(void);
void restart(void);
void startVerificationPhase(void);
void done(const CmiUInt8 &globalNumScannedEdges);
void startGraphConstruction(void);
void exit(void);
};
 */
 class TestDriver;
 class CkIndex_TestDriver;
 class CProxy_TestDriver;
/* --------------- index object ------------------ */
class CkIndex_TestDriver:public CkIndex_Chare{
  public:
    typedef TestDriver local_t;
    typedef CkIndex_TestDriver index_t;
    typedef CProxy_TestDriver proxy_t;
    typedef CProxy_TestDriver element_t;

    static int __idx;
    static void __register(const char *s, size_t size);
    /* DECLS: TestDriver(CkArgMsg* impl_msg);
     */
    // Entry point registration at startup
    
    static int reg_TestDriver_CkArgMsg();
    // Entry point index lookup
    
    inline static int idx_TestDriver_CkArgMsg() {
      static int epidx = reg_TestDriver_CkArgMsg();
      return epidx;
    }

    
    static int ckNew(CkArgMsg* impl_msg) { return idx_TestDriver_CkArgMsg(); }
    
    static void _call_TestDriver_CkArgMsg(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_TestDriver_CkArgMsg(void* impl_msg, void* impl_obj);
    /* DECLS: void start(void);
     */
    // Entry point registration at startup
    
    static int reg_start_void();
    // Entry point index lookup
    
    inline static int idx_start_void() {
      static int epidx = reg_start_void();
      return epidx;
    }

    
    inline static int idx_start(void (TestDriver::*)(void) ) {
      return idx_start_void();
    }


    
    static int start(void) { return idx_start_void(); }
    
    static void _call_start_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_start_void(void* impl_msg, void* impl_obj);
    /* DECLS: void restart(void);
     */
    // Entry point registration at startup
    
    static int reg_restart_void();
    // Entry point index lookup
    
    inline static int idx_restart_void() {
      static int epidx = reg_restart_void();
      return epidx;
    }

    
    inline static int idx_restart(void (TestDriver::*)(void) ) {
      return idx_restart_void();
    }


    
    static int restart(void) { return idx_restart_void(); }
    
    static void _call_restart_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_restart_void(void* impl_msg, void* impl_obj);
    /* DECLS: void startVerificationPhase(void);
     */
    // Entry point registration at startup
    
    static int reg_startVerificationPhase_void();
    // Entry point index lookup
    
    inline static int idx_startVerificationPhase_void() {
      static int epidx = reg_startVerificationPhase_void();
      return epidx;
    }

    
    inline static int idx_startVerificationPhase(void (TestDriver::*)(void) ) {
      return idx_startVerificationPhase_void();
    }


    
    static int startVerificationPhase(void) { return idx_startVerificationPhase_void(); }
    
    static void _call_startVerificationPhase_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_startVerificationPhase_void(void* impl_msg, void* impl_obj);
    /* DECLS: void done(const CmiUInt8 &globalNumScannedEdges);
     */
    // Entry point registration at startup
    
    static int reg_done_marshall5();
    // Entry point index lookup
    
    inline static int idx_done_marshall5() {
      static int epidx = reg_done_marshall5();
      return epidx;
    }

    
    inline static int idx_done(void (TestDriver::*)(const CmiUInt8 &globalNumScannedEdges) ) {
      return idx_done_marshall5();
    }


    
    static int done(const CmiUInt8 &globalNumScannedEdges) { return idx_done_marshall5(); }
    // Entry point registration at startup
    
    static int reg_redn_wrapper_done_marshall5();
    // Entry point index lookup
    
    inline static int idx_redn_wrapper_done_marshall5() {
      static int epidx = reg_redn_wrapper_done_marshall5();
      return epidx;
    }
    
    static int redn_wrapper_done(CkReductionMsg* impl_msg) { return idx_redn_wrapper_done_marshall5(); }
    
    static void _call_redn_wrapper_done_marshall5(void* impl_msg, void* impl_obj_void);
    
    static void _call_done_marshall5(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_done_marshall5(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_done_marshall5(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_done_marshall5(PUP::er &p,void *msg);
    /* DECLS: void startGraphConstruction(void);
     */
    // Entry point registration at startup
    
    static int reg_startGraphConstruction_void();
    // Entry point index lookup
    
    inline static int idx_startGraphConstruction_void() {
      static int epidx = reg_startGraphConstruction_void();
      return epidx;
    }

    
    inline static int idx_startGraphConstruction(void (TestDriver::*)(void) ) {
      return idx_startGraphConstruction_void();
    }


    
    static int startGraphConstruction(void) { return idx_startGraphConstruction_void(); }
    
    static void _call_startGraphConstruction_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_startGraphConstruction_void(void* impl_msg, void* impl_obj);
    /* DECLS: void exit(void);
     */
    // Entry point registration at startup
    
    static int reg_exit_void();
    // Entry point index lookup
    
    inline static int idx_exit_void() {
      static int epidx = reg_exit_void();
      return epidx;
    }

    
    inline static int idx_exit(void (TestDriver::*)(void) ) {
      return idx_exit_void();
    }


    
    static int exit(void) { return idx_exit_void(); }
    
    static void _call_exit_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_exit_void(void* impl_msg, void* impl_obj);
};
/* --------------- element proxy ------------------ */
class CProxy_TestDriver:public CProxy_Chare{
  public:
    typedef TestDriver local_t;
    typedef CkIndex_TestDriver index_t;
    typedef CProxy_TestDriver proxy_t;
    typedef CProxy_TestDriver element_t;

    CProxy_TestDriver(void) {};
    CProxy_TestDriver(CkChareID __cid) : CProxy_Chare(__cid){  }
    CProxy_TestDriver(const Chare *c) : CProxy_Chare(c){  }

    int ckIsDelegated(void) const
    { return CProxy_Chare::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxy_Chare::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxy_Chare::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxy_Chare::ckDelegatedIdx(); }

    inline void ckCheck(void) const
    { CProxy_Chare::ckCheck(); }
    const CkChareID &ckGetChareID(void) const
    { return CProxy_Chare::ckGetChareID(); }
    operator const CkChareID &(void) const
    { return ckGetChareID(); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxy_Chare::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxy_Chare::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxy_Chare::pup(p); }

    void ckSetChareID(const CkChareID &c)
    {      CProxy_Chare::ckSetChareID(c); }
    TestDriver *ckLocal(void) const
    { return (TestDriver *)CkLocalChare(&ckGetChareID()); }
/* DECLS: TestDriver(CkArgMsg* impl_msg);
 */
    static CkChareID ckNew(CkArgMsg* impl_msg, int onPE=CK_PE_ANY);
    static void ckNew(CkArgMsg* impl_msg, CkChareID* pcid, int onPE=CK_PE_ANY);
    CProxy_TestDriver(CkArgMsg* impl_msg, int onPE=CK_PE_ANY);

/* DECLS: void start(void);
 */
    
    void start(void);

/* DECLS: void restart(void);
 */
    
    void restart(void);

/* DECLS: void startVerificationPhase(void);
 */
    
    void startVerificationPhase(void);

/* DECLS: void done(const CmiUInt8 &globalNumScannedEdges);
 */
    
    void done(const CmiUInt8 &globalNumScannedEdges, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void startGraphConstruction(void);
 */
    
    void startGraphConstruction(void);

/* DECLS: void exit(void);
 */
    
    void exit(void);

};
PUPmarshall(CProxy_TestDriver)
#define TestDriver_SDAG_CODE 
typedef CBaseT1<Chare, CProxy_TestDriver>CBase_TestDriver;

class SSSPEdge;

/* DECLS: array SSSPVertex: ArrayElement{
SSSPVertex(void);
void connectVertex(const SSSPEdge &edge);
void make_root(void);
void update(const CmiInt8 &v, const double &w);
void verify(void);
void getScannedVertexNum(void);
void check(const double &w);
void print(void);
void dump_edges(void);
SSSPVertex(CkMigrateMessage* impl_msg);
};
 */
 class SSSPVertex;
 class CkIndex_SSSPVertex;
 class CProxy_SSSPVertex;
 class CProxyElement_SSSPVertex;
 class CProxySection_SSSPVertex;
/* --------------- index object ------------------ */
class CkIndex_SSSPVertex:public CkIndex_ArrayElement{
  public:
    typedef SSSPVertex local_t;
    typedef CkIndex_SSSPVertex index_t;
    typedef CProxy_SSSPVertex proxy_t;
    typedef CProxyElement_SSSPVertex element_t;
    typedef CProxySection_SSSPVertex section_t;

    static int __idx;
    static void __register(const char *s, size_t size);
    /* DECLS: SSSPVertex(void);
     */
    // Entry point registration at startup
    
    static int reg_SSSPVertex_void();
    // Entry point index lookup
    
    inline static int idx_SSSPVertex_void() {
      static int epidx = reg_SSSPVertex_void();
      return epidx;
    }

    
    static int ckNew(void) { return idx_SSSPVertex_void(); }
    
    static void _call_SSSPVertex_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_SSSPVertex_void(void* impl_msg, void* impl_obj);
    /* DECLS: void connectVertex(const SSSPEdge &edge);
     */
    // Entry point registration at startup
    
    static int reg_connectVertex_marshall2();
    // Entry point index lookup
    
    inline static int idx_connectVertex_marshall2() {
      static int epidx = reg_connectVertex_marshall2();
      return epidx;
    }

    
    inline static int idx_connectVertex(void (SSSPVertex::*)(const SSSPEdge &edge) ) {
      return idx_connectVertex_marshall2();
    }


    
    static int connectVertex(const SSSPEdge &edge) { return idx_connectVertex_marshall2(); }
    
    static void _call_connectVertex_marshall2(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_connectVertex_marshall2(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_connectVertex_marshall2(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_connectVertex_marshall2(PUP::er &p,void *msg);
    /* DECLS: void make_root(void);
     */
    // Entry point registration at startup
    
    static int reg_make_root_void();
    // Entry point index lookup
    
    inline static int idx_make_root_void() {
      static int epidx = reg_make_root_void();
      return epidx;
    }

    
    inline static int idx_make_root(void (SSSPVertex::*)(void) ) {
      return idx_make_root_void();
    }


    
    static int make_root(void) { return idx_make_root_void(); }
    
    static void _call_make_root_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_make_root_void(void* impl_msg, void* impl_obj);
    /* DECLS: void update(const CmiInt8 &v, const double &w);
     */
    // Entry point registration at startup
    
    static int reg_update_marshall4();
    // Entry point index lookup
    
    inline static int idx_update_marshall4() {
      static int epidx = reg_update_marshall4();
      return epidx;
    }

    
    inline static int idx_update(void (SSSPVertex::*)(const CmiInt8 &v, const double &w) ) {
      return idx_update_marshall4();
    }


    
    static int update(const CmiInt8 &v, const double &w) { return idx_update_marshall4(); }
    
    static void _call_update_marshall4(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_update_marshall4(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_update_marshall4(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_update_marshall4(PUP::er &p,void *msg);
    /* DECLS: void verify(void);
     */
    // Entry point registration at startup
    
    static int reg_verify_void();
    // Entry point index lookup
    
    inline static int idx_verify_void() {
      static int epidx = reg_verify_void();
      return epidx;
    }

    
    inline static int idx_verify(void (SSSPVertex::*)(void) ) {
      return idx_verify_void();
    }


    
    static int verify(void) { return idx_verify_void(); }
    
    static void _call_verify_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_verify_void(void* impl_msg, void* impl_obj);
    /* DECLS: void getScannedVertexNum(void);
     */
    // Entry point registration at startup
    
    static int reg_getScannedVertexNum_void();
    // Entry point index lookup
    
    inline static int idx_getScannedVertexNum_void() {
      static int epidx = reg_getScannedVertexNum_void();
      return epidx;
    }

    
    inline static int idx_getScannedVertexNum(void (SSSPVertex::*)(void) ) {
      return idx_getScannedVertexNum_void();
    }


    
    static int getScannedVertexNum(void) { return idx_getScannedVertexNum_void(); }
    
    static void _call_getScannedVertexNum_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_getScannedVertexNum_void(void* impl_msg, void* impl_obj);
    /* DECLS: void check(const double &w);
     */
    // Entry point registration at startup
    
    static int reg_check_marshall7();
    // Entry point index lookup
    
    inline static int idx_check_marshall7() {
      static int epidx = reg_check_marshall7();
      return epidx;
    }

    
    inline static int idx_check(void (SSSPVertex::*)(const double &w) ) {
      return idx_check_marshall7();
    }


    
    static int check(const double &w) { return idx_check_marshall7(); }
    
    static void _call_check_marshall7(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_check_marshall7(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_check_marshall7(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_check_marshall7(PUP::er &p,void *msg);
    /* DECLS: void print(void);
     */
    // Entry point registration at startup
    
    static int reg_print_void();
    // Entry point index lookup
    
    inline static int idx_print_void() {
      static int epidx = reg_print_void();
      return epidx;
    }

    
    inline static int idx_print(void (SSSPVertex::*)(void) ) {
      return idx_print_void();
    }


    
    static int print(void) { return idx_print_void(); }
    
    static void _call_print_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_print_void(void* impl_msg, void* impl_obj);
    /* DECLS: void dump_edges(void);
     */
    // Entry point registration at startup
    
    static int reg_dump_edges_void();
    // Entry point index lookup
    
    inline static int idx_dump_edges_void() {
      static int epidx = reg_dump_edges_void();
      return epidx;
    }

    
    inline static int idx_dump_edges(void (SSSPVertex::*)(void) ) {
      return idx_dump_edges_void();
    }


    
    static int dump_edges(void) { return idx_dump_edges_void(); }
    
    static void _call_dump_edges_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_dump_edges_void(void* impl_msg, void* impl_obj);
    /* DECLS: SSSPVertex(CkMigrateMessage* impl_msg);
     */
    // Entry point registration at startup
    
    static int reg_SSSPVertex_CkMigrateMessage();
    // Entry point index lookup
    
    inline static int idx_SSSPVertex_CkMigrateMessage() {
      static int epidx = reg_SSSPVertex_CkMigrateMessage();
      return epidx;
    }

    
    static int ckNew(CkMigrateMessage* impl_msg) { return idx_SSSPVertex_CkMigrateMessage(); }
    
    static void _call_SSSPVertex_CkMigrateMessage(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_SSSPVertex_CkMigrateMessage(void* impl_msg, void* impl_obj);
};
/* --------------- element proxy ------------------ */
 class CProxyElement_SSSPVertex : public CProxyElement_ArrayElement{
  public:
    typedef SSSPVertex local_t;
    typedef CkIndex_SSSPVertex index_t;
    typedef CProxy_SSSPVertex proxy_t;
    typedef CProxyElement_SSSPVertex element_t;
    typedef CProxySection_SSSPVertex section_t;

    CProxyElement_SSSPVertex(void) {}
    CProxyElement_SSSPVertex(const ArrayElement *e) : CProxyElement_ArrayElement(e){  }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxyElement_ArrayElement::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxyElement_ArrayElement::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxyElement_ArrayElement::pup(p); }

    int ckIsDelegated(void) const
    { return CProxyElement_ArrayElement::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxyElement_ArrayElement::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxyElement_ArrayElement::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxyElement_ArrayElement::ckDelegatedIdx(); }

    inline void ckCheck(void) const
    { CProxyElement_ArrayElement::ckCheck(); }
    inline operator CkArrayID () const
    { return ckGetArrayID(); }
    inline CkArrayID ckGetArrayID(void) const
    { return CProxyElement_ArrayElement::ckGetArrayID(); }
    inline CkArray *ckLocalBranch(void) const
    { return CProxyElement_ArrayElement::ckLocalBranch(); }
    inline CkLocMgr *ckLocMgr(void) const
    { return CProxyElement_ArrayElement::ckLocMgr(); }

    inline static CkArrayID ckCreateEmptyArray(CkArrayOptions opts = CkArrayOptions())
    { return CProxyElement_ArrayElement::ckCreateEmptyArray(opts); }
    inline static CkArrayID ckCreateArray(CkArrayMessage *m,int ctor,const CkArrayOptions &opts)
    { return CProxyElement_ArrayElement::ckCreateArray(m,ctor,opts); }
    inline void ckInsertIdx(CkArrayMessage *m,int ctor,int onPe,const CkArrayIndex &idx)
    { CProxyElement_ArrayElement::ckInsertIdx(m,ctor,onPe,idx); }
    inline void doneInserting(void)
    { CProxyElement_ArrayElement::doneInserting(); }

    inline void ckBroadcast(CkArrayMessage *m, int ep, int opts=0) const
    { CProxyElement_ArrayElement::ckBroadcast(m,ep,opts); }
    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_ArrayElement::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_ArrayElement::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxyElement_ArrayElement::ckSetReductionClient(cb); }

    inline void ckInsert(CkArrayMessage *m,int ctor,int onPe)
    { CProxyElement_ArrayElement::ckInsert(m,ctor,onPe); }
    inline void ckSend(CkArrayMessage *m, int ep, int opts = 0) const
    { CProxyElement_ArrayElement::ckSend(m,ep,opts); }
    inline void *ckSendSync(CkArrayMessage *m, int ep) const
    { return CProxyElement_ArrayElement::ckSendSync(m,ep); }
    inline const CkArrayIndex &ckGetIndex() const
    { return CProxyElement_ArrayElement::ckGetIndex(); }

    SSSPVertex *ckLocal(void) const
    { return (SSSPVertex *)CProxyElement_ArrayElement::ckLocal(); }

    CProxyElement_SSSPVertex(const CkArrayID &aid,const CkArrayIndex1D &idx,CK_DELCTOR_PARAM)
        :CProxyElement_ArrayElement(aid,idx,CK_DELCTOR_ARGS)
    {}
    CProxyElement_SSSPVertex(const CkArrayID &aid,const CkArrayIndex1D &idx)
        :CProxyElement_ArrayElement(aid,idx)
    {}

    CProxyElement_SSSPVertex(const CkArrayID &aid,const CkArrayIndex &idx,CK_DELCTOR_PARAM)
        :CProxyElement_ArrayElement(aid,idx,CK_DELCTOR_ARGS)
    {}
    CProxyElement_SSSPVertex(const CkArrayID &aid,const CkArrayIndex &idx)
        :CProxyElement_ArrayElement(aid,idx)
    {}
/* DECLS: SSSPVertex(void);
 */
    
    void insert(int onPE=-1);
/* DECLS: void connectVertex(const SSSPEdge &edge);
 */
    
    void connectVertex(const SSSPEdge &edge, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void make_root(void);
 */
    
    void make_root(void) ;

/* DECLS: void update(const CmiInt8 &v, const double &w);
 */
    
    void update(const CmiInt8 &v, const double &w, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void verify(void);
 */
    
    void verify(void) ;

/* DECLS: void getScannedVertexNum(void);
 */
    
    void getScannedVertexNum(void) ;

/* DECLS: void check(const double &w);
 */
    
    void check(const double &w, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void print(void);
 */
    
    void print(void) ;

/* DECLS: void dump_edges(void);
 */
    
    void dump_edges(void) ;

/* DECLS: SSSPVertex(CkMigrateMessage* impl_msg);
 */

};
PUPmarshall(CProxyElement_SSSPVertex)
/* ---------------- collective proxy -------------- */
 class CProxy_SSSPVertex : public CProxy_ArrayElement{
  public:
    typedef SSSPVertex local_t;
    typedef CkIndex_SSSPVertex index_t;
    typedef CProxy_SSSPVertex proxy_t;
    typedef CProxyElement_SSSPVertex element_t;
    typedef CProxySection_SSSPVertex section_t;

    CProxy_SSSPVertex(void) {}
    CProxy_SSSPVertex(const ArrayElement *e) : CProxy_ArrayElement(e){  }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxy_ArrayElement::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxy_ArrayElement::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxy_ArrayElement::pup(p); }

    int ckIsDelegated(void) const
    { return CProxy_ArrayElement::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxy_ArrayElement::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxy_ArrayElement::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxy_ArrayElement::ckDelegatedIdx(); }

    inline void ckCheck(void) const
    { CProxy_ArrayElement::ckCheck(); }
    inline operator CkArrayID () const
    { return ckGetArrayID(); }
    inline CkArrayID ckGetArrayID(void) const
    { return CProxy_ArrayElement::ckGetArrayID(); }
    inline CkArray *ckLocalBranch(void) const
    { return CProxy_ArrayElement::ckLocalBranch(); }
    inline CkLocMgr *ckLocMgr(void) const
    { return CProxy_ArrayElement::ckLocMgr(); }

    inline static CkArrayID ckCreateEmptyArray(CkArrayOptions opts = CkArrayOptions())
    { return CProxy_ArrayElement::ckCreateEmptyArray(opts); }
    inline static CkArrayID ckCreateArray(CkArrayMessage *m,int ctor,const CkArrayOptions &opts)
    { return CProxy_ArrayElement::ckCreateArray(m,ctor,opts); }
    inline void ckInsertIdx(CkArrayMessage *m,int ctor,int onPe,const CkArrayIndex &idx)
    { CProxy_ArrayElement::ckInsertIdx(m,ctor,onPe,idx); }
    inline void doneInserting(void)
    { CProxy_ArrayElement::doneInserting(); }

    inline void ckBroadcast(CkArrayMessage *m, int ep, int opts=0) const
    { CProxy_ArrayElement::ckBroadcast(m,ep,opts); }
    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_ArrayElement::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_ArrayElement::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxy_ArrayElement::ckSetReductionClient(cb); }

    // Generalized array indexing:
    CProxyElement_SSSPVertex operator [] (const CkArrayIndex1D &idx) const
    { return CProxyElement_SSSPVertex(ckGetArrayID(), idx, CK_DELCTOR_CALL); }
    CProxyElement_SSSPVertex operator() (const CkArrayIndex1D &idx) const
    { return CProxyElement_SSSPVertex(ckGetArrayID(), idx, CK_DELCTOR_CALL); }
    CProxyElement_SSSPVertex operator [] (int idx) const 
        {return CProxyElement_SSSPVertex(ckGetArrayID(), CkArrayIndex1D(idx), CK_DELCTOR_CALL);}
    CProxyElement_SSSPVertex operator () (int idx) const 
        {return CProxyElement_SSSPVertex(ckGetArrayID(), CkArrayIndex1D(idx), CK_DELCTOR_CALL);}
    CProxy_SSSPVertex(const CkArrayID &aid,CK_DELCTOR_PARAM) 
        :CProxy_ArrayElement(aid,CK_DELCTOR_ARGS) {}
    CProxy_SSSPVertex(const CkArrayID &aid) 
        :CProxy_ArrayElement(aid) {}
/* DECLS: SSSPVertex(void);
 */
    
    static CkArrayID ckNew(const CkArrayOptions &opts = CkArrayOptions());
    static void      ckNew(const CkArrayOptions &opts, CkCallback _ck_array_creation_cb);
    static CkArrayID ckNew(const int s1);
    static void ckNew(const int s1, CkCallback _ck_array_creation_cb);

/* DECLS: void connectVertex(const SSSPEdge &edge);
 */
    
    void connectVertex(const SSSPEdge &edge, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void make_root(void);
 */
    
    void make_root(void) ;

/* DECLS: void update(const CmiInt8 &v, const double &w);
 */
    
    void update(const CmiInt8 &v, const double &w, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void verify(void);
 */
    
    void verify(void) ;

/* DECLS: void getScannedVertexNum(void);
 */
    
    void getScannedVertexNum(void) ;

/* DECLS: void check(const double &w);
 */
    
    void check(const double &w, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void print(void);
 */
    
    void print(void) ;

/* DECLS: void dump_edges(void);
 */
    
    void dump_edges(void) ;

/* DECLS: SSSPVertex(CkMigrateMessage* impl_msg);
 */

};
PUPmarshall(CProxy_SSSPVertex)
/* ---------------- section proxy -------------- */
 class CProxySection_SSSPVertex : public CProxySection_ArrayElement{
  public:
    typedef SSSPVertex local_t;
    typedef CkIndex_SSSPVertex index_t;
    typedef CProxy_SSSPVertex proxy_t;
    typedef CProxyElement_SSSPVertex element_t;
    typedef CProxySection_SSSPVertex section_t;

    CProxySection_SSSPVertex(void) {}

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxySection_ArrayElement::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxySection_ArrayElement::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxySection_ArrayElement::pup(p); }

    int ckIsDelegated(void) const
    { return CProxySection_ArrayElement::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxySection_ArrayElement::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxySection_ArrayElement::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxySection_ArrayElement::ckDelegatedIdx(); }

    inline void ckCheck(void) const
    { CProxySection_ArrayElement::ckCheck(); }
    inline operator CkArrayID () const
    { return ckGetArrayID(); }
    inline CkArrayID ckGetArrayID(void) const
    { return CProxySection_ArrayElement::ckGetArrayID(); }
    inline CkArray *ckLocalBranch(void) const
    { return CProxySection_ArrayElement::ckLocalBranch(); }
    inline CkLocMgr *ckLocMgr(void) const
    { return CProxySection_ArrayElement::ckLocMgr(); }

    inline static CkArrayID ckCreateEmptyArray(CkArrayOptions opts = CkArrayOptions())
    { return CProxySection_ArrayElement::ckCreateEmptyArray(opts); }
    inline static CkArrayID ckCreateArray(CkArrayMessage *m,int ctor,const CkArrayOptions &opts)
    { return CProxySection_ArrayElement::ckCreateArray(m,ctor,opts); }
    inline void ckInsertIdx(CkArrayMessage *m,int ctor,int onPe,const CkArrayIndex &idx)
    { CProxySection_ArrayElement::ckInsertIdx(m,ctor,onPe,idx); }
    inline void doneInserting(void)
    { CProxySection_ArrayElement::doneInserting(); }

    inline void ckBroadcast(CkArrayMessage *m, int ep, int opts=0) const
    { CProxySection_ArrayElement::ckBroadcast(m,ep,opts); }
    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_ArrayElement::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_ArrayElement::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxySection_ArrayElement::ckSetReductionClient(cb); }

    inline void ckSend(CkArrayMessage *m, int ep, int opts = 0)
    { CProxySection_ArrayElement::ckSend(m,ep,opts); }
    inline CkSectionInfo &ckGetSectionInfo()
    { return CProxySection_ArrayElement::ckGetSectionInfo(); }
    inline CkSectionID *ckGetSectionIDs()
    { return CProxySection_ArrayElement::ckGetSectionIDs(); }
    inline CkSectionID &ckGetSectionID()
    { return CProxySection_ArrayElement::ckGetSectionID(); }
    inline CkSectionID &ckGetSectionID(int i)
    { return CProxySection_ArrayElement::ckGetSectionID(i); }
    inline CkArrayID ckGetArrayIDn(int i) const
    { return CProxySection_ArrayElement::ckGetArrayIDn(i); } 
    inline CkArrayIndex *ckGetArrayElements() const
    { return CProxySection_ArrayElement::ckGetArrayElements(); }
    inline CkArrayIndex *ckGetArrayElements(int i) const
    { return CProxySection_ArrayElement::ckGetArrayElements(i); }
    inline int ckGetNumElements() const
    { return CProxySection_ArrayElement::ckGetNumElements(); } 
    inline int ckGetNumElements(int i) const
    { return CProxySection_ArrayElement::ckGetNumElements(i); }    // Generalized array indexing:
    CProxyElement_SSSPVertex operator [] (const CkArrayIndex1D &idx) const
        {return CProxyElement_SSSPVertex(ckGetArrayID(), idx, CK_DELCTOR_CALL);}
    CProxyElement_SSSPVertex operator() (const CkArrayIndex1D &idx) const
        {return CProxyElement_SSSPVertex(ckGetArrayID(), idx, CK_DELCTOR_CALL);}
    CProxyElement_SSSPVertex operator [] (int idx) const 
        {return CProxyElement_SSSPVertex(ckGetArrayID(), *(CkArrayIndex1D*)&ckGetArrayElements()[idx], CK_DELCTOR_CALL);}
    CProxyElement_SSSPVertex operator () (int idx) const 
        {return CProxyElement_SSSPVertex(ckGetArrayID(), *(CkArrayIndex1D*)&ckGetArrayElements()[idx], CK_DELCTOR_CALL);}
    static CkSectionID ckNew(const CkArrayID &aid, CkArrayIndex1D *elems, int nElems) {
      return CkSectionID(aid, elems, nElems);
    } 
    static CkSectionID ckNew(const CkArrayID &aid, int l, int u, int s) {
      CkVec<CkArrayIndex1D> al;
      for (int i=l; i<=u; i+=s) al.push_back(CkArrayIndex1D(i));
      return CkSectionID(aid, al.getVec(), al.size());
    } 
    CProxySection_SSSPVertex(const CkArrayID &aid, CkArrayIndex *elems, int nElems, CK_DELCTOR_PARAM) 
        :CProxySection_ArrayElement(aid,elems,nElems,CK_DELCTOR_ARGS) {}
    CProxySection_SSSPVertex(const CkArrayID &aid, CkArrayIndex *elems, int nElems) 
        :CProxySection_ArrayElement(aid,elems,nElems) {}
    CProxySection_SSSPVertex(const CkSectionID &sid)       :CProxySection_ArrayElement(sid) {}
    CProxySection_SSSPVertex(int n, const CkArrayID *aid, CkArrayIndex const * const *elems, const int *nElems, CK_DELCTOR_PARAM) 
        :CProxySection_ArrayElement(n,aid,elems,nElems,CK_DELCTOR_ARGS) {}
    CProxySection_SSSPVertex(int n, const CkArrayID *aid, CkArrayIndex const * const *elems, const int *nElems) 
        :CProxySection_ArrayElement(n,aid,elems,nElems) {}
    static CkSectionID ckNew(const CkArrayID &aid, CkArrayIndex *elems, int nElems) {
      return CkSectionID(aid, elems, nElems);
    } 
/* DECLS: SSSPVertex(void);
 */
    

/* DECLS: void connectVertex(const SSSPEdge &edge);
 */
    
    void connectVertex(const SSSPEdge &edge, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void make_root(void);
 */
    
    void make_root(void) ;

/* DECLS: void update(const CmiInt8 &v, const double &w);
 */
    
    void update(const CmiInt8 &v, const double &w, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void verify(void);
 */
    
    void verify(void) ;

/* DECLS: void getScannedVertexNum(void);
 */
    
    void getScannedVertexNum(void) ;

/* DECLS: void check(const double &w);
 */
    
    void check(const double &w, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void print(void);
 */
    
    void print(void) ;

/* DECLS: void dump_edges(void);
 */
    
    void dump_edges(void) ;

/* DECLS: SSSPVertex(CkMigrateMessage* impl_msg);
 */

};
PUPmarshall(CProxySection_SSSPVertex)
#define SSSPVertex_SDAG_CODE 
typedef CBaseT1<ArrayElementT<CkIndex1D>, CProxy_SSSPVertex>CBase_SSSPVertex;















/* ---------------- method closures -------------- */
class Closure_TestDriver {
  public:


    struct start_2_closure;


    struct restart_3_closure;


    struct startVerificationPhase_4_closure;


    struct done_5_closure;


    struct startGraphConstruction_6_closure;


    struct exit_7_closure;

};


/* ---------------- method closures -------------- */
class Closure_SSSPVertex {
  public:


    struct connectVertex_2_closure;


    struct make_root_3_closure;


    struct update_4_closure;


    struct verify_5_closure;


    struct getScannedVertexNum_6_closure;


    struct check_7_closure;


    struct print_8_closure;


    struct dump_edges_9_closure;


};










extern void _registercharm_sssp(void);
extern "C" void CkRegisterMainModule(void);

template <>
void recursive_pup_impl<TestDriver, 1>::operator()(TestDriver *obj, PUP::er &p);

template <>
void recursive_pup_impl<SSSPVertex, 1>::operator()(SSSPVertex *obj, PUP::er &p);

template <>
void recursive_pup_impl<ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter >, 1>::operator()(ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > *obj, PUP::er &p);

template <>
void recursive_pup_impl<MeshStreamer<ArrayDataItem<SSSPEdge,long long >,SimpleMeshRouter >, 1>::operator()(MeshStreamer<ArrayDataItem<SSSPEdge,long long >,SimpleMeshRouter > *obj, PUP::er &p);

template <>
void recursive_pup_impl<GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >, 1>::operator()(GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex > *obj, PUP::er &p);

template <>
void recursive_pup_impl<GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >, 1>::operator()(GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > > *obj, PUP::er &p);

template <>
void recursive_pup_impl<GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >, 1>::operator()(GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex > *obj, PUP::er &p);

template <>
void recursive_pup_impl<GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >, 1>::operator()(GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > > *obj, PUP::er &p);

template <>
void recursive_pup_impl<GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >, 1>::operator()(GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex > *obj, PUP::er &p);

template <>
void recursive_pup_impl<GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >, 1>::operator()(GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > > *obj, PUP::er &p);
#endif

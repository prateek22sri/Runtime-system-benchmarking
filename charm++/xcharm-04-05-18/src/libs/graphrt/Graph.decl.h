#ifndef _DECL_Graph_H_
#define _DECL_Graph_H_
#include "charm++.h"
#include "envelope.h"
#include <memory>
#include "sdag.h"
#include "GraphGenerator.decl.h"

#include "NDMeshStreamer.decl.h"

#include "DataTypes.h"

namespace GraphLib {
/* DECLS: template < typename V, typename E, TransportType transportType > array Vertex: ArrayElement{
Vertex(void);
void connectVertex(const E &e);
void addEdge(const E &e);
void recv(const M &m);
void init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator);
Vertex(CkMigrateMessage* impl_msg);
};
 */
template < typename V, typename E, TransportType transportType >  class Vertex;
template < typename V, typename E, TransportType transportType >  class CkIndex_Vertex;
template < typename V, typename E, TransportType transportType >  class CProxy_Vertex;
template < typename V, typename E, TransportType transportType >  class CProxyElement_Vertex;
template < typename V, typename E, TransportType transportType >  class CProxySection_Vertex;
/* --------------- index object ------------------ */
template < typename V, typename E, TransportType transportType > class CkIndex_Vertex:public CkIndex_ArrayElement{
  public:
    typedef Vertex < V, E, transportType >  local_t;
    typedef CkIndex_Vertex < V, E, transportType >  index_t;
    typedef CProxy_Vertex < V, E, transportType >  proxy_t;
    typedef CProxyElement_Vertex < V, E, transportType >  element_t;
    typedef CProxySection_Vertex < V, E, transportType >  section_t;

    static int __idx;
    static void __register(const char *s, size_t size);
    /* DECLS: Vertex(void);
     */
    // Entry point registration at startup
    
    static int reg_Vertex_void();
    // Entry point index lookup
    
    inline static int idx_Vertex_void() {
      static int epidx = reg_Vertex_void();
      return epidx;
    }

    
    static int ckNew(void) { return idx_Vertex_void(); }
    
    static void _call_Vertex_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_Vertex_void(void* impl_msg, void* impl_obj);
    /* DECLS: void connectVertex(const E &e);
     */
    // Entry point registration at startup
    
    static int reg_connectVertex_marshall2();
    // Entry point index lookup
    
    inline static int idx_connectVertex_marshall2() {
      static int epidx = reg_connectVertex_marshall2();
      return epidx;
    }

    
    inline static int idx_connectVertex(void (Vertex < V, E, transportType > ::*)(const E &e) ) {
      return idx_connectVertex_marshall2();
    }


    
    static int connectVertex(const E &e) { return idx_connectVertex_marshall2(); }
    
    static void _call_connectVertex_marshall2(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_connectVertex_marshall2(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_connectVertex_marshall2(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_connectVertex_marshall2(PUP::er &p,void *msg);
    /* DECLS: void addEdge(const E &e);
     */
    // Entry point registration at startup
    
    static int reg_addEdge_marshall3();
    // Entry point index lookup
    
    inline static int idx_addEdge_marshall3() {
      static int epidx = reg_addEdge_marshall3();
      return epidx;
    }

    
    inline static int idx_addEdge(void (Vertex < V, E, transportType > ::*)(const E &e) ) {
      return idx_addEdge_marshall3();
    }


    
    static int addEdge(const E &e) { return idx_addEdge_marshall3(); }
    
    static void _call_addEdge_marshall3(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_addEdge_marshall3(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_addEdge_marshall3(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_addEdge_marshall3(PUP::er &p,void *msg);
    /* DECLS: void recv(const M &m);
     */
    // Entry point registration at startup
    template < typename M > 
    static int reg_recv_marshall4();
    // Entry point index lookup
    template < typename M > 
    inline static int idx_recv_marshall4() {
      static int epidx = reg_recv_marshall4< M >();
      return epidx;
    }

    template < typename M > 
    inline static int idx_recv(void (Vertex < V, E, transportType > ::*)(const M &m) ) {
      return idx_recv_marshall4< M >();
    }


    template < typename M > 
    static int recv(const M &m) { return idx_recv_marshall4< M >(); }
    template < typename M > 
    static void _call_recv_marshall4(void* impl_msg, void* impl_obj);
    template < typename M > 
    static void _call_sdag_recv_marshall4(void* impl_msg, void* impl_obj);
    template < typename M > 
    static int _callmarshall_recv_marshall4(char* impl_buf, void* impl_obj_void);
    template < typename M > 
    static void _marshallmessagepup_recv_marshall4(PUP::er &p,void *msg);
    /* DECLS: void init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator);
     */
    // Entry point registration at startup
    template < typename M > 
    static int reg_init_marshall5();
    // Entry point index lookup
    template < typename M > 
    inline static int idx_init_marshall5() {
      static int epidx = reg_init_marshall5< M >();
      return epidx;
    }

    template < typename M > 
    inline static int idx_init(void (Vertex < V, E, transportType > ::*)(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator) ) {
      return idx_init_marshall5< M >();
    }


    template < typename M > 
    static int init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator) { return idx_init_marshall5< M >(); }
    template < typename M > 
    static void _call_init_marshall5(void* impl_msg, void* impl_obj);
    template < typename M > 
    static void _call_sdag_init_marshall5(void* impl_msg, void* impl_obj);
    template < typename M > 
    static int _callmarshall_init_marshall5(char* impl_buf, void* impl_obj_void);
    template < typename M > 
    static void _marshallmessagepup_init_marshall5(PUP::er &p,void *msg);
    /* DECLS: Vertex(CkMigrateMessage* impl_msg);
     */
    // Entry point registration at startup
    
    static int reg_Vertex_CkMigrateMessage();
    // Entry point index lookup
    
    inline static int idx_Vertex_CkMigrateMessage() {
      static int epidx = reg_Vertex_CkMigrateMessage();
      return epidx;
    }

    
    static int ckNew(CkMigrateMessage* impl_msg) { return idx_Vertex_CkMigrateMessage(); }
    
    static void _call_Vertex_CkMigrateMessage(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_Vertex_CkMigrateMessage(void* impl_msg, void* impl_obj);
};
/* --------------- element proxy ------------------ */
template < typename V, typename E, TransportType transportType >  class CProxyElement_Vertex : public CProxyElement_ArrayElement{
  public:
    typedef Vertex < V, E, transportType >  local_t;
    typedef CkIndex_Vertex < V, E, transportType >  index_t;
    typedef CProxy_Vertex < V, E, transportType >  proxy_t;
    typedef CProxyElement_Vertex < V, E, transportType >  element_t;
    typedef CProxySection_Vertex < V, E, transportType >  section_t;

    CProxyElement_Vertex(void) {}
    CProxyElement_Vertex(const ArrayElement *e) : CProxyElement_ArrayElement(e){  }

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

    Vertex < V, E, transportType >  *ckLocal(void) const
    { return (Vertex < V, E, transportType >  *)CProxyElement_ArrayElement::ckLocal(); }

    CProxyElement_Vertex(const CkArrayID &aid,const CkArrayIndex1D &idx,CK_DELCTOR_PARAM)
        :CProxyElement_ArrayElement(aid,idx,CK_DELCTOR_ARGS)
    {}
    CProxyElement_Vertex(const CkArrayID &aid,const CkArrayIndex1D &idx)
        :CProxyElement_ArrayElement(aid,idx)
    {}

    CProxyElement_Vertex(const CkArrayID &aid,const CkArrayIndex &idx,CK_DELCTOR_PARAM)
        :CProxyElement_ArrayElement(aid,idx,CK_DELCTOR_ARGS)
    {}
    CProxyElement_Vertex(const CkArrayID &aid,const CkArrayIndex &idx)
        :CProxyElement_ArrayElement(aid,idx)
    {}
/* DECLS: Vertex(void);
 */
    
    void insert(int onPE=-1);
/* DECLS: void connectVertex(const E &e);
 */
    
    void connectVertex(const E &e, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void addEdge(const E &e);
 */
    
    void addEdge(const E &e, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void recv(const M &m);
 */
    template < typename M > 
    void recv(const M &m, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator);
 */
    template < typename M > 
    void init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: Vertex(CkMigrateMessage* impl_msg);
 */

};
/* ---------------- collective proxy -------------- */
template < typename V, typename E, TransportType transportType >  class CProxy_Vertex : public CProxy_ArrayElement{
  public:
    typedef Vertex < V, E, transportType >  local_t;
    typedef CkIndex_Vertex < V, E, transportType >  index_t;
    typedef CProxy_Vertex < V, E, transportType >  proxy_t;
    typedef CProxyElement_Vertex < V, E, transportType >  element_t;
    typedef CProxySection_Vertex < V, E, transportType >  section_t;

    CProxy_Vertex(void) {}
    CProxy_Vertex(const ArrayElement *e) : CProxy_ArrayElement(e){  }

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
    CProxyElement_Vertex < V, E, transportType >  operator [] (const CkArrayIndex1D &idx) const
    { return CProxyElement_Vertex < V, E, transportType > (ckGetArrayID(), idx, CK_DELCTOR_CALL); }
    CProxyElement_Vertex < V, E, transportType >  operator() (const CkArrayIndex1D &idx) const
    { return CProxyElement_Vertex < V, E, transportType > (ckGetArrayID(), idx, CK_DELCTOR_CALL); }
    CProxyElement_Vertex < V, E, transportType >  operator [] (int idx) const 
        {return CProxyElement_Vertex < V, E, transportType > (ckGetArrayID(), CkArrayIndex1D(idx), CK_DELCTOR_CALL);}
    CProxyElement_Vertex < V, E, transportType >  operator () (int idx) const 
        {return CProxyElement_Vertex < V, E, transportType > (ckGetArrayID(), CkArrayIndex1D(idx), CK_DELCTOR_CALL);}
    CProxy_Vertex(const CkArrayID &aid,CK_DELCTOR_PARAM) 
        :CProxy_ArrayElement(aid,CK_DELCTOR_ARGS) {}
    CProxy_Vertex(const CkArrayID &aid) 
        :CProxy_ArrayElement(aid) {}
/* DECLS: Vertex(void);
 */
    
    static CkArrayID ckNew(const CkArrayOptions &opts = CkArrayOptions());
    static void      ckNew(const CkArrayOptions &opts, CkCallback _ck_array_creation_cb);
    static CkArrayID ckNew(const int s1);
    static void ckNew(const int s1, CkCallback _ck_array_creation_cb);

/* DECLS: void connectVertex(const E &e);
 */
    
    void connectVertex(const E &e, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void addEdge(const E &e);
 */
    
    void addEdge(const E &e, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void recv(const M &m);
 */
    template < typename M > 
    void recv(const M &m, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator);
 */
    template < typename M > 
    void init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: Vertex(CkMigrateMessage* impl_msg);
 */

};
/* ---------------- section proxy -------------- */
template < typename V, typename E, TransportType transportType >  class CProxySection_Vertex : public CProxySection_ArrayElement{
  public:
    typedef Vertex < V, E, transportType >  local_t;
    typedef CkIndex_Vertex < V, E, transportType >  index_t;
    typedef CProxy_Vertex < V, E, transportType >  proxy_t;
    typedef CProxyElement_Vertex < V, E, transportType >  element_t;
    typedef CProxySection_Vertex < V, E, transportType >  section_t;

    CProxySection_Vertex(void) {}

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
    CProxyElement_Vertex < V, E, transportType >  operator [] (const CkArrayIndex1D &idx) const
        {return CProxyElement_Vertex < V, E, transportType > (ckGetArrayID(), idx, CK_DELCTOR_CALL);}
    CProxyElement_Vertex < V, E, transportType >  operator() (const CkArrayIndex1D &idx) const
        {return CProxyElement_Vertex < V, E, transportType > (ckGetArrayID(), idx, CK_DELCTOR_CALL);}
    CProxyElement_Vertex < V, E, transportType >  operator [] (int idx) const 
        {return CProxyElement_Vertex < V, E, transportType > (ckGetArrayID(), *(CkArrayIndex1D*)&ckGetArrayElements()[idx], CK_DELCTOR_CALL);}
    CProxyElement_Vertex < V, E, transportType >  operator () (int idx) const 
        {return CProxyElement_Vertex < V, E, transportType > (ckGetArrayID(), *(CkArrayIndex1D*)&ckGetArrayElements()[idx], CK_DELCTOR_CALL);}
    static CkSectionID ckNew(const CkArrayID &aid, CkArrayIndex1D *elems, int nElems) {
      return CkSectionID(aid, elems, nElems);
    } 
    static CkSectionID ckNew(const CkArrayID &aid, int l, int u, int s) {
      CkVec<CkArrayIndex1D> al;
      for (int i=l; i<=u; i+=s) al.push_back(CkArrayIndex1D(i));
      return CkSectionID(aid, al.getVec(), al.size());
    } 
    CProxySection_Vertex(const CkArrayID &aid, CkArrayIndex *elems, int nElems, CK_DELCTOR_PARAM) 
        :CProxySection_ArrayElement(aid,elems,nElems,CK_DELCTOR_ARGS) {}
    CProxySection_Vertex(const CkArrayID &aid, CkArrayIndex *elems, int nElems) 
        :CProxySection_ArrayElement(aid,elems,nElems) {}
    CProxySection_Vertex(const CkSectionID &sid)       :CProxySection_ArrayElement(sid) {}
    CProxySection_Vertex(int n, const CkArrayID *aid, CkArrayIndex const * const *elems, const int *nElems, CK_DELCTOR_PARAM) 
        :CProxySection_ArrayElement(n,aid,elems,nElems,CK_DELCTOR_ARGS) {}
    CProxySection_Vertex(int n, const CkArrayID *aid, CkArrayIndex const * const *elems, const int *nElems) 
        :CProxySection_ArrayElement(n,aid,elems,nElems) {}
    static CkSectionID ckNew(const CkArrayID &aid, CkArrayIndex *elems, int nElems) {
      return CkSectionID(aid, elems, nElems);
    } 
/* DECLS: Vertex(void);
 */
    

/* DECLS: void connectVertex(const E &e);
 */
    
    void connectVertex(const E &e, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void addEdge(const E &e);
 */
    
    void addEdge(const E &e, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void recv(const M &m);
 */
    template < typename M > 
    void recv(const M &m, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: void init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator);
 */
    template < typename M > 
    void init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator, const CkEntryOptions *impl_e_opts=NULL) ;

/* DECLS: Vertex(CkMigrateMessage* impl_msg);
 */

};
#define Vertex_SDAG_CODE 
template < typename V, typename E, TransportType transportType > 
struct CBase_Vertex;

} // namespace GraphLib




namespace GraphLib {
/* ---------------- method closures -------------- */
template < typename V, typename E, TransportType transportType > class Closure_Vertex {
  public:


    struct connectVertex_2_closure;


    struct addEdge_3_closure;

template < typename M > 
    struct recv_4_closure;

template < typename M > 
    struct init_5_closure;


};

} // namespace GraphLib

extern void _registerGraph(void);
#endif

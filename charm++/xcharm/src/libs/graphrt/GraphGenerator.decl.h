#ifndef _DECL_GraphGenerator_H_
#define _DECL_GraphGenerator_H_
#include "charm++.h"
#include "envelope.h"
#include <memory>
#include "sdag.h"
#include "NDMeshStreamer.decl.h"

namespace GraphLib {
/* DECLS: template < class Graph, class Opts, VertexMapping vertexMapping > group SSCA_Generator_Charm: IrrGroup{
SSCA_Generator_Charm(const Graph &g, const Opts &opts);
threaded void generate(const CkCallback &cb);
void do_cliqueGeneration(void);
void do_edgeGeneration(void);
void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
void dump_vertices(void);
void dump_cliques(void);
void dump_edges(void);
};
 */
template < class Graph, class Opts, VertexMapping vertexMapping >  class SSCA_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CkIndex_SSCA_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CProxy_SSCA_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CProxyElement_SSCA_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CProxySection_SSCA_Generator_Charm;
/* --------------- index object ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping > class CkIndex_SSCA_Generator_Charm:public CkIndex_IrrGroup{
  public:
    typedef SSCA_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    static int __idx;
    static void __register(const char *s, size_t size);
    /* DECLS: SSCA_Generator_Charm(const Graph &g, const Opts &opts);
     */
    // Entry point registration at startup
    
    static int reg_SSCA_Generator_Charm_marshall1();
    // Entry point index lookup
    
    inline static int idx_SSCA_Generator_Charm_marshall1() {
      static int epidx = reg_SSCA_Generator_Charm_marshall1();
      return epidx;
    }

    
    static int ckNew(const Graph &g, const Opts &opts) { return idx_SSCA_Generator_Charm_marshall1(); }
    
    static void _call_SSCA_Generator_Charm_marshall1(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_SSCA_Generator_Charm_marshall1(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_SSCA_Generator_Charm_marshall1(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_SSCA_Generator_Charm_marshall1(PUP::er &p,void *msg);
    /* DECLS: threaded void generate(const CkCallback &cb);
     */
    // Entry point registration at startup
    
    static int reg_generate_marshall2();
    // Entry point index lookup
    
    inline static int idx_generate_marshall2() {
      static int epidx = reg_generate_marshall2();
      return epidx;
    }

    
    inline static int idx_generate(void (SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::*)(const CkCallback &cb) ) {
      return idx_generate_marshall2();
    }


    
    static int generate(const CkCallback &cb) { return idx_generate_marshall2(); }
    
    static void _call_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _callthr_generate_marshall2(CkThrCallArg *);
    
    static void _marshallmessagepup_generate_marshall2(PUP::er &p,void *msg);
    /* DECLS: void do_cliqueGeneration(void);
     */
    // Entry point registration at startup
    
    static int reg_do_cliqueGeneration_void();
    // Entry point index lookup
    
    inline static int idx_do_cliqueGeneration_void() {
      static int epidx = reg_do_cliqueGeneration_void();
      return epidx;
    }

    
    inline static int idx_do_cliqueGeneration(void (SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::*)(void) ) {
      return idx_do_cliqueGeneration_void();
    }


    
    static int do_cliqueGeneration(void) { return idx_do_cliqueGeneration_void(); }
    
    static void _call_do_cliqueGeneration_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_do_cliqueGeneration_void(void* impl_msg, void* impl_obj);
    /* DECLS: void do_edgeGeneration(void);
     */
    // Entry point registration at startup
    
    static int reg_do_edgeGeneration_void();
    // Entry point index lookup
    
    inline static int idx_do_edgeGeneration_void() {
      static int epidx = reg_do_edgeGeneration_void();
      return epidx;
    }

    
    inline static int idx_do_edgeGeneration(void (SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::*)(void) ) {
      return idx_do_edgeGeneration_void();
    }


    
    static int do_edgeGeneration(void) { return idx_do_edgeGeneration_void(); }
    
    static void _call_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
    /* DECLS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
     */
    // Entry point registration at startup
    
    static int reg_addVertexToClique_marshall5();
    // Entry point index lookup
    
    inline static int idx_addVertexToClique_marshall5() {
      static int epidx = reg_addVertexToClique_marshall5();
      return epidx;
    }

    
    inline static int idx_addVertexToClique(void (SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::*)(const CmiUInt8 &v, const CmiUInt8 &c) ) {
      return idx_addVertexToClique_marshall5();
    }


    
    static int addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c) { return idx_addVertexToClique_marshall5(); }
    
    static void _call_addVertexToClique_marshall5(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_addVertexToClique_marshall5(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_addVertexToClique_marshall5(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_addVertexToClique_marshall5(PUP::er &p,void *msg);
    /* DECLS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
     */
    // Entry point registration at startup
    
    static int reg_addInterCliqueEdge_marshall6();
    // Entry point index lookup
    
    inline static int idx_addInterCliqueEdge_marshall6() {
      static int epidx = reg_addInterCliqueEdge_marshall6();
      return epidx;
    }

    
    inline static int idx_addInterCliqueEdge(void (SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::*)(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv) ) {
      return idx_addInterCliqueEdge_marshall6();
    }


    
    static int addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv) { return idx_addInterCliqueEdge_marshall6(); }
    
    static void _call_addInterCliqueEdge_marshall6(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_addInterCliqueEdge_marshall6(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_addInterCliqueEdge_marshall6(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_addInterCliqueEdge_marshall6(PUP::er &p,void *msg);
    /* DECLS: void dump_vertices(void);
     */
    // Entry point registration at startup
    
    static int reg_dump_vertices_void();
    // Entry point index lookup
    
    inline static int idx_dump_vertices_void() {
      static int epidx = reg_dump_vertices_void();
      return epidx;
    }

    
    inline static int idx_dump_vertices(void (SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::*)(void) ) {
      return idx_dump_vertices_void();
    }


    
    static int dump_vertices(void) { return idx_dump_vertices_void(); }
    
    static void _call_dump_vertices_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_dump_vertices_void(void* impl_msg, void* impl_obj);
    /* DECLS: void dump_cliques(void);
     */
    // Entry point registration at startup
    
    static int reg_dump_cliques_void();
    // Entry point index lookup
    
    inline static int idx_dump_cliques_void() {
      static int epidx = reg_dump_cliques_void();
      return epidx;
    }

    
    inline static int idx_dump_cliques(void (SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::*)(void) ) {
      return idx_dump_cliques_void();
    }


    
    static int dump_cliques(void) { return idx_dump_cliques_void(); }
    
    static void _call_dump_cliques_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_dump_cliques_void(void* impl_msg, void* impl_obj);
    /* DECLS: void dump_edges(void);
     */
    // Entry point registration at startup
    
    static int reg_dump_edges_void();
    // Entry point index lookup
    
    inline static int idx_dump_edges_void() {
      static int epidx = reg_dump_edges_void();
      return epidx;
    }

    
    inline static int idx_dump_edges(void (SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::*)(void) ) {
      return idx_dump_edges_void();
    }


    
    static int dump_edges(void) { return idx_dump_edges_void(); }
    
    static void _call_dump_edges_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_dump_edges_void(void* impl_msg, void* impl_obj);
};
/* --------------- element proxy ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping > class CProxyElement_SSCA_Generator_Charm: public CProxyElement_IrrGroup{
  public:
    typedef SSCA_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    CProxyElement_SSCA_Generator_Charm(void) {}
    CProxyElement_SSCA_Generator_Charm(const IrrGroup *g) : CProxyElement_IrrGroup(g){  }
    CProxyElement_SSCA_Generator_Charm(CkGroupID _gid,int _onPE,CK_DELCTOR_PARAM) : CProxyElement_IrrGroup(_gid,_onPE,CK_DELCTOR_ARGS){  }
    CProxyElement_SSCA_Generator_Charm(CkGroupID _gid,int _onPE) : CProxyElement_IrrGroup(_gid,_onPE){  }

    int ckIsDelegated(void) const
    { return CProxyElement_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxyElement_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxyElement_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxyElement_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxyElement_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxyElement_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxyElement_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxyElement_IrrGroup::ckSetReductionClient(cb); }
int ckGetGroupPe(void) const
{return CProxyElement_IrrGroup::ckGetGroupPe();}

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxyElement_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxyElement_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxyElement_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxyElement_IrrGroup::ckSetGroupID(g);
    }
    SSCA_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static SSCA_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(CkGroupID gID) {
      return (SSCA_Generator_Charm < Graph, Opts, vertexMapping > *)CkLocalBranch(gID);
    }
/* DECLS: SSCA_Generator_Charm(const Graph &g, const Opts &opts);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_cliqueGeneration(void);
 */
    
    void do_cliqueGeneration(void);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

/* DECLS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
 */
    
    void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
 */
    
    void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void dump_vertices(void);
 */
    
    void dump_vertices(void);

/* DECLS: void dump_cliques(void);
 */
    
    void dump_cliques(void);

/* DECLS: void dump_edges(void);
 */
    
    void dump_edges(void);

};
/* ---------------- collective proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping > class CProxy_SSCA_Generator_Charm: public CProxy_IrrGroup{
  public:
    typedef SSCA_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    CProxy_SSCA_Generator_Charm(void) {}
    CProxy_SSCA_Generator_Charm(const IrrGroup *g) : CProxy_IrrGroup(g){  }
    CProxy_SSCA_Generator_Charm(CkGroupID _gid,CK_DELCTOR_PARAM) : CProxy_IrrGroup(_gid,CK_DELCTOR_ARGS){  }
    CProxy_SSCA_Generator_Charm(CkGroupID _gid) : CProxy_IrrGroup(_gid){  }
    CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  operator[](int onPE) const
      {return CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping > (ckGetGroupID(),onPE,CK_DELCTOR_CALL);}

    int ckIsDelegated(void) const
    { return CProxy_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxy_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxy_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxy_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxy_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxy_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxy_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxy_IrrGroup::ckSetReductionClient(cb); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxy_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxy_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxy_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxy_IrrGroup::ckSetGroupID(g);
    }
    SSCA_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static SSCA_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(CkGroupID gID) {
      return (SSCA_Generator_Charm < Graph, Opts, vertexMapping > *)CkLocalBranch(gID);
    }
/* DECLS: SSCA_Generator_Charm(const Graph &g, const Opts &opts);
 */
    
    static CkGroupID ckNew(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts=NULL);
    CProxy_SSCA_Generator_Charm(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_cliqueGeneration(void);
 */
    
    void do_cliqueGeneration(void);
    
    void do_cliqueGeneration(int npes, int *pes);
    
    void do_cliqueGeneration(CmiGroup &grp);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);
    
    void do_edgeGeneration(int npes, int *pes);
    
    void do_edgeGeneration(CmiGroup &grp);

/* DECLS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
 */
    
    void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, const CkEntryOptions *impl_e_opts=NULL);
    
    void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, int npes, int *pes, const CkEntryOptions *impl_e_opts=NULL);
    
    void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, CmiGroup &grp, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
 */
    
    void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, const CkEntryOptions *impl_e_opts=NULL);
    
    void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, int npes, int *pes, const CkEntryOptions *impl_e_opts=NULL);
    
    void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, CmiGroup &grp, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void dump_vertices(void);
 */
    
    void dump_vertices(void);
    
    void dump_vertices(int npes, int *pes);
    
    void dump_vertices(CmiGroup &grp);

/* DECLS: void dump_cliques(void);
 */
    
    void dump_cliques(void);
    
    void dump_cliques(int npes, int *pes);
    
    void dump_cliques(CmiGroup &grp);

/* DECLS: void dump_edges(void);
 */
    
    void dump_edges(void);
    
    void dump_edges(int npes, int *pes);
    
    void dump_edges(CmiGroup &grp);

};
/* ---------------- section proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping > class CProxySection_SSCA_Generator_Charm: public CProxySection_IrrGroup{
  public:
    typedef SSCA_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    CProxySection_SSCA_Generator_Charm(void) {}
    CProxySection_SSCA_Generator_Charm(const IrrGroup *g) : CProxySection_IrrGroup(g){  }
    CProxySection_SSCA_Generator_Charm(const CkGroupID &_gid,const int *_pelist,int _npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }
    CProxySection_SSCA_Generator_Charm(const CkGroupID &_gid,const int *_pelist,int _npes) : CProxySection_IrrGroup(_gid,_pelist,_npes){  }
    CProxySection_SSCA_Generator_Charm(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes) : CProxySection_IrrGroup(n,_gid,_pelist,_npes){  }
    CProxySection_SSCA_Generator_Charm(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(n,_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }

    int ckIsDelegated(void) const
    { return CProxySection_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxySection_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxySection_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxySection_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxySection_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxySection_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxySection_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxySection_IrrGroup::ckSetReductionClient(cb); }
inline int ckGetNumSections() const
{ return CProxySection_IrrGroup::ckGetNumSections(); }
inline CkSectionInfo &ckGetSectionInfo()
{ return CProxySection_IrrGroup::ckGetSectionInfo(); }
inline CkSectionID *ckGetSectionIDs()
{ return CProxySection_IrrGroup::ckGetSectionIDs(); }
inline CkSectionID &ckGetSectionID()
{ return CProxySection_IrrGroup::ckGetSectionID(); }
inline CkSectionID &ckGetSectionID(int i)
{ return CProxySection_IrrGroup::ckGetSectionID(i); }
inline CkGroupID ckGetGroupIDn(int i) const
{ return CProxySection_IrrGroup::ckGetGroupIDn(i); }
inline int *ckGetElements() const
{ return CProxySection_IrrGroup::ckGetElements(); }
inline int *ckGetElements(int i) const
{ return CProxySection_IrrGroup::ckGetElements(i); }
inline int ckGetNumElements() const
{ return CProxySection_IrrGroup::ckGetNumElements(); } 
inline int ckGetNumElements(int i) const
{ return CProxySection_IrrGroup::ckGetNumElements(i); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxySection_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxySection_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxySection_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxySection_IrrGroup::ckSetGroupID(g);
    }
    SSCA_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static SSCA_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(CkGroupID gID) {
      return (SSCA_Generator_Charm < Graph, Opts, vertexMapping > *)CkLocalBranch(gID);
    }
/* DECLS: SSCA_Generator_Charm(const Graph &g, const Opts &opts);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_cliqueGeneration(void);
 */
    
    void do_cliqueGeneration(void);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

/* DECLS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
 */
    
    void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
 */
    
    void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void dump_vertices(void);
 */
    
    void dump_vertices(void);

/* DECLS: void dump_cliques(void);
 */
    
    void dump_cliques(void);

/* DECLS: void dump_edges(void);
 */
    
    void dump_edges(void);

};
#define SSCA_Generator_Charm_SDAG_CODE 
template < class Graph, class Opts, VertexMapping vertexMapping > 
struct CBase_SSCA_Generator_Charm;

/* DECLS: template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > group SSCA_Generator_Tram: IrrGroup{
SSCA_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
threaded void generate(const CkCallback &cb);
void do_cliqueGeneration(void);
void do_edgeGeneration(void);
void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
void dump_vertices(void);
void dump_cliques(void);
void dump_edges(void);
};
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class SSCA_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CkIndex_SSCA_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CProxy_SSCA_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CProxyElement_SSCA_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CProxySection_SSCA_Generator_Tram;
/* --------------- index object ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CkIndex_SSCA_Generator_Tram:public CkIndex_IrrGroup{
  public:
    typedef SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    static int __idx;
    static void __register(const char *s, size_t size);
    /* DECLS: SSCA_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
     */
    // Entry point registration at startup
    
    static int reg_SSCA_Generator_Tram_marshall1();
    // Entry point index lookup
    
    inline static int idx_SSCA_Generator_Tram_marshall1() {
      static int epidx = reg_SSCA_Generator_Tram_marshall1();
      return epidx;
    }

    
    static int ckNew(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator) { return idx_SSCA_Generator_Tram_marshall1(); }
    
    static void _call_SSCA_Generator_Tram_marshall1(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_SSCA_Generator_Tram_marshall1(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_SSCA_Generator_Tram_marshall1(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_SSCA_Generator_Tram_marshall1(PUP::er &p,void *msg);
    /* DECLS: threaded void generate(const CkCallback &cb);
     */
    // Entry point registration at startup
    
    static int reg_generate_marshall2();
    // Entry point index lookup
    
    inline static int idx_generate_marshall2() {
      static int epidx = reg_generate_marshall2();
      return epidx;
    }

    
    inline static int idx_generate(void (SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(const CkCallback &cb) ) {
      return idx_generate_marshall2();
    }


    
    static int generate(const CkCallback &cb) { return idx_generate_marshall2(); }
    
    static void _call_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _callthr_generate_marshall2(CkThrCallArg *);
    
    static void _marshallmessagepup_generate_marshall2(PUP::er &p,void *msg);
    /* DECLS: void do_cliqueGeneration(void);
     */
    // Entry point registration at startup
    
    static int reg_do_cliqueGeneration_void();
    // Entry point index lookup
    
    inline static int idx_do_cliqueGeneration_void() {
      static int epidx = reg_do_cliqueGeneration_void();
      return epidx;
    }

    
    inline static int idx_do_cliqueGeneration(void (SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(void) ) {
      return idx_do_cliqueGeneration_void();
    }


    
    static int do_cliqueGeneration(void) { return idx_do_cliqueGeneration_void(); }
    
    static void _call_do_cliqueGeneration_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_do_cliqueGeneration_void(void* impl_msg, void* impl_obj);
    /* DECLS: void do_edgeGeneration(void);
     */
    // Entry point registration at startup
    
    static int reg_do_edgeGeneration_void();
    // Entry point index lookup
    
    inline static int idx_do_edgeGeneration_void() {
      static int epidx = reg_do_edgeGeneration_void();
      return epidx;
    }

    
    inline static int idx_do_edgeGeneration(void (SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(void) ) {
      return idx_do_edgeGeneration_void();
    }


    
    static int do_edgeGeneration(void) { return idx_do_edgeGeneration_void(); }
    
    static void _call_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
    /* DECLS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
     */
    // Entry point registration at startup
    
    static int reg_addVertexToClique_marshall5();
    // Entry point index lookup
    
    inline static int idx_addVertexToClique_marshall5() {
      static int epidx = reg_addVertexToClique_marshall5();
      return epidx;
    }

    
    inline static int idx_addVertexToClique(void (SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(const CmiUInt8 &v, const CmiUInt8 &c) ) {
      return idx_addVertexToClique_marshall5();
    }


    
    static int addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c) { return idx_addVertexToClique_marshall5(); }
    
    static void _call_addVertexToClique_marshall5(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_addVertexToClique_marshall5(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_addVertexToClique_marshall5(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_addVertexToClique_marshall5(PUP::er &p,void *msg);
    /* DECLS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
     */
    // Entry point registration at startup
    
    static int reg_addInterCliqueEdge_marshall6();
    // Entry point index lookup
    
    inline static int idx_addInterCliqueEdge_marshall6() {
      static int epidx = reg_addInterCliqueEdge_marshall6();
      return epidx;
    }

    
    inline static int idx_addInterCliqueEdge(void (SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv) ) {
      return idx_addInterCliqueEdge_marshall6();
    }


    
    static int addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv) { return idx_addInterCliqueEdge_marshall6(); }
    
    static void _call_addInterCliqueEdge_marshall6(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_addInterCliqueEdge_marshall6(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_addInterCliqueEdge_marshall6(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_addInterCliqueEdge_marshall6(PUP::er &p,void *msg);
    /* DECLS: void dump_vertices(void);
     */
    // Entry point registration at startup
    
    static int reg_dump_vertices_void();
    // Entry point index lookup
    
    inline static int idx_dump_vertices_void() {
      static int epidx = reg_dump_vertices_void();
      return epidx;
    }

    
    inline static int idx_dump_vertices(void (SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(void) ) {
      return idx_dump_vertices_void();
    }


    
    static int dump_vertices(void) { return idx_dump_vertices_void(); }
    
    static void _call_dump_vertices_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_dump_vertices_void(void* impl_msg, void* impl_obj);
    /* DECLS: void dump_cliques(void);
     */
    // Entry point registration at startup
    
    static int reg_dump_cliques_void();
    // Entry point index lookup
    
    inline static int idx_dump_cliques_void() {
      static int epidx = reg_dump_cliques_void();
      return epidx;
    }

    
    inline static int idx_dump_cliques(void (SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(void) ) {
      return idx_dump_cliques_void();
    }


    
    static int dump_cliques(void) { return idx_dump_cliques_void(); }
    
    static void _call_dump_cliques_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_dump_cliques_void(void* impl_msg, void* impl_obj);
    /* DECLS: void dump_edges(void);
     */
    // Entry point registration at startup
    
    static int reg_dump_edges_void();
    // Entry point index lookup
    
    inline static int idx_dump_edges_void() {
      static int epidx = reg_dump_edges_void();
      return epidx;
    }

    
    inline static int idx_dump_edges(void (SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(void) ) {
      return idx_dump_edges_void();
    }


    
    static int dump_edges(void) { return idx_dump_edges_void(); }
    
    static void _call_dump_edges_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_dump_edges_void(void* impl_msg, void* impl_obj);
};
/* --------------- element proxy ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CProxyElement_SSCA_Generator_Tram: public CProxyElement_IrrGroup{
  public:
    typedef SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    CProxyElement_SSCA_Generator_Tram(void) {}
    CProxyElement_SSCA_Generator_Tram(const IrrGroup *g) : CProxyElement_IrrGroup(g){  }
    CProxyElement_SSCA_Generator_Tram(CkGroupID _gid,int _onPE,CK_DELCTOR_PARAM) : CProxyElement_IrrGroup(_gid,_onPE,CK_DELCTOR_ARGS){  }
    CProxyElement_SSCA_Generator_Tram(CkGroupID _gid,int _onPE) : CProxyElement_IrrGroup(_gid,_onPE){  }

    int ckIsDelegated(void) const
    { return CProxyElement_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxyElement_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxyElement_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxyElement_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxyElement_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxyElement_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxyElement_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxyElement_IrrGroup::ckSetReductionClient(cb); }
int ckGetGroupPe(void) const
{return CProxyElement_IrrGroup::ckGetGroupPe();}

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxyElement_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxyElement_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxyElement_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxyElement_IrrGroup::ckSetGroupID(g);
    }
    SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(CkGroupID gID) {
      return (SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > *)CkLocalBranch(gID);
    }
/* DECLS: SSCA_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_cliqueGeneration(void);
 */
    
    void do_cliqueGeneration(void);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

/* DECLS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
 */
    
    void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
 */
    
    void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void dump_vertices(void);
 */
    
    void dump_vertices(void);

/* DECLS: void dump_cliques(void);
 */
    
    void dump_cliques(void);

/* DECLS: void dump_edges(void);
 */
    
    void dump_edges(void);

};
/* ---------------- collective proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CProxy_SSCA_Generator_Tram: public CProxy_IrrGroup{
  public:
    typedef SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    CProxy_SSCA_Generator_Tram(void) {}
    CProxy_SSCA_Generator_Tram(const IrrGroup *g) : CProxy_IrrGroup(g){  }
    CProxy_SSCA_Generator_Tram(CkGroupID _gid,CK_DELCTOR_PARAM) : CProxy_IrrGroup(_gid,CK_DELCTOR_ARGS){  }
    CProxy_SSCA_Generator_Tram(CkGroupID _gid) : CProxy_IrrGroup(_gid){  }
    CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  operator[](int onPE) const
      {return CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > (ckGetGroupID(),onPE,CK_DELCTOR_CALL);}

    int ckIsDelegated(void) const
    { return CProxy_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxy_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxy_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxy_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxy_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxy_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxy_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxy_IrrGroup::ckSetReductionClient(cb); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxy_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxy_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxy_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxy_IrrGroup::ckSetGroupID(g);
    }
    SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(CkGroupID gID) {
      return (SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > *)CkLocalBranch(gID);
    }
/* DECLS: SSCA_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
    
    static CkGroupID ckNew(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts=NULL);
    CProxy_SSCA_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_cliqueGeneration(void);
 */
    
    void do_cliqueGeneration(void);
    
    void do_cliqueGeneration(int npes, int *pes);
    
    void do_cliqueGeneration(CmiGroup &grp);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);
    
    void do_edgeGeneration(int npes, int *pes);
    
    void do_edgeGeneration(CmiGroup &grp);

/* DECLS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
 */
    
    void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, const CkEntryOptions *impl_e_opts=NULL);
    
    void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, int npes, int *pes, const CkEntryOptions *impl_e_opts=NULL);
    
    void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, CmiGroup &grp, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
 */
    
    void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, const CkEntryOptions *impl_e_opts=NULL);
    
    void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, int npes, int *pes, const CkEntryOptions *impl_e_opts=NULL);
    
    void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, CmiGroup &grp, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void dump_vertices(void);
 */
    
    void dump_vertices(void);
    
    void dump_vertices(int npes, int *pes);
    
    void dump_vertices(CmiGroup &grp);

/* DECLS: void dump_cliques(void);
 */
    
    void dump_cliques(void);
    
    void dump_cliques(int npes, int *pes);
    
    void dump_cliques(CmiGroup &grp);

/* DECLS: void dump_edges(void);
 */
    
    void dump_edges(void);
    
    void dump_edges(int npes, int *pes);
    
    void dump_edges(CmiGroup &grp);

};
/* ---------------- section proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CProxySection_SSCA_Generator_Tram: public CProxySection_IrrGroup{
  public:
    typedef SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    CProxySection_SSCA_Generator_Tram(void) {}
    CProxySection_SSCA_Generator_Tram(const IrrGroup *g) : CProxySection_IrrGroup(g){  }
    CProxySection_SSCA_Generator_Tram(const CkGroupID &_gid,const int *_pelist,int _npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }
    CProxySection_SSCA_Generator_Tram(const CkGroupID &_gid,const int *_pelist,int _npes) : CProxySection_IrrGroup(_gid,_pelist,_npes){  }
    CProxySection_SSCA_Generator_Tram(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes) : CProxySection_IrrGroup(n,_gid,_pelist,_npes){  }
    CProxySection_SSCA_Generator_Tram(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(n,_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }

    int ckIsDelegated(void) const
    { return CProxySection_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxySection_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxySection_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxySection_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxySection_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxySection_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxySection_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxySection_IrrGroup::ckSetReductionClient(cb); }
inline int ckGetNumSections() const
{ return CProxySection_IrrGroup::ckGetNumSections(); }
inline CkSectionInfo &ckGetSectionInfo()
{ return CProxySection_IrrGroup::ckGetSectionInfo(); }
inline CkSectionID *ckGetSectionIDs()
{ return CProxySection_IrrGroup::ckGetSectionIDs(); }
inline CkSectionID &ckGetSectionID()
{ return CProxySection_IrrGroup::ckGetSectionID(); }
inline CkSectionID &ckGetSectionID(int i)
{ return CProxySection_IrrGroup::ckGetSectionID(i); }
inline CkGroupID ckGetGroupIDn(int i) const
{ return CProxySection_IrrGroup::ckGetGroupIDn(i); }
inline int *ckGetElements() const
{ return CProxySection_IrrGroup::ckGetElements(); }
inline int *ckGetElements(int i) const
{ return CProxySection_IrrGroup::ckGetElements(i); }
inline int ckGetNumElements() const
{ return CProxySection_IrrGroup::ckGetNumElements(); } 
inline int ckGetNumElements(int i) const
{ return CProxySection_IrrGroup::ckGetNumElements(i); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxySection_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxySection_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxySection_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxySection_IrrGroup::ckSetGroupID(g);
    }
    SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(CkGroupID gID) {
      return (SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > *)CkLocalBranch(gID);
    }
/* DECLS: SSCA_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_cliqueGeneration(void);
 */
    
    void do_cliqueGeneration(void);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

/* DECLS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
 */
    
    void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
 */
    
    void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void dump_vertices(void);
 */
    
    void dump_vertices(void);

/* DECLS: void dump_cliques(void);
 */
    
    void dump_cliques(void);

/* DECLS: void dump_edges(void);
 */
    
    void dump_edges(void);

};
#define SSCA_Generator_Tram_SDAG_CODE 
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 
struct CBase_SSCA_Generator_Tram;

/* DECLS: template < class Graph, class Opts, VertexMapping vertexMapping > group RMAT_Generator_Charm: IrrGroup{
RMAT_Generator_Charm(const Graph &g, const Opts &opts);
threaded void generate(const CkCallback &cb);
void do_edgeGeneration(void);
};
 */
template < class Graph, class Opts, VertexMapping vertexMapping >  class RMAT_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CkIndex_RMAT_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CProxy_RMAT_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CProxyElement_RMAT_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CProxySection_RMAT_Generator_Charm;
/* --------------- index object ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping > class CkIndex_RMAT_Generator_Charm:public CkIndex_IrrGroup{
  public:
    typedef RMAT_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    static int __idx;
    static void __register(const char *s, size_t size);
    /* DECLS: RMAT_Generator_Charm(const Graph &g, const Opts &opts);
     */
    // Entry point registration at startup
    
    static int reg_RMAT_Generator_Charm_marshall1();
    // Entry point index lookup
    
    inline static int idx_RMAT_Generator_Charm_marshall1() {
      static int epidx = reg_RMAT_Generator_Charm_marshall1();
      return epidx;
    }

    
    static int ckNew(const Graph &g, const Opts &opts) { return idx_RMAT_Generator_Charm_marshall1(); }
    
    static void _call_RMAT_Generator_Charm_marshall1(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_RMAT_Generator_Charm_marshall1(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_RMAT_Generator_Charm_marshall1(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_RMAT_Generator_Charm_marshall1(PUP::er &p,void *msg);
    /* DECLS: threaded void generate(const CkCallback &cb);
     */
    // Entry point registration at startup
    
    static int reg_generate_marshall2();
    // Entry point index lookup
    
    inline static int idx_generate_marshall2() {
      static int epidx = reg_generate_marshall2();
      return epidx;
    }

    
    inline static int idx_generate(void (RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::*)(const CkCallback &cb) ) {
      return idx_generate_marshall2();
    }


    
    static int generate(const CkCallback &cb) { return idx_generate_marshall2(); }
    
    static void _call_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _callthr_generate_marshall2(CkThrCallArg *);
    
    static void _marshallmessagepup_generate_marshall2(PUP::er &p,void *msg);
    /* DECLS: void do_edgeGeneration(void);
     */
    // Entry point registration at startup
    
    static int reg_do_edgeGeneration_void();
    // Entry point index lookup
    
    inline static int idx_do_edgeGeneration_void() {
      static int epidx = reg_do_edgeGeneration_void();
      return epidx;
    }

    
    inline static int idx_do_edgeGeneration(void (RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::*)(void) ) {
      return idx_do_edgeGeneration_void();
    }


    
    static int do_edgeGeneration(void) { return idx_do_edgeGeneration_void(); }
    
    static void _call_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
};
/* --------------- element proxy ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping > class CProxyElement_RMAT_Generator_Charm: public CProxyElement_IrrGroup{
  public:
    typedef RMAT_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    CProxyElement_RMAT_Generator_Charm(void) {}
    CProxyElement_RMAT_Generator_Charm(const IrrGroup *g) : CProxyElement_IrrGroup(g){  }
    CProxyElement_RMAT_Generator_Charm(CkGroupID _gid,int _onPE,CK_DELCTOR_PARAM) : CProxyElement_IrrGroup(_gid,_onPE,CK_DELCTOR_ARGS){  }
    CProxyElement_RMAT_Generator_Charm(CkGroupID _gid,int _onPE) : CProxyElement_IrrGroup(_gid,_onPE){  }

    int ckIsDelegated(void) const
    { return CProxyElement_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxyElement_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxyElement_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxyElement_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxyElement_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxyElement_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxyElement_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxyElement_IrrGroup::ckSetReductionClient(cb); }
int ckGetGroupPe(void) const
{return CProxyElement_IrrGroup::ckGetGroupPe();}

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxyElement_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxyElement_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxyElement_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxyElement_IrrGroup::ckSetGroupID(g);
    }
    RMAT_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static RMAT_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(CkGroupID gID) {
      return (RMAT_Generator_Charm < Graph, Opts, vertexMapping > *)CkLocalBranch(gID);
    }
/* DECLS: RMAT_Generator_Charm(const Graph &g, const Opts &opts);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

};
/* ---------------- collective proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping > class CProxy_RMAT_Generator_Charm: public CProxy_IrrGroup{
  public:
    typedef RMAT_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    CProxy_RMAT_Generator_Charm(void) {}
    CProxy_RMAT_Generator_Charm(const IrrGroup *g) : CProxy_IrrGroup(g){  }
    CProxy_RMAT_Generator_Charm(CkGroupID _gid,CK_DELCTOR_PARAM) : CProxy_IrrGroup(_gid,CK_DELCTOR_ARGS){  }
    CProxy_RMAT_Generator_Charm(CkGroupID _gid) : CProxy_IrrGroup(_gid){  }
    CProxyElement_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  operator[](int onPE) const
      {return CProxyElement_RMAT_Generator_Charm < Graph, Opts, vertexMapping > (ckGetGroupID(),onPE,CK_DELCTOR_CALL);}

    int ckIsDelegated(void) const
    { return CProxy_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxy_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxy_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxy_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxy_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxy_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxy_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxy_IrrGroup::ckSetReductionClient(cb); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxy_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxy_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxy_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxy_IrrGroup::ckSetGroupID(g);
    }
    RMAT_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static RMAT_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(CkGroupID gID) {
      return (RMAT_Generator_Charm < Graph, Opts, vertexMapping > *)CkLocalBranch(gID);
    }
/* DECLS: RMAT_Generator_Charm(const Graph &g, const Opts &opts);
 */
    
    static CkGroupID ckNew(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts=NULL);
    CProxy_RMAT_Generator_Charm(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);
    
    void do_edgeGeneration(int npes, int *pes);
    
    void do_edgeGeneration(CmiGroup &grp);

};
/* ---------------- section proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping > class CProxySection_RMAT_Generator_Charm: public CProxySection_IrrGroup{
  public:
    typedef RMAT_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    CProxySection_RMAT_Generator_Charm(void) {}
    CProxySection_RMAT_Generator_Charm(const IrrGroup *g) : CProxySection_IrrGroup(g){  }
    CProxySection_RMAT_Generator_Charm(const CkGroupID &_gid,const int *_pelist,int _npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }
    CProxySection_RMAT_Generator_Charm(const CkGroupID &_gid,const int *_pelist,int _npes) : CProxySection_IrrGroup(_gid,_pelist,_npes){  }
    CProxySection_RMAT_Generator_Charm(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes) : CProxySection_IrrGroup(n,_gid,_pelist,_npes){  }
    CProxySection_RMAT_Generator_Charm(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(n,_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }

    int ckIsDelegated(void) const
    { return CProxySection_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxySection_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxySection_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxySection_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxySection_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxySection_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxySection_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxySection_IrrGroup::ckSetReductionClient(cb); }
inline int ckGetNumSections() const
{ return CProxySection_IrrGroup::ckGetNumSections(); }
inline CkSectionInfo &ckGetSectionInfo()
{ return CProxySection_IrrGroup::ckGetSectionInfo(); }
inline CkSectionID *ckGetSectionIDs()
{ return CProxySection_IrrGroup::ckGetSectionIDs(); }
inline CkSectionID &ckGetSectionID()
{ return CProxySection_IrrGroup::ckGetSectionID(); }
inline CkSectionID &ckGetSectionID(int i)
{ return CProxySection_IrrGroup::ckGetSectionID(i); }
inline CkGroupID ckGetGroupIDn(int i) const
{ return CProxySection_IrrGroup::ckGetGroupIDn(i); }
inline int *ckGetElements() const
{ return CProxySection_IrrGroup::ckGetElements(); }
inline int *ckGetElements(int i) const
{ return CProxySection_IrrGroup::ckGetElements(i); }
inline int ckGetNumElements() const
{ return CProxySection_IrrGroup::ckGetNumElements(); } 
inline int ckGetNumElements(int i) const
{ return CProxySection_IrrGroup::ckGetNumElements(i); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxySection_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxySection_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxySection_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxySection_IrrGroup::ckSetGroupID(g);
    }
    RMAT_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static RMAT_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(CkGroupID gID) {
      return (RMAT_Generator_Charm < Graph, Opts, vertexMapping > *)CkLocalBranch(gID);
    }
/* DECLS: RMAT_Generator_Charm(const Graph &g, const Opts &opts);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

};
#define RMAT_Generator_Charm_SDAG_CODE 
template < class Graph, class Opts, VertexMapping vertexMapping > 
struct CBase_RMAT_Generator_Charm;

/* DECLS: template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > group RMAT_Generator_Tram: IrrGroup{
RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
threaded void generate(const CkCallback &cb);
void do_edgeGeneration(void);
};
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class RMAT_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CkIndex_RMAT_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CProxy_RMAT_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CProxyElement_RMAT_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CProxySection_RMAT_Generator_Tram;
/* --------------- index object ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CkIndex_RMAT_Generator_Tram:public CkIndex_IrrGroup{
  public:
    typedef RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    static int __idx;
    static void __register(const char *s, size_t size);
    /* DECLS: RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
     */
    // Entry point registration at startup
    
    static int reg_RMAT_Generator_Tram_marshall1();
    // Entry point index lookup
    
    inline static int idx_RMAT_Generator_Tram_marshall1() {
      static int epidx = reg_RMAT_Generator_Tram_marshall1();
      return epidx;
    }

    
    static int ckNew(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator) { return idx_RMAT_Generator_Tram_marshall1(); }
    
    static void _call_RMAT_Generator_Tram_marshall1(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_RMAT_Generator_Tram_marshall1(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_RMAT_Generator_Tram_marshall1(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_RMAT_Generator_Tram_marshall1(PUP::er &p,void *msg);
    /* DECLS: threaded void generate(const CkCallback &cb);
     */
    // Entry point registration at startup
    
    static int reg_generate_marshall2();
    // Entry point index lookup
    
    inline static int idx_generate_marshall2() {
      static int epidx = reg_generate_marshall2();
      return epidx;
    }

    
    inline static int idx_generate(void (RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(const CkCallback &cb) ) {
      return idx_generate_marshall2();
    }


    
    static int generate(const CkCallback &cb) { return idx_generate_marshall2(); }
    
    static void _call_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _callthr_generate_marshall2(CkThrCallArg *);
    
    static void _marshallmessagepup_generate_marshall2(PUP::er &p,void *msg);
    /* DECLS: void do_edgeGeneration(void);
     */
    // Entry point registration at startup
    
    static int reg_do_edgeGeneration_void();
    // Entry point index lookup
    
    inline static int idx_do_edgeGeneration_void() {
      static int epidx = reg_do_edgeGeneration_void();
      return epidx;
    }

    
    inline static int idx_do_edgeGeneration(void (RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(void) ) {
      return idx_do_edgeGeneration_void();
    }


    
    static int do_edgeGeneration(void) { return idx_do_edgeGeneration_void(); }
    
    static void _call_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
};
/* --------------- element proxy ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CProxyElement_RMAT_Generator_Tram: public CProxyElement_IrrGroup{
  public:
    typedef RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    CProxyElement_RMAT_Generator_Tram(void) {}
    CProxyElement_RMAT_Generator_Tram(const IrrGroup *g) : CProxyElement_IrrGroup(g){  }
    CProxyElement_RMAT_Generator_Tram(CkGroupID _gid,int _onPE,CK_DELCTOR_PARAM) : CProxyElement_IrrGroup(_gid,_onPE,CK_DELCTOR_ARGS){  }
    CProxyElement_RMAT_Generator_Tram(CkGroupID _gid,int _onPE) : CProxyElement_IrrGroup(_gid,_onPE){  }

    int ckIsDelegated(void) const
    { return CProxyElement_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxyElement_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxyElement_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxyElement_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxyElement_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxyElement_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxyElement_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxyElement_IrrGroup::ckSetReductionClient(cb); }
int ckGetGroupPe(void) const
{return CProxyElement_IrrGroup::ckGetGroupPe();}

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxyElement_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxyElement_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxyElement_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxyElement_IrrGroup::ckSetGroupID(g);
    }
    RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(CkGroupID gID) {
      return (RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > *)CkLocalBranch(gID);
    }
/* DECLS: RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

};
/* ---------------- collective proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CProxy_RMAT_Generator_Tram: public CProxy_IrrGroup{
  public:
    typedef RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    CProxy_RMAT_Generator_Tram(void) {}
    CProxy_RMAT_Generator_Tram(const IrrGroup *g) : CProxy_IrrGroup(g){  }
    CProxy_RMAT_Generator_Tram(CkGroupID _gid,CK_DELCTOR_PARAM) : CProxy_IrrGroup(_gid,CK_DELCTOR_ARGS){  }
    CProxy_RMAT_Generator_Tram(CkGroupID _gid) : CProxy_IrrGroup(_gid){  }
    CProxyElement_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  operator[](int onPE) const
      {return CProxyElement_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > (ckGetGroupID(),onPE,CK_DELCTOR_CALL);}

    int ckIsDelegated(void) const
    { return CProxy_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxy_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxy_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxy_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxy_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxy_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxy_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxy_IrrGroup::ckSetReductionClient(cb); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxy_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxy_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxy_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxy_IrrGroup::ckSetGroupID(g);
    }
    RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(CkGroupID gID) {
      return (RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > *)CkLocalBranch(gID);
    }
/* DECLS: RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
    
    static CkGroupID ckNew(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts=NULL);
    CProxy_RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);
    
    void do_edgeGeneration(int npes, int *pes);
    
    void do_edgeGeneration(CmiGroup &grp);

};
/* ---------------- section proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CProxySection_RMAT_Generator_Tram: public CProxySection_IrrGroup{
  public:
    typedef RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    CProxySection_RMAT_Generator_Tram(void) {}
    CProxySection_RMAT_Generator_Tram(const IrrGroup *g) : CProxySection_IrrGroup(g){  }
    CProxySection_RMAT_Generator_Tram(const CkGroupID &_gid,const int *_pelist,int _npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }
    CProxySection_RMAT_Generator_Tram(const CkGroupID &_gid,const int *_pelist,int _npes) : CProxySection_IrrGroup(_gid,_pelist,_npes){  }
    CProxySection_RMAT_Generator_Tram(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes) : CProxySection_IrrGroup(n,_gid,_pelist,_npes){  }
    CProxySection_RMAT_Generator_Tram(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(n,_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }

    int ckIsDelegated(void) const
    { return CProxySection_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxySection_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxySection_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxySection_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxySection_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxySection_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxySection_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxySection_IrrGroup::ckSetReductionClient(cb); }
inline int ckGetNumSections() const
{ return CProxySection_IrrGroup::ckGetNumSections(); }
inline CkSectionInfo &ckGetSectionInfo()
{ return CProxySection_IrrGroup::ckGetSectionInfo(); }
inline CkSectionID *ckGetSectionIDs()
{ return CProxySection_IrrGroup::ckGetSectionIDs(); }
inline CkSectionID &ckGetSectionID()
{ return CProxySection_IrrGroup::ckGetSectionID(); }
inline CkSectionID &ckGetSectionID(int i)
{ return CProxySection_IrrGroup::ckGetSectionID(i); }
inline CkGroupID ckGetGroupIDn(int i) const
{ return CProxySection_IrrGroup::ckGetGroupIDn(i); }
inline int *ckGetElements() const
{ return CProxySection_IrrGroup::ckGetElements(); }
inline int *ckGetElements(int i) const
{ return CProxySection_IrrGroup::ckGetElements(i); }
inline int ckGetNumElements() const
{ return CProxySection_IrrGroup::ckGetNumElements(); } 
inline int ckGetNumElements(int i) const
{ return CProxySection_IrrGroup::ckGetNumElements(i); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxySection_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxySection_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxySection_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxySection_IrrGroup::ckSetGroupID(g);
    }
    RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(CkGroupID gID) {
      return (RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > *)CkLocalBranch(gID);
    }
/* DECLS: RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

};
#define RMAT_Generator_Tram_SDAG_CODE 
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 
struct CBase_RMAT_Generator_Tram;

/* DECLS: template < class Graph, class Opts, VertexMapping vertexMapping > group Random_Generator_Charm: IrrGroup{
Random_Generator_Charm(const Graph &g, const Opts &opts);
threaded void generate(const CkCallback &cb);
void do_edgeGeneration(void);
};
 */
template < class Graph, class Opts, VertexMapping vertexMapping >  class Random_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CkIndex_Random_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CProxy_Random_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CProxyElement_Random_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CProxySection_Random_Generator_Charm;
/* --------------- index object ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping > class CkIndex_Random_Generator_Charm:public CkIndex_IrrGroup{
  public:
    typedef Random_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_Random_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_Random_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    static int __idx;
    static void __register(const char *s, size_t size);
    /* DECLS: Random_Generator_Charm(const Graph &g, const Opts &opts);
     */
    // Entry point registration at startup
    
    static int reg_Random_Generator_Charm_marshall1();
    // Entry point index lookup
    
    inline static int idx_Random_Generator_Charm_marshall1() {
      static int epidx = reg_Random_Generator_Charm_marshall1();
      return epidx;
    }

    
    static int ckNew(const Graph &g, const Opts &opts) { return idx_Random_Generator_Charm_marshall1(); }
    
    static void _call_Random_Generator_Charm_marshall1(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_Random_Generator_Charm_marshall1(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_Random_Generator_Charm_marshall1(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_Random_Generator_Charm_marshall1(PUP::er &p,void *msg);
    /* DECLS: threaded void generate(const CkCallback &cb);
     */
    // Entry point registration at startup
    
    static int reg_generate_marshall2();
    // Entry point index lookup
    
    inline static int idx_generate_marshall2() {
      static int epidx = reg_generate_marshall2();
      return epidx;
    }

    
    inline static int idx_generate(void (Random_Generator_Charm < Graph, Opts, vertexMapping > ::*)(const CkCallback &cb) ) {
      return idx_generate_marshall2();
    }


    
    static int generate(const CkCallback &cb) { return idx_generate_marshall2(); }
    
    static void _call_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _callthr_generate_marshall2(CkThrCallArg *);
    
    static void _marshallmessagepup_generate_marshall2(PUP::er &p,void *msg);
    /* DECLS: void do_edgeGeneration(void);
     */
    // Entry point registration at startup
    
    static int reg_do_edgeGeneration_void();
    // Entry point index lookup
    
    inline static int idx_do_edgeGeneration_void() {
      static int epidx = reg_do_edgeGeneration_void();
      return epidx;
    }

    
    inline static int idx_do_edgeGeneration(void (Random_Generator_Charm < Graph, Opts, vertexMapping > ::*)(void) ) {
      return idx_do_edgeGeneration_void();
    }


    
    static int do_edgeGeneration(void) { return idx_do_edgeGeneration_void(); }
    
    static void _call_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
};
/* --------------- element proxy ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping > class CProxyElement_Random_Generator_Charm: public CProxyElement_IrrGroup{
  public:
    typedef Random_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_Random_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_Random_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    CProxyElement_Random_Generator_Charm(void) {}
    CProxyElement_Random_Generator_Charm(const IrrGroup *g) : CProxyElement_IrrGroup(g){  }
    CProxyElement_Random_Generator_Charm(CkGroupID _gid,int _onPE,CK_DELCTOR_PARAM) : CProxyElement_IrrGroup(_gid,_onPE,CK_DELCTOR_ARGS){  }
    CProxyElement_Random_Generator_Charm(CkGroupID _gid,int _onPE) : CProxyElement_IrrGroup(_gid,_onPE){  }

    int ckIsDelegated(void) const
    { return CProxyElement_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxyElement_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxyElement_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxyElement_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxyElement_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxyElement_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxyElement_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxyElement_IrrGroup::ckSetReductionClient(cb); }
int ckGetGroupPe(void) const
{return CProxyElement_IrrGroup::ckGetGroupPe();}

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxyElement_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxyElement_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxyElement_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxyElement_IrrGroup::ckSetGroupID(g);
    }
    Random_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static Random_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(CkGroupID gID) {
      return (Random_Generator_Charm < Graph, Opts, vertexMapping > *)CkLocalBranch(gID);
    }
/* DECLS: Random_Generator_Charm(const Graph &g, const Opts &opts);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

};
/* ---------------- collective proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping > class CProxy_Random_Generator_Charm: public CProxy_IrrGroup{
  public:
    typedef Random_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_Random_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_Random_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    CProxy_Random_Generator_Charm(void) {}
    CProxy_Random_Generator_Charm(const IrrGroup *g) : CProxy_IrrGroup(g){  }
    CProxy_Random_Generator_Charm(CkGroupID _gid,CK_DELCTOR_PARAM) : CProxy_IrrGroup(_gid,CK_DELCTOR_ARGS){  }
    CProxy_Random_Generator_Charm(CkGroupID _gid) : CProxy_IrrGroup(_gid){  }
    CProxyElement_Random_Generator_Charm < Graph, Opts, vertexMapping >  operator[](int onPE) const
      {return CProxyElement_Random_Generator_Charm < Graph, Opts, vertexMapping > (ckGetGroupID(),onPE,CK_DELCTOR_CALL);}

    int ckIsDelegated(void) const
    { return CProxy_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxy_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxy_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxy_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxy_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxy_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxy_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxy_IrrGroup::ckSetReductionClient(cb); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxy_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxy_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxy_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxy_IrrGroup::ckSetGroupID(g);
    }
    Random_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static Random_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(CkGroupID gID) {
      return (Random_Generator_Charm < Graph, Opts, vertexMapping > *)CkLocalBranch(gID);
    }
/* DECLS: Random_Generator_Charm(const Graph &g, const Opts &opts);
 */
    
    static CkGroupID ckNew(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts=NULL);
    CProxy_Random_Generator_Charm(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);
    
    void do_edgeGeneration(int npes, int *pes);
    
    void do_edgeGeneration(CmiGroup &grp);

};
/* ---------------- section proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping > class CProxySection_Random_Generator_Charm: public CProxySection_IrrGroup{
  public:
    typedef Random_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_Random_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_Random_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    CProxySection_Random_Generator_Charm(void) {}
    CProxySection_Random_Generator_Charm(const IrrGroup *g) : CProxySection_IrrGroup(g){  }
    CProxySection_Random_Generator_Charm(const CkGroupID &_gid,const int *_pelist,int _npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }
    CProxySection_Random_Generator_Charm(const CkGroupID &_gid,const int *_pelist,int _npes) : CProxySection_IrrGroup(_gid,_pelist,_npes){  }
    CProxySection_Random_Generator_Charm(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes) : CProxySection_IrrGroup(n,_gid,_pelist,_npes){  }
    CProxySection_Random_Generator_Charm(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(n,_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }

    int ckIsDelegated(void) const
    { return CProxySection_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxySection_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxySection_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxySection_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxySection_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxySection_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxySection_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxySection_IrrGroup::ckSetReductionClient(cb); }
inline int ckGetNumSections() const
{ return CProxySection_IrrGroup::ckGetNumSections(); }
inline CkSectionInfo &ckGetSectionInfo()
{ return CProxySection_IrrGroup::ckGetSectionInfo(); }
inline CkSectionID *ckGetSectionIDs()
{ return CProxySection_IrrGroup::ckGetSectionIDs(); }
inline CkSectionID &ckGetSectionID()
{ return CProxySection_IrrGroup::ckGetSectionID(); }
inline CkSectionID &ckGetSectionID(int i)
{ return CProxySection_IrrGroup::ckGetSectionID(i); }
inline CkGroupID ckGetGroupIDn(int i) const
{ return CProxySection_IrrGroup::ckGetGroupIDn(i); }
inline int *ckGetElements() const
{ return CProxySection_IrrGroup::ckGetElements(); }
inline int *ckGetElements(int i) const
{ return CProxySection_IrrGroup::ckGetElements(i); }
inline int ckGetNumElements() const
{ return CProxySection_IrrGroup::ckGetNumElements(); } 
inline int ckGetNumElements(int i) const
{ return CProxySection_IrrGroup::ckGetNumElements(i); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxySection_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxySection_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxySection_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxySection_IrrGroup::ckSetGroupID(g);
    }
    Random_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static Random_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(CkGroupID gID) {
      return (Random_Generator_Charm < Graph, Opts, vertexMapping > *)CkLocalBranch(gID);
    }
/* DECLS: Random_Generator_Charm(const Graph &g, const Opts &opts);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

};
#define Random_Generator_Charm_SDAG_CODE 
template < class Graph, class Opts, VertexMapping vertexMapping > 
struct CBase_Random_Generator_Charm;

/* DECLS: template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > group Random_Generator_Tram: IrrGroup{
Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
threaded void generate(const CkCallback &cb);
void do_edgeGeneration(void);
};
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class Random_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CkIndex_Random_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CProxy_Random_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CProxyElement_Random_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CProxySection_Random_Generator_Tram;
/* --------------- index object ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CkIndex_Random_Generator_Tram:public CkIndex_IrrGroup{
  public:
    typedef Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    static int __idx;
    static void __register(const char *s, size_t size);
    /* DECLS: Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
     */
    // Entry point registration at startup
    
    static int reg_Random_Generator_Tram_marshall1();
    // Entry point index lookup
    
    inline static int idx_Random_Generator_Tram_marshall1() {
      static int epidx = reg_Random_Generator_Tram_marshall1();
      return epidx;
    }

    
    static int ckNew(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator) { return idx_Random_Generator_Tram_marshall1(); }
    
    static void _call_Random_Generator_Tram_marshall1(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_Random_Generator_Tram_marshall1(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_Random_Generator_Tram_marshall1(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_Random_Generator_Tram_marshall1(PUP::er &p,void *msg);
    /* DECLS: threaded void generate(const CkCallback &cb);
     */
    // Entry point registration at startup
    
    static int reg_generate_marshall2();
    // Entry point index lookup
    
    inline static int idx_generate_marshall2() {
      static int epidx = reg_generate_marshall2();
      return epidx;
    }

    
    inline static int idx_generate(void (Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(const CkCallback &cb) ) {
      return idx_generate_marshall2();
    }


    
    static int generate(const CkCallback &cb) { return idx_generate_marshall2(); }
    
    static void _call_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _callthr_generate_marshall2(CkThrCallArg *);
    
    static void _marshallmessagepup_generate_marshall2(PUP::er &p,void *msg);
    /* DECLS: void do_edgeGeneration(void);
     */
    // Entry point registration at startup
    
    static int reg_do_edgeGeneration_void();
    // Entry point index lookup
    
    inline static int idx_do_edgeGeneration_void() {
      static int epidx = reg_do_edgeGeneration_void();
      return epidx;
    }

    
    inline static int idx_do_edgeGeneration(void (Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(void) ) {
      return idx_do_edgeGeneration_void();
    }


    
    static int do_edgeGeneration(void) { return idx_do_edgeGeneration_void(); }
    
    static void _call_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
};
/* --------------- element proxy ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CProxyElement_Random_Generator_Tram: public CProxyElement_IrrGroup{
  public:
    typedef Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    CProxyElement_Random_Generator_Tram(void) {}
    CProxyElement_Random_Generator_Tram(const IrrGroup *g) : CProxyElement_IrrGroup(g){  }
    CProxyElement_Random_Generator_Tram(CkGroupID _gid,int _onPE,CK_DELCTOR_PARAM) : CProxyElement_IrrGroup(_gid,_onPE,CK_DELCTOR_ARGS){  }
    CProxyElement_Random_Generator_Tram(CkGroupID _gid,int _onPE) : CProxyElement_IrrGroup(_gid,_onPE){  }

    int ckIsDelegated(void) const
    { return CProxyElement_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxyElement_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxyElement_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxyElement_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxyElement_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxyElement_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxyElement_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxyElement_IrrGroup::ckSetReductionClient(cb); }
int ckGetGroupPe(void) const
{return CProxyElement_IrrGroup::ckGetGroupPe();}

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxyElement_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxyElement_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxyElement_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxyElement_IrrGroup::ckSetGroupID(g);
    }
    Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(CkGroupID gID) {
      return (Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > *)CkLocalBranch(gID);
    }
/* DECLS: Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

};
/* ---------------- collective proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CProxy_Random_Generator_Tram: public CProxy_IrrGroup{
  public:
    typedef Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    CProxy_Random_Generator_Tram(void) {}
    CProxy_Random_Generator_Tram(const IrrGroup *g) : CProxy_IrrGroup(g){  }
    CProxy_Random_Generator_Tram(CkGroupID _gid,CK_DELCTOR_PARAM) : CProxy_IrrGroup(_gid,CK_DELCTOR_ARGS){  }
    CProxy_Random_Generator_Tram(CkGroupID _gid) : CProxy_IrrGroup(_gid){  }
    CProxyElement_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  operator[](int onPE) const
      {return CProxyElement_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > (ckGetGroupID(),onPE,CK_DELCTOR_CALL);}

    int ckIsDelegated(void) const
    { return CProxy_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxy_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxy_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxy_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxy_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxy_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxy_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxy_IrrGroup::ckSetReductionClient(cb); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxy_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxy_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxy_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxy_IrrGroup::ckSetGroupID(g);
    }
    Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(CkGroupID gID) {
      return (Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > *)CkLocalBranch(gID);
    }
/* DECLS: Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
    
    static CkGroupID ckNew(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts=NULL);
    CProxy_Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);
    
    void do_edgeGeneration(int npes, int *pes);
    
    void do_edgeGeneration(CmiGroup &grp);

};
/* ---------------- section proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CProxySection_Random_Generator_Tram: public CProxySection_IrrGroup{
  public:
    typedef Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    CProxySection_Random_Generator_Tram(void) {}
    CProxySection_Random_Generator_Tram(const IrrGroup *g) : CProxySection_IrrGroup(g){  }
    CProxySection_Random_Generator_Tram(const CkGroupID &_gid,const int *_pelist,int _npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }
    CProxySection_Random_Generator_Tram(const CkGroupID &_gid,const int *_pelist,int _npes) : CProxySection_IrrGroup(_gid,_pelist,_npes){  }
    CProxySection_Random_Generator_Tram(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes) : CProxySection_IrrGroup(n,_gid,_pelist,_npes){  }
    CProxySection_Random_Generator_Tram(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(n,_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }

    int ckIsDelegated(void) const
    { return CProxySection_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxySection_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxySection_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxySection_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxySection_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxySection_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxySection_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxySection_IrrGroup::ckSetReductionClient(cb); }
inline int ckGetNumSections() const
{ return CProxySection_IrrGroup::ckGetNumSections(); }
inline CkSectionInfo &ckGetSectionInfo()
{ return CProxySection_IrrGroup::ckGetSectionInfo(); }
inline CkSectionID *ckGetSectionIDs()
{ return CProxySection_IrrGroup::ckGetSectionIDs(); }
inline CkSectionID &ckGetSectionID()
{ return CProxySection_IrrGroup::ckGetSectionID(); }
inline CkSectionID &ckGetSectionID(int i)
{ return CProxySection_IrrGroup::ckGetSectionID(i); }
inline CkGroupID ckGetGroupIDn(int i) const
{ return CProxySection_IrrGroup::ckGetGroupIDn(i); }
inline int *ckGetElements() const
{ return CProxySection_IrrGroup::ckGetElements(); }
inline int *ckGetElements(int i) const
{ return CProxySection_IrrGroup::ckGetElements(i); }
inline int ckGetNumElements() const
{ return CProxySection_IrrGroup::ckGetNumElements(); } 
inline int ckGetNumElements(int i) const
{ return CProxySection_IrrGroup::ckGetNumElements(i); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxySection_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxySection_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxySection_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxySection_IrrGroup::ckSetGroupID(g);
    }
    Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(CkGroupID gID) {
      return (Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > *)CkLocalBranch(gID);
    }
/* DECLS: Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

};
#define Random_Generator_Tram_SDAG_CODE 
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 
struct CBase_Random_Generator_Tram;

/* DECLS: template < class Graph, class Opts, VertexMapping vertexMapping > group Ring_Generator_Charm: IrrGroup{
Ring_Generator_Charm(const Graph &g, const Opts &opts);
threaded void generate(const CkCallback &cb);
void do_edgeGeneration(void);
};
 */
template < class Graph, class Opts, VertexMapping vertexMapping >  class Ring_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CkIndex_Ring_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CProxy_Ring_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CProxyElement_Ring_Generator_Charm;
template < class Graph, class Opts, VertexMapping vertexMapping >  class CProxySection_Ring_Generator_Charm;
/* --------------- index object ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping > class CkIndex_Ring_Generator_Charm:public CkIndex_IrrGroup{
  public:
    typedef Ring_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_Ring_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_Ring_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    static int __idx;
    static void __register(const char *s, size_t size);
    /* DECLS: Ring_Generator_Charm(const Graph &g, const Opts &opts);
     */
    // Entry point registration at startup
    
    static int reg_Ring_Generator_Charm_marshall1();
    // Entry point index lookup
    
    inline static int idx_Ring_Generator_Charm_marshall1() {
      static int epidx = reg_Ring_Generator_Charm_marshall1();
      return epidx;
    }

    
    static int ckNew(const Graph &g, const Opts &opts) { return idx_Ring_Generator_Charm_marshall1(); }
    
    static void _call_Ring_Generator_Charm_marshall1(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_Ring_Generator_Charm_marshall1(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_Ring_Generator_Charm_marshall1(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_Ring_Generator_Charm_marshall1(PUP::er &p,void *msg);
    /* DECLS: threaded void generate(const CkCallback &cb);
     */
    // Entry point registration at startup
    
    static int reg_generate_marshall2();
    // Entry point index lookup
    
    inline static int idx_generate_marshall2() {
      static int epidx = reg_generate_marshall2();
      return epidx;
    }

    
    inline static int idx_generate(void (Ring_Generator_Charm < Graph, Opts, vertexMapping > ::*)(const CkCallback &cb) ) {
      return idx_generate_marshall2();
    }


    
    static int generate(const CkCallback &cb) { return idx_generate_marshall2(); }
    
    static void _call_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _callthr_generate_marshall2(CkThrCallArg *);
    
    static void _marshallmessagepup_generate_marshall2(PUP::er &p,void *msg);
    /* DECLS: void do_edgeGeneration(void);
     */
    // Entry point registration at startup
    
    static int reg_do_edgeGeneration_void();
    // Entry point index lookup
    
    inline static int idx_do_edgeGeneration_void() {
      static int epidx = reg_do_edgeGeneration_void();
      return epidx;
    }

    
    inline static int idx_do_edgeGeneration(void (Ring_Generator_Charm < Graph, Opts, vertexMapping > ::*)(void) ) {
      return idx_do_edgeGeneration_void();
    }


    
    static int do_edgeGeneration(void) { return idx_do_edgeGeneration_void(); }
    
    static void _call_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
};
/* --------------- element proxy ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping > class CProxyElement_Ring_Generator_Charm: public CProxyElement_IrrGroup{
  public:
    typedef Ring_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_Ring_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_Ring_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    CProxyElement_Ring_Generator_Charm(void) {}
    CProxyElement_Ring_Generator_Charm(const IrrGroup *g) : CProxyElement_IrrGroup(g){  }
    CProxyElement_Ring_Generator_Charm(CkGroupID _gid,int _onPE,CK_DELCTOR_PARAM) : CProxyElement_IrrGroup(_gid,_onPE,CK_DELCTOR_ARGS){  }
    CProxyElement_Ring_Generator_Charm(CkGroupID _gid,int _onPE) : CProxyElement_IrrGroup(_gid,_onPE){  }

    int ckIsDelegated(void) const
    { return CProxyElement_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxyElement_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxyElement_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxyElement_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxyElement_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxyElement_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxyElement_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxyElement_IrrGroup::ckSetReductionClient(cb); }
int ckGetGroupPe(void) const
{return CProxyElement_IrrGroup::ckGetGroupPe();}

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxyElement_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxyElement_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxyElement_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxyElement_IrrGroup::ckSetGroupID(g);
    }
    Ring_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static Ring_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(CkGroupID gID) {
      return (Ring_Generator_Charm < Graph, Opts, vertexMapping > *)CkLocalBranch(gID);
    }
/* DECLS: Ring_Generator_Charm(const Graph &g, const Opts &opts);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

};
/* ---------------- collective proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping > class CProxy_Ring_Generator_Charm: public CProxy_IrrGroup{
  public:
    typedef Ring_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_Ring_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_Ring_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    CProxy_Ring_Generator_Charm(void) {}
    CProxy_Ring_Generator_Charm(const IrrGroup *g) : CProxy_IrrGroup(g){  }
    CProxy_Ring_Generator_Charm(CkGroupID _gid,CK_DELCTOR_PARAM) : CProxy_IrrGroup(_gid,CK_DELCTOR_ARGS){  }
    CProxy_Ring_Generator_Charm(CkGroupID _gid) : CProxy_IrrGroup(_gid){  }
    CProxyElement_Ring_Generator_Charm < Graph, Opts, vertexMapping >  operator[](int onPE) const
      {return CProxyElement_Ring_Generator_Charm < Graph, Opts, vertexMapping > (ckGetGroupID(),onPE,CK_DELCTOR_CALL);}

    int ckIsDelegated(void) const
    { return CProxy_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxy_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxy_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxy_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxy_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxy_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxy_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxy_IrrGroup::ckSetReductionClient(cb); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxy_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxy_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxy_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxy_IrrGroup::ckSetGroupID(g);
    }
    Ring_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static Ring_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(CkGroupID gID) {
      return (Ring_Generator_Charm < Graph, Opts, vertexMapping > *)CkLocalBranch(gID);
    }
/* DECLS: Ring_Generator_Charm(const Graph &g, const Opts &opts);
 */
    
    static CkGroupID ckNew(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts=NULL);
    CProxy_Ring_Generator_Charm(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);
    
    void do_edgeGeneration(int npes, int *pes);
    
    void do_edgeGeneration(CmiGroup &grp);

};
/* ---------------- section proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping > class CProxySection_Ring_Generator_Charm: public CProxySection_IrrGroup{
  public:
    typedef Ring_Generator_Charm < Graph, Opts, vertexMapping >  local_t;
    typedef CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping >  index_t;
    typedef CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping >  proxy_t;
    typedef CProxyElement_Ring_Generator_Charm < Graph, Opts, vertexMapping >  element_t;
    typedef CProxySection_Ring_Generator_Charm < Graph, Opts, vertexMapping >  section_t;

    CProxySection_Ring_Generator_Charm(void) {}
    CProxySection_Ring_Generator_Charm(const IrrGroup *g) : CProxySection_IrrGroup(g){  }
    CProxySection_Ring_Generator_Charm(const CkGroupID &_gid,const int *_pelist,int _npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }
    CProxySection_Ring_Generator_Charm(const CkGroupID &_gid,const int *_pelist,int _npes) : CProxySection_IrrGroup(_gid,_pelist,_npes){  }
    CProxySection_Ring_Generator_Charm(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes) : CProxySection_IrrGroup(n,_gid,_pelist,_npes){  }
    CProxySection_Ring_Generator_Charm(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(n,_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }

    int ckIsDelegated(void) const
    { return CProxySection_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxySection_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxySection_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxySection_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxySection_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxySection_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxySection_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxySection_IrrGroup::ckSetReductionClient(cb); }
inline int ckGetNumSections() const
{ return CProxySection_IrrGroup::ckGetNumSections(); }
inline CkSectionInfo &ckGetSectionInfo()
{ return CProxySection_IrrGroup::ckGetSectionInfo(); }
inline CkSectionID *ckGetSectionIDs()
{ return CProxySection_IrrGroup::ckGetSectionIDs(); }
inline CkSectionID &ckGetSectionID()
{ return CProxySection_IrrGroup::ckGetSectionID(); }
inline CkSectionID &ckGetSectionID(int i)
{ return CProxySection_IrrGroup::ckGetSectionID(i); }
inline CkGroupID ckGetGroupIDn(int i) const
{ return CProxySection_IrrGroup::ckGetGroupIDn(i); }
inline int *ckGetElements() const
{ return CProxySection_IrrGroup::ckGetElements(); }
inline int *ckGetElements(int i) const
{ return CProxySection_IrrGroup::ckGetElements(i); }
inline int ckGetNumElements() const
{ return CProxySection_IrrGroup::ckGetNumElements(); } 
inline int ckGetNumElements(int i) const
{ return CProxySection_IrrGroup::ckGetNumElements(i); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxySection_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxySection_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxySection_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxySection_IrrGroup::ckSetGroupID(g);
    }
    Ring_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static Ring_Generator_Charm < Graph, Opts, vertexMapping > * ckLocalBranch(CkGroupID gID) {
      return (Ring_Generator_Charm < Graph, Opts, vertexMapping > *)CkLocalBranch(gID);
    }
/* DECLS: Ring_Generator_Charm(const Graph &g, const Opts &opts);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

};
#define Ring_Generator_Charm_SDAG_CODE 
template < class Graph, class Opts, VertexMapping vertexMapping > 
struct CBase_Ring_Generator_Charm;

/* DECLS: template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > group Ring_Generator_Tram: IrrGroup{
Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
threaded void generate(const CkCallback &cb);
void do_edgeGeneration(void);
};
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class Ring_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CkIndex_Ring_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CProxy_Ring_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CProxyElement_Ring_Generator_Tram;
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  class CProxySection_Ring_Generator_Tram;
/* --------------- index object ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CkIndex_Ring_Generator_Tram:public CkIndex_IrrGroup{
  public:
    typedef Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    static int __idx;
    static void __register(const char *s, size_t size);
    /* DECLS: Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
     */
    // Entry point registration at startup
    
    static int reg_Ring_Generator_Tram_marshall1();
    // Entry point index lookup
    
    inline static int idx_Ring_Generator_Tram_marshall1() {
      static int epidx = reg_Ring_Generator_Tram_marshall1();
      return epidx;
    }

    
    static int ckNew(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator) { return idx_Ring_Generator_Tram_marshall1(); }
    
    static void _call_Ring_Generator_Tram_marshall1(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_Ring_Generator_Tram_marshall1(void* impl_msg, void* impl_obj);
    
    static int _callmarshall_Ring_Generator_Tram_marshall1(char* impl_buf, void* impl_obj_void);
    
    static void _marshallmessagepup_Ring_Generator_Tram_marshall1(PUP::er &p,void *msg);
    /* DECLS: threaded void generate(const CkCallback &cb);
     */
    // Entry point registration at startup
    
    static int reg_generate_marshall2();
    // Entry point index lookup
    
    inline static int idx_generate_marshall2() {
      static int epidx = reg_generate_marshall2();
      return epidx;
    }

    
    inline static int idx_generate(void (Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(const CkCallback &cb) ) {
      return idx_generate_marshall2();
    }


    
    static int generate(const CkCallback &cb) { return idx_generate_marshall2(); }
    
    static void _call_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_generate_marshall2(void* impl_msg, void* impl_obj);
    
    static void _callthr_generate_marshall2(CkThrCallArg *);
    
    static void _marshallmessagepup_generate_marshall2(PUP::er &p,void *msg);
    /* DECLS: void do_edgeGeneration(void);
     */
    // Entry point registration at startup
    
    static int reg_do_edgeGeneration_void();
    // Entry point index lookup
    
    inline static int idx_do_edgeGeneration_void() {
      static int epidx = reg_do_edgeGeneration_void();
      return epidx;
    }

    
    inline static int idx_do_edgeGeneration(void (Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::*)(void) ) {
      return idx_do_edgeGeneration_void();
    }


    
    static int do_edgeGeneration(void) { return idx_do_edgeGeneration_void(); }
    
    static void _call_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
    
    static void _call_sdag_do_edgeGeneration_void(void* impl_msg, void* impl_obj);
};
/* --------------- element proxy ------------------ */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CProxyElement_Ring_Generator_Tram: public CProxyElement_IrrGroup{
  public:
    typedef Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    CProxyElement_Ring_Generator_Tram(void) {}
    CProxyElement_Ring_Generator_Tram(const IrrGroup *g) : CProxyElement_IrrGroup(g){  }
    CProxyElement_Ring_Generator_Tram(CkGroupID _gid,int _onPE,CK_DELCTOR_PARAM) : CProxyElement_IrrGroup(_gid,_onPE,CK_DELCTOR_ARGS){  }
    CProxyElement_Ring_Generator_Tram(CkGroupID _gid,int _onPE) : CProxyElement_IrrGroup(_gid,_onPE){  }

    int ckIsDelegated(void) const
    { return CProxyElement_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxyElement_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxyElement_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxyElement_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxyElement_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxyElement_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxyElement_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxyElement_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxyElement_IrrGroup::ckSetReductionClient(cb); }
int ckGetGroupPe(void) const
{return CProxyElement_IrrGroup::ckGetGroupPe();}

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxyElement_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxyElement_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxyElement_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxyElement_IrrGroup::ckSetGroupID(g);
    }
    Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(CkGroupID gID) {
      return (Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > *)CkLocalBranch(gID);
    }
/* DECLS: Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

};
/* ---------------- collective proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CProxy_Ring_Generator_Tram: public CProxy_IrrGroup{
  public:
    typedef Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    CProxy_Ring_Generator_Tram(void) {}
    CProxy_Ring_Generator_Tram(const IrrGroup *g) : CProxy_IrrGroup(g){  }
    CProxy_Ring_Generator_Tram(CkGroupID _gid,CK_DELCTOR_PARAM) : CProxy_IrrGroup(_gid,CK_DELCTOR_ARGS){  }
    CProxy_Ring_Generator_Tram(CkGroupID _gid) : CProxy_IrrGroup(_gid){  }
    CProxyElement_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  operator[](int onPE) const
      {return CProxyElement_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > (ckGetGroupID(),onPE,CK_DELCTOR_CALL);}

    int ckIsDelegated(void) const
    { return CProxy_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxy_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxy_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxy_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxy_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxy_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxy_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxy_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxy_IrrGroup::ckSetReductionClient(cb); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxy_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxy_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxy_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxy_IrrGroup::ckSetGroupID(g);
    }
    Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(CkGroupID gID) {
      return (Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > *)CkLocalBranch(gID);
    }
/* DECLS: Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
    
    static CkGroupID ckNew(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts=NULL);
    CProxy_Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts=NULL);
    
    void generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);
    
    void do_edgeGeneration(int npes, int *pes);
    
    void do_edgeGeneration(CmiGroup &grp);

};
/* ---------------- section proxy -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class CProxySection_Ring_Generator_Tram: public CProxySection_IrrGroup{
  public:
    typedef Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  local_t;
    typedef CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  index_t;
    typedef CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  proxy_t;
    typedef CProxyElement_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  element_t;
    typedef CProxySection_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  section_t;

    CProxySection_Ring_Generator_Tram(void) {}
    CProxySection_Ring_Generator_Tram(const IrrGroup *g) : CProxySection_IrrGroup(g){  }
    CProxySection_Ring_Generator_Tram(const CkGroupID &_gid,const int *_pelist,int _npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }
    CProxySection_Ring_Generator_Tram(const CkGroupID &_gid,const int *_pelist,int _npes) : CProxySection_IrrGroup(_gid,_pelist,_npes){  }
    CProxySection_Ring_Generator_Tram(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes) : CProxySection_IrrGroup(n,_gid,_pelist,_npes){  }
    CProxySection_Ring_Generator_Tram(int n,const CkGroupID *_gid, int const * const *_pelist,const int *_npes,CK_DELCTOR_PARAM) : CProxySection_IrrGroup(n,_gid,_pelist,_npes,CK_DELCTOR_ARGS){  }

    int ckIsDelegated(void) const
    { return CProxySection_IrrGroup::ckIsDelegated(); }
    inline CkDelegateMgr *ckDelegatedTo(void) const
    { return CProxySection_IrrGroup::ckDelegatedTo(); }
    inline CkDelegateData *ckDelegatedPtr(void) const
    { return CProxySection_IrrGroup::ckDelegatedPtr(); }
    CkGroupID ckDelegatedIdx(void) const
    { return CProxySection_IrrGroup::ckDelegatedIdx(); }
inline void ckCheck(void) const {CProxySection_IrrGroup::ckCheck();}
CkChareID ckGetChareID(void) const
   {return CProxySection_IrrGroup::ckGetChareID();}
CkGroupID ckGetGroupID(void) const
   {return CProxySection_IrrGroup::ckGetGroupID();}
operator CkGroupID () const { return ckGetGroupID(); }

    inline void setReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::setReductionClient(fn,param); }
    inline void ckSetReductionClient(CkReductionClientFn fn,void *param=NULL) const
    { CProxySection_IrrGroup::ckSetReductionClient(fn,param); }
    inline void ckSetReductionClient(CkCallback *cb) const
    { CProxySection_IrrGroup::ckSetReductionClient(cb); }
inline int ckGetNumSections() const
{ return CProxySection_IrrGroup::ckGetNumSections(); }
inline CkSectionInfo &ckGetSectionInfo()
{ return CProxySection_IrrGroup::ckGetSectionInfo(); }
inline CkSectionID *ckGetSectionIDs()
{ return CProxySection_IrrGroup::ckGetSectionIDs(); }
inline CkSectionID &ckGetSectionID()
{ return CProxySection_IrrGroup::ckGetSectionID(); }
inline CkSectionID &ckGetSectionID(int i)
{ return CProxySection_IrrGroup::ckGetSectionID(i); }
inline CkGroupID ckGetGroupIDn(int i) const
{ return CProxySection_IrrGroup::ckGetGroupIDn(i); }
inline int *ckGetElements() const
{ return CProxySection_IrrGroup::ckGetElements(); }
inline int *ckGetElements(int i) const
{ return CProxySection_IrrGroup::ckGetElements(i); }
inline int ckGetNumElements() const
{ return CProxySection_IrrGroup::ckGetNumElements(); } 
inline int ckGetNumElements(int i) const
{ return CProxySection_IrrGroup::ckGetNumElements(i); }

    void ckDelegate(CkDelegateMgr *dTo,CkDelegateData *dPtr=NULL)
    {       CProxySection_IrrGroup::ckDelegate(dTo,dPtr); }
    void ckUndelegate(void)
    {       CProxySection_IrrGroup::ckUndelegate(); }
    void pup(PUP::er &p)
    {       CProxySection_IrrGroup::pup(p); }
    void ckSetGroupID(CkGroupID g) {
      CProxySection_IrrGroup::ckSetGroupID(g);
    }
    Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(void) const {
      return ckLocalBranch(ckGetGroupID());
    }
    static Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * ckLocalBranch(CkGroupID gID) {
      return (Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > *)CkLocalBranch(gID);
    }
/* DECLS: Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
    

/* DECLS: threaded void generate(const CkCallback &cb);
 */
    
    void generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts=NULL);

/* DECLS: void do_edgeGeneration(void);
 */
    
    void do_edgeGeneration(void);

};
#define Ring_Generator_Tram_SDAG_CODE 
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 
struct CBase_Ring_Generator_Tram;

} // namespace GraphLib


namespace GraphLib {
/* ---------------- method closures -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping > class Closure_SSCA_Generator_Charm {
  public:


    struct generate_2_closure;


    struct do_cliqueGeneration_3_closure;


    struct do_edgeGeneration_4_closure;


    struct addVertexToClique_5_closure;


    struct addInterCliqueEdge_6_closure;


    struct dump_vertices_7_closure;


    struct dump_cliques_8_closure;


    struct dump_edges_9_closure;

};

/* ---------------- method closures -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class Closure_SSCA_Generator_Tram {
  public:


    struct generate_2_closure;


    struct do_cliqueGeneration_3_closure;


    struct do_edgeGeneration_4_closure;


    struct addVertexToClique_5_closure;


    struct addInterCliqueEdge_6_closure;


    struct dump_vertices_7_closure;


    struct dump_cliques_8_closure;


    struct dump_edges_9_closure;

};

/* ---------------- method closures -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping > class Closure_RMAT_Generator_Charm {
  public:


    struct generate_2_closure;


    struct do_edgeGeneration_3_closure;

};

/* ---------------- method closures -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class Closure_RMAT_Generator_Tram {
  public:


    struct generate_2_closure;


    struct do_edgeGeneration_3_closure;

};

/* ---------------- method closures -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping > class Closure_Random_Generator_Charm {
  public:


    struct generate_2_closure;


    struct do_edgeGeneration_3_closure;

};

/* ---------------- method closures -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class Closure_Random_Generator_Tram {
  public:


    struct generate_2_closure;


    struct do_edgeGeneration_3_closure;

};

/* ---------------- method closures -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping > class Closure_Ring_Generator_Charm {
  public:


    struct generate_2_closure;


    struct do_edgeGeneration_3_closure;

};

/* ---------------- method closures -------------- */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > class Closure_Ring_Generator_Tram {
  public:


    struct generate_2_closure;


    struct do_edgeGeneration_3_closure;

};

} // namespace GraphLib

extern void _registerGraphGenerator(void);
#endif

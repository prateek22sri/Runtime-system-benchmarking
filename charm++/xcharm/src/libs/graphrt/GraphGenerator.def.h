
namespace GraphLib {
/* ---------------- method closures -------------- */
#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::generate_2_closure : public SDAG::Closure {
      CkCallback cb;


      generate_2_closure() {
        init();
      }
      generate_2_closure(CkMigrateMessage*) {
        init();
      }
      CkCallback & getP0() { return cb;}
      void pup(PUP::er& __p) {
        __p | cb;
        packClosure(__p);
      }
      virtual ~generate_2_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(generate_2_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_cliqueGeneration_3_closure : public SDAG::Closure {
      

      do_cliqueGeneration_3_closure() {
        init();
      }
      do_cliqueGeneration_3_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~do_cliqueGeneration_3_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(do_cliqueGeneration_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration_4_closure : public SDAG::Closure {
      

      do_edgeGeneration_4_closure() {
        init();
      }
      do_edgeGeneration_4_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~do_edgeGeneration_4_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(do_edgeGeneration_4_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addVertexToClique_5_closure : public SDAG::Closure {
      CmiUInt8 v;
      CmiUInt8 c;


      addVertexToClique_5_closure() {
        init();
      }
      addVertexToClique_5_closure(CkMigrateMessage*) {
        init();
      }
      CmiUInt8 & getP0() { return v;}
      CmiUInt8 & getP1() { return c;}
      void pup(PUP::er& __p) {
        __p | v;
        __p | c;
        packClosure(__p);
      }
      virtual ~addVertexToClique_5_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(addVertexToClique_5_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addInterCliqueEdge_6_closure : public SDAG::Closure {
      CmiUInt8 v;
      CmiUInt8 u;
      CmiUInt8 cv;


      addInterCliqueEdge_6_closure() {
        init();
      }
      addInterCliqueEdge_6_closure(CkMigrateMessage*) {
        init();
      }
      CmiUInt8 & getP0() { return v;}
      CmiUInt8 & getP1() { return u;}
      CmiUInt8 & getP2() { return cv;}
      void pup(PUP::er& __p) {
        __p | v;
        __p | u;
        __p | cv;
        packClosure(__p);
      }
      virtual ~addInterCliqueEdge_6_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(addInterCliqueEdge_6_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_vertices_7_closure : public SDAG::Closure {
      

      dump_vertices_7_closure() {
        init();
      }
      dump_vertices_7_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~dump_vertices_7_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(dump_vertices_7_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_cliques_8_closure : public SDAG::Closure {
      

      dump_cliques_8_closure() {
        init();
      }
      dump_cliques_8_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~dump_cliques_8_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(dump_cliques_8_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_edges_9_closure : public SDAG::Closure {
      

      dump_edges_9_closure() {
        init();
      }
      dump_edges_9_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~dump_edges_9_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(dump_edges_9_closure));
    };
#endif /* CK_TEMPLATES_ONLY */


/* ---------------- method closures -------------- */
#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate_2_closure : public SDAG::Closure {
      CkCallback cb;


      generate_2_closure() {
        init();
      }
      generate_2_closure(CkMigrateMessage*) {
        init();
      }
      CkCallback & getP0() { return cb;}
      void pup(PUP::er& __p) {
        __p | cb;
        packClosure(__p);
      }
      virtual ~generate_2_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(generate_2_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_cliqueGeneration_3_closure : public SDAG::Closure {
      

      do_cliqueGeneration_3_closure() {
        init();
      }
      do_cliqueGeneration_3_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~do_cliqueGeneration_3_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(do_cliqueGeneration_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration_4_closure : public SDAG::Closure {
      

      do_edgeGeneration_4_closure() {
        init();
      }
      do_edgeGeneration_4_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~do_edgeGeneration_4_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(do_edgeGeneration_4_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addVertexToClique_5_closure : public SDAG::Closure {
      CmiUInt8 v;
      CmiUInt8 c;


      addVertexToClique_5_closure() {
        init();
      }
      addVertexToClique_5_closure(CkMigrateMessage*) {
        init();
      }
      CmiUInt8 & getP0() { return v;}
      CmiUInt8 & getP1() { return c;}
      void pup(PUP::er& __p) {
        __p | v;
        __p | c;
        packClosure(__p);
      }
      virtual ~addVertexToClique_5_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(addVertexToClique_5_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addInterCliqueEdge_6_closure : public SDAG::Closure {
      CmiUInt8 v;
      CmiUInt8 u;
      CmiUInt8 cv;


      addInterCliqueEdge_6_closure() {
        init();
      }
      addInterCliqueEdge_6_closure(CkMigrateMessage*) {
        init();
      }
      CmiUInt8 & getP0() { return v;}
      CmiUInt8 & getP1() { return u;}
      CmiUInt8 & getP2() { return cv;}
      void pup(PUP::er& __p) {
        __p | v;
        __p | u;
        __p | cv;
        packClosure(__p);
      }
      virtual ~addInterCliqueEdge_6_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(addInterCliqueEdge_6_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_vertices_7_closure : public SDAG::Closure {
      

      dump_vertices_7_closure() {
        init();
      }
      dump_vertices_7_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~dump_vertices_7_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(dump_vertices_7_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_cliques_8_closure : public SDAG::Closure {
      

      dump_cliques_8_closure() {
        init();
      }
      dump_cliques_8_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~dump_cliques_8_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(dump_cliques_8_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_edges_9_closure : public SDAG::Closure {
      

      dump_edges_9_closure() {
        init();
      }
      dump_edges_9_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~dump_edges_9_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(dump_edges_9_closure));
    };
#endif /* CK_TEMPLATES_ONLY */


/* ---------------- method closures -------------- */
#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::generate_2_closure : public SDAG::Closure {
      CkCallback cb;


      generate_2_closure() {
        init();
      }
      generate_2_closure(CkMigrateMessage*) {
        init();
      }
      CkCallback & getP0() { return cb;}
      void pup(PUP::er& __p) {
        __p | cb;
        packClosure(__p);
      }
      virtual ~generate_2_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(generate_2_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration_3_closure : public SDAG::Closure {
      

      do_edgeGeneration_3_closure() {
        init();
      }
      do_edgeGeneration_3_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~do_edgeGeneration_3_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(do_edgeGeneration_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */


/* ---------------- method closures -------------- */
#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate_2_closure : public SDAG::Closure {
      CkCallback cb;


      generate_2_closure() {
        init();
      }
      generate_2_closure(CkMigrateMessage*) {
        init();
      }
      CkCallback & getP0() { return cb;}
      void pup(PUP::er& __p) {
        __p | cb;
        packClosure(__p);
      }
      virtual ~generate_2_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(generate_2_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration_3_closure : public SDAG::Closure {
      

      do_edgeGeneration_3_closure() {
        init();
      }
      do_edgeGeneration_3_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~do_edgeGeneration_3_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(do_edgeGeneration_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */


/* ---------------- method closures -------------- */
#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_Random_Generator_Charm < Graph, Opts, vertexMapping > ::generate_2_closure : public SDAG::Closure {
      CkCallback cb;


      generate_2_closure() {
        init();
      }
      generate_2_closure(CkMigrateMessage*) {
        init();
      }
      CkCallback & getP0() { return cb;}
      void pup(PUP::er& __p) {
        __p | cb;
        packClosure(__p);
      }
      virtual ~generate_2_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(generate_2_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_Random_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration_3_closure : public SDAG::Closure {
      

      do_edgeGeneration_3_closure() {
        init();
      }
      do_edgeGeneration_3_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~do_edgeGeneration_3_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(do_edgeGeneration_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */


/* ---------------- method closures -------------- */
#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate_2_closure : public SDAG::Closure {
      CkCallback cb;


      generate_2_closure() {
        init();
      }
      generate_2_closure(CkMigrateMessage*) {
        init();
      }
      CkCallback & getP0() { return cb;}
      void pup(PUP::er& __p) {
        __p | cb;
        packClosure(__p);
      }
      virtual ~generate_2_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(generate_2_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration_3_closure : public SDAG::Closure {
      

      do_edgeGeneration_3_closure() {
        init();
      }
      do_edgeGeneration_3_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~do_edgeGeneration_3_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(do_edgeGeneration_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */


/* ---------------- method closures -------------- */
#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::generate_2_closure : public SDAG::Closure {
      CkCallback cb;


      generate_2_closure() {
        init();
      }
      generate_2_closure(CkMigrateMessage*) {
        init();
      }
      CkCallback & getP0() { return cb;}
      void pup(PUP::er& __p) {
        __p | cb;
        packClosure(__p);
      }
      virtual ~generate_2_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(generate_2_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 

    struct Closure_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration_3_closure : public SDAG::Closure {
      

      do_edgeGeneration_3_closure() {
        init();
      }
      do_edgeGeneration_3_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~do_edgeGeneration_3_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(do_edgeGeneration_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */


/* ---------------- method closures -------------- */
#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate_2_closure : public SDAG::Closure {
      CkCallback cb;


      generate_2_closure() {
        init();
      }
      generate_2_closure(CkMigrateMessage*) {
        init();
      }
      CkCallback & getP0() { return cb;}
      void pup(PUP::er& __p) {
        __p | cb;
        packClosure(__p);
      }
      virtual ~generate_2_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(generate_2_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

    struct Closure_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration_3_closure : public SDAG::Closure {
      

      do_edgeGeneration_3_closure() {
        init();
      }
      do_edgeGeneration_3_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~do_edgeGeneration_3_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(do_edgeGeneration_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */


} // namespace GraphLib


namespace GraphLib {
/* DEFS: template < class Graph, class Opts, VertexMapping vertexMapping > group SSCA_Generator_Charm: IrrGroup{
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
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping >  int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
/* DEFS: SSCA_Generator_Charm(const Graph &g, const Opts &opts);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_cliqueGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_cliqueGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &c
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_vertices(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_vertices(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_vertices_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_vertices_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_cliques(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_cliques(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_cliques_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_cliques_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_edges(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_edges(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_edges_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_edges_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_edges_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: SSCA_Generator_Charm(const Graph &g, const Opts &opts);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

CkGroupID CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::ckNew(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  return CkCreateGroup(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::__idx, CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_SSCA_Generator_Charm_marshall1(), impl_msg);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

  CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::CProxy_SSCA_Generator_Charm(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  ckSetGroupID(CkCreateGroup(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::__idx, CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_SSCA_Generator_Charm_marshall1(), impl_msg));
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::reg_SSCA_Generator_Charm_marshall1() {
  int epidx = CkRegisterEp("SSCA_Generator_Charm(const Graph &g, const Opts &opts)",
      _call_SSCA_Generator_Charm_marshall1, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_SSCA_Generator_Charm_marshall1);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_SSCA_Generator_Charm_marshall1);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_call_SSCA_Generator_Charm_marshall1(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) SSCA_Generator_Charm < Graph, Opts, vertexMapping > (g, opts);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_callmarshall_SSCA_Generator_Charm_marshall1(char* impl_buf, void* impl_obj_void) {
  SSCA_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast< SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) SSCA_Generator_Charm < Graph, Opts, vertexMapping > (g, opts);
  return implP.size();
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_marshallmessagepup_SSCA_Generator_Charm_marshall1(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("g");
  implDestP|g;
  if (implDestP.hasComments()) implDestP.comment("opts");
  implDestP|opts;
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::reg_generate_marshall2() {
  int epidx = CkRegisterEp("generate(const CkCallback &cb)",
      _call_generate_marshall2, CkMarshallMsg::__idx, __idx, 0);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_generate_marshall2);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_call_generate_marshall2(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CthThread tid = CthCreate((CthVoidFn)_callthr_generate_marshall2, new CkThrCallArg(impl_msg,impl_obj), 0);
  ((Chare *)impl_obj)->CkAddThreadListeners(tid,impl_msg);
  CthResume(tid);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_callthr_generate_marshall2(CkThrCallArg *impl_arg)
{
  void *impl_msg = impl_arg->msg;
  SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *impl_obj = (SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *) impl_arg->obj;
  delete impl_arg;
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->generate(cb);
  delete impl_msg_typed;
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_marshallmessagepup_generate_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("cb");
  implDestP|cb;
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::generate_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_cliqueGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_cliqueGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_cliqueGeneration(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_cliqueGeneration(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::reg_do_cliqueGeneration_void() {
  int epidx = CkRegisterEp("do_cliqueGeneration(void)",
      _call_do_cliqueGeneration_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_call_do_cliqueGeneration_void(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->do_cliqueGeneration();
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_cliqueGeneration_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::reg_do_edgeGeneration_void() {
  int epidx = CkRegisterEp("do_edgeGeneration(void)",
      _call_do_edgeGeneration_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_call_do_edgeGeneration_void(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->do_edgeGeneration();
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration_4_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &c
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, int npes, int *pes, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &c
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
  }
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, CmiGroup &grp, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &c
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
  }
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::reg_addVertexToClique_marshall5() {
  int epidx = CkRegisterEp("addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c)",
      _call_addVertexToClique_marshall5, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_addVertexToClique_marshall5);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_addVertexToClique_marshall5);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_call_addVertexToClique_marshall5(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const CmiUInt8 &c*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  CmiUInt8 c; implP|c;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->addVertexToClique(v, c);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_callmarshall_addVertexToClique_marshall5(char* impl_buf, void* impl_obj_void) {
  SSCA_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast< SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const CmiUInt8 &c*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  CmiUInt8 c; implP|c;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->addVertexToClique(v, c);
  return implP.size();
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_marshallmessagepup_addVertexToClique_marshall5(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const CmiUInt8 &c*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  CmiUInt8 c; implP|c;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("v");
  implDestP|v;
  if (implDestP.hasComments()) implDestP.comment("c");
  implDestP|c;
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addVertexToClique_5_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, int npes, int *pes, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
  }
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, CmiGroup &grp, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
  }
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::reg_addInterCliqueEdge_marshall6() {
  int epidx = CkRegisterEp("addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv)",
      _call_addInterCliqueEdge_marshall6, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_addInterCliqueEdge_marshall6);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_addInterCliqueEdge_marshall6);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_call_addInterCliqueEdge_marshall6(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  CmiUInt8 u; implP|u;
  CmiUInt8 cv; implP|cv;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->addInterCliqueEdge(v, u, cv);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_callmarshall_addInterCliqueEdge_marshall6(char* impl_buf, void* impl_obj_void) {
  SSCA_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast< SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  CmiUInt8 u; implP|u;
  CmiUInt8 cv; implP|cv;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->addInterCliqueEdge(v, u, cv);
  return implP.size();
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_marshallmessagepup_addInterCliqueEdge_marshall6(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  CmiUInt8 u; implP|u;
  CmiUInt8 cv; implP|cv;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("v");
  implDestP|v;
  if (implDestP.hasComments()) implDestP.comment("u");
  implDestP|u;
  if (implDestP.hasComments()) implDestP.comment("cv");
  implDestP|cv;
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addInterCliqueEdge_6_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_vertices(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_vertices(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_vertices(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_vertices(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::reg_dump_vertices_void() {
  int epidx = CkRegisterEp("dump_vertices(void)",
      _call_dump_vertices_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_call_dump_vertices_void(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->dump_vertices();
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_vertices_7_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_cliques(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_cliques(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_cliques(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_cliques(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::reg_dump_cliques_void() {
  int epidx = CkRegisterEp("dump_cliques(void)",
      _call_dump_cliques_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_call_dump_cliques_void(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->dump_cliques();
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_cliques_8_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_edges(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_edges(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_edges_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_edges_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_edges_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_edges(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_edges_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_edges(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_edges_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::reg_dump_edges_void() {
  int epidx = CkRegisterEp("dump_edges(void)",
      _call_dump_edges_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::_call_dump_edges_void(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<SSCA_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->dump_edges();
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_edges_9_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: SSCA_Generator_Charm(const Graph &g, const Opts &opts);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_cliqueGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_cliqueGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_cliqueGeneration_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &c
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addVertexToClique_marshall5(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_addInterCliqueEdge_marshall6(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_vertices(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_vertices(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_vertices_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_vertices_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_cliques(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_cliques(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_cliques_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_cliques_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_edges(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::dump_edges(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_edges_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_edges_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::idx_dump_edges_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > void CkIndex_SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeGroup);
  CkRegisterBase(__idx, CkIndex_IrrGroup::__idx);
   CkRegisterGroupIrr(__idx,SSCA_Generator_Charm < Graph, Opts, vertexMapping > ::isIrreducible());
  // REG: SSCA_Generator_Charm(const Graph &g, const Opts &opts);
  idx_SSCA_Generator_Charm_marshall1();

  // REG: threaded void generate(const CkCallback &cb);
  idx_generate_marshall2();

  // REG: void do_cliqueGeneration(void);
  idx_do_cliqueGeneration_void();

  // REG: void do_edgeGeneration(void);
  idx_do_edgeGeneration_void();

  // REG: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
  idx_addVertexToClique_marshall5();

  // REG: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
  idx_addInterCliqueEdge_marshall6();

  // REG: void dump_vertices(void);
  idx_dump_vertices_void();

  // REG: void dump_cliques(void);
  idx_dump_cliques_void();

  // REG: void dump_edges(void);
  idx_dump_edges_void();

}
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 
struct CBase_SSCA_Generator_Charm : public Group, virtual CBase
 {
  CProxy_SSCA_Generator_Charm < Graph, Opts, vertexMapping >  thisProxy;
  CBase_SSCA_Generator_Charm() : thisProxy(this)
    , Group()
  { }
  CBase_SSCA_Generator_Charm(CkMigrateMessage* m) : thisProxy(this)
    , Group(m)  { }
  template <typename... Args>
  CBase_SSCA_Generator_Charm(Args... args) : thisProxy(this)
    , Group(args...)  { }
  void pup(PUP::er &p) { }
  void virtual_pup(PUP::er &p) {
    recursive_pup<SSCA_Generator_Charm < Graph, Opts, vertexMapping >  >(dynamic_cast<SSCA_Generator_Charm < Graph, Opts, vertexMapping > * >(this), p);
}
  void parent_pup(PUP::er &p) {
    recursive_pup< Group >(this, p);
    p|thisProxy;
  }
};
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > group SSCA_Generator_Tram: IrrGroup{
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
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
/* DEFS: SSCA_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_cliqueGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_cliqueGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &c
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_vertices(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_vertices(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_vertices_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_vertices_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_cliques(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_cliques(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_cliques_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_cliques_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_edges(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_edges(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_edges_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_edges_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_edges_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: SSCA_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

CkGroupID CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::ckNew(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  return CkCreateGroup(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__idx, CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_SSCA_Generator_Tram_marshall1(), impl_msg);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

  CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::CProxy_SSCA_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  ckSetGroupID(CkCreateGroup(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__idx, CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_SSCA_Generator_Tram_marshall1(), impl_msg));
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_SSCA_Generator_Tram_marshall1() {
  int epidx = CkRegisterEp("SSCA_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator)",
      _call_SSCA_Generator_Tram_marshall1, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_SSCA_Generator_Tram_marshall1);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_SSCA_Generator_Tram_marshall1);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_SSCA_Generator_Tram_marshall1(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  CProxy_Aggregator aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > (g, opts, aggregator);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_callmarshall_SSCA_Generator_Tram_marshall1(char* impl_buf, void* impl_obj_void) {
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast< SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  CProxy_Aggregator aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > (g, opts, aggregator);
  return implP.size();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_marshallmessagepup_SSCA_Generator_Tram_marshall1(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  CProxy_Aggregator aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("g");
  implDestP|g;
  if (implDestP.hasComments()) implDestP.comment("opts");
  implDestP|opts;
  if (implDestP.hasComments()) implDestP.comment("aggregator");
  implDestP|aggregator;
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_generate_marshall2() {
  int epidx = CkRegisterEp("generate(const CkCallback &cb)",
      _call_generate_marshall2, CkMarshallMsg::__idx, __idx, 0);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_generate_marshall2);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_generate_marshall2(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CthThread tid = CthCreate((CthVoidFn)_callthr_generate_marshall2, new CkThrCallArg(impl_msg,impl_obj), 0);
  ((Chare *)impl_obj)->CkAddThreadListeners(tid,impl_msg);
  CthResume(tid);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_callthr_generate_marshall2(CkThrCallArg *impl_arg)
{
  void *impl_msg = impl_arg->msg;
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *impl_obj = (SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *) impl_arg->obj;
  delete impl_arg;
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->generate(cb);
  delete impl_msg_typed;
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_marshallmessagepup_generate_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("cb");
  implDestP|cb;
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_cliqueGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_cliqueGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_cliqueGeneration(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_cliqueGeneration(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_do_cliqueGeneration_void() {
  int epidx = CkRegisterEp("do_cliqueGeneration(void)",
      _call_do_cliqueGeneration_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_do_cliqueGeneration_void(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->do_cliqueGeneration();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_cliqueGeneration_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_do_edgeGeneration_void() {
  int epidx = CkRegisterEp("do_edgeGeneration(void)",
      _call_do_edgeGeneration_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_do_edgeGeneration_void(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->do_edgeGeneration();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration_4_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &c
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, int npes, int *pes, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &c
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
  }
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, CmiGroup &grp, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &c
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
  }
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_addVertexToClique_marshall5() {
  int epidx = CkRegisterEp("addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c)",
      _call_addVertexToClique_marshall5, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_addVertexToClique_marshall5);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_addVertexToClique_marshall5);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_addVertexToClique_marshall5(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const CmiUInt8 &c*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  CmiUInt8 c; implP|c;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->addVertexToClique(v, c);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_callmarshall_addVertexToClique_marshall5(char* impl_buf, void* impl_obj_void) {
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast< SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const CmiUInt8 &c*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  CmiUInt8 c; implP|c;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->addVertexToClique(v, c);
  return implP.size();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_marshallmessagepup_addVertexToClique_marshall5(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const CmiUInt8 &c*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  CmiUInt8 c; implP|c;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("v");
  implDestP|v;
  if (implDestP.hasComments()) implDestP.comment("c");
  implDestP|c;
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addVertexToClique_5_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, int npes, int *pes, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
  }
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, CmiGroup &grp, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
  }
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_addInterCliqueEdge_marshall6() {
  int epidx = CkRegisterEp("addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv)",
      _call_addInterCliqueEdge_marshall6, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_addInterCliqueEdge_marshall6);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_addInterCliqueEdge_marshall6);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_addInterCliqueEdge_marshall6(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  CmiUInt8 u; implP|u;
  CmiUInt8 cv; implP|cv;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->addInterCliqueEdge(v, u, cv);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_callmarshall_addInterCliqueEdge_marshall6(char* impl_buf, void* impl_obj_void) {
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast< SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  CmiUInt8 u; implP|u;
  CmiUInt8 cv; implP|cv;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->addInterCliqueEdge(v, u, cv);
  return implP.size();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_marshallmessagepup_addInterCliqueEdge_marshall6(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  CmiUInt8 u; implP|u;
  CmiUInt8 cv; implP|cv;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("v");
  implDestP|v;
  if (implDestP.hasComments()) implDestP.comment("u");
  implDestP|u;
  if (implDestP.hasComments()) implDestP.comment("cv");
  implDestP|cv;
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addInterCliqueEdge_6_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_vertices(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_vertices(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_vertices(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_vertices(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_dump_vertices_void() {
  int epidx = CkRegisterEp("dump_vertices(void)",
      _call_dump_vertices_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_dump_vertices_void(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->dump_vertices();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_vertices_7_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_cliques(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_cliques(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_cliques(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_cliques(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_dump_cliques_void() {
  int epidx = CkRegisterEp("dump_cliques(void)",
      _call_dump_cliques_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_dump_cliques_void(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->dump_cliques();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_cliques_8_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_edges(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_edges(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_edges_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_edges_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_edges_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_edges(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_edges_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_edges(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_edges_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_dump_edges_void() {
  int epidx = CkRegisterEp("dump_edges(void)",
      _call_dump_edges_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_dump_edges_void(void* impl_msg, void* impl_obj_void)
{
  SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->dump_edges();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_edges_9_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: SSCA_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_cliqueGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_cliqueGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_cliqueGeneration_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_cliqueGeneration_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &c
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)c;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addVertexToClique_marshall5(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addVertexToClique_marshall5(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)u;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)cv;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addInterCliqueEdge_marshall6(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_addInterCliqueEdge_marshall6(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_vertices(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_vertices(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_vertices_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_vertices_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_vertices_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_cliques(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_cliques(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_cliques_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_cliques_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_cliques_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void dump_edges(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::dump_edges(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_edges_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_edges_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_dump_edges_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > void CkIndex_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeGroup);
  CkRegisterBase(__idx, CkIndex_IrrGroup::__idx);
   CkRegisterGroupIrr(__idx,SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::isIrreducible());
  // REG: SSCA_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
  idx_SSCA_Generator_Tram_marshall1();

  // REG: threaded void generate(const CkCallback &cb);
  idx_generate_marshall2();

  // REG: void do_cliqueGeneration(void);
  idx_do_cliqueGeneration_void();

  // REG: void do_edgeGeneration(void);
  idx_do_edgeGeneration_void();

  // REG: void addVertexToClique(const CmiUInt8 &v, const CmiUInt8 &c);
  idx_addVertexToClique_marshall5();

  // REG: void addInterCliqueEdge(const CmiUInt8 &v, const CmiUInt8 &u, const CmiUInt8 &cv);
  idx_addInterCliqueEdge_marshall6();

  // REG: void dump_vertices(void);
  idx_dump_vertices_void();

  // REG: void dump_cliques(void);
  idx_dump_cliques_void();

  // REG: void dump_edges(void);
  idx_dump_edges_void();

}
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 
struct CBase_SSCA_Generator_Tram : public Group, virtual CBase
 {
  CProxy_SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  thisProxy;
  CBase_SSCA_Generator_Tram() : thisProxy(this)
    , Group()
  { }
  CBase_SSCA_Generator_Tram(CkMigrateMessage* m) : thisProxy(this)
    , Group(m)  { }
  template <typename... Args>
  CBase_SSCA_Generator_Tram(Args... args) : thisProxy(this)
    , Group(args...)  { }
  void pup(PUP::er &p) { }
  void virtual_pup(PUP::er &p) {
    recursive_pup<SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  >(dynamic_cast<SSCA_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * >(this), p);
}
  void parent_pup(PUP::er &p) {
    recursive_pup< Group >(this, p);
    p|thisProxy;
  }
};
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: template < class Graph, class Opts, VertexMapping vertexMapping > group RMAT_Generator_Charm: IrrGroup{
RMAT_Generator_Charm(const Graph &g, const Opts &opts);
threaded void generate(const CkCallback &cb);
void do_edgeGeneration(void);
};
 */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping >  int CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
/* DEFS: RMAT_Generator_Charm(const Graph &g, const Opts &opts);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: RMAT_Generator_Charm(const Graph &g, const Opts &opts);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

CkGroupID CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::ckNew(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  return CkCreateGroup(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::__idx, CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_RMAT_Generator_Charm_marshall1(), impl_msg);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

  CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::CProxy_RMAT_Generator_Charm(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  ckSetGroupID(CkCreateGroup(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::__idx, CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_RMAT_Generator_Charm_marshall1(), impl_msg));
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::reg_RMAT_Generator_Charm_marshall1() {
  int epidx = CkRegisterEp("RMAT_Generator_Charm(const Graph &g, const Opts &opts)",
      _call_RMAT_Generator_Charm_marshall1, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_RMAT_Generator_Charm_marshall1);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_RMAT_Generator_Charm_marshall1);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::_call_RMAT_Generator_Charm_marshall1(void* impl_msg, void* impl_obj_void)
{
  RMAT_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<RMAT_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) RMAT_Generator_Charm < Graph, Opts, vertexMapping > (g, opts);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::_callmarshall_RMAT_Generator_Charm_marshall1(char* impl_buf, void* impl_obj_void) {
  RMAT_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast< RMAT_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) RMAT_Generator_Charm < Graph, Opts, vertexMapping > (g, opts);
  return implP.size();
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::_marshallmessagepup_RMAT_Generator_Charm_marshall1(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("g");
  implDestP|g;
  if (implDestP.hasComments()) implDestP.comment("opts");
  implDestP|opts;
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchMulti(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchGroup(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::reg_generate_marshall2() {
  int epidx = CkRegisterEp("generate(const CkCallback &cb)",
      _call_generate_marshall2, CkMarshallMsg::__idx, __idx, 0);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_generate_marshall2);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::_call_generate_marshall2(void* impl_msg, void* impl_obj_void)
{
  RMAT_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<RMAT_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CthThread tid = CthCreate((CthVoidFn)_callthr_generate_marshall2, new CkThrCallArg(impl_msg,impl_obj), 0);
  ((Chare *)impl_obj)->CkAddThreadListeners(tid,impl_msg);
  CthResume(tid);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::_callthr_generate_marshall2(CkThrCallArg *impl_arg)
{
  void *impl_msg = impl_arg->msg;
  RMAT_Generator_Charm < Graph, Opts, vertexMapping >  *impl_obj = (RMAT_Generator_Charm < Graph, Opts, vertexMapping >  *) impl_arg->obj;
  delete impl_arg;
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->generate(cb);
  delete impl_msg_typed;
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::_marshallmessagepup_generate_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("cb");
  implDestP|cb;
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::generate_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::reg_do_edgeGeneration_void() {
  int epidx = CkRegisterEp("do_edgeGeneration(void)",
      _call_do_edgeGeneration_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::_call_do_edgeGeneration_void(void* impl_msg, void* impl_obj_void)
{
  RMAT_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<RMAT_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->do_edgeGeneration();
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: RMAT_Generator_Charm(const Graph &g, const Opts &opts);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > void CkIndex_RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeGroup);
  CkRegisterBase(__idx, CkIndex_IrrGroup::__idx);
   CkRegisterGroupIrr(__idx,RMAT_Generator_Charm < Graph, Opts, vertexMapping > ::isIrreducible());
  // REG: RMAT_Generator_Charm(const Graph &g, const Opts &opts);
  idx_RMAT_Generator_Charm_marshall1();

  // REG: threaded void generate(const CkCallback &cb);
  idx_generate_marshall2();

  // REG: void do_edgeGeneration(void);
  idx_do_edgeGeneration_void();

}
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 
struct CBase_RMAT_Generator_Charm : public Group, virtual CBase
 {
  CProxy_RMAT_Generator_Charm < Graph, Opts, vertexMapping >  thisProxy;
  CBase_RMAT_Generator_Charm() : thisProxy(this)
    , Group()
  { }
  CBase_RMAT_Generator_Charm(CkMigrateMessage* m) : thisProxy(this)
    , Group(m)  { }
  template <typename... Args>
  CBase_RMAT_Generator_Charm(Args... args) : thisProxy(this)
    , Group(args...)  { }
  void pup(PUP::er &p) { }
  void virtual_pup(PUP::er &p) {
    recursive_pup<RMAT_Generator_Charm < Graph, Opts, vertexMapping >  >(dynamic_cast<RMAT_Generator_Charm < Graph, Opts, vertexMapping > * >(this), p);
}
  void parent_pup(PUP::er &p) {
    recursive_pup< Group >(this, p);
    p|thisProxy;
  }
};
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > group RMAT_Generator_Tram: IrrGroup{
RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
threaded void generate(const CkCallback &cb);
void do_edgeGeneration(void);
};
 */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  int CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
/* DEFS: RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

CkGroupID CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::ckNew(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  return CkCreateGroup(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__idx, CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_RMAT_Generator_Tram_marshall1(), impl_msg);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

  CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::CProxy_RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  ckSetGroupID(CkCreateGroup(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__idx, CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_RMAT_Generator_Tram_marshall1(), impl_msg));
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_RMAT_Generator_Tram_marshall1() {
  int epidx = CkRegisterEp("RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator)",
      _call_RMAT_Generator_Tram_marshall1, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_RMAT_Generator_Tram_marshall1);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_RMAT_Generator_Tram_marshall1);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_RMAT_Generator_Tram_marshall1(void* impl_msg, void* impl_obj_void)
{
  RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  CProxy_Aggregator aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > (g, opts, aggregator);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_callmarshall_RMAT_Generator_Tram_marshall1(char* impl_buf, void* impl_obj_void) {
  RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast< RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  CProxy_Aggregator aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > (g, opts, aggregator);
  return implP.size();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_marshallmessagepup_RMAT_Generator_Tram_marshall1(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  CProxy_Aggregator aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("g");
  implDestP|g;
  if (implDestP.hasComments()) implDestP.comment("opts");
  implDestP|opts;
  if (implDestP.hasComments()) implDestP.comment("aggregator");
  implDestP|aggregator;
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchMulti(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchGroup(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_generate_marshall2() {
  int epidx = CkRegisterEp("generate(const CkCallback &cb)",
      _call_generate_marshall2, CkMarshallMsg::__idx, __idx, 0);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_generate_marshall2);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_generate_marshall2(void* impl_msg, void* impl_obj_void)
{
  RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CthThread tid = CthCreate((CthVoidFn)_callthr_generate_marshall2, new CkThrCallArg(impl_msg,impl_obj), 0);
  ((Chare *)impl_obj)->CkAddThreadListeners(tid,impl_msg);
  CthResume(tid);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_callthr_generate_marshall2(CkThrCallArg *impl_arg)
{
  void *impl_msg = impl_arg->msg;
  RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *impl_obj = (RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *) impl_arg->obj;
  delete impl_arg;
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->generate(cb);
  delete impl_msg_typed;
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_marshallmessagepup_generate_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("cb");
  implDestP|cb;
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_do_edgeGeneration_void() {
  int epidx = CkRegisterEp("do_edgeGeneration(void)",
      _call_do_edgeGeneration_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_do_edgeGeneration_void(void* impl_msg, void* impl_obj_void)
{
  RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->do_edgeGeneration();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > void CkIndex_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeGroup);
  CkRegisterBase(__idx, CkIndex_IrrGroup::__idx);
   CkRegisterGroupIrr(__idx,RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::isIrreducible());
  // REG: RMAT_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
  idx_RMAT_Generator_Tram_marshall1();

  // REG: threaded void generate(const CkCallback &cb);
  idx_generate_marshall2();

  // REG: void do_edgeGeneration(void);
  idx_do_edgeGeneration_void();

}
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 
struct CBase_RMAT_Generator_Tram : public Group, virtual CBase
 {
  CProxy_RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  thisProxy;
  CBase_RMAT_Generator_Tram() : thisProxy(this)
    , Group()
  { }
  CBase_RMAT_Generator_Tram(CkMigrateMessage* m) : thisProxy(this)
    , Group(m)  { }
  template <typename... Args>
  CBase_RMAT_Generator_Tram(Args... args) : thisProxy(this)
    , Group(args...)  { }
  void pup(PUP::er &p) { }
  void virtual_pup(PUP::er &p) {
    recursive_pup<RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  >(dynamic_cast<RMAT_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * >(this), p);
}
  void parent_pup(PUP::er &p) {
    recursive_pup< Group >(this, p);
    p|thisProxy;
  }
};
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: template < class Graph, class Opts, VertexMapping vertexMapping > group Random_Generator_Charm: IrrGroup{
Random_Generator_Charm(const Graph &g, const Opts &opts);
threaded void generate(const CkCallback &cb);
void do_edgeGeneration(void);
};
 */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping >  int CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
/* DEFS: Random_Generator_Charm(const Graph &g, const Opts &opts);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_Random_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_Random_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Random_Generator_Charm(const Graph &g, const Opts &opts);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

CkGroupID CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping > ::ckNew(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  return CkCreateGroup(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::__idx, CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_Random_Generator_Charm_marshall1(), impl_msg);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

  CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping > ::CProxy_Random_Generator_Charm(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  ckSetGroupID(CkCreateGroup(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::__idx, CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_Random_Generator_Charm_marshall1(), impl_msg));
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::reg_Random_Generator_Charm_marshall1() {
  int epidx = CkRegisterEp("Random_Generator_Charm(const Graph &g, const Opts &opts)",
      _call_Random_Generator_Charm_marshall1, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_Random_Generator_Charm_marshall1);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_Random_Generator_Charm_marshall1);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::_call_Random_Generator_Charm_marshall1(void* impl_msg, void* impl_obj_void)
{
  Random_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<Random_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) Random_Generator_Charm < Graph, Opts, vertexMapping > (g, opts);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::_callmarshall_Random_Generator_Charm_marshall1(char* impl_buf, void* impl_obj_void) {
  Random_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast< Random_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) Random_Generator_Charm < Graph, Opts, vertexMapping > (g, opts);
  return implP.size();
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::_marshallmessagepup_Random_Generator_Charm_marshall1(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("g");
  implDestP|g;
  if (implDestP.hasComments()) implDestP.comment("opts");
  implDestP|opts;
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchMulti(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchGroup(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::reg_generate_marshall2() {
  int epidx = CkRegisterEp("generate(const CkCallback &cb)",
      _call_generate_marshall2, CkMarshallMsg::__idx, __idx, 0);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_generate_marshall2);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::_call_generate_marshall2(void* impl_msg, void* impl_obj_void)
{
  Random_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<Random_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CthThread tid = CthCreate((CthVoidFn)_callthr_generate_marshall2, new CkThrCallArg(impl_msg,impl_obj), 0);
  ((Chare *)impl_obj)->CkAddThreadListeners(tid,impl_msg);
  CthResume(tid);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::_callthr_generate_marshall2(CkThrCallArg *impl_arg)
{
  void *impl_msg = impl_arg->msg;
  Random_Generator_Charm < Graph, Opts, vertexMapping >  *impl_obj = (Random_Generator_Charm < Graph, Opts, vertexMapping >  *) impl_arg->obj;
  delete impl_arg;
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->generate(cb);
  delete impl_msg_typed;
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::_marshallmessagepup_generate_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("cb");
  implDestP|cb;
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_Random_Generator_Charm < Graph, Opts, vertexMapping > ::generate_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::reg_do_edgeGeneration_void() {
  int epidx = CkRegisterEp("do_edgeGeneration(void)",
      _call_do_edgeGeneration_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::_call_do_edgeGeneration_void(void* impl_msg, void* impl_obj_void)
{
  Random_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<Random_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->do_edgeGeneration();
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_Random_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Random_Generator_Charm(const Graph &g, const Opts &opts);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_Random_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_Random_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > void CkIndex_Random_Generator_Charm < Graph, Opts, vertexMapping > ::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeGroup);
  CkRegisterBase(__idx, CkIndex_IrrGroup::__idx);
   CkRegisterGroupIrr(__idx,Random_Generator_Charm < Graph, Opts, vertexMapping > ::isIrreducible());
  // REG: Random_Generator_Charm(const Graph &g, const Opts &opts);
  idx_Random_Generator_Charm_marshall1();

  // REG: threaded void generate(const CkCallback &cb);
  idx_generate_marshall2();

  // REG: void do_edgeGeneration(void);
  idx_do_edgeGeneration_void();

}
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 
struct CBase_Random_Generator_Charm : public Group, virtual CBase
 {
  CProxy_Random_Generator_Charm < Graph, Opts, vertexMapping >  thisProxy;
  CBase_Random_Generator_Charm() : thisProxy(this)
    , Group()
  { }
  CBase_Random_Generator_Charm(CkMigrateMessage* m) : thisProxy(this)
    , Group(m)  { }
  template <typename... Args>
  CBase_Random_Generator_Charm(Args... args) : thisProxy(this)
    , Group(args...)  { }
  void pup(PUP::er &p) { }
  void virtual_pup(PUP::er &p) {
    recursive_pup<Random_Generator_Charm < Graph, Opts, vertexMapping >  >(dynamic_cast<Random_Generator_Charm < Graph, Opts, vertexMapping > * >(this), p);
}
  void parent_pup(PUP::er &p) {
    recursive_pup< Group >(this, p);
    p|thisProxy;
  }
};
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > group Random_Generator_Tram: IrrGroup{
Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
threaded void generate(const CkCallback &cb);
void do_edgeGeneration(void);
};
 */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  int CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
/* DEFS: Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

CkGroupID CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::ckNew(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  return CkCreateGroup(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__idx, CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_Random_Generator_Tram_marshall1(), impl_msg);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

  CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::CProxy_Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  ckSetGroupID(CkCreateGroup(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__idx, CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_Random_Generator_Tram_marshall1(), impl_msg));
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_Random_Generator_Tram_marshall1() {
  int epidx = CkRegisterEp("Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator)",
      _call_Random_Generator_Tram_marshall1, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_Random_Generator_Tram_marshall1);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_Random_Generator_Tram_marshall1);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_Random_Generator_Tram_marshall1(void* impl_msg, void* impl_obj_void)
{
  Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  CProxy_Aggregator aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > (g, opts, aggregator);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_callmarshall_Random_Generator_Tram_marshall1(char* impl_buf, void* impl_obj_void) {
  Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast< Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  CProxy_Aggregator aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > (g, opts, aggregator);
  return implP.size();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_marshallmessagepup_Random_Generator_Tram_marshall1(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  CProxy_Aggregator aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("g");
  implDestP|g;
  if (implDestP.hasComments()) implDestP.comment("opts");
  implDestP|opts;
  if (implDestP.hasComments()) implDestP.comment("aggregator");
  implDestP|aggregator;
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchMulti(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchGroup(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_generate_marshall2() {
  int epidx = CkRegisterEp("generate(const CkCallback &cb)",
      _call_generate_marshall2, CkMarshallMsg::__idx, __idx, 0);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_generate_marshall2);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_generate_marshall2(void* impl_msg, void* impl_obj_void)
{
  Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CthThread tid = CthCreate((CthVoidFn)_callthr_generate_marshall2, new CkThrCallArg(impl_msg,impl_obj), 0);
  ((Chare *)impl_obj)->CkAddThreadListeners(tid,impl_msg);
  CthResume(tid);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_callthr_generate_marshall2(CkThrCallArg *impl_arg)
{
  void *impl_msg = impl_arg->msg;
  Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *impl_obj = (Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *) impl_arg->obj;
  delete impl_arg;
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->generate(cb);
  delete impl_msg_typed;
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_marshallmessagepup_generate_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("cb");
  implDestP|cb;
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_do_edgeGeneration_void() {
  int epidx = CkRegisterEp("do_edgeGeneration(void)",
      _call_do_edgeGeneration_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_do_edgeGeneration_void(void* impl_msg, void* impl_obj_void)
{
  Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->do_edgeGeneration();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > void CkIndex_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeGroup);
  CkRegisterBase(__idx, CkIndex_IrrGroup::__idx);
   CkRegisterGroupIrr(__idx,Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::isIrreducible());
  // REG: Random_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
  idx_Random_Generator_Tram_marshall1();

  // REG: threaded void generate(const CkCallback &cb);
  idx_generate_marshall2();

  // REG: void do_edgeGeneration(void);
  idx_do_edgeGeneration_void();

}
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 
struct CBase_Random_Generator_Tram : public Group, virtual CBase
 {
  CProxy_Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  thisProxy;
  CBase_Random_Generator_Tram() : thisProxy(this)
    , Group()
  { }
  CBase_Random_Generator_Tram(CkMigrateMessage* m) : thisProxy(this)
    , Group(m)  { }
  template <typename... Args>
  CBase_Random_Generator_Tram(Args... args) : thisProxy(this)
    , Group(args...)  { }
  void pup(PUP::er &p) { }
  void virtual_pup(PUP::er &p) {
    recursive_pup<Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  >(dynamic_cast<Random_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * >(this), p);
}
  void parent_pup(PUP::er &p) {
    recursive_pup< Group >(this, p);
    p|thisProxy;
  }
};
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: template < class Graph, class Opts, VertexMapping vertexMapping > group Ring_Generator_Charm: IrrGroup{
Ring_Generator_Charm(const Graph &g, const Opts &opts);
threaded void generate(const CkCallback &cb);
void do_edgeGeneration(void);
};
 */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping >  int CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
/* DEFS: Ring_Generator_Charm(const Graph &g, const Opts &opts);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxyElement_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Ring_Generator_Charm(const Graph &g, const Opts &opts);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

CkGroupID CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::ckNew(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  return CkCreateGroup(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::__idx, CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_Ring_Generator_Charm_marshall1(), impl_msg);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

  CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::CProxy_Ring_Generator_Charm(const Graph &g, const Opts &opts, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  ckSetGroupID(CkCreateGroup(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::__idx, CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_Ring_Generator_Charm_marshall1(), impl_msg));
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::reg_Ring_Generator_Charm_marshall1() {
  int epidx = CkRegisterEp("Ring_Generator_Charm(const Graph &g, const Opts &opts)",
      _call_Ring_Generator_Charm_marshall1, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_Ring_Generator_Charm_marshall1);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_Ring_Generator_Charm_marshall1);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::_call_Ring_Generator_Charm_marshall1(void* impl_msg, void* impl_obj_void)
{
  Ring_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<Ring_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) Ring_Generator_Charm < Graph, Opts, vertexMapping > (g, opts);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::_callmarshall_Ring_Generator_Charm_marshall1(char* impl_buf, void* impl_obj_void) {
  Ring_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast< Ring_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) Ring_Generator_Charm < Graph, Opts, vertexMapping > (g, opts);
  return implP.size();
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::_marshallmessagepup_Ring_Generator_Charm_marshall1(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("g");
  implDestP|g;
  if (implDestP.hasComments()) implDestP.comment("opts");
  implDestP|opts;
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchMulti(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchGroup(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::reg_generate_marshall2() {
  int epidx = CkRegisterEp("generate(const CkCallback &cb)",
      _call_generate_marshall2, CkMarshallMsg::__idx, __idx, 0);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_generate_marshall2);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::_call_generate_marshall2(void* impl_msg, void* impl_obj_void)
{
  Ring_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<Ring_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CthThread tid = CthCreate((CthVoidFn)_callthr_generate_marshall2, new CkThrCallArg(impl_msg,impl_obj), 0);
  ((Chare *)impl_obj)->CkAddThreadListeners(tid,impl_msg);
  CthResume(tid);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::_callthr_generate_marshall2(CkThrCallArg *impl_arg)
{
  void *impl_msg = impl_arg->msg;
  Ring_Generator_Charm < Graph, Opts, vertexMapping >  *impl_obj = (Ring_Generator_Charm < Graph, Opts, vertexMapping >  *) impl_arg->obj;
  delete impl_arg;
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->generate(cb);
  delete impl_msg_typed;
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::_marshallmessagepup_generate_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("cb");
  implDestP|cb;
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::generate_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping > 

int CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::reg_do_edgeGeneration_void() {
  int epidx = CkRegisterEp("do_edgeGeneration(void)",
      _call_do_edgeGeneration_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping > 

void CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::_call_do_edgeGeneration_void(void* impl_msg, void* impl_obj_void)
{
  Ring_Generator_Charm < Graph, Opts, vertexMapping > * impl_obj = static_cast<Ring_Generator_Charm < Graph, Opts, vertexMapping >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->do_edgeGeneration();
}
template < class Graph, class Opts, VertexMapping vertexMapping > PUPable_def_template_nonInst(SINGLE_ARG(Closure_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Ring_Generator_Charm(const Graph &g, const Opts &opts);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_generate_marshall2(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping > 

void CProxySection_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::idx_do_edgeGeneration_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > void CkIndex_Ring_Generator_Charm < Graph, Opts, vertexMapping > ::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeGroup);
  CkRegisterBase(__idx, CkIndex_IrrGroup::__idx);
   CkRegisterGroupIrr(__idx,Ring_Generator_Charm < Graph, Opts, vertexMapping > ::isIrreducible());
  // REG: Ring_Generator_Charm(const Graph &g, const Opts &opts);
  idx_Ring_Generator_Charm_marshall1();

  // REG: threaded void generate(const CkCallback &cb);
  idx_generate_marshall2();

  // REG: void do_edgeGeneration(void);
  idx_do_edgeGeneration_void();

}
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping > 
struct CBase_Ring_Generator_Charm : public Group, virtual CBase
 {
  CProxy_Ring_Generator_Charm < Graph, Opts, vertexMapping >  thisProxy;
  CBase_Ring_Generator_Charm() : thisProxy(this)
    , Group()
  { }
  CBase_Ring_Generator_Charm(CkMigrateMessage* m) : thisProxy(this)
    , Group(m)  { }
  template <typename... Args>
  CBase_Ring_Generator_Charm(Args... args) : thisProxy(this)
    , Group(args...)  { }
  void pup(PUP::er &p) { }
  void virtual_pup(PUP::er &p) {
    recursive_pup<Ring_Generator_Charm < Graph, Opts, vertexMapping >  >(dynamic_cast<Ring_Generator_Charm < Graph, Opts, vertexMapping > * >(this), p);
}
  void parent_pup(PUP::er &p) {
    recursive_pup< Group >(this, p);
    p|thisProxy;
  }
};
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > group Ring_Generator_Tram: IrrGroup{
Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
threaded void generate(const CkCallback &cb);
void do_edgeGeneration(void);
};
 */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator >  int CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
/* DEFS: Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxyElement_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSend(ckDelegatedPtr(),CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID());
  } else CkSendMsgBranch(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupPe(), ckGetGroupID(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

CkGroupID CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::ckNew(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  return CkCreateGroup(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__idx, CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_Ring_Generator_Tram_marshall1(), impl_msg);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

  CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::CProxy_Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator, const CkEntryOptions *impl_e_opts)
{
  //Marshall: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(Graph &)g;
    //Have to cast away const-ness to get pup routine
    implP|(Opts &)opts;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_Aggregator &)aggregator;
  }
  UsrToEnv(impl_msg)->setMsgtype(BocInitMsg);
  if (impl_e_opts)
    UsrToEnv(impl_msg)->setGroupDep(impl_e_opts->getGroupDepID());
  ckSetGroupID(CkCreateGroup(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__idx, CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_Ring_Generator_Tram_marshall1(), impl_msg));
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_Ring_Generator_Tram_marshall1() {
  int epidx = CkRegisterEp("Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator)",
      _call_Ring_Generator_Tram_marshall1, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_Ring_Generator_Tram_marshall1);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_Ring_Generator_Tram_marshall1);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_Ring_Generator_Tram_marshall1(void* impl_msg, void* impl_obj_void)
{
  Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  CProxy_Aggregator aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > (g, opts, aggregator);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_callmarshall_Ring_Generator_Tram_marshall1(char* impl_buf, void* impl_obj_void) {
  Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast< Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  CProxy_Aggregator aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  new (impl_obj) Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > (g, opts, aggregator);
  return implP.size();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_marshallmessagepup_Ring_Generator_Tram_marshall1(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator*/
  PUP::fromMem implP(impl_buf);
  Graph g; implP|g;
  Opts opts; implP|opts;
  CProxy_Aggregator aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("g");
  implDestP|g;
  if (implDestP.hasComments()) implDestP.comment("opts");
  implDestP|opts;
  if (implDestP.hasComments()) implDestP.comment("aggregator");
  implDestP|aggregator;
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, int npes, int *pes, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchMulti(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, CmiGroup &grp, const CkEntryOptions *impl_e_opts) {
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  CkSendMsgBranchGroup(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_generate_marshall2() {
  int epidx = CkRegisterEp("generate(const CkCallback &cb)",
      _call_generate_marshall2, CkMarshallMsg::__idx, __idx, 0);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_generate_marshall2);

  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_generate_marshall2(void* impl_msg, void* impl_obj_void)
{
  Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CthThread tid = CthCreate((CthVoidFn)_callthr_generate_marshall2, new CkThrCallArg(impl_msg,impl_obj), 0);
  ((Chare *)impl_obj)->CkAddThreadListeners(tid,impl_msg);
  CthResume(tid);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_callthr_generate_marshall2(CkThrCallArg *impl_arg)
{
  void *impl_msg = impl_arg->msg;
  Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *impl_obj = (Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *) impl_arg->obj;
  delete impl_arg;
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->generate(cb);
  delete impl_msg_typed;
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_marshallmessagepup_generate_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CkCallback &cb*/
  PUP::fromMem implP(impl_buf);
  CkCallback cb; implP|cb;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("cb");
  implDestP|cb;
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupBroadcast(ckDelegatedPtr(),CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
  } else CkBroadcastMsgBranch(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(),0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(int npes, int *pes) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchMulti(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), npes, pes,0);
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(CmiGroup &grp) {
  void *impl_msg = CkAllocSysMsg();
  CkSendMsgBranchGroup(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID(), grp,0);
}

// Entry point registration function
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

int CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::reg_do_edgeGeneration_void() {
  int epidx = CkRegisterEp("do_edgeGeneration(void)",
      _call_do_edgeGeneration_void, 0, __idx, 0);
  return epidx;
}

template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::_call_do_edgeGeneration_void(void* impl_msg, void* impl_obj_void)
{
  Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * impl_obj = static_cast<Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->do_edgeGeneration();
}
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > PUPable_def_template_nonInst(SINGLE_ARG(Closure_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: threaded void generate(const CkCallback &cb);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::generate(const CkCallback &cb, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CkCallback &cb
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CkCallback &)cb;
  }
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_generate_marshall2(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void do_edgeGeneration(void);
 */
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 

void CProxySection_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::do_edgeGeneration(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
     CkGroupMsgPrep(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetGroupID());
     ckDelegatedTo()->GroupSectionSend(ckDelegatedPtr(),CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg, ckGetNumSections(), ckGetSectionIDs());
  } else {
    void *impl_msg_tmp = (ckGetNumSections()>1) ? CkCopyMsg((void **) &impl_msg) : impl_msg;
    for (int i=0; i<ckGetNumSections(); ++i) {
       impl_msg_tmp= (i<ckGetNumSections()-1) ? CkCopyMsg((void **) &impl_msg):impl_msg;
       CkSendMsgBranchMulti(CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::idx_do_edgeGeneration_void(), impl_msg_tmp, ckGetGroupIDn(i), ckGetNumElements(i), ckGetElements(i),0);
    }
  }
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > void CkIndex_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeGroup);
  CkRegisterBase(__idx, CkIndex_IrrGroup::__idx);
   CkRegisterGroupIrr(__idx,Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > ::isIrreducible());
  // REG: Ring_Generator_Tram(const Graph &g, const Opts &opts, const CProxy_Aggregator &aggregator);
  idx_Ring_Generator_Tram_marshall1();

  // REG: threaded void generate(const CkCallback &cb);
  idx_generate_marshall2();

  // REG: void do_edgeGeneration(void);
  idx_do_edgeGeneration_void();

}
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < class Graph, class Opts, VertexMapping vertexMapping, class CProxy_Aggregator > 
struct CBase_Ring_Generator_Tram : public Group, virtual CBase
 {
  CProxy_Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  thisProxy;
  CBase_Ring_Generator_Tram() : thisProxy(this)
    , Group()
  { }
  CBase_Ring_Generator_Tram(CkMigrateMessage* m) : thisProxy(this)
    , Group(m)  { }
  template <typename... Args>
  CBase_Ring_Generator_Tram(Args... args) : thisProxy(this)
    , Group(args...)  { }
  void pup(PUP::er &p) { }
  void virtual_pup(PUP::er &p) {
    recursive_pup<Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator >  >(dynamic_cast<Ring_Generator_Tram < Graph, Opts, vertexMapping, CProxy_Aggregator > * >(this), p);
}
  void parent_pup(PUP::er &p) {
    recursive_pup< Group >(this, p);
    p|thisProxy;
  }
};
#endif /* CK_TEMPLATES_ONLY */

} // namespace GraphLib

#ifndef CK_TEMPLATES_ONLY
void _registerGraphGenerator(void)
{
  static int _done = 0; if(_done) return; _done = 1;
  _registerNDMeshStreamer();

using namespace GraphLib;









}
#endif /* CK_TEMPLATES_ONLY */

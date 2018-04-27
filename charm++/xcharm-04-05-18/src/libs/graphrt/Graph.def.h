


namespace GraphLib {
/* ---------------- method closures -------------- */
#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < typename V, typename E, TransportType transportType > 

    struct Closure_Vertex < V, E, transportType > ::connectVertex_2_closure : public SDAG::Closure {
      E e;


      connectVertex_2_closure() {
        init();
      }
      connectVertex_2_closure(CkMigrateMessage*) {
        init();
      }
      E & getP0() { return e;}
      void pup(PUP::er& __p) {
        __p | e;
        packClosure(__p);
      }
      virtual ~connectVertex_2_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(connectVertex_2_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < typename V, typename E, TransportType transportType > 

    struct Closure_Vertex < V, E, transportType > ::addEdge_3_closure : public SDAG::Closure {
      E e;


      addEdge_3_closure() {
        init();
      }
      addEdge_3_closure(CkMigrateMessage*) {
        init();
      }
      E & getP0() { return e;}
      void pup(PUP::er& __p) {
        __p | e;
        packClosure(__p);
      }
      virtual ~addEdge_3_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(addEdge_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < typename V, typename E, TransportType transportType > 
template < typename M > 
    struct Closure_Vertex < V, E, transportType > ::recv_4_closure : public SDAG::Closure {
      M m;


      recv_4_closure() {
        init();
      }
      recv_4_closure(CkMigrateMessage*) {
        init();
      }
      M & getP0() { return m;}
      void pup(PUP::er& __p) {
        __p | m;
        packClosure(__p);
      }
      virtual ~recv_4_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(recv_4_closure<M>));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
template < typename V, typename E, TransportType transportType > 
template < typename M > 
    struct Closure_Vertex < V, E, transportType > ::init_5_closure : public SDAG::Closure {
      CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > aggregator;


      init_5_closure() {
        init();
      }
      init_5_closure(CkMigrateMessage*) {
        init();
      }
      CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > & getP0() { return aggregator;}
      void pup(PUP::er& __p) {
        __p | aggregator;
        packClosure(__p);
      }
      virtual ~init_5_closure() {
      }
      PUPable_decl_template(SINGLE_ARG(init_5_closure<M>));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */


} // namespace GraphLib




namespace GraphLib {
/* DEFS: template < typename V, typename E, TransportType transportType > array Vertex: ArrayElement{
Vertex(void);
void connectVertex(const E &e);
void addEdge(const E &e);
void recv(const M &m);
void init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator);
Vertex(CkMigrateMessage* impl_msg);
};
 */
#ifdef CK_TEMPLATES_ONLY
template < typename V, typename E, TransportType transportType >  int CkIndex_Vertex < V, E, transportType > ::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
/* DEFS: Vertex(void);
 */
template < typename V, typename E, TransportType transportType > 

void CProxyElement_Vertex < V, E, transportType > ::insert(int onPE)
{ 
  void *impl_msg = CkAllocSysMsg();
   UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
   ckInsert((CkArrayMessage *)impl_msg,CkIndex_Vertex < V, E, transportType > ::idx_Vertex_void(),onPE);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void connectVertex(const E &e);
 */
template < typename V, typename E, TransportType transportType > 

void CProxyElement_Vertex < V, E, transportType > ::connectVertex(const E &e, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const E &e
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(E &)e;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(E &)e;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_Vertex < V, E, transportType > ::idx_connectVertex_marshall2(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addEdge(const E &e);
 */
template < typename V, typename E, TransportType transportType > 

void CProxyElement_Vertex < V, E, transportType > ::addEdge(const E &e, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const E &e
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(E &)e;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(E &)e;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_Vertex < V, E, transportType > ::idx_addEdge_marshall3(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void recv(const M &m);
 */
template < typename V, typename E, TransportType transportType > 
template < typename M > 
void CProxyElement_Vertex < V, E, transportType > ::recv(const M &m, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const M &m
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(M &)m;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(M &)m;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_Vertex < V, E, transportType > ::template idx_recv_marshall4< M >(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator);
 */
template < typename V, typename E, TransportType transportType > 
template < typename M > 
void CProxyElement_Vertex < V, E, transportType > ::init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &)aggregator;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &)aggregator;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_Vertex < V, E, transportType > ::template idx_init_marshall5< M >(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Vertex(CkMigrateMessage* impl_msg);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Vertex(void);
 */
template < typename V, typename E, TransportType transportType > 

CkArrayID CProxy_Vertex < V, E, transportType > ::ckNew(const CkArrayOptions &opts)
{
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  return ckCreateArray((CkArrayMessage *)impl_msg, CkIndex_Vertex < V, E, transportType > ::idx_Vertex_void(), opts);
}
template < typename V, typename E, TransportType transportType > 

void CProxy_Vertex < V, E, transportType > ::ckNew(const CkArrayOptions &opts, CkCallback _ck_array_creation_cb)
{
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  CkSendAsyncCreateArray(CkIndex_Vertex < V, E, transportType > ::idx_Vertex_void(), _ck_array_creation_cb, opts, impl_msg);
}
template < typename V, typename E, TransportType transportType > 

CkArrayID CProxy_Vertex < V, E, transportType > ::ckNew(const int s1)
{
  void *impl_msg = CkAllocSysMsg();
  CkArrayOptions opts(s1);
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  return ckCreateArray((CkArrayMessage *)impl_msg, CkIndex_Vertex < V, E, transportType > ::idx_Vertex_void(), opts);
}
template < typename V, typename E, TransportType transportType > 

void CProxy_Vertex < V, E, transportType > ::ckNew(const int s1, CkCallback _ck_array_creation_cb)
{
  void *impl_msg = CkAllocSysMsg();
  CkArrayOptions opts(s1);
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  CkSendAsyncCreateArray(CkIndex_Vertex < V, E, transportType > ::idx_Vertex_void(), _ck_array_creation_cb, opts, impl_msg);
}

// Entry point registration function
template < typename V, typename E, TransportType transportType > 

int CkIndex_Vertex < V, E, transportType > ::reg_Vertex_void() {
  int epidx = CkRegisterEp("Vertex(void)",
      _call_Vertex_void, 0, __idx, 0);
  return epidx;
}

template < typename V, typename E, TransportType transportType > 

void CkIndex_Vertex < V, E, transportType > ::_call_Vertex_void(void* impl_msg, void* impl_obj_void)
{
  Vertex < V, E, transportType > * impl_obj = static_cast<Vertex < V, E, transportType >  *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  new (impl_obj) Vertex < V, E, transportType > ();
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void connectVertex(const E &e);
 */
template < typename V, typename E, TransportType transportType > 

void CProxy_Vertex < V, E, transportType > ::connectVertex(const E &e, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const E &e
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(E &)e;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(E &)e;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_Vertex < V, E, transportType > ::idx_connectVertex_marshall2(),0);
}

// Entry point registration function
template < typename V, typename E, TransportType transportType > 

int CkIndex_Vertex < V, E, transportType > ::reg_connectVertex_marshall2() {
  int epidx = CkRegisterEp("connectVertex(const E &e)",
      _call_connectVertex_marshall2, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_connectVertex_marshall2);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_connectVertex_marshall2);

  return epidx;
}

template < typename V, typename E, TransportType transportType > 

void CkIndex_Vertex < V, E, transportType > ::_call_connectVertex_marshall2(void* impl_msg, void* impl_obj_void)
{
  Vertex < V, E, transportType > * impl_obj = static_cast<Vertex < V, E, transportType >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const E &e*/
  PUP::fromMem implP(impl_buf);
  E e; implP|e;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->connectVertex(e);
}
template < typename V, typename E, TransportType transportType > 

int CkIndex_Vertex < V, E, transportType > ::_callmarshall_connectVertex_marshall2(char* impl_buf, void* impl_obj_void) {
  Vertex < V, E, transportType > * impl_obj = static_cast< Vertex < V, E, transportType >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const E &e*/
  PUP::fromMem implP(impl_buf);
  E e; implP|e;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->connectVertex(e);
  return implP.size();
}
template < typename V, typename E, TransportType transportType > 

void CkIndex_Vertex < V, E, transportType > ::_marshallmessagepup_connectVertex_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const E &e*/
  PUP::fromMem implP(impl_buf);
  E e; implP|e;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("e");
  implDestP|e;
}
template < typename V, typename E, TransportType transportType > PUPable_def_template_nonInst(SINGLE_ARG(Closure_Vertex < V, E, transportType > ::connectVertex_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addEdge(const E &e);
 */
template < typename V, typename E, TransportType transportType > 

void CProxy_Vertex < V, E, transportType > ::addEdge(const E &e, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const E &e
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(E &)e;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(E &)e;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_Vertex < V, E, transportType > ::idx_addEdge_marshall3(),0);
}

// Entry point registration function
template < typename V, typename E, TransportType transportType > 

int CkIndex_Vertex < V, E, transportType > ::reg_addEdge_marshall3() {
  int epidx = CkRegisterEp("addEdge(const E &e)",
      _call_addEdge_marshall3, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_addEdge_marshall3);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_addEdge_marshall3);

  return epidx;
}

template < typename V, typename E, TransportType transportType > 

void CkIndex_Vertex < V, E, transportType > ::_call_addEdge_marshall3(void* impl_msg, void* impl_obj_void)
{
  Vertex < V, E, transportType > * impl_obj = static_cast<Vertex < V, E, transportType >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const E &e*/
  PUP::fromMem implP(impl_buf);
  E e; implP|e;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->addEdge(e);
}
template < typename V, typename E, TransportType transportType > 

int CkIndex_Vertex < V, E, transportType > ::_callmarshall_addEdge_marshall3(char* impl_buf, void* impl_obj_void) {
  Vertex < V, E, transportType > * impl_obj = static_cast< Vertex < V, E, transportType >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const E &e*/
  PUP::fromMem implP(impl_buf);
  E e; implP|e;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->addEdge(e);
  return implP.size();
}
template < typename V, typename E, TransportType transportType > 

void CkIndex_Vertex < V, E, transportType > ::_marshallmessagepup_addEdge_marshall3(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const E &e*/
  PUP::fromMem implP(impl_buf);
  E e; implP|e;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("e");
  implDestP|e;
}
template < typename V, typename E, TransportType transportType > PUPable_def_template_nonInst(SINGLE_ARG(Closure_Vertex < V, E, transportType > ::addEdge_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void recv(const M &m);
 */
template < typename V, typename E, TransportType transportType > 
template < typename M > 
void CProxy_Vertex < V, E, transportType > ::recv(const M &m, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const M &m
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(M &)m;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(M &)m;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_Vertex < V, E, transportType > ::template idx_recv_marshall4< M >(),0);
}

// Entry point registration function
template < typename V, typename E, TransportType transportType > 
template < typename M > 
int CkIndex_Vertex < V, E, transportType > ::reg_recv_marshall4() {
  int epidx = CkRegisterEp("recv(const M &m)",
      _call_recv_marshall4< M >, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_recv_marshall4< M >);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_recv_marshall4< M >);

  return epidx;
}

template < typename V, typename E, TransportType transportType > 
template < typename M > 
void CkIndex_Vertex < V, E, transportType > ::_call_recv_marshall4(void* impl_msg, void* impl_obj_void)
{
  Vertex < V, E, transportType > * impl_obj = static_cast<Vertex < V, E, transportType >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const M &m*/
  PUP::fromMem implP(impl_buf);
  M m; implP|m;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->template recv< M >(m);
}
template < typename V, typename E, TransportType transportType > 
template < typename M > 
int CkIndex_Vertex < V, E, transportType > ::_callmarshall_recv_marshall4(char* impl_buf, void* impl_obj_void) {
  Vertex < V, E, transportType > * impl_obj = static_cast< Vertex < V, E, transportType >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const M &m*/
  PUP::fromMem implP(impl_buf);
  M m; implP|m;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->template recv< M >(m);
  return implP.size();
}
template < typename V, typename E, TransportType transportType > 
template < typename M > 
void CkIndex_Vertex < V, E, transportType > ::_marshallmessagepup_recv_marshall4(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const M &m*/
  PUP::fromMem implP(impl_buf);
  M m; implP|m;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("m");
  implDestP|m;
}
template < typename V, typename E, TransportType transportType > template <typename M> PUPable_def_template_nonInst(SINGLE_ARG(Closure_Vertex < V, E, transportType > ::recv_4_closure<M>));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator);
 */
template < typename V, typename E, TransportType transportType > 
template < typename M > 
void CProxy_Vertex < V, E, transportType > ::init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &)aggregator;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &)aggregator;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_Vertex < V, E, transportType > ::template idx_init_marshall5< M >(),0);
}

// Entry point registration function
template < typename V, typename E, TransportType transportType > 
template < typename M > 
int CkIndex_Vertex < V, E, transportType > ::reg_init_marshall5() {
  int epidx = CkRegisterEp("init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator)",
      _call_init_marshall5< M >, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_init_marshall5< M >);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_init_marshall5< M >);

  return epidx;
}

template < typename V, typename E, TransportType transportType > 
template < typename M > 
void CkIndex_Vertex < V, E, transportType > ::_call_init_marshall5(void* impl_msg, void* impl_obj_void)
{
  Vertex < V, E, transportType > * impl_obj = static_cast<Vertex < V, E, transportType >  *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator*/
  PUP::fromMem implP(impl_buf);
  CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->template init< M >(aggregator);
}
template < typename V, typename E, TransportType transportType > 
template < typename M > 
int CkIndex_Vertex < V, E, transportType > ::_callmarshall_init_marshall5(char* impl_buf, void* impl_obj_void) {
  Vertex < V, E, transportType > * impl_obj = static_cast< Vertex < V, E, transportType >  *>(impl_obj_void);
  /*Unmarshall pup'd fields: const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator*/
  PUP::fromMem implP(impl_buf);
  CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->template init< M >(aggregator);
  return implP.size();
}
template < typename V, typename E, TransportType transportType > 
template < typename M > 
void CkIndex_Vertex < V, E, transportType > ::_marshallmessagepup_init_marshall5(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator*/
  PUP::fromMem implP(impl_buf);
  CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > aggregator; implP|aggregator;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("aggregator");
  implDestP|aggregator;
}
template < typename V, typename E, TransportType transportType > template <typename M> PUPable_def_template_nonInst(SINGLE_ARG(Closure_Vertex < V, E, transportType > ::init_5_closure<M>));
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Vertex(CkMigrateMessage* impl_msg);
 */

// Entry point registration function
template < typename V, typename E, TransportType transportType > 

int CkIndex_Vertex < V, E, transportType > ::reg_Vertex_CkMigrateMessage() {
  int epidx = CkRegisterEp("Vertex(CkMigrateMessage* impl_msg)",
      _call_Vertex_CkMigrateMessage, 0, __idx, 0);
  return epidx;
}

template < typename V, typename E, TransportType transportType > 

void CkIndex_Vertex < V, E, transportType > ::_call_Vertex_CkMigrateMessage(void* impl_msg, void* impl_obj_void)
{
  Vertex < V, E, transportType > * impl_obj = static_cast<Vertex < V, E, transportType >  *>(impl_obj_void);
  new (impl_obj) Vertex < V, E, transportType > ((CkMigrateMessage*)impl_msg);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Vertex(void);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void connectVertex(const E &e);
 */
template < typename V, typename E, TransportType transportType > 

void CProxySection_Vertex < V, E, transportType > ::connectVertex(const E &e, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const E &e
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(E &)e;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(E &)e;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_Vertex < V, E, transportType > ::idx_connectVertex_marshall2(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void addEdge(const E &e);
 */
template < typename V, typename E, TransportType transportType > 

void CProxySection_Vertex < V, E, transportType > ::addEdge(const E &e, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const E &e
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(E &)e;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(E &)e;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_Vertex < V, E, transportType > ::idx_addEdge_marshall3(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void recv(const M &m);
 */
template < typename V, typename E, TransportType transportType > 
template < typename M > 
void CProxySection_Vertex < V, E, transportType > ::recv(const M &m, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const M &m
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(M &)m;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(M &)m;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_Vertex < V, E, transportType > ::template idx_recv_marshall4< M >(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: void init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator);
 */
template < typename V, typename E, TransportType transportType > 
template < typename M > 
void CProxySection_Vertex < V, E, transportType > ::init(const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &aggregator
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &)aggregator;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CProxy_ArrayMeshStreamer<M,long long,V,SimpleMeshRouter > &)aggregator;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_Vertex < V, E, transportType > ::template idx_init_marshall5< M >(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
/* DEFS: Vertex(CkMigrateMessage* impl_msg);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifdef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < typename V, typename E, TransportType transportType > void CkIndex_Vertex < V, E, transportType > ::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeArray);
  CkRegisterBase(__idx, CkIndex_ArrayElement::__idx);
  // REG: Vertex(void);
  idx_Vertex_void();
  CkRegisterDefaultCtor(__idx, idx_Vertex_void());

  // REG: void connectVertex(const E &e);
  idx_connectVertex_marshall2();

  // REG: void addEdge(const E &e);
  idx_addEdge_marshall3();



  // REG: Vertex(CkMigrateMessage* impl_msg);
  idx_Vertex_CkMigrateMessage();
  CkRegisterMigCtor(__idx, idx_Vertex_CkMigrateMessage());

}
#endif /* CK_TEMPLATES_ONLY */
#ifdef CK_TEMPLATES_ONLY
template < typename V, typename E, TransportType transportType > 
struct CBase_Vertex : public ArrayElementT<CkIndex1D>, virtual CBase
 {
  CProxy_Vertex < V, E, transportType >  thisProxy;
  CBase_Vertex() : thisProxy(this)
    , ArrayElementT<CkIndex1D>()
  { }
  CBase_Vertex(CkMigrateMessage* m) : thisProxy(this)
    , ArrayElementT<CkIndex1D>(m)  { }
  template <typename... Args>
  CBase_Vertex(Args... args) : thisProxy(this)
    , ArrayElementT<CkIndex1D>(args...)  { }
  void pup(PUP::er &p) { }
  void virtual_pup(PUP::er &p) {
    recursive_pup<Vertex < V, E, transportType >  >(dynamic_cast<Vertex < V, E, transportType > * >(this), p);
}
  void parent_pup(PUP::er &p) {
    recursive_pup< ArrayElementT<CkIndex1D> >(this, p);
    p|thisProxy;
  }
};
#endif /* CK_TEMPLATES_ONLY */

} // namespace GraphLib

#ifndef CK_TEMPLATES_ONLY
void _registerGraph(void)
{
  static int _done = 0; if(_done) return; _done = 1;
  _registerGraphGenerator();

  _registerNDMeshStreamer();


using namespace GraphLib;


}
#endif /* CK_TEMPLATES_ONLY */

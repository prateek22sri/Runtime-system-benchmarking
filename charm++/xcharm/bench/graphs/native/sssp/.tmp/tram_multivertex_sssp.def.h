





/* ---------------- method closures -------------- */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_TestDriver::start_2_closure : public SDAG::Closure {
      

      start_2_closure() {
        init();
      }
      start_2_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~start_2_closure() {
      }
      PUPable_decl(SINGLE_ARG(start_2_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_TestDriver::restart_3_closure : public SDAG::Closure {
      

      restart_3_closure() {
        init();
      }
      restart_3_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~restart_3_closure() {
      }
      PUPable_decl(SINGLE_ARG(restart_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_TestDriver::startVerificationPhase_4_closure : public SDAG::Closure {
      

      startVerificationPhase_4_closure() {
        init();
      }
      startVerificationPhase_4_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~startVerificationPhase_4_closure() {
      }
      PUPable_decl(SINGLE_ARG(startVerificationPhase_4_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_TestDriver::done_5_closure : public SDAG::Closure {
      CmiUInt8 globalNumScannedEdges;


      done_5_closure() {
        init();
      }
      done_5_closure(CkMigrateMessage*) {
        init();
      }
      CmiUInt8 & getP0() { return globalNumScannedEdges;}
      void pup(PUP::er& __p) {
        __p | globalNumScannedEdges;
        packClosure(__p);
      }
      virtual ~done_5_closure() {
      }
      PUPable_decl(SINGLE_ARG(done_5_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_TestDriver::startGraphConstruction_6_closure : public SDAG::Closure {
      

      startGraphConstruction_6_closure() {
        init();
      }
      startGraphConstruction_6_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~startGraphConstruction_6_closure() {
      }
      PUPable_decl(SINGLE_ARG(startGraphConstruction_6_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_TestDriver::exit_7_closure : public SDAG::Closure {
      

      exit_7_closure() {
        init();
      }
      exit_7_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~exit_7_closure() {
      }
      PUPable_decl(SINGLE_ARG(exit_7_closure));
    };
#endif /* CK_TEMPLATES_ONLY */



/* ---------------- method closures -------------- */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_SSSPMultiVertex::connectVertex_2_closure : public SDAG::Closure {
      std::pair<CmiUInt8,SSSPEdge > edge;


      connectVertex_2_closure() {
        init();
      }
      connectVertex_2_closure(CkMigrateMessage*) {
        init();
      }
      std::pair<CmiUInt8,SSSPEdge > & getP0() { return edge;}
      void pup(PUP::er& __p) {
        __p | edge;
        packClosure(__p);
      }
      virtual ~connectVertex_2_closure() {
      }
      PUPable_decl(SINGLE_ARG(connectVertex_2_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_SSSPMultiVertex::make_root_3_closure : public SDAG::Closure {
      CmiUInt8 root;


      make_root_3_closure() {
        init();
      }
      make_root_3_closure(CkMigrateMessage*) {
        init();
      }
      CmiUInt8 & getP0() { return root;}
      void pup(PUP::er& __p) {
        __p | root;
        packClosure(__p);
      }
      virtual ~make_root_3_closure() {
      }
      PUPable_decl(SINGLE_ARG(make_root_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_SSSPMultiVertex::verify_4_closure : public SDAG::Closure {
      

      verify_4_closure() {
        init();
      }
      verify_4_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~verify_4_closure() {
      }
      PUPable_decl(SINGLE_ARG(verify_4_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_SSSPMultiVertex::check_5_closure : public SDAG::Closure {
      CmiUInt8 v;
      double weight;


      check_5_closure() {
        init();
      }
      check_5_closure(CkMigrateMessage*) {
        init();
      }
      CmiUInt8 & getP0() { return v;}
      double & getP1() { return weight;}
      void pup(PUP::er& __p) {
        __p | v;
        __p | weight;
        packClosure(__p);
      }
      virtual ~check_5_closure() {
      }
      PUPable_decl(SINGLE_ARG(check_5_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_SSSPMultiVertex::getScannedVertexNum_6_closure : public SDAG::Closure {
      

      getScannedVertexNum_6_closure() {
        init();
      }
      getScannedVertexNum_6_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~getScannedVertexNum_6_closure() {
      }
      PUPable_decl(SINGLE_ARG(getScannedVertexNum_6_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_SSSPMultiVertex::foo_7_closure : public SDAG::Closure {
      

      foo_7_closure() {
        init();
      }
      foo_7_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~foo_7_closure() {
      }
      PUPable_decl(SINGLE_ARG(foo_7_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */












/* DEFS: readonly int K;
 */
extern int K;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_K(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|K;
}
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: readonly CmiUInt8 N;
 */
extern CmiUInt8 N;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_N(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|N;
}
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: readonly CmiUInt8 M;
 */
extern CmiUInt8 M;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_M(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|M;
}
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: readonly CProxy_TestDriver driverProxy;
 */
extern CProxy_TestDriver driverProxy;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_driverProxy(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|driverProxy;
}
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: readonly CProxy_ArrayMeshStreamer<dtype,long long,SSSPMultiVertex,SimpleMeshRouter > aggregator;
 */
extern CProxy_ArrayMeshStreamer<dtype,long long,SSSPMultiVertex,SimpleMeshRouter > aggregator;
#ifndef CK_TEMPLATES_ONLY
extern "C" void __xlater_roPup_aggregator(void *_impl_pup_er) {
  PUP::er &_impl_p=*(PUP::er *)_impl_pup_er;
  _impl_p|aggregator;
}
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: mainchare TestDriver: Chare{
TestDriver(CkArgMsg* impl_msg);
threaded void start(void);
threaded void restart(void);
void startVerificationPhase(void);
void done(const CmiUInt8 &globalNumScannedEdges);
void startGraphConstruction(void);
void exit(void);
};
 */
#ifndef CK_TEMPLATES_ONLY
 int CkIndex_TestDriver::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
/* DEFS: TestDriver(CkArgMsg* impl_msg);
 */

CkChareID CProxy_TestDriver::ckNew(CkArgMsg* impl_msg, int impl_onPE)
{
  CkChareID impl_ret;
  CkCreateChare(CkIndex_TestDriver::__idx, CkIndex_TestDriver::idx_TestDriver_CkArgMsg(), impl_msg, &impl_ret, impl_onPE);
  return impl_ret;
}

void CProxy_TestDriver::ckNew(CkArgMsg* impl_msg, CkChareID* pcid, int impl_onPE)
{
  CkCreateChare(CkIndex_TestDriver::__idx, CkIndex_TestDriver::idx_TestDriver_CkArgMsg(), impl_msg, pcid, impl_onPE);
}

  CProxy_TestDriver::CProxy_TestDriver(CkArgMsg* impl_msg, int impl_onPE)
{
  CkChareID impl_ret;
  CkCreateChare(CkIndex_TestDriver::__idx, CkIndex_TestDriver::idx_TestDriver_CkArgMsg(), impl_msg, &impl_ret, impl_onPE);
  ckSetChareID(impl_ret);
}

// Entry point registration function

int CkIndex_TestDriver::reg_TestDriver_CkArgMsg() {
  int epidx = CkRegisterEp("TestDriver(CkArgMsg* impl_msg)",
      _call_TestDriver_CkArgMsg, CMessage_CkArgMsg::__idx, __idx, 0);
  CkRegisterMessagePupFn(epidx, (CkMessagePupFn)CkArgMsg::ckDebugPup);
  return epidx;
}


void CkIndex_TestDriver::_call_TestDriver_CkArgMsg(void* impl_msg, void* impl_obj_void)
{
  TestDriver* impl_obj = static_cast<TestDriver *>(impl_obj_void);
  new (impl_obj) TestDriver((CkArgMsg*)impl_msg);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: threaded void start(void);
 */

void CProxy_TestDriver::start(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
    int destPE=CkChareMsgPrep(CkIndex_TestDriver::idx_start_void(), impl_msg, &ckGetChareID());
    if (destPE!=-1) ckDelegatedTo()->ChareSend(ckDelegatedPtr(),CkIndex_TestDriver::idx_start_void(), impl_msg, &ckGetChareID(),destPE);
  }
  else CkSendMsg(CkIndex_TestDriver::idx_start_void(), impl_msg, &ckGetChareID(),0);
}

// Entry point registration function

int CkIndex_TestDriver::reg_start_void() {
  int epidx = CkRegisterEp("start(void)",
      _call_start_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_TestDriver::_call_start_void(void* impl_msg, void* impl_obj_void)
{
  TestDriver* impl_obj = static_cast<TestDriver *>(impl_obj_void);
  CthThread tid = CthCreate((CthVoidFn)_callthr_start_void, new CkThrCallArg(impl_msg,impl_obj), 0);
  ((Chare *)impl_obj)->CkAddThreadListeners(tid,impl_msg);
  CthResume(tid);
}

void CkIndex_TestDriver::_callthr_start_void(CkThrCallArg *impl_arg)
{
  void *impl_msg = impl_arg->msg;
  TestDriver *impl_obj = (TestDriver *) impl_arg->obj;
  delete impl_arg;
  CkFreeSysMsg(impl_msg);
  impl_obj->start();
}
PUPable_def(SINGLE_ARG(Closure_TestDriver::start_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: threaded void restart(void);
 */

void CProxy_TestDriver::restart(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
    int destPE=CkChareMsgPrep(CkIndex_TestDriver::idx_restart_void(), impl_msg, &ckGetChareID());
    if (destPE!=-1) ckDelegatedTo()->ChareSend(ckDelegatedPtr(),CkIndex_TestDriver::idx_restart_void(), impl_msg, &ckGetChareID(),destPE);
  }
  else CkSendMsg(CkIndex_TestDriver::idx_restart_void(), impl_msg, &ckGetChareID(),0);
}

// Entry point registration function

int CkIndex_TestDriver::reg_restart_void() {
  int epidx = CkRegisterEp("restart(void)",
      _call_restart_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_TestDriver::_call_restart_void(void* impl_msg, void* impl_obj_void)
{
  TestDriver* impl_obj = static_cast<TestDriver *>(impl_obj_void);
  CthThread tid = CthCreate((CthVoidFn)_callthr_restart_void, new CkThrCallArg(impl_msg,impl_obj), 0);
  ((Chare *)impl_obj)->CkAddThreadListeners(tid,impl_msg);
  CthResume(tid);
}

void CkIndex_TestDriver::_callthr_restart_void(CkThrCallArg *impl_arg)
{
  void *impl_msg = impl_arg->msg;
  TestDriver *impl_obj = (TestDriver *) impl_arg->obj;
  delete impl_arg;
  CkFreeSysMsg(impl_msg);
  impl_obj->restart();
}
PUPable_def(SINGLE_ARG(Closure_TestDriver::restart_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void startVerificationPhase(void);
 */

void CProxy_TestDriver::startVerificationPhase(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
    int destPE=CkChareMsgPrep(CkIndex_TestDriver::idx_startVerificationPhase_void(), impl_msg, &ckGetChareID());
    if (destPE!=-1) ckDelegatedTo()->ChareSend(ckDelegatedPtr(),CkIndex_TestDriver::idx_startVerificationPhase_void(), impl_msg, &ckGetChareID(),destPE);
  }
  else CkSendMsg(CkIndex_TestDriver::idx_startVerificationPhase_void(), impl_msg, &ckGetChareID(),0);
}

// Entry point registration function

int CkIndex_TestDriver::reg_startVerificationPhase_void() {
  int epidx = CkRegisterEp("startVerificationPhase(void)",
      _call_startVerificationPhase_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_TestDriver::_call_startVerificationPhase_void(void* impl_msg, void* impl_obj_void)
{
  TestDriver* impl_obj = static_cast<TestDriver *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->startVerificationPhase();
}
PUPable_def(SINGLE_ARG(Closure_TestDriver::startVerificationPhase_4_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void done(const CmiUInt8 &globalNumScannedEdges);
 */

void CProxy_TestDriver::done(const CmiUInt8 &globalNumScannedEdges, const CkEntryOptions *impl_e_opts)
{
  ckCheck();
  //Marshall: const CmiUInt8 &globalNumScannedEdges
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)globalNumScannedEdges;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)globalNumScannedEdges;
  }
  if (ckIsDelegated()) {
    int destPE=CkChareMsgPrep(CkIndex_TestDriver::idx_done_marshall5(), impl_msg, &ckGetChareID());
    if (destPE!=-1) ckDelegatedTo()->ChareSend(ckDelegatedPtr(),CkIndex_TestDriver::idx_done_marshall5(), impl_msg, &ckGetChareID(),destPE);
  }
  else CkSendMsg(CkIndex_TestDriver::idx_done_marshall5(), impl_msg, &ckGetChareID(),0);
}

void CkIndex_TestDriver::_call_redn_wrapper_done_marshall5(void* impl_msg, void* impl_obj_void)
{
  TestDriver* impl_obj = static_cast<TestDriver*> (impl_obj_void);
  char* impl_buf = (char*)((CkReductionMsg*)impl_msg)->getData();
  /*Unmarshall pup'd fields: const CmiUInt8 &globalNumScannedEdges*/
  PUP::fromMem implP(impl_buf);
  /* non two-param case */
  CmiUInt8 globalNumScannedEdges; implP|globalNumScannedEdges;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->done(globalNumScannedEdges);
  delete (CkReductionMsg*)impl_msg;
}


// Entry point registration function

int CkIndex_TestDriver::reg_done_marshall5() {
  int epidx = CkRegisterEp("done(const CmiUInt8 &globalNumScannedEdges)",
      _call_done_marshall5, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_done_marshall5);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_done_marshall5);

  return epidx;
}


// Redn wrapper registration function

int CkIndex_TestDriver::reg_redn_wrapper_done_marshall5() {
  return CkRegisterEp("redn_wrapper_done(CkReductionMsg *impl_msg)",
      _call_redn_wrapper_done_marshall5, CkMarshallMsg::__idx, __idx, 0);
}


void CkIndex_TestDriver::_call_done_marshall5(void* impl_msg, void* impl_obj_void)
{
  TestDriver* impl_obj = static_cast<TestDriver *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &globalNumScannedEdges*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 globalNumScannedEdges; implP|globalNumScannedEdges;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->done(globalNumScannedEdges);
}

int CkIndex_TestDriver::_callmarshall_done_marshall5(char* impl_buf, void* impl_obj_void) {
  TestDriver* impl_obj = static_cast< TestDriver *>(impl_obj_void);
  /*Unmarshall pup'd fields: const CmiUInt8 &globalNumScannedEdges*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 globalNumScannedEdges; implP|globalNumScannedEdges;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->done(globalNumScannedEdges);
  return implP.size();
}

void CkIndex_TestDriver::_marshallmessagepup_done_marshall5(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &globalNumScannedEdges*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 globalNumScannedEdges; implP|globalNumScannedEdges;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("globalNumScannedEdges");
  implDestP|globalNumScannedEdges;
}
PUPable_def(SINGLE_ARG(Closure_TestDriver::done_5_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void startGraphConstruction(void);
 */

void CProxy_TestDriver::startGraphConstruction(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
    int destPE=CkChareMsgPrep(CkIndex_TestDriver::idx_startGraphConstruction_void(), impl_msg, &ckGetChareID());
    if (destPE!=-1) ckDelegatedTo()->ChareSend(ckDelegatedPtr(),CkIndex_TestDriver::idx_startGraphConstruction_void(), impl_msg, &ckGetChareID(),destPE);
  }
  else CkSendMsg(CkIndex_TestDriver::idx_startGraphConstruction_void(), impl_msg, &ckGetChareID(),0);
}

// Entry point registration function

int CkIndex_TestDriver::reg_startGraphConstruction_void() {
  int epidx = CkRegisterEp("startGraphConstruction(void)",
      _call_startGraphConstruction_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_TestDriver::_call_startGraphConstruction_void(void* impl_msg, void* impl_obj_void)
{
  TestDriver* impl_obj = static_cast<TestDriver *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->startGraphConstruction();
}
PUPable_def(SINGLE_ARG(Closure_TestDriver::startGraphConstruction_6_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void exit(void);
 */

void CProxy_TestDriver::exit(void)
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  if (ckIsDelegated()) {
    int destPE=CkChareMsgPrep(CkIndex_TestDriver::idx_exit_void(), impl_msg, &ckGetChareID());
    if (destPE!=-1) ckDelegatedTo()->ChareSend(ckDelegatedPtr(),CkIndex_TestDriver::idx_exit_void(), impl_msg, &ckGetChareID(),destPE);
  }
  else CkSendMsg(CkIndex_TestDriver::idx_exit_void(), impl_msg, &ckGetChareID(),0);
}

// Entry point registration function

int CkIndex_TestDriver::reg_exit_void() {
  int epidx = CkRegisterEp("exit(void)",
      _call_exit_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_TestDriver::_call_exit_void(void* impl_msg, void* impl_obj_void)
{
  TestDriver* impl_obj = static_cast<TestDriver *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->exit();
}
PUPable_def(SINGLE_ARG(Closure_TestDriver::exit_7_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
void CkIndex_TestDriver::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeMainChare);
  CkRegisterBase(__idx, CkIndex_Chare::__idx);
  // REG: TestDriver(CkArgMsg* impl_msg);
  idx_TestDriver_CkArgMsg();
  CkRegisterMainChare(__idx, idx_TestDriver_CkArgMsg());

  // REG: threaded void start(void);
  idx_start_void();

  // REG: threaded void restart(void);
  idx_restart_void();

  // REG: void startVerificationPhase(void);
  idx_startVerificationPhase_void();

  // REG: void done(const CmiUInt8 &globalNumScannedEdges);
  idx_done_marshall5();
  idx_redn_wrapper_done_marshall5();

  // REG: void startGraphConstruction(void);
  idx_startGraphConstruction_void();

  // REG: void exit(void);
  idx_exit_void();

}
#endif /* CK_TEMPLATES_ONLY */


/* DEFS: array SSSPMultiVertex: ArrayElement{
SSSPMultiVertex(void);
void connectVertex(const std::pair<CmiUInt8,SSSPEdge > &edge);
void make_root(const CmiUInt8 &root);
void verify(void);
void check(const CmiUInt8 &v, const double &weight);
void getScannedVertexNum(void);
void foo(void);
SSSPMultiVertex(CkMigrateMessage* impl_msg);
};
 */
#ifndef CK_TEMPLATES_ONLY
 int CkIndex_SSSPMultiVertex::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
/* DEFS: SSSPMultiVertex(void);
 */

void CProxyElement_SSSPMultiVertex::insert(int onPE)
{ 
  void *impl_msg = CkAllocSysMsg();
   UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
   ckInsert((CkArrayMessage *)impl_msg,CkIndex_SSSPMultiVertex::idx_SSSPMultiVertex_void(),onPE);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void connectVertex(const std::pair<CmiUInt8,SSSPEdge > &edge);
 */

void CProxyElement_SSSPMultiVertex::connectVertex(const std::pair<CmiUInt8,SSSPEdge > &edge, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const std::pair<CmiUInt8,SSSPEdge > &edge
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(std::pair<CmiUInt8,SSSPEdge > &)edge;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(std::pair<CmiUInt8,SSSPEdge > &)edge;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPMultiVertex::idx_connectVertex_marshall2(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void make_root(const CmiUInt8 &root);
 */

void CProxyElement_SSSPMultiVertex::make_root(const CmiUInt8 &root, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const CmiUInt8 &root
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)root;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)root;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPMultiVertex::idx_make_root_marshall3(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void verify(void);
 */

void CProxyElement_SSSPMultiVertex::verify(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPMultiVertex::idx_verify_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void check(const CmiUInt8 &v, const double &weight);
 */

void CProxyElement_SSSPMultiVertex::check(const CmiUInt8 &v, const double &weight, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const double &weight
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(double &)weight;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(double &)weight;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPMultiVertex::idx_check_marshall5(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void getScannedVertexNum(void);
 */

void CProxyElement_SSSPMultiVertex::getScannedVertexNum(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPMultiVertex::idx_getScannedVertexNum_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void foo(void);
 */

void CProxyElement_SSSPMultiVertex::foo(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPMultiVertex::idx_foo_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: SSSPMultiVertex(CkMigrateMessage* impl_msg);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: SSSPMultiVertex(void);
 */

CkArrayID CProxy_SSSPMultiVertex::ckNew(const CkArrayOptions &opts)
{
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  return ckCreateArray((CkArrayMessage *)impl_msg, CkIndex_SSSPMultiVertex::idx_SSSPMultiVertex_void(), opts);
}

void CProxy_SSSPMultiVertex::ckNew(const CkArrayOptions &opts, CkCallback _ck_array_creation_cb)
{
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  CkSendAsyncCreateArray(CkIndex_SSSPMultiVertex::idx_SSSPMultiVertex_void(), _ck_array_creation_cb, opts, impl_msg);
}

CkArrayID CProxy_SSSPMultiVertex::ckNew(const int s1)
{
  void *impl_msg = CkAllocSysMsg();
  CkArrayOptions opts(s1);
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  return ckCreateArray((CkArrayMessage *)impl_msg, CkIndex_SSSPMultiVertex::idx_SSSPMultiVertex_void(), opts);
}

void CProxy_SSSPMultiVertex::ckNew(const int s1, CkCallback _ck_array_creation_cb)
{
  void *impl_msg = CkAllocSysMsg();
  CkArrayOptions opts(s1);
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  CkSendAsyncCreateArray(CkIndex_SSSPMultiVertex::idx_SSSPMultiVertex_void(), _ck_array_creation_cb, opts, impl_msg);
}

// Entry point registration function

int CkIndex_SSSPMultiVertex::reg_SSSPMultiVertex_void() {
  int epidx = CkRegisterEp("SSSPMultiVertex(void)",
      _call_SSSPMultiVertex_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_SSSPMultiVertex::_call_SSSPMultiVertex_void(void* impl_msg, void* impl_obj_void)
{
  SSSPMultiVertex* impl_obj = static_cast<SSSPMultiVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  new (impl_obj) SSSPMultiVertex();
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void connectVertex(const std::pair<CmiUInt8,SSSPEdge > &edge);
 */

void CProxy_SSSPMultiVertex::connectVertex(const std::pair<CmiUInt8,SSSPEdge > &edge, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const std::pair<CmiUInt8,SSSPEdge > &edge
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(std::pair<CmiUInt8,SSSPEdge > &)edge;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(std::pair<CmiUInt8,SSSPEdge > &)edge;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPMultiVertex::idx_connectVertex_marshall2(),0);
}

// Entry point registration function

int CkIndex_SSSPMultiVertex::reg_connectVertex_marshall2() {
  int epidx = CkRegisterEp("connectVertex(const std::pair<CmiUInt8,SSSPEdge > &edge)",
      _call_connectVertex_marshall2, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_connectVertex_marshall2);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_connectVertex_marshall2);

  return epidx;
}


void CkIndex_SSSPMultiVertex::_call_connectVertex_marshall2(void* impl_msg, void* impl_obj_void)
{
  SSSPMultiVertex* impl_obj = static_cast<SSSPMultiVertex *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const std::pair<CmiUInt8,SSSPEdge > &edge*/
  PUP::fromMem implP(impl_buf);
  std::pair<CmiUInt8,SSSPEdge > edge; implP|edge;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->connectVertex(edge);
}

int CkIndex_SSSPMultiVertex::_callmarshall_connectVertex_marshall2(char* impl_buf, void* impl_obj_void) {
  SSSPMultiVertex* impl_obj = static_cast< SSSPMultiVertex *>(impl_obj_void);
  /*Unmarshall pup'd fields: const std::pair<CmiUInt8,SSSPEdge > &edge*/
  PUP::fromMem implP(impl_buf);
  std::pair<CmiUInt8,SSSPEdge > edge; implP|edge;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->connectVertex(edge);
  return implP.size();
}

void CkIndex_SSSPMultiVertex::_marshallmessagepup_connectVertex_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const std::pair<CmiUInt8,SSSPEdge > &edge*/
  PUP::fromMem implP(impl_buf);
  std::pair<CmiUInt8,SSSPEdge > edge; implP|edge;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("edge");
  implDestP|edge;
}
PUPable_def(SINGLE_ARG(Closure_SSSPMultiVertex::connectVertex_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void make_root(const CmiUInt8 &root);
 */

void CProxy_SSSPMultiVertex::make_root(const CmiUInt8 &root, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const CmiUInt8 &root
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)root;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)root;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPMultiVertex::idx_make_root_marshall3(),0);
}

// Entry point registration function

int CkIndex_SSSPMultiVertex::reg_make_root_marshall3() {
  int epidx = CkRegisterEp("make_root(const CmiUInt8 &root)",
      _call_make_root_marshall3, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_make_root_marshall3);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_make_root_marshall3);

  return epidx;
}


void CkIndex_SSSPMultiVertex::_call_make_root_marshall3(void* impl_msg, void* impl_obj_void)
{
  SSSPMultiVertex* impl_obj = static_cast<SSSPMultiVertex *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &root*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 root; implP|root;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->make_root(root);
}

int CkIndex_SSSPMultiVertex::_callmarshall_make_root_marshall3(char* impl_buf, void* impl_obj_void) {
  SSSPMultiVertex* impl_obj = static_cast< SSSPMultiVertex *>(impl_obj_void);
  /*Unmarshall pup'd fields: const CmiUInt8 &root*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 root; implP|root;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->make_root(root);
  return implP.size();
}

void CkIndex_SSSPMultiVertex::_marshallmessagepup_make_root_marshall3(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &root*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 root; implP|root;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("root");
  implDestP|root;
}
PUPable_def(SINGLE_ARG(Closure_SSSPMultiVertex::make_root_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void verify(void);
 */

void CProxy_SSSPMultiVertex::verify(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPMultiVertex::idx_verify_void(),0);
}

// Entry point registration function

int CkIndex_SSSPMultiVertex::reg_verify_void() {
  int epidx = CkRegisterEp("verify(void)",
      _call_verify_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_SSSPMultiVertex::_call_verify_void(void* impl_msg, void* impl_obj_void)
{
  SSSPMultiVertex* impl_obj = static_cast<SSSPMultiVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->verify();
}
PUPable_def(SINGLE_ARG(Closure_SSSPMultiVertex::verify_4_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void check(const CmiUInt8 &v, const double &weight);
 */

void CProxy_SSSPMultiVertex::check(const CmiUInt8 &v, const double &weight, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const double &weight
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(double &)weight;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(double &)weight;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPMultiVertex::idx_check_marshall5(),0);
}

// Entry point registration function

int CkIndex_SSSPMultiVertex::reg_check_marshall5() {
  int epidx = CkRegisterEp("check(const CmiUInt8 &v, const double &weight)",
      _call_check_marshall5, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_check_marshall5);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_check_marshall5);

  return epidx;
}


void CkIndex_SSSPMultiVertex::_call_check_marshall5(void* impl_msg, void* impl_obj_void)
{
  SSSPMultiVertex* impl_obj = static_cast<SSSPMultiVertex *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const double &weight*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  double weight; implP|weight;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->check(v, weight);
}

int CkIndex_SSSPMultiVertex::_callmarshall_check_marshall5(char* impl_buf, void* impl_obj_void) {
  SSSPMultiVertex* impl_obj = static_cast< SSSPMultiVertex *>(impl_obj_void);
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const double &weight*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  double weight; implP|weight;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->check(v, weight);
  return implP.size();
}

void CkIndex_SSSPMultiVertex::_marshallmessagepup_check_marshall5(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiUInt8 &v, const double &weight*/
  PUP::fromMem implP(impl_buf);
  CmiUInt8 v; implP|v;
  double weight; implP|weight;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("v");
  implDestP|v;
  if (implDestP.hasComments()) implDestP.comment("weight");
  implDestP|weight;
}
PUPable_def(SINGLE_ARG(Closure_SSSPMultiVertex::check_5_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void getScannedVertexNum(void);
 */

void CProxy_SSSPMultiVertex::getScannedVertexNum(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPMultiVertex::idx_getScannedVertexNum_void(),0);
}

// Entry point registration function

int CkIndex_SSSPMultiVertex::reg_getScannedVertexNum_void() {
  int epidx = CkRegisterEp("getScannedVertexNum(void)",
      _call_getScannedVertexNum_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_SSSPMultiVertex::_call_getScannedVertexNum_void(void* impl_msg, void* impl_obj_void)
{
  SSSPMultiVertex* impl_obj = static_cast<SSSPMultiVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->getScannedVertexNum();
}
PUPable_def(SINGLE_ARG(Closure_SSSPMultiVertex::getScannedVertexNum_6_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void foo(void);
 */

void CProxy_SSSPMultiVertex::foo(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPMultiVertex::idx_foo_void(),0);
}

// Entry point registration function

int CkIndex_SSSPMultiVertex::reg_foo_void() {
  int epidx = CkRegisterEp("foo(void)",
      _call_foo_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_SSSPMultiVertex::_call_foo_void(void* impl_msg, void* impl_obj_void)
{
  SSSPMultiVertex* impl_obj = static_cast<SSSPMultiVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->foo();
}
PUPable_def(SINGLE_ARG(Closure_SSSPMultiVertex::foo_7_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: SSSPMultiVertex(CkMigrateMessage* impl_msg);
 */

// Entry point registration function

int CkIndex_SSSPMultiVertex::reg_SSSPMultiVertex_CkMigrateMessage() {
  int epidx = CkRegisterEp("SSSPMultiVertex(CkMigrateMessage* impl_msg)",
      _call_SSSPMultiVertex_CkMigrateMessage, 0, __idx, 0);
  return epidx;
}


void CkIndex_SSSPMultiVertex::_call_SSSPMultiVertex_CkMigrateMessage(void* impl_msg, void* impl_obj_void)
{
  SSSPMultiVertex* impl_obj = static_cast<SSSPMultiVertex *>(impl_obj_void);
  new (impl_obj) SSSPMultiVertex((CkMigrateMessage*)impl_msg);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: SSSPMultiVertex(void);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void connectVertex(const std::pair<CmiUInt8,SSSPEdge > &edge);
 */

void CProxySection_SSSPMultiVertex::connectVertex(const std::pair<CmiUInt8,SSSPEdge > &edge, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const std::pair<CmiUInt8,SSSPEdge > &edge
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(std::pair<CmiUInt8,SSSPEdge > &)edge;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(std::pair<CmiUInt8,SSSPEdge > &)edge;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPMultiVertex::idx_connectVertex_marshall2(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void make_root(const CmiUInt8 &root);
 */

void CProxySection_SSSPMultiVertex::make_root(const CmiUInt8 &root, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const CmiUInt8 &root
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)root;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)root;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPMultiVertex::idx_make_root_marshall3(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void verify(void);
 */

void CProxySection_SSSPMultiVertex::verify(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPMultiVertex::idx_verify_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void check(const CmiUInt8 &v, const double &weight);
 */

void CProxySection_SSSPMultiVertex::check(const CmiUInt8 &v, const double &weight, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const CmiUInt8 &v, const double &weight
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(double &)weight;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(double &)weight;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPMultiVertex::idx_check_marshall5(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void getScannedVertexNum(void);
 */

void CProxySection_SSSPMultiVertex::getScannedVertexNum(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPMultiVertex::idx_getScannedVertexNum_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void foo(void);
 */

void CProxySection_SSSPMultiVertex::foo(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPMultiVertex::idx_foo_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: SSSPMultiVertex(CkMigrateMessage* impl_msg);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
void CkIndex_SSSPMultiVertex::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeArray);
  CkRegisterBase(__idx, CkIndex_ArrayElement::__idx);
  // REG: SSSPMultiVertex(void);
  idx_SSSPMultiVertex_void();
  CkRegisterDefaultCtor(__idx, idx_SSSPMultiVertex_void());

  // REG: void connectVertex(const std::pair<CmiUInt8,SSSPEdge > &edge);
  idx_connectVertex_marshall2();

  // REG: void make_root(const CmiUInt8 &root);
  idx_make_root_marshall3();

  // REG: void verify(void);
  idx_verify_void();

  // REG: void check(const CmiUInt8 &v, const double &weight);
  idx_check_marshall5();

  // REG: void getScannedVertexNum(void);
  idx_getScannedVertexNum_void();

  // REG: void foo(void);
  idx_foo_void();

  // REG: SSSPMultiVertex(CkMigrateMessage* impl_msg);
  idx_SSSPMultiVertex_CkMigrateMessage();
  CkRegisterMigCtor(__idx, idx_SSSPMultiVertex_CkMigrateMessage());

}
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: message MeshStreamerMessage<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long > >;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group MeshStreamer<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long >,SimpleMeshRouter >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
void _registertram_multivertex_sssp(void)
{
  static int _done = 0; if(_done) return; _done = 1;
  _registerGraphGenerator();

  CkRegisterReadonly("K","int",sizeof(K),(void *) &K,__xlater_roPup_K);

  CkRegisterReadonly("N","CmiUInt8",sizeof(N),(void *) &N,__xlater_roPup_N);

  CkRegisterReadonly("M","CmiUInt8",sizeof(M),(void *) &M,__xlater_roPup_M);

  CkRegisterReadonly("driverProxy","CProxy_TestDriver",sizeof(driverProxy),(void *) &driverProxy,__xlater_roPup_driverProxy);

  CkRegisterReadonly("aggregator","CProxy_ArrayMeshStreamer<dtype,long long,SSSPMultiVertex,SimpleMeshRouter >",sizeof(aggregator),(void *) &aggregator,__xlater_roPup_aggregator);

/* REG: mainchare TestDriver: Chare{
TestDriver(CkArgMsg* impl_msg);
threaded void start(void);
threaded void restart(void);
void startVerificationPhase(void);
void done(const CmiUInt8 &globalNumScannedEdges);
void startGraphConstruction(void);
void exit(void);
};
*/
  CkIndex_TestDriver::__register("TestDriver", sizeof(TestDriver));


/* REG: array SSSPMultiVertex: ArrayElement{
SSSPMultiVertex(void);
void connectVertex(const std::pair<CmiUInt8,SSSPEdge > &edge);
void make_root(const CmiUInt8 &root);
void verify(void);
void check(const CmiUInt8 &v, const double &weight);
void getScannedVertexNum(void);
void foo(void);
SSSPMultiVertex(CkMigrateMessage* impl_msg);
};
*/
  CkIndex_SSSPMultiVertex::__register("SSSPMultiVertex", sizeof(SSSPMultiVertex));

/* REG: message MeshStreamerMessage<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long > >;
*/
CMessage_MeshStreamerMessage<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long > >::__register("MeshStreamerMessage<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long > >", sizeof(MeshStreamerMessage<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long > >),(CkPackFnPtr) MeshStreamerMessage<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long > >::pack,(CkUnpackFnPtr) MeshStreamerMessage<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long > >::unpack);

/* REG: group ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter >: IrrGroup;
*/
  CkIndex_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter >::__register("ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter >", sizeof(ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter >));

/* REG: group MeshStreamer<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long >,SimpleMeshRouter >: IrrGroup;
*/
  CkIndex_MeshStreamer<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long >,SimpleMeshRouter >::__register("MeshStreamer<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long >,SimpleMeshRouter >", sizeof(MeshStreamer<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long >,SimpleMeshRouter >));

/* REG: group GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >: IrrGroup;
*/
  GraphLib::CkIndex_RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >::__register("GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >", sizeof(GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >));

/* REG: group GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >: IrrGroup;
*/
  GraphLib::CkIndex_RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >::__register("GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >", sizeof(GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >));

/* REG: group GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >: IrrGroup;
*/
  GraphLib::CkIndex_SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >::__register("GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >", sizeof(GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >));

/* REG: group GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >: IrrGroup;
*/
  GraphLib::CkIndex_SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >::__register("GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >", sizeof(GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >));

/* REG: group GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >: IrrGroup;
*/
  GraphLib::CkIndex_Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >::__register("GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >", sizeof(GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >));

/* REG: group GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >: IrrGroup;
*/
  GraphLib::CkIndex_Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >::__register("GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >", sizeof(GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >));

}
extern "C" void CkRegisterMainModule(void) {
  _registertram_multivertex_sssp();
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<TestDriver, 1>::operator()(TestDriver *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->TestDriver::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void CBase_TestDriver::virtual_pup(PUP::er &p) {
    recursive_pup<TestDriver >(dynamic_cast<TestDriver* >(this), p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<SSSPMultiVertex, 1>::operator()(SSSPMultiVertex *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->SSSPMultiVertex::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void CBase_SSSPMultiVertex::virtual_pup(PUP::er &p) {
    recursive_pup<SSSPMultiVertex >(dynamic_cast<SSSPMultiVertex* >(this), p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter >, 1>::operator()(ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<MeshStreamer<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long >,SimpleMeshRouter >, 1>::operator()(MeshStreamer<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long >,SimpleMeshRouter > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->MeshStreamer<ArrayDataItem<std::pair<CmiUInt8,SSSPEdge >,long long >,SimpleMeshRouter >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >, 1>::operator()(GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >, 1>::operator()(GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >, 1>::operator()(GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >, 1>::operator()(GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >, 1>::operator()(GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex,CProxy_ArrayMeshStreamer<std::pair<CmiUInt8,SSSPEdge >,long long,SSSPMultiVertex,SimpleMeshRouter > >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >, 1>::operator()(GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::MultiVertex >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */

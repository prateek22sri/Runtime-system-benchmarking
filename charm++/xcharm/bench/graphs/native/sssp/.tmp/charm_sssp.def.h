




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

    struct Closure_SSSPVertex::connectVertex_2_closure : public SDAG::Closure {
      SSSPEdge edge;


      connectVertex_2_closure() {
        init();
      }
      connectVertex_2_closure(CkMigrateMessage*) {
        init();
      }
      SSSPEdge & getP0() { return edge;}
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

    struct Closure_SSSPVertex::make_root_3_closure : public SDAG::Closure {
      

      make_root_3_closure() {
        init();
      }
      make_root_3_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~make_root_3_closure() {
      }
      PUPable_decl(SINGLE_ARG(make_root_3_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_SSSPVertex::update_4_closure : public SDAG::Closure {
      CmiInt8 v;
      double w;


      update_4_closure() {
        init();
      }
      update_4_closure(CkMigrateMessage*) {
        init();
      }
      CmiInt8 & getP0() { return v;}
      double & getP1() { return w;}
      void pup(PUP::er& __p) {
        __p | v;
        __p | w;
        packClosure(__p);
      }
      virtual ~update_4_closure() {
      }
      PUPable_decl(SINGLE_ARG(update_4_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_SSSPVertex::verify_5_closure : public SDAG::Closure {
      

      verify_5_closure() {
        init();
      }
      verify_5_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~verify_5_closure() {
      }
      PUPable_decl(SINGLE_ARG(verify_5_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_SSSPVertex::getScannedVertexNum_6_closure : public SDAG::Closure {
      

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

    struct Closure_SSSPVertex::check_7_closure : public SDAG::Closure {
      double w;


      check_7_closure() {
        init();
      }
      check_7_closure(CkMigrateMessage*) {
        init();
      }
      double & getP0() { return w;}
      void pup(PUP::er& __p) {
        __p | w;
        packClosure(__p);
      }
      virtual ~check_7_closure() {
      }
      PUPable_decl(SINGLE_ARG(check_7_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_SSSPVertex::print_8_closure : public SDAG::Closure {
      

      print_8_closure() {
        init();
      }
      print_8_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~print_8_closure() {
      }
      PUPable_decl(SINGLE_ARG(print_8_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_SSSPVertex::dump_edges_9_closure : public SDAG::Closure {
      

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
      PUPable_decl(SINGLE_ARG(dump_edges_9_closure));
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

/* DEFS: mainchare TestDriver: Chare{
TestDriver(CkArgMsg* impl_msg);
void start(void);
void restart(void);
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
/* DEFS: void start(void);
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
  CkFreeSysMsg(impl_msg);
  impl_obj->start();
}
PUPable_def(SINGLE_ARG(Closure_TestDriver::start_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void restart(void);
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

  // REG: void start(void);
  idx_start_void();

  // REG: void restart(void);
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


/* DEFS: array SSSPVertex: ArrayElement{
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
#ifndef CK_TEMPLATES_ONLY
 int CkIndex_SSSPVertex::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
/* DEFS: SSSPVertex(void);
 */

void CProxyElement_SSSPVertex::insert(int onPE)
{ 
  void *impl_msg = CkAllocSysMsg();
   UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
   ckInsert((CkArrayMessage *)impl_msg,CkIndex_SSSPVertex::idx_SSSPVertex_void(),onPE);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void connectVertex(const SSSPEdge &edge);
 */

void CProxyElement_SSSPVertex::connectVertex(const SSSPEdge &edge, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const SSSPEdge &edge
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(SSSPEdge &)edge;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(SSSPEdge &)edge;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_connectVertex_marshall2(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void make_root(void);
 */

void CProxyElement_SSSPVertex::make_root(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_make_root_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void update(const CmiInt8 &v, const double &w);
 */

void CProxyElement_SSSPVertex::update(const CmiInt8 &v, const double &w, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const CmiInt8 &v, const double &w
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(double &)w;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(double &)w;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_update_marshall4(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void verify(void);
 */

void CProxyElement_SSSPVertex::verify(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_verify_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void getScannedVertexNum(void);
 */

void CProxyElement_SSSPVertex::getScannedVertexNum(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_getScannedVertexNum_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void check(const double &w);
 */

void CProxyElement_SSSPVertex::check(const double &w, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const double &w
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(double &)w;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(double &)w;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_check_marshall7(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void print(void);
 */

void CProxyElement_SSSPVertex::print(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_print_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void dump_edges(void);
 */

void CProxyElement_SSSPVertex::dump_edges(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_dump_edges_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: SSSPVertex(CkMigrateMessage* impl_msg);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: SSSPVertex(void);
 */

CkArrayID CProxy_SSSPVertex::ckNew(const CkArrayOptions &opts)
{
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  return ckCreateArray((CkArrayMessage *)impl_msg, CkIndex_SSSPVertex::idx_SSSPVertex_void(), opts);
}

void CProxy_SSSPVertex::ckNew(const CkArrayOptions &opts, CkCallback _ck_array_creation_cb)
{
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  CkSendAsyncCreateArray(CkIndex_SSSPVertex::idx_SSSPVertex_void(), _ck_array_creation_cb, opts, impl_msg);
}

CkArrayID CProxy_SSSPVertex::ckNew(const int s1)
{
  void *impl_msg = CkAllocSysMsg();
  CkArrayOptions opts(s1);
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  return ckCreateArray((CkArrayMessage *)impl_msg, CkIndex_SSSPVertex::idx_SSSPVertex_void(), opts);
}

void CProxy_SSSPVertex::ckNew(const int s1, CkCallback _ck_array_creation_cb)
{
  void *impl_msg = CkAllocSysMsg();
  CkArrayOptions opts(s1);
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  CkSendAsyncCreateArray(CkIndex_SSSPVertex::idx_SSSPVertex_void(), _ck_array_creation_cb, opts, impl_msg);
}

// Entry point registration function

int CkIndex_SSSPVertex::reg_SSSPVertex_void() {
  int epidx = CkRegisterEp("SSSPVertex(void)",
      _call_SSSPVertex_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_SSSPVertex::_call_SSSPVertex_void(void* impl_msg, void* impl_obj_void)
{
  SSSPVertex* impl_obj = static_cast<SSSPVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  new (impl_obj) SSSPVertex();
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void connectVertex(const SSSPEdge &edge);
 */

void CProxy_SSSPVertex::connectVertex(const SSSPEdge &edge, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const SSSPEdge &edge
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(SSSPEdge &)edge;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(SSSPEdge &)edge;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPVertex::idx_connectVertex_marshall2(),0);
}

// Entry point registration function

int CkIndex_SSSPVertex::reg_connectVertex_marshall2() {
  int epidx = CkRegisterEp("connectVertex(const SSSPEdge &edge)",
      _call_connectVertex_marshall2, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_connectVertex_marshall2);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_connectVertex_marshall2);

  return epidx;
}


void CkIndex_SSSPVertex::_call_connectVertex_marshall2(void* impl_msg, void* impl_obj_void)
{
  SSSPVertex* impl_obj = static_cast<SSSPVertex *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const SSSPEdge &edge*/
  PUP::fromMem implP(impl_buf);
  SSSPEdge edge; implP|edge;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->connectVertex(edge);
}

int CkIndex_SSSPVertex::_callmarshall_connectVertex_marshall2(char* impl_buf, void* impl_obj_void) {
  SSSPVertex* impl_obj = static_cast< SSSPVertex *>(impl_obj_void);
  /*Unmarshall pup'd fields: const SSSPEdge &edge*/
  PUP::fromMem implP(impl_buf);
  SSSPEdge edge; implP|edge;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->connectVertex(edge);
  return implP.size();
}

void CkIndex_SSSPVertex::_marshallmessagepup_connectVertex_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const SSSPEdge &edge*/
  PUP::fromMem implP(impl_buf);
  SSSPEdge edge; implP|edge;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("edge");
  implDestP|edge;
}
PUPable_def(SINGLE_ARG(Closure_SSSPVertex::connectVertex_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void make_root(void);
 */

void CProxy_SSSPVertex::make_root(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPVertex::idx_make_root_void(),0);
}

// Entry point registration function

int CkIndex_SSSPVertex::reg_make_root_void() {
  int epidx = CkRegisterEp("make_root(void)",
      _call_make_root_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_SSSPVertex::_call_make_root_void(void* impl_msg, void* impl_obj_void)
{
  SSSPVertex* impl_obj = static_cast<SSSPVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->make_root();
}
PUPable_def(SINGLE_ARG(Closure_SSSPVertex::make_root_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void update(const CmiInt8 &v, const double &w);
 */

void CProxy_SSSPVertex::update(const CmiInt8 &v, const double &w, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const CmiInt8 &v, const double &w
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(double &)w;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(double &)w;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPVertex::idx_update_marshall4(),0);
}

// Entry point registration function

int CkIndex_SSSPVertex::reg_update_marshall4() {
  int epidx = CkRegisterEp("update(const CmiInt8 &v, const double &w)",
      _call_update_marshall4, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_update_marshall4);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_update_marshall4);

  return epidx;
}


void CkIndex_SSSPVertex::_call_update_marshall4(void* impl_msg, void* impl_obj_void)
{
  SSSPVertex* impl_obj = static_cast<SSSPVertex *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiInt8 &v, const double &w*/
  PUP::fromMem implP(impl_buf);
  CmiInt8 v; implP|v;
  double w; implP|w;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->update(v, w);
}

int CkIndex_SSSPVertex::_callmarshall_update_marshall4(char* impl_buf, void* impl_obj_void) {
  SSSPVertex* impl_obj = static_cast< SSSPVertex *>(impl_obj_void);
  /*Unmarshall pup'd fields: const CmiInt8 &v, const double &w*/
  PUP::fromMem implP(impl_buf);
  CmiInt8 v; implP|v;
  double w; implP|w;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->update(v, w);
  return implP.size();
}

void CkIndex_SSSPVertex::_marshallmessagepup_update_marshall4(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const CmiInt8 &v, const double &w*/
  PUP::fromMem implP(impl_buf);
  CmiInt8 v; implP|v;
  double w; implP|w;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("v");
  implDestP|v;
  if (implDestP.hasComments()) implDestP.comment("w");
  implDestP|w;
}
PUPable_def(SINGLE_ARG(Closure_SSSPVertex::update_4_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void verify(void);
 */

void CProxy_SSSPVertex::verify(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPVertex::idx_verify_void(),0);
}

// Entry point registration function

int CkIndex_SSSPVertex::reg_verify_void() {
  int epidx = CkRegisterEp("verify(void)",
      _call_verify_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_SSSPVertex::_call_verify_void(void* impl_msg, void* impl_obj_void)
{
  SSSPVertex* impl_obj = static_cast<SSSPVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->verify();
}
PUPable_def(SINGLE_ARG(Closure_SSSPVertex::verify_5_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void getScannedVertexNum(void);
 */

void CProxy_SSSPVertex::getScannedVertexNum(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPVertex::idx_getScannedVertexNum_void(),0);
}

// Entry point registration function

int CkIndex_SSSPVertex::reg_getScannedVertexNum_void() {
  int epidx = CkRegisterEp("getScannedVertexNum(void)",
      _call_getScannedVertexNum_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_SSSPVertex::_call_getScannedVertexNum_void(void* impl_msg, void* impl_obj_void)
{
  SSSPVertex* impl_obj = static_cast<SSSPVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->getScannedVertexNum();
}
PUPable_def(SINGLE_ARG(Closure_SSSPVertex::getScannedVertexNum_6_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void check(const double &w);
 */

void CProxy_SSSPVertex::check(const double &w, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const double &w
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(double &)w;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(double &)w;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPVertex::idx_check_marshall7(),0);
}

// Entry point registration function

int CkIndex_SSSPVertex::reg_check_marshall7() {
  int epidx = CkRegisterEp("check(const double &w)",
      _call_check_marshall7, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_check_marshall7);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_check_marshall7);

  return epidx;
}


void CkIndex_SSSPVertex::_call_check_marshall7(void* impl_msg, void* impl_obj_void)
{
  SSSPVertex* impl_obj = static_cast<SSSPVertex *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const double &w*/
  PUP::fromMem implP(impl_buf);
  double w; implP|w;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->check(w);
}

int CkIndex_SSSPVertex::_callmarshall_check_marshall7(char* impl_buf, void* impl_obj_void) {
  SSSPVertex* impl_obj = static_cast< SSSPVertex *>(impl_obj_void);
  /*Unmarshall pup'd fields: const double &w*/
  PUP::fromMem implP(impl_buf);
  double w; implP|w;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->check(w);
  return implP.size();
}

void CkIndex_SSSPVertex::_marshallmessagepup_check_marshall7(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const double &w*/
  PUP::fromMem implP(impl_buf);
  double w; implP|w;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("w");
  implDestP|w;
}
PUPable_def(SINGLE_ARG(Closure_SSSPVertex::check_7_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void print(void);
 */

void CProxy_SSSPVertex::print(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPVertex::idx_print_void(),0);
}

// Entry point registration function

int CkIndex_SSSPVertex::reg_print_void() {
  int epidx = CkRegisterEp("print(void)",
      _call_print_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_SSSPVertex::_call_print_void(void* impl_msg, void* impl_obj_void)
{
  SSSPVertex* impl_obj = static_cast<SSSPVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->print();
}
PUPable_def(SINGLE_ARG(Closure_SSSPVertex::print_8_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void dump_edges(void);
 */

void CProxy_SSSPVertex::dump_edges(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_SSSPVertex::idx_dump_edges_void(),0);
}

// Entry point registration function

int CkIndex_SSSPVertex::reg_dump_edges_void() {
  int epidx = CkRegisterEp("dump_edges(void)",
      _call_dump_edges_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_SSSPVertex::_call_dump_edges_void(void* impl_msg, void* impl_obj_void)
{
  SSSPVertex* impl_obj = static_cast<SSSPVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->dump_edges();
}
PUPable_def(SINGLE_ARG(Closure_SSSPVertex::dump_edges_9_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: SSSPVertex(CkMigrateMessage* impl_msg);
 */

// Entry point registration function

int CkIndex_SSSPVertex::reg_SSSPVertex_CkMigrateMessage() {
  int epidx = CkRegisterEp("SSSPVertex(CkMigrateMessage* impl_msg)",
      _call_SSSPVertex_CkMigrateMessage, 0, __idx, 0);
  return epidx;
}


void CkIndex_SSSPVertex::_call_SSSPVertex_CkMigrateMessage(void* impl_msg, void* impl_obj_void)
{
  SSSPVertex* impl_obj = static_cast<SSSPVertex *>(impl_obj_void);
  new (impl_obj) SSSPVertex((CkMigrateMessage*)impl_msg);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: SSSPVertex(void);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void connectVertex(const SSSPEdge &edge);
 */

void CProxySection_SSSPVertex::connectVertex(const SSSPEdge &edge, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const SSSPEdge &edge
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(SSSPEdge &)edge;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(SSSPEdge &)edge;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_connectVertex_marshall2(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void make_root(void);
 */

void CProxySection_SSSPVertex::make_root(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_make_root_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void update(const CmiInt8 &v, const double &w);
 */

void CProxySection_SSSPVertex::update(const CmiInt8 &v, const double &w, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const CmiInt8 &v, const double &w
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(CmiInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(double &)w;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(CmiInt8 &)v;
    //Have to cast away const-ness to get pup routine
    implP|(double &)w;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_update_marshall4(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void verify(void);
 */

void CProxySection_SSSPVertex::verify(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_verify_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void getScannedVertexNum(void);
 */

void CProxySection_SSSPVertex::getScannedVertexNum(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_getScannedVertexNum_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void check(const double &w);
 */

void CProxySection_SSSPVertex::check(const double &w, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const double &w
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(double &)w;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(double &)w;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_check_marshall7(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void print(void);
 */

void CProxySection_SSSPVertex::print(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_print_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void dump_edges(void);
 */

void CProxySection_SSSPVertex::dump_edges(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_SSSPVertex::idx_dump_edges_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: SSSPVertex(CkMigrateMessage* impl_msg);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
void CkIndex_SSSPVertex::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeArray);
  CkRegisterBase(__idx, CkIndex_ArrayElement::__idx);
  // REG: SSSPVertex(void);
  idx_SSSPVertex_void();
  CkRegisterDefaultCtor(__idx, idx_SSSPVertex_void());

  // REG: void connectVertex(const SSSPEdge &edge);
  idx_connectVertex_marshall2();

  // REG: void make_root(void);
  idx_make_root_void();

  // REG: void update(const CmiInt8 &v, const double &w);
  idx_update_marshall4();

  // REG: void verify(void);
  idx_verify_void();

  // REG: void getScannedVertexNum(void);
  idx_getScannedVertexNum_void();

  // REG: void check(const double &w);
  idx_check_marshall7();

  // REG: void print(void);
  idx_print_void();

  // REG: void dump_edges(void);
  idx_dump_edges_void();

  // REG: SSSPVertex(CkMigrateMessage* impl_msg);
  idx_SSSPVertex_CkMigrateMessage();
  CkRegisterMigCtor(__idx, idx_SSSPVertex_CkMigrateMessage());

}
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: message MeshStreamerMessage<ArrayDataItem<SSSPEdge,long long > >;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group MeshStreamer<ArrayDataItem<SSSPEdge,long long >,SimpleMeshRouter >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
void _registercharm_sssp(void)
{
  static int _done = 0; if(_done) return; _done = 1;
  _registerGraphGenerator();

  CkRegisterReadonly("K","int",sizeof(K),(void *) &K,__xlater_roPup_K);

  CkRegisterReadonly("N","CmiUInt8",sizeof(N),(void *) &N,__xlater_roPup_N);

  CkRegisterReadonly("M","CmiUInt8",sizeof(M),(void *) &M,__xlater_roPup_M);

  CkRegisterReadonly("driverProxy","CProxy_TestDriver",sizeof(driverProxy),(void *) &driverProxy,__xlater_roPup_driverProxy);

/* REG: mainchare TestDriver: Chare{
TestDriver(CkArgMsg* impl_msg);
void start(void);
void restart(void);
void startVerificationPhase(void);
void done(const CmiUInt8 &globalNumScannedEdges);
void startGraphConstruction(void);
void exit(void);
};
*/
  CkIndex_TestDriver::__register("TestDriver", sizeof(TestDriver));


/* REG: array SSSPVertex: ArrayElement{
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
  CkIndex_SSSPVertex::__register("SSSPVertex", sizeof(SSSPVertex));

/* REG: message MeshStreamerMessage<ArrayDataItem<SSSPEdge,long long > >;
*/
CMessage_MeshStreamerMessage<ArrayDataItem<SSSPEdge,long long > >::__register("MeshStreamerMessage<ArrayDataItem<SSSPEdge,long long > >", sizeof(MeshStreamerMessage<ArrayDataItem<SSSPEdge,long long > >),(CkPackFnPtr) MeshStreamerMessage<ArrayDataItem<SSSPEdge,long long > >::pack,(CkUnpackFnPtr) MeshStreamerMessage<ArrayDataItem<SSSPEdge,long long > >::unpack);

/* REG: group ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter >: IrrGroup;
*/
  CkIndex_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter >::__register("ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter >", sizeof(ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter >));

/* REG: group MeshStreamer<ArrayDataItem<SSSPEdge,long long >,SimpleMeshRouter >: IrrGroup;
*/
  CkIndex_MeshStreamer<ArrayDataItem<SSSPEdge,long long >,SimpleMeshRouter >::__register("MeshStreamer<ArrayDataItem<SSSPEdge,long long >,SimpleMeshRouter >", sizeof(MeshStreamer<ArrayDataItem<SSSPEdge,long long >,SimpleMeshRouter >));

/* REG: group GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >: IrrGroup;
*/
  GraphLib::CkIndex_RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >::__register("GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >", sizeof(GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >));

/* REG: group GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >: IrrGroup;
*/
  GraphLib::CkIndex_RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >::__register("GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >", sizeof(GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >));

/* REG: group GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >: IrrGroup;
*/
  GraphLib::CkIndex_SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >::__register("GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >", sizeof(GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >));

/* REG: group GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >: IrrGroup;
*/
  GraphLib::CkIndex_SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >::__register("GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >", sizeof(GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >));

/* REG: group GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >: IrrGroup;
*/
  GraphLib::CkIndex_Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >::__register("GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >", sizeof(GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >));

/* REG: group GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >: IrrGroup;
*/
  GraphLib::CkIndex_Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >::__register("GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >", sizeof(GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >));

}
extern "C" void CkRegisterMainModule(void) {
  _registercharm_sssp();
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
void recursive_pup_impl<SSSPVertex, 1>::operator()(SSSPVertex *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->SSSPVertex::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void CBase_SSSPVertex::virtual_pup(PUP::er &p) {
    recursive_pup<SSSPVertex >(dynamic_cast<SSSPVertex* >(this), p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter >, 1>::operator()(ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<MeshStreamer<ArrayDataItem<SSSPEdge,long long >,SimpleMeshRouter >, 1>::operator()(MeshStreamer<ArrayDataItem<SSSPEdge,long long >,SimpleMeshRouter > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->MeshStreamer<ArrayDataItem<SSSPEdge,long long >,SimpleMeshRouter >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >, 1>::operator()(GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::RMAT_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >, 1>::operator()(GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::RMAT_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >, 1>::operator()(GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::SSCA_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >, 1>::operator()(GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::SSCA_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >, 1>::operator()(GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::Random_Generator_Charm<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >, 1>::operator()(GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::Random_Generator_Tram<SSSPGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<SSSPEdge,long long,SSSPVertex,SimpleMeshRouter > >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */

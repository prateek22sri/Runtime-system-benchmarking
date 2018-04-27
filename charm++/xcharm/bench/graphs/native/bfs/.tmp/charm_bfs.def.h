




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

    struct Closure_BFSVertex::connectVertex_2_closure : public SDAG::Closure {
      BFSEdge edge;


      connectVertex_2_closure() {
        init();
      }
      connectVertex_2_closure(CkMigrateMessage*) {
        init();
      }
      BFSEdge & getP0() { return edge;}
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

    struct Closure_BFSVertex::make_root_3_closure : public SDAG::Closure {
      

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

    struct Closure_BFSVertex::update_4_closure : public SDAG::Closure {
      int level;
      CmiUInt8 parent;


      update_4_closure() {
        init();
      }
      update_4_closure(CkMigrateMessage*) {
        init();
      }
      int & getP0() { return level;}
      CmiUInt8 & getP1() { return parent;}
      void pup(PUP::er& __p) {
        __p | level;
        __p | parent;
        packClosure(__p);
      }
      virtual ~update_4_closure() {
      }
      PUPable_decl(SINGLE_ARG(update_4_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_BFSVertex::getScannedVertexNum_5_closure : public SDAG::Closure {
      

      getScannedVertexNum_5_closure() {
        init();
      }
      getScannedVertexNum_5_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~getScannedVertexNum_5_closure() {
      }
      PUPable_decl(SINGLE_ARG(getScannedVertexNum_5_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_BFSVertex::verify_6_closure : public SDAG::Closure {
      

      verify_6_closure() {
        init();
      }
      verify_6_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~verify_6_closure() {
      }
      PUPable_decl(SINGLE_ARG(verify_6_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_BFSVertex::check_7_closure : public SDAG::Closure {
      int level;


      check_7_closure() {
        init();
      }
      check_7_closure(CkMigrateMessage*) {
        init();
      }
      int & getP0() { return level;}
      void pup(PUP::er& __p) {
        __p | level;
        packClosure(__p);
      }
      virtual ~check_7_closure() {
      }
      PUPable_decl(SINGLE_ARG(check_7_closure));
    };
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY

    struct Closure_BFSVertex::dump_edges_8_closure : public SDAG::Closure {
      

      dump_edges_8_closure() {
        init();
      }
      dump_edges_8_closure(CkMigrateMessage*) {
        init();
      }
            void pup(PUP::er& __p) {
        packClosure(__p);
      }
      virtual ~dump_edges_8_closure() {
      }
      PUPable_decl(SINGLE_ARG(dump_edges_8_closure));
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


/* DEFS: array BFSVertex: ArrayElement{
BFSVertex(void);
void connectVertex(const BFSEdge &edge);
void make_root(void);
void update(int level, const CmiUInt8 &parent);
void getScannedVertexNum(void);
void verify(void);
void check(int level);
void dump_edges(void);
BFSVertex(CkMigrateMessage* impl_msg);
};
 */
#ifndef CK_TEMPLATES_ONLY
 int CkIndex_BFSVertex::__idx=0;
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
/* DEFS: BFSVertex(void);
 */

void CProxyElement_BFSVertex::insert(int onPE)
{ 
  void *impl_msg = CkAllocSysMsg();
   UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
   ckInsert((CkArrayMessage *)impl_msg,CkIndex_BFSVertex::idx_BFSVertex_void(),onPE);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void connectVertex(const BFSEdge &edge);
 */

void CProxyElement_BFSVertex::connectVertex(const BFSEdge &edge, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const BFSEdge &edge
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(BFSEdge &)edge;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(BFSEdge &)edge;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_connectVertex_marshall2(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void make_root(void);
 */

void CProxyElement_BFSVertex::make_root(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_make_root_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void update(int level, const CmiUInt8 &parent);
 */

void CProxyElement_BFSVertex::update(int level, const CmiUInt8 &parent, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: int level, const CmiUInt8 &parent
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|level;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)parent;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|level;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)parent;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_update_marshall4(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void getScannedVertexNum(void);
 */

void CProxyElement_BFSVertex::getScannedVertexNum(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_getScannedVertexNum_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void verify(void);
 */

void CProxyElement_BFSVertex::verify(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_verify_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void check(int level);
 */

void CProxyElement_BFSVertex::check(int level, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: int level
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|level;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|level;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_check_marshall7(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void dump_edges(void);
 */

void CProxyElement_BFSVertex::dump_edges(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_dump_edges_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: BFSVertex(CkMigrateMessage* impl_msg);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: BFSVertex(void);
 */

CkArrayID CProxy_BFSVertex::ckNew(const CkArrayOptions &opts)
{
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  return ckCreateArray((CkArrayMessage *)impl_msg, CkIndex_BFSVertex::idx_BFSVertex_void(), opts);
}

void CProxy_BFSVertex::ckNew(const CkArrayOptions &opts, CkCallback _ck_array_creation_cb)
{
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  CkSendAsyncCreateArray(CkIndex_BFSVertex::idx_BFSVertex_void(), _ck_array_creation_cb, opts, impl_msg);
}

CkArrayID CProxy_BFSVertex::ckNew(const int s1)
{
  void *impl_msg = CkAllocSysMsg();
  CkArrayOptions opts(s1);
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  return ckCreateArray((CkArrayMessage *)impl_msg, CkIndex_BFSVertex::idx_BFSVertex_void(), opts);
}

void CProxy_BFSVertex::ckNew(const int s1, CkCallback _ck_array_creation_cb)
{
  void *impl_msg = CkAllocSysMsg();
  CkArrayOptions opts(s1);
  UsrToEnv(impl_msg)->setMsgtype(ArrayEltInitMsg);
  CkSendAsyncCreateArray(CkIndex_BFSVertex::idx_BFSVertex_void(), _ck_array_creation_cb, opts, impl_msg);
}

// Entry point registration function

int CkIndex_BFSVertex::reg_BFSVertex_void() {
  int epidx = CkRegisterEp("BFSVertex(void)",
      _call_BFSVertex_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_BFSVertex::_call_BFSVertex_void(void* impl_msg, void* impl_obj_void)
{
  BFSVertex* impl_obj = static_cast<BFSVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  new (impl_obj) BFSVertex();
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void connectVertex(const BFSEdge &edge);
 */

void CProxy_BFSVertex::connectVertex(const BFSEdge &edge, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const BFSEdge &edge
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(BFSEdge &)edge;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(BFSEdge &)edge;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_BFSVertex::idx_connectVertex_marshall2(),0);
}

// Entry point registration function

int CkIndex_BFSVertex::reg_connectVertex_marshall2() {
  int epidx = CkRegisterEp("connectVertex(const BFSEdge &edge)",
      _call_connectVertex_marshall2, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_connectVertex_marshall2);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_connectVertex_marshall2);

  return epidx;
}


void CkIndex_BFSVertex::_call_connectVertex_marshall2(void* impl_msg, void* impl_obj_void)
{
  BFSVertex* impl_obj = static_cast<BFSVertex *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const BFSEdge &edge*/
  PUP::fromMem implP(impl_buf);
  BFSEdge edge; implP|edge;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->connectVertex(edge);
}

int CkIndex_BFSVertex::_callmarshall_connectVertex_marshall2(char* impl_buf, void* impl_obj_void) {
  BFSVertex* impl_obj = static_cast< BFSVertex *>(impl_obj_void);
  /*Unmarshall pup'd fields: const BFSEdge &edge*/
  PUP::fromMem implP(impl_buf);
  BFSEdge edge; implP|edge;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->connectVertex(edge);
  return implP.size();
}

void CkIndex_BFSVertex::_marshallmessagepup_connectVertex_marshall2(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: const BFSEdge &edge*/
  PUP::fromMem implP(impl_buf);
  BFSEdge edge; implP|edge;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("edge");
  implDestP|edge;
}
PUPable_def(SINGLE_ARG(Closure_BFSVertex::connectVertex_2_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void make_root(void);
 */

void CProxy_BFSVertex::make_root(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_BFSVertex::idx_make_root_void(),0);
}

// Entry point registration function

int CkIndex_BFSVertex::reg_make_root_void() {
  int epidx = CkRegisterEp("make_root(void)",
      _call_make_root_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_BFSVertex::_call_make_root_void(void* impl_msg, void* impl_obj_void)
{
  BFSVertex* impl_obj = static_cast<BFSVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->make_root();
}
PUPable_def(SINGLE_ARG(Closure_BFSVertex::make_root_3_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void update(int level, const CmiUInt8 &parent);
 */

void CProxy_BFSVertex::update(int level, const CmiUInt8 &parent, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: int level, const CmiUInt8 &parent
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|level;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)parent;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|level;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)parent;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_BFSVertex::idx_update_marshall4(),0);
}

// Entry point registration function

int CkIndex_BFSVertex::reg_update_marshall4() {
  int epidx = CkRegisterEp("update(int level, const CmiUInt8 &parent)",
      _call_update_marshall4, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_update_marshall4);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_update_marshall4);

  return epidx;
}


void CkIndex_BFSVertex::_call_update_marshall4(void* impl_msg, void* impl_obj_void)
{
  BFSVertex* impl_obj = static_cast<BFSVertex *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: int level, const CmiUInt8 &parent*/
  PUP::fromMem implP(impl_buf);
  int level; implP|level;
  CmiUInt8 parent; implP|parent;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->update(level, parent);
}

int CkIndex_BFSVertex::_callmarshall_update_marshall4(char* impl_buf, void* impl_obj_void) {
  BFSVertex* impl_obj = static_cast< BFSVertex *>(impl_obj_void);
  /*Unmarshall pup'd fields: int level, const CmiUInt8 &parent*/
  PUP::fromMem implP(impl_buf);
  int level; implP|level;
  CmiUInt8 parent; implP|parent;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->update(level, parent);
  return implP.size();
}

void CkIndex_BFSVertex::_marshallmessagepup_update_marshall4(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: int level, const CmiUInt8 &parent*/
  PUP::fromMem implP(impl_buf);
  int level; implP|level;
  CmiUInt8 parent; implP|parent;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("level");
  implDestP|level;
  if (implDestP.hasComments()) implDestP.comment("parent");
  implDestP|parent;
}
PUPable_def(SINGLE_ARG(Closure_BFSVertex::update_4_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void getScannedVertexNum(void);
 */

void CProxy_BFSVertex::getScannedVertexNum(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_BFSVertex::idx_getScannedVertexNum_void(),0);
}

// Entry point registration function

int CkIndex_BFSVertex::reg_getScannedVertexNum_void() {
  int epidx = CkRegisterEp("getScannedVertexNum(void)",
      _call_getScannedVertexNum_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_BFSVertex::_call_getScannedVertexNum_void(void* impl_msg, void* impl_obj_void)
{
  BFSVertex* impl_obj = static_cast<BFSVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->getScannedVertexNum();
}
PUPable_def(SINGLE_ARG(Closure_BFSVertex::getScannedVertexNum_5_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void verify(void);
 */

void CProxy_BFSVertex::verify(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_BFSVertex::idx_verify_void(),0);
}

// Entry point registration function

int CkIndex_BFSVertex::reg_verify_void() {
  int epidx = CkRegisterEp("verify(void)",
      _call_verify_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_BFSVertex::_call_verify_void(void* impl_msg, void* impl_obj_void)
{
  BFSVertex* impl_obj = static_cast<BFSVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->verify();
}
PUPable_def(SINGLE_ARG(Closure_BFSVertex::verify_6_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void check(int level);
 */

void CProxy_BFSVertex::check(int level, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: int level
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|level;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|level;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_BFSVertex::idx_check_marshall7(),0);
}

// Entry point registration function

int CkIndex_BFSVertex::reg_check_marshall7() {
  int epidx = CkRegisterEp("check(int level)",
      _call_check_marshall7, CkMarshallMsg::__idx, __idx, 0+CK_EP_NOKEEP);
  CkRegisterMarshallUnpackFn(epidx, _callmarshall_check_marshall7);
  CkRegisterMessagePupFn(epidx, _marshallmessagepup_check_marshall7);

  return epidx;
}


void CkIndex_BFSVertex::_call_check_marshall7(void* impl_msg, void* impl_obj_void)
{
  BFSVertex* impl_obj = static_cast<BFSVertex *>(impl_obj_void);
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: int level*/
  PUP::fromMem implP(impl_buf);
  int level; implP|level;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->check(level);
}

int CkIndex_BFSVertex::_callmarshall_check_marshall7(char* impl_buf, void* impl_obj_void) {
  BFSVertex* impl_obj = static_cast< BFSVertex *>(impl_obj_void);
  /*Unmarshall pup'd fields: int level*/
  PUP::fromMem implP(impl_buf);
  int level; implP|level;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  impl_obj->check(level);
  return implP.size();
}

void CkIndex_BFSVertex::_marshallmessagepup_check_marshall7(PUP::er &implDestP,void *impl_msg) {
  CkMarshallMsg *impl_msg_typed=(CkMarshallMsg *)impl_msg;
  char *impl_buf=impl_msg_typed->msgBuf;
  /*Unmarshall pup'd fields: int level*/
  PUP::fromMem implP(impl_buf);
  int level; implP|level;
  impl_buf+=CK_ALIGN(implP.size(),16);
  /*Unmarshall arrays:*/
  if (implDestP.hasComments()) implDestP.comment("level");
  implDestP|level;
}
PUPable_def(SINGLE_ARG(Closure_BFSVertex::check_7_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void dump_edges(void);
 */

void CProxy_BFSVertex::dump_edges(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckBroadcast(impl_amsg, CkIndex_BFSVertex::idx_dump_edges_void(),0);
}

// Entry point registration function

int CkIndex_BFSVertex::reg_dump_edges_void() {
  int epidx = CkRegisterEp("dump_edges(void)",
      _call_dump_edges_void, 0, __idx, 0);
  return epidx;
}


void CkIndex_BFSVertex::_call_dump_edges_void(void* impl_msg, void* impl_obj_void)
{
  BFSVertex* impl_obj = static_cast<BFSVertex *>(impl_obj_void);
  CkFreeSysMsg(impl_msg);
  impl_obj->dump_edges();
}
PUPable_def(SINGLE_ARG(Closure_BFSVertex::dump_edges_8_closure));
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: BFSVertex(CkMigrateMessage* impl_msg);
 */

// Entry point registration function

int CkIndex_BFSVertex::reg_BFSVertex_CkMigrateMessage() {
  int epidx = CkRegisterEp("BFSVertex(CkMigrateMessage* impl_msg)",
      _call_BFSVertex_CkMigrateMessage, 0, __idx, 0);
  return epidx;
}


void CkIndex_BFSVertex::_call_BFSVertex_CkMigrateMessage(void* impl_msg, void* impl_obj_void)
{
  BFSVertex* impl_obj = static_cast<BFSVertex *>(impl_obj_void);
  new (impl_obj) BFSVertex((CkMigrateMessage*)impl_msg);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: BFSVertex(void);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void connectVertex(const BFSEdge &edge);
 */

void CProxySection_BFSVertex::connectVertex(const BFSEdge &edge, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: const BFSEdge &edge
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    //Have to cast away const-ness to get pup routine
    implP|(BFSEdge &)edge;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    //Have to cast away const-ness to get pup routine
    implP|(BFSEdge &)edge;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_connectVertex_marshall2(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void make_root(void);
 */

void CProxySection_BFSVertex::make_root(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_make_root_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void update(int level, const CmiUInt8 &parent);
 */

void CProxySection_BFSVertex::update(int level, const CmiUInt8 &parent, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: int level, const CmiUInt8 &parent
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|level;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)parent;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|level;
    //Have to cast away const-ness to get pup routine
    implP|(CmiUInt8 &)parent;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_update_marshall4(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void getScannedVertexNum(void);
 */

void CProxySection_BFSVertex::getScannedVertexNum(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_getScannedVertexNum_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void verify(void);
 */

void CProxySection_BFSVertex::verify(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_verify_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void check(int level);
 */

void CProxySection_BFSVertex::check(int level, const CkEntryOptions *impl_e_opts) 
{
  ckCheck();
  //Marshall: int level
  int impl_off=0;
  { //Find the size of the PUP'd data
    PUP::sizer implP;
    implP|level;
    impl_off+=implP.size();
  }
  CkMarshallMsg *impl_msg=CkAllocateMarshallMsg(impl_off,impl_e_opts);
  { //Copy over the PUP'd data
    PUP::toMem implP((void *)impl_msg->msgBuf);
    implP|level;
  }
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_check_marshall7(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: void dump_edges(void);
 */

void CProxySection_BFSVertex::dump_edges(void) 
{
  ckCheck();
  void *impl_msg = CkAllocSysMsg();
  UsrToEnv(impl_msg)->setMsgtype(ForArrayEltMsg);
  CkArrayMessage *impl_amsg=(CkArrayMessage *)impl_msg;
  impl_amsg->array_setIfNotThere(CkArray_IfNotThere_buffer);
  ckSend(impl_amsg, CkIndex_BFSVertex::idx_dump_edges_void(),0);
}
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
/* DEFS: BFSVertex(CkMigrateMessage* impl_msg);
 */
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
void CkIndex_BFSVertex::__register(const char *s, size_t size) {
  __idx = CkRegisterChare(s, size, TypeArray);
  CkRegisterBase(__idx, CkIndex_ArrayElement::__idx);
  // REG: BFSVertex(void);
  idx_BFSVertex_void();
  CkRegisterDefaultCtor(__idx, idx_BFSVertex_void());

  // REG: void connectVertex(const BFSEdge &edge);
  idx_connectVertex_marshall2();

  // REG: void make_root(void);
  idx_make_root_void();

  // REG: void update(int level, const CmiUInt8 &parent);
  idx_update_marshall4();

  // REG: void getScannedVertexNum(void);
  idx_getScannedVertexNum_void();

  // REG: void verify(void);
  idx_verify_void();

  // REG: void check(int level);
  idx_check_marshall7();

  // REG: void dump_edges(void);
  idx_dump_edges_void();

  // REG: BFSVertex(CkMigrateMessage* impl_msg);
  idx_BFSVertex_CkMigrateMessage();
  CkRegisterMigCtor(__idx, idx_BFSVertex_CkMigrateMessage());

}
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: message MeshStreamerMessage<ArrayDataItem<BFSEdge,long long > >;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group MeshStreamer<ArrayDataItem<BFSEdge,long long >,SimpleMeshRouter >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::RMAT_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::RMAT_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::SSCA_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::SSCA_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::Random_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

/* DEFS: group GraphLib::Random_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >: IrrGroup;
 */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
#endif /* CK_TEMPLATES_ONLY */

#ifndef CK_TEMPLATES_ONLY
void _registercharm_bfs(void)
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


/* REG: array BFSVertex: ArrayElement{
BFSVertex(void);
void connectVertex(const BFSEdge &edge);
void make_root(void);
void update(int level, const CmiUInt8 &parent);
void getScannedVertexNum(void);
void verify(void);
void check(int level);
void dump_edges(void);
BFSVertex(CkMigrateMessage* impl_msg);
};
*/
  CkIndex_BFSVertex::__register("BFSVertex", sizeof(BFSVertex));

/* REG: message MeshStreamerMessage<ArrayDataItem<BFSEdge,long long > >;
*/
CMessage_MeshStreamerMessage<ArrayDataItem<BFSEdge,long long > >::__register("MeshStreamerMessage<ArrayDataItem<BFSEdge,long long > >", sizeof(MeshStreamerMessage<ArrayDataItem<BFSEdge,long long > >),(CkPackFnPtr) MeshStreamerMessage<ArrayDataItem<BFSEdge,long long > >::pack,(CkUnpackFnPtr) MeshStreamerMessage<ArrayDataItem<BFSEdge,long long > >::unpack);

/* REG: group ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter >: IrrGroup;
*/
  CkIndex_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter >::__register("ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter >", sizeof(ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter >));

/* REG: group MeshStreamer<ArrayDataItem<BFSEdge,long long >,SimpleMeshRouter >: IrrGroup;
*/
  CkIndex_MeshStreamer<ArrayDataItem<BFSEdge,long long >,SimpleMeshRouter >::__register("MeshStreamer<ArrayDataItem<BFSEdge,long long >,SimpleMeshRouter >", sizeof(MeshStreamer<ArrayDataItem<BFSEdge,long long >,SimpleMeshRouter >));

/* REG: group GraphLib::RMAT_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >: IrrGroup;
*/
  GraphLib::CkIndex_RMAT_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >::__register("GraphLib::RMAT_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >", sizeof(GraphLib::RMAT_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >));

/* REG: group GraphLib::RMAT_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >: IrrGroup;
*/
  GraphLib::CkIndex_RMAT_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >::__register("GraphLib::RMAT_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >", sizeof(GraphLib::RMAT_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >));

/* REG: group GraphLib::SSCA_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >: IrrGroup;
*/
  GraphLib::CkIndex_SSCA_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >::__register("GraphLib::SSCA_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >", sizeof(GraphLib::SSCA_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >));

/* REG: group GraphLib::SSCA_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >: IrrGroup;
*/
  GraphLib::CkIndex_SSCA_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >::__register("GraphLib::SSCA_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >", sizeof(GraphLib::SSCA_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >));

/* REG: group GraphLib::Random_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >: IrrGroup;
*/
  GraphLib::CkIndex_Random_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >::__register("GraphLib::Random_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >", sizeof(GraphLib::Random_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >));

/* REG: group GraphLib::Random_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >: IrrGroup;
*/
  GraphLib::CkIndex_Random_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >::__register("GraphLib::Random_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >", sizeof(GraphLib::Random_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >));

}
extern "C" void CkRegisterMainModule(void) {
  _registercharm_bfs();
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
void recursive_pup_impl<BFSVertex, 1>::operator()(BFSVertex *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->BFSVertex::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void CBase_BFSVertex::virtual_pup(PUP::er &p) {
    recursive_pup<BFSVertex >(dynamic_cast<BFSVertex* >(this), p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter >, 1>::operator()(ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<MeshStreamer<ArrayDataItem<BFSEdge,long long >,SimpleMeshRouter >, 1>::operator()(MeshStreamer<ArrayDataItem<BFSEdge,long long >,SimpleMeshRouter > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->MeshStreamer<ArrayDataItem<BFSEdge,long long >,SimpleMeshRouter >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::RMAT_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >, 1>::operator()(GraphLib::RMAT_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::RMAT_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::RMAT_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >, 1>::operator()(GraphLib::RMAT_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::RMAT_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::SSCA_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >, 1>::operator()(GraphLib::SSCA_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::SSCA_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::SSCA_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >, 1>::operator()(GraphLib::SSCA_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::SSCA_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::Random_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >, 1>::operator()(GraphLib::Random_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::Random_Generator_Charm<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */
#ifndef CK_TEMPLATES_ONLY
template <>
void recursive_pup_impl<GraphLib::Random_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >, 1>::operator()(GraphLib::Random_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > > *obj, PUP::er &p) {
    obj->parent_pup(p);
    obj->GraphLib::Random_Generator_Tram<BFSGraph,Options,GraphLib::VertexMapping::SingleVertex,CProxy_ArrayMeshStreamer<BFSEdge,long long,BFSVertex,SimpleMeshRouter > >::pup(p);
}
#endif /* CK_TEMPLATES_ONLY */

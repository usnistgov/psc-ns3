/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service. You may
 * use, copy and distribute copies of the software in any medium, provided that
 * you keep intact this entire notice. You may improve, modify and create
 * derivative works of the software or any portion of the software, and you may
 * copy and distribute such modifications or works. Modified works should carry
 * a notice stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the National
 * Institute of Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
 * NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
 * DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

#include <ns3/callback.h>
#include <ns3/core-module.h>
#include <ns3/mcptt-off-network-floor-participant.h>
#include <ns3/mcptt-floor-msg.h>
#include "mcptt-test-case.h"
#include "mcptt-test-case-config.h"
#include "mcptt-msg-dropper.h"
#include <ns3/mcptt-ptt-app.h>
#include <ns3/mcptt-pusher.h>
#include <ns3/mcptt-queued-user-info.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttFloorControlTest");

namespace psc {
namespace tests {

class TestFloorMachine : public McpttOffNetworkFloorParticipant
{
public:
  static TypeId GetTypeId (void);
  TestFloorMachine (void);
  virtual ~TestFloorMachine (void);
  virtual void AddDropper (Ptr<McpttMsgDropper>  dropper);
  virtual void ChangeState (Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void ExpiryOfT201 (void);
  virtual void ExpiryOfT203 (void);
  virtual void ExpiryOfT205 (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Receive (const McpttFloorMsg& msg);
  virtual void Receive (const McpttMediaMsg& msg);
  virtual void Send (const McpttFloorMsg& msg);
  virtual void Start (void);

protected:
  virtual void DoDispose (void);

private:
  std::vector<Ptr<McpttMsgDropper> >* m_droppers;
  Callback<void, const TestFloorMachine&, const McpttMsg&> m_postRxCb;
  Callback<void, const TestFloorMachine&, Ptr<McpttTimer> > m_postTimerExpCb;
  Callback<void, const TestFloorMachine&, const McpttMsg&> m_postTxCb;
  Callback<void, const TestFloorMachine&, const McpttMsg&> m_preRxCb;
  Callback<void, const TestFloorMachine&, Ptr<McpttTimer> > m_preTimerExpCb;
  Callback<void, const TestFloorMachine&, const McpttMsg&> m_preTxCb;
  Ptr<McpttOffNetworkFloorParticipantState> m_startState;
  Callback<void, const TestFloorMachine&, Ptr<McpttOffNetworkFloorParticipantState>, Ptr<McpttOffNetworkFloorParticipantState> > m_stateChangeCb;

protected:
  virtual std::vector<Ptr<McpttMsgDropper> >* GetDroppers (void) const;
  virtual void SetDroppers (std::vector<Ptr<McpttMsgDropper> >* const& droppers);

public:
  virtual Callback<void, const TestFloorMachine&, const McpttMsg&> GetPostRxCb (void) const;
  virtual Callback<void, const TestFloorMachine&, Ptr<McpttTimer> > GetPostTimerExpCb (void) const;
  virtual Callback<void, const TestFloorMachine&, const McpttMsg&> GetPostTxCb (void) const;
  virtual Callback<void, const TestFloorMachine&, const McpttMsg&> GetPreRxCb (void) const;
  virtual Callback<void, const TestFloorMachine&, Ptr<McpttTimer> > GetPreTimerExpCb (void) const;
  virtual Callback<void, const TestFloorMachine&, const McpttMsg&> GetPreTxCb (void) const;
  virtual Ptr<McpttOffNetworkFloorParticipantState> GetStartState (void) const;
  virtual Callback<void, const TestFloorMachine&, Ptr<McpttOffNetworkFloorParticipantState>, Ptr<McpttOffNetworkFloorParticipantState> > GetStateChangeCb (void) const;
  virtual void SetPostRxCb (const Callback<void, const TestFloorMachine&, const McpttMsg&>  postRxCb);
  virtual void SetPostTimerExpCb (const Callback<void, const TestFloorMachine&, Ptr<McpttTimer> >  timerExpCb);
  virtual void SetPostTxCb (const Callback<void, const TestFloorMachine&, const McpttMsg&>  postTxCb);
  virtual void SetPreRxCb (const Callback<void, const TestFloorMachine&, const McpttMsg&>  preRxCb);
  virtual void SetPreTimerExpCb (const Callback<void, const TestFloorMachine&, Ptr<McpttTimer> >  timerExpCb);
  virtual void SetPreTxCb (const Callback<void, const TestFloorMachine&, const McpttMsg&>  preTxCb);
  virtual void SetStartState (Ptr<McpttOffNetworkFloorParticipantState>  startState);
  virtual void SetStateChangeTestCb (const Callback<void, const TestFloorMachine&, Ptr<McpttOffNetworkFloorParticipantState>, Ptr<McpttOffNetworkFloorParticipantState> >  stateChangeCb);
};

/**
 * Test case with one UE sending a floor request.
 */
class McpttFloorRequestIdle : public McpttTestCase
{
public:
  McpttFloorRequestIdle (const std::string& name = "Floor Request - Idle", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void IncrementReqCount (const uint32_t& amount = 1);
  virtual void IncrementTakenCount (const uint32_t& amount = 1);
  virtual void RxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void StateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void TxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);

private:
  bool m_pendReqToHasPerm;
  uint32_t m_reqCount;
  bool m_startStopToPendReq;
  uint32_t m_takenCount;
  bool m_ueBRxReq;
  bool m_ueBSilenceToNoPerm;
  bool m_ueBRxTaken;
  bool m_ueCRxReq;
  bool m_ueCSilenceToNoPerm;
  bool m_ueCRxTaken;
};

/**
 * UE A request floor while UE B already has the floor.
 */
class McpttFloorRequestDenied : public McpttTestCase
{
public:
  McpttFloorRequestDenied (const std::string& name = "Floor Request - Denied", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg);

private:
  bool m_hasNoPermToPendReq;
  bool m_pendReqToNoPerm;
  bool m_rxOfFloorDeny;
  bool m_rxOfFloorReq;
  bool m_txOfFloorDeny;
  bool m_txOfFloorReq;
  bool m_ueCRxOfFloorDeny;
  bool m_ueCRxOfFloorReq;
};

/**
 * UE A request floor with higher priority than that of UE B, who currently has
 * the floor, and becomes arbitrator.
 */
class McpttFloorRequestPreemptive : public McpttTestCase
{
public:
  McpttFloorRequestPreemptive (const std::string& name = "Floor Request - Preemptive", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBPreRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg);

private:
  bool m_ueANoPermToPendReq;
  bool m_ueAPendReqToHasPerm;
  bool m_ueARxRtp;
  bool m_ueATxReq;
  bool m_ueARxGrant;
  bool m_ueBHasPermToPendGrant;
  bool m_ueBPendGrantToNoPerm;
  bool m_ueBRxReq;
  bool m_ueBTxGrant;
  bool m_ueBRxRtp;
  bool m_ueCRxGrant;
  bool m_ueCRxReq;
  bool m_ueCRxRtp;
};

/**
 * UE A request floor while UE B has floor with queueing enabled.
 */
class McpttFloorRequestQueued : public McpttTestCase
{
public:
  McpttFloorRequestQueued (const std::string& name = "Floor Request - Queued", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeATimerExpCb (const TestFloorMachine& machine, Ptr<McpttTimer>  timer);
  virtual void UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeBTimerExpCb (const TestFloorMachine& machine, Ptr<McpttTimer>  timer);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg);

private:
  bool m_ueANoPermToPendReq;
  bool m_ueAPendReqToQueued;
  bool m_ueAQueuedToHasPerm;
  bool m_ueARxGrant;
  bool m_ueARxInfo;
  bool m_ueARxRtp;
  bool m_ueATxReq;
  uint32_t m_ueAT201ExpCt;
  bool m_ueBHasPermToPendGrant;
  bool m_ueBPendGrantToNoPerm;
  bool m_ueBRxReq;
  bool m_ueBRxRtp;
  bool m_ueBTxInfo;
  bool m_ueBTxGrant;
  uint32_t m_ueBT205ExpCt;
  bool m_ueCRxGrant;
  bool m_ueCRxInfo;
  bool m_ueCRxReq;
  bool m_ueCRxRtp;
};

// Floor control initialization with originating user.
class McpttSessionInitNormal : public McpttTestCase
{
public:
  McpttSessionInitNormal (const std::string& name = "Session Initialization - Normal", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg);

private:
  bool m_ueATxGrant;
  bool m_ueAStartStopToHasPerm;
  bool m_ueBRxGrant;
  bool m_ueBStartStopToSilence;
  bool m_ueBSilenceToNoPerm;
  bool m_ueCRxGrant;
  bool m_ueCStartStopToSilence;
  bool m_ueCSilenceToNoPerm;
};

// Floor control initialization with originating user and loss of grant message.
class McpttSessionInitLoss : public McpttTestCase
{
public:
  McpttSessionInitLoss (const std::string& name = "Session Initialization - Message Lost", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg);

private:
  bool m_ueATxGrant;
  bool m_ueAStartStopToHasPerm;
  bool m_ueBRxRtp;
  bool m_ueBStartStopToSilence;
  bool m_ueBSilenceToNoPerm;
  bool m_ueCRxRtp;
  bool m_ueCStartStopToSilence;
  bool m_ueCSilenceToNoPerm;
};

// Floor release
class McpttFloorReleaseNormal : public McpttTestCase
{
public:
  McpttFloorReleaseNormal (const std::string& name = "Floor Release - Normal", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);

private:
  bool m_ueATxRelease;
  bool m_ueAHasPermToSilence;
  bool m_ueBRxRelease;
  bool m_ueBNoPermToSilence;
};

// Floor control release with support of queuing.
class McpttFloorReleaseAfterReq : public McpttTestCase
{
public:
  McpttFloorReleaseAfterReq (const std::string& name = "Floor Release - After Request", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg);

private:
  bool m_ueATxRelease;
  bool m_ueARxRtp;
  bool m_ueAPendReqToSilence;
  bool m_ueASilenceToNoPerm;
  bool m_ueBRxRelease;
  bool m_ueCRxRelease;
  bool m_ueCRxRtp;
};

class McpttFloorReleaseDuringGrantWhileQueued : public McpttTestCase
{
public:
  McpttFloorReleaseDuringGrantWhileQueued (const std::string& name = "Floor Release - During Grant While Queued", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeATimerExpCb (const TestFloorMachine& machine, Ptr<McpttTimer>  timer);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeBTimerExpCb (const TestFloorMachine& machine, Ptr<McpttTimer>  timer);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCTimerExpCb (const TestFloorMachine& machine, Ptr<McpttTimer>  timer);

private:
  bool m_ueAExpT203;
  bool m_ueANoPermToSilence;
  bool m_ueAQueuedToNoPerm;
  uint32_t m_ueARxGrant;
  bool m_ueATxRelease;
  uint32_t m_ueBExpT205;
  bool m_ueBHasPermToPendGrant;
  bool m_ueBPendGrantToSilence;
  bool m_ueBRxRelease;
  uint32_t m_ueBTxGrant;
  bool m_ueCExpT203;
  uint32_t m_ueCNoPermToSilence;
  uint32_t m_ueCRxGrant;
  bool m_ueCRxRelease;
  bool m_ueCSilenceToNoPerm;
};

// Floor control release with support of queuing.
class McpttFloorReleaseWhileQueued : public McpttTestCase
{
public:
  McpttFloorReleaseWhileQueued (const std::string& name = "Floor Release - While Queued", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg);

private:
  bool m_ueATxRelease;
  bool m_ueAQueuedToNoPerm;
  bool m_ueBRxRelease;
  bool m_ueCRxRelease;
};

/**
 * Test case with one UE sending a floor request.
 */
class McpttFloorRequestIdleBis : public McpttTestCase
{
public:
  McpttFloorRequestIdleBis (const std::string& name = "Floor Request - Idle Bis", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCFloorGrantedCb (void);
  virtual void UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg);

private:
  bool m_ueAHasPermToPendGrant;
  bool m_ueAPendReqToHasPerm;
  bool m_ueARxQueuePosInfoReq;
  uint32_t m_ueARxReq;
  bool m_ueASilenceToStartStop;
  bool m_ueAStartStopToPendReq;
  bool m_ueATxGranted;
  uint32_t m_ueATxReq;
  bool m_ueATxTaken;
  uint32_t m_ueATxQueuePosInfo;
  bool m_ueBRxGranted;
  uint32_t m_ueBRxQueuePosInfo;
  uint32_t m_ueBRxReq;
  bool m_ueBSilenceToStartStop;
  bool m_ueBStartStopToNoPerm;
  bool m_ueBRxQueuePosInfoReq;
  bool m_ueBRxTaken;
  uint32_t m_ueBRxRtp;
  bool m_ueCPendReqToQueued;
  bool m_ueCQueuedToSilence;
  bool m_ueCRxGranted;
  uint32_t m_ueCRxQueuePosInfo;
  uint32_t m_ueCRxReq;
  uint32_t m_ueCRxRtp;
  bool m_ueCRxTaken;
  bool m_ueCStartStopToPendReq;
  bool m_ueCSilenceToStartStop;
  uint32_t m_ueCTxReq;
  bool m_ueCTxQueuePosInfoReq;
};

class McpttSessionInitPrivateCall : public McpttTestCase
{
public:
  McpttSessionInitPrivateCall (const std::string& name = "Session Initialization - Private Call", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);

private:
  bool m_ueAHasPermToSilence;
  bool m_ueARxReq;
  bool m_ueASilenceToPendGrant;
  bool m_ueAStartStopToHasPerm;
  uint32_t m_ueATxGrant;
  bool m_ueATxRelease;
  bool m_ueBNoPermToSilence;
  bool m_ueBPendReqToHasPerm;
  bool m_ueBSilenceToPendReq;
  bool m_ueBStartStopToNoPerm;
  uint32_t m_ueBRxGrant;
  bool m_ueBRxRtp;
  bool m_ueBRxRelease;
  bool m_ueBTxReq;
};

/**
 * Test case with one UE sending a floor request.
 */
class McpttFloorReleasePreemptedFloorArbitrator : public McpttTestCase
{
public:
  McpttFloorReleasePreemptedFloorArbitrator (const std::string& name = "Floor Release - Preemptive Floor Arbitrator", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg);
  virtual void UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState);
  virtual void UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg);

private:
  bool m_ueAHasPermToPendGrant;
  bool m_ueAPendGrantToSilence;
  bool m_ueARxDeny;
  bool m_ueARxRelease;
  uint32_t m_ueARxReq;
  bool m_ueATxDeny;
  uint32_t m_ueATxGrant;
  bool m_ueBNoPermToPendReq;
  bool m_ueBPendReqToHasPerm;
  bool m_ueBHasPermToSilence;
  bool m_ueBRxDeny;
  uint32_t m_ueBRxGrant;
  bool m_ueBRxReq;
  bool m_ueBTxDeny;
  bool m_ueBTxReq;
  bool m_ueBTxRelease;
  bool m_ueCNoPermToPendReq;
  bool m_ueCPendReqToNoPerm;
  bool m_ueCNoPermToSilence;
  bool m_ueCSilenceToNoPerm;
  uint32_t m_ueCRxDeny;
  bool m_ueCRxRelease;
  bool m_ueCRxReq;
  uint32_t m_ueCRxGrant;
  bool m_ueCTxReq;
};

class McpttFloorControlTestSuite : public TestSuite
{
public:
  McpttFloorControlTestSuite (void);
};

/***************************************************************
 *  Implementation of the code declared above.
 ***************************************************************/

static McpttFloorControlTestSuite suite;

TypeId
TestFloorMachine::GetTypeId (void)
{
  static TypeId tid = TypeId ("TestFloorMachine")
    .SetParent<McpttOffNetworkFloorParticipant> ()
    .AddConstructor<TestFloorMachine> ()
  ;

  return tid;
}

TestFloorMachine::TestFloorMachine (void)
  : McpttOffNetworkFloorParticipant (),
    m_droppers (new std::vector<Ptr<McpttMsgDropper> > ()),
    m_startState (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ())
{
  SetPostRxCb (MakeNullCallback<void, const TestFloorMachine&, const McpttMsg&> ());
  SetPostTimerExpCb (MakeNullCallback<void, const TestFloorMachine&, Ptr<McpttTimer> > ());
  SetPostTxCb (MakeNullCallback<void, const TestFloorMachine&, const McpttMsg&> ());
  SetPreRxCb (MakeNullCallback<void, const TestFloorMachine&, const McpttMsg&> ());
  SetPreTimerExpCb (MakeNullCallback<void, const TestFloorMachine&, Ptr<McpttTimer> > ());
  SetPreTxCb (MakeNullCallback<void, const TestFloorMachine&, const McpttMsg&> ());
  SetStateChangeTestCb (MakeNullCallback<void, const TestFloorMachine&, Ptr<McpttOffNetworkFloorParticipantState>, Ptr<McpttOffNetworkFloorParticipantState> > ());
}

TestFloorMachine::~TestFloorMachine (void)
{ }

void
TestFloorMachine::AddDropper (Ptr<McpttMsgDropper>  dropper)
{
  std::vector<Ptr<McpttMsgDropper> >* droppers = GetDroppers ();

  droppers->push_back (dropper);
}

void
TestFloorMachine::ChangeState (Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  Ptr<McpttOffNetworkFloorParticipantState> oldState = GetState ();
  Callback<void, const TestFloorMachine&, Ptr<McpttOffNetworkFloorParticipantState>, Ptr<McpttOffNetworkFloorParticipantState> > stateChangeCb = GetStateChangeCb ();

  McpttOffNetworkFloorParticipant::ChangeState (newState);

  if (!stateChangeCb.IsNull ()
      && oldState->GetInstanceStateId () != newState->GetInstanceStateId ())
    {
      stateChangeCb (*this, oldState, newState);
    }
}

void
TestFloorMachine::ExpiryOfT201 (void)
{
  Callback<void, const TestFloorMachine&, Ptr<McpttTimer> > postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestFloorMachine&, Ptr<McpttTimer> > preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, GetT201 ());
    }

  McpttOffNetworkFloorParticipant::ExpiryOfT201 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, GetT201 ());
    }
}

void
TestFloorMachine::ExpiryOfT203 (void)
{
  Callback<void, const TestFloorMachine&, Ptr<McpttTimer> > postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestFloorMachine&, Ptr<McpttTimer> > preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, GetT203 ());
    }

  McpttOffNetworkFloorParticipant::ExpiryOfT203 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, GetT203 ());
    }
}

void
TestFloorMachine::ExpiryOfT205 (void)
{
  Callback<void, const TestFloorMachine&, Ptr<McpttTimer> > postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestFloorMachine&, Ptr<McpttTimer> > preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, GetT205 ());
    }

  McpttOffNetworkFloorParticipant::ExpiryOfT205 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, GetT205 ());
    }
}

TypeId
TestFloorMachine::GetInstanceTypeId (void) const
{
  return TestFloorMachine::GetTypeId ();
}

void
TestFloorMachine::Receive (const McpttFloorMsg& msg)
{
  bool drop = false;
  std::vector<Ptr<McpttMsgDropper> >* droppers = GetDroppers ();
  Callback<void, const TestFloorMachine&, const McpttMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestFloorMachine&, const McpttMsg&> preRxCb = GetPreRxCb ();

  for (std::vector<Ptr<McpttMsgDropper> >::iterator it = droppers->begin (); it != droppers->end () && drop == false; it++)
    {
      drop = (*it)->ShouldDropMsg (msg);
    }

  if (drop == false)
    {
      if (!preRxCb.IsNull ())
        {
          preRxCb (*this, msg);
        }

      McpttOffNetworkFloorParticipant::Receive (msg);

      if (!postRxCb.IsNull ())
        {
          postRxCb (*this, msg);
        }
    }
}

void
TestFloorMachine::Receive (const McpttMediaMsg& msg)
{
  bool drop = false;
  std::vector<Ptr<McpttMsgDropper> >* droppers = GetDroppers ();
  Callback<void, const TestFloorMachine&, const McpttMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestFloorMachine&, const McpttMsg&> preRxCb = GetPreRxCb ();

  for (std::vector<Ptr<McpttMsgDropper> >::iterator it = droppers->begin (); it != droppers->end () && drop == false; it++)
    {
      drop = (*it)->ShouldDropMsg (msg);
    }

  if (drop == false)
    {
      if (!preRxCb.IsNull ())
        {
          preRxCb (*this, msg);
        }

      McpttOffNetworkFloorParticipant::ReceiveMedia (msg);

      if (!postRxCb.IsNull ())
        {
          postRxCb (*this, msg);
        }
    }
}

void
TestFloorMachine::Send (const McpttFloorMsg& msg)
{
  Callback<void, const TestFloorMachine&, const McpttMsg&> postTxCb = GetPostTxCb ();
  Callback<void, const TestFloorMachine&, const McpttMsg&> preTxCb = GetPreTxCb ();

  if (!preTxCb.IsNull ())
    {
      preTxCb (*this, msg);
    }

  McpttOffNetworkFloorParticipant::Send (msg);

  if (!postTxCb.IsNull ())
    {
      postTxCb (*this, msg);
    }
}

void
TestFloorMachine::Start (void)
{
  Ptr<McpttOffNetworkFloorParticipantState> startState = GetStartState ();

  SetState (startState);

  McpttOffNetworkFloorParticipant::Start ();
}

void
TestFloorMachine::DoDispose (void)
{
  std::vector<Ptr<McpttMsgDropper> >* droppers = GetDroppers ();
  for (std::vector<Ptr<McpttMsgDropper> >::iterator it = droppers->begin (); it != droppers->end (); it++)
    {
      (*it) = 0;
    }

  delete droppers;

  SetDroppers (0);

  McpttOffNetworkFloorParticipant::DoDispose ();
}

std::vector<Ptr<McpttMsgDropper> >*
TestFloorMachine::GetDroppers (void) const
{
  return m_droppers;
}

void
TestFloorMachine::SetDroppers (std::vector<Ptr<McpttMsgDropper> >* const& droppers)
{
  m_droppers = droppers;
}

Callback<void, const TestFloorMachine&, const McpttMsg&>
TestFloorMachine::GetPostRxCb (void) const
{
  return m_postRxCb;
}

Callback<void, const TestFloorMachine&, Ptr<McpttTimer> >
TestFloorMachine::GetPostTimerExpCb (void) const
{
  return m_postTimerExpCb;
}


Callback<void, const TestFloorMachine&, const McpttMsg&>
TestFloorMachine::GetPostTxCb (void) const
{
  return m_postTxCb;
}

Callback<void, const TestFloorMachine&, const McpttMsg&>
TestFloorMachine::GetPreRxCb (void) const
{
  return m_preRxCb;
}

Callback<void, const TestFloorMachine&, Ptr<McpttTimer> >
TestFloorMachine::GetPreTimerExpCb (void) const
{
  return m_preTimerExpCb;
}

Callback<void, const TestFloorMachine&, const McpttMsg&>
TestFloorMachine::GetPreTxCb (void) const
{
  return m_preTxCb;
}

Ptr<McpttOffNetworkFloorParticipantState>
TestFloorMachine::GetStartState (void) const
{
  return m_startState;
}

Callback<void, const TestFloorMachine&, Ptr<McpttOffNetworkFloorParticipantState>, Ptr<McpttOffNetworkFloorParticipantState> >
TestFloorMachine::GetStateChangeCb (void) const
{
  return m_stateChangeCb;
}

void
TestFloorMachine::SetPostRxCb (const Callback<void, const TestFloorMachine&, const McpttMsg&>  postRxCb)
{
  m_postRxCb = postRxCb;
}

void
TestFloorMachine::SetPostTimerExpCb (const Callback<void, const TestFloorMachine&, Ptr<McpttTimer> >  timerExpCb)
{
  m_postTimerExpCb = timerExpCb;
}

void
TestFloorMachine::SetPostTxCb (const Callback<void, const TestFloorMachine&, const McpttMsg&>  postTxCb)
{
  m_postTxCb = postTxCb;
}

void
TestFloorMachine::SetPreRxCb (const Callback<void, const TestFloorMachine&, const McpttMsg&>  preRxCb)
{
  m_preRxCb = preRxCb;
}

void
TestFloorMachine::SetPreTimerExpCb (const Callback<void, const TestFloorMachine&, Ptr<McpttTimer> >  timerExpCb)
{
  m_preTimerExpCb = timerExpCb;
}

void
TestFloorMachine::SetPreTxCb (const Callback<void, const TestFloorMachine&, const McpttMsg&>  preTxCb)
{
  m_preTxCb = preTxCb;
}

void
TestFloorMachine::SetStartState (Ptr<McpttOffNetworkFloorParticipantState>  startState)
{
  m_startState = startState;
}

void
TestFloorMachine::SetStateChangeTestCb (const Callback<void, const TestFloorMachine&, Ptr<McpttOffNetworkFloorParticipantState>, Ptr<McpttOffNetworkFloorParticipantState> >  stateChangeCb)
{
  m_stateChangeCb = stateChangeCb;
}

McpttFloorRequestIdle::McpttFloorRequestIdle (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_pendReqToHasPerm (false),
    m_reqCount (0),
    m_startStopToPendReq (false),
    m_takenCount (0),
    m_ueBRxReq (false),
    m_ueBSilenceToNoPerm (false),
    m_ueBRxTaken (false),
    m_ueCRxReq (false),
    m_ueCSilenceToNoPerm (false),
    m_ueCRxTaken (false)
{ }

void
McpttFloorRequestIdle::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();

  config->SetAppCount (3);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<TestFloorMachine> ueCMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (2));

  Ptr<McpttTimer> ueAT230 = ueAMachine->GetT230 ();
  Ptr<McpttTimer> ueBT230 = ueBMachine->GetT230 ();
  Ptr<McpttTimer> ueCT230 = ueCMachine->GetT230 ();

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestIdle::TxCb, this));
  ueAMachine->SetPreRxCb (MakeCallback (&McpttFloorRequestIdle::RxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestIdle::StateChangeCb, this));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestIdle::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestIdle::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestIdle::UeBStateChangeCb, this));

  ueCMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueCMachine->SetStartState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestIdle::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestIdle::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestIdle::UeCStateChangeCb, this));

  Ptr<McpttPusher> ueAPusher = ueAPttApp->GetPusher ();
  ueAPusher->SchedulePush (Seconds (2.1));

  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueAT230);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueBT230);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueCT230);
}

void
McpttFloorRequestIdle::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_startStopToPendReq, true, "Did not make transition from 'O: start stop' to 'O: pending request'.");
  NS_TEST_ASSERT_MSG_EQ (m_pendReqToHasPerm, true, "Did not make transistion from 'O: pending request' to 'O: has permission'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBSilenceToNoPerm, true, "UE B did not make transition from 'O: silence' to 'O: has no permission'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCSilenceToNoPerm, true, "UE C did not make transition from 'O: silence' to 'O: has no permission'.");
}

void
McpttFloorRequestIdle::IncrementReqCount (const uint32_t& amount)
{
  m_reqCount += amount;
}

void
McpttFloorRequestIdle::IncrementTakenCount (const uint32_t& amount)
{
  m_takenCount += amount;
}

void
McpttFloorRequestIdle::RxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "A message was received even though there is only one UE.");
}

void
McpttFloorRequestIdle::StateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_startStopToPendReq, false, "UE A already made transition from " << *oldState << " to " << *newState << ".");
      m_startStopToPendReq = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () && !m_pendReqToHasPerm)
    {
      NS_TEST_ASSERT_MSG_EQ (m_startStopToPendReq, true, "UE A already made transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << "'.");
      m_pendReqToHasPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestIdle::TxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  TypeId msgTypeId = msg.GetInstanceTypeId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  uint32_t c201Val = c201->GetValue ();

  if (msgTypeId == McpttFloorMsgRequest::GetTypeId ())
    {
      if (c201Val == 1)
        {
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateSilence::GetStateId (), "Floor machine not in '" << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << "' after floor request was sent.");
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStatePendReq::GetStateId (), "Floor machine not in '" << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << "' after floor request was sent.");
        }

      NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), true, "Timer T201 is not running.");
      NS_TEST_ASSERT_MSG_LT (c201Val, 4, "The counter C201 has gone above three.");
      NS_TEST_ASSERT_MSG_GT_OR_EQ (c201Val, 1, "The counter C201 is below 1.");

      IncrementReqCount ();
    }
  else if (msgTypeId == McpttFloorMsgTaken::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStatePendReq::GetStateId (), "Floor machine not in '" << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << "' after floor taken was sent.");
      NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer T201 is still running.");
      NS_TEST_ASSERT_MSG_EQ (c201Val, 3, "The counter C201 was reset.");

      IncrementTakenCount ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "A message other than a floor request or floor taken message was sent.");
    }

  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not at one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not at one.");
}

void
McpttFloorRequestIdle::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  TypeId msgTid = msg.GetInstanceTypeId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msgTid == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueBRxReq = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateSilence::GetStateId (), "UE B in invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
    }
  else if (msgTid == McpttFloorMsgTaken::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, true, "UE B never received a floor request message.");
      m_ueBRxTaken = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE B in invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
      if (m_ueCRxTaken == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorRequestIdle::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  TypeId msgTid = msg.GetInstanceTypeId ();

  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg << ".");
}

void
McpttFloorRequestIdle::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBSilenceToNoPerm, false, "UE B already made transition from " << *oldState << " to " << *newState << ".");
      m_ueBSilenceToNoPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestIdle::UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  TypeId msgTid = msg.GetInstanceTypeId ();
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msgTid == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueCRxReq = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateSilence::GetStateId (), "UE C in invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
    }
  else if (msgTid == McpttFloorMsgTaken::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxReq, true, "UE C never received a floor request message.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C in invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      m_ueCRxTaken = true;
      if (m_ueBRxTaken == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorRequestIdle::UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  TypeId msgTid = msg.GetInstanceTypeId ();

  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg << ".");
}

void
McpttFloorRequestIdle::UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCSilenceToNoPerm, false, "UE C already made transition from " << *oldState << " to " << *newState << ".");
      m_ueCSilenceToNoPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << *oldState << " to " << *newState << ".");
    }
}

McpttFloorRequestDenied::McpttFloorRequestDenied (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_hasNoPermToPendReq (false),
    m_pendReqToNoPerm (false),
    m_rxOfFloorDeny (false),
    m_rxOfFloorReq (false),
    m_txOfFloorDeny (false),
    m_txOfFloorReq (false),
    m_ueCRxOfFloorDeny (false),
    m_ueCRxOfFloorReq (false)
{ }

void
McpttFloorRequestDenied::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();
  config->SetAppCount (3);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());
  config->SetStart (Seconds (2.0));
  config->SetStop (Seconds (7.0));

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<TestFloorMachine> ueCMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (2));

  Ptr<McpttTimer> ueAT201 = ueAMachine->GetT201 ();
  Ptr<McpttTimer> ueAT203 = ueAMachine->GetT203 ();
  Ptr<McpttTimer> ueBT206 = ueBMachine->GetT206 ();
  Ptr<McpttTimer> ueCT203 = ueCMachine->GetT203 ();

  Ptr<McpttPusher> ueAPusher = ueAPttApp->GetPusher ();
  Ptr<McpttPusher> ueBPusher = ueBPttApp->GetPusher ();

  ueAMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());
  ueBMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestDenied::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestDenied::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestDenied::UeAStateChangeCb, this));

  ueAT201->SetDelay (Seconds (0.120));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
  ueBMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestDenied::UeBTxCb, this));
  ueBMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestDenied::UeBRxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestDenied::UeBStateChangeCb, this));

  ueCMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueCMachine->SetStartState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
  ueCMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestDenied::UeCTxCb, this));
  ueCMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestDenied::UeCRxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestDenied::UeCStateChangeCb, this));
  ueCMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());

  ueAPusher->SchedulePush (Seconds (2.1));

  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueAT203);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueBT206);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueCT203);
}

void
McpttFloorRequestDenied::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << ".");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_hasNoPermToPendReq, true, "Did not make transition from 'O: has no permission' to 'O: pending request'.");
  NS_TEST_ASSERT_MSG_EQ (m_pendReqToNoPerm, true, "Did not make transistion from 'O: pending request' to 'O: has no permission'.");
  NS_TEST_ASSERT_MSG_EQ (m_rxOfFloorDeny, true, "UE A did not recieve a floor deny message.");
  NS_TEST_ASSERT_MSG_EQ (m_rxOfFloorReq, true, "UE B did not recieve a floor request message.");
  NS_TEST_ASSERT_MSG_EQ (m_txOfFloorDeny, true, "UE B did not send a floor deny message.");
  NS_TEST_ASSERT_MSG_EQ (m_txOfFloorReq, true, "UE A did not send a floor request message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxOfFloorDeny, true, "UE C did not recieve a floor deny message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxOfFloorReq, true, "UE C did not recieve a floor request message.");
}

void
McpttFloorRequestDenied::UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgDeny::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_rxOfFloorDeny, false, "Received more than one floor deny message.");
      m_rxOfFloorDeny = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "In state " << *state << " but should be in state '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is still running.");
      NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 was not reset.");

      if (m_ueCRxOfFloorDeny == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, msg << " received. Only expected one floor deny message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorRequestDenied::UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_hasNoPermToPendReq, false, "A transition was already made from " << *oldState << " to " << *newState << ".");
      NS_TEST_ASSERT_MSG_EQ (m_pendReqToNoPerm, false, "A transition was already made from '" << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");

      m_hasNoPermToPendReq = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_pendReqToNoPerm, false, "A transition was already made from " << *oldState << " to " << *newState << ".");
      NS_TEST_ASSERT_MSG_EQ (m_hasNoPermToPendReq, true, "A transition was never made from '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << "'.");
      m_pendReqToNoPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestDenied::UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE A in invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (m_txOfFloorReq, false, "A floor request message has already been sent.");
      m_txOfFloorReq = true;
      NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), true, "Timer " << *t201 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "The counter C201 is not equal to one.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Only expected to send one floor request message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorRequestDenied::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_rxOfFloorReq, false, "More than one floor request message was received.");

      m_rxOfFloorReq = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, msg << " received. Only expecting one floor request message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), true, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");

  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter c201 is not at one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter c204 is not at one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter c205 is not at one.");
}

void
McpttFloorRequestDenied::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestDenied::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgDeny::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_txOfFloorDeny, false, "More than one floor deny message was sent.");

      m_txOfFloorDeny = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, msg << " sent. Only expecting to send one floor deny message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), true, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");

  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter c201 is not at one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter c204 is not at one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter c205 is not at one.");
}

void
McpttFloorRequestDenied::UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgDeny::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxOfFloorReq, true, "UE C has not seen a floor request message yet.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxOfFloorDeny, false, "UE C saw more than one floor deny message.");
      m_ueCRxOfFloorDeny = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C In state " << *state << " but should be in state '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
      if (m_rxOfFloorDeny == true)
        {
          Stop ();
        }
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxOfFloorReq, false, "UE C saw more than one floor request message.");
      m_ueCRxOfFloorReq = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C In state " << *state << " but should be in state '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, msg << " received. Only expected one floor deny message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorRequestDenied::UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestDenied::UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  TypeId msgTid = msg.GetInstanceTypeId ();

  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg << ".");
}

McpttFloorRequestPreemptive::McpttFloorRequestPreemptive (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueANoPermToPendReq (false),
    m_ueAPendReqToHasPerm (false),
    m_ueARxRtp (false),
    m_ueATxReq (false),
    m_ueARxGrant (false),
    m_ueBHasPermToPendGrant (false),
    m_ueBPendGrantToNoPerm (false),
    m_ueBRxReq (false),
    m_ueBTxGrant (false),
    m_ueBRxRtp (false),
    m_ueCRxGrant (false),
    m_ueCRxReq (false),
    m_ueCRxRtp (false)
{ }

void
McpttFloorRequestPreemptive::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();
  config->SetAppCount (3);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());
  config->SetStart (Seconds (2.0));
  config->SetStop (Seconds (2.5));

  Config::SetDefault ("ns3::psc::McpttOffNetworkFloorParticipant::T201", TimeValue (Time ("120ms")));

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<TestFloorMachine> ueCMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (2));

  Ptr<McpttTimer> ueAT201 = ueAMachine->GetT201 ();
  Ptr<McpttTimer> ueBT205 = ueBMachine->GetT205 ();

  Ptr<McpttPusher> ueAPusher = ueAPttApp->GetPusher ();
  Ptr<McpttPusher> ueBPusher = ueBPttApp->GetPusher ();

  Ptr<McpttMediaSrc> ueAMediaSrc = ueAPttApp->GetMediaSrc ();
  Ptr<McpttMediaSrc> ueBMediaSrc = ueBPttApp->GetMediaSrc ();

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (true));
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestPreemptive::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestPreemptive::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestPreemptive::UeAStateChangeCb, this));

  ueAT201->SetDelay (Seconds (0.160));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (true));
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
  ueBMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestPreemptive::UeBTxCb, this));
  ueBMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestPreemptive::UeBRxCb, this));
  ueBMachine->SetPreRxCb (MakeCallback (&McpttFloorRequestPreemptive::UeBPreRxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestPreemptive::UeBStateChangeCb, this));

  ueBT205->SetDelay (Seconds (0.120));

  ueCMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueCMachine->SetStartState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
  ueCMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestPreemptive::UeCTxCb, this));
  ueCMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestPreemptive::UeCRxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestPreemptive::UeCStateChangeCb, this));

  ueAPusher->SchedulePush (Seconds (2.24));

  ueAMediaSrc->SetAttribute ("DataRate", DataRateValue (DataRate ("275b/s")));
  ueAMediaSrc->SetAttribute ("Bytes", UintegerValue (8));
  ueBMediaSrc->SetAttribute ("DataRate", DataRateValue (DataRate ("275b/s")));
  ueBMediaSrc->SetAttribute ("Bytes", UintegerValue (8));

  ueAMachine->SetPriority (7);
  ueAMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());
  ueBMachine->SetPriority (1);
  ueBMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());
  ueCMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());
}

void
McpttFloorRequestPreemptive::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << ".");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueANoPermToPendReq, true, "UE A did not make transition from 'O: has no permission' to 'O: pending request'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAPendReqToHasPerm, true, "UE A did not make transition from 'O: pending request' to 'O: has permission'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxRtp, true, "UE A did not receive any RTP packets.");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, true, "UE A did not send a floor request message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxGrant, true, "UE A did not receive a floor granted message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBHasPermToPendGrant, true, "UE B did not make transition from 'O: has permission' to 'O: pending grant'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBPendGrantToNoPerm, true, "UE B did not make transition from 'O: pending grant' to 'O: has no permission'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, true, "UE B did not receive a floor request message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxGrant, true, "UE B did not send a floor granted message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRtp, true, "UE B did not receive any RTP media packets.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrant, true, "UE C never saw the grant message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxReq, true, "UE C never saw the request message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxRtp, true, "UE C never saw RTP media message.");
}

void
McpttFloorRequestPreemptive::UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      m_ueARxRtp = true;
      bool validState = (state->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ());

      if (!validState && state->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ())
        {
          validState = true;
          NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), true, "Timer " << *t201 << " is not running.");
        }

      NS_TEST_ASSERT_MSG_EQ (validState, true, "Invalid state " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      m_ueARxGrant = true;
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), true, "Timer " << *t206 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, true, "UE A has not sent floor request yet.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateHasPerm::GetStateId (), "Not in '" << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << "' state.");
      NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is still running.");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is still running.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message received: " << msg);
    }

  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is still running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");

  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "The counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "The counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "The counter C205 is not one.");
}

void
McpttFloorRequestPreemptive::UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueANoPermToPendReq, false, "Invalid sequence of state transitions.");

      m_ueANoPermToPendReq = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueANoPermToPendReq, true, "Invalid sequence of state transitions.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAPendReqToHasPerm, false, "Invalid sequence of state transitions.");

      m_ueAPendReqToHasPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestPreemptive::UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRtp, true, "UE A has not received any RTP packets yet.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "Invalid state " << *state << ".");
      m_ueATxReq = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message sent: " << msg);
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), true, "Timer " << *t201 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");

  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "The counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "The counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "The counter C205 is not one.");
}

void
McpttFloorRequestPreemptive::UeBPreRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRtp, true, "UE B has not sent any RTP packets.");

      m_ueBRxReq = true;

      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateHasPerm::GetStateId (), "Invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), true, "Timer " << *t206 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "The counter C201 is not one.");
      NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "The counter C204 is not one.");
      NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "The counter C205 is not one.");
    }
}

void
McpttFloorRequestPreemptive::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      // Do nothing because the "pre Rx" callback handled this.
    }
  else if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxGrant, true, "UE B has not sent grant message yet.");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "Invalid state: " << *state << ".");
      m_ueBRxRtp = true;
      if (m_ueCRxRtp == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message received: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "The counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "The counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "The counter C205 is not one.");
}

void
McpttFloorRequestPreemptive::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendGrant::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBHasPermToPendGrant, false, "Invalid sequence of state transitions.");

      m_ueBHasPermToPendGrant = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendGrant::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBHasPermToPendGrant, true, "Invalid sequence of state transitions.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBPendGrantToNoPerm, false, "Invalid sequence of state transitions.");

      m_ueBPendGrantToNoPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestPreemptive::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      m_ueBTxGrant = true;

      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateHasPerm::GetStateId (), "Invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), true, "Timer " << *t205 << " is not running.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message sent: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_LT (c205->GetValue (), 3, "Counter C205 is not less than three.");
}

void
McpttFloorRequestPreemptive::UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxReq, true, "UE C has not seen a floor request message yet.");
      m_ueCRxGrant = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C In state " << *state << " but should be in state '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxRtp, true, "UE C has not seen any RTP media messages yet.");
      m_ueCRxReq = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C In state " << *state << " but should be in state '" <<  McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
    }
  else if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      m_ueCRxRtp = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C In state " << *state << " but should be in state '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
      if (m_ueBRxRtp == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, msg << " received. Only expected one floor deny message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorRequestPreemptive::UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestPreemptive::UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  TypeId msgTid = msg.GetInstanceTypeId ();

  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg << ".");
}

McpttFloorRequestQueued::McpttFloorRequestQueued (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueANoPermToPendReq (false),
    m_ueAPendReqToQueued (false),
    m_ueAQueuedToHasPerm (false),
    m_ueARxGrant (false),
    m_ueARxInfo (false),
    m_ueARxRtp (false),
    m_ueATxReq (false),
    m_ueAT201ExpCt (0),
    m_ueBHasPermToPendGrant (false),
    m_ueBPendGrantToNoPerm (false),
    m_ueBRxReq (false),
    m_ueBRxRtp (false),
    m_ueBTxInfo (false),
    m_ueBTxGrant (false),
    m_ueBT205ExpCt (0),
    m_ueCRxGrant (false),
    m_ueCRxInfo (false),
    m_ueCRxReq (false),
    m_ueCRxRtp (false)
{ }

void McpttFloorRequestQueued::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();
  config->SetAppCount (3);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());
  config->SetStart (Seconds (2.0));
  config->SetStop (Seconds (5.0));

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<TestFloorMachine> ueCMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (2));

  Ptr<McpttTimer> ueAT201 = ueAMachine->GetT201 ();
  Ptr<McpttTimer> ueAT203 = ueAMachine->GetT203 ();
  Ptr<McpttTimer> ueBT205 = ueBMachine->GetT205 ();
  Ptr<McpttTimer> ueBT206 = ueBMachine->GetT206 ();
  Ptr<McpttTimer> ueCT203 = ueCMachine->GetT203 ();

  Ptr<McpttFloorQueue> ueAQueue = ueAMachine->GetQueue ();
  Ptr<McpttFloorQueue> ueBQueue = ueBMachine->GetQueue ();

  Ptr<McpttPusher> ueAPusher = ueAPttApp->GetPusher ();
  Ptr<McpttPusher> ueBPusher = ueBPttApp->GetPusher ();

  Ptr<McpttMediaSrc> ueAMediaSrc = ueAPttApp->GetMediaSrc ();
  Ptr<McpttMediaSrc> ueBMediaSrc = ueBPttApp->GetMediaSrc ();

  Ptr<McpttMsgDropperImpl> ueAPttAppMcpttMsgDropper = Create<McpttMsgDropperImpl> (3, McpttFloorMsgGranted::GetTypeId ());
  Ptr<McpttMsgDropperImpl> ueBPttAppMcpttMsgDropper = Create<McpttMsgDropperImpl> (5, McpttFloorMsgRequest::GetTypeId ());

  ueAMachine->AddDropper (ueAPttAppMcpttMsgDropper);
  ueBMachine->AddDropper (ueBPttAppMcpttMsgDropper);

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (true));
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestQueued::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestQueued::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestQueued::UeAStateChangeCb, this));
  ueAMachine->SetPreTimerExpCb (MakeCallback (&McpttFloorRequestQueued::UeATimerExpCb, this));

  ueAT201->SetDelay (Seconds (0.160));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (true));
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
  ueBMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestQueued::UeBTxCb, this));
  ueBMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestQueued::UeBRxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestQueued::UeBStateChangeCb, this));
  ueBMachine->SetPreTimerExpCb (MakeCallback (&McpttFloorRequestQueued::UeBTimerExpCb, this));

  ueBT205->SetDelay (Seconds (0.120));

  ueCMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueCMachine->SetStartState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
  ueCMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestQueued::UeCTxCb, this));
  ueCMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestQueued::UeCRxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestQueued::UeCStateChangeCb, this));
  ueCMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());

  ueAMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());
  ueBMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());

  ueAQueue->SetAttribute ("Capacity", UintegerValue (1));
  ueBQueue->SetAttribute ("Capacity", UintegerValue (1));

  ueAPusher->SchedulePush (Seconds (2.04179));
  ueBPusher->SetAttribute ("PttDurationVariable", StringValue ("ns3::ConstantRandomVariable[Constant=0.90]"));

  ueAMediaSrc->SetAttribute ("DataRate", DataRateValue (DataRate ("200b/s")));
  ueAMediaSrc->SetAttribute ("Bytes", UintegerValue (8));
  ueBMediaSrc->SetAttribute ("DataRate", DataRateValue (DataRate ("192b/s")));
  ueBMediaSrc->SetAttribute ("Bytes", UintegerValue (8));

  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueAT203);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueBT206);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueCT203);
}

void McpttFloorRequestQueued::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << ".");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueANoPermToPendReq, true, "UE A did not make transition from 'O: has no permission' to 'O: pending request'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAPendReqToQueued, true, "UE A did not make transition from 'O: pending request' to 'O: queued'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQueuedToHasPerm, true, "UE A did not make transition from 'O: queued' to 'O: has permission'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxGrant, true, "UE A did not send receive a 'Floor Granted' message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxInfo, true, "UE A did not receive a 'Queue Position Info' message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxRtp, true, "UE A did not recieve any RTP media packets.");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, true, "UE A did not send a 'Floor Request' message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAT201ExpCt, 4, "UE A's T201 timer did not expire four times.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBHasPermToPendGrant, true, "UE B did not make transition from 'O: has permission' to 'O: pending grant'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBPendGrantToNoPerm, true, "UE B did not make transition from 'O: pending grant' to 'O: has no permission'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, true, "UE B did not receive a 'Floor Request' message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRtp, true, "UE B did not receive any RTP media packets.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxInfo, true, "UE B did not send a 'Queue Position Info' message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxGrant, true, "UE B did not send a 'Floor Granted' message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBT205ExpCt, 2, "UE B's T205 timer did not expire two times.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrant, true, "UE C never saw the grant message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxInfo, true, "UE C never saw the queue position info message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxReq, true, "UE C never saw the request message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxRtp, true, "UE C never saw RTP media message.");
}

void McpttFloorRequestQueued::UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, true, "UE A has not sent 'Floor Request' message yet.");

      bool validState = (state->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateQueued::GetStateId ());

      if (!validState && state->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ())
        {
          validState = true;
          NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), true, "Timer " << *t201 << " is not running.");
        }

      m_ueARxRtp = true;
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (validState, true, "Invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
      NS_TEST_ASSERT_MSG_LT (c201->GetValue (), 4, "Counter C201 was not reset.");
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgQueuePositionInfo::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRtp, true, "UE A has not received any RTP media packets yet.");
      m_ueARxInfo = true;
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateQueued::GetStateId (), "Invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
      NS_TEST_ASSERT_MSG_LT (c201->GetValue (), 4, "Counter C201 was not reset.");
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRtp, true, "UE A has not received any RTP media packets yet.");
      m_ueARxGrant = true;
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateQueued::GetStateId (), "Invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), true, "Timer " << *t233 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 was not reset.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message sent: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 was not reset.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 was not reset.");
}

void
McpttFloorRequestQueued::UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ())
    {
      m_ueANoPermToPendReq = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateQueued::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueANoPermToPendReq, true, "UE A made invalid state transition.");
      m_ueAPendReqToQueued = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateQueued::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAPendReqToQueued, true, "UE A made invalid state transition.");
      m_ueAQueuedToHasPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A made invalid state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestQueued::UeATimerExpCb (const TestFloorMachine& machine, Ptr<McpttTimer>  timer)
{
  Ptr<McpttCounter> c201 = machine.GetC201 ();

  if (timer == machine.GetT201 ())
    {
      m_ueAT201ExpCt += 1;
      NS_TEST_ASSERT_MSG_LT (c201->GetValue (), c201->GetLimit (), "The counter C201 is going to exceeded it's limit.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A's " << timer << " timer unexpectedly expired.");
    }
}

void
McpttFloorRequestQueued::UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      if (c201->GetValue () == 1)
        {
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE A in invalid state: " << *state << ".");
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStatePendReq::GetStateId (), "UE A in invalid state: " << *state << ".");
        }
      m_ueATxReq = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected message: " << msg);
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), true, "Timer " << *t201 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_LT (c201->GetValue (), 4, "Counter C201 reached limit and was not reset.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorRequestQueued::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueBRxReq = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateHasPerm::GetStateId (), "UE B in invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), true, "Timer " << *t206 << " is not running.");
    }
  else if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxGrant, true, "UE B has not sent a 'Floor Granted' message yet.");
      m_ueBRxRtp = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE B in invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
      if (m_ueBRxRtp == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg);
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_LT (c205->GetValue (), 4, "Counter C205 is not less than four.");
}

void
McpttFloorRequestQueued::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendGrant::GetStateId ())
    {
      m_ueBHasPermToPendGrant = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendGrant::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBHasPermToPendGrant, true, "UE B has not transition from 'O: has permission' to 'O: pending grant'.");
      m_ueBPendGrantToNoPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestQueued::UeBTimerExpCb (const TestFloorMachine& machine, Ptr<McpttTimer>  timer)
{
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (timer == machine.GetT205 ())
    {
      m_ueBT205ExpCt += 1;
      NS_TEST_ASSERT_MSG_LT (c205->GetValue (), c205->GetLimit (), "The counter C205 is going to reach it's limit.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B's " << timer << " timer unexpectedly expired.");
    }
}

void
McpttFloorRequestQueued::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgQueuePositionInfo::GetTypeId ())
    {
      m_ueBTxInfo = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateHasPerm::GetStateId (), "UE B in invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), true, "Timer " << *t206 << " is not running.");
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxInfo, true, "UE B has not sent a queue info message yet.");
      NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), true, "Timer " << *t205 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");

      if (c205->GetValue () == 1)
        {
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateHasPerm::GetStateId (), "UE B in invalid state: " << *state << ".");
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStatePendGrant::GetStateId (), "UE B in invalid state: " << *state << ".");
        }

      m_ueBTxGrant = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_LT (c205->GetValue (), 5, "Counter C205 reached its limit and was not reset.");
}

void
McpttFloorRequestQueued::UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxInfo, true, "UE C has not seen a queue position info message yet.");
      m_ueCRxGrant = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C In state " << *state << " but should be in state '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgQueuePositionInfo::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxReq, true, "UE C has not seen a received request message yet.");
      m_ueCRxInfo = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C In state " << *state << " but should be in state '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueCRxReq = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C In state " << *state << " but should be in state '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
    }
  else if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      m_ueCRxRtp = true;
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C In state " << *state << " but should be in state '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
      if (m_ueBRxRtp == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, msg << " received. Only expected one floor deny message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorRequestQueued::UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestQueued::UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  TypeId msgTid = msg.GetInstanceTypeId ();

  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg << ".");
}

McpttSessionInitNormal::McpttSessionInitNormal (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueATxGrant (false),
    m_ueAStartStopToHasPerm (false),
    m_ueBRxGrant (false),
    m_ueBStartStopToSilence (false),
    m_ueBSilenceToNoPerm (false),
    m_ueCRxGrant (false),
    m_ueCStartStopToSilence (false),
    m_ueCSilenceToNoPerm (false)
{ }

void
McpttSessionInitNormal::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();
  config->SetAppCount (3);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<TestFloorMachine> ueCMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (2));

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueAMachine->SetOriginator (true);
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttSessionInitNormal::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttSessionInitNormal::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttSessionInitNormal::UeAStateChangeCb, this));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ());
  ueBMachine->SetPostTxCb (MakeCallback (&McpttSessionInitNormal::UeBTxCb, this));
  ueBMachine->SetPostRxCb (MakeCallback (&McpttSessionInitNormal::UeBRxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttSessionInitNormal::UeBStateChangeCb, this));

  ueCMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueCMachine->SetStartState (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ());
  ueCMachine->SetPostTxCb (MakeCallback (&McpttSessionInitNormal::UeCTxCb, this));
  ueCMachine->SetPostRxCb (MakeCallback (&McpttSessionInitNormal::UeCRxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttSessionInitNormal::UeCStateChangeCb, this));
}

void
McpttSessionInitNormal::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << ".");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueATxGrant, true, "UE A never sent the grant message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAStartStopToHasPerm, true, "UE A never made the transition from '" << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrant, true, "UE B never saw the grant message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBStartStopToSilence, true, "UE B never made the transition from '" << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBSilenceToNoPerm, true, "UE B never made the transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrant, true, "UE C never saw the grant message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCStartStopToSilence, true, "UE C never made the transition from '" << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCSilenceToNoPerm, true, "UE C never made the transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
}

void
McpttSessionInitNormal::UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message: " << msg << ".");
}

void
McpttSessionInitNormal::UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAStartStopToHasPerm, false, "UE A already made transition from " << *oldState << " to " << *newState << ".");
      m_ueAStartStopToHasPerm = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttSessionInitNormal::UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxGrant, false, "UE A already sent " << msg << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateStartStop::GetStateId (), "UE A in invalid state: " << *state << ".");
      m_ueATxGrant = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttSessionInitNormal::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrant, false, "UE B already received " << msg << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE B in invalid state: " << *state << ".");
      m_ueBRxGrant = true;
      if (m_ueCRxGrant == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttSessionInitNormal::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBStartStopToSilence, false, "UE B already made transition from " << *oldState << " to " << *newState << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBSilenceToNoPerm, false, "UE B already made transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
      m_ueBStartStopToSilence = true;
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBStartStopToSilence, true, "UE B never made transition from '" << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBSilenceToNoPerm, false, "UE B already made transition from " << *oldState << " to " << *newState << ".");
      m_ueBSilenceToNoPerm = true;
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttSessionInitNormal::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg << ".");
}

void
McpttSessionInitNormal::UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrant, false, "UE C already received " << msg << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C in invalid state: " << *state << ".");
      m_ueCRxGrant = true;
      if (m_ueBRxGrant == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttSessionInitNormal::UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCStartStopToSilence, false, "UE C already made transition from " << *oldState << " to " << *newState << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCSilenceToNoPerm, false, "UE C already made transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
      m_ueCStartStopToSilence = true;
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCStartStopToSilence, true, "UE C never made transition from '" << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCSilenceToNoPerm, false, "UE C already made transition from " << *oldState << " to " << *newState << ".");
      m_ueCSilenceToNoPerm = true;
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttSessionInitNormal::UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg << ".");
}

McpttSessionInitLoss::McpttSessionInitLoss (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueATxGrant (false),
    m_ueAStartStopToHasPerm (false),
    m_ueBRxRtp (false),
    m_ueBStartStopToSilence (false),
    m_ueBSilenceToNoPerm (false),
    m_ueCRxRtp (false),
    m_ueCStartStopToSilence (false),
    m_ueCSilenceToNoPerm (false)
{ }

void
McpttSessionInitLoss::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();
  config->SetAppCount (3);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<TestFloorMachine> ueCMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (2));

  Ptr<McpttMediaSrc> ueAMediaSrc = ueAPttApp->GetMediaSrc ();

  Ptr<McpttMsgDropperImpl> ueBPttAppDropper = Create<McpttMsgDropperImpl> (2, McpttFloorMsgGranted::GetTypeId ());
  Ptr<McpttMsgDropperImpl> ueCPttAppDropper = Create<McpttMsgDropperImpl> (2, McpttFloorMsgGranted::GetTypeId ());

  ueBMachine->AddDropper (ueBPttAppDropper);
  ueCMachine->AddDropper (ueCPttAppDropper);

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (true));
  ueAMachine->SetOriginator (true);
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttSessionInitLoss::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttSessionInitLoss::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttSessionInitLoss::UeAStateChangeCb, this));
  ueAMediaSrc->SetAttribute ("DataRate", DataRateValue (DataRate ("64b/s")));
  ueAMediaSrc->SetAttribute ("Bytes", UintegerValue (8));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ());
  ueBMachine->SetPostTxCb (MakeCallback (&McpttSessionInitLoss::UeBTxCb, this));
  ueBMachine->SetPostRxCb (MakeCallback (&McpttSessionInitLoss::UeBRxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttSessionInitLoss::UeBStateChangeCb, this));

  ueCMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueCMachine->SetStartState (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ());
  ueCMachine->SetPostTxCb (MakeCallback (&McpttSessionInitLoss::UeCTxCb, this));
  ueCMachine->SetPostRxCb (MakeCallback (&McpttSessionInitLoss::UeCRxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttSessionInitLoss::UeCStateChangeCb, this));
}

void
McpttSessionInitLoss::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << ".");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueATxGrant, true, "UE A never sent the grant message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAStartStopToHasPerm, true, "UE A never made the transition from '" << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRtp, true, "UE B never saw the RTP message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBStartStopToSilence, true, "UE B never made the transition from '" << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBSilenceToNoPerm, true, "UE B never made the transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxRtp, true, "UE C never saw the RTP message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCStartStopToSilence, true, "UE C never made the transition from '" << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCSilenceToNoPerm, true, "UE C never made the transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" <<  McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
}

void
McpttSessionInitLoss::UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message: " << msg << ".");
}

void
McpttSessionInitLoss::UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAStartStopToHasPerm, false, "UE A already made transition from " << *oldState << " to " << *newState << ".");
      m_ueAStartStopToHasPerm = true;
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), true, "Timer " << *t206 << " is not running.");
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttSessionInitLoss::UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxGrant, false, "UE A already sent " << msg << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateStartStop::GetStateId (), "UE A in invalid state: " << *state << ".");
      m_ueATxGrant = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttSessionInitLoss::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRtp, false, "UE B already received " << msg << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE B not in '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "' state.");
      m_ueBRxRtp = true;
      if (m_ueCRxRtp == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttSessionInitLoss::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBStartStopToSilence, false, "UE B already made transition from " << *oldState << " to " << *newState << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBSilenceToNoPerm, false, "UE B already made transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
      m_ueBStartStopToSilence = true;
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBStartStopToSilence, true, "UE B never made transition from '" << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBSilenceToNoPerm, false, "UE B already made transition from " << *oldState << " to " << *newState << ".");
      m_ueBSilenceToNoPerm = true;
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttSessionInitLoss::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg << ".");
}

void
McpttSessionInitLoss::UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE A in invalid state: " << *state << ".");
      m_ueCRxRtp = true;
      if (m_ueBRxRtp == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttSessionInitLoss::UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCStartStopToSilence, false, "UE C already made transition from " << *oldState << " to " << *newState << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCSilenceToNoPerm, false, "UE C already made transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
      m_ueCStartStopToSilence = true;
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCStartStopToSilence, true, "UE C never made transition from '" << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCSilenceToNoPerm, false, "UE C already made transition from " << *oldState << " to " << *newState << ".");
      m_ueCSilenceToNoPerm = true;
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttSessionInitLoss::UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C SENT UNEXPECTED MESSAGE: " << msg << ".");
}

McpttFloorReleaseNormal::McpttFloorReleaseNormal (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueATxRelease (false),
    m_ueAHasPermToSilence (false),
    m_ueBRxRelease (false),
    m_ueBNoPermToSilence (false)
{ }

void
McpttFloorReleaseNormal::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();
  config->SetAppCount (2);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));

  Ptr<McpttTimer> ueAT206 = ueAMachine->GetT206 ();
  Ptr<McpttTimer> ueBT203 = ueBMachine->GetT203 ();

  Ptr<McpttPusher> ueAPusher = ueAPttApp->GetPusher ();

  ueAPusher->NotifyPushed ();
  ueAPusher->ScheduleRelease (Seconds (2.1));

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueAMachine->SetOriginator (false);
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttFloorReleaseNormal::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttFloorReleaseNormal::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleaseNormal::UeAStateChangeCb, this));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
  ueBMachine->SetPostTxCb (MakeCallback (&McpttFloorReleaseNormal::UeBTxCb, this));
  ueBMachine->SetPostRxCb (MakeCallback (&McpttFloorReleaseNormal::UeBRxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleaseNormal::UeBStateChangeCb, this));

  ueAMachine->SetCurrentSsrc (ueAMachine->GetTxSsrc ());
  ueBMachine->SetCurrentSsrc (ueAMachine->GetTxSsrc ());

  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueAT206);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueBT203);
}

void
McpttFloorReleaseNormal::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << ".");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent release message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAHasPermToSilence, true, "UE A never made transition from '" << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, true, "UE B never received release message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBNoPermToSilence, true, "UE B never made transition from '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
}

void
McpttFloorReleaseNormal::UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message: " << msg << ".");
}

void
McpttFloorReleaseNormal::UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAHasPermToSilence, false, "UE A already made transition from " << *oldState << " to " << *newState << ".");
      m_ueAHasPermToSilence = true;
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseNormal::UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, false, "UE A already sent " << msg << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateHasPerm::GetStateId (), "UE A in invalid state: " << *state << ".");
      m_ueATxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseNormal::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << msg << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateSilence::GetStateId (), "UE A in invalid state: " << *state << ".");
      m_ueBRxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseNormal::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBNoPermToSilence, false, "UE B already made transition from " << *oldState << " to " << *newState << ".");
      m_ueBNoPermToSilence = true;
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");

      Stop ();
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseNormal::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg << ".");
}

McpttFloorReleaseAfterReq::McpttFloorReleaseAfterReq (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueATxRelease (false),
    m_ueARxRtp (false),
    m_ueAPendReqToSilence (false),
    m_ueASilenceToNoPerm (false),
    m_ueBRxRelease (false),
    m_ueCRxRelease (false),
    m_ueCRxRtp (false)
{ }

void
McpttFloorReleaseAfterReq::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();
  config->SetAppCount (3);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());
  config->SetStart (Seconds (1.9));

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttMediaSrc> ueBMediaSrc = ueBPttApp->GetMediaSrc ();

  Ptr<McpttPusher> ueAPttAppPusher = ueAPttApp->GetPusher ();
  Ptr<McpttPusher> ueBPttAppPusher = ueBPttApp->GetPusher ();

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<TestFloorMachine> ueCMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (2));

  Ptr<McpttTimer> ueAT201 = ueAMachine->GetT201 ();
  Ptr<McpttTimer> ueAT203 = ueAMachine->GetT203 ();
  Ptr<McpttTimer> ueBT206 = ueBMachine->GetT206 ();
  Ptr<McpttTimer> ueCT203 = ueCMachine->GetT203 ();

  uint32_t ueAId = ueAPttApp->GetUserId ();

  Ptr<McpttFloorQueue> ueBMachineQueue = ueBMachine->GetQueue ();

  ueBMediaSrc->SetAttribute ("DataRate", DataRateValue (DataRate ("64b/s")));
  ueBMediaSrc->SetAttribute ("Bytes", UintegerValue (8));

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStatePendReq::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttFloorReleaseAfterReq::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttFloorReleaseAfterReq::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleaseAfterReq::UeAStateChangeCb, this));

  ueAT201->SetDelay (Seconds (0.160));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (true));
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
  ueBMachine->SetPostTxCb (MakeCallback (&McpttFloorReleaseAfterReq::UeBTxCb, this));
  ueBMachine->SetPostRxCb (MakeCallback (&McpttFloorReleaseAfterReq::UeBRxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleaseAfterReq::UeBStateChangeCb, this));

  ueCMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueCMachine->SetStartState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
  ueCMachine->SetPostTxCb (MakeCallback (&McpttFloorReleaseAfterReq::UeCTxCb, this));
  ueCMachine->SetPostRxCb (MakeCallback (&McpttFloorReleaseAfterReq::UeCRxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleaseAfterReq::UeCStateChangeCb, this));

  ueAMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());
  ueBMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());
  ueCMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());

  McpttQueuedUserInfo userInfo = McpttQueuedUserInfo (ueAId, McpttFloorMsgFieldQueuedUserId (ueAId), McpttFloorMsgFieldQueuePositionInfo (1, 7));

  ueBMachineQueue->SetAttribute ("Capacity", UintegerValue (1));
  ueBMachineQueue->Enqueue (userInfo);

  ueAPttAppPusher->NotifyPushed ();
  bool success = ueAPttAppPusher->ScheduleRelease (Seconds (2.041));
  if (!success)
    {
      NS_LOG_DEBUG ("release event must first be cancelled");
      ueAPttAppPusher->CancelRelease ();
      ueAPttAppPusher->ScheduleRelease (Seconds (2.041));
    }
  ueBPttAppPusher->NotifyPushed ();

  Simulator::Schedule (Seconds (1.9), &McpttTimer::Start, ueAT201);
  Simulator::Schedule (Seconds (1.9), &McpttTimer::Start, ueAT203);
  Simulator::Schedule (Seconds (1.9), &McpttTimer::Start, ueBT206);
  Simulator::Schedule (Seconds (1.9), &McpttTimer::Start, ueCT203);
}

void
McpttFloorReleaseAfterReq::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << ".");

  Simulator::Run ();

  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<McpttFloorQueue> ueBMachineQueue = ueBMachine->GetQueue (); //!< The floor queue.

  NS_TEST_ASSERT_MSG_EQ (ueBMachineQueue->GetCount (), 0, "UE B did not remove UE a from the queue.");

  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent release message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxRtp, true, "UE A never received RTP media.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAPendReqToSilence, true, "UE A never made transition from '" << McpttOffNetworkFloorParticipantStatePendGrant::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueASilenceToNoPerm, true, "UE A never made transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, true, "UE B never received release message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxRelease, true, "UE C never received release message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxRtp, true, "UE C never received RTP media.");
}

void
McpttFloorReleaseAfterReq::UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      if (state->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
        {
          m_ueARxRtp = true;
          NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
          if (m_ueCRxRtp == true)
            {
              Stop ();
            }
        }
      else if (state->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ())
        {
          // Do nothing.
          NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), true, "Timer " << *t201 << " is not running.");
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received RTP media in unexpected state.");
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseAfterReq::UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAPendReqToSilence, false, "UE A already made transition from " << *oldState << " to " << *newState << ".");
      m_ueAPendReqToSilence = true;
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAPendReqToSilence, true, "UE A never made transition from '" << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
      NS_TEST_ASSERT_MSG_EQ (m_ueASilenceToNoPerm, false, "UE A already made transition from " << *oldState << " to " << *newState << ".");
      m_ueASilenceToNoPerm = true;
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseAfterReq::UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received release message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxRelease, false, "UE C already received release message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, false, "UE A already sent release message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStatePendReq::GetStateId (), "UE A in invalid state: " << *state << ".");
      m_ueATxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseAfterReq::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent release message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received release message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateHasPerm::GetStateId (), "UE B in invalid state: " << *state << ".");
      m_ueBRxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), true, "Timer " << *t206 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseAfterReq::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorReleaseAfterReq::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected " << msg << ".");
}

void
McpttFloorReleaseAfterReq::UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent floor release message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxRelease, false, "UE C already recieved release message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C in invalid state: " << *state << ".");
      m_ueCRxRelease = true;
    }
  else if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      if (m_ueCRxRtp == true)
        {}
      else
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueCRxRtp, false, "UE C already received RTP media.");
          NS_TEST_ASSERT_MSG_EQ (m_ueCRxRelease, false, "UE C already received release message.");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C in invalid state: " << *state << ".");
          m_ueCRxRtp = true;
        }
      if (m_ueARxRtp == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseAfterReq::UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorReleaseAfterReq::UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C SENT UNEXPECTED MESSAGE: " << msg << ".");
}

McpttFloorReleaseDuringGrantWhileQueued::McpttFloorReleaseDuringGrantWhileQueued (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAExpT203 (0),
    m_ueANoPermToSilence (false),
    m_ueAQueuedToNoPerm (false),
    m_ueARxGrant (0),
    m_ueATxRelease (false),
    m_ueBExpT205 (0),
    m_ueBHasPermToPendGrant (false),
    m_ueBPendGrantToSilence (false),
    m_ueBRxRelease (false),
    m_ueBTxGrant (0),
    m_ueCExpT203 (false),
    m_ueCNoPermToSilence (0),
    m_ueCRxGrant (0),
    m_ueCRxRelease (false),
    m_ueCSilenceToNoPerm (false)
{ }

void
McpttFloorReleaseDuringGrantWhileQueued::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();
  config->SetAppCount (3);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttPusher> ueAPttAppPusher = ueAPttApp->GetPusher ();
  Ptr<McpttPusher> ueBPttAppPusher = ueBPttApp->GetPusher ();

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<TestFloorMachine> ueCMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (2));

  Ptr<McpttTimer> ueAT201 = ueAMachine->GetT201 ();
  Ptr<McpttTimer> ueAT203 = ueAMachine->GetT203 ();
  Ptr<McpttTimer> ueBT206 = ueBMachine->GetT206 ();
  Ptr<McpttTimer> ueBT205 = ueBMachine->GetT205 ();
  Ptr<McpttTimer> ueCT203 = ueCMachine->GetT203 ();

  uint32_t ueAId = ueAPttApp->GetUserId ();

  Ptr<McpttFloorQueue> ueBMachineQueue = ueBMachine->GetQueue ();

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStateQueued::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttFloorReleaseDuringGrantWhileQueued::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttFloorReleaseDuringGrantWhileQueued::UeARxCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&McpttFloorReleaseDuringGrantWhileQueued::UeATimerExpCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleaseDuringGrantWhileQueued::UeAStateChangeCb, this));

  ueAT201->SetDelay (Seconds (0.160));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueBMachine->SetAttribute ("C205", UintegerValue (2));
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
  ueBMachine->SetPostTxCb (MakeCallback (&McpttFloorReleaseDuringGrantWhileQueued::UeBTxCb, this));
  ueBMachine->SetPostRxCb (MakeCallback (&McpttFloorReleaseDuringGrantWhileQueued::UeBRxCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&McpttFloorReleaseDuringGrantWhileQueued::UeBTimerExpCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleaseDuringGrantWhileQueued::UeBStateChangeCb, this));

  ueBT205->SetDelay (Seconds (0.120));

  ueCMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueCMachine->SetStartState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
  ueCMachine->SetPostTxCb (MakeCallback (&McpttFloorReleaseDuringGrantWhileQueued::UeCTxCb, this));
  ueCMachine->SetPostRxCb (MakeCallback (&McpttFloorReleaseDuringGrantWhileQueued::UeCRxCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&McpttFloorReleaseDuringGrantWhileQueued::UeCTimerExpCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleaseDuringGrantWhileQueued::UeCStateChangeCb, this));

  ueAMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());
  ueBMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());
  ueCMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());

  McpttQueuedUserInfo userInfo = McpttQueuedUserInfo (ueAId, McpttFloorMsgFieldQueuedUserId (ueAId), McpttFloorMsgFieldQueuePositionInfo (1, 7));

  ueBMachineQueue->SetAttribute ("Capacity", UintegerValue (1));
  ueBMachineQueue->Enqueue (userInfo);

  Simulator::Schedule (Seconds (2.037), &McpttFloorParticipant::ReleaseRequest, ueAMachine);

  ueBPttAppPusher->NotifyPushed ();
  bool success = ueBPttAppPusher->ScheduleRelease (Seconds (2.036));
  if (!success)
    {
      ueBPttAppPusher->CancelRelease ();
      ueBPttAppPusher->ScheduleRelease (Seconds (2.036));
    }
  ueBPttAppPusher->SetAttribute ("Automatic", BooleanValue (false));

  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueAT203);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueBT206);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueCT203);
}

void
McpttFloorReleaseDuringGrantWhileQueued::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << ".");

  Simulator::Run ();

  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<McpttFloorQueue> ueBMachineQueue = ueBMachine->GetQueue (); //!< The floor queue.

  NS_TEST_ASSERT_MSG_EQ (ueBMachineQueue->GetCount (), 0, "UE B did not remove UE a from the queue.");

  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAExpT203, true, "UE A timer T203 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent release message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQueuedToNoPerm, true, "UE A never made transition from '" << McpttOffNetworkFloorParticipantStateQueued::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxGrant, 2, "UE A never received 3 grant messages.");
  NS_TEST_ASSERT_MSG_EQ (m_ueANoPermToSilence, true, "UE A never made transition from '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxGrant, 2, "UE B never sent 2 grant messages.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBHasPermToPendGrant, true, "UE B never made transition from '" << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStatePendGrant::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, true, "UE B never received a release message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBPendGrantToSilence, true, "UE B never made transition from '" << McpttOffNetworkFloorParticipantStatePendGrant::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCExpT203, true, "UE C timer T203 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCNoPermToSilence, 2, "UE C never made transition from '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' 2 times.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCSilenceToNoPerm, true, "UE C never made transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrant, 2, "UE C never received 3 grant messages.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxRelease, true, "UE C never received release message.");
}

void
McpttFloorReleaseDuringGrantWhileQueued::UeATimerExpCb (const TestFloorMachine& machine, Ptr<McpttTimer>  timer)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (timer == machine.GetT203 ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpT203, false, "UE A timer " << *timer << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxGrant, 2, "UE A never received 3 floor grant message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateSilence::GetStateId (), "UE A in invalid state: " << *state << ".");
      m_ueAExpT203 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A timer " << *timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseDuringGrantWhileQueued::UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_LT (m_ueARxGrant, 3, "UE A received too many grant messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent release message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE A in invalid state: " << *state << ".");
      m_ueARxGrant += 1;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseDuringGrantWhileQueued::UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateQueued::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAQueuedToNoPerm, false, "UE A already made transition from '" << oldState->GetInstanceStateId () << "' to '" << newState->GetInstanceStateId () << "'.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent release message.");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
      m_ueAQueuedToNoPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueANoPermToSilence, false, "UE A already made transition from '" << oldState->GetInstanceStateId () << "' to '" << newState->GetInstanceStateId () << "'.");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxGrant, 2, "UE A never received 3 grant messages.");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
      m_ueANoPermToSilence = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseDuringGrantWhileQueued::UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, false, "UE A already sent floor release message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateQueued::GetStateId (), "UE A in invalid state: " << *state << ".");
      m_ueATxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A unexpectedly sent a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseDuringGrantWhileQueued::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received floor release message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxGrant, 1, "UE B never sent first floor granted message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStatePendGrant::GetStateId (), "UE B in invalid state: " << *state << ".");
      m_ueBRxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), true, "Timer " << *t205 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseDuringGrantWhileQueued::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendGrant::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBHasPermToPendGrant, false, "UE B already made transition from '" << oldState->GetInstanceStateId () << "' to '" << newState->GetInstanceStateId () << "'.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxGrant, 1, "UE B never sent first grant message.");
      NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), true, "Timer " << *t205 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
      m_ueBHasPermToPendGrant = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendGrant::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBPendGrantToSilence, false, "UE B already made transition from '" << oldState->GetInstanceStateId () << "' to '" << newState->GetInstanceStateId () << "'.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxGrant, 2, "UE B never sent 2 grant messages.");
      NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
      m_ueBPendGrantToSilence = true;
      if (m_ueCNoPermToSilence == 2)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseDuringGrantWhileQueued::UeBTimerExpCb (const TestFloorMachine& machine, Ptr<McpttTimer>  timer)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (timer == machine.GetT205 ())
    {
      if (m_ueBExpT205 == 1)
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, true, "UE B never received floor release message.");
          NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
          NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateSilence::GetStateId (), "UE B in invalid state: " << *state << ".");
          NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
          m_ueBExpT205 += 1;
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueBExpT205, c205->GetValue () - 2, "UE B timer " << *timer << " expired too many times.");
          NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, true, "UE B never received floor release message.");
          NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), true, "Timer " << *t205 << " is not running.");
          NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStatePendGrant::GetStateId (), "UE B in invalid state: " << *state << ".");
          m_ueBExpT205 += 1;
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B timer " << *timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
}

void
McpttFloorReleaseDuringGrantWhileQueued::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_LT (m_ueBTxGrant, 3, "UE B sent too many floor grant message.");
      if (m_ueBTxGrant == 0)
        {
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateHasPerm::GetStateId (), "UE B in invalid state: " << *state << ".");
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStatePendGrant::GetStateId (), "UE B in invalid state: " << *state << ".");
        }
      m_ueBTxGrant += 1;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), true, "Timer " << *t205 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), m_ueBTxGrant, "Counter C205 is not one.");
}

void
McpttFloorReleaseDuringGrantWhileQueued::UeCTimerExpCb (const TestFloorMachine& machine, Ptr<McpttTimer>  timer)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (timer == machine.GetT203 ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCExpT203, false, "UE C timer " << *timer << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrant, 2, "UE C never received 3 floor grant message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateSilence::GetStateId (), "UE C in invalid state: " << *state << ".");
      m_ueCExpT203 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C timer " << *timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseDuringGrantWhileQueued::UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_LT (m_ueCRxGrant, 3, "UE C received too many floor grant message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C in invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
      m_ueCRxGrant += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxRelease, false, "UE C already received floor release message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateSilence::GetStateId (), "UE C in invalid state: " << *state << ".");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
      m_ueCRxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseDuringGrantWhileQueued::UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_LT (m_ueCNoPermToSilence, 2, "UE C made too many transitions from '" << oldState->GetInstanceStateId () << "' to '" << newState->GetInstanceStateId () << "'.");
      NS_TEST_ASSERT_MSG_GT (m_ueCRxGrant, 0, "UE C never received any granted message.");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), true, "Timer " << *t230 << " is not running.");
      m_ueCNoPermToSilence += 1;
      if (m_ueCNoPermToSilence == 2
          && m_ueBPendGrantToSilence == true)
        {
          Stop ();
        }
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCSilenceToNoPerm, false, "UE C already made transition from '" << oldState->GetInstanceStateId () << "' to '" << newState->GetInstanceStateId () << "'.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrant, 1, "UE C never received 3 grant messages.");
      NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
      m_ueCSilenceToNoPerm = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseDuringGrantWhileQueued::UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C unexpectedly sent a " << msg << " message.");
}

McpttFloorReleaseWhileQueued::McpttFloorReleaseWhileQueued (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueATxRelease (false),
    m_ueAQueuedToNoPerm (false),
    m_ueBRxRelease (false),
    m_ueCRxRelease (false)
{ }

void
McpttFloorReleaseWhileQueued::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();
  config->SetAppCount (3);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());
  config->SetStart (Seconds (1.9));

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttPusher> ueAPttAppPusher = ueAPttApp->GetPusher ();
  Ptr<McpttPusher> ueBPttAppPusher = ueBPttApp->GetPusher ();

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<TestFloorMachine> ueCMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (2));

  Ptr<McpttTimer> ueAT203 = ueAMachine->GetT203 ();
  Ptr<McpttTimer> ueBT206 = ueBMachine->GetT206 ();
  Ptr<McpttTimer> ueCT203 = ueCMachine->GetT203 ();

  uint32_t ueAId = ueAPttApp->GetUserId ();

  Ptr<McpttFloorQueue> ueBMachineQueue = ueBMachine->GetQueue ();

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStateQueued::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttFloorReleaseWhileQueued::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttFloorReleaseWhileQueued::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleaseWhileQueued::UeAStateChangeCb, this));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
  ueBMachine->SetPostTxCb (MakeCallback (&McpttFloorReleaseWhileQueued::UeBTxCb, this));
  ueBMachine->SetPostRxCb (MakeCallback (&McpttFloorReleaseWhileQueued::UeBRxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleaseWhileQueued::UeBStateChangeCb, this));

  ueCMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueCMachine->SetStartState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
  ueCMachine->SetPostTxCb (MakeCallback (&McpttFloorReleaseWhileQueued::UeCTxCb, this));
  ueCMachine->SetPostRxCb (MakeCallback (&McpttFloorReleaseWhileQueued::UeCRxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleaseWhileQueued::UeCStateChangeCb, this));

  ueAMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());
  ueBMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());
  ueCMachine->SetCurrentSsrc (ueBMachine->GetTxSsrc ());

  McpttQueuedUserInfo userInfo = McpttQueuedUserInfo (ueAId, McpttFloorMsgFieldQueuedUserId (ueAId), McpttFloorMsgFieldQueuePositionInfo (1, 7));

  ueBMachineQueue->SetAttribute ("Capacity", UintegerValue (1));
  ueBMachineQueue->Enqueue (userInfo);

  Simulator::Schedule (Seconds (2.037), &McpttFloorParticipant::ReleaseRequest, ueAMachine);

  Simulator::Schedule (Seconds (1.9), &McpttTimer::Start, ueAT203);
  Simulator::Schedule (Seconds (1.9), &McpttTimer::Start, ueBT206);
  Simulator::Schedule (Seconds (1.9), &McpttTimer::Start, ueCT203);

  ueBPttAppPusher->NotifyPushed ();
}

void
McpttFloorReleaseWhileQueued::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << ".");

  Simulator::Run ();

  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<McpttFloorQueue> ueBMachineQueue = ueBMachine->GetQueue (); //!< The floor queue.

  NS_TEST_ASSERT_MSG_EQ (ueBMachineQueue->GetCount (), 0, "UE B did not remove UE a from the queue.");

  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent release message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQueuedToNoPerm, true, "UE A never made transition from '" << McpttOffNetworkFloorParticipantStateQueued::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, true, "UE B never received release message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxRelease, true, "UE C never received release message.");
}

void
McpttFloorReleaseWhileQueued::UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message.");
}

void
McpttFloorReleaseWhileQueued::UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateQueued::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAQueuedToNoPerm, false, "UE A already made transition from " << *oldState << " to " << *newState << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent release message.");
      m_ueAQueuedToNoPerm = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseWhileQueued::UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, false, "UE A already sent release message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateQueued::GetStateId (), "UE A in invalid state: " << *state << ".");
      m_ueATxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseWhileQueued::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received release message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateHasPerm::GetStateId (), "UE B in invalid state: " << *state << ".");
      m_ueBRxRelease = true;
      if (m_ueCRxRelease == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), false, "Timer " << *t203 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), true, "Timer " << *t206 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseWhileQueued::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made unexpected state transition from " << *oldState << " to " << *newState << ".");
}

void
McpttFloorReleaseWhileQueued::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected " << msg << ".");
}

void
McpttFloorReleaseWhileQueued::UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  Ptr<McpttOffNetworkFloorParticipantState> state = machine.GetState ();

  Ptr<McpttTimer> t201 = machine.GetT201 ();
  Ptr<McpttTimer> t203 = machine.GetT203 ();
  Ptr<McpttTimer> t204 = machine.GetT204 ();
  Ptr<McpttTimer> t205 = machine.GetT205 ();
  Ptr<McpttTimer> t206 = machine.GetT206 ();
  Ptr<McpttTimer> t207 = machine.GetT207 ();
  Ptr<McpttTimer> t230 = machine.GetT230 ();
  Ptr<McpttTimer> t233 = machine.GetT233 ();

  Ptr<McpttCounter> c201 = machine.GetC201 ();
  Ptr<McpttCounter> c204 = machine.GetC204 ();
  Ptr<McpttCounter> c205 = machine.GetC205 ();

  if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxRelease, false, "UE C already recieved release message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttOffNetworkFloorParticipantStateNoPerm::GetStateId (), "UE C in invalid state: " << *state << ".");
      m_ueCRxRelease = true;
      if (m_ueBRxRelease == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C unexpectedly received a " << msg << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (t201->IsRunning (), false, "Timer " << *t201 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t203->IsRunning (), true, "Timer " << *t203 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (t204->IsRunning (), false, "Timer " << *t204 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t205->IsRunning (), false, "Timer " << *t205 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t206->IsRunning (), false, "Timer " << *t206 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t207->IsRunning (), false, "Timer " << *t207 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t230->IsRunning (), false, "Timer " << *t230 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (t233->IsRunning (), false, "Timer " << *t233 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (c201->GetValue (), 1, "Counter C201 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c204->GetValue (), 1, "Counter C204 is not one.");
  NS_TEST_ASSERT_MSG_EQ (c205->GetValue (), 1, "Counter C205 is not one.");
}

void
McpttFloorReleaseWhileQueued::UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made unexpected state transition from " << *oldState << " to " << *newState << ".");
}

void
McpttFloorReleaseWhileQueued::UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C SENT UNEXPECTED MESSAGE: " << msg << ".");
}

McpttFloorRequestIdleBis::McpttFloorRequestIdleBis (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAHasPermToPendGrant (false),
    m_ueAPendReqToHasPerm (false),
    m_ueARxQueuePosInfoReq (false),
    m_ueARxReq (0),
    m_ueASilenceToStartStop (false),
    m_ueAStartStopToPendReq (false),
    m_ueATxGranted (false),
    m_ueATxReq (0),
    m_ueATxTaken (false),
    m_ueATxQueuePosInfo (0),
    m_ueBRxGranted (false),
    m_ueBRxQueuePosInfo (0),
    m_ueBRxReq (0),
    m_ueBSilenceToStartStop (false),
    m_ueBStartStopToNoPerm (false),
    m_ueBRxQueuePosInfoReq (false),
    m_ueBRxTaken (false),
    m_ueBRxRtp (0),
    m_ueCPendReqToQueued (false),
    m_ueCQueuedToSilence (false),
    m_ueCRxGranted (false),
    m_ueCRxQueuePosInfo (0),
    m_ueCRxReq (0),
    m_ueCRxRtp (0),
    m_ueCRxTaken (false),
    m_ueCStartStopToPendReq (false),
    m_ueCSilenceToStartStop (false),
    m_ueCTxReq (0),
    m_ueCTxQueuePosInfoReq (false)
{ }

void
McpttFloorRequestIdleBis::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();

  config->SetAppCount (3);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<TestFloorMachine> ueCMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (2));

  Ptr<McpttMediaSrc> ueAMediaSrc = ueAPttApp->GetMediaSrc ();

  Ptr<McpttTimer> ueAT201 = ueAMachine->GetT201 ();
  Ptr<McpttTimer> ueAT205 = ueAMachine->GetT205 ();
  Ptr<McpttTimer> ueAT230 = ueAMachine->GetT230 ();
  Ptr<McpttTimer> ueBT230 = ueBMachine->GetT230 ();
  Ptr<McpttTimer> ueCT201 = ueCMachine->GetT201 ();
  Ptr<McpttTimer> ueCT204 = ueCMachine->GetT204 ();
  Ptr<McpttTimer> ueCT230 = ueCMachine->GetT230 ();
  Ptr<McpttTimer> ueCT233 = ueCMachine->GetT233 ();

  Ptr<McpttFloorQueue> ueAQueue = ueAMachine->GetQueue ();
  Ptr<McpttFloorQueue> ueCQueue = ueCMachine->GetQueue ();

  ueAPttApp->SetUserId (ueCPttApp->GetUserId () + 1);

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (true));
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestIdleBis::UeATxCb, this));
  ueAMachine->SetPreRxCb (MakeCallback (&McpttFloorRequestIdleBis::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestIdleBis::UeAStateChangeCb, this));

  ueAMediaSrc->SetAttribute ("DataRate", DataRateValue (DataRate ("64b/s")));
  ueAMediaSrc->SetAttribute ("Bytes", UintegerValue (8));

  ueAT201->SetDelay (Seconds (0.160));
  ueAT205->SetDelay (Seconds (0.120));
  ueAT230->SetDelay (Seconds (0.050));

  ueAQueue->SetAttribute ("Capacity", UintegerValue (1));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestIdleBis::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestIdleBis::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestIdleBis::UeBStateChangeCb, this));

  ueBT230->SetDelay (Seconds (0.050));

  ueCPttApp->SetFloorGrantedCb ((MakeCallback (&McpttFloorRequestIdleBis::UeCFloorGrantedCb, this)));

  ueCMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueCMachine->SetStartState (McpttOffNetworkFloorParticipantStateSilence::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttFloorRequestIdleBis::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttFloorRequestIdleBis::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorRequestIdleBis::UeCStateChangeCb, this));

  ueCT201->SetDelay (Seconds (0.160));
  ueCT204->SetDelay (Seconds (0.160));
  ueCT230->SetDelay (Seconds (0.050));
  ueCT233->SetDelay (Seconds (0.050));

  ueCQueue->SetAttribute ("Capacity", UintegerValue (1));

  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueAT230);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueBT230);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueCT230);
  Simulator::Schedule (Seconds (2.1), &McpttFloorParticipant::PttPush, ueCMachine);
  Simulator::Schedule (Seconds (2.14), &McpttFloorParticipant::PttPush, ueAMachine);
  Simulator::Schedule (Seconds (3.0), &McpttFloorParticipant::SendFloorQueuePositionRequest, ueCMachine);
  Simulator::Schedule (Seconds (4.0), &McpttFloorParticipant::PttRelease, ueAMachine);
}

void
McpttFloorRequestIdleBis::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAHasPermToPendGrant, true, "UE A never made transition from " << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << " to " << McpttOffNetworkFloorParticipantStatePendGrant::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAPendReqToHasPerm, true, "UE A never made transition from " << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxQueuePosInfoReq, true, "UE A never received " << McpttFloorMsgQueuePositionInfo::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxReq, 3, "UE A never received 3 " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueASilenceToStartStop, true, "UE A never made transition from " << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAStartStopToPendReq, true, "UE A never made transition from " << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << " to " << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxGranted, true, "UE A never sent 2 " << McpttFloorMsgGranted::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, 3, "UE A never sent 3 " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxTaken, true, "UE A never sent " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxQueuePosInfo, 2, "UE A never sent 2 " << McpttFloorMsgQueuePositionInfo::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxGranted, true, "UE B never received " << McpttFloorMsgGranted::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxQueuePosInfo, 2, "UE B never received 2 " << McpttFloorMsgQueuePositionInfo::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, 6, "UE B never received 6 " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBSilenceToStartStop, true, "UE B never made transition from " << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBStartStopToNoPerm, true, "UE B never made transition from " << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxQueuePosInfoReq, true, "UE B never received " << McpttFloorMsgQueuePositionRequest::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxTaken, true, "UE B never received " << McpttFloorMsgTaken::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRtp, 2, "UE B received more than 2 " << McpttMediaMsg::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCPendReqToQueued, true, "UE C never made transition from " << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateQueued::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCQueuedToSilence, true, "UE C never made transition from " << McpttOffNetworkFloorParticipantStateQueued::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxGranted, true, "UE C never received " << McpttFloorMsgGranted::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxQueuePosInfo, 2, "UE C never received 2 " << McpttFloorMsgQueuePositionInfo::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxReq, 3, "UE C never received 3 " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxRtp, 2, "UE C never received 2 " << McpttMediaMsg::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxTaken, true, "UE C never received " << McpttFloorMsgTaken::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCStartStopToPendReq, true, "UE C never made transition from " << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << " to " << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCSilenceToStartStop, true, "UE C never made transition from " << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCTxReq, 3, "UE C never sent 3 " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCTxQueuePosInfoReq, true, "UE C never sent " << McpttFloorMsgQueuePositionRequest::GetTypeId ().GetName () << ".");
}

void
McpttFloorRequestIdleBis::UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueARxReq += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgQueuePositionRequest::GetTypeId ())
    {
      m_ueARxQueuePosInfoReq = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId ().GetName () << ".");
    }
}

void
McpttFloorRequestIdleBis::UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      m_ueASilenceToStartStop = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ())
    {
      m_ueAStartStopToPendReq = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ())
    {
      m_ueAPendReqToHasPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendGrant::GetStateId ())
    {
      m_ueAHasPermToPendGrant = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestIdleBis::UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueATxReq += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgTaken::GetTypeId ())
    {
      m_ueATxTaken = true;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgQueuePositionInfo::GetTypeId ())
    {
      m_ueATxQueuePosInfo += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      m_ueATxGranted = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << ".");
    }
}

void
McpttFloorRequestIdleBis::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueBRxReq += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgTaken::GetTypeId ())
    {
      m_ueBRxTaken = true;
    }
  else if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      m_ueBRxRtp += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgQueuePositionRequest::GetTypeId ())
    {
      m_ueBRxQueuePosInfoReq = true;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgQueuePositionInfo::GetTypeId ())
    {
      m_ueBRxQueuePosInfo += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      m_ueBRxGranted = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected " << msg.GetInstanceTypeId ().GetName () << ".");
    }
}

void
McpttFloorRequestIdleBis::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      m_ueBSilenceToStartStop = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      m_ueBStartStopToNoPerm = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestIdleBis::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected " << msg << ".");
}

void
McpttFloorRequestIdleBis::UeCFloorGrantedCb (void)
{
  // Do nothing.
}

void
McpttFloorRequestIdleBis::UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueCRxReq += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgTaken::GetTypeId ())
    {
      m_ueCRxTaken = true;
    }
  else if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      m_ueCRxRtp += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgQueuePositionInfo::GetTypeId ())
    {
      m_ueCRxQueuePosInfo += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      m_ueCRxGranted = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected " << msg.GetInstanceTypeId ().GetName () << ".");
    }
}

void
McpttFloorRequestIdleBis::UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ())
    {
      m_ueCSilenceToStartStop = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ())
    {
      m_ueCStartStopToPendReq = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateQueued::GetStateId ())
    {
      m_ueCPendReqToQueued = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateQueued::GetStateId () && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      m_ueCQueuedToSilence = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorRequestIdleBis::UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueCTxReq += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgQueuePositionRequest::GetTypeId ())
    {
      m_ueCTxQueuePosInfoReq = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected " << msg.GetInstanceTypeId () << ".");
    }
}

McpttSessionInitPrivateCall::McpttSessionInitPrivateCall (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAHasPermToSilence (false),
    m_ueARxReq (false),
    m_ueASilenceToPendGrant (false),
    m_ueAStartStopToHasPerm (false),
    m_ueATxGrant (0),
    m_ueATxRelease (false),
    m_ueBNoPermToSilence (false),
    m_ueBPendReqToHasPerm (false),
    m_ueBSilenceToPendReq (false),
    m_ueBStartStopToNoPerm (false),
    m_ueBRxGrant (0),
    m_ueBRxRtp (false),
    m_ueBRxRelease (false),
    m_ueBTxReq (false)
{ }

void
McpttSessionInitPrivateCall::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();
  config->SetAppCount (2);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));

  Ptr<McpttMediaSrc> ueAMediaSrc = ueAPttApp->GetMediaSrc ();

  Ptr<McpttCallMachineNull> ueACallMachine = DynamicCast<McpttCallMachineNull, McpttCallMachine> (ueAMachine->GetCall ()->GetCallMachine ());
  Ptr<McpttCallMachineNull> ueBCallMachine = DynamicCast<McpttCallMachineNull, McpttCallMachine> (ueBMachine->GetCall ()->GetCallMachine ());

  Ptr<McpttTimer> ueAT205 = ueAMachine->GetT205 ();
  Ptr<McpttTimer> ueBT201 = ueBMachine->GetT201 ();

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (true));
  ueAMachine->SetOriginator (true);
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttSessionInitPrivateCall::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttSessionInitPrivateCall::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttSessionInitPrivateCall::UeAStateChangeCb, this));

  ueAMediaSrc->SetAttribute ("DataRate", DataRateValue (DataRate ("64b/s")));
  ueAMediaSrc->SetAttribute ("Bytes", UintegerValue (8));

  ueACallMachine->SetAttribute ("CallType", UintegerValue (McpttCallMsgFieldCallType::PRIVATE));

  ueAT205->SetDelay (Seconds (0.240));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueBMachine->SetOriginator (false);
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateStartStop::GetInstance ());
  ueBMachine->SetPostTxCb (MakeCallback (&McpttSessionInitPrivateCall::UeBTxCb, this));
  ueBMachine->SetPostRxCb (MakeCallback (&McpttSessionInitPrivateCall::UeBRxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttSessionInitPrivateCall::UeBStateChangeCb, this));

  ueBCallMachine->SetAttribute ("CallType", UintegerValue (McpttCallMsgFieldCallType::PRIVATE));

  ueBT201->SetDelay (Seconds (0.160));

  Simulator::Schedule (Seconds (2.1), &McpttFloorParticipant::PttRelease, ueAMachine);
  Simulator::Schedule (Seconds (2.2), &McpttFloorParticipant::PttPush, ueBMachine);
}

void
McpttSessionInitPrivateCall::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << ".");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAHasPermToSilence, true, "UE A never made the transition from '" << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxReq, true, "UE A never received " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueASilenceToPendGrant, true, "UE A never made the transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStatePendGrant::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAStartStopToHasPerm, true, "UE A never made the transition from '" << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxGrant, 2, "UE A never sent 2 " << McpttFloorMsgGranted::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent " << McpttFloorMsgRelease::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBNoPermToSilence, true, "UE B never made the transition from '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBPendReqToHasPerm, true, "UE B never made the transition from '" << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBSilenceToPendReq, true, "UE B never made the transition from '" << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBStartStopToNoPerm, true, "UE B never made the transition from '" << McpttOffNetworkFloorParticipantStateStartStop::GetStateId () << "' to '" << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << "'.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrant, 2, "UE B never received 2 " << McpttFloorMsgGranted::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRtp, true, "UE B never received " << McpttMediaMsg::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, true, "UE B never received " << McpttFloorMsgRelease::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxReq, true, "UE B never sent " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");

}

void
McpttSessionInitPrivateCall::UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueARxReq = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg << ".");
    }
}

void
McpttSessionInitPrivateCall::UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ())
    {
      m_ueAStartStopToHasPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      m_ueAHasPermToSilence = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendGrant::GetStateId ())
    {
      m_ueASilenceToPendGrant = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttSessionInitPrivateCall::UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      m_ueATxGrant += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      m_ueATxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected message: " << msg << ".");
    }
}

void
McpttSessionInitPrivateCall::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      m_ueBRxGrant += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttMediaMsg::GetTypeId ())
    {
      m_ueBRxRtp = true;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      m_ueBRxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B unexpectedly received " << msg << ".");
    }
}

void
McpttSessionInitPrivateCall::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateStartStop::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      m_ueBStartStopToNoPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      m_ueBNoPermToSilence = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ())
    {
      m_ueBSilenceToPendReq = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ())
    {
      m_ueBPendReqToHasPerm = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttSessionInitPrivateCall::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueBTxReq = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B unexpectedly sent " << msg << ".");
    }
}

McpttFloorReleasePreemptedFloorArbitrator::McpttFloorReleasePreemptedFloorArbitrator (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAHasPermToPendGrant (false),
    m_ueAPendGrantToSilence (false),
    m_ueARxDeny (false),
    m_ueARxRelease (false),
    m_ueARxReq (0),
    m_ueATxDeny (false),
    m_ueATxGrant (0),
    m_ueBNoPermToPendReq (false),
    m_ueBPendReqToHasPerm (false),
    m_ueBHasPermToSilence (false),
    m_ueBRxDeny (false),
    m_ueBRxGrant (0),
    m_ueBRxReq (false),
    m_ueBTxDeny (false),
    m_ueBTxReq (false),
    m_ueBTxRelease (false),
    m_ueCNoPermToPendReq (false),
    m_ueCPendReqToNoPerm (false),
    m_ueCNoPermToSilence (false),
    m_ueCSilenceToNoPerm (false),
    m_ueCRxDeny (0),
    m_ueCRxRelease (false),
    m_ueCRxReq (false),
    m_ueCRxGrant (0),
    m_ueCTxReq (false)
{ }

void
McpttFloorReleasePreemptedFloorArbitrator::Configure (void)
{
  Ptr<McpttTestCaseConfig> config = GetConfig ();

  config->SetAppCount (3);
  config->SetFloorTid (TestFloorMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<TestFloorMachine> ueAMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (0));
  Ptr<TestFloorMachine> ueBMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (1));
  Ptr<TestFloorMachine> ueCMachine = DynamicCast<TestFloorMachine, McpttFloorParticipant> (GetFloorMachine (2));

  Ptr<McpttTimer> ueAT201 = ueAMachine->GetT201 ();
  Ptr<McpttTimer> ueAT205 = ueAMachine->GetT205 ();
  Ptr<McpttTimer> ueAT206 = ueAMachine->GetT206 ();
  Ptr<McpttTimer> ueBT201 = ueBMachine->GetT201 ();
  Ptr<McpttTimer> ueBT203 = ueBMachine->GetT203 ();
  Ptr<McpttTimer> ueBT205 = ueBMachine->GetT205 ();
  Ptr<McpttTimer> ueCT201 = ueCMachine->GetT201 ();
  Ptr<McpttTimer> ueCT203 = ueCMachine->GetT203 ();
  Ptr<McpttTimer> ueCT205 = ueCMachine->GetT205 ();

  ueAMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueAMachine->SetStartState (McpttOffNetworkFloorParticipantStateHasPerm::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttFloorReleasePreemptedFloorArbitrator::UeATxCb, this));
  ueAMachine->SetPreRxCb (MakeCallback (&McpttFloorReleasePreemptedFloorArbitrator::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleasePreemptedFloorArbitrator::UeAStateChangeCb, this));
  ueAMachine->SetCurrentSsrc (ueAMachine->GetTxSsrc ());

  ueAT205->SetDelay (Seconds (0.120));

  ueBMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueBMachine->SetStartState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttFloorReleasePreemptedFloorArbitrator::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttFloorReleasePreemptedFloorArbitrator::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleasePreemptedFloorArbitrator::UeBStateChangeCb, this));
  ueBMachine->SetCurrentSsrc (ueAMachine->GetTxSsrc ());
  ueBMachine->SetPriority (7);

  ueBT201->SetDelay (Seconds (0.160));
  ueBT205->SetDelay (Seconds (0.120));

  ueCMachine->SetAttribute ("GenMedia", BooleanValue (false));
  ueCMachine->SetStartState (McpttOffNetworkFloorParticipantStateNoPerm::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttFloorReleasePreemptedFloorArbitrator::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttFloorReleasePreemptedFloorArbitrator::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttFloorReleasePreemptedFloorArbitrator::UeCStateChangeCb, this));
  ueCMachine->SetCurrentSsrc (ueAMachine->GetTxSsrc ());

  ueCT201->SetDelay (Seconds (0.160));
  ueCT205->SetDelay (Seconds (0.120));

  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueAT206);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueBT203);
  Simulator::Schedule (Seconds (2.0), &McpttTimer::Start, ueCT203);
  Simulator::Schedule (Seconds (2.1), &McpttFloorParticipant::PttPush, ueBMachine);
  Simulator::Schedule (Seconds (2.2), &McpttFloorParticipant::PttPush, ueCMachine);
  Simulator::Schedule (Seconds (2.3), &McpttFloorParticipant::PttRelease, ueBMachine);
}

void
McpttFloorReleasePreemptedFloorArbitrator::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAHasPermToPendGrant, true, "UE A never made transition from " << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << " to " << McpttOffNetworkFloorParticipantStatePendGrant::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAPendGrantToSilence, true, "UE A never made transition from " << McpttOffNetworkFloorParticipantStatePendGrant::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxDeny, true, "UE A never received " << McpttFloorMsgDeny::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxRelease, true, "UE A never received " << McpttFloorMsgRelease::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxReq, 2, "UE A never received 2 " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxDeny, true, "UE A never sent " << McpttFloorMsgDeny::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxGrant, 4, "UE A never sent 4 " << McpttFloorMsgGranted::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBNoPermToPendReq, true, "UE B never made transition from " << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << " to " << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBPendReqToHasPerm, true, "UE B never made transition from " << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBHasPermToSilence, true, "UE B never made transition from " << McpttOffNetworkFloorParticipantStateHasPerm::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxDeny, true, "UE B never received " << McpttFloorMsgDeny::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrant, 4, "UE B never received 4 " << McpttFloorMsgGranted::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, true, "UE B never received " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxDeny, true, "UE B never sent " << McpttFloorMsgDeny::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxReq, true, "UE B never sent " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxRelease, true, "UE B never sent " << McpttFloorMsgRelease::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCNoPermToPendReq, true, "UE C never made transition from " << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << " to " << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCPendReqToNoPerm, true, "UE C never made transition from " << McpttOffNetworkFloorParticipantStatePendReq::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCNoPermToSilence, true, "UE C never made transition from " << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCSilenceToNoPerm, true, "UE C never made transition from " << McpttOffNetworkFloorParticipantStateSilence::GetStateId () << " to " << McpttOffNetworkFloorParticipantStateNoPerm::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxRelease, true, "UE A never received " << McpttFloorMsgRelease::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxDeny, 2, "UE C never received 2 " << McpttFloorMsgDeny::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxReq, true, "UE C never received " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrant, 4, "UE C never received 4 " << McpttFloorMsgGranted::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCTxReq, true, "UE C never sent " << McpttFloorMsgRequest::GetTypeId ().GetName () << ".");
}

void
McpttFloorReleasePreemptedFloorArbitrator::UeARxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueARxReq += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgDeny::GetTypeId ())
    {
      m_ueARxDeny = true;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      m_ueARxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId ().GetName () << ".");
    }
}

void
McpttFloorReleasePreemptedFloorArbitrator::UeAStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendGrant::GetStateId ())
    {
      m_ueAHasPermToPendGrant = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendGrant::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      m_ueAPendGrantToSilence = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorReleasePreemptedFloorArbitrator::UeATxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      m_ueATxGrant += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgDeny::GetTypeId ())
    {
      m_ueATxDeny = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << ".");
    }
}

void
McpttFloorReleasePreemptedFloorArbitrator::UeBRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueBRxReq = true;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgDeny::GetTypeId ())
    {
      m_ueBRxDeny = true;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      m_ueBRxGrant += 1;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected " << msg.GetInstanceTypeId ().GetName () << ".");
    }
}

void
McpttFloorReleasePreemptedFloorArbitrator::UeBStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ())
    {
      m_ueBNoPermToPendReq = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ())
    {
      m_ueBPendReqToHasPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateHasPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      m_ueBHasPermToSilence = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorReleasePreemptedFloorArbitrator::UeBTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueBTxReq = true;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgDeny::GetTypeId ())
    {
      m_ueBTxDeny = true;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      m_ueBTxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected " << msg.GetInstanceTypeId ().GetName () << ".");
    }
}

void
McpttFloorReleasePreemptedFloorArbitrator::UeCRxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueCRxReq = true;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgGranted::GetTypeId ())
    {
      m_ueCRxGrant += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgDeny::GetTypeId ())
    {
      m_ueCRxDeny += 1;
    }
  else if (msg.GetInstanceTypeId () == McpttFloorMsgRelease::GetTypeId ())
    {
      m_ueCRxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected " << msg.GetInstanceTypeId ().GetName () << ".");
    }
}

void
McpttFloorReleasePreemptedFloorArbitrator::UeCStateChangeCb (const TestFloorMachine& machine, Ptr<McpttOffNetworkFloorParticipantState>  oldState, Ptr<McpttOffNetworkFloorParticipantState>  newState)
{
  if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
      && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ())
    {
      m_ueCNoPermToPendReq = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStatePendReq::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      m_ueCPendReqToNoPerm = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ())
    {
      m_ueCNoPermToSilence = true;
    }
  else if (oldState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateSilence::GetStateId ()
           && newState->GetInstanceStateId () == McpttOffNetworkFloorParticipantStateNoPerm::GetStateId ())
    {
      m_ueCSilenceToNoPerm = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }
}

void
McpttFloorReleasePreemptedFloorArbitrator::UeCTxCb (const TestFloorMachine& machine, const McpttMsg& msg)
{
  if (msg.GetInstanceTypeId () == McpttFloorMsgRequest::GetTypeId ())
    {
      m_ueCTxReq = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected " << msg << ".");
    }
}

McpttFloorControlTestSuite::McpttFloorControlTestSuite (void)
  : TestSuite ("mcptt-floor-control", TestSuite::SYSTEM)
{
  AddTestCase (new McpttFloorRequestDenied (), TestCase::QUICK);
  AddTestCase (new McpttFloorRequestIdle (), TestCase::QUICK);
  AddTestCase (new McpttFloorRequestPreemptive (), TestCase::QUICK);
  AddTestCase (new McpttFloorRequestQueued (), TestCase::QUICK);
  AddTestCase (new McpttSessionInitNormal (), TestCase::QUICK);
  AddTestCase (new McpttSessionInitLoss (), TestCase::QUICK);
  AddTestCase (new McpttFloorReleaseNormal (), TestCase::QUICK);
  AddTestCase (new McpttFloorReleaseAfterReq (), TestCase::QUICK);
  AddTestCase (new McpttFloorReleaseDuringGrantWhileQueued (), TestCase::QUICK);
  AddTestCase (new McpttFloorReleaseWhileQueued (), TestCase::QUICK);
  AddTestCase (new McpttFloorRequestIdleBis (), TestCase::QUICK);
  AddTestCase (new McpttSessionInitPrivateCall (), TestCase::QUICK);
  AddTestCase (new McpttFloorReleasePreemptedFloorArbitrator (), TestCase::QUICK);
}

} // namespace tests
} // namespace psc
} // namespace ns3

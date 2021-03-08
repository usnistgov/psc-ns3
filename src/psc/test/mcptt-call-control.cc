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
#include <ns3/mcptt-call-machine-grp-basic.h>
#include <ns3/mcptt-call-msg.h>
#include <ns3/mcptt-ptt-app.h>

#include "mcptt-test-call.h"
#include "mcptt-test-case.h"
#include "mcptt-test-case-config.h"
#include "mcptt-msg-dropper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallControlTest");
namespace psc {
namespace tests {

class TestCallMachine : public McpttCallMachineGrpBasic
{
public:
  static TypeId GetTypeId (void);
  TestCallMachine (void);
  virtual ~TestCallMachine (void);
  virtual void ChangeState (Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Receive (const McpttCallMsg& msg);
  virtual void Start (void);
  virtual void Send (const McpttCallMsg& msg);

protected:
  virtual void ExpiryOfTfg1 (void);
  virtual void ExpiryOfTfg2 (void);
  virtual void ExpiryOfTfg3 (void);
  virtual void ExpiryOfTfg4 (void);
  virtual void ExpiryOfTfg5 (void);
  virtual void ExpiryOfTfg6 (void);

private:
  Callback<void, const TestCallMachine&, const McpttCallMsg&> m_postRxCb;
  Callback<void, const TestCallMachine&, const McpttTimer&> m_postTimerExpCb;
  Callback<void, const TestCallMachine&, const McpttCallMsg&> m_postTxCb;
  Callback<void, const TestCallMachine&, const McpttCallMsg&> m_preRxCb;
  Callback<void, const TestCallMachine&, const McpttTimer&> m_preTimerExpCb;
  Callback<void, const TestCallMachine&, const McpttCallMsg&> m_preTxCb;
  Ptr<McpttCallMachineGrpBasicState> m_startState;
  Callback<void, const TestCallMachine&, Ptr<McpttCallMachineGrpBasicState>, Ptr<McpttCallMachineGrpBasicState> > m_stateChangeCb;

public:
  virtual Callback<void, const TestCallMachine&, const McpttCallMsg&> GetPostRxCb (void) const;
  virtual Callback<void, const TestCallMachine&, const McpttTimer&> GetPostTimerExpCb (void) const;
  virtual Callback<void, const TestCallMachine&, const McpttCallMsg&> GetPostTxCb (void) const;
  virtual Callback<void, const TestCallMachine&, const McpttCallMsg&> GetPreRxCb (void) const;
  virtual Callback<void, const TestCallMachine&, const McpttTimer&> GetPreTimerExpCb (void) const;
  virtual Callback<void, const TestCallMachine&, const McpttCallMsg&> GetPreTxCb (void) const;
  virtual Ptr<McpttCallMachineGrpBasicState> GetStartState (void) const;
  virtual Callback<void, const TestCallMachine&, Ptr<McpttCallMachineGrpBasicState>, Ptr<McpttCallMachineGrpBasicState> > GetStateChangeCb (void) const;
  virtual void SetPostRxCb (const Callback<void, const TestCallMachine&, const McpttCallMsg&>  postRxCb);
  virtual void SetPostTimerExpCb (const Callback<void, const TestCallMachine&, const McpttTimer&>  timerExpCb);
  virtual void SetPostTxCb (const Callback<void, const TestCallMachine&, const McpttCallMsg&>  postTxCb);
  virtual void SetPreRxCb (const Callback<void, const TestCallMachine&, const McpttCallMsg&>  preRxCb);
  virtual void SetPreTimerExpCb (const Callback<void, const TestCallMachine&, const McpttTimer&>  timerExpCb);
  virtual void SetPreTxCb (const Callback<void, const TestCallMachine&, const McpttCallMsg&>  preTxCb);
  virtual void SetStartState (Ptr<McpttCallMachineGrpBasicState>  startState);
  virtual void SetStateChangeTestCb (const Callback<void, const TestCallMachine&, Ptr<McpttCallMachineGrpBasicState>, Ptr<McpttCallMachineGrpBasicState> >  stateChangeCb);
};

class McpttCallSetupJoinCall : public McpttTestCase
{
public:
  McpttCallSetupJoinCall (const std::string& name = "Call Setup - Join Call", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueARxAnnoun;
  bool m_ueAS1ToS2;
  bool m_ueAS2ToS3;
  bool m_ueATxProbe;
  bool m_ueBTfg2Exp;
  bool m_ueBRxProbe;
  bool m_ueBTxAnnoun;
  bool m_ueCTfg2Exp;
  bool m_ueCRxProbe;
  bool m_ueCTxAnnoun;
};

class McpttCallSetupNewCall : public McpttTestCase
{
public:
  McpttCallSetupNewCall (const std::string& name = "Call Setup - New Call", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);

private:
  uint32_t m_ueARxAcceptCount;
  bool m_ueAS1ToS2;
  bool m_ueAS2ToS3;
  uint32_t m_ueATfg1ExpCount;
  uint32_t m_ueATfg3ExpCount;
  uint32_t m_ueATxProbeCount;
  bool m_ueATxAnnoun;
  bool m_ueBRxAnnoun;
  uint32_t m_ueBRxProbeCount;
  bool m_ueBS1ToS3;
  bool m_ueBTxAccept;
  uint32_t m_ueCRxProbeCount;
  bool m_ueCS1ToS3;
  bool m_ueCRxAnnoun;
  bool m_ueCTxAccept;
};

class McpttCallMerge : public McpttTestCase
{
public:
  McpttCallMerge (const std::string& name = "Call Merge", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueATfg2Exp;
  bool m_ueATxAnnoun;
  bool m_ueARxAnnoun;
  bool m_ueBTfg2Exp;
  bool m_ueBTxAnnoun;
  uint32_t m_ueBRxAnnoun;
  bool m_ueCS1ToS2;
  bool m_ueCS2ToS3;
  bool m_ueCRxAnnoun;
  bool m_ueCTfg1Exp;
  uint32_t m_ueCTfg3Exp;
  uint32_t m_ueCTxProbe;
  bool m_ueCTxAnnoun;
};

class McpttCallRelease : public McpttTestCase
{
public:
  McpttCallRelease (const std::string& name = "Call Release", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAS3ToS6;
  bool m_ueAS6ToS1;
  bool m_ueARxAnnoun;
  bool m_ueATfg5Exp;
  bool m_ueBTfg2Exp;
  bool m_ueBTxAnnoun;
};

class McpttCallReleaseAfterProbe : public McpttTestCase
{
public:
  McpttCallReleaseAfterProbe (const std::string& name = "Call Release - After Probe", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAS2ToS7;
  bool m_ueAS7ToS1;
  bool m_ueATfg1Exp;
};

class McpttCallReleasePendingUserAction1 : public McpttTestCase
{
public:
  McpttCallReleasePendingUserAction1 (const std::string& name = "Call Release - Pending User Action 1", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAS4ToS6;
  bool m_ueAS6ToS3;
};

class McpttCallReleasePendingUserAction2 : public McpttTestCase
{
public:
  McpttCallReleasePendingUserAction2 (const std::string& name = "Call Release - Pending User Action 2", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());
  ~McpttCallReleasePendingUserAction2 (void);

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueARxAnnoun;
  bool m_ueAS5ToS6;
  bool m_ueAS6ToS1;
  bool m_ueATfg5Exp;
  double* m_ueBRndVals;
  bool m_ueBTfg2Exp;
  bool m_ueBTxAnnoun;
  bool m_ueCRxAnnoun;
};

class McpttCallReleaseMaxDuration : public McpttTestCase
{
public:
  McpttCallReleaseMaxDuration (const std::string& name = "Call Release - Max Duration", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAS3ToS6;
  bool m_ueAS6ToS1;
  bool m_ueATfg5Exp;
  bool m_ueATfg6Exp;
};

class McpttCallReleaseAndSetup : public McpttTestCase
{
public:
  McpttCallReleaseAndSetup (const std::string& name = "Call Release - And Setup", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAS1ToS2;
  bool m_ueAS2ToS7;
  bool m_ueAS7ToS2;
  uint32_t m_ueATxProbe;
};

class McpttCallReject : public McpttTestCase
{
public:
  McpttCallReject (const std::string& name = "Call Reject", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBNewCallCb (uint16_t callId);
  virtual void UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCNewCallCb (uint16_t callId);
  virtual void UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState);
  virtual void UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAS1ToS2;
  bool m_ueAS2ToS3;
  uint32_t m_ueATfg1ExpCount;
  uint32_t m_ueATfg3ExpCount;
  uint32_t m_ueATxProbeCount;
  bool m_ueATxAnnoun;
  bool m_ueBRxAnnoun;
  uint32_t m_ueBRxProbeCount;
  bool m_ueBS1ToS4;
  bool m_ueBS4ToS6;
  bool m_ueCRxAnnoun;
  uint32_t m_ueCRxProbeCount;
  bool m_ueCS1ToS4;
  bool m_ueCS4ToS6;
  bool m_ueCTfg4Exp;
};

class McpttCallControlTestSuite : public TestSuite
{
public:
  McpttCallControlTestSuite (void);
};

/***************************************************************
 *  Implementation of the code declared above.
 ***************************************************************/

static McpttCallControlTestSuite callSuite;

TypeId
TestCallMachine::GetTypeId (void)
{
  static TypeId tid = TypeId ("TestCallMachine")
    .SetParent<McpttCallMachineGrpBasic> ()
    .AddConstructor<TestCallMachine> ()
  ;

  return tid;
}

TestCallMachine::TestCallMachine (void)
  : McpttCallMachineGrpBasic (),
    m_postRxCb (MakeNullCallback<void, const TestCallMachine&, const McpttCallMsg&> ()),
    m_postTimerExpCb (MakeNullCallback<void, const TestCallMachine&, const McpttTimer&> ()),
    m_postTxCb (MakeNullCallback<void, const TestCallMachine&, const McpttCallMsg&> ()),
    m_preRxCb (MakeNullCallback<void, const TestCallMachine&, const McpttCallMsg&> ()),
    m_preTimerExpCb (MakeNullCallback<void, const TestCallMachine&, const McpttTimer&> ()),
    m_preTxCb (MakeNullCallback<void, const TestCallMachine&, const McpttCallMsg&> ()),
    m_startState (McpttCallMachineGrpBasicStateS1::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const TestCallMachine&, Ptr<McpttCallMachineGrpBasicState>, Ptr<McpttCallMachineGrpBasicState> > ())
{ }

TestCallMachine::~TestCallMachine (void)
{ }

void
TestCallMachine::Receive (const McpttCallMsg& msg)
{
  Callback<void, const TestCallMachine&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestCallMachine&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallMachineGrpBasic::Receive (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
TestCallMachine::Send (const McpttCallMsg& msg)
{
  Callback<void, const TestCallMachine&, const McpttCallMsg&> postTxCb = GetPostTxCb ();
  Callback<void, const TestCallMachine&, const McpttCallMsg&> preTxCb = GetPreTxCb ();

  if (!preTxCb.IsNull ())
    {
      preTxCb (*this, msg);
    }

  McpttCallMachineGrpBasic::Send (msg);

  if (!postTxCb.IsNull ())
    {
      postTxCb (*this, msg);
    }
}

void
TestCallMachine::ExpiryOfTfg1 (void)
{
  Ptr<McpttTimer> tfg = GetTfg1 ();
  Callback<void, const TestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachineGrpBasic::ExpiryOfTfg1 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
TestCallMachine::ExpiryOfTfg2 (void)
{
  Ptr<McpttTimer> tfg = GetTfg2 ();
  Callback<void, const TestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachineGrpBasic::ExpiryOfTfg2 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
TestCallMachine::ExpiryOfTfg3 (void)
{
  Ptr<McpttTimer> tfg = GetTfg3 ();
  Callback<void, const TestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachineGrpBasic::ExpiryOfTfg3 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
TestCallMachine::ExpiryOfTfg4 (void)
{
  Ptr<McpttTimer> tfg = GetTfg4 ();
  Callback<void, const TestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachineGrpBasic::ExpiryOfTfg4 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
TestCallMachine::ExpiryOfTfg5 (void)
{
  Ptr<McpttTimer> tfg = GetTfg5 ();
  Callback<void, const TestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachineGrpBasic::ExpiryOfTfg5 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
TestCallMachine::ExpiryOfTfg6 (void)
{
  Ptr<McpttTimer> tfg = GetTfg6 ();
  Callback<void, const TestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachineGrpBasic::ExpiryOfTfg6 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
TestCallMachine::Start (void)
{
  Ptr<McpttCallMachineGrpBasicState> startState = GetStartState ();

  SetState (startState);

  McpttCallMachineGrpBasic::Start ();
}

void
TestCallMachine::ChangeState (Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttCallMachineGrpBasicState> oldState = GetState ();
  Callback<void, const TestCallMachine&, Ptr<McpttCallMachineGrpBasicState>, Ptr<McpttCallMachineGrpBasicState> > stateChangeCb = GetStateChangeCb ();

  McpttCallMachineGrpBasic::ChangeState (newState);

  if (!stateChangeCb.IsNull ())
    {
      stateChangeCb (*this, oldState, newState);
    }
}

TypeId
TestCallMachine::GetInstanceTypeId (void) const
{
  return TestCallMachine::GetTypeId ();
}

Callback<void, const TestCallMachine&, const McpttCallMsg&>
TestCallMachine::GetPostRxCb (void) const
{
  return m_postRxCb;
}

Callback<void, const TestCallMachine&, const McpttTimer&>
TestCallMachine::GetPostTimerExpCb (void) const
{
  return m_postTimerExpCb;
}


Callback<void, const TestCallMachine&, const McpttCallMsg&>
TestCallMachine::GetPostTxCb (void) const
{
  return m_postTxCb;
}

Callback<void, const TestCallMachine&, const McpttCallMsg&>
TestCallMachine::GetPreRxCb (void) const
{
  return m_preRxCb;
}

Callback<void, const TestCallMachine&, const McpttTimer&>
TestCallMachine::GetPreTimerExpCb (void) const
{
  return m_preTimerExpCb;
}

Callback<void, const TestCallMachine&, const McpttCallMsg&>
TestCallMachine::GetPreTxCb (void) const
{
  return m_preTxCb;
}

Ptr<McpttCallMachineGrpBasicState>
TestCallMachine::GetStartState (void) const
{
  return m_startState;
}

Callback<void, const TestCallMachine&, Ptr<McpttCallMachineGrpBasicState>, Ptr<McpttCallMachineGrpBasicState> >
TestCallMachine::GetStateChangeCb (void) const
{
  return m_stateChangeCb;
}

void
TestCallMachine::SetPostRxCb (const Callback<void, const TestCallMachine&, const McpttCallMsg&>  postRxCb)
{
  m_postRxCb = postRxCb;
}

void
TestCallMachine::SetPostTimerExpCb (const Callback<void, const TestCallMachine&, const McpttTimer&>  timerExpCb)
{
  m_postTimerExpCb = timerExpCb;
}

void
TestCallMachine::SetPostTxCb (const Callback<void, const TestCallMachine&, const McpttCallMsg&>  postTxCb)
{
  m_postTxCb = postTxCb;
}

void
TestCallMachine::SetPreRxCb (const Callback<void, const TestCallMachine&, const McpttCallMsg&>  preRxCb)
{
  m_preRxCb = preRxCb;
}

void
TestCallMachine::SetPreTimerExpCb (const Callback<void, const TestCallMachine&, const McpttTimer&>  timerExpCb)
{
  m_preTimerExpCb = timerExpCb;
}

void
TestCallMachine::SetPreTxCb (const Callback<void, const TestCallMachine&, const McpttCallMsg&>  preTxCb)
{
  m_preTxCb = preTxCb;
}

void
TestCallMachine::SetStartState (Ptr<McpttCallMachineGrpBasicState>  startState)
{
  m_startState = startState;
}

void
TestCallMachine::SetStateChangeTestCb (const Callback<void, const TestCallMachine&, Ptr<McpttCallMachineGrpBasicState>, Ptr<McpttCallMachineGrpBasicState> >  stateChangeCb)
{
  m_stateChangeCb = stateChangeCb;
}


McpttCallSetupJoinCall::McpttCallSetupJoinCall (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueARxAnnoun (false),
    m_ueAS1ToS2 (false),
    m_ueAS2ToS3 (false),
    m_ueATxProbe (false),
    m_ueBTfg2Exp (false),
    m_ueBRxProbe (false),
    m_ueBTxAnnoun (false),
    m_ueCTfg2Exp (false),
    m_ueCRxProbe (false),
    m_ueCTxAnnoun (false)
{ }

void
McpttCallSetupJoinCall::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (TestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<TestCallMachine> ueAMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<TestCallMachine> ueBMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<TestCallMachine> ueCMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueBTypeMachine = ueBMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();

  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueCTypeMachine = ueCMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
  Time joinTime = Seconds (2.2);
  uint32_t origId = ueBPttApp->GetUserId ();
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  Address origAddress = ueBPttApp->GetLocalAddress ();

  ueBPttApp->GetSelectedCall ()->GetAttribute ("PeerAddress", grpAddress);

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress);
  sdp.SetSpeechPort (speechPort);

  // UE A
  ueAMachine->SetAttribute ("TFG3", TimeValue (Seconds (1.0)));
  ueAMachine->SetAttribute ("TFG1", TimeValue (Seconds (1.0)));
  ueAMachine->SetGrpId (grpId);
  ueAMachine->SetStartState (McpttCallMachineGrpBasicStateS1::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttCallSetupJoinCall::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttCallSetupJoinCall::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttCallSetupJoinCall::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&McpttCallSetupJoinCall::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttCallSetupJoinCall::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttCallSetupJoinCall::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttCallSetupJoinCall::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&McpttCallSetupJoinCall::UeBTimerExpCb, this));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueBTypeMachine->SetStarted (true);

  Time ueBTfg2Delay = Seconds (0.25); //ueBMachine->CalcDelayForPeriodicAnnoun ();
  Time ueBTfg6Delay = ueBMachine->CalcDelayForMaxDuration ();
  ueBTfg2->SetDelay (ueBTfg2Delay);
  ueBTfg6->SetDelay (ueBTfg6Delay);

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttCallSetupJoinCall::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttCallSetupJoinCall::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttCallSetupJoinCall::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&McpttCallSetupJoinCall::UeCTimerExpCb, this));

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueCTypeMachine->SetStarted (true);

  Time ueCTfg2Delay = Seconds (0.275); //ueCMachine->CalcDelayForPeriodicAnnoun ();
  Time ueCTfg6Delay = ueCMachine->CalcDelayForMaxDuration ();
  ueCTfg2->SetDelay (ueCTfg2Delay);
  ueCTfg6->SetDelay (ueCTfg6Delay);

  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (joinTime, &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
McpttCallSetupJoinCall::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A did not receive an announcement message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A did not make transistion from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, true, "UE A did not make transistion from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxProbe, true, "UE A did not send a probe message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTfg2Exp, true, "UE B TFG2 did not expire.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxProbe, true, "UE B did not receive a probe message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, true, "UE B did not send an announcement message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCTfg2Exp, true, "UE C TFG2 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbe, true, "UE C did not receive a probe message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCTxAnnoun, true, "UE C did not send an announcement message.");
}

void
McpttCallSetupJoinCall::UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallSetupJoinCall::UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId ());
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId ());
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbe, true, "UE A never sent a " << McpttCallMsgGrpProbe::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "Already recieved a " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      m_ueARxAnnoun = true;
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A TFG1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A TFG3 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A TFG4 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A TFG5 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A TFG6 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE A not in " << McpttCallMachineGrpBasicStateS3::GetStateId ());
      if (m_ueBTfg2Exp == true
          && m_ueCTfg2Exp == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }
}

void
McpttCallSetupJoinCall::UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbe, true, "UE never sent " << McpttCallMsgGrpProbe::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE already received " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, false, "UE A already made transition from " << *oldState << " to " << *newState << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A TFG1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A TFG2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE A TFG3 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A TFG6 is running.");

      m_ueAS1ToS2 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS2::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbe, true, "UE never sent " << McpttCallMsgGrpProbe::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A hasn't made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << *oldState << " to " << *newState << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A TFG1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A TFG3 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A TFG6 is not running.");
      m_ueAS2ToS3 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A TFG5 is running.");
}

void
McpttCallSetupJoinCall::UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpProbe::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbe, false, "UE A already sent " << McpttCallMsgGrpProbe::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE already received " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A TFG1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE A TFG3 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A TFG6 is running.");
      m_ueATxProbe = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A TFG2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A TFG5 is running.");
}

void
McpttCallSetupJoinCall::UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg2 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBTfg2Exp, false, "UE B timer " << timer << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxProbe, true, "UE B never recieved " << McpttCallMsgGrpProbe::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, true, "UE B never sent " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B timer TFG6 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueBTfg2Exp = true;
      if (m_ueCTfg2Exp == true
          && m_ueARxAnnoun == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
}

void
McpttCallSetupJoinCall::UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpProbe::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBTfg2Exp, false, "UE B timer TFG2 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxProbe, false, "UE B already recieved " << McpttCallMsgGrpProbe::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, false, "UE B already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B timer TFG6 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE B not in state " << McpttCallMachineGrpBasicStateS3::GetStateId ());
      m_ueBRxProbe = true;
    }
  else if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      //Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
}

void
McpttCallSetupJoinCall::UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBTfg2Exp, false, "UE B timer TFG2 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxProbe, true, "UE B never recieved " << McpttCallMsgGrpProbe::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, false, "UE B already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B timer TFG6 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE B not in state " << McpttCallMachineGrpBasicStateS3::GetStateId ());
      m_ueBTxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetInstanceTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
}

void
McpttCallSetupJoinCall::UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << *oldState << " to " << *newState << ".");
}

void
McpttCallSetupJoinCall::UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg2 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg2Exp, false, "UE C timer " << timer << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbe, true, "UE C never recieved " << McpttCallMsgGrpProbe::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAnnoun, true, "UE C never sent " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE C timer TFG6 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE C in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueCTfg2Exp = true;
      if (m_ueBTfg2Exp == true
          && m_ueARxAnnoun == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE C timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
}

void
McpttCallSetupJoinCall::UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpProbe::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg2Exp, false, "UE C timer TFG2 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbe, false, "UE C already recieved " << McpttCallMsgGrpProbe::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAnnoun, false, "UE C already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE C timer TFG6 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE C not in state " << McpttCallMachineGrpBasicStateS3::GetStateId ());
      m_ueCRxProbe = true;
    }
  else if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      //Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
}

void
McpttCallSetupJoinCall::UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg2Exp, false, "UE C timer TFG2 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbe, true, "UE C never recieved " << McpttCallMsgGrpProbe::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAnnoun, false, "UE C already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE C timer TFG6 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE C not in state " << McpttCallMachineGrpBasicStateS3::GetStateId ());
      m_ueCTxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetInstanceTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE C timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
}

void
McpttCallSetupJoinCall::UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << *oldState << " to " << *newState << ".");
}

McpttCallSetupNewCall::McpttCallSetupNewCall (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueARxAcceptCount (0),
    m_ueAS1ToS2 (false),
    m_ueAS2ToS3 (false),
    m_ueATfg1ExpCount (0),
    m_ueATfg3ExpCount (0),
    m_ueATxProbeCount (0),
    m_ueATxAnnoun (false),
    m_ueBRxAnnoun (false),
    m_ueBRxProbeCount (0),
    m_ueBS1ToS3 (false),
    m_ueBTxAccept (false),
    m_ueCRxProbeCount (0),
    m_ueCS1ToS3 (false),
    m_ueCRxAnnoun (false),
    m_ueCTxAccept (false)
{ }

void
McpttCallSetupNewCall::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (TestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);

  Ptr<TestCallMachine> ueAMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<TestCallMachine> ueBMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<TestCallMachine> ueCMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (2));

  Time startTime = Seconds (2.1);

  // UE A
  ueAMachine->SetStartState (McpttCallMachineGrpBasicStateS1::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttCallSetupNewCall::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttCallSetupNewCall::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttCallSetupNewCall::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&McpttCallSetupNewCall::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetStartState (McpttCallMachineGrpBasicStateS1::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttCallSetupNewCall::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttCallSetupNewCall::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttCallSetupNewCall::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&McpttCallSetupNewCall::UeBTimerExpCb, this));

  // UE C
  ueCMachine->SetStartState (McpttCallMachineGrpBasicStateS1::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttCallSetupNewCall::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttCallSetupNewCall::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttCallSetupNewCall::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&McpttCallSetupNewCall::UeCTimerExpCb, this));

  Simulator::Schedule (startTime, &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
McpttCallSetupNewCall::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueARxAcceptCount, 2, "UE A recieved " << m_ueARxAcceptCount << " " << McpttCallMsgGrpAccept::GetTypeId () << " messages.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 1, "UE A TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
  NS_TEST_ASSERT_MSG_EQ (m_ueATfg3ExpCount, 3, "UE A TFG1 expired " << m_ueATfg3ExpCount << " time(s).");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxProbeCount, 4, "UE A TFG1 expired " << m_ueATxProbeCount << " time(s).");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent send " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, true, "UE B never recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxProbeCount, 4, "UE B recieved " << m_ueBRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBS1ToS3, true, "UE B never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgGrpAccept::GetTypeId () << " message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, true, "UE C never recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbeCount, 4, "UE C recieved " << m_ueCRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS3, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCTxAccept, true, "UE C never sent " << McpttCallMsgGrpAccept::GetTypeId () << " message.");
}

void
McpttCallSetupNewCall::UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg1 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 0, "UE A timer TFG1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbeCount, 4, "UE A sent " << m_ueATxProbeCount << " message(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAcceptCount, 0, "UE A already recieved " << McpttCallMsgGrpAccept::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg3ExpCount, 3, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A timer TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A timer TFG6 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " instead of " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueATfg1ExpCount += 1;
    }
  else if (timer.GetId () == machine.GetTfg3 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 0, "UE A timer TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_LT (m_ueATxProbeCount, 5, "UE A sent " << m_ueATxProbeCount << " message(s).");
      NS_TEST_ASSERT_MSG_LT (m_ueATfg3ExpCount, 4, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAcceptCount, 0, "UE A already recieved " << McpttCallMsgGrpAccept::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, false, "UE A already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A timer TFG1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE A timer TFG3 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      m_ueATfg3ExpCount += 1;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
}

void
McpttCallSetupNewCall::UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_LT (m_ueARxAcceptCount, 2, "UE A recieved " << m_ueARxAcceptCount << " " << McpttCallMsgGrpAccept::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbeCount, 4, "UE A sent " << m_ueATxProbeCount << " message(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 1, "UE A timer TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg3ExpCount, 3, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A timer TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A timer TFG6 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueARxAcceptCount += 1;

      if (m_ueARxAcceptCount == 2)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
}

void
McpttCallSetupNewCall::UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbeCount, 1, "UE A sent " << m_ueATxProbeCount << " message(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAcceptCount, 0, "UE A recieved " << m_ueARxAcceptCount << " " << McpttCallMsgGrpAccept::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 0, "UE A timer TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg3ExpCount, 0, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, false, "UE A already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A timer TFG1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE A timer TFG3 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
      m_ueAS1ToS2 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS2::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbeCount, 4, "UE A sent " << m_ueATxProbeCount << " message(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAcceptCount, 0, "UE A recieved " << m_ueARxAcceptCount << " " << McpttCallMsgGrpAccept::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 0, "UE A timer TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_LT (m_ueATfg3ExpCount, m_ueATxProbeCount, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A timer TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A timer TFG6 is not running.");
      m_ueAS2ToS3 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
}

void
McpttCallSetupNewCall::UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpProbe::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_GT_OR_EQ (m_ueATxProbeCount, m_ueATfg3ExpCount, "UE A sent " << m_ueATxProbeCount << " message(s).");
      if (m_ueATxProbeCount == 0)
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
        }
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAcceptCount, 0, "UE A recieved " << m_ueARxAcceptCount << " " << McpttCallMsgGrpAccept::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 0, "UE A timer TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_LT_OR_EQ (m_ueATfg3ExpCount, m_ueATxProbeCount, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, false, "UE A already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A timer TFG1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE A timer TFG3 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
      m_ueATxProbeCount += 1;
    }
  else if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbeCount, 4, "UE A sent " << m_ueATxProbeCount << " message(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAcceptCount, 0, "UE A recieved " << m_ueARxAcceptCount << " " << McpttCallMsgGrpAccept::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 0, "UE A timer TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg3ExpCount, m_ueATxProbeCount - 1, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, false, "UE A already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A timer TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A timer TFG6 is not running.");
      m_ueATxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
}

void
McpttCallSetupNewCall::UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B timer " << timer.GetId () << " unexpectedly expired..");
}

void
McpttCallSetupNewCall::UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpProbe::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE B already recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_LT (m_ueBRxProbeCount, 4, "UE B recieved " << m_ueBRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBS1ToS3, false, "UE B already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B already sent " << McpttCallMsgGrpAccept::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer " << tfg1->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer " << tfg3->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE B timer " << tfg2->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE B timer " << tfg6->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS1::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      m_ueBRxProbeCount += 1;
    }
  else if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE B already recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxProbeCount, 4, "UE B recieved " << m_ueBRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBS1ToS3, true, "UE B never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgGrpAccept::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer " << tfg1->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer " << tfg3->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B timer " << tfg2->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B timer " << tfg6->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueBRxAnnoun = true;
    }
  else if (msg.IsA (McpttCallMsgGrpAccept::GetTypeId ()))
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
}

void
McpttCallSetupNewCall::UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE B already recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxProbeCount, 4, "UE B recieved " << m_ueBRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBS1ToS3, false, "UE B already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B already sent " << McpttCallMsgGrpAccept::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer " << tfg1->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer " << tfg3->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B timer " << tfg2->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B timer " << tfg6->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS1::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      m_ueBTxAccept = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
}

void
McpttCallSetupNewCall::UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE B never recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxProbeCount, 4, "UE B recieved " << m_ueBRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBS1ToS3, false, "UE B already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B already sent " << McpttCallMsgGrpAccept::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer " << tfg1->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer " << tfg3->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B timer " << tfg2->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B timer " << tfg6->GetId () << " is not running.");
      m_ueBS1ToS3 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
}

void
McpttCallSetupNewCall::UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C timer " << timer.GetId () << " unexpectedly expired..");
}

void
McpttCallSetupNewCall::UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpProbe::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_LT (m_ueCRxProbeCount, 4, "UE C recieved " << m_ueCRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS3, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAccept, false, "UE C already sent " << McpttCallMsgGrpAccept::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer " << tfg1->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer " << tfg3->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE C timer " << tfg2->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE C timer " << tfg6->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS1::GetStateId (), "UE C in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      m_ueCRxProbeCount += 1;
    }
  else if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbeCount, 4, "UE C recieved " << m_ueCRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS3, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAccept, true, "UE C never sent " << McpttCallMsgGrpAccept::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer " << tfg1->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer " << tfg3->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE C timer " << tfg2->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE C timer " << tfg6->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE C in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueCRxAnnoun = true;
    }
  else if (msg.IsA (McpttCallMsgGrpAccept::GetTypeId ()))
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
}

void
McpttCallSetupNewCall::UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbeCount, 4, "UE C recieved " << m_ueCRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS3, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAccept, false, "UE C already sent " << McpttCallMsgGrpAccept::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer " << tfg1->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer " << tfg3->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE C timer " << tfg2->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE C timer " << tfg6->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS1::GetStateId (), "UE C in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      m_ueCTxAccept = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
}

void
McpttCallSetupNewCall::UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbeCount, 4, "UE C recieved " << m_ueCRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS3, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAccept, true, "UE C already sent " << McpttCallMsgGrpAccept::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer " << tfg1->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer " << tfg3->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE C timer " << tfg2->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE C timer " << tfg6->GetId () << " is not running.");
      m_ueCS1ToS3 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
}

McpttCallMerge::McpttCallMerge (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueATfg2Exp (false),
    m_ueATxAnnoun (false),
    m_ueARxAnnoun (false),
    m_ueBTfg2Exp (false),
    m_ueBTxAnnoun (false),
    m_ueBRxAnnoun (0),
    m_ueCS1ToS2 (false),
    m_ueCS2ToS3 (false),
    m_ueCRxAnnoun (false),
    m_ueCTfg1Exp (false),
    m_ueCTfg3Exp (0),
    m_ueCTxProbe (0),
    m_ueCTxAnnoun (false)
{ }

void
McpttCallMerge::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (TestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttTestCall> ueACall = ueAPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();
  Ptr<McpttTestCall> ueBCall = ueBPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();

  Ptr<TestCallMachine> ueAMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<TestCallMachine> ueBMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<TestCallMachine> ueCMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueATfg6 = ueAMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueATypeMachine = ueAMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueBTypeMachine = ueBMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
  Time joinTime = Seconds (2.2);
  uint32_t origId = ueBPttApp->GetUserId ();
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  Address origAddress = ueBPttApp->GetLocalAddress ();

  ueBPttApp->GetSelectedCall ()->GetAttribute ("PeerAddress", grpAddress);

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress);
  sdp.SetSpeechPort (speechPort);

  // UE A
  ueACall->AddDropper (Create<McpttMsgDropperImpl> (5, McpttCallMsgGrpProbe::GetTypeId ()));
  ueACall->AddDropper (Create<McpttMsgDropperImpl> (0, McpttCallMsgGrpAnnoun::GetTypeId ()));

  ueAMachine->SetCallId (callId);
  ueAMachine->SetGrpId (grpId);
  ueAMachine->SetOrigId (origId);
  ueAMachine->SetRefInt (refInt);
  ueAMachine->SetSdp (sdp);
  ueAMachine->SetStartTime (startTime);
  ueAMachine->SetRefInt (refInt);
  ueAMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttCallMerge::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttCallMerge::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttCallMerge::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&McpttCallMerge::UeATimerExpCb, this));

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);
  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueATypeMachine->SetStarted (true);

  Time ueATfg2Delay = Seconds (3); //ueAMachine->CalcDelayForPeriodicAnnoun ();
  Time ueATfg6Delay = ueAMachine->CalcDelayForMaxDuration ();
  ueATfg2->SetDelay (ueATfg2Delay);
  ueATfg6->SetDelay (ueATfg6Delay);

  // UE B
  ueBCall->AddDropper (Create<McpttMsgDropperImpl> (5, McpttCallMsgGrpProbe::GetTypeId ()));
  ueBCall->AddDropper (Create<McpttMsgDropperImpl> (0, McpttCallMsgGrpAnnoun::GetTypeId ()));

  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttCallMerge::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttCallMerge::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttCallMerge::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&McpttCallMerge::UeBTimerExpCb, this));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueBTypeMachine->SetStarted (true);

  Time ueBTfg2Delay = ueBMachine->CalcDelayForPeriodicAnnoun ();
  Time ueBTfg6Delay = ueBMachine->CalcDelayForMaxDuration ();
  ueBTfg2->SetDelay (ueBTfg2Delay);
  ueBTfg6->SetDelay (ueBTfg6Delay);

  // UE C
  ueCMachine->SetStartState (McpttCallMachineGrpBasicStateS1::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttCallMerge::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttCallMerge::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttCallMerge::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&McpttCallMerge::UeCTimerExpCb, this));

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (joinTime, &McpttPttApp::TakePushNotification, ueCPttApp);
}

void
McpttCallMerge::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueATfg2Exp, true, "UE A timer TFG2 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent an announcement message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received an announcement message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTfg2Exp, false, "UE B timer TFG2 expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, false, "UE B sent an announcement message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, 2, "UE B never received 2 announcement messages.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS2, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCS2ToS3, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, true, "UE C never received an announcement message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCTfg1Exp, true, "UE C timer TFG1 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCTfg3Exp, 3, "UE C timer TFG3 did not expire 3 times.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCTxProbe, 4, "UE C did not send 4 probe messages.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCTxAnnoun, true, "UE C never sent an announcement message.");
}

void
McpttCallMerge::UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg2 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg2Exp, false, "UE A timer " << timer << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueATfg2Exp = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A timer TFG6 is not running.");
}

void
McpttCallMerge::UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg2Exp, false, "UE A timer " << *tfg2 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, false, "UE A already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueARxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message received " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A TFG6 is not running.");
}

void
McpttCallMerge::UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
}

void
McpttCallMerge::UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg2Exp, false, "UE A timer " << *tfg2 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, false, "UE A already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueATxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A TFG6 is not running.");
}

void
McpttCallMerge::UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg2 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBTfg2Exp, false, "UE B timer " << *tfg2 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, true, "UE B never sent " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, 1, "UE B never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueBTfg2Exp = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B timer TFG6 is not running.");
}

void
McpttCallMerge::UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBTfg2Exp, false, "UE B timer " << *tfg2 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, false, "UE B already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_LT (m_ueBRxAnnoun, 2, "UE B received too many " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueBRxAnnoun += 1;
      if (m_ueBRxAnnoun == 2
          && m_ueCRxAnnoun == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B TFG6 is not running.");
}

void
McpttCallMerge::UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
}

void
McpttCallMerge::UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBTfg2Exp, false, "UE B timer " << *tfg2 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, false, "UE B already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, 1, "UE B never received " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueBTxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B TFG6 is not running.");
}

void
McpttCallMerge::UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg3 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS2, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS2ToS3, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already received an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg1Exp, false, "UE C timer TFG1 already expired.");
      NS_TEST_ASSERT_MSG_LT (m_ueCTfg3Exp, 3, "UE C timer TFG3 expired too many times.");
      if (m_ueCTxProbe > 1)
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueCTxProbe, m_ueCTfg3Exp + 2, "UE C did not send " << (m_ueCTfg3Exp + 2) << " probe messages.");
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueCTxProbe, 0, "UE C did already sent probe message.");
        }
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAnnoun, false, "UE C already sent an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS2::GetStateId (), "UE C not in state " << McpttCallMachineGrpBasicStateS2::GetStateId ());
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE C timer TFG1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE C timer TFG3 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE C timer TFG2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE C timer TFG6 is running.");
      m_ueCTfg3Exp += 1;
    }
  else if (timer.GetId () == machine.GetTfg1 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS2, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS2ToS3, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already received an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg1Exp, false, "UE C timer TFG1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg3Exp, 3, "UE C timer TFG3 did not expire 3 times.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxProbe, 4, "UE C did not send 4 probe messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAnnoun, true, "UE C never sent an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE C not in state " << McpttCallMachineGrpBasicStateS3::GetStateId ());
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer TFG1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE C timer TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer TFG3 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE C timer TFG6 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE C in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueCTfg1Exp = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
}

void
McpttCallMerge::UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS2, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS2ToS3, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already received an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg1Exp, true, "UE C timer TFG1 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg3Exp, 3, "UE C timer TFG3 did not expire 3 times.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxProbe, 4, "UE C did not send 4 probe messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAnnoun, true, "UE C never sent an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE C not in state " << McpttCallMachineGrpBasicStateS3::GetStateId ());
      m_ueCRxAnnoun = true;
      if (m_ueBRxAnnoun == 2)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE C timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE C timer TFG6 is not running.");
}

void
McpttCallMerge::UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpProbe::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS2, (m_ueCTxProbe > 0), "UE C never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS2ToS3, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already received an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg1Exp, false, "UE C timer TFG1 already expired.");
      if (m_ueCTfg3Exp > 0)
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueCTfg3Exp, m_ueCTxProbe - 1, "UE C timer TFG3 did not expire " << (m_ueCTxProbe - 1) << " times.");
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueCTfg3Exp, 0, "UE C timer TFG3 already expired.");
        }
      if (m_ueCTxProbe > 0)
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueCTxProbe, m_ueCTfg3Exp + 1, "UE C did not send " << (m_ueCTfg3Exp + 1) << " probe messages.");
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueCTxProbe, 0, "UE C did already sent probe message.");
        }
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAnnoun, false, "UE C already sent an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE C timer TFG1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE C timer TFG2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE C timer TFG3 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE C timer TFG6 is running.");
      m_ueCTxProbe += 1;
    }
  else if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS2, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS2ToS3, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already received an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg1Exp, false, "UE C timer TFG1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg3Exp, m_ueCTxProbe - 1, "UE C timer TFG3 did not expire " << (m_ueCTxProbe - 1) << " times.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxProbe, m_ueCTfg3Exp + 1, "UE C did not send " << (m_ueCTfg3Exp + 1) << " probe messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAnnoun, false, "UE C already sent an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer TFG1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer TFG3 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE C timer TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE C timer TFG6 is not running.");
      m_ueCTxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetInstanceTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
}

void
McpttCallMerge::UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS2, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS2ToS3, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already received an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg1Exp, false, "UE C timer TFG1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg3Exp, 0, "UE C timer TFG3 did not expire 3 times.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxProbe, 1, "UE C did not send 1 probe messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAnnoun, false, "UE C already sent an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE C timer TFG1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE C timer TFG2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE C timer TFG3 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE C timer TFG6 is running.");
      m_ueCS1ToS2 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS2::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS2, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS2ToS3, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already received an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg1Exp, false, "UE C timer TFG1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg3Exp, 3, "UE C timer TFG3 did not expire 3 times.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxProbe, 4, "UE C did not send 4 probe messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTxAnnoun, true, "UE C never sent an announcement message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer TFG1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE C timer TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer TFG3 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE C timer TFG6 is not running.");
      m_ueCS2ToS3 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made unexpected state transition from " << oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
}

McpttCallRelease::McpttCallRelease (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAS3ToS6 (false),
    m_ueAS6ToS1 (false),
    m_ueARxAnnoun (false),
    m_ueATfg5Exp (false),
    m_ueBTfg2Exp (false),
    m_ueBTxAnnoun (false)
{ }

void
McpttCallRelease::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (TestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();

  Ptr<TestCallMachine> ueAMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<TestCallMachine> ueBMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<TestCallMachine> ueCMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueATfg5 = ueAMachine->GetTfg5 ();
  Ptr<McpttTimer> ueATfg6 = ueAMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueATypeMachine = ueAMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueBTypeMachine = ueBMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();

  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueCTypeMachine = ueCMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
  Time exitTime = Seconds (2.9);
  Time ueATfg5Delay = Seconds (0.5);
  uint32_t origId = ueBPttApp->GetUserId ();
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  Address origAddress = ueBPttApp->GetLocalAddress ();

  ueBPttApp->GetSelectedCall ()->GetAttribute ("PeerAddress", grpAddress);

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress);
  sdp.SetSpeechPort (speechPort);

  // UE A
  ueAMachine->SetCallId (callId);
  ueAMachine->SetGrpId (grpId);
  ueAMachine->SetOrigId (origId);
  ueAMachine->SetRefInt (refInt);
  ueAMachine->SetSdp (sdp);
  ueAMachine->SetStartTime (startTime);
  ueAMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueAMachine->SetPostRxCb (MakeCallback (&McpttCallRelease::UeARxCb, this));
  ueAMachine->SetPostTxCb (MakeCallback (&McpttCallRelease::UeATxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttCallRelease::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&McpttCallRelease::UeATimerExpCb, this));

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueATypeMachine->SetStarted (true);

  Time ueATfg2Delay = ueAMachine->CalcDelayForPeriodicAnnoun ();
  Time ueATfg6Delay = ueAMachine->CalcDelayForMaxDuration ();
  ueATfg2->SetDelay (ueATfg2Delay);
  ueATfg5->SetDelay (ueATfg5Delay);
  ueATfg6->SetDelay (ueATfg6Delay);

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (1000);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttCallRelease::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttCallRelease::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttCallRelease::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&McpttCallRelease::UeBTimerExpCb, this));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueBTypeMachine->SetStarted (true);

  Time ueBTfg2Delay = ueBMachine->CalcDelayForPeriodicAnnoun ();
  Time ueBTfg6Delay = ueBMachine->CalcDelayForMaxDuration ();
  ueBTfg2->SetDelay (ueBTfg2Delay);
  ueBTfg6->SetDelay (ueBTfg6Delay);

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttCallRelease::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttCallRelease::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttCallRelease::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&McpttCallRelease::UeCTimerExpCb, this));

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueCTypeMachine->SetStarted (true);

  Time ueCTfg2Delay = ueCMachine->CalcDelayForPeriodicAnnoun ();
  Time ueCTfg6Delay = ueCMachine->CalcDelayForMaxDuration ();
  ueCTfg2->SetDelay (ueCTfg2Delay);
  ueCTfg6->SetDelay (ueCTfg6Delay);

  // Events
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (exitTime, &McpttCallMachineGrpBasic::ReleaseCall, ueAMachine);
}

void
McpttCallRelease::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAS3ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS3::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, true, "UE A timer TFG5 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTfg2Exp, true, "UE B timer TFG2 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, true, "UE B never sent a " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
}

void
McpttCallRelease::UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg5 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, false, "UE A " << timer << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS3ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS3::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      m_ueATfg5Exp = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS1::GetStateId ());
}

void
McpttCallRelease::UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, false, "UE A TFG5 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS3ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS3::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      m_ueARxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), true, "UE A timer TFG5 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS6::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS1::GetStateId ());
}

void
McpttCallRelease::UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS3::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS6::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS3ToS6, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS3::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A received a " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, false, "UE A TFG5 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), true, "UE A timer TFG5 is not running.");
      m_ueAS3ToS6 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS6::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, false, "UE A TFG5 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS3ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS3::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
      m_ueAS6ToS1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
}

void
McpttCallRelease::UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallRelease::UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg2 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, true, "UE B never sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      m_ueBTfg2Exp = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B timer TFG6 is not running.");
  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS3::GetStateId ());
}

void
McpttCallRelease::UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      // Do nothing
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B timer TFG6 is not running.");
  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS3::GetStateId ());
}

void
McpttCallRelease::UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      m_ueBTxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetInstanceTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B timer TFG6 is not running.");
  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS3::GetStateId ());
}

void
McpttCallRelease::UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << *oldState << " to " << *newState << ".");
}

void
McpttCallRelease::UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallRelease::UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      // Do nothing
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE C timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE C timer TFG6 is not running.");
  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS3::GetStateId ());
}

void
McpttCallRelease::UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallRelease::UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << *oldState << " to " << *newState << ".");
}

McpttCallReleaseAfterProbe::McpttCallReleaseAfterProbe (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAS2ToS7 (false),
    m_ueAS7ToS1 (false),
    m_ueATfg1Exp (false)
{ }

void
McpttCallReleaseAfterProbe::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (TestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();

  Ptr<TestCallMachine> ueAMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<TestCallMachine> ueBMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<TestCallMachine> ueCMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg1 = ueAMachine->GetTfg1 ();
  Ptr<McpttTimer> ueATfg3 = ueAMachine->GetTfg3 ();

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueBTypeMachine = ueBMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();

  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueCTypeMachine = ueCMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 1000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
  Time exitTime = Seconds (2.11);
  Time ueATfg5Delay = Seconds (0.5);
  uint32_t origId = ueBPttApp->GetUserId ();
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  Address origAddress = ueBPttApp->GetLocalAddress ();

  ueBPttApp->GetSelectedCall ()->GetAttribute ("PeerAddress", grpAddress);

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress);
  sdp.SetSpeechPort (speechPort);

  // UE A
  ueAMachine->SetStartState (McpttCallMachineGrpBasicStateS2::GetInstance ());
  ueAMachine->SetPostRxCb (MakeCallback (&McpttCallReleaseAfterProbe::UeARxCb, this));
  ueAMachine->SetPostTxCb (MakeCallback (&McpttCallReleaseAfterProbe::UeATxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleaseAfterProbe::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleaseAfterProbe::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttCallReleaseAfterProbe::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttCallReleaseAfterProbe::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleaseAfterProbe::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleaseAfterProbe::UeBTimerExpCb, this));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueBTypeMachine->SetStarted (true);

  Time ueBTfg2Delay = ueBMachine->CalcDelayForPeriodicAnnoun ();
  Time ueBTfg6Delay = ueBMachine->CalcDelayForMaxDuration ();
  ueBTfg2->SetDelay (ueBTfg2Delay);
  ueBTfg6->SetDelay (ueBTfg6Delay);

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttCallReleaseAfterProbe::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttCallReleaseAfterProbe::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleaseAfterProbe::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleaseAfterProbe::UeCTimerExpCb, this));

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueCTypeMachine->SetStarted (true);

  Time ueCTfg2Delay = ueCMachine->CalcDelayForPeriodicAnnoun ();
  Time ueCTfg6Delay = ueCMachine->CalcDelayForMaxDuration ();
  ueCTfg2->SetDelay (ueCTfg2Delay);
  ueCTfg6->SetDelay (ueCTfg6Delay);

  // Events
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg1);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg3);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (exitTime, &McpttCallMachineGrpBasic::ReleaseCall, ueAMachine);
}

void
McpttCallReleaseAfterProbe::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS7, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS7::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS7ToS1, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS7::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATfg1Exp, true, "UE A timer TFG1 never expired.");
}

void
McpttCallReleaseAfterProbe::UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg1 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS7, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS7::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS7ToS1, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS7::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1Exp, false, "UE A timer TFG1 never expired.");
      m_ueATfg1Exp = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is not running.");
  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS1::GetStateId ());
}

void
McpttCallReleaseAfterProbe::UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallReleaseAfterProbe::UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS2::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS7::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS7, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS7::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS7ToS1, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS7::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1Exp, false, "UE A timer TFG1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A timer TFG1 is not running.");
      m_ueAS2ToS7 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS7::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS7, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS7::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS7ToS1, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS7::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1Exp, false, "UE A timer TFG1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
      m_ueAS7ToS1 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
}

void
McpttCallReleaseAfterProbe::UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallReleaseAfterProbe::UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallReleaseAfterProbe::UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
}

void
McpttCallReleaseAfterProbe::UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallReleaseAfterProbe::UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << *oldState << " to " << *newState << ".");
}

void
McpttCallReleaseAfterProbe::UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallReleaseAfterProbe::UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
}

void
McpttCallReleaseAfterProbe::UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallReleaseAfterProbe::UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << *oldState << " to " << *newState << ".");
}

McpttCallReleasePendingUserAction1::McpttCallReleasePendingUserAction1 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAS4ToS6 (false),
    m_ueAS6ToS3 (false)
{ }

void
McpttCallReleasePendingUserAction1::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (TestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();

  Ptr<TestCallMachine> ueAMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<TestCallMachine> ueBMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<TestCallMachine> ueCMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueATfg4 = ueAMachine->GetTfg4 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueATypeMachine = ueAMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueBTypeMachine = ueBMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();

  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueCTypeMachine = ueCMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 1000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.10);
  Time exitTime = Seconds (2.11);
  Time joinTime = Seconds (2.12);
  Time ueATfg5Delay = Seconds (0.5);
  uint32_t origId = ueBPttApp->GetUserId ();
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  Address origAddress = ueBPttApp->GetLocalAddress ();

  ueBPttApp->GetSelectedCall ()->GetAttribute ("PeerAddress", grpAddress);

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress);
  sdp.SetSpeechPort (speechPort);

  // UE A
  ueAMachine->SetCallId (callId);
  ueAMachine->SetGrpId (grpId);
  ueAMachine->SetOrigId (origId);
  ueAMachine->SetRefInt (refInt);
  ueAMachine->SetSdp (sdp);
  ueAMachine->SetStartTime (startTime);
  ueAMachine->SetStartState (McpttCallMachineGrpBasicStateS4::GetInstance ());
  ueAMachine->SetPostRxCb (MakeCallback (&McpttCallReleasePendingUserAction1::UeARxCb, this));
  ueAMachine->SetPostTxCb (MakeCallback (&McpttCallReleasePendingUserAction1::UeATxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleasePendingUserAction1::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleasePendingUserAction1::UeATimerExpCb, this));

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueATypeMachine->SetStarted (true);

  Time ueATfg2Delay = ueAMachine->CalcDelayForPeriodicAnnoun ();
  ueATfg2->SetDelay (ueATfg2Delay);

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttCallReleasePendingUserAction1::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttCallReleasePendingUserAction1::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleasePendingUserAction1::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleasePendingUserAction1::UeBTimerExpCb, this));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueBTypeMachine->SetStarted (true);

  Time ueBTfg2Delay = ueBMachine->CalcDelayForPeriodicAnnoun ();
  Time ueBTfg6Delay = ueBMachine->CalcDelayForMaxDuration ();
  ueBTfg2->SetDelay (ueBTfg2Delay);
  ueBTfg6->SetDelay (ueBTfg6Delay);

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttCallReleasePendingUserAction1::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttCallReleasePendingUserAction1::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleasePendingUserAction1::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleasePendingUserAction1::UeCTimerExpCb, this));

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueCTypeMachine->SetStarted (true);

  Time ueCTfg2Delay = ueCMachine->CalcDelayForPeriodicAnnoun ();
  Time ueCTfg6Delay = ueCMachine->CalcDelayForMaxDuration ();
  ueCTfg2->SetDelay (ueCTfg2Delay);
  ueCTfg6->SetDelay (ueCTfg6Delay);

  // Events
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg4);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (exitTime, &McpttCallMachineGrpBasic::ReleaseCall, ueAMachine);
  Simulator::Schedule (joinTime, &McpttCallMachineGrpBasic::InitiateCall, ueAMachine);
}

void
McpttCallReleasePendingUserAction1::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAS4ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS3, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
}

void
McpttCallReleasePendingUserAction1::UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " unexpectedly expired.");
}

void
McpttCallReleasePendingUserAction1::UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallReleasePendingUserAction1::UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS4::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS6::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS4ToS6, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), true, "UE A timer TFG5 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
      m_ueAS4ToS6 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS6::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS4ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A timer TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A timer TFG6 is not running.");
      m_ueAS6ToS3 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
}

void
McpttCallReleasePendingUserAction1::UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallReleasePendingUserAction1::UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallReleasePendingUserAction1::UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
}

void
McpttCallReleasePendingUserAction1::UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallReleasePendingUserAction1::UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << *oldState << " to " << *newState << ".");
}

void
McpttCallReleasePendingUserAction1::UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallReleasePendingUserAction1::UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
}

void
McpttCallReleasePendingUserAction1::UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallReleasePendingUserAction1::UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << *oldState << " to " << *newState << ".");
}

McpttCallReleasePendingUserAction2::McpttCallReleasePendingUserAction2 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueARxAnnoun (false),
    m_ueAS5ToS6 (false),
    m_ueAS6ToS1 (false),
    m_ueATfg5Exp (false),
    m_ueBRndVals (0),
    m_ueBTfg2Exp (false),
    m_ueBTxAnnoun (false),
    m_ueCRxAnnoun (false)
{ }

McpttCallReleasePendingUserAction2::~McpttCallReleasePendingUserAction2 (void)
{
  if (m_ueBRndVals != 0)
    {
      delete[] m_ueBRndVals;
      m_ueBRndVals = 0;
    }
}

void
McpttCallReleasePendingUserAction2::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (TestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();

  Ptr<TestCallMachine> ueAMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<TestCallMachine> ueBMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<TestCallMachine> ueCMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueATfg4 = ueAMachine->GetTfg4 ();
  Ptr<McpttTimer> ueATfg5 = ueAMachine->GetTfg5 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueATypeMachine = ueAMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<DeterministicRandomVariable> ueBRndDelays = CreateObject<DeterministicRandomVariable> ();
  Ptr<McpttCallTypeMachineGrpBasic> ueBTypeMachine = ueBMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();

  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueCTypeMachine = ueCMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();
  Ptr<ConstantRandomVariable> ueCRndDelays = CreateObject<ConstantRandomVariable> ();
  ueCRndDelays->SetAttribute ("Constant", DoubleValue (1.0));

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.10);
  Time exitTime = Seconds (2.11);
  Time ueATfg5Delay = Seconds (10);
  uint32_t origId = ueBPttApp->GetUserId ();
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  Address origAddress = ueBPttApp->GetLocalAddress ();

  ueBPttApp->GetSelectedCall ()->GetAttribute ("PeerAddress", grpAddress);

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress);
  sdp.SetSpeechPort (speechPort);

  // UE A
  ueAMachine->SetCallId (callId);
  ueAMachine->SetGrpId (grpId);
  ueAMachine->SetOrigId (origId);
  ueAMachine->SetRefInt (refInt);
  ueAMachine->SetSdp (sdp);
  ueAMachine->SetStartTime (startTime);
  ueAMachine->SetStartState (McpttCallMachineGrpBasicStateS5::GetInstance ());
  ueAMachine->SetPostRxCb (MakeCallback (&McpttCallReleasePendingUserAction2::UeARxCb, this));
  ueAMachine->SetPostTxCb (MakeCallback (&McpttCallReleasePendingUserAction2::UeATxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleasePendingUserAction2::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleasePendingUserAction2::UeATimerExpCb, this));

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueATypeMachine->SetStarted (true);

  Time ueATfg2Delay = ueAMachine->CalcDelayForPeriodicAnnoun ();
  ueATfg2->SetDelay (ueATfg2Delay);

  ueATfg5->SetDelay (ueATfg5Delay);

  // UE B
  m_ueBRndVals = new double [2];
  m_ueBRndVals[0] = 0.0;
  m_ueBRndVals[1] = 1.0;

  ueBRndDelays->SetValueArray (m_ueBRndVals, 2);
  ueBMachine->SetRndDelays (ueBRndDelays);
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttCallReleasePendingUserAction2::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttCallReleasePendingUserAction2::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleasePendingUserAction2::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleasePendingUserAction2::UeBTimerExpCb, this));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueBTypeMachine->SetStarted (true);

  Time ueBTfg2Delay = ueBMachine->CalcDelayForPeriodicAnnoun ();
  Time ueBTfg6Delay = ueBMachine->CalcDelayForMaxDuration ();
  ueBTfg2->SetDelay (ueBTfg2Delay);
  ueBTfg6->SetDelay (ueBTfg6Delay);

  // UE C
  ueCMachine->SetRndDelays (ueCRndDelays);
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttCallReleasePendingUserAction2::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttCallReleasePendingUserAction2::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleasePendingUserAction2::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleasePendingUserAction2::UeCTimerExpCb, this));

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueCTypeMachine->SetStarted (true);

  Time ueCTfg2Delay = ueCMachine->CalcDelayForPeriodicAnnoun ();
  Time ueCTfg6Delay = ueCMachine->CalcDelayForMaxDuration ();
  ueCTfg2->SetDelay (ueCTfg2Delay);
  ueCTfg6->SetDelay (ueCTfg6Delay);

  // Events
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg4);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (exitTime, &McpttCallMachineGrpBasic::ReleaseCall, ueAMachine);
}

void
McpttCallReleasePendingUserAction2::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received announcement message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS5ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, true, "UE A timer TFG5 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTfg2Exp, true, "UE B timer TFG2 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, true, "UE B never sent announcement message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, true, "UE C never received announcement message.");
}

void
McpttCallReleasePendingUserAction2::UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg5 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS5ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS5::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, false, "UE A timer TFG5 already expired.");
      m_ueATfg5Exp = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A timer " << timer.GetId () << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
}

void
McpttCallReleasePendingUserAction2::UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS5ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS5::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, false, "UE A timer TFG5 already expired.");
      m_ueARxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected message: " << msg.GetInstanceTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), true, "UE A timer TFG5 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS6::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS6::GetStateId ());
}

void
McpttCallReleasePendingUserAction2::UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS5::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS6::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS5ToS6, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS5::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, false, "UE A timer TFG5 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), true, "UE A timer TFG5 is not running.");
      m_ueAS5ToS6 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS6::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS5ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS5::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, false, "UE A timer TFG5 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
      m_ueAS6ToS1 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
}

void
McpttCallReleasePendingUserAction2::UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallReleasePendingUserAction2::UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg2 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, true, "UE B never sent announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTfg2Exp, false, "UE B timer TFG2 already expired.");
      m_ueBTfg2Exp = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B timer " << timer.GetId () << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B timer TFG6 is not running.");
  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS3::GetStateId ());
}

void
McpttCallReleasePendingUserAction2::UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
}

void
McpttCallReleasePendingUserAction2::UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAnnoun, false, "UE B already sent announcement message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTfg2Exp, false, "UE B timer TFG2 already expired.");
      m_ueBTxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetInstanceTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE B timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE B timer TFG6 is not running.");
  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS3::GetStateId ());
}

void
McpttCallReleasePendingUserAction2::UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << *oldState << " to " << *newState << ".");
}

void
McpttCallReleasePendingUserAction2::UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallReleasePendingUserAction2::UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already received announcement message.");
      m_ueCRxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetInstanceTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE C timer TFG2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE C timer TFG6 is not running.");
  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE C in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS3::GetStateId ());
}

void
McpttCallReleasePendingUserAction2::UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallReleasePendingUserAction2::UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << *oldState << " to " << *newState << ".");
}

McpttCallReleaseMaxDuration::McpttCallReleaseMaxDuration (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAS3ToS6 (false),
    m_ueAS6ToS1 (false),
    m_ueATfg5Exp (false),
    m_ueATfg6Exp (false)
{ }

void
McpttCallReleaseMaxDuration::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (TestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();

  Ptr<TestCallMachine> ueAMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<TestCallMachine> ueBMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<TestCallMachine> ueCMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueATfg5 = ueAMachine->GetTfg5 ();
  Ptr<McpttTimer> ueATfg6 = ueAMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueATypeMachine = ueAMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueBTypeMachine = ueBMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();

  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueCTypeMachine = ueCMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.10);
  Time ueATfg5Delay = Seconds (0.5);
  uint32_t origId = ueBPttApp->GetUserId ();
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  Address origAddress = ueBPttApp->GetLocalAddress ();

  ueBPttApp->GetSelectedCall ()->GetAttribute ("PeerAddress", grpAddress);

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress);
  sdp.SetSpeechPort (speechPort);

  // UE A
  ueAMachine->SetCallId (callId);
  ueAMachine->SetGrpId (grpId);
  ueAMachine->SetOrigId (origId);
  ueAMachine->SetRefInt (refInt);
  ueAMachine->SetSdp (sdp);
  ueAMachine->SetStartTime (startTime);
  ueAMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueAMachine->SetPostRxCb (MakeCallback (&McpttCallReleaseMaxDuration::UeARxCb, this));
  ueAMachine->SetPostTxCb (MakeCallback (&McpttCallReleaseMaxDuration::UeATxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleaseMaxDuration::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleaseMaxDuration::UeATimerExpCb, this));

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueATypeMachine->SetStarted (true);

  Time ueATfg2Delay = ueAMachine->CalcDelayForPeriodicAnnoun ();
  ueATfg2->SetDelay (ueATfg2Delay);
  ueATfg5->SetDelay (Seconds (1.0));
  ueATfg6->SetDelay (Seconds (1.0));

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttCallReleaseMaxDuration::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttCallReleaseMaxDuration::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleaseMaxDuration::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleaseMaxDuration::UeBTimerExpCb, this));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueBTypeMachine->SetStarted (true);

  Time ueBTfg2Delay = ueBMachine->CalcDelayForPeriodicAnnoun ();
  Time ueBTfg6Delay = ueBMachine->CalcDelayForMaxDuration ();
  ueBTfg2->SetDelay (ueBTfg2Delay);
  ueBTfg6->SetDelay (ueBTfg6Delay);

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetStartState (McpttCallMachineGrpBasicStateS3::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttCallReleaseMaxDuration::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttCallReleaseMaxDuration::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleaseMaxDuration::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleaseMaxDuration::UeCTimerExpCb, this));

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueCTypeMachine->SetStarted (true);

  Time ueCTfg2Delay = ueCMachine->CalcDelayForPeriodicAnnoun ();
  Time ueCTfg6Delay = ueCMachine->CalcDelayForMaxDuration ();
  ueCTfg2->SetDelay (ueCTfg2Delay);
  ueCTfg6->SetDelay (ueCTfg6Delay);

  // Events
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg6);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
}

void
McpttCallReleaseMaxDuration::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAS3ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, true, "UE A timer TFG5 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueATfg6Exp, true, "UE A timer TFG6 never expired.");
}

void
McpttCallReleaseMaxDuration::UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg6 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS3ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS3::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, false, "UE A timer TFG5 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg6Exp, false, "UE A timer TFG6 already expired.");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), true, "UE A timer TFG5 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS6::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS6::GetStateId ());
      m_ueATfg6Exp = true;
    }
  else if (timer.GetId () == machine.GetTfg5 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS3ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, false, "UE A already TFG5 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg6Exp, true, "UE A timer TFG6 never expired.");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS1::GetStateId ());
      m_ueATfg5Exp = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
}

void
McpttCallReleaseMaxDuration::UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallReleaseMaxDuration::UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS3::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS6::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS3ToS6, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS3::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, false, "UE A never made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, false, "UE A timer TFG5 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg6Exp, false, "UE A timer TFG6 never expired.");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), true, "UE A timer TFG5 is not running.");
      m_ueAS3ToS6 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS6::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS3ToS6, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS6ToS1, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS6::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg5Exp, false, "UE A already TFG5 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg6Exp, true, "UE A timer TFG6 never expired.");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
      m_ueAS6ToS1 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A made unexpected state transition to " << state->GetInstanceStateId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
}

void
McpttCallReleaseMaxDuration::UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallReleaseMaxDuration::UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallReleaseMaxDuration::UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
}

void
McpttCallReleaseMaxDuration::UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallReleaseMaxDuration::UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << *oldState << " to " << *newState << ".");
}

void
McpttCallReleaseMaxDuration::UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallReleaseMaxDuration::UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
}

void
McpttCallReleaseMaxDuration::UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallReleaseMaxDuration::UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << *oldState << " to " << *newState << ".");
}

McpttCallReleaseAndSetup::McpttCallReleaseAndSetup (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAS1ToS2 (false),
    m_ueAS2ToS7 (false),
    m_ueAS7ToS2 (false),
    m_ueATxProbe (0)
{ }

void
McpttCallReleaseAndSetup::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (TestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();

  Ptr<TestCallMachine> ueAMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<TestCallMachine> ueBMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<TestCallMachine> ueCMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg1 = ueAMachine->GetTfg1 ();
  Ptr<McpttTimer> ueATfg3 = ueAMachine->GetTfg3 ();

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueBTypeMachine = ueBMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();

  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();
  Ptr<McpttCallTypeMachineGrpBasic> ueCTypeMachine = ueCMachine->GetTypeMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Time startTime = Seconds (2.1);
  Time exitTime = Seconds (2.11);
  Time joinTime = Seconds (2.12);

  // UE A
  ueAMachine->SetStartState (McpttCallMachineGrpBasicStateS1::GetInstance ());
  ueAMachine->SetPostRxCb (MakeCallback (&McpttCallReleaseAndSetup::UeARxCb, this));
  ueAMachine->SetPostTxCb (MakeCallback (&McpttCallReleaseAndSetup::UeATxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleaseAndSetup::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleaseAndSetup::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetStartState (McpttCallMachineGrpBasicStateS1::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttCallReleaseAndSetup::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttCallReleaseAndSetup::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleaseAndSetup::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleaseAndSetup::UeBTimerExpCb, this));

  // UE C
  ueCMachine->SetStartState (McpttCallMachineGrpBasicStateS1::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttCallReleaseAndSetup::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttCallReleaseAndSetup::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReleaseAndSetup::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReleaseAndSetup::UeCTimerExpCb, this));

  // Events
  Simulator::Schedule (startTime, &McpttCallMachineGrpBasic::InitiateCall, ueAMachine);
  Simulator::Schedule (exitTime, &McpttCallMachineGrpBasic::ReleaseCall, ueAMachine);
  Simulator::Schedule (joinTime, &McpttCallMachineGrpBasic::InitiateCall, ueAMachine);
}

void
McpttCallReleaseAndSetup::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS7, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS7::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS7ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS7::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxProbe, 2, "UE A did not send two probe messages.");
}

void
McpttCallReleaseAndSetup::UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " unexpectedly expired.");
}

void
McpttCallReleaseAndSetup::UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallReleaseAndSetup::UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS7, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS7::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS7ToS2, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS7::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbe, 1, "UE A never sent probe.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE A timer TFG3 is not running.");
      m_ueAS1ToS2 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS2::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS7::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS7, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS7::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS7ToS2, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS7::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbe, 1, "UE A did not send one probe.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
      m_ueAS2ToS7 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS7::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS7, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS7::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS7ToS2, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS7::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbe, 2, "UE A did not send two probes.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE A timer TFG3 is not running.");
      m_ueAS7ToS2 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A timer TFG1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A timer TFG1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
}

void
McpttCallReleaseAndSetup::UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpProbe::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE A timer TFG3 is not running.");
      if (state->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId ())
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS7, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS7::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (m_ueAS7ToS2, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS7::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (m_ueATxProbe, 0, "UE A already sent one probe message.");
        }
      else if (state->GetInstanceStateId () == McpttCallMachineGrpBasicStateS7::GetStateId ())
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS7, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS7::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (m_ueAS7ToS2, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS7::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (m_ueATxProbe, 1, "UE A already sent two probe messages.");
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (true, false, "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS1::GetStateId () << " or " << McpttCallMachineGrpBasicStateS7::GetInstance ());
        }
      m_ueATxProbe += 1;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A timer TFG1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A timer TFG1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
}

void
McpttCallReleaseAndSetup::UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallReleaseAndSetup::UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
}

void
McpttCallReleaseAndSetup::UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallReleaseAndSetup::UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << *oldState << " to " << *newState << ".");
}

void
McpttCallReleaseAndSetup::UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallReleaseAndSetup::UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
}

void
McpttCallReleaseAndSetup::UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetInstanceTypeId () << ".");
}

void
McpttCallReleaseAndSetup::UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << *oldState << " to " << *newState << ".");
}

McpttCallReject::McpttCallReject (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAS1ToS2 (false),
    m_ueAS2ToS3 (false),
    m_ueATfg1ExpCount (0),
    m_ueATfg3ExpCount (0),
    m_ueATxProbeCount (0),
    m_ueATxAnnoun (false),
    m_ueBRxAnnoun (false),
    m_ueBRxProbeCount (0),
    m_ueBS1ToS4 (false),
    m_ueBS4ToS6 (false),
    m_ueCRxAnnoun (false),
    m_ueCRxProbeCount (0),
    m_ueCS1ToS4 (false),
    m_ueCS4ToS6 (false),
    m_ueCTfg4Exp (false)
{ }

void
McpttCallReject::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (TestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<TestCallMachine> ueAMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<TestCallMachine> ueBMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<TestCallMachine> ueCMachine = DynamicCast<TestCallMachine, McpttCallMachine> (GetCallMachine (2));

  Time startTime = Seconds (2.1);

  // UE A
  ueAMachine->SetAttribute ("ConfirmMode", BooleanValue (false));
  ueAMachine->SetStartState (McpttCallMachineGrpBasicStateS1::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&McpttCallReject::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&McpttCallReject::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReject::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReject::UeATimerExpCb, this));

  // UE B
  ueBPttApp->SetNewCallCb (MakeCallback (&McpttCallReject::UeBNewCallCb, this));

  ueBMachine->SetAttribute ("UserAckRequired", BooleanValue (true));
  ueBMachine->SetStartState (McpttCallMachineGrpBasicStateS1::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&McpttCallReject::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&McpttCallReject::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReject::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReject::UeBTimerExpCb, this));

  // UE C
  ueCPttApp->SetNewCallCb (MakeCallback (&McpttCallReject::UeCNewCallCb, this));

  ueCMachine->SetAttribute ("UserAckRequired", BooleanValue (true));
  ueCMachine->SetAttribute ("TFG4", TimeValue (Seconds (2)));
  ueCMachine->SetStartState (McpttCallMachineGrpBasicStateS1::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&McpttCallReject::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&McpttCallReject::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&McpttCallReject::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&McpttCallReject::UeCTimerExpCb, this));

  Simulator::Schedule (startTime, &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
McpttCallReject::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 1, "UE A TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
  NS_TEST_ASSERT_MSG_EQ (m_ueATfg3ExpCount, 3, "UE A TFG1 expired " << m_ueATfg3ExpCount << " time(s).");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxProbeCount, 4, "UE A TFG1 expired " << m_ueATxProbeCount << " time(s).");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent send " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, true, "UE B never recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxProbeCount, 4, "UE B recieved " << m_ueBRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBS1ToS4, true, "UE B never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBS4ToS6, true, "UE B never made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, true, "UE C never recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbeCount, 4, "UE C recieved " << m_ueCRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS4, true, "UE B never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCS4ToS6, true, "UE B never made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCTfg4Exp, true, "UE C timer TFG4 never expired.");
}

void
McpttCallReject::UeATimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfg1 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 0, "UE A timer TFG1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbeCount, 4, "UE A sent " << m_ueATxProbeCount << " message(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg3ExpCount, 3, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A timer TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A timer TFG6 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " instead of " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      m_ueATfg1ExpCount += 1;
    }
  else if (timer.GetId () == machine.GetTfg3 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 0, "UE A timer TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_LT (m_ueATxProbeCount, 5, "UE A sent " << m_ueATxProbeCount << " message(s).");
      NS_TEST_ASSERT_MSG_LT (m_ueATfg3ExpCount, 4, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, false, "UE A already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A timer TFG1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE A timer TFG3 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      m_ueATfg3ExpCount += 1;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
}

void
McpttCallReject::UeARxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallReject::UeAStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbeCount, 1, "UE A sent " << m_ueATxProbeCount << " message(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 0, "UE A timer TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg3ExpCount, 0, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, false, "UE A already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A timer TFG1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE A timer TFG3 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
      m_ueAS1ToS2 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS2::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbeCount, 4, "UE A sent " << m_ueATxProbeCount << " message(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 0, "UE A timer TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_LT (m_ueATfg3ExpCount, m_ueATxProbeCount, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A timer TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A timer TFG6 is not running.");
      m_ueAS2ToS3 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
}

void
McpttCallReject::UeATxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpProbe::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_GT_OR_EQ (m_ueATxProbeCount, m_ueATfg3ExpCount, "UE A sent " << m_ueATxProbeCount << " message(s).");
      if (m_ueATxProbeCount == 0)
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
        }
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 0, "UE A timer TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_LT_OR_EQ (m_ueATfg3ExpCount, m_ueATxProbeCount, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, false, "UE A already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), true, "UE A timer TFG1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE A timer TFG2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), true, "UE A timer TFG3 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE A timer TFG6 is running.");
      m_ueATxProbeCount += 1;
    }
  else if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxProbeCount, 4, "UE A sent " << m_ueATxProbeCount << " message(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS1ToS2, true, "UE A already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAS2ToS3, false, "UE A already made transition from " << McpttCallMachineGrpBasicStateS2::GetStateId () << " to " << McpttCallMachineGrpBasicStateS3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1ExpCount, 0, "UE A timer TFG1 expired " << m_ueATfg1ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg3ExpCount, m_ueATxProbeCount - 1, "UE A timer TFG3 expired " << m_ueATfg3ExpCount << " time(s).");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, false, "UE A already sent " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE A timer TFG1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), true, "UE A timer TFG2 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE A timer TFG3 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), true, "UE A timer TFG6 is not running.");
      m_ueATxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE A timer TFG4 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE A timer TFG5 is running.");
}

void
McpttCallReject::UeBNewCallCb (uint16_t callId)
{
  Ptr<McpttPttApp> pttApp = GetApp (1);

  pttApp->RejectCall ();
}

void
McpttCallReject::UeBTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B timer " << timer.GetId () << " unexpectedly expired..");
}

void
McpttCallReject::UeBRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpProbe::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE B already recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_LT (m_ueBRxProbeCount, 4, "UE B recieved " << m_ueBRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBS1ToS4, false, "UE B already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBS4ToS6, false, "UE B already made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer " << tfg4->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS1::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      m_ueBRxProbeCount += 1;
    }
  else if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE B already recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxProbeCount, 4, "UE B recieved " << m_ueBRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBS1ToS4, true, "UE B never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBS4ToS6, false, "UE B already made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), true, "UE B timer " << tfg4->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS4::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
      m_ueBRxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer " << tfg1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE B timer " << tfg2->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer " << tfg3->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE B timer " << tfg6->GetId () << " is running.");
}

void
McpttCallReject::UeBTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallReject::UeBStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS4::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE B never recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxProbeCount, 4, "UE B recieved " << m_ueBRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBS1ToS4, false, "UE B already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBS4ToS6, false, "UE B already made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), true, "UE B timer  " << tfg4->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE B timer TFG5 is running.");
      m_ueBS1ToS4 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS4::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS6::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, true, "UE B never recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxProbeCount, 4, "UE B recieved " << m_ueBRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBS1ToS4, true, "UE B already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBS4ToS6, false, "UE B already made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE B timer  " << tfg4->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), true, "UE B timer TFG5 is not running.");
      m_ueBS4ToS6 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE B timer " << tfg1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE B timer " << tfg2->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE B timer " << tfg3->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE B timer " << tfg6->GetId () << " is running.");
}

void
McpttCallReject::UeCNewCallCb (uint16_t callId)
{
  //Do nothing.
}

void
McpttCallReject::UeCTimerExpCb (const TestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();

  if (timer.GetId () == tfg4->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, true, "UE C never recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbeCount, 4, "UE C recieved " << m_ueCRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS4, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS4ToS6, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg4Exp, false, "UE C timer TFG4 already expired.");
      m_ueCTfg4Exp = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C timer " << timer.GetId () <<  " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer " << tfg1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE C timer " << tfg2->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer " << tfg3->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer " << tfg4->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), true, "UE C timer " << tfg5->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE C timer " << tfg6->GetId () << " is running.");
}

void
McpttCallReject::UeCRxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();
  Ptr<McpttCallMachineGrpBasicState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpProbe::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_LT (m_ueCRxProbeCount, 4, "UE C recieved " << m_ueCRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS4, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS4ToS6, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg4Exp, false, "UE C timer TFG4 already expired.");
      NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS1::GetStateId (), "UE C in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS1::GetStateId () << ".");
      m_ueCRxProbeCount += 1;
    }
  else if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbeCount, 4, "UE C recieved " << m_ueCRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS4, true, "UE C never made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS4ToS6, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg4Exp, false, "UE C timer TFG4 already expired.");
      NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), true, "UE C timer TFG4 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachineGrpBasicStateS4::GetStateId (), "UE C in state " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
      m_ueCRxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer " << tfg1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE C timer " << tfg2->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer " << tfg3->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE C timer " << tfg6->GetId () << " is running.");
}

void
McpttCallReject::UeCTxCb (const TestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallReject::UeCStateChangeCb (const TestCallMachine& machine, Ptr<McpttCallMachineGrpBasicState>  oldState, Ptr<McpttCallMachineGrpBasicState>  newState)
{
  Ptr<McpttTimer> tfg1 = machine.GetTfg1 ();
  Ptr<McpttTimer> tfg2 = machine.GetTfg2 ();
  Ptr<McpttTimer> tfg3 = machine.GetTfg3 ();
  Ptr<McpttTimer> tfg4 = machine.GetTfg4 ();
  Ptr<McpttTimer> tfg5 = machine.GetTfg5 ();
  Ptr<McpttTimer> tfg6 = machine.GetTfg6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS1::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS4::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C never recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbeCount, 4, "UE C recieved " << m_ueCRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS4, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS4ToS6, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg4Exp, false, "UE C timer TFG4 already expired.");
      NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), true, "UE C timer TFG4 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), false, "UE C timer TFG5 is running.");
      m_ueCS1ToS4 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS4::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachineGrpBasicStateS6::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, true, "UE C never recieved " << McpttCallMsgGrpAnnoun::GetTypeId () << " message.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxProbeCount, 4, "UE C recieved " << m_ueCRxProbeCount << " " << McpttCallMsgGrpProbe::GetTypeId () << " messages.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS1ToS4, true, "UE C already made transition from " << McpttCallMachineGrpBasicStateS1::GetStateId () << " to " << McpttCallMachineGrpBasicStateS4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCS4ToS6, false, "UE C already made transition from " << McpttCallMachineGrpBasicStateS4::GetStateId () << " to " << McpttCallMachineGrpBasicStateS6::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCTfg4Exp, false, "UE C timer TFG4 already expired.");
      NS_TEST_ASSERT_MSG_EQ (tfg4->IsRunning (), false, "UE C timer TFG4 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfg5->IsRunning (), true, "UE C timer TFG5 is not running.");
      m_ueCS4ToS6 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg1->IsRunning (), false, "UE C timer " << tfg1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg2->IsRunning (), false, "UE C timer " << tfg2->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg3->IsRunning (), false, "UE C timer " << tfg3->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg6->IsRunning (), false, "UE C timer " << tfg6->GetId () << " is running.");
}

McpttCallControlTestSuite::McpttCallControlTestSuite (void)
  : TestSuite ("mcptt-call-control", TestSuite::SYSTEM)
{
  AddTestCase (new McpttCallSetupJoinCall (), TestCase::QUICK);
  AddTestCase (new McpttCallSetupNewCall (), TestCase::QUICK);
  AddTestCase (new McpttCallMerge (), TestCase::QUICK);
  AddTestCase (new McpttCallRelease (), TestCase::QUICK);
  AddTestCase (new McpttCallReleaseAfterProbe (), TestCase::QUICK);
  AddTestCase (new McpttCallReleasePendingUserAction1 (), TestCase::QUICK);
  AddTestCase (new McpttCallReleasePendingUserAction2 (), TestCase::QUICK);
  AddTestCase (new McpttCallReleaseMaxDuration (), TestCase::QUICK);
  AddTestCase (new McpttCallReleaseAndSetup (), TestCase::QUICK);
  AddTestCase (new McpttCallReject (), TestCase::QUICK);
}

} // namespace tests
} // namespace psc
} // namespace ns3

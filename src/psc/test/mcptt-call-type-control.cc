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
#include <ns3/mcptt-call-type-machine-grp-basic.h>
#include <ns3/mcptt-call-msg.h>
#include <ns3/mcptt-ptt-app.h>

#include "mcptt-test-case.h"
#include "mcptt-test-case-config.h"
#include "mcptt-test-call.h"
#include "mcptt-msg-dropper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallTypeControlTest");

namespace psc {
namespace tests {

class TestCallTypeMachine : public McpttCallTypeMachineGrpBasic
{
public:
  static TypeId GetTypeId (void);
  TestCallTypeMachine (void);
  virtual ~TestCallTypeMachine (void);
  virtual void ChangeState (const McpttEntityId& stateId);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void ReceiveGrpCallAnnoun (const McpttCallMsgGrpAnnoun& msg);
  virtual void ReceiveGrpCallEmergEnd (const McpttCallMsgGrpEmergEnd& msg);
  virtual void ReceiveGrpCallImmPerilEnd (const McpttCallMsgGrpImmPerilEnd& msg);
  virtual void Start (void);
  virtual void Send (const McpttCallMsg& msg);

protected:
  virtual void ExpiryOfTfg1 (void);
  virtual void ExpiryOfTfg11 (void);
  virtual void ExpiryOfTfg12 (void);
  virtual void ExpiryOfTfg13 (void);
  virtual void ExpiryOfTfg14 (void);

private:
  Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> m_postRxCb;
  Callback<void, const TestCallTypeMachine&, const McpttTimer&> m_postTimerExpCb;
  Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> m_postTxCb;
  Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> m_preRxCb;
  Callback<void, const TestCallTypeMachine&, const McpttTimer&> m_preTimerExpCb;
  Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> m_preTxCb;
  McpttEntityId m_startStateId;
  Callback<void, const TestCallTypeMachine&, const McpttEntityId&, const McpttEntityId&> m_stateChangeCb;

public:
  virtual Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> GetPostRxCb (void) const;
  virtual Callback<void, const TestCallTypeMachine&, const McpttTimer&> GetPostTimerExpCb (void) const;
  virtual Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> GetPostTxCb (void) const;
  virtual Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> GetPreRxCb (void) const;
  virtual Callback<void, const TestCallTypeMachine&, const McpttTimer&> GetPreTimerExpCb (void) const;
  virtual Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> GetPreTxCb (void) const;
  virtual Callback<void, const TestCallTypeMachine&, const McpttEntityId&, const McpttEntityId&> GetStateChangeCb (void) const;
  virtual void SetPostRxCb (const Callback<void, const TestCallTypeMachine&, const McpttCallMsg&>  postRxCb);
  virtual void SetPostTimerExpCb (const Callback<void, const TestCallTypeMachine&, const McpttTimer&>  timerExpCb);
  virtual void SetPostTxCb (const Callback<void, const TestCallTypeMachine&, const McpttCallMsg&>  postTxCb);
  virtual void SetPreRxCb (const Callback<void, const TestCallTypeMachine&, const McpttCallMsg&>  preRxCb);
  virtual void SetPreTimerExpCb (const Callback<void, const TestCallTypeMachine&, const McpttTimer&>  timerExpCb);
  virtual void SetPreTxCb (const Callback<void, const TestCallTypeMachine&, const McpttCallMsg&>  preTxCb);
  virtual void SetStateChangeTestCb (const Callback<void, const TestCallTypeMachine&, const McpttEntityId&, const McpttEntityId&>  stateChangeCb);
};

class McpttCallTypeInitEstabNewCall : public McpttTestCase
{
public:
  McpttCallTypeInitEstabNewCall (const std::string& name = "Call Type Initialization - Establish New Call", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAT0ToT2;
  bool m_ueATfg1Exp;
};

class McpttCallTypeInitJoinEmergCall : public McpttTestCase
{
public:
  McpttCallTypeInitJoinEmergCall (const std::string& name = "Call Type Initialization - Join Emergency Call", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueARxAnnoun;
  bool m_ueAT0ToT1;
};

class McpttCallTypeInitJoinImmPerilCall : public McpttTestCase
{
public:
  McpttCallTypeInitJoinImmPerilCall (const std::string& name = "Call Type Initialization - Join Imminent Peril Call", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueARxAnnoun;
  bool m_ueAT0ToT3;
};

class McpttCallTypeInitJoinBasicCall : public McpttTestCase
{
public:
  McpttCallTypeInitJoinBasicCall (const std::string& name = "Call Type Initialization - Join Basic Call", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueARxAnnoun;
  bool m_ueAT0ToT2;
};

class McpttCallTypeInitJoinEmergCallUserAck : public McpttTestCase
{
public:
  McpttCallTypeInitJoinEmergCallUserAck (const std::string& name = "Call Type Initialization - Join Emergency Call with User Acknowledgement", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueARxAnnoun;
  bool m_ueAT0ToT1;
};

class McpttCallTypeInitJoinEmergCallNoUserAck : public McpttTestCase
{
public:
  McpttCallTypeInitJoinEmergCallNoUserAck (const std::string& name = "Call Type Initialization - Join Emergency Call with User Acknowledgement", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueARxAnnoun;
  bool m_ueAT0ToT1;
};

class McpttCallTypeUpgradeFromBasicCall : public McpttTestCase
{
public:
  McpttCallTypeUpgradeFromBasicCall (const std::string& name = "Call Type Upgrade - From Basic Call", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAT2ToT3;
  bool m_ueATxAnnoun;
  bool m_ueBRxAnnoun;
  bool m_ueBT2ToT3;
  bool m_ueCRxAnnoun;
  bool m_ueCT2ToT3;
};

class McpttCallTypeUpgradeFromImmPerilCall : public McpttTestCase
{
public:
  McpttCallTypeUpgradeFromImmPerilCall (const std::string& name = "Call Type Upgrade - From Imminent Peril Call", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAT3ToT1;
  bool m_ueATxAnnoun;
  bool m_ueBRxAnnoun;
  bool m_ueBT3ToT1;
  bool m_ueCRxAnnoun;
  bool m_ueCT3ToT1;
};

class McpttCallTypeExplicitDowngradeFromEmerg : public McpttTestCase
{
public:
  McpttCallTypeExplicitDowngradeFromEmerg (const std::string& name = "Call Type Explicit Downgrade - From Emergency", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAT1ToT2;
  bool m_ueATxEmergEnd;
  bool m_ueBRxEmergEnd;
  bool m_ueBT1ToT2;
  bool m_ueCRxEmergEnd;
  bool m_ueCT1ToT2;
};

class McpttCallTypeExplicitDowngradeFromImmPeril : public McpttTestCase
{
public:
  McpttCallTypeExplicitDowngradeFromImmPeril (const std::string& name = "Call Type Explicit Downgrade - From Imminent Peril", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAT3ToT2;
  bool m_ueATxImmPerilEnd;
  bool m_ueBRxImmPerilEnd;
  bool m_ueBT3ToT2;
  bool m_ueCRxImmPerilEnd;
  bool m_ueCT3ToT2;
};

class McpttCallTypeImplicitDowngradeFromEmerg : public McpttTestCase
{
public:
  McpttCallTypeImplicitDowngradeFromEmerg (const std::string& name = "Call Type Implicit Downgrade - From Emergency", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAT1ToT2;
  bool m_ueATfg13Exp;
};

class McpttCallTypeImplicitDowngradeFromImmPeril : public McpttTestCase
{
public:
  McpttCallTypeImplicitDowngradeFromImmPeril (const std::string& name = "Call Type Implicit Downgrade - From Imminent Peril", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAT3ToT2;
  bool m_ueATfg14Exp;
};

class McpttCallTypeReleaseAfterCallEstab : public McpttTestCase
{
public:
  McpttCallTypeReleaseAfterCallEstab (const std::string& name = "Call Type Release - After Call Establishment", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAT2ToT0;
};

class McpttCallTypeMerge : public McpttTestCase
{
public:
  McpttCallTypeMerge (const std::string& name = "Call Type Merge", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId);
  virtual void UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueARxAnnoun;
  bool m_ueAT2ToT1;
  bool m_ueBRxAnnoun;
  bool m_ueBT3ToT1;
};

class McpttCallTypeControlTestSuite : public TestSuite
{
public:
  McpttCallTypeControlTestSuite (void);
};

/***************************************************************
 *  Implementation of the code declared above.
 ***************************************************************/

static McpttCallTypeControlTestSuite callSuite;

TypeId
TestCallTypeMachine::GetTypeId (void)
{
  static TypeId tid = TypeId ("TestCallTypeMachine")
    .SetParent<McpttCallTypeMachineGrpBasic> ()
    .AddConstructor<TestCallTypeMachine> ()
  ;

  return tid;
}

TestCallTypeMachine::TestCallTypeMachine (void)
  : McpttCallTypeMachineGrpBasic (),
    m_postRxCb (MakeNullCallback<void, const TestCallTypeMachine&, const McpttCallMsg&> ()),
    m_postTimerExpCb (MakeNullCallback<void, const TestCallTypeMachine&, const McpttTimer&> ()),
    m_postTxCb (MakeNullCallback<void, const TestCallTypeMachine&, const McpttCallMsg&> ()),
    m_preRxCb (MakeNullCallback<void, const TestCallTypeMachine&, const McpttCallMsg&> ()),
    m_preTimerExpCb (MakeNullCallback<void, const TestCallTypeMachine&, const McpttTimer&> ()),
    m_preTxCb (MakeNullCallback<void, const TestCallTypeMachine&, const McpttCallMsg&> ()),
    m_startStateId (McpttCallTypeMachineGrpBasic::T0),
    m_stateChangeCb (MakeNullCallback<void, const TestCallTypeMachine&, const McpttEntityId&, const McpttEntityId&> ())
{ }

TestCallTypeMachine::~TestCallTypeMachine (void)
{ }

void
TestCallTypeMachine::ReceiveGrpCallAnnoun (const McpttCallMsgGrpAnnoun& msg)
{
  Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallTypeMachineGrpBasic::ReceiveGrpCallAnnoun (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
TestCallTypeMachine::ReceiveGrpCallEmergEnd (const McpttCallMsgGrpEmergEnd& msg)
{
  Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallTypeMachineGrpBasic::ReceiveGrpCallEmergEnd (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
TestCallTypeMachine::ReceiveGrpCallImmPerilEnd (const McpttCallMsgGrpImmPerilEnd& msg)
{
  Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallTypeMachineGrpBasic::ReceiveGrpCallImmPerilEnd (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
TestCallTypeMachine::Send (const McpttCallMsg& msg)
{
  Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> postTxCb = GetPostTxCb ();
  Callback<void, const TestCallTypeMachine&, const McpttCallMsg&> preTxCb = GetPreTxCb ();

  if (!preTxCb.IsNull ())
    {
      preTxCb (*this, msg);
    }

  McpttCallTypeMachineGrpBasic::Send (msg);

  if (!postTxCb.IsNull ())
    {
      postTxCb (*this, msg);
    }
}

void
TestCallTypeMachine::ExpiryOfTfg1 (void)
{
  Ptr<McpttTimer> tfg = GetOwner ()->GetTfg1 ();
  Callback<void, const TestCallTypeMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallTypeMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallTypeMachineGrpBasic::ExpiryOfTfg1 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
TestCallTypeMachine::ExpiryOfTfg11 (void)
{
  Ptr<McpttTimer> tfg = GetTfg11 ();
  Callback<void, const TestCallTypeMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallTypeMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallTypeMachineGrpBasic::ExpiryOfTfg11 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
TestCallTypeMachine::ExpiryOfTfg12 (void)
{
  Ptr<McpttTimer> tfg = GetTfg12 ();
  Callback<void, const TestCallTypeMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallTypeMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallTypeMachineGrpBasic::ExpiryOfTfg12 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
TestCallTypeMachine::ExpiryOfTfg13 (void)
{
  Ptr<McpttTimer> tfg = GetTfg13 ();
  Callback<void, const TestCallTypeMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallTypeMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallTypeMachineGrpBasic::ExpiryOfTfg13 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
TestCallTypeMachine::ExpiryOfTfg14 (void)
{
  Ptr<McpttTimer> tfg = GetTfg14 ();
  Callback<void, const TestCallTypeMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallTypeMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallTypeMachineGrpBasic::ExpiryOfTfg14 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
TestCallTypeMachine::Start (void)
{
  McpttCallTypeMachineGrpBasic::Start ();
}

void
TestCallTypeMachine::ChangeState (const McpttEntityId& newStateId)
{
  McpttEntityId oldStateId = GetStateId ();
  Callback<void, const TestCallTypeMachine&, const McpttEntityId&, const McpttEntityId&> stateChangeCb = GetStateChangeCb ();

  McpttCallTypeMachineGrpBasic::ChangeState (newStateId);

  if (!stateChangeCb.IsNull ())
    {
      stateChangeCb (*this, oldStateId, newStateId);
    }
}

TypeId
TestCallTypeMachine::GetInstanceTypeId (void) const
{
  return TestCallTypeMachine::GetTypeId ();
}

Callback<void, const TestCallTypeMachine&, const McpttCallMsg&>
TestCallTypeMachine::GetPostRxCb (void) const
{
  return m_postRxCb;
}

Callback<void, const TestCallTypeMachine&, const McpttTimer&>
TestCallTypeMachine::GetPostTimerExpCb (void) const
{
  return m_postTimerExpCb;
}


Callback<void, const TestCallTypeMachine&, const McpttCallMsg&>
TestCallTypeMachine::GetPostTxCb (void) const
{
  return m_postTxCb;
}

Callback<void, const TestCallTypeMachine&, const McpttCallMsg&>
TestCallTypeMachine::GetPreRxCb (void) const
{
  return m_preRxCb;
}

Callback<void, const TestCallTypeMachine&, const McpttTimer&>
TestCallTypeMachine::GetPreTimerExpCb (void) const
{
  return m_preTimerExpCb;
}

Callback<void, const TestCallTypeMachine&, const McpttCallMsg&>
TestCallTypeMachine::GetPreTxCb (void) const
{
  return m_preTxCb;
}

Callback<void, const TestCallTypeMachine&, const McpttEntityId&, const McpttEntityId&>
TestCallTypeMachine::GetStateChangeCb (void) const
{
  return m_stateChangeCb;
}

void
TestCallTypeMachine::SetPostRxCb (const Callback<void, const TestCallTypeMachine&, const McpttCallMsg&>  postRxCb)
{
  m_postRxCb = postRxCb;
}

void
TestCallTypeMachine::SetPostTimerExpCb (const Callback<void, const TestCallTypeMachine&, const McpttTimer&>  timerExpCb)
{
  m_postTimerExpCb = timerExpCb;
}

void
TestCallTypeMachine::SetPostTxCb (const Callback<void, const TestCallTypeMachine&, const McpttCallMsg&>  postTxCb)
{
  m_postTxCb = postTxCb;
}

void
TestCallTypeMachine::SetPreRxCb (const Callback<void, const TestCallTypeMachine&, const McpttCallMsg&>  preRxCb)
{
  m_preRxCb = preRxCb;
}

void
TestCallTypeMachine::SetPreTimerExpCb (const Callback<void, const TestCallTypeMachine&, const McpttTimer&>  timerExpCb)
{
  m_preTimerExpCb = timerExpCb;
}

void
TestCallTypeMachine::SetPreTxCb (const Callback<void, const TestCallTypeMachine&, const McpttCallMsg&>  preTxCb)
{
  m_preTxCb = preTxCb;
}

void
TestCallTypeMachine::SetStateChangeTestCb (const Callback<void, const TestCallTypeMachine&, const McpttEntityId&, const McpttEntityId&>  stateChangeCb)
{
  m_stateChangeCb = stateChangeCb;
}

McpttCallTypeInitEstabNewCall::McpttCallTypeInitEstabNewCall (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAT0ToT2 (false),
    m_ueATfg1Exp (false)
{ }

void
McpttCallTypeInitEstabNewCall::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  Time startTime = Seconds (2.1);

  // UE A
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T0);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitEstabNewCall::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitEstabNewCall::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitEstabNewCall::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitEstabNewCall::UeATimerExpCb, this));

  // UE B
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T0);
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitEstabNewCall::UeBRxCb, this));
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitEstabNewCall::UeBTxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitEstabNewCall::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitEstabNewCall::UeBTimerExpCb, this));

  // UE C
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T0);
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitEstabNewCall::UeCRxCb, this));
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitEstabNewCall::UeCTxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitEstabNewCall::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitEstabNewCall::UeCTimerExpCb, this));

  Simulator::Schedule (startTime, &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
McpttCallTypeInitEstabNewCall::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT2, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATfg1Exp, true, "UE A timer TFG1 never expired.");
}

void
McpttCallTypeInitEstabNewCall::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (timer.GetId () == machine.GetOwner ()->GetTfg1 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT2, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1Exp, false, "UE A timer TFG1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T2, "UE A in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T2 << ".");
      m_ueATfg1Exp = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeInitEstabNewCall::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeInitEstabNewCall::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T0
      && newStateId == McpttCallTypeMachineGrpBasic::T2)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT2, false, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg1Exp, false, "UE A timer TFG1 already expired.");
      m_ueAT0ToT2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeInitEstabNewCall::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypeInitEstabNewCall::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeInitEstabNewCall::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
}

void
McpttCallTypeInitEstabNewCall::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeInitEstabNewCall::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << oldStateId << " to " << newStateId << ".");
}

void
McpttCallTypeInitEstabNewCall::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeInitEstabNewCall::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
}

void
McpttCallTypeInitEstabNewCall::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeInitEstabNewCall::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << oldStateId << " to " << newStateId << ".");
}

McpttCallTypeInitJoinEmergCall::McpttCallTypeInitJoinEmergCall (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueARxAnnoun (false),
    m_ueAT0ToT1 (false)
{ }

void
McpttCallTypeInitJoinEmergCall::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg1 = ueAMachine->GetTfg1 ();

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  Ptr<McpttTimer> ueBTfg13 = ueBTypeMachine->GetTfg13 ();
  Ptr<McpttTimer> ueCTfg13 = ueCTypeMachine->GetTfg13 ();

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
  ueAMachine->SetGrpId (grpId);

  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T0);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinEmergCall::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinEmergCall::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinEmergCall::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinEmergCall::UeATimerExpCb, this));

  ueATfg1->SetDelay (Seconds (1.0));

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinEmergCall::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinEmergCall::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinEmergCall::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinEmergCall::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());
  ueBTfg13->SetDelay (ueBTypeMachine->CalcDelayForTfg13 ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinEmergCall::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinEmergCall::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinEmergCall::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinEmergCall::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (ueCMachine->CalcDelayForPeriodicAnnoun ());
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());
  ueCTfg13->SetDelay (ueCTypeMachine->CalcDelayForTfg13 ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg13);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg13);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (joinTime, &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
McpttCallTypeInitJoinEmergCall::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT1, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
}

void
McpttCallTypeInitJoinEmergCall::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypeInitJoinEmergCall::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT1, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T1, "UE A in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueARxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE A timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeInitJoinEmergCall::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T0
      && newStateId == McpttCallTypeMachineGrpBasic::T1)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT1, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueAT0ToT1 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE A timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeInitJoinEmergCall::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypeInitJoinEmergCall::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeInitJoinEmergCall::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
}

void
McpttCallTypeInitJoinEmergCall::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeInitJoinEmergCall::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << oldStateId << " to " << newStateId << ".");
}

void
McpttCallTypeInitJoinEmergCall::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeInitJoinEmergCall::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      //Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
    }
}

void
McpttCallTypeInitJoinEmergCall::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeInitJoinEmergCall::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << oldStateId << " to " << newStateId << ".");
}

McpttCallTypeInitJoinImmPerilCall::McpttCallTypeInitJoinImmPerilCall (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueARxAnnoun (false),
    m_ueAT0ToT3 (false)
{ }

void
McpttCallTypeInitJoinImmPerilCall::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg1 = ueAMachine->GetTfg1 ();

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  Ptr<McpttTimer> ueBTfg14 = ueBTypeMachine->GetTfg14 ();
  Ptr<McpttTimer> ueCTfg14 = ueCTypeMachine->GetTfg14 ();

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
  ueAMachine->SetGrpId (grpId);

  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T0);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinImmPerilCall::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinImmPerilCall::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinImmPerilCall::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinImmPerilCall::UeATimerExpCb, this));

  ueATfg1->SetDelay (Seconds (1.0));

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T3);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinImmPerilCall::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinImmPerilCall::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinImmPerilCall::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinImmPerilCall::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());
  ueBTfg14->SetDelay (ueBTypeMachine->CalcDelayForTfg14 ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T3);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinImmPerilCall::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinImmPerilCall::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinImmPerilCall::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinImmPerilCall::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (ueCMachine->CalcDelayForPeriodicAnnoun ());
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());
  ueCTfg14->SetDelay (ueCTypeMachine->CalcDelayForTfg14 ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg14);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg14);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (joinTime, &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
McpttCallTypeInitJoinImmPerilCall::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT3, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T3 << ".");
}

void
McpttCallTypeInitJoinImmPerilCall::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypeInitJoinImmPerilCall::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT3, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T3 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T3, "UE A in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T3 << ".");
      m_ueARxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), true, "UE A timer " << tfg14->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeInitJoinImmPerilCall::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T0
      && newStateId == McpttCallTypeMachineGrpBasic::T3)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT3, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueAT0ToT3 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), true, "UE A timer " << tfg14->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeInitJoinImmPerilCall::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypeInitJoinImmPerilCall::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeInitJoinImmPerilCall::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
}

void
McpttCallTypeInitJoinImmPerilCall::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeInitJoinImmPerilCall::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << oldStateId << " to " << newStateId << ".");
}

void
McpttCallTypeInitJoinImmPerilCall::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeInitJoinImmPerilCall::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      //Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
    }
}

void
McpttCallTypeInitJoinImmPerilCall::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeInitJoinImmPerilCall::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << oldStateId << " to " << newStateId << ".");
}

McpttCallTypeInitJoinBasicCall::McpttCallTypeInitJoinBasicCall (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueARxAnnoun (false),
    m_ueAT0ToT2 (false)
{ }

void
McpttCallTypeInitJoinBasicCall::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg1 = ueAMachine->GetTfg1 ();

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

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
  ueAMachine->SetGrpId (grpId);

  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T0);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinBasicCall::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinBasicCall::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinBasicCall::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinBasicCall::UeATimerExpCb, this));

  ueATfg1->SetDelay (Seconds (1.0));

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinBasicCall::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinBasicCall::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinBasicCall::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinBasicCall::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinBasicCall::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinBasicCall::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinBasicCall::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinBasicCall::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (ueCMachine->CalcDelayForPeriodicAnnoun ());
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (joinTime, &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
McpttCallTypeInitJoinBasicCall::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT2, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
}

void
McpttCallTypeInitJoinBasicCall::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypeInitJoinBasicCall::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT2, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T2, "UE A in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T2 << ".");
      m_ueARxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeInitJoinBasicCall::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T0
      && newStateId == McpttCallTypeMachineGrpBasic::T2)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT2, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      m_ueAT0ToT2 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeInitJoinBasicCall::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypeInitJoinBasicCall::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeInitJoinBasicCall::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      //Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }
}

void
McpttCallTypeInitJoinBasicCall::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeInitJoinBasicCall::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << oldStateId << " to " << newStateId << ".");
}

void
McpttCallTypeInitJoinBasicCall::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeInitJoinBasicCall::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
}

void
McpttCallTypeInitJoinBasicCall::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeInitJoinBasicCall::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << oldStateId << " to " << newStateId << ".");
}

McpttCallTypeInitJoinEmergCallUserAck::McpttCallTypeInitJoinEmergCallUserAck (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueARxAnnoun (false),
    m_ueAT0ToT1 (false)
{ }

void
McpttCallTypeInitJoinEmergCallUserAck::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg1 = ueAMachine->GetTfg1 ();

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  Ptr<McpttTimer> ueBTfg13 = ueBTypeMachine->GetTfg13 ();
  Ptr<McpttTimer> ueCTfg13 = ueCTypeMachine->GetTfg13 ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
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
  ueAMachine->SetGrpId (grpId);
  ueAMachine->SetAttribute ("UserAckRequired", BooleanValue (true));

  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T0);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinEmergCallUserAck::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinEmergCallUserAck::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinEmergCallUserAck::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinEmergCallUserAck::UeATimerExpCb, this));

  ueATfg1->SetDelay (Seconds (1.0));

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinEmergCallUserAck::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinEmergCallUserAck::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinEmergCallUserAck::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinEmergCallUserAck::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());
  ueBTfg13->SetDelay (ueBTypeMachine->CalcDelayForTfg13 ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinEmergCallUserAck::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinEmergCallUserAck::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinEmergCallUserAck::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinEmergCallUserAck::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (ueCMachine->CalcDelayForPeriodicAnnoun ());
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());
  ueCTfg13->SetDelay (ueCTypeMachine->CalcDelayForTfg13 ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg13);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg13);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
}

void
McpttCallTypeInitJoinEmergCallUserAck::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT1, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
}

void
McpttCallTypeInitJoinEmergCallUserAck::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypeInitJoinEmergCallUserAck::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT1, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T0, "UE A in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T0 << ".");
      m_ueARxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeInitJoinEmergCallUserAck::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T0
      && newStateId == McpttCallTypeMachineGrpBasic::T1)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT1, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueAT0ToT1 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE A timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeInitJoinEmergCallUserAck::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypeInitJoinEmergCallUserAck::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeInitJoinEmergCallUserAck::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      //Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }
}

void
McpttCallTypeInitJoinEmergCallUserAck::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeInitJoinEmergCallUserAck::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << oldStateId << " to " << newStateId << ".");
}

void
McpttCallTypeInitJoinEmergCallUserAck::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeInitJoinEmergCallUserAck::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      //Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
    }
}

void
McpttCallTypeInitJoinEmergCallUserAck::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeInitJoinEmergCallUserAck::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << oldStateId << " to " << newStateId << ".");
}

McpttCallTypeInitJoinEmergCallNoUserAck::McpttCallTypeInitJoinEmergCallNoUserAck (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueARxAnnoun (false),
    m_ueAT0ToT1 (false)
{ }

void
McpttCallTypeInitJoinEmergCallNoUserAck::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg1 = ueAMachine->GetTfg1 ();

  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  Ptr<McpttTimer> ueBTfg13 = ueBTypeMachine->GetTfg13 ();
  Ptr<McpttTimer> ueCTfg13 = ueCTypeMachine->GetTfg13 ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
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
  ueAMachine->SetGrpId (grpId);
  ueAMachine->SetAttribute ("UserAckRequired", BooleanValue (false));

  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T0);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinEmergCallNoUserAck::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinEmergCallNoUserAck::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinEmergCallNoUserAck::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinEmergCallNoUserAck::UeATimerExpCb, this));

  ueATfg1->SetDelay (Seconds (1.0));

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinEmergCallNoUserAck::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinEmergCallNoUserAck::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinEmergCallNoUserAck::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinEmergCallNoUserAck::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());
  ueBTfg13->SetDelay (ueBTypeMachine->CalcDelayForTfg13 ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeInitJoinEmergCallNoUserAck::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeInitJoinEmergCallNoUserAck::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeInitJoinEmergCallNoUserAck::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeInitJoinEmergCallNoUserAck::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (ueCMachine->CalcDelayForPeriodicAnnoun ());
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());
  ueCTfg13->SetDelay (ueCTypeMachine->CalcDelayForTfg13 ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg13);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg13);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT1, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT1, true, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T1, "UE A in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueARxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE A timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T0
      && newStateId == McpttCallTypeMachineGrpBasic::T1)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAT0ToT1, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueAT0ToT1 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE A timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      //Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B made invalid state transition from " << oldStateId << " to " << newStateId << ".");
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeInitJoinEmergCallNoUserAck::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C made invalid state transition from " << oldStateId << " to " << newStateId << ".");
}

McpttCallTypeUpgradeFromBasicCall::McpttCallTypeUpgradeFromBasicCall (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAT2ToT3 (false),
    m_ueATxAnnoun (false),
    m_ueBRxAnnoun (false),
    m_ueBT2ToT3 (false),
    m_ueCRxAnnoun (false),
    m_ueCT2ToT3 (false)
{ }

void
McpttCallTypeUpgradeFromBasicCall::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueATfg6 = ueAMachine->GetTfg6 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
  Time upgradeTime = Seconds (2.2);
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
  ueAMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeUpgradeFromBasicCall::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeUpgradeFromBasicCall::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeUpgradeFromBasicCall::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeUpgradeFromBasicCall::UeATimerExpCb, this));

  ueATfg2->SetDelay (ueAMachine->CalcDelayForPeriodicAnnoun ());
  ueATfg6->SetDelay (ueAMachine->CalcDelayForMaxDuration ());

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeUpgradeFromBasicCall::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeUpgradeFromBasicCall::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeUpgradeFromBasicCall::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeUpgradeFromBasicCall::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeUpgradeFromBasicCall::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeUpgradeFromBasicCall::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeUpgradeFromBasicCall::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeUpgradeFromBasicCall::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (ueCMachine->CalcDelayForPeriodicAnnoun ());
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (upgradeTime, &McpttPttApp::UpgradeCallType, ueAPttApp, McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
}

void
McpttCallTypeUpgradeFromBasicCall::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAT2ToT3, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T3 << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, true, "UE B never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBT2ToT3, true, "UE B never made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T3 << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, true, "UE C never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCT2ToT3, true, "UE C never made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T3 << ".");
}

void
McpttCallTypeUpgradeFromBasicCall::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypeUpgradeFromBasicCall::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeUpgradeFromBasicCall::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T2
      && newStateId == McpttCallTypeMachineGrpBasic::T3)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT2ToT3, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T3 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      m_ueAT2ToT3 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), true, "UE A timer " << tfg14->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeUpgradeFromBasicCall::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT2ToT3, false, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T3 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, false, "UE A already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T2, "UE A in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T2 << ".");
      m_ueATxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), true, "UE A timer " << tfg14->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeUpgradeFromBasicCall::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeUpgradeFromBasicCall::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE B already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBT2ToT3, true, "UE B never made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T3 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T3, "UE B in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T3 << ".");
      m_ueBRxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE B timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE B timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE B timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), true, "UE B timer " << tfg14->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE B counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE B counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeUpgradeFromBasicCall::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeUpgradeFromBasicCall::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T2
      && newStateId == McpttCallTypeMachineGrpBasic::T3)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE B already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBT2ToT3, false, "UE B already made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T3 << ".");
      m_ueBT2ToT3 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE B timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE B timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE B timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), true, "UE B timer " << tfg14->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE B counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE B counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeUpgradeFromBasicCall::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeUpgradeFromBasicCall::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCT2ToT3, true, "UE C never made transition from " << McpttCallTypeMachineGrpBasic::T0 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T3, "UE C in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T3 << ".");
      m_ueCRxAnnoun = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE C timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE C timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE C timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), true, "UE C timer " << tfg14->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE C counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE C counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeUpgradeFromBasicCall::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeUpgradeFromBasicCall::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T2
      && newStateId == McpttCallTypeMachineGrpBasic::T3)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCT2ToT3, false, "UE C already made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T3 << ".");
      m_ueCT2ToT3 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE C timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE C timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE C timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), true, "UE C timer " << tfg14->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE C counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE C counter " << cfg12->GetId () << " is not one.");
}

McpttCallTypeUpgradeFromImmPerilCall::McpttCallTypeUpgradeFromImmPerilCall (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAT3ToT1 (false),
    m_ueATxAnnoun (false),
    m_ueBRxAnnoun (false),
    m_ueBT3ToT1 (false),
    m_ueCRxAnnoun (false),
    m_ueCT3ToT1 (false)
{ }

void
McpttCallTypeUpgradeFromImmPerilCall::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueATfg6 = ueAMachine->GetTfg6 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  Ptr<McpttTimer> ueATfg14 = ueATypeMachine->GetTfg14 ();
  Ptr<McpttTimer> ueBTfg14 = ueBTypeMachine->GetTfg14 ();
  Ptr<McpttTimer> ueCTfg14 = ueCTypeMachine->GetTfg14 ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
  Time upgradeTime = Seconds (2.2);
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
  ueAMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T3);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeUpgradeFromImmPerilCall::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeUpgradeFromImmPerilCall::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeUpgradeFromImmPerilCall::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeUpgradeFromImmPerilCall::UeATimerExpCb, this));

  ueATfg2->SetDelay (ueAMachine->CalcDelayForPeriodicAnnoun ());
  ueATfg6->SetDelay (ueAMachine->CalcDelayForMaxDuration ());
  ueATfg14->SetDelay (ueATypeMachine->CalcDelayForTfg14 ());

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T3);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeUpgradeFromImmPerilCall::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeUpgradeFromImmPerilCall::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeUpgradeFromImmPerilCall::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeUpgradeFromImmPerilCall::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());
  ueBTfg14->SetDelay (ueBTypeMachine->CalcDelayForTfg14 ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T3);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeUpgradeFromImmPerilCall::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeUpgradeFromImmPerilCall::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeUpgradeFromImmPerilCall::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeUpgradeFromImmPerilCall::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (ueCMachine->CalcDelayForPeriodicAnnoun ());
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());
  ueCTfg14->SetDelay (ueCTypeMachine->CalcDelayForTfg14 ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg14);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg14);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg14);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (upgradeTime, &McpttPttApp::UpgradeCallType, ueAPttApp, McpttCallMsgFieldCallType::EMERGENCY_GROUP);
}

void
McpttCallTypeUpgradeFromImmPerilCall::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAT3ToT1, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, true, "UE B never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBT3ToT1, true, "UE B never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, true, "UE C never received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCT3ToT1, true, "UE C never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
}

void
McpttCallTypeUpgradeFromImmPerilCall::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypeUpgradeFromImmPerilCall::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeUpgradeFromImmPerilCall::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T3
      && newStateId == McpttCallTypeMachineGrpBasic::T1)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT3ToT1, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, true, "UE A never sent a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      m_ueAT3ToT1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE A timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeUpgradeFromImmPerilCall::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT3ToT1, false, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAnnoun, false, "UE A already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T3, "UE A in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T3 << ".");
      m_ueATxAnnoun = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE A timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeUpgradeFromImmPerilCall::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeUpgradeFromImmPerilCall::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE B already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBT3ToT1, true, "UE B never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T1, "UE B in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueBRxAnnoun = true;
      if (m_ueCRxAnnoun == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE B timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE B timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE B timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE B timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE B counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE B counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeUpgradeFromImmPerilCall::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeUpgradeFromImmPerilCall::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T3
      && newStateId == McpttCallTypeMachineGrpBasic::T1)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE B already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBT3ToT1, false, "UE B already made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueBT3ToT1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE B timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE B timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE B timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE B timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE B counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE B counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeUpgradeFromImmPerilCall::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeUpgradeFromImmPerilCall::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCT3ToT1, true, "UE C never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T1, "UE C in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueCRxAnnoun = true;
      if (m_ueBRxAnnoun == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE C timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE C timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE C timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE C timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE C counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE C counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeUpgradeFromImmPerilCall::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeUpgradeFromImmPerilCall::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T3
      && newStateId == McpttCallTypeMachineGrpBasic::T1)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxAnnoun, false, "UE C already received a " << McpttCallMsgGrpAnnoun::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCT3ToT1, false, "UE C already made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueCT3ToT1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE C timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE C timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE C timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE C timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE C counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE C counter " << cfg12->GetId () << " is not one.");
}

McpttCallTypeExplicitDowngradeFromEmerg::McpttCallTypeExplicitDowngradeFromEmerg (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAT1ToT2 (false),
    m_ueATxEmergEnd (false),
    m_ueBRxEmergEnd (false),
    m_ueBT1ToT2 (false),
    m_ueCRxEmergEnd (false),
    m_ueCT1ToT2 (false)
{ }

void
McpttCallTypeExplicitDowngradeFromEmerg::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueATfg6 = ueAMachine->GetTfg6 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  Ptr<McpttTimer> ueATfg13 = ueATypeMachine->GetTfg13 ();
  Ptr<McpttTimer> ueBTfg13 = ueBTypeMachine->GetTfg13 ();
  Ptr<McpttTimer> ueCTfg13 = ueCTypeMachine->GetTfg13 ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
  Time upgradeTime = Seconds (2.2);
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
  ueAMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromEmerg::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromEmerg::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromEmerg::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromEmerg::UeATimerExpCb, this));

  ueATfg2->SetDelay (ueAMachine->CalcDelayForPeriodicAnnoun ());
  ueATfg6->SetDelay (ueAMachine->CalcDelayForMaxDuration ());
  ueATfg13->SetDelay (ueATypeMachine->CalcDelayForTfg13 ());

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromEmerg::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromEmerg::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromEmerg::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromEmerg::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());
  ueBTfg13->SetDelay (ueBTypeMachine->CalcDelayForTfg13 ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromEmerg::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromEmerg::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromEmerg::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromEmerg::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (ueCMachine->CalcDelayForPeriodicAnnoun ());
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());
  ueCTfg13->SetDelay (ueCTypeMachine->CalcDelayForTfg13 ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg13);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg13);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg13);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (upgradeTime, &McpttPttApp::DowngradeCallType, ueAPttApp);
}

void
McpttCallTypeExplicitDowngradeFromEmerg::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAT1ToT2, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T1 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxEmergEnd, true, "UE A never sent a " << McpttCallMsgGrpEmergEnd::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxEmergEnd, true, "UE B never received a " << McpttCallMsgGrpEmergEnd::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBT1ToT2, true, "UE B never made transition from " << McpttCallTypeMachineGrpBasic::T1 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxEmergEnd, true, "UE C never received a " << McpttCallMsgGrpEmergEnd::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCT1ToT2, true, "UE C never made transition from " << McpttCallTypeMachineGrpBasic::T1 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
}

void
McpttCallTypeExplicitDowngradeFromEmerg::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypeExplicitDowngradeFromEmerg::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeExplicitDowngradeFromEmerg::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T1
      && newStateId == McpttCallTypeMachineGrpBasic::T2)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT1ToT2, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T1 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxEmergEnd, true, "UE A never sent a " << McpttCallMsgGrpEmergEnd::GetTypeId () << ".");
      m_ueAT1ToT2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), true, "UE A timer " << tfg11->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeExplicitDowngradeFromEmerg::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpEmergEnd::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT1ToT2, false, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T1 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxEmergEnd, false, "UE A already received a " << McpttCallMsgGrpEmergEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T1, "UE A in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueATxEmergEnd = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), true, "UE A timer " << tfg11->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeExplicitDowngradeFromEmerg::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeExplicitDowngradeFromEmerg::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpEmergEnd::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxEmergEnd, false, "UE B already received a " << McpttCallMsgGrpEmergEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBT1ToT2, true, "UE B never made transition from " << McpttCallTypeMachineGrpBasic::T1 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T2, "UE B in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T2 << ".");
      m_ueBRxEmergEnd = true;
      if (m_ueCRxEmergEnd == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE B timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE B timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE B timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE B timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE B counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE B counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeExplicitDowngradeFromEmerg::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeExplicitDowngradeFromEmerg::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T1
      && newStateId == McpttCallTypeMachineGrpBasic::T2)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxEmergEnd, false, "UE B already received a " << McpttCallMsgGrpEmergEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBT1ToT2, false, "UE B already made transition from " << McpttCallTypeMachineGrpBasic::T1 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      m_ueBT1ToT2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE B timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE B timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE B timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE B timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE B counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE B counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeExplicitDowngradeFromEmerg::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeExplicitDowngradeFromEmerg::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpEmergEnd::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxEmergEnd, false, "UE C already received a " << McpttCallMsgGrpEmergEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCT1ToT2, true, "UE C never made transition from " << McpttCallTypeMachineGrpBasic::T1 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T2, "UE C in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T2 << ".");
      m_ueCRxEmergEnd = true;
      if (m_ueBRxEmergEnd == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE C timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE C timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE C timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE C timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE C counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE C counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeExplicitDowngradeFromEmerg::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeExplicitDowngradeFromEmerg::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T1
      && newStateId == McpttCallTypeMachineGrpBasic::T2)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxEmergEnd, false, "UE C already received a " << McpttCallMsgGrpEmergEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCT1ToT2, false, "UE C already made transition from " << McpttCallTypeMachineGrpBasic::T1 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      m_ueCT1ToT2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE C timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE C timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE C timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE C timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE C counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE C counter " << cfg12->GetId () << " is not one.");
}

McpttCallTypeExplicitDowngradeFromImmPeril::McpttCallTypeExplicitDowngradeFromImmPeril (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAT3ToT2 (false),
    m_ueATxImmPerilEnd (false),
    m_ueBRxImmPerilEnd (false),
    m_ueBT3ToT2 (false),
    m_ueCRxImmPerilEnd (false),
    m_ueCT3ToT2 (false)
{ }

void
McpttCallTypeExplicitDowngradeFromImmPeril::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueATfg6 = ueAMachine->GetTfg6 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  Ptr<McpttTimer> ueATfg14 = ueATypeMachine->GetTfg14 ();
  Ptr<McpttTimer> ueBTfg14 = ueBTypeMachine->GetTfg14 ();
  Ptr<McpttTimer> ueCTfg14 = ueCTypeMachine->GetTfg14 ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
  Time upgradeTime = Seconds (2.2);
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
  ueAMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T3);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromImmPeril::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromImmPeril::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromImmPeril::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromImmPeril::UeATimerExpCb, this));

  ueATfg2->SetDelay (ueAMachine->CalcDelayForPeriodicAnnoun ());
  ueATfg6->SetDelay (ueAMachine->CalcDelayForMaxDuration ());
  ueATfg14->SetDelay (ueATypeMachine->CalcDelayForTfg14 ());

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T3);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromImmPeril::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromImmPeril::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromImmPeril::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromImmPeril::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());
  ueBTfg14->SetDelay (ueBTypeMachine->CalcDelayForTfg14 ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T3);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromImmPeril::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromImmPeril::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromImmPeril::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeExplicitDowngradeFromImmPeril::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (ueCMachine->CalcDelayForPeriodicAnnoun ());
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());
  ueCTfg14->SetDelay (ueCTypeMachine->CalcDelayForTfg14 ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg14);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg14);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg14);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (upgradeTime, &McpttPttApp::DowngradeCallType, ueAPttApp);
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAT3ToT2, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxImmPerilEnd, true, "UE A never sent a " << McpttCallMsgGrpImmPerilEnd::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxImmPerilEnd, true, "UE B never received a " << McpttCallMsgGrpImmPerilEnd::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBT3ToT2, true, "UE B never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxImmPerilEnd, true, "UE C never received a " << McpttCallMsgGrpImmPerilEnd::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCT3ToT2, true, "UE C never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T3
      && newStateId == McpttCallTypeMachineGrpBasic::T2)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT3ToT2, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxImmPerilEnd, true, "UE A never sent a " << McpttCallMsgGrpImmPerilEnd::GetTypeId () << ".");
      m_ueAT3ToT2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), true, "UE A timer " << tfg12->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpImmPerilEnd::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT3ToT2, false, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxImmPerilEnd, false, "UE A already received a " << McpttCallMsgGrpImmPerilEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T3, "UE A in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T3 << ".");
      m_ueATxImmPerilEnd = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), true, "UE A timer " << tfg12->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpImmPerilEnd::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxImmPerilEnd, false, "UE B already received a " << McpttCallMsgGrpImmPerilEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBT3ToT2, true, "UE B never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T2, "UE B in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T2 << ".");
      m_ueBRxImmPerilEnd = true;
      if (m_ueCRxImmPerilEnd == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE B timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE B timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE B timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE B timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE B counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE B counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T3
      && newStateId == McpttCallTypeMachineGrpBasic::T2)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxImmPerilEnd, false, "UE B already received a " << McpttCallMsgGrpImmPerilEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBT3ToT2, false, "UE B already made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      m_ueBT3ToT2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE B timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE B timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE B timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE B timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE B counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE B counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpImmPerilEnd::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxImmPerilEnd, false, "UE C already received a " << McpttCallMsgGrpImmPerilEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCT3ToT2, true, "UE C never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T2, "UE C in state " << stateId << " but should be in " << McpttCallTypeMachineGrpBasic::T2 << ".");
      m_ueCRxImmPerilEnd = true;
      if (m_ueBRxImmPerilEnd == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE C timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE C timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE C timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE C timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE C counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE C counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeExplicitDowngradeFromImmPeril::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T3
      && newStateId == McpttCallTypeMachineGrpBasic::T2)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxImmPerilEnd, false, "UE C already received a " << McpttCallMsgGrpImmPerilEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCT3ToT2, false, "UE C already made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      m_ueCT3ToT2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE C timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE C timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE C timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE C timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE C counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE C counter " << cfg12->GetId () << " is not one.");
}

McpttCallTypeImplicitDowngradeFromEmerg::McpttCallTypeImplicitDowngradeFromEmerg (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAT1ToT2 (false),
    m_ueATfg13Exp (false)
{ }

void
McpttCallTypeImplicitDowngradeFromEmerg::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueATfg6 = ueAMachine->GetTfg6 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  Ptr<McpttTimer> ueATfg13 = ueATypeMachine->GetTfg13 ();
  Ptr<McpttTimer> ueBTfg13 = ueBTypeMachine->GetTfg13 ();
  Ptr<McpttTimer> ueCTfg13 = ueCTypeMachine->GetTfg13 ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
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
  ueAMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromEmerg::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromEmerg::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromEmerg::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromEmerg::UeATimerExpCb, this));

  ueATfg2->SetDelay (ueAMachine->CalcDelayForPeriodicAnnoun ());
  ueATfg6->SetDelay (ueAMachine->CalcDelayForMaxDuration ());
  ueATfg13->SetDelay (Seconds (2.0));

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromEmerg::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromEmerg::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromEmerg::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromEmerg::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());
  ueBTfg13->SetDelay (ueBTypeMachine->CalcDelayForTfg13 ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromEmerg::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromEmerg::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromEmerg::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromEmerg::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (ueCMachine->CalcDelayForPeriodicAnnoun ());
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());
  ueCTfg13->SetDelay (ueCTypeMachine->CalcDelayForTfg13 ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg13);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg13);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg13);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
}

void
McpttCallTypeImplicitDowngradeFromEmerg::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAT1ToT2, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T1 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATfg13Exp, true, "UE A timer TFG13 never expired.");
}

void
McpttCallTypeImplicitDowngradeFromEmerg::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (timer.GetId () == tfg13->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT1ToT2, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T1 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg13Exp, false, "UE A timer TFG13 already expired.");
      m_ueATfg13Exp = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T2, "UE A not in state " << McpttCallTypeMachineGrpBasic::T2 << ".");
}

void
McpttCallTypeImplicitDowngradeFromEmerg::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeImplicitDowngradeFromEmerg::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T1
      && newStateId == McpttCallTypeMachineGrpBasic::T2)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT1ToT2, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T1 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg13Exp, false, "UE A timer TFG13 already expired.");
      m_ueAT1ToT2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeImplicitDowngradeFromEmerg::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypeImplicitDowngradeFromEmerg::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeImplicitDowngradeFromEmerg::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeImplicitDowngradeFromEmerg::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeImplicitDowngradeFromEmerg::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
}

void
McpttCallTypeImplicitDowngradeFromEmerg::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeImplicitDowngradeFromEmerg::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeImplicitDowngradeFromEmerg::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeImplicitDowngradeFromEmerg::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
}

McpttCallTypeImplicitDowngradeFromImmPeril::McpttCallTypeImplicitDowngradeFromImmPeril (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAT3ToT2 (false),
    m_ueATfg14Exp (false)
{ }

void
McpttCallTypeImplicitDowngradeFromImmPeril::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueATfg6 = ueAMachine->GetTfg6 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  Ptr<McpttTimer> ueATfg14 = ueATypeMachine->GetTfg14 ();
  Ptr<McpttTimer> ueBTfg14 = ueBTypeMachine->GetTfg14 ();
  Ptr<McpttTimer> ueCTfg14 = ueCTypeMachine->GetTfg14 ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
  Time upgradeTime = Seconds (2.2);
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
  ueAMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T3);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromImmPeril::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromImmPeril::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromImmPeril::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromImmPeril::UeATimerExpCb, this));

  ueATfg2->SetDelay (ueAMachine->CalcDelayForPeriodicAnnoun ());
  ueATfg6->SetDelay (ueAMachine->CalcDelayForMaxDuration ());
  ueATfg14->SetDelay (Seconds (2.0));

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T3);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromImmPeril::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromImmPeril::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromImmPeril::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromImmPeril::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());
  ueBTfg14->SetDelay (ueBTypeMachine->CalcDelayForTfg14 ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T3);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromImmPeril::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromImmPeril::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromImmPeril::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeImplicitDowngradeFromImmPeril::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (ueCMachine->CalcDelayForPeriodicAnnoun ());
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());
  ueCTfg14->SetDelay (ueCTypeMachine->CalcDelayForTfg14 ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg14);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg14);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg14);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAT3ToT2, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATfg14Exp, true, "UE A timer TFG14 never expired.");
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (timer.GetId () == tfg14->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT3ToT2, true, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg14Exp, false, "UE A timer TFG14 already expired.");
      m_ueATfg14Exp = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (stateId, McpttCallTypeMachineGrpBasic::T2, "UE A in state " << stateId << " not " << McpttCallTypeMachineGrpBasic::T2 << ".");
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T3
      && newStateId == McpttCallTypeMachineGrpBasic::T2)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT3ToT2, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T2 << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATfg14Exp, false, "UE A timer TFG14 already expired.");
      m_ueAT3ToT2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeImplicitDowngradeFromImmPeril::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
}

McpttCallTypeReleaseAfterCallEstab::McpttCallTypeReleaseAfterCallEstab (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAT2ToT0 (false)
{ }

void
McpttCallTypeReleaseAfterCallEstab::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueATfg5 = ueAMachine->GetTfg5 ();

  Ptr<McpttTimer> ueATfg6 = ueAMachine->GetTfg6 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
  Time exitTime = Seconds (2.2);
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
  ueAMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeReleaseAfterCallEstab::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeReleaseAfterCallEstab::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeReleaseAfterCallEstab::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeReleaseAfterCallEstab::UeATimerExpCb, this));

  ueATfg2->SetDelay (ueAMachine->CalcDelayForPeriodicAnnoun ());
  ueATfg5->SetDelay (Seconds (1.0));
  ueATfg6->SetDelay (ueAMachine->CalcDelayForMaxDuration ());

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeReleaseAfterCallEstab::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeReleaseAfterCallEstab::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeReleaseAfterCallEstab::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeReleaseAfterCallEstab::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime);
  ueCTypeMachine->SetLastChgUserId (origId);
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeReleaseAfterCallEstab::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeReleaseAfterCallEstab::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeReleaseAfterCallEstab::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeReleaseAfterCallEstab::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (ueCMachine->CalcDelayForPeriodicAnnoun ());
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (exitTime, &McpttPttApp::ReleaseCall, ueAPttApp);
}

void
McpttCallTypeReleaseAfterCallEstab::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAT2ToT0, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T0 << ".");
}

void
McpttCallTypeReleaseAfterCallEstab::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypeReleaseAfterCallEstab::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeReleaseAfterCallEstab::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T2
      && newStateId == McpttCallTypeMachineGrpBasic::T0)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAT2ToT0, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T0 << ".");
      m_ueAT2ToT0 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), false, "UE A timer " << tfg13->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeReleaseAfterCallEstab::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypeReleaseAfterCallEstab::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeReleaseAfterCallEstab::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeReleaseAfterCallEstab::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeReleaseAfterCallEstab::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
}

void
McpttCallTypeReleaseAfterCallEstab::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeReleaseAfterCallEstab::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeReleaseAfterCallEstab::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeReleaseAfterCallEstab::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
}

McpttCallTypeMerge::McpttCallTypeMerge (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueARxAnnoun (false),
    m_ueAT2ToT1 (false),
    m_ueBRxAnnoun (false),
    m_ueBT3ToT1 (false)
{ }

void
McpttCallTypeMerge::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (McpttCallMachineGrpBasic::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<McpttCallMachineGrpBasic> ueAMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachineGrpBasic> ueBMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (1));
  Ptr<McpttCallMachineGrpBasic> ueCMachine = DynamicCast<McpttCallMachineGrpBasic, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfg2 = ueAMachine->GetTfg2 ();
  Ptr<McpttTimer> ueBTfg2 = ueBMachine->GetTfg2 ();
  Ptr<McpttTimer> ueCTfg2 = ueCMachine->GetTfg2 ();

  Ptr<McpttTimer> ueATfg6 = ueAMachine->GetTfg6 ();
  Ptr<McpttTimer> ueBTfg6 = ueBMachine->GetTfg6 ();
  Ptr<McpttTimer> ueCTfg6 = ueCMachine->GetTfg6 ();

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueCEmergMachine = ueCMachine->GetEmergMachine ();

  Ptr<TestCallTypeMachine> ueATypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueBTypeMachine = CreateObject<TestCallTypeMachine> ();
  Ptr<TestCallTypeMachine> ueCTypeMachine = CreateObject<TestCallTypeMachine> ();

  ueAMachine->SetTypeMachine (ueATypeMachine);
  ueBMachine->SetTypeMachine (ueBTypeMachine);
  ueCMachine->SetTypeMachine (ueCTypeMachine);

  uint32_t grpId = 1;
  uint16_t callId = 1;
  uint16_t refInt = 10000;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
  Time exitTime = Seconds (2.2);
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
  ueAMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetLastChgTime (startTime);
  ueATypeMachine->SetLastChgUserId (origId);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BASIC_GROUP));
  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::BASIC_GROUP);
  ueATypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T2);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeMerge::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeMerge::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeMerge::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeMerge::UeATimerExpCb, this));

  ueATfg2->SetDelay (ueAMachine->CalcDelayForPeriodicAnnoun ());
  ueATfg6->SetDelay (ueAMachine->CalcDelayForMaxDuration ());

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetRefInt (refInt);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartTime (startTime);
  ueBMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetLastChgTime (startTime);
  ueBTypeMachine->SetLastChgUserId (origId);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP));
  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::IMMINENT_PERIL_GROUP);
  ueBTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T3);
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeMerge::UeBTxCb, this));
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeMerge::UeBRxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeMerge::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeMerge::UeBTimerExpCb, this));

  ueBTfg2->SetDelay (ueBMachine->CalcDelayForPeriodicAnnoun ());
  ueBTfg6->SetDelay (ueBMachine->CalcDelayForMaxDuration ());

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetRefInt (refInt);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetStartTime (startTime);
  ueCMachine->SetState (McpttCallMachineGrpBasicStateS3::GetInstance ());

  ueCEmergMachine->SetEmerg (false);
  ueCEmergMachine->SetStarted (true);

  ueCTypeMachine->SetStarted (true);
  ueCTypeMachine->SetLastChgTime (startTime + (Seconds (1)));
  ueCTypeMachine->SetLastChgUserId (ueCPttApp->GetUserId ());
  ueCTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_GROUP));
  ueCTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_GROUP);
  ueCTypeMachine->SetStateId (McpttCallTypeMachineGrpBasic::T1);
  ueCTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypeMerge::UeCTxCb, this));
  ueCTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypeMerge::UeCRxCb, this));
  ueCTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypeMerge::UeCStateChangeCb, this));
  ueCTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypeMerge::UeCTimerExpCb, this));

  ueCTfg2->SetDelay (Seconds (2.0));
  ueCTfg6->SetDelay (ueCMachine->CalcDelayForMaxDuration ());

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfg6);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg2);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueCTfg6);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
}

void
McpttCallTypeMerge::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, true, "UE A never received " << McpttCallMsgGrpAnnoun::GetTypeId ());
  NS_TEST_ASSERT_MSG_EQ (m_ueAT2ToT1, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, true, "UE B never received " << McpttCallMsgGrpAnnoun::GetTypeId ());
  NS_TEST_ASSERT_MSG_EQ (m_ueBT3ToT1, true, "UE B never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
}

void
McpttCallTypeMerge::UeATimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypeMerge::UeARxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received " << McpttCallMsgGrpAnnoun::GetTypeId ());
      NS_TEST_ASSERT_MSG_EQ (m_ueAT2ToT1, true, "UE A never made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueARxAnnoun = true;
      if (m_ueBRxAnnoun == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE A timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeMerge::UeAStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T2
      && newStateId == McpttCallTypeMachineGrpBasic::T1)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAnnoun, false, "UE A already received " << McpttCallMsgGrpAnnoun::GetTypeId ());
      NS_TEST_ASSERT_MSG_EQ (m_ueAT2ToT1, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T2 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueAT2ToT1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE A timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeMerge::UeATxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypeMerge::UeBTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeMerge::UeBRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (msg.IsA (McpttCallMsgGrpAnnoun::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE B already received " << McpttCallMsgGrpAnnoun::GetTypeId ());
      NS_TEST_ASSERT_MSG_EQ (m_ueBT3ToT1, true, "UE B never made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueBRxAnnoun = true;
      if (m_ueARxAnnoun == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE A timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeMerge::UeBTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeMerge::UeBStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  Ptr<McpttTimer> tfg11 = machine.GetTfg11 ();
  Ptr<McpttTimer> tfg12 = machine.GetTfg12 ();
  Ptr<McpttTimer> tfg13 = machine.GetTfg13 ();
  Ptr<McpttTimer> tfg14 = machine.GetTfg14 ();
  Ptr<McpttCounter> cfg11 = machine.GetCfg11 ();
  Ptr<McpttCounter> cfg12 = machine.GetCfg12 ();
  McpttEntityId stateId = machine.GetStateId ();

  if (oldStateId == McpttCallTypeMachineGrpBasic::T3
      && newStateId == McpttCallTypeMachineGrpBasic::T1)
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAnnoun, false, "UE A already received " << McpttCallMsgGrpAnnoun::GetTypeId ());
      NS_TEST_ASSERT_MSG_EQ (m_ueBT3ToT1, false, "UE A already made transition from " << McpttCallTypeMachineGrpBasic::T3 << " to " << McpttCallTypeMachineGrpBasic::T1 << ".");
      m_ueBT3ToT1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfg11->IsRunning (), false, "UE A timer " << tfg11->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg12->IsRunning (), false, "UE A timer " << tfg12->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfg13->IsRunning (), true, "UE A timer " << tfg13->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfg14->IsRunning (), false, "UE A timer " << tfg14->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfg11->GetValue (), 1, "UE A counter " << cfg11->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfg12->GetValue (), 1, "UE A counter " << cfg12->GetId () << " is not one.");
}

void
McpttCallTypeMerge::UeCTimerExpCb (const TestCallTypeMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypeMerge::UeCRxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypeMerge::UeCTxCb (const TestCallTypeMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypeMerge::UeCStateChangeCb (const TestCallTypeMachine& machine, const McpttEntityId& oldStateId, const McpttEntityId& newStateId)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C state transition from " << oldStateId << " to " << newStateId << ".");
}


McpttCallTypeControlTestSuite::McpttCallTypeControlTestSuite (void)
  : TestSuite ("mcptt-call-type-control", TestSuite::SYSTEM)
{
  AddTestCase (new McpttCallTypeInitEstabNewCall (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeInitJoinEmergCall (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeInitJoinImmPerilCall (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeInitJoinBasicCall (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeInitJoinEmergCallUserAck (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeInitJoinEmergCallNoUserAck (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeUpgradeFromBasicCall (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeUpgradeFromImmPerilCall (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeExplicitDowngradeFromEmerg (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeExplicitDowngradeFromImmPeril (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeImplicitDowngradeFromEmerg (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeImplicitDowngradeFromImmPeril (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeReleaseAfterCallEstab (), TestCase::QUICK);
  AddTestCase (new McpttCallTypeMerge (), TestCase::QUICK);
}

} // namespace tests
} // namespace psc
} // namespace ns3

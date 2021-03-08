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
#include <ns3/mcptt-call-machine-grp-broadcast.h>
#include <ns3/mcptt-call-msg.h>
#include <ns3/mcptt-ptt-app.h>

#include "mcptt-test-case.h"
#include "mcptt-test-case-config.h"
#include "mcptt-msg-dropper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttBroadcastCallControlTest");

namespace psc {
namespace tests {

class BroadcastTestCallMachine : public McpttCallMachineGrpBroadcast
{
public:
  static TypeId GetTypeId (void);
  BroadcastTestCallMachine (void);
  virtual ~BroadcastTestCallMachine (void);
  virtual void ChangeState (Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Receive (const McpttCallMsg& msg);
  virtual void Start (void);
  virtual void Send (const McpttCallMsg& msg);

protected:
  virtual void ExpiryOfTfb1 (void);
  virtual void ExpiryOfTfb2 (void);
  virtual void ExpiryOfTfb3 (void);

private:
  Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> m_postRxCb;
  Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> m_postTimerExpCb;
  Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> m_postTxCb;
  Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> m_preRxCb;
  Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> m_preTimerExpCb;
  Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> m_preTxCb;
  Ptr<McpttCallMachineGrpBroadcastState> m_startState;
  Callback<void, const BroadcastTestCallMachine&, Ptr<McpttCallMachineGrpBroadcastState>, Ptr<McpttCallMachineGrpBroadcastState> > m_stateChangeCb;

public:
  virtual Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> GetPostRxCb (void) const;
  virtual Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> GetPostTimerExpCb (void) const;
  virtual Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> GetPostTxCb (void) const;
  virtual Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> GetPreRxCb (void) const;
  virtual Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> GetPreTimerExpCb (void) const;
  virtual Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> GetPreTxCb (void) const;
  virtual Ptr<McpttCallMachineGrpBroadcastState> GetStartState (void) const;
  virtual Callback<void, const BroadcastTestCallMachine&, Ptr<McpttCallMachineGrpBroadcastState>, Ptr<McpttCallMachineGrpBroadcastState> > GetStateChangeCb (void) const;
  virtual void SetPostRxCb (const Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>  postRxCb);
  virtual void SetPostTimerExpCb (const Callback<void, const BroadcastTestCallMachine&, const McpttTimer&>  timerExpCb);
  virtual void SetPostTxCb (const Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>  postTxCb);
  virtual void SetPreRxCb (const Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>  preRxCb);
  virtual void SetPreTimerExpCb (const Callback<void, const BroadcastTestCallMachine&, const McpttTimer&>  timerExpCb);
  virtual void SetPreTxCb (const Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>  preTxCb);
  virtual void SetStartState (Ptr<McpttCallMachineGrpBroadcastState>  startState);
  virtual void SetStateChangeTestCb (const Callback<void, const BroadcastTestCallMachine&, Ptr<McpttCallMachineGrpBroadcastState>, Ptr<McpttCallMachineGrpBroadcastState> >  stateChangeCb);
};

/**
 * UE A decides to establish a Broadcast Group Call (no existing one yet). UE
 * B requires user acklowedgement when accpeting the call, while UE C does
 * not. UE B accepts the call.
 */
class BroadcastCallScenario1 : public McpttTestCase
{
public:
  BroadcastCallScenario1 (const std::string& name = "BroadcastCallScenario1", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual void UeATimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual void UeBTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual void UeCTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAB1ToB2;
  bool m_ueATxGrpBroadcast;
  bool m_ueBB1ToB3;
  bool m_ueBB3ToB2;
  bool m_ueBRxGrpBroadcast;
  bool m_ueCB1ToB2;
  bool m_ueCRxGrpBroadcast;
};

/**
 * UE A decides to establish a Broadcast Group Call (no existing one yet). UE
 * B requires user acklowedgement when accpeting the call, while UE C does
 * not. UE B rejects the call.
 */
class BroadcastCallScenario2 : public McpttTestCase
{
public:
  BroadcastCallScenario2 (const std::string& name = "BroadcastCallScenario2", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual void UeATimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBNewCallCb (uint16_t callId);
  virtual void UeBRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual void UeBTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual void UeCTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAB1ToB2;
  bool m_ueATxGrpBroadcast;
  bool m_ueBB1ToB3;
  bool m_ueBB3ToB4;
  bool m_ueBRxGrpBroadcast;
  bool m_ueCB1ToB2;
  bool m_ueCRxGrpBroadcast;
};

/**
 * UE A, B, and C are part of an on-going call that UE A originated. UE A
 * then releases the call.
 */
class BroadcastCallScenario3 : public McpttTestCase
{
public:
  BroadcastCallScenario3 (const std::string& name = "BroadcastCallScenario3", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual void UeATimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual void UeBTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual void UeCTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAB2ToB1;
  bool m_ueATxGrpBroadcastEnd;
  bool m_ueBB2ToB1;
  bool m_ueBRxGrpBroadcastEnd;
  bool m_ueCB2ToB1;
  bool m_ueCRxGrpBroadcastEnd;
};

/**
 * UE A, B, and C are part of an on-going call that UE A originated. UE B
 * then releases the call.
 */
class BroadcastCallScenario4 : public McpttTestCase
{
public:
  BroadcastCallScenario4 (const std::string& name = "BroadcastCallScenario4", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual void UeATimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual void UeBTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeCStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState);
  virtual void UeCTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeCTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAB2ToB1;
  bool m_ueAExpTfb1;
  bool m_ueAExpTfb2;
  bool m_ueATxGrpBroadcast;
  bool m_ueBB2ToB4;
  bool m_ueBB4ToB1;
  bool m_ueBExpTfb1;
  bool m_ueBRxGrpBroadcast;
  bool m_ueCB2ToB1;
  bool m_ueCExpTfb1;
  bool m_ueCRxGrpBroadcast;
};

class McpttBroadcastCallControlTestSuite : public TestSuite
{
public:
  McpttBroadcastCallControlTestSuite (void);
};

/***************************************************************
 *  Implementation of the code declared above.
 ***************************************************************/

static McpttBroadcastCallControlTestSuite callSuite;

TypeId
BroadcastTestCallMachine::GetTypeId (void)
{
  static TypeId tid = TypeId ("BroadcastTestCallMachine")
    .SetParent<McpttCallMachineGrpBroadcast> ()
    .AddConstructor<BroadcastTestCallMachine> ()
  ;

  return tid;
}

BroadcastTestCallMachine::BroadcastTestCallMachine (void)
  : McpttCallMachineGrpBroadcast (),
    m_postRxCb (MakeNullCallback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> ()),
    m_postTimerExpCb (MakeNullCallback<void, const BroadcastTestCallMachine&, const McpttTimer&> ()),
    m_postTxCb (MakeNullCallback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> ()),
    m_preRxCb (MakeNullCallback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> ()),
    m_preTimerExpCb (MakeNullCallback<void, const BroadcastTestCallMachine&, const McpttTimer&> ()),
    m_preTxCb (MakeNullCallback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> ()),
    m_startState (McpttCallMachineGrpBroadcastStateB1::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const BroadcastTestCallMachine&, Ptr<McpttCallMachineGrpBroadcastState>, Ptr<McpttCallMachineGrpBroadcastState> > ())
{ }

BroadcastTestCallMachine::~BroadcastTestCallMachine (void)
{ }

void
BroadcastTestCallMachine::Receive (const McpttCallMsg& msg)
{
  Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallMachineGrpBroadcast::Receive (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
BroadcastTestCallMachine::Send (const McpttCallMsg& msg)
{
  Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> postTxCb = GetPostTxCb ();
  Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&> preTxCb = GetPreTxCb ();

  if (!preTxCb.IsNull ())
    {
      preTxCb (*this, msg);
    }

  McpttCallMachineGrpBroadcast::Send (msg);

  if (!postTxCb.IsNull ())
    {
      postTxCb (*this, msg);
    }
}

void
BroadcastTestCallMachine::ExpiryOfTfb1 (void)
{
  Ptr<McpttTimer> tfg = GetTfb1 ();
  Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachineGrpBroadcast::ExpiryOfTfb1 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
BroadcastTestCallMachine::ExpiryOfTfb2 (void)
{
  Ptr<McpttTimer> tfg = GetTfb2 ();
  Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachineGrpBroadcast::ExpiryOfTfb2 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
BroadcastTestCallMachine::ExpiryOfTfb3 (void)
{
  Ptr<McpttTimer> tfg = GetTfb3 ();
  Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const BroadcastTestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachineGrpBroadcast::ExpiryOfTfb3 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
BroadcastTestCallMachine::Start (void)
{
  Ptr<McpttCallMachineGrpBroadcastState> startState = GetStartState ();

  SetState (startState);

  McpttCallMachineGrpBroadcast::Start ();
}

void
BroadcastTestCallMachine::ChangeState (Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttCallMachineGrpBroadcastState> oldState = GetState ();
  Callback<void, const BroadcastTestCallMachine&, Ptr<McpttCallMachineGrpBroadcastState>, Ptr<McpttCallMachineGrpBroadcastState> > stateChangeCb = GetStateChangeCb ();

  McpttCallMachineGrpBroadcast::ChangeState (newState);

  if (!stateChangeCb.IsNull ())
    {
      stateChangeCb (*this, oldState, newState);
    }
}

TypeId
BroadcastTestCallMachine::GetInstanceTypeId (void) const
{
  return BroadcastTestCallMachine::GetTypeId ();
}

Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>
BroadcastTestCallMachine::GetPostRxCb (void) const
{
  return m_postRxCb;
}

Callback<void, const BroadcastTestCallMachine&, const McpttTimer&>
BroadcastTestCallMachine::GetPostTimerExpCb (void) const
{
  return m_postTimerExpCb;
}


Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>
BroadcastTestCallMachine::GetPostTxCb (void) const
{
  return m_postTxCb;
}

Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>
BroadcastTestCallMachine::GetPreRxCb (void) const
{
  return m_preRxCb;
}

Callback<void, const BroadcastTestCallMachine&, const McpttTimer&>
BroadcastTestCallMachine::GetPreTimerExpCb (void) const
{
  return m_preTimerExpCb;
}

Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>
BroadcastTestCallMachine::GetPreTxCb (void) const
{
  return m_preTxCb;
}

Ptr<McpttCallMachineGrpBroadcastState>
BroadcastTestCallMachine::GetStartState (void) const
{
  return m_startState;
}

Callback<void, const BroadcastTestCallMachine&, Ptr<McpttCallMachineGrpBroadcastState>, Ptr<McpttCallMachineGrpBroadcastState> >
BroadcastTestCallMachine::GetStateChangeCb (void) const
{
  return m_stateChangeCb;
}

void
BroadcastTestCallMachine::SetPostRxCb (const Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>  postRxCb)
{
  m_postRxCb = postRxCb;
}

void
BroadcastTestCallMachine::SetPostTimerExpCb (const Callback<void, const BroadcastTestCallMachine&, const McpttTimer&>  timerExpCb)
{
  m_postTimerExpCb = timerExpCb;
}

void
BroadcastTestCallMachine::SetPostTxCb (const Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>  postTxCb)
{
  m_postTxCb = postTxCb;
}

void
BroadcastTestCallMachine::SetPreRxCb (const Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>  preRxCb)
{
  m_preRxCb = preRxCb;
}

void
BroadcastTestCallMachine::SetPreTimerExpCb (const Callback<void, const BroadcastTestCallMachine&, const McpttTimer&>  timerExpCb)
{
  m_preTimerExpCb = timerExpCb;
}

void
BroadcastTestCallMachine::SetPreTxCb (const Callback<void, const BroadcastTestCallMachine&, const McpttCallMsg&>  preTxCb)
{
  m_preTxCb = preTxCb;
}

void
BroadcastTestCallMachine::SetStartState (Ptr<McpttCallMachineGrpBroadcastState>  startState)
{
  m_startState = startState;
}

void
BroadcastTestCallMachine::SetStateChangeTestCb (const Callback<void, const BroadcastTestCallMachine&, Ptr<McpttCallMachineGrpBroadcastState>, Ptr<McpttCallMachineGrpBroadcastState> >  stateChangeCb)
{
  m_stateChangeCb = stateChangeCb;
}

BroadcastCallScenario1::BroadcastCallScenario1 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAB1ToB2 (false),
    m_ueATxGrpBroadcast (false),
    m_ueBB1ToB3 (false),
    m_ueBB3ToB2 (false),
    m_ueBRxGrpBroadcast (false),
    m_ueCB1ToB2 (false),
    m_ueCRxGrpBroadcast (false)
{ }

void
BroadcastCallScenario1::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (BroadcastTestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<BroadcastTestCallMachine> ueAMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<BroadcastTestCallMachine> ueBMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<BroadcastTestCallMachine> ueCMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (2));

  // UE A
  ueAMachine->SetStartState (McpttCallMachineGrpBroadcastStateB1::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&BroadcastCallScenario1::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&BroadcastCallScenario1::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&BroadcastCallScenario1::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&BroadcastCallScenario1::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetStartState (McpttCallMachineGrpBroadcastStateB1::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&BroadcastCallScenario1::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&BroadcastCallScenario1::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&BroadcastCallScenario1::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&BroadcastCallScenario1::UeBTimerExpCb, this));
  ueBMachine->SetAttribute ("UserAckRequired", BooleanValue (true));

  // UE C
  ueCMachine->SetStartState (McpttCallMachineGrpBroadcastStateB1::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&BroadcastCallScenario1::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&BroadcastCallScenario1::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&BroadcastCallScenario1::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&BroadcastCallScenario1::UeCTimerExpCb, this));


  Simulator::Schedule (Seconds (2.2), &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
BroadcastCallScenario1::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAB1ToB2, true, "UE A never made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcast, true, "UE A never sent " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBB1ToB3, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBB3ToB2, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, true, "UE B never received " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCB1ToB2, true, "UE C never made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcast, true, "UE C never received " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
}

void
BroadcastCallScenario1::UeATimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
BroadcastCallScenario1::UeARxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message " << msg << ".");
}

void
BroadcastCallScenario1::UeAStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB1::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcast, true, "UE A never sent " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAB1ToB2, false, "UE A already made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
      m_ueAB1ToB2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE A TFB1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), true, "UE A TFB2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE A TFB3 is running.");
}

void
BroadcastCallScenario1::UeATxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpBroadcast::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB1::GetInstance (), "UE A is not in the " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " state.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcast, false, "UE A already sent " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAB1ToB2, false, "UE A already made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
      m_ueATxGrpBroadcast = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE A TFB1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE A TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE A TFB3 is running.");
}

void
BroadcastCallScenario1::UeBTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
BroadcastCallScenario1::UeBRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpBroadcast::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB2::GetInstance (), "UE B is not in the " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " state.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, false, "UE B already received " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB1ToB3, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB3ToB2, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
      m_ueBRxGrpBroadcast = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE B timer TFB1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE B timer TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE B timer TFB3 is running.");
}

void
BroadcastCallScenario1::UeBTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
BroadcastCallScenario1::UeBStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB1::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, false, "UE B already received " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB1ToB3, false, "UE B already made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB3ToB2, false, "UE B already made transition from " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE B TFB1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), true, "UE B TFB3 is not running.");
      m_ueBB1ToB3 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB3::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, false, "UE B already received " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB1ToB3, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB3ToB2, false, "UE B already made transition from " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE B TFB1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE B TFB3 is running.");
      m_ueBB3ToB2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE B TFB2 is not running.");
}

void
BroadcastCallScenario1::UeCTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
BroadcastCallScenario1::UeCRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpBroadcast::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB2::GetInstance (), "UE C is not in the " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " state.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcast, false, "UE C already recieved " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCB1ToB2, true, "UE C never made transiton from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
      m_ueCRxGrpBroadcast = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE C timer TFB1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE C timer TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE C timer TFB3 is running.");
}

void
BroadcastCallScenario1::UeCTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
BroadcastCallScenario1::UeCStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB1::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcast, false, "UE C already received " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCB1ToB2, false, "UE C already made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
      m_ueCB1ToB2 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE C TFB1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE C TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE C TFB3 is running.");
}

BroadcastCallScenario2::BroadcastCallScenario2 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAB1ToB2 (false),
    m_ueATxGrpBroadcast (false),
    m_ueBB1ToB3 (false),
    m_ueBB3ToB4 (false),
    m_ueBRxGrpBroadcast (false),
    m_ueCB1ToB2 (false),
    m_ueCRxGrpBroadcast (false)
{ }

void
BroadcastCallScenario2::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (BroadcastTestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<BroadcastTestCallMachine> ueAMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<BroadcastTestCallMachine> ueBMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<BroadcastTestCallMachine> ueCMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (2));

  // UE A
  ueAMachine->SetStartState (McpttCallMachineGrpBroadcastStateB1::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&BroadcastCallScenario2::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&BroadcastCallScenario2::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&BroadcastCallScenario2::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&BroadcastCallScenario2::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetStartState (McpttCallMachineGrpBroadcastStateB1::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&BroadcastCallScenario2::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&BroadcastCallScenario2::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&BroadcastCallScenario2::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&BroadcastCallScenario2::UeBTimerExpCb, this));
  ueBMachine->SetNewCallCb (MakeCallback (&BroadcastCallScenario2::UeBNewCallCb, this));
  ueBMachine->SetAttribute ("UserAckRequired", BooleanValue ((true)));

  // UE C
  ueCMachine->SetStartState (McpttCallMachineGrpBroadcastStateB1::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&BroadcastCallScenario2::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&BroadcastCallScenario2::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&BroadcastCallScenario2::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&BroadcastCallScenario2::UeCTimerExpCb, this));

  Simulator::Schedule (Seconds (2.2), &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
BroadcastCallScenario2::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAB1ToB2, true, "UE A never made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcast, true, "UE A never sent " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBB1ToB3, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBB3ToB4, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, true, "UE B never received " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCB1ToB2, true, "UE C never made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcast, true, "UE C never received " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
}

void
BroadcastCallScenario2::UeATimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
BroadcastCallScenario2::UeARxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message " << msg << ".");
}

void
BroadcastCallScenario2::UeAStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB1::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcast, true, "UE A never sent " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAB1ToB2, false, "UE A already made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
      m_ueAB1ToB2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE A TFB1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), true, "UE A TFB2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE A TFB3 is running.");
}

void
BroadcastCallScenario2::UeATxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpBroadcast::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB1::GetInstance (), "UE A is not in the " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " state.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcast, false, "UE A already sent " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAB1ToB2, false, "UE A already made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
      m_ueATxGrpBroadcast = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE A TFB1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE A TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE A TFB3 is running.");
}

void
BroadcastCallScenario2::UeBNewCallCb (uint16_t callId)
{
  NS_LOG_FUNCTION (this);

  Ptr<McpttCallMachineGrpBroadcast> callMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (1));
  callMachine->RejectCall ();
}

void
BroadcastCallScenario2::UeBTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
BroadcastCallScenario2::UeBRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpBroadcast::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB4::GetInstance (), "UE B is not in the " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << " state.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, false, "UE B already received " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB1ToB3, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB3ToB4, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << ".");
      m_ueBRxGrpBroadcast = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE B timer TFB1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE B timer TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE B timer TFB3 is running.");
}

void
BroadcastCallScenario2::UeBTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
BroadcastCallScenario2::UeBStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB1::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, false, "UE B already received " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB1ToB3, false, "UE B already made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB3ToB4, false, "UE B already made transition from " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE B TFB1 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), true, "UE B TFB3 is not running.");
      m_ueBB1ToB3 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB3::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB4::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, false, "UE B already received " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB1ToB3, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB3ToB4, false, "UE B already made transition from " << McpttCallMachineGrpBroadcastStateB3::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE B TFB1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE B TFB3 is running.");
      m_ueBB3ToB4 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE B TFB2 is not running.");
}

void
BroadcastCallScenario2::UeCTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
BroadcastCallScenario2::UeCRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpBroadcast::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB2::GetInstance (), "UE C is not in the " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " state.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcast, false, "UE C already recieved " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCB1ToB2, true, "UE C never made transiton from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
      m_ueCRxGrpBroadcast = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE C timer TFB1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE C timer TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE C timer TFB3 is running.");
}

void
BroadcastCallScenario2::UeCTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
BroadcastCallScenario2::UeCStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB1::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcast, false, "UE C already received " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCB1ToB2, false, "UE C already made transition from " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
      m_ueCB1ToB2 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE C TFB1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE C TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE C TFB3 is running.");
}

BroadcastCallScenario3::BroadcastCallScenario3 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAB2ToB1 (false),
    m_ueATxGrpBroadcastEnd (false),
    m_ueBB2ToB1 (false),
    m_ueBRxGrpBroadcastEnd (false),
    m_ueCB2ToB1 (false),
    m_ueCRxGrpBroadcastEnd (false)
{ }

void
BroadcastCallScenario3::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (BroadcastTestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<BroadcastTestCallMachine> ueAMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<BroadcastTestCallMachine> ueBMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<BroadcastTestCallMachine> ueCMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfb1 = ueAMachine->GetTfb1 ();
  Ptr<McpttTimer> ueATfb2 = ueAMachine->GetTfb2 ();
  Ptr<McpttTimer> ueBTfb1 = ueBMachine->GetTfb1 ();
  Ptr<McpttTimer> ueCTfb1 = ueCMachine->GetTfb1 ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  AddressValue grpAddress;
  std::string orgName = "EMS";
  Time startTime = Seconds (2.1);
  Time joinTime = Seconds (2.2);
  uint32_t origId = ueAPttApp->GetUserId ();
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
  ueAMachine->SetSdp (sdp);
  ueAMachine->SetCallType (McpttCallMsgFieldCallType::BROADCAST_GROUP);
  ueAMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BROADCAST_GROUP));
  ueAMachine->SetStartState (McpttCallMachineGrpBroadcastStateB2::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&BroadcastCallScenario3::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&BroadcastCallScenario3::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&BroadcastCallScenario3::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&BroadcastCallScenario3::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetCallType (McpttCallMsgFieldCallType::BROADCAST_GROUP);
  ueBMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BROADCAST_GROUP));
  ueBMachine->SetStartState (McpttCallMachineGrpBroadcastStateB2::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&BroadcastCallScenario3::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&BroadcastCallScenario3::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&BroadcastCallScenario3::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&BroadcastCallScenario3::UeBTimerExpCb, this));

  // UE C
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetCallType (McpttCallMsgFieldCallType::BROADCAST_GROUP);
  ueCMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BROADCAST_GROUP));
  ueCMachine->SetStartState (McpttCallMachineGrpBroadcastStateB2::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&BroadcastCallScenario3::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&BroadcastCallScenario3::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&BroadcastCallScenario3::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&BroadcastCallScenario3::UeCTimerExpCb, this));

  Simulator::Schedule (Seconds (2.1), &McpttTimer::Start, ueATfb1);
  Simulator::Schedule (Seconds (2.1), &McpttTimer::Start, ueATfb2);
  Simulator::Schedule (Seconds (2.1), &McpttTimer::Start, ueBTfb1);
  Simulator::Schedule (Seconds (2.1), &McpttTimer::Start, ueCTfb1);
  Simulator::Schedule (Seconds (2.15), &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (Seconds (2.15), &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (Seconds (2.15), &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (Seconds (2.15), &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (Seconds (2.15), &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (Seconds (2.15), &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (Seconds (2.2), &McpttPttApp::ReleaseCall, ueAPttApp);
}

void
BroadcastCallScenario3::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAB2ToB1, true, "UE A never made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcastEnd, true, "UE A never sent " << McpttCallMsgGrpBroadcastEnd::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBB2ToB1, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcastEnd, true, "UE B never received " << McpttCallMsgGrpBroadcastEnd::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCB2ToB1, true, "UE C never made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcastEnd, true, "UE C never received " << McpttCallMsgGrpBroadcastEnd::GetTypeId ().GetName () << ".");
}

void
BroadcastCallScenario3::UeATimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
BroadcastCallScenario3::UeARxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message " << msg << ".");
}

void
BroadcastCallScenario3::UeAStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB2::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcastEnd, true, "UE A never sent " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAB2ToB1, false, "UE A already made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      m_ueAB2ToB1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE A TFB1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE A TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE A TFB3 is running.");
}

void
BroadcastCallScenario3::UeATxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpBroadcastEnd::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB2::GetInstance (), "UE A is not in the " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " state.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcastEnd, false, "UE A already sent " << McpttCallMsgGrpBroadcastEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAB2ToB1, false, "UE A already made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      m_ueATxGrpBroadcastEnd = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE A TFB1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), true, "UE A TFB2 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE A TFB3 is running.");
}

void
BroadcastCallScenario3::UeBTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
BroadcastCallScenario3::UeBRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpBroadcastEnd::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB1::GetInstance (), "UE B is not in the " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " state.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcastEnd, false, "UE B already received " << McpttCallMsgGrpBroadcastEnd::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB2ToB1, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      m_ueBRxGrpBroadcastEnd = true;
      if (m_ueCRxGrpBroadcastEnd == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE B timer TFB1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE B timer TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE B timer TFB3 is running.");
}

void
BroadcastCallScenario3::UeBTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
BroadcastCallScenario3::UeBStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB2::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcastEnd, false, "UE B already received " << McpttCallMsgGrpBroadcastEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB2ToB1, false, "UE B already made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      m_ueBB2ToB1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE B TFB1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE B TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE B TFB3 is running.");
}

void
BroadcastCallScenario3::UeCTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE C timer " << timer << " unexpectedly expired.");
}

void
BroadcastCallScenario3::UeCRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpBroadcastEnd::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB1::GetInstance (), "UE C is not in the " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << " state.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcastEnd, false, "UE C already recieved " << McpttCallMsgGrpBroadcastEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCB2ToB1, true, "UE C never made transiton from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      m_ueCRxGrpBroadcastEnd = true;
      if (m_ueBRxGrpBroadcastEnd == true)
        {
          Stop ();
        }
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE C timer TFB1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE C timer TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE C timer TFB3 is running.");
}

void
BroadcastCallScenario3::UeCTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
BroadcastCallScenario3::UeCStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB2::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcastEnd, false, "UE C already received " << McpttCallMsgGrpBroadcastEnd::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCB2ToB1, false, "UE C already made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      m_ueCB2ToB1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE C TFB1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE C TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE C TFB3 is running.");
}

BroadcastCallScenario4::BroadcastCallScenario4 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAB2ToB1 (false),
    m_ueAExpTfb1 (false),
    m_ueAExpTfb2 (false),
    m_ueATxGrpBroadcast (false),
    m_ueBB2ToB4 (false),
    m_ueBB4ToB1 (false),
    m_ueBExpTfb1 (false),
    m_ueBRxGrpBroadcast (false),
    m_ueCB2ToB1 (false),
    m_ueCExpTfb1 (false),
    m_ueCRxGrpBroadcast (false)
{ }

void
BroadcastCallScenario4::Configure (void)
{
  GetConfig ()->SetAppCount (3);
  GetConfig ()->SetCallTid (BroadcastTestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);
  Ptr<McpttPttApp> ueCPttApp = GetApp (2);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueCCall = ueCPttApp->GetSelectedCall ();

  Ptr<BroadcastTestCallMachine> ueAMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<BroadcastTestCallMachine> ueBMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (1));
  Ptr<BroadcastTestCallMachine> ueCMachine = DynamicCast<BroadcastTestCallMachine, McpttCallMachine> (GetCallMachine (2));

  Ptr<McpttTimer> ueATfb1 = ueAMachine->GetTfb1 ();
  Ptr<McpttTimer> ueATfb2 = ueAMachine->GetTfb2 ();
  Ptr<McpttTimer> ueBTfb1 = ueBMachine->GetTfb1 ();
  Ptr<McpttTimer> ueCTfb1 = ueCMachine->GetTfb1 ();

  uint32_t grpId = 1;
  uint16_t callId = 1;
  std::string orgName = "EMS";
  AddressValue grpAddress;
  Time startTime = Seconds (2.1);
  Time joinTime = Seconds (2.2);
  uint32_t origId = ueAPttApp->GetUserId ();
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
  ueAMachine->SetAttribute ("TFB1", TimeValue (Seconds (5)));
  ueAMachine->SetCallId (callId);
  ueAMachine->SetGrpId (grpId);
  ueAMachine->SetOrigId (origId);
  ueAMachine->SetSdp (sdp);
  ueAMachine->SetCallType (McpttCallMsgFieldCallType::BROADCAST_GROUP);
  ueAMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BROADCAST_GROUP));
  ueAMachine->SetStartState (McpttCallMachineGrpBroadcastStateB2::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&BroadcastCallScenario4::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&BroadcastCallScenario4::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&BroadcastCallScenario4::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&BroadcastCallScenario4::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetAttribute ("TFB1", TimeValue (Seconds (5)));
  ueBMachine->SetCallId (callId);
  ueBMachine->SetGrpId (grpId);
  ueBMachine->SetOrigId (origId);
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetCallType (McpttCallMsgFieldCallType::BROADCAST_GROUP);
  ueBMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BROADCAST_GROUP));
  ueBMachine->SetStartState (McpttCallMachineGrpBroadcastStateB2::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&BroadcastCallScenario4::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&BroadcastCallScenario4::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&BroadcastCallScenario4::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&BroadcastCallScenario4::UeBTimerExpCb, this));

  // UE C
  ueCMachine->SetAttribute ("TFB1", TimeValue (Seconds (5)));
  ueCMachine->SetCallId (callId);
  ueCMachine->SetGrpId (grpId);
  ueCMachine->SetOrigId (origId);
  ueCMachine->SetSdp (sdp);
  ueCMachine->SetCallType (McpttCallMsgFieldCallType::BROADCAST_GROUP);
  ueCMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::BROADCAST_GROUP));
  ueCMachine->SetStartState (McpttCallMachineGrpBroadcastStateB2::GetInstance ());
  ueCMachine->SetPostRxCb (MakeCallback (&BroadcastCallScenario4::UeCRxCb, this));
  ueCMachine->SetPostTxCb (MakeCallback (&BroadcastCallScenario4::UeCTxCb, this));
  ueCMachine->SetStateChangeTestCb (MakeCallback (&BroadcastCallScenario4::UeCStateChangeCb, this));
  ueCMachine->SetPostTimerExpCb (MakeCallback (&BroadcastCallScenario4::UeCTimerExpCb, this));

  Simulator::Schedule (Seconds (2.1), &McpttTimer::Start, ueATfb1);
  Simulator::Schedule (Seconds (2.1), &McpttTimer::Start, ueATfb2);
  Simulator::Schedule (Seconds (2.1), &McpttTimer::Start, ueBTfb1);
  Simulator::Schedule (Seconds (2.1), &McpttTimer::Start, ueCTfb1);
  Simulator::Schedule (Seconds (2.15), &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (Seconds (2.15), &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (Seconds (2.15), &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (Seconds (2.15), &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (Seconds (2.15), &McpttCall::OpenFloorChannel, ueCCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (Seconds (2.15), &McpttCall::OpenMediaChannel, ueCCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (Seconds (2.2), &McpttPttApp::ReleaseCall, ueBPttApp);
}

void
BroadcastCallScenario4::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAB2ToB1, true, "UE A never made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfb1, true, "UE A TFB1 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfb2, true, "UE A TFB2 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcast, true, "UE A never sent " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBB2ToB4, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBB4ToB1, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfb1, true, "UE B TFB1 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, true, "UE B never received " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCB2ToB1, true, "UE C never made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueCExpTfb1, true, "UE C TFB1 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcast, true, "UE C never received " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
}

void
BroadcastCallScenario4::UeATimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfb1 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfb1, false, "UE A timer " << timer << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfb2, true, "UE A timer " << timer << " never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAB2ToB1, true, "UE A never made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcast, true, "UE A never sent a " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE A TFB2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE A TFB1 is running.");
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB1::GetInstance (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBroadcastStateB1::GetInstance () << ".");
      m_ueAExpTfb1 = true;
    }
  else if (timer.GetId () == machine.GetTfb2 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfb2, false, "UE A timer " << timer << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfb1, false, "UE A timer " << timer << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAB2ToB1, false, "UE A already made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcast, true, "UE A never sent a " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), true, "UE A TFB2 is running.");
      NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE A TFB1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB2::GetInstance (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBroadcastStateB2::GetInstance () << ".");
      m_ueAExpTfb2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A timer " << timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE A TFB3 is running.");
}

void
BroadcastCallScenario4::UeARxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message " << msg << ".");
}

void
BroadcastCallScenario4::UeAStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB2::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAB2ToB1, false, "UE A already made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfb1, false, "UE A timer " << tfb1 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfb2, true, "UE A timer " << tfb2 << " never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcast, true, "UE A never sent a " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
      m_ueAB2ToB1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE A TFB1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE A TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE A TFB3 is running.");
}

void
BroadcastCallScenario4::UeATxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpBroadcast::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueATxGrpBroadcast, false, "UE A already sent " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfb1, false, "UE A timer " << tfb1 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfb2, false, "UE A timer " << tfb2 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAB2ToB1, false, "UE A already made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB2::GetInstance (), "UE A in" << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << ".");
      m_ueATxGrpBroadcast = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE A TFB1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE A TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE A TFB3 is running.");
}

void
BroadcastCallScenario4::UeBTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfb1 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfb1, false, "UE B timer " << timer << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, true, "UE B never received " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB2ToB4, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB4ToB1, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB1::GetInstance (), "UE A in" << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      m_ueBExpTfb1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE A TFB1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE A TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE A TFB3 is running.");
}

void
BroadcastCallScenario4::UeBRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpBroadcast::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, false, "UE B already received " << McpttCallMsgGrpBroadcast::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfb1, false, "UE B timer " << tfb1 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB2ToB4, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB4ToB1, false, "UE B already made transition from " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB4::GetInstance (), "UE B in " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " state.");
      m_ueBRxGrpBroadcast = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE B timer TFB1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE B timer TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE B timer TFB3 is running.");
}

void
BroadcastCallScenario4::UeBTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
BroadcastCallScenario4::UeBStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB2::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB4::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBB2ToB4, false, "UE B already made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, false, "UE B already received " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE B TFB1 is not running.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfb1, false, "UE B timer " << tfb1 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB4ToB1, false, "UE B already made transition from " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      m_ueBB2ToB4 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB4::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBB4ToB1, false, "UE B already made transition from " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxGrpBroadcast, true, "UE B never received " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfb1, false, "UE B timer " << tfb1 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE B TFB1 is running.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBB2ToB4, true, "UE B never made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB4::GetStateId () << ".");
      m_ueBB4ToB1 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE B TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE B TFB3 is running.");
}

void
BroadcastCallScenario4::UeCTimerExpCb (const BroadcastTestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (timer.GetId () == machine.GetTfb1 ()->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCExpTfb1, false, "UE C timer " << timer << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcast, true, "UE C never received " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCB2ToB1, true, "UE C never made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB1::GetInstance (), "UE A in" << state->GetInstanceStateId () << " but should be in " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      m_ueCExpTfb1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE A TFB1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE A TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE A TFB3 is running.");
}

void
BroadcastCallScenario4::UeCRxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();
  Ptr<McpttCallMachineGrpBroadcastState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgGrpBroadcast::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcast, false, "UE C already recieved " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCExpTfb1, false, "UE B timer " << tfb1 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCB2ToB1, false, "UE C already made transiton from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state, McpttCallMachineGrpBroadcastStateB2::GetInstance (), "UE C is in " << state->GetInstanceStateId () << " not " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " state.");
      m_ueCRxGrpBroadcast = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE C received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), true, "UE C timer TFB1 is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE C timer TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE C timer TFB3 is running.");
}

void
BroadcastCallScenario4::UeCTxCb (const BroadcastTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE C sent unexpected message: " << msg.GetTypeId () << ".");
}

void
BroadcastCallScenario4::UeCStateChangeCb (const BroadcastTestCallMachine& machine, Ptr<McpttCallMachineGrpBroadcastState>  oldState, Ptr<McpttCallMachineGrpBroadcastState>  newState)
{
  Ptr<McpttTimer> tfb1 = machine.GetTfb1 ();
  Ptr<McpttTimer> tfb2 = machine.GetTfb2 ();
  Ptr<McpttTimer> tfb3 = machine.GetTfb3 ();

  if (oldState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB2::GetStateId () && newState->GetInstanceStateId () == McpttCallMachineGrpBroadcastStateB1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueCB2ToB1, false, "UE C already made transition from " << McpttCallMachineGrpBroadcastStateB2::GetStateId () << " to " << McpttCallMachineGrpBroadcastStateB1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueCExpTfb1, false, "UE B timer " << tfb1 << " already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueCRxGrpBroadcast, true, "UE C never received " << McpttCallMsgGrpBroadcast::GetTypeId () << ".");
      m_ueCB2ToB1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfb1->IsRunning (), false, "UE C TFB1 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb2->IsRunning (), false, "UE C TFB2 is running.");
  NS_TEST_ASSERT_MSG_EQ (tfb3->IsRunning (), false, "UE C TFB3 is running.");
}

McpttBroadcastCallControlTestSuite::McpttBroadcastCallControlTestSuite (void)
  : TestSuite ("mcptt-call-control-broadcast", TestSuite::SYSTEM)
{
  AddTestCase (new BroadcastCallScenario1 (), TestCase::QUICK);
  AddTestCase (new BroadcastCallScenario2 (), TestCase::QUICK);
  AddTestCase (new BroadcastCallScenario3 (), TestCase::QUICK);
  AddTestCase (new BroadcastCallScenario4 (), TestCase::QUICK);
}

} // namespace tests
} // namespace psc
} // namespace ns3

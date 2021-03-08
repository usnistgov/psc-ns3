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
#include <ns3/mcptt-call-machine-private.h>
#include <ns3/mcptt-call-msg.h>
#include <ns3/mcptt-ptt-app.h>

#include "mcptt-test-call.h"
#include "mcptt-test-case.h"
#include "mcptt-test-case-config.h"
#include "mcptt-msg-dropper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttPrivateCallControlTest");

namespace psc {
namespace tests {

class PrivateTestCallMachine : public McpttCallMachinePrivate
{
public:
  static TypeId GetTypeId (void);
  PrivateTestCallMachine (void);
  virtual ~PrivateTestCallMachine (void);
  virtual void ChangeState (Ptr<McpttCallMachinePrivateState>  newState);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Receive (const McpttCallMsg& msg);
  virtual void Start (void);
  virtual void Send (const McpttCallMsg& msg);

protected:
  virtual void ExpiryOfTfp1 (void);
  virtual void ExpiryOfTfp2 (void);
  virtual void ExpiryOfTfp3 (void);
  virtual void ExpiryOfTfp5 (void);

private:
  Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&> m_postRxCb;
  Callback<void, const PrivateTestCallMachine&, const McpttTimer&> m_postTimerExpCb;
  Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&> m_postTxCb;
  Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&> m_preRxCb;
  Callback<void, const PrivateTestCallMachine&, const McpttTimer&> m_preTimerExpCb;
  Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&> m_preTxCb;
  Ptr<McpttCallMachinePrivateState> m_startState;
  Callback<void, const PrivateTestCallMachine&, Ptr<McpttCallMachinePrivateState>, Ptr<McpttCallMachinePrivateState> > m_stateChangeCb;

public:
  virtual Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&> GetPostRxCb (void) const;
  virtual Callback<void, const PrivateTestCallMachine&, const McpttTimer&> GetPostTimerExpCb (void) const;
  virtual Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&> GetPostTxCb (void) const;
  virtual Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&> GetPreRxCb (void) const;
  virtual Callback<void, const PrivateTestCallMachine&, const McpttTimer&> GetPreTimerExpCb (void) const;
  virtual Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&> GetPreTxCb (void) const;
  virtual Ptr<McpttCallMachinePrivateState> GetStartState (void) const;
  virtual Callback<void, const PrivateTestCallMachine&, Ptr<McpttCallMachinePrivateState>, Ptr<McpttCallMachinePrivateState> > GetStateChangeCb (void) const;
  virtual void SetPostRxCb (const Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&>  postRxCb);
  virtual void SetPostTimerExpCb (const Callback<void, const PrivateTestCallMachine&, const McpttTimer&>  timerExpCb);
  virtual void SetPostTxCb (const Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&>  postTxCb);
  virtual void SetPreRxCb (const Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&>  preRxCb);
  virtual void SetPreTimerExpCb (const Callback<void, const PrivateTestCallMachine&, const McpttTimer&>  timerExpCb);
  virtual void SetPreTxCb (const Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&>  preTxCb);
  virtual void SetStartState (Ptr<McpttCallMachinePrivateState>  startState);
  virtual void SetStateChangeTestCb (const Callback<void, const PrivateTestCallMachine&, Ptr<McpttCallMachinePrivateState>, Ptr<McpttCallMachinePrivateState> >  stateChangeCb);
};

class PrivateCallSetupAuto1 : public McpttTestCase
{
public:
  PrivateCallSetupAuto1 (const std::string& name = "Private Call Setup - Establish New Call in Automatic Mode 1", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAP0ToP2;
  bool m_ueAP2ToP4;
  bool m_ueARxAccept;
  bool m_ueATxAcceptAck;
  bool m_ueATxSetup;
  bool m_ueBP0ToP5;
  bool m_ueBP5ToP4;
  bool m_ueBRxSetup;
  bool m_ueBRxAcceptAck;
  bool m_ueBTxAccept;
};

class PrivateCallSetupAuto3 : public McpttTestCase
{
public:
  PrivateCallSetupAuto3 (const std::string& name = "Private Call Setup - Establish New Call in Automatic Mode 3", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAP0ToP2;
  bool m_ueAP2ToP3;
  bool m_ueARxAccept;
  bool m_ueATxRelease;
  bool m_ueATxSetup;
  bool m_ueBP0ToP5;
  bool m_ueBP5ToP1;
  bool m_ueBRxSetup;
  bool m_ueBRxRelease;
  bool m_ueBTxAccept;
  bool m_ueBTxReleaseAck;
};

class PrivateCallSetupManual1 : public McpttTestCase
{
public:
  PrivateCallSetupManual1 (const std::string& name = "Private Call Setup - Establish New Call in Manual Mode 1", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAP0ToP2;
  bool m_ueAP2ToP4;
  bool m_ueARxAccept;
  bool m_ueARxRinging;
  bool m_ueATxAcceptAck;
  bool m_ueATxSetup;
  bool m_ueBP0ToP5;
  bool m_ueBP5ToP4;
  bool m_ueBRxSetup;
  bool m_ueBRxAcceptAck;
  bool m_ueBTxAccept;
  bool m_ueBTxRinging;
};

class PrivateCallSetupManual3 : public McpttTestCase
{
public:
  PrivateCallSetupManual3 (const std::string& name = "Private Call Setup - Establish New Call in Manual Mode 3", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAP0ToP2;
  bool m_ueAP2ToP3;
  bool m_ueARxAccept;
  bool m_ueARxRinging;
  bool m_ueATxRelease;
  bool m_ueATxSetup;
  bool m_ueBP0ToP5;
  bool m_ueBP5ToP1;
  bool m_ueBRxSetup;
  bool m_ueBRxRelease;
  bool m_ueBTxAccept;
  bool m_ueBTxReleaseAck;
  bool m_ueBTxRinging;
};

class PrivateCallSetupFailure : public McpttTestCase
{
public:
  PrivateCallSetupFailure (const std::string& name = "Private Call Setup - Failure to New Call", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAP0ToP2;
  bool m_ueAP2ToP1;
  bool m_ueARxReject;
  bool m_ueATxSetup;
  bool m_ueBP0ToP1;
  bool m_ueBRxSetup;
  bool m_ueBTxReject;
};

class PrivateCallCancel1 : public McpttTestCase
{
public:
  PrivateCallCancel1 (const std::string& name = "Private Call Cancelation - Cancel an Ongoing Call 1", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAP4ToP3;
  bool m_ueAP3ToP1;
  bool m_ueARxReleaseAck;
  bool m_ueATxRelease;
  bool m_ueBP4ToP1;
  bool m_ueBRxRelease;
  bool m_ueBTxReleaseAck;
};

class PrivateCallCancel2 : public McpttTestCase
{
public:
  PrivateCallCancel2 (const std::string& name = "Private Call Cancelation - Cancel an Ongoing Call 2", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAP4ToP3;
  bool m_ueAP3ToP1;
  uint32_t m_ueATxRelease;
};

class PrivateCallExpiration : public McpttTestCase
{
public:
  PrivateCallExpiration (const std::string& name = "Private Call Expiration", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void NewCallCb (uint16_t callId);
  virtual void UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState);
  virtual void UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg);

private:
  bool m_ueAExpTfp5;
  bool m_ueAP1ToP2;
  bool m_ueAP4ToP1;
  bool m_ueARxRinging;
  bool m_ueATxSetup;
  bool m_ueBExpTfp5;
  bool m_ueBP1ToP5;
  bool m_ueBP4ToP1;
  bool m_ueBRxSetup;
  bool m_ueBTxRinging;
};

class McpttPrivateCallControlTestSuite : public TestSuite
{
public:
  McpttPrivateCallControlTestSuite (void);
};

/***************************************************************
 *  Implementation of the code declared above.
 ***************************************************************/

static McpttPrivateCallControlTestSuite callSuite;

TypeId
PrivateTestCallMachine::GetTypeId (void)
{
  static TypeId tid = TypeId ("PrivateTestCallMachine")
    .SetParent<McpttCallMachinePrivate> ()
    .AddConstructor<PrivateTestCallMachine> ()
  ;

  return tid;
}

PrivateTestCallMachine::PrivateTestCallMachine (void)
  : McpttCallMachinePrivate (),
    m_postRxCb (MakeNullCallback<void, const PrivateTestCallMachine&, const McpttCallMsg&> ()),
    m_postTimerExpCb (MakeNullCallback<void, const PrivateTestCallMachine&, const McpttTimer&> ()),
    m_postTxCb (MakeNullCallback<void, const PrivateTestCallMachine&, const McpttCallMsg&> ()),
    m_preRxCb (MakeNullCallback<void, const PrivateTestCallMachine&, const McpttCallMsg&> ()),
    m_preTimerExpCb (MakeNullCallback<void, const PrivateTestCallMachine&, const McpttTimer&> ()),
    m_preTxCb (MakeNullCallback<void, const PrivateTestCallMachine&, const McpttCallMsg&> ()),
    m_startState (McpttCallMachinePrivateStateP1::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const PrivateTestCallMachine&, Ptr<McpttCallMachinePrivateState>, Ptr<McpttCallMachinePrivateState> > ())
{ }

PrivateTestCallMachine::~PrivateTestCallMachine (void)
{ }

void
PrivateTestCallMachine::Receive (const McpttCallMsg& msg)
{
  Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallMachinePrivate::Receive (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
PrivateTestCallMachine::Send (const McpttCallMsg& msg)
{
  Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&> postTxCb = GetPostTxCb ();
  Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&> preTxCb = GetPreTxCb ();

  if (!preTxCb.IsNull ())
    {
      preTxCb (*this, msg);
    }

  McpttCallMachinePrivate::Send (msg);

  if (!postTxCb.IsNull ())
    {
      postTxCb (*this, msg);
    }
}

void
PrivateTestCallMachine::ExpiryOfTfp1 (void)
{
  Ptr<McpttTimer> tfg = GetTfp1 ();
  Callback<void, const PrivateTestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const PrivateTestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachinePrivate::ExpiryOfTfp1 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
PrivateTestCallMachine::ExpiryOfTfp2 (void)
{
  Ptr<McpttTimer> tfg = GetTfp2 ();
  Callback<void, const PrivateTestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const PrivateTestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachinePrivate::ExpiryOfTfp2 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
PrivateTestCallMachine::ExpiryOfTfp3 (void)
{
  Ptr<McpttTimer> tfg = GetTfp3 ();
  Callback<void, const PrivateTestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const PrivateTestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachinePrivate::ExpiryOfTfp3 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
PrivateTestCallMachine::ExpiryOfTfp5 (void)
{
  Ptr<McpttTimer> tfg = GetTfp5 ();
  Callback<void, const PrivateTestCallMachine&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const PrivateTestCallMachine&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfg);
    }

  McpttCallMachinePrivate::ExpiryOfTfp5 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfg);
    }
}

void
PrivateTestCallMachine::Start (void)
{
  Ptr<McpttCallMachinePrivateState> startState = GetStartState ();

  SetState (startState);

  McpttCallMachinePrivate::Start ();
}

void
PrivateTestCallMachine::ChangeState (Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttCallMachinePrivateState> oldState = GetState ();
  Callback<void, const PrivateTestCallMachine&, Ptr<McpttCallMachinePrivateState>, Ptr<McpttCallMachinePrivateState> > stateChangeCb = GetStateChangeCb ();

  McpttCallMachinePrivate::ChangeState (newState);

  if (!stateChangeCb.IsNull ())
    {
      stateChangeCb (*this, oldState, newState);
    }
}

TypeId
PrivateTestCallMachine::GetInstanceTypeId (void) const
{
  return PrivateTestCallMachine::GetTypeId ();
}

Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&>
PrivateTestCallMachine::GetPostRxCb (void) const
{
  return m_postRxCb;
}

Callback<void, const PrivateTestCallMachine&, const McpttTimer&>
PrivateTestCallMachine::GetPostTimerExpCb (void) const
{
  return m_postTimerExpCb;
}


Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&>
PrivateTestCallMachine::GetPostTxCb (void) const
{
  return m_postTxCb;
}

Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&>
PrivateTestCallMachine::GetPreRxCb (void) const
{
  return m_preRxCb;
}

Callback<void, const PrivateTestCallMachine&, const McpttTimer&>
PrivateTestCallMachine::GetPreTimerExpCb (void) const
{
  return m_preTimerExpCb;
}

Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&>
PrivateTestCallMachine::GetPreTxCb (void) const
{
  return m_preTxCb;
}

Ptr<McpttCallMachinePrivateState>
PrivateTestCallMachine::GetStartState (void) const
{
  return m_startState;
}

Callback<void, const PrivateTestCallMachine&, Ptr<McpttCallMachinePrivateState>, Ptr<McpttCallMachinePrivateState> >
PrivateTestCallMachine::GetStateChangeCb (void) const
{
  return m_stateChangeCb;
}

void
PrivateTestCallMachine::SetPostRxCb (const Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&>  postRxCb)
{
  m_postRxCb = postRxCb;
}

void
PrivateTestCallMachine::SetPostTimerExpCb (const Callback<void, const PrivateTestCallMachine&, const McpttTimer&>  timerExpCb)
{
  m_postTimerExpCb = timerExpCb;
}

void
PrivateTestCallMachine::SetPostTxCb (const Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&>  postTxCb)
{
  m_postTxCb = postTxCb;
}

void
PrivateTestCallMachine::SetPreRxCb (const Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&>  preRxCb)
{
  m_preRxCb = preRxCb;
}

void
PrivateTestCallMachine::SetPreTimerExpCb (const Callback<void, const PrivateTestCallMachine&, const McpttTimer&>  timerExpCb)
{
  m_preTimerExpCb = timerExpCb;
}

void
PrivateTestCallMachine::SetPreTxCb (const Callback<void, const PrivateTestCallMachine&, const McpttCallMsg&>  preTxCb)
{
  m_preTxCb = preTxCb;
}

void
PrivateTestCallMachine::SetStartState (Ptr<McpttCallMachinePrivateState>  startState)
{
  m_startState = startState;
}

void
PrivateTestCallMachine::SetStateChangeTestCb (const Callback<void, const PrivateTestCallMachine&, Ptr<McpttCallMachinePrivateState>, Ptr<McpttCallMachinePrivateState> >  stateChangeCb)
{
  m_stateChangeCb = stateChangeCb;
}

PrivateCallSetupAuto1::PrivateCallSetupAuto1 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAP0ToP2 (false),
    m_ueAP2ToP4 (false),
    m_ueARxAccept (false),
    m_ueATxAcceptAck (false),
    m_ueATxSetup (false),
    m_ueBP0ToP5 (false),
    m_ueBP5ToP4 (false),
    m_ueBRxSetup (false),
    m_ueBRxAcceptAck (false),
    m_ueBTxAccept (false)
{ }

void
PrivateCallSetupAuto1::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (PrivateTestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<PrivateTestCallMachine> ueAMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<PrivateTestCallMachine> ueBMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (1));

  // UE A
  ueAMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));
  ueAMachine->SetAttribute ("TFP1", TimeValue (MilliSeconds (240)));
  ueAMachine->SetAttribute ("AutoCommencement", BooleanValue (true));
  ueAMachine->SetStartState (McpttCallMachinePrivateStateP0::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&PrivateCallSetupAuto1::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&PrivateCallSetupAuto1::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallSetupAuto1::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallSetupAuto1::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetAttribute ("TFP4", TimeValue (MilliSeconds (240)));
  ueBMachine->SetStartState (McpttCallMachinePrivateStateP0::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&PrivateCallSetupAuto1::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&PrivateCallSetupAuto1::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallSetupAuto1::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallSetupAuto1::UeBTimerExpCb, this));

  Simulator::Schedule (Seconds (2.2), &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
PrivateCallSetupAuto1::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, true, "UE A never made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, true, "UE A never receieved " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, true, "UE A never sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, true, "UE B never made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, true, "UE B never received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
}

void
PrivateCallSetupAuto1::UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
PrivateCallSetupAuto1::UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, true, "UE A never made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, true, "UE A never sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      m_ueARxAccept = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP4::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP4::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), true, "UE A timer " << *tfp5 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupAuto1::UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, false, "UE A already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, false, "UE A already sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
      m_ueAP0ToP2 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP2::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP4::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, true, "UE A never sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), true, "UE A timer " << *tfp5 << " is not running.");
      m_ueAP2ToP4 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupAuto1::UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, false, "UE A already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, false, "UE A already sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, false, "UE A already sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP0::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP0::GetStateId () << ".");
      m_ueATxSetup = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateAcceptAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, false, "UE A already sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      m_ueATxAcceptAck = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupAuto1::UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
PrivateCallSetupAuto1::UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), true, "UE A timer " << *tfp4 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP5::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      m_ueBRxSetup = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateAcceptAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, true, "UE B never made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B never received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), true, "UE A timer " << *tfp5 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP4::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP4::GetStateId () << ".");
      m_ueBRxAcceptAck = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupAuto1::UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, false, "UE B already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B already sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      m_ueBTxAccept = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP0::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP0::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupAuto1::UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP5::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, false, "UE B already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), true, "UE A timer " << *tfp4 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
      m_ueBP0ToP5 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP5::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP4::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), true, "UE A timer " << *tfp5 << " is not running.");
      m_ueBP5ToP4 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

PrivateCallSetupAuto3::PrivateCallSetupAuto3 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAP0ToP2 (false),
    m_ueAP2ToP3 (false),
    m_ueARxAccept (false),
    m_ueATxRelease (false),
    m_ueATxSetup (false),
    m_ueBP0ToP5 (false),
    m_ueBP5ToP1 (false),
    m_ueBRxSetup (false),
    m_ueBRxRelease (false),
    m_ueBTxAccept (false),
    m_ueBTxReleaseAck (false)
{ }

void
PrivateCallSetupAuto3::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (PrivateTestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<PrivateTestCallMachine> ueAMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<PrivateTestCallMachine> ueBMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (1));

  // UE A
  ueAMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));
  ueAMachine->SetAttribute ("TFP1", TimeValue (MilliSeconds (240)));
  ueAMachine->SetAttribute ("TFP3", TimeValue (MilliSeconds (240)));
  ueAMachine->SetAttribute ("AutoCommencement", BooleanValue (true));
  ueAMachine->SetStartState (McpttCallMachinePrivateStateP0::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&PrivateCallSetupAuto3::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&PrivateCallSetupAuto3::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallSetupAuto3::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallSetupAuto3::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetAttribute ("TFP4", TimeValue (MilliSeconds (240)));
  ueBMachine->SetStartState (McpttCallMachinePrivateStateP0::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&PrivateCallSetupAuto3::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&PrivateCallSetupAuto3::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallSetupAuto3::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallSetupAuto3::UeBTimerExpCb, this));

  Simulator::Schedule (Seconds (2.2), &McpttPttApp::TakePushNotification, ueAPttApp);
  Simulator::Schedule (Seconds (2.30), &McpttPttApp::ReleaseCall, ueAPttApp);
}

void
PrivateCallSetupAuto3::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP2::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, true, "UE A never received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, true, "UE B never made the transition from " << McpttCallMachinePrivateStateP5::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, true, "UE B never received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, true, "UE B never sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
}

void
PrivateCallSetupAuto3::UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
PrivateCallSetupAuto3::UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP2::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A never received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      m_ueARxAccept = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), true, "UE A timer " << *tfp3 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupAuto3::UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP2::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, false, "UE A already sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
      m_ueAP0ToP2 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP2::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP2::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), true, "UE A timer " << *tfp3 << " is not running.");
      m_ueAP2ToP3 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupAuto3::UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP2::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, false, "UE A already sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, false, "UE A already sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP0::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP0::GetStateId () << ".");
      m_ueATxSetup = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateRelease::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP2::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, false, "UE A already sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      m_ueATxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupAuto3::UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
PrivateCallSetupAuto3::UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, false, "UE B already made the transition from " << McpttCallMachinePrivateStateP5::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, false, "UE B already sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), true, "UE A timer " << *tfp4 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP5::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      m_ueBRxSetup = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateRelease::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, true, "UE B never made the transition from " << McpttCallMachinePrivateStateP5::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, true, "UE B never sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      m_ueBRxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupAuto3::UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, false, "UE B already made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, false, "UE B already made the transition from " << McpttCallMachinePrivateStateP5::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B already sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, false, "UE B already sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP0::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP0::GetStateId () << ".");
      m_ueBTxAccept = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateReleaseAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, false, "UE B already made the transition from " << McpttCallMachinePrivateStateP5::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, false, "UE B already sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), true, "UE A timer " << *tfp4 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP5::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      m_ueBTxReleaseAck = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetInstanceTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupAuto3::UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP5::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, false, "UE B already made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, false, "UE B already made the transition from " << McpttCallMachinePrivateStateP5::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, false, "UE B already sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), true, "UE A timer " << *tfp4 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
      m_ueBP0ToP5 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP5::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made the transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, false, "UE B already made the transition from " << McpttCallMachinePrivateStateP5::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, true, "UE B never sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
      m_ueBP5ToP1 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

PrivateCallSetupManual1::PrivateCallSetupManual1 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAP0ToP2 (false),
    m_ueAP2ToP4 (false),
    m_ueARxAccept (false),
    m_ueARxRinging (false),
    m_ueATxAcceptAck (false),
    m_ueATxSetup (false),
    m_ueBP0ToP5 (false),
    m_ueBP5ToP4 (false),
    m_ueBRxSetup (false),
    m_ueBRxAcceptAck (false),
    m_ueBTxAccept (false),
    m_ueBTxRinging (false)
{ }

void
PrivateCallSetupManual1::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (PrivateTestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<PrivateTestCallMachine> ueAMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<PrivateTestCallMachine> ueBMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (1));

  // UE A
  ueAMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));
  ueAMachine->SetAttribute ("AutoCommencement", BooleanValue (false));
  ueAMachine->SetAttribute ("TFP1", TimeValue (MilliSeconds (240)));
  ueAMachine->SetStartState (McpttCallMachinePrivateStateP0::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&PrivateCallSetupManual1::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&PrivateCallSetupManual1::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallSetupManual1::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallSetupManual1::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetAttribute ("TFP4", TimeValue (MilliSeconds (240)));
  ueBMachine->SetStartState (McpttCallMachinePrivateStateP0::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&PrivateCallSetupManual1::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&PrivateCallSetupManual1::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallSetupManual1::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallSetupManual1::UeBTimerExpCb, this));

  Simulator::Schedule (Seconds (2.2), &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
PrivateCallSetupManual1::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, true, "UE A never made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, true, "UE A never receieved " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, true, "UE A never receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, true, "UE A never sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, true, "UE B never made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, true, "UE B never received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
}

void
PrivateCallSetupManual1::UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
PrivateCallSetupManual1::UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, true, "UE A never made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, true, "UE A never receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, true, "UE A never sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), true, "UE A timer " << *tfp5 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP4::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP4::GetStateId () << ".");
      m_ueARxAccept = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateRinging::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, false, "UE A already receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, false, "UE A already sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      m_ueARxRinging = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupManual1::UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, false, "UE A already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, false, "UE A already receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, false, "UE A already sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
      m_ueAP0ToP2 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP2::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP4::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, true, "UE A never sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), true, "UE A timer " << *tfp5 << " is not running.");
      m_ueAP2ToP4 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupManual1::UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, false, "UE A already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, false, "UE A already receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, false, "UE A already sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, false, "UE A already sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP0::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP0::GetStateId () << ".");
      m_ueATxSetup = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateAcceptAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP4, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, false, "UE A already sent " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      m_ueATxAcceptAck = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupManual1::UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
PrivateCallSetupManual1::UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B already sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), true, "UE A timer " << *tfp2 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP5::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      m_ueBRxSetup = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateAcceptAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, true, "UE B never made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B never received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), true, "UE A timer " << *tfp5 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP4::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP4::GetStateId () << ".");
      m_ueBRxAcceptAck = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupManual1::UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B already sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), true, "UE A timer " << *tfp2 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP5::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      m_ueBTxAccept = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateRinging::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, false, "UE B already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B already sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, false, "UE B already sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP0::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP0::GetStateId () << ".");
      m_ueBTxRinging = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupManual1::UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP5::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, false, "UE B already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B already sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), true, "UE A timer " << *tfp2 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
      m_ueBP0ToP5 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP5::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP4::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP4, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), true, "UE A timer " << *tfp5 << " is not running.");
      m_ueBP5ToP4 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

PrivateCallSetupManual3::PrivateCallSetupManual3 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAP0ToP2 (false),
    m_ueAP2ToP3 (false),
    m_ueARxAccept (false),
    m_ueARxRinging (false),
    m_ueATxRelease (false),
    m_ueATxSetup (false),
    m_ueBP0ToP5 (false),
    m_ueBP5ToP1 (false),
    m_ueBRxSetup (false),
    m_ueBRxRelease (false),
    m_ueBTxAccept (false),
    m_ueBTxReleaseAck (false),
    m_ueBTxRinging (false)
{ }

void
PrivateCallSetupManual3::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (PrivateTestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<PrivateTestCallMachine> ueAMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<PrivateTestCallMachine> ueBMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (1));

  // UE A
  ueAMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));
  ueAMachine->SetAttribute ("AutoCommencement", BooleanValue (false));
  ueAMachine->SetAttribute ("TFP1", TimeValue (MilliSeconds (240)));
  ueAMachine->SetAttribute ("TFP3", TimeValue (MilliSeconds (240)));
  ueAMachine->SetStartState (McpttCallMachinePrivateStateP0::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&PrivateCallSetupManual3::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&PrivateCallSetupManual3::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallSetupManual3::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallSetupManual3::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetAttribute ("TFP4", TimeValue (MilliSeconds (240)));
  ueBMachine->SetStartState (McpttCallMachinePrivateStateP0::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&PrivateCallSetupManual3::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&PrivateCallSetupManual3::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallSetupManual3::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallSetupManual3::UeBTimerExpCb, this));

  Simulator::Schedule (Seconds (2.2), &McpttPttApp::TakePushNotification, ueAPttApp);
  Simulator::Schedule (Seconds (2.31), &McpttPttApp::ReleaseCall, ueAPttApp);
}

void
PrivateCallSetupManual3::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, true, "UE A never made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP3::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, true, "UE A never receieved " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, true, "UE A never receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, true, "UE B never made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, true, "UE B never received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, true, "UE B never sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
}

void
PrivateCallSetupManual3::UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
PrivateCallSetupManual3::UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, true, "UE A never made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP3::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, true, "UE A never receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), true, "UE A timer " << *tfp3 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
      m_ueARxAccept = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateRinging::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP3::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, false, "UE A already receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, false, "UE A already sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      m_ueARxRinging = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupManual3::UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, false, "UE A already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP3::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, false, "UE A already receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, false, "UE A already sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
      m_ueAP0ToP2 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP2::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP3::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), true, "UE A timer " << *tfp3 << " is not running.");
      m_ueAP2ToP3 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupManual3::UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, false, "UE A already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP3::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, false, "UE A already receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, false, "UE A already sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, false, "UE A already sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP0::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP0::GetStateId () << ".");
      m_ueATxSetup = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateRelease::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP3, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP3::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, false, "UE A already sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      m_ueATxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupManual3::UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
PrivateCallSetupManual3::UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B already sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, false, "UE B already sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), true, "UE A timer " << *tfp2 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP5::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      m_ueBRxSetup = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateRelease::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, true, "UE B never made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B never received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, true, "UE B never sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      m_ueBRxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupManual3::UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B already sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, false, "UE B already sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), true, "UE A timer " << *tfp2 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP5::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      m_ueBTxAccept = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateRinging::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, false, "UE B already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B already sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, false, "UE B already sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, false, "UE B already sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP0::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP0::GetStateId () << ".");
      m_ueBTxRinging = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateReleaseAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, false, "UE B already sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), true, "UE A timer " << *tfp4 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP5::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
      m_ueBTxReleaseAck = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupManual3::UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP5::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, false, "UE B already made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B already sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, false, "UE B already sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), true, "UE A timer " << *tfp2 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
      m_ueBP0ToP5 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP5::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP0::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP5ToP1, false, "UE B already made transition from " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, true, "UE B never sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
      m_ueBP5ToP1 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

PrivateCallSetupFailure::PrivateCallSetupFailure (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAP0ToP2 (false),
    m_ueAP2ToP1 (false),
    m_ueARxReject (false),
    m_ueATxSetup (false),
    m_ueBP0ToP1 (false),
    m_ueBRxSetup (false),
    m_ueBTxReject (false)
{ }

void
PrivateCallSetupFailure::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (PrivateTestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<PrivateTestCallMachine> ueAMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<PrivateTestCallMachine> ueBMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (1));

  // UE A
  ueAMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));
  ueAMachine->SetAttribute ("TFP1", TimeValue (MilliSeconds (240)));
  ueAMachine->SetAttribute ("AutoCommencement", BooleanValue (true));
  ueAMachine->SetStartState (McpttCallMachinePrivateStateP0::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&PrivateCallSetupFailure::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&PrivateCallSetupFailure::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallSetupFailure::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallSetupFailure::UeATimerExpCb, this));

  // UE B
  ueBMachine->SetAttribute ("TFP4", TimeValue (MilliSeconds (240)));
  ueBMachine->SetAttribute ("Failure", BooleanValue (true));
  ueBMachine->SetStartState (McpttCallMachinePrivateStateP0::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&PrivateCallSetupFailure::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&PrivateCallSetupFailure::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallSetupFailure::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallSetupFailure::UeBTimerExpCb, this));

  Simulator::Schedule (Seconds (2.2), &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
PrivateCallSetupFailure::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP1, true, "UE A never made transition from " << McpttCallMachinePrivateStateP2::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxReject, true, "UE A never received " << McpttCallMsgPrivateReject::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP1, true, "UE B never made transition from " << McpttCallMachinePrivateStateP1::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxReject, true, "UE B never sent " << McpttCallMsgPrivateReject::GetTypeId ().GetName () << ".");
}

void
PrivateCallSetupFailure::UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
PrivateCallSetupFailure::UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateReject::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP1, true, "UE A never made transition from " << McpttCallMachinePrivateStateP2::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReject, false, "UE A already received " << McpttCallMsgPrivateReject::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      m_ueARxReject = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupFailure::UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, false, "UE A already made transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP1, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReject, false, "UE A already received " << McpttCallMsgPrivateReject::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
      m_ueAP0ToP2 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP2::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP1, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReject, false, "UE A already received " << McpttCallMsgPrivateReject::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
      m_ueAP2ToP1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupFailure::UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP0ToP2, false, "UE A already made transition from " << McpttCallMachinePrivateStateP0::GetStateId () << " to " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP2ToP1, false, "UE A already made transition from " << McpttCallMachinePrivateStateP2::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReject, false, "UE A already received " << McpttCallMsgPrivateReject::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, false, "UE A already sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      m_ueATxSetup = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP0::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP0::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupFailure::UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
PrivateCallSetupFailure::UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP1, true, "UE B never made transition from " << McpttCallMachinePrivateStateP1::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReject, true, "UE B never sent " << McpttCallMsgPrivateReject::GetTypeId ().GetName () << ".");
      m_ueBRxSetup = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupFailure::UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateReject::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP1, false, "UE B already made transition from " << McpttCallMachinePrivateStateP1::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReject, false, "UE B already sent " << McpttCallMsgPrivateReject::GetTypeId ().GetName () << ".");
      m_ueBTxReject = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP0::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP0::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallSetupFailure::UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP0ToP1, false, "UE B already made transition from " << McpttCallMachinePrivateStateP1::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReject, true, "UE B never sent " << McpttCallMsgPrivateReject::GetTypeId ().GetName () << ".");
      m_ueBP0ToP1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

PrivateCallCancel1::PrivateCallCancel1 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAP4ToP3 (false),
    m_ueAP3ToP1 (false),
    m_ueARxReleaseAck (false),
    m_ueATxRelease (false),
    m_ueBP4ToP1 (false),
    m_ueBRxRelease (false),
    m_ueBTxReleaseAck (false)
{ }

void
PrivateCallCancel1::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (PrivateTestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();

  Ptr<PrivateTestCallMachine> ueAMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<PrivateTestCallMachine> ueBMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (1));

  Ptr<McpttTimer> ueATfp5 = ueAMachine->GetTfp5 ();
  Ptr<McpttTimer> ueBTfp5 = ueBMachine->GetTfp5 ();

  Ptr<McpttCallTypeMachinePrivate> ueATypeMachine = ueAMachine->GetTypeMachine ();
  Ptr<McpttCallTypeMachinePrivate> ueBTypeMachine = ueBMachine->GetTypeMachine ();

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();

  uint16_t callId = 1;
  Time startTime = Seconds (2.1);
  Time cancelTime = Seconds (2.2);
  uint32_t callerId = ueAPttApp->GetUserId ();
  uint32_t calleeId = ueBPttApp->GetUserId ();
  uint8_t commMode = McpttCallMsgFieldCommMode::AUTOMATIC_MODE;
  AddressValue grpAddress;
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  Address origAddress = ueBPttApp->GetLocalAddress ();

  ueAPttApp->GetSelectedCall ()->GetAttribute ("PeerAddress", grpAddress);

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress);
  sdp.SetSpeechPort (speechPort);

  // UE A
  ueAMachine->SetCallId (callId);
  ueAMachine->SetCallerId (callerId);
  ueAMachine->SetCalleeId (calleeId);
  ueAMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueAMachine->SetSdp (sdp);
  ueAMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));
  ueAMachine->SetAttribute ("TFP1", TimeValue (MilliSeconds (240)));
  ueAMachine->SetAttribute ("TFP3", TimeValue (MilliSeconds (240)));
  ueAMachine->SetAttribute ("AutoCommencement", BooleanValue (true));
  ueAMachine->SetStartState (McpttCallMachinePrivateStateP4::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&PrivateCallCancel1::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&PrivateCallCancel1::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallCancel1::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallCancel1::UeATimerExpCb, this));

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::PRIVATE);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  ueATypeMachine->SetState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
  ueATypeMachine->SetStarted (true);

  // UE B
  ueBMachine->SetCallId (callId);
  ueBMachine->SetCallerId (callerId);
  ueBMachine->SetCalleeId (calleeId);
  ueBMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetAttribute ("TFP4", TimeValue (MilliSeconds (240)));
  ueBMachine->SetStartState (McpttCallMachinePrivateStateP4::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&PrivateCallCancel1::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&PrivateCallCancel1::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallCancel1::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallCancel1::UeBTimerExpCb, this));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::PRIVATE);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  ueBTypeMachine->SetState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
  ueBTypeMachine->SetStarted (true);

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfp5);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfp5);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (cancelTime, &McpttPttApp::ReleaseCall, ueAPttApp);
}

void
PrivateCallCancel1::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAP3ToP1, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP3::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP3, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxReleaseAck, true, "UE A never received " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBP4ToP1, true, "UE B never made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, true, "UE B never received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, true, "UE B never sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
}

void
PrivateCallCancel1::UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
PrivateCallCancel1::UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateReleaseAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP3ToP1, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP3::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP3, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReleaseAck, false, "UE A already received " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      m_ueARxReleaseAck = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallCancel1::UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP4::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP3ToP1, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP3::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP3, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReleaseAck, false, "UE A already received " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
      m_ueAP4ToP3 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP3::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP3ToP1, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP3::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP3, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReleaseAck, false, "UE A already received " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, true, "UE A never sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
      m_ueAP3ToP1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallCancel1::UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateRelease::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP3ToP1, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP3::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP3, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReleaseAck, false, "UE A already received " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, false, "UE A already sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      m_ueATxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP4::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP4::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), true, "UE A timer " << *tfp5 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallCancel1::UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
PrivateCallCancel1::UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateRelease::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP4ToP1, true, "UE B never made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, true, "UE B never sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      m_ueBRxRelease = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallCancel1::UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateReleaseAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP4ToP1, false, "UE B already made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, false, "UE B already sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      m_ueBTxReleaseAck = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetInstanceTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP4::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP4::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), true, "UE A timer " << *tfp5 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallCancel1::UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP4::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBP4ToP1, false, "UE B already made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxRelease, false, "UE B already received " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReleaseAck, true, "UE B never sent " << McpttCallMsgPrivateReleaseAck::GetTypeId ().GetName () << ".");
      m_ueBP4ToP1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

PrivateCallCancel2::PrivateCallCancel2 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAP4ToP3 (false),
    m_ueAP3ToP1 (false),
    m_ueATxRelease (false)
{ }

void
PrivateCallCancel2::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (PrivateTestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttTestCall> ueACall = ueAPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();
  Ptr<McpttTestCall> ueBCall = ueBPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();

  Ptr<PrivateTestCallMachine> ueAMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<PrivateTestCallMachine> ueBMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (1));

  Ptr<McpttTimer> ueATfp5 = ueAMachine->GetTfp5 ();
  Ptr<McpttTimer> ueBTfp5 = ueBMachine->GetTfp5 ();

  Ptr<McpttCallTypeMachinePrivate> ueATypeMachine = ueAMachine->GetTypeMachine ();
  Ptr<McpttCallTypeMachinePrivate> ueBTypeMachine = ueBMachine->GetTypeMachine ();

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();

  uint16_t callId = 1;
  Time startTime = Seconds (2.1);
  Time cancelTime = Seconds (2.2);
  uint32_t callerId = ueAPttApp->GetUserId ();
  uint32_t calleeId = ueBPttApp->GetUserId ();
  uint8_t commMode = McpttCallMsgFieldCommMode::AUTOMATIC_MODE;
  AddressValue grpAddress;
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  Address origAddress = ueBPttApp->GetLocalAddress ();

  ueAPttApp->GetSelectedCall ()->GetAttribute ("PeerAddress", grpAddress);

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress);
  sdp.SetSpeechPort (speechPort);

  // UE A
  ueAMachine->SetCallId (callId);
  ueAMachine->SetCallerId (callerId);
  ueAMachine->SetCalleeId (calleeId);
  ueAMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueAMachine->SetSdp (sdp);
  ueAMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));
  ueAMachine->SetAttribute ("AutoCommencement", BooleanValue (true));
  ueAMachine->SetStartState (McpttCallMachinePrivateStateP4::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&PrivateCallCancel2::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&PrivateCallCancel2::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallCancel2::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallCancel2::UeATimerExpCb, this));

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::PRIVATE);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  ueATypeMachine->SetState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
  ueATypeMachine->SetStarted (true);

  // UE B
  ueBCall->AddDropper (Create<McpttMsgDropperImpl> (4, McpttCallMsgPrivateRelease::GetTypeId ()));

  ueBMachine->SetCallId (callId);
  ueBMachine->SetCallerId (callerId);
  ueBMachine->SetCalleeId (calleeId);
  ueBMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetStartState (McpttCallMachinePrivateStateP4::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&PrivateCallCancel2::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&PrivateCallCancel2::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallCancel2::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallCancel2::UeBTimerExpCb, this));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::PRIVATE);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  ueBTypeMachine->SetState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
  ueBTypeMachine->SetStarted (true);

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfp5);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfp5);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (cancelTime, &McpttPttApp::ReleaseCall, ueAPttApp);
}

void
PrivateCallCancel2::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAP3ToP1, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP3::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP3, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, 3, "UE A never sent three " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
}

void
PrivateCallCancel2::UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  if (timer.GetId () == machine.GetTfp3 ()->GetId ())
    {
      // Do nothing.
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }
}

void
PrivateCallCancel2::UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message " << msg << ".");
}

void
PrivateCallCancel2::UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP4::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP3::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP3ToP1, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP3::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP3, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, 1, "UE A never sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
      m_ueAP4ToP3 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP3::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP3ToP1, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP3::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP3, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, 3, "UE A never sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 3, "UE A counter " << *cfp3 << " is not one.");
      m_ueAP3ToP1 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallCancel2::UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateRelease::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAP3ToP1, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP3::GetStateId () << " to " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      if (m_ueATxRelease == 0)
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP3, false, "UE A already made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
          NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), true, "UE A timer " << *tfp5 << " is not running.");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP4::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP4::GetStateId () << ".");
        }
      else
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP3, true, "UE A never made the transition from " << McpttCallMachinePrivateStateP4::GetStateId () << " to " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (m_ueATxRelease, cfp3->GetValue (), "UE A already sent " << McpttCallMsgPrivateRelease::GetTypeId ().GetName () << ".");
          NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP3::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP3::GetStateId () << ".");
        }
      m_ueATxRelease += 1;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallCancel2::UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
PrivateCallCancel2::UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
}

void
PrivateCallCancel2::UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetInstanceTypeId () << ".");
}

void
PrivateCallCancel2::UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
}

PrivateCallExpiration::PrivateCallExpiration (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAExpTfp5 (false),
    m_ueAP1ToP2 (false),
    m_ueAP4ToP1 (false),
    m_ueARxRinging (false),
    m_ueATxSetup (false),
    m_ueBExpTfp5 (false),
    m_ueBP1ToP5 (false),
    m_ueBP4ToP1 (false),
    m_ueBRxSetup (false),
    m_ueBTxRinging (false)
{ }

void
PrivateCallExpiration::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (PrivateTestCallMachine::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();

  Ptr<PrivateTestCallMachine> ueAMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (0));
  Ptr<PrivateTestCallMachine> ueBMachine = DynamicCast<PrivateTestCallMachine, McpttCallMachine> (GetCallMachine (1));

  Ptr<McpttTimer> ueATfp5 = ueAMachine->GetTfp5 ();
  Ptr<McpttTimer> ueBTfp5 = ueBMachine->GetTfp5 ();

  Ptr<McpttCallTypeMachinePrivate> ueATypeMachine = ueAMachine->GetTypeMachine ();
  Ptr<McpttCallTypeMachinePrivate> ueBTypeMachine = ueBMachine->GetTypeMachine ();

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueAMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBMachine->GetEmergMachine ();

  uint16_t callId = 1;
  Time startTime = Seconds (2.1);
  Time joinTime = Seconds (2.3);
  uint32_t callerId = ueAPttApp->GetUserId ();
  uint32_t calleeId = ueBPttApp->GetUserId ();
  uint8_t commMode = McpttCallMsgFieldCommMode::AUTOMATIC_MODE;
  AddressValue grpAddress;
  uint16_t floorPort = McpttPttApp::AllocateNextPortNumber ();
  uint16_t speechPort = McpttPttApp::AllocateNextPortNumber ();
  Address origAddress = ueBPttApp->GetLocalAddress ();

  ueAPttApp->GetSelectedCall ()->GetAttribute ("PeerAddress", grpAddress);

  McpttCallMsgFieldSdp sdp;
  sdp.SetFloorPort (floorPort);
  sdp.SetGrpAddr (grpAddress.Get ());
  sdp.SetOrigAddr (origAddress);
  sdp.SetSpeechPort (speechPort);

  // UE A
  ueAMachine->SetCallId (callId);
  ueAMachine->SetCallerId (callerId);
  ueAMachine->SetCalleeId (calleeId);
  ueAMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueAMachine->SetSdp (sdp);
  ueAMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));
  ueAMachine->SetAttribute ("AutoCommencement", BooleanValue (false));
  ueAMachine->SetAttribute ("TFP1", TimeValue (MilliSeconds (240)));
  ueAMachine->SetAttribute ("TFP5", TimeValue (MilliSeconds (100)));
  ueAMachine->SetStartState (McpttCallMachinePrivateStateP4::GetInstance ());
  ueAMachine->SetPostTxCb (MakeCallback (&PrivateCallExpiration::UeATxCb, this));
  ueAMachine->SetPostRxCb (MakeCallback (&PrivateCallExpiration::UeARxCb, this));
  ueAMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallExpiration::UeAStateChangeCb, this));
  ueAMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallExpiration::UeATimerExpCb, this));

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::PRIVATE);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  ueATypeMachine->SetState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
  ueATypeMachine->SetStarted (true);

  // UE B
  ueBPttApp->SetNewCallCb (MakeCallback (&PrivateCallExpiration::NewCallCb, this));

  ueBMachine->SetCallId (callId);
  ueBMachine->SetCallerId (callerId);
  ueBMachine->SetCalleeId (calleeId);
  ueBMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueBMachine->SetSdp (sdp);
  ueBMachine->SetAttribute ("TFP5", TimeValue (MilliSeconds (100)));
  ueBMachine->SetStartState (McpttCallMachinePrivateStateP4::GetInstance ());
  ueBMachine->SetPostRxCb (MakeCallback (&PrivateCallExpiration::UeBRxCb, this));
  ueBMachine->SetPostTxCb (MakeCallback (&PrivateCallExpiration::UeBTxCb, this));
  ueBMachine->SetStateChangeTestCb (MakeCallback (&PrivateCallExpiration::UeBStateChangeCb, this));
  ueBMachine->SetPostTimerExpCb (MakeCallback (&PrivateCallExpiration::UeBTimerExpCb, this));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::PRIVATE);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  ueBTypeMachine->SetState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
  ueBTypeMachine->SetStarted (true);

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfp5);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfp5);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (joinTime, &McpttPttApp::InitiateCall, ueAPttApp);
}

void
PrivateCallExpiration::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp5, true, "UE A TFP5 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAP1ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP1, true, "UE A never made transition from " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, true, "UE A never receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfp5, true, "UE B TFP5 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBP1ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBP4ToP1, true, "UE B never made transition from " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
}

void
PrivateCallExpiration::NewCallCb (uint16_t callId)
{}

void
PrivateCallExpiration::UeATimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (timer.GetId () == tfp5->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp5, false, "UE A TFP5 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP1ToP2, false, "UE A already made transition from " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP1, true, "UE A never made transition from " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, false, "UE A already receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, false, "UE A already sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      m_ueAExpTfp5 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallExpiration::UeARxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateRinging::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp5, true, "UE A TFP5 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP1ToP2, true, "UE A never made transition from " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP1, true, "UE A never made transition from " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, false, "UE A already receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP2::GetStateId () << ".");
      m_ueARxRinging = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected message " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallExpiration::UeAStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();


  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP4::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp5, false, "UE A TFP5 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP1ToP2, false, "UE A already made transition from " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP1, false, "UE A already made transition from " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, false, "UE A already receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, false, "UE A already sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
      m_ueAP4ToP1 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP1::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp5, true, "UE A TFP5 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP1ToP2, false, "UE A already made transition from " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP1, true, "UE A never made transition from " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, false, "UE A already receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << *tfp1 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE A timer " << *tfp7 << " is running.");
      m_ueAP1ToP2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallExpiration::UeATxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();


  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp5, true, "UE A TFP5 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP1ToP2, false, "UE A already made transition from " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP2::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAP4ToP1, true, "UE A never made transition from " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxRinging, false, "UE A already receieved " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxSetup, false, "UE A already sent " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
      m_ueATxSetup = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A received unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE A timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE A timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE A timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE A timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE A timer " << *tfp7 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE A counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE A counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << *cfp6 << " is not one.");
}

void
PrivateCallExpiration::UeBTimerExpCb (const PrivateTestCallMachine& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (timer.GetId () == tfp5->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfp5, false, "UE B TFP5 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP1ToP5, false, "UE B already made transition from " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP4ToP1, true, "UE B never made transition from " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, false, "UE B already sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      m_ueBExpTfp5 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP1::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE B timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE B timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE B timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE B timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE B timer " << *tfp7 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE B counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE B counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << *cfp6 << " is not one.");
}

void
PrivateCallExpiration::UeBRxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfp5, true, "UE B TFP5 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP1ToP5, true, "UE B never made transition from " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP4ToP1, true, "UE B never made transition from " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      m_ueBRxSetup = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP5::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP5::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), true, "UE B timer " << *tfp2 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE B timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE B timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE B timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE B timer " << *tfp7 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE B counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE B counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << *cfp6 << " is not one.");
}

void
PrivateCallExpiration::UeBTxCb (const PrivateTestCallMachine& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateRinging::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfp5, true, "UE B TFP5 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP1ToP5, false, "UE B already made transition from " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP4ToP1, true, "UE B never made transition from " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, false, "UE B already sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      m_ueBTxRinging = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallMachinePrivateStateP1::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " but should be in " << McpttCallMachinePrivateStateP1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE B timer " << *tfp2 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE B timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE B timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE B timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE B timer " << *tfp7 << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE B counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE B counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << *cfp6 << " is not one.");
}

void
PrivateCallExpiration::UeBStateChangeCb (const PrivateTestCallMachine& machine, Ptr<McpttCallMachinePrivateState>  oldState, Ptr<McpttCallMachinePrivateState>  newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetTfp1 ();
  Ptr<McpttTimer> tfp2 = machine.GetTfp2 ();
  Ptr<McpttTimer> tfp3 = machine.GetTfp3 ();
  Ptr<McpttTimer> tfp4 = machine.GetTfp4 ();
  Ptr<McpttTimer> tfp5 = machine.GetTfp5 ();
  Ptr<McpttTimer> tfp6 = machine.GetTfp6 ();
  Ptr<McpttTimer> tfp7 = machine.GetTfp7 ();
  Ptr<McpttTimer> tfp8 = machine.GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetCfp1 ();
  Ptr<McpttCounter> cfp3 = machine.GetCfp3 ();
  Ptr<McpttCounter> cfp4 = machine.GetCfp4 ();
  Ptr<McpttCounter> cfp6 = machine.GetCfp6 ();
  Ptr<McpttCallMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP4::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfp5, false, "UE B TFP5 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP1ToP5, false, "UE B already made transition from " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP4ToP1, false, "UE B already made transition from " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, false, "UE B already sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), false, "UE B timer " << *tfp2 << " is running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), true, "UE B timer " << *tfp7 << " is not running.");
      m_ueBP4ToP1 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallMachinePrivateStateP1::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallMachinePrivateStateP5::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfp5, true, "UE B TFP5 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP1ToP5, false, "UE B already made transition from " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP5::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBP4ToP1, true, "UE B never made transition from " << McpttCallMachinePrivateStateP4::GetStateId ().GetName () << " to " << McpttCallMachinePrivateStateP1::GetStateId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxSetup, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxRinging, true, "UE B never sent " << McpttCallMsgPrivateRinging::GetTypeId ().GetName () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp2->IsRunning (), true, "UE B timer " << *tfp2 << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (tfp7->IsRunning (), false, "UE B timer " << *tfp7 << " is running.");
      m_ueBP1ToP5 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected state transition from " << *oldState << " to " << *newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << *tfp1 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp3->IsRunning (), false, "UE B timer " << *tfp3 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp4->IsRunning (), false, "UE B timer " << *tfp4 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp5->IsRunning (), false, "UE B timer " << *tfp5 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << *tfp6 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << *tfp8 << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << *cfp1 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp3->GetValue (), 1, "UE B counter " << *cfp3 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp4->GetValue (), 1, "UE B counter " << *cfp4 << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << *cfp6 << " is not one.");
}

McpttPrivateCallControlTestSuite::McpttPrivateCallControlTestSuite (void)
  : TestSuite ("mcptt-call-control-private", TestSuite::SYSTEM)
{
  AddTestCase (new PrivateCallSetupAuto1 (), TestCase::QUICK);
  AddTestCase (new PrivateCallSetupAuto3 (), TestCase::QUICK);
  AddTestCase (new PrivateCallSetupManual1 (), TestCase::QUICK);
  AddTestCase (new PrivateCallSetupManual3 (), TestCase::QUICK);
  AddTestCase (new PrivateCallSetupFailure (), TestCase::QUICK);
  AddTestCase (new PrivateCallCancel1 (), TestCase::QUICK);
  AddTestCase (new PrivateCallCancel2 (), TestCase::QUICK);
  AddTestCase (new PrivateCallExpiration (), TestCase::QUICK);
}

} // namespace tests
} // namespace psc
} // namespace ns3

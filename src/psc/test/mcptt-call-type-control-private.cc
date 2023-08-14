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
#include <ns3/mcptt-call-type-machine-private.h>
#include <ns3/mcptt-call-msg.h>
#include <ns3/mcptt-ptt-app.h>

#include "mcptt-test-call.h"
#include "mcptt-test-case.h"
#include "mcptt-test-case-config.h"
#include "mcptt-msg-dropper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("McpttCallTypePrivateControlTest");

namespace psc {
namespace tests {

class TestCallTypeMachinePrivate : public McpttCallTypeMachinePrivate
{
public:
  static TypeId GetTypeId (void);
  TestCallTypeMachinePrivate (void);
  virtual ~TestCallTypeMachinePrivate (void);
  virtual void ChangeState (Ptr<McpttCallTypeMachinePrivateState> state);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void ReceiveAccept (const McpttCallMsgPrivateAccept& msg);
  virtual void ReceiveAcceptAck (const McpttCallMsgPrivateAcceptAck& msg);
  virtual void ReceiveEmergencyCancel (const McpttCallMsgPrivateEmergCancel& msg);
  virtual void ReceiveEmergencyCancelAck (const McpttCallMsgPrivateEmergCancelAck& msg);
  virtual void ReceiveReject (const McpttCallMsgPrivateReject& msg);
  virtual void ReceiveRelease (const McpttCallMsgPrivateRelease& msg);
  virtual void ReceiveSetupRequest (const McpttCallMsgPrivateSetupReq& msg);
  virtual void Start (void);
  virtual void Send (const McpttCallMsg& msg);

protected:
  virtual void ExpiryOfTfp1 (void);
  virtual void ExpiryOfTfp6 (void);
  virtual void ExpiryOfTfp8 (void);

private:
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> m_postRxCb;
  Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&> m_postTimerExpCb;
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> m_postTxCb;
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> m_preRxCb;
  Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&> m_preTimerExpCb;
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> m_preTxCb;
  Ptr<McpttCallTypeMachinePrivateState> m_startState;
  Callback<void, const TestCallTypeMachinePrivate&, Ptr<McpttCallTypeMachinePrivateState>, Ptr<McpttCallTypeMachinePrivateState> > m_stateChangeCb;

public:
  virtual Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> GetPostRxCb (void) const;
  virtual Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&> GetPostTimerExpCb (void) const;
  virtual Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> GetPostTxCb (void) const;
  virtual Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> GetPreRxCb (void) const;
  virtual Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&> GetPreTimerExpCb (void) const;
  virtual Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> GetPreTxCb (void) const;
  virtual Callback<void, const TestCallTypeMachinePrivate&, Ptr<McpttCallTypeMachinePrivateState>, Ptr<McpttCallTypeMachinePrivateState> > GetStateChangeCb (void) const;
  virtual void SetPostRxCb (const Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&>  postRxCb);
  virtual void SetPostTimerExpCb (const Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&>  timerExpCb);
  virtual void SetPostTxCb (const Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&>  postTxCb);
  virtual void SetPreRxCb (const Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&>  preRxCb);
  virtual void SetPreTimerExpCb (const Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&>  timerExpCb);
  virtual void SetPreTxCb (const Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&>  preTxCb);
  virtual void SetStateChangeTestCb (const Callback<void, const TestCallTypeMachinePrivate&, Ptr<McpttCallTypeMachinePrivateState>, Ptr<McpttCallTypeMachinePrivateState> >  stateChangeCb);
};

class McpttCallTypePrivateEnterPrivateCall : public McpttTestCase
{
public:
  McpttCallTypePrivateEnterPrivateCall (const std::string& name = "Private Call Type - Enter Private Call", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);

private:
  bool m_ueARxAccept;
  bool m_ueAQ0ToQ1;
  bool m_ueBRxReq;
  bool m_ueBRxAcceptAck;
  bool m_ueBQ0ToQ1;
};

class McpttCallTypePrivateEnterPrivateEmergencyCall : public McpttTestCase
{
public:
  McpttCallTypePrivateEnterPrivateEmergencyCall (const std::string& name = "Private Call Type - Enter Private Emergency Call", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);

private:
  bool m_ueARxAccept;
  bool m_ueAQ0ToQ2;
  bool m_ueBRxReq;
  bool m_ueBRxAcceptAck;
  bool m_ueBQ0ToQ2;
};

class McpttCallTypePrivateUpgradeCall1 : public McpttTestCase
{
public:
  McpttCallTypePrivateUpgradeCall1 (const std::string& name = "Private Call Type - Upgrade Call 1", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);

private:
  bool m_ueAQ1ToQ2;
  bool m_ueARxAccept;
  bool m_ueATxReq;
  bool m_ueATxAcceptAck;
  bool m_ueBQ1ToQ2;
  bool m_ueBRxReq;
  bool m_ueBTxAccept;
};

class McpttCallTypePrivateUpgradeCall2 : public McpttTestCase
{
public:
  McpttCallTypePrivateUpgradeCall2 (const std::string& name = "Private Call Type - Upgrade Call 2", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);

private:
  bool m_ueAExpTfp1;
  bool m_ueAQ1ToQ2;
  bool m_ueAQ2ToQ1;
  bool m_ueARxReject;
  uint32_t m_ueATxReq;
  bool m_ueBRxReq;
  bool m_ueBTxReject;
};

class McpttCallTypePrivateUpgradeCall3 : public McpttTestCase
{
public:
  McpttCallTypePrivateUpgradeCall3 (const std::string& name = "Private Call Type - Upgrade Call 3", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);

private:
  uint32_t m_ueAExpTfp1;
  bool m_ueAQ1ToQ2;
  bool m_ueAQ2ToQ0;
  uint32_t m_ueATxReq;
};

class McpttCallTypePrivateDowngradeCall1 : public McpttTestCase
{
public:
  McpttCallTypePrivateDowngradeCall1 (const std::string& name = "Private Call Type - Downgrade Call 1", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);

private:
  bool m_ueAExpTfp6;
  bool m_ueAQ2ToQ1;
  bool m_ueARxCancelAck;
  uint32_t m_ueATxCancel;
  bool m_ueBQ2ToQ1;
  bool m_ueBRxCancel;
  bool m_ueBTxCancelAck;
};

class McpttCallTypePrivateDowngradeCall2 : public McpttTestCase
{
public:
  McpttCallTypePrivateDowngradeCall2 (const std::string& name = "Private Call Type - Downgrade Call 2", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);

private:
  bool m_ueAExpTfp6;
  bool m_ueAQ1ToQ0;
  bool m_ueAQ2ToQ1;
  bool m_ueATxCancel;
};

class McpttCallTypePrivateDowngradeCall3 : public McpttTestCase
{
public:
  McpttCallTypePrivateDowngradeCall3 (const std::string& name = "Private Call Type - Downgrade Call 3", Ptr<McpttTestCaseConfig> config = Create<McpttTestCaseConfig> ());

protected:
  virtual void Configure (void);
  virtual void Execute (void);
  virtual void UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);
  virtual void UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState);
  virtual void UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer);
  virtual void UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg);

private:
  bool m_ueAExpTfp8;
  bool m_ueAQ2ToQ1;
  bool m_ueBExpTfp8;
  bool m_ueBQ2ToQ1;
};

class McpttCallTypePrivateControlTestSuite : public TestSuite
{
public:
  McpttCallTypePrivateControlTestSuite (void);
};

/***************************************************************
 *  Implementation of the code declared above.
 ***************************************************************/

static McpttCallTypePrivateControlTestSuite callSuite;

TypeId
TestCallTypeMachinePrivate::GetTypeId (void)
{
  static TypeId tid = TypeId ("TestCallTypeMachinePrivate")
    .SetParent<McpttCallTypeMachinePrivate> ()
    .AddConstructor<TestCallTypeMachinePrivate> ()
  ;

  return tid;
}

TestCallTypeMachinePrivate::TestCallTypeMachinePrivate (void)
  : McpttCallTypeMachinePrivate (),
    m_postRxCb (MakeNullCallback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> ()),
    m_postTimerExpCb (MakeNullCallback<void, const TestCallTypeMachinePrivate&, const McpttTimer&> ()),
    m_postTxCb (MakeNullCallback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> ()),
    m_preRxCb (MakeNullCallback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> ()),
    m_preTimerExpCb (MakeNullCallback<void, const TestCallTypeMachinePrivate&, const McpttTimer&> ()),
    m_preTxCb (MakeNullCallback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> ()),
    m_startState (McpttCallTypeMachinePrivateStateQ0::GetInstance ()),
    m_stateChangeCb (MakeNullCallback<void, const TestCallTypeMachinePrivate&, Ptr<McpttCallTypeMachinePrivateState>, Ptr<McpttCallTypeMachinePrivateState> > ())
{ }

TestCallTypeMachinePrivate::~TestCallTypeMachinePrivate (void)
{ }

void
TestCallTypeMachinePrivate::ReceiveAccept (const McpttCallMsgPrivateAccept& msg)
{
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallTypeMachinePrivate::ReceiveAccept (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
TestCallTypeMachinePrivate::ReceiveAcceptAck (const McpttCallMsgPrivateAcceptAck& msg)
{
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallTypeMachinePrivate::ReceiveAcceptAck (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
TestCallTypeMachinePrivate::ReceiveEmergencyCancel (const McpttCallMsgPrivateEmergCancel& msg)
{
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallTypeMachinePrivate::ReceiveEmergencyCancel (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
TestCallTypeMachinePrivate::ReceiveEmergencyCancelAck (const McpttCallMsgPrivateEmergCancelAck& msg)
{
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallTypeMachinePrivate::ReceiveEmergencyCancelAck (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
TestCallTypeMachinePrivate::ReceiveReject (const McpttCallMsgPrivateReject& msg)
{
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallTypeMachinePrivate::ReceiveReject (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
TestCallTypeMachinePrivate::ReceiveRelease (const McpttCallMsgPrivateRelease& msg)
{
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallTypeMachinePrivate::ReceiveRelease (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
TestCallTypeMachinePrivate::ReceiveSetupRequest (const McpttCallMsgPrivateSetupReq& msg)
{
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> postRxCb = GetPostRxCb ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> preRxCb = GetPreRxCb ();

  if (!preRxCb.IsNull ())
    {
      preRxCb (*this, msg);
    }

  McpttCallTypeMachinePrivate::ReceiveSetupRequest (msg);

  if (!postRxCb.IsNull ())
    {
      postRxCb (*this, msg);
    }
}

void
TestCallTypeMachinePrivate::Send (const McpttCallMsg& msg)
{
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> postTxCb = GetPostTxCb ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&> preTxCb = GetPreTxCb ();

  if (!preTxCb.IsNull ())
    {
      preTxCb (*this, msg);
    }

  McpttCallTypeMachinePrivate::Send (msg);

  if (!postTxCb.IsNull ())
    {
      postTxCb (*this, msg);
    }
}

void
TestCallTypeMachinePrivate::ExpiryOfTfp1 (void)
{
  Ptr<McpttTimer> tfp = GetOwner ()->GetTfp1 ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfp);
    }

  McpttCallTypeMachinePrivate::ExpiryOfTfp1 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfp);
    }
}

void
TestCallTypeMachinePrivate::ExpiryOfTfp6 (void)
{
  Ptr<McpttTimer> tfp = GetOwner ()->GetTfp6 ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfp);
    }

  McpttCallTypeMachinePrivate::ExpiryOfTfp6 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfp);
    }
}

void
TestCallTypeMachinePrivate::ExpiryOfTfp8 (void)
{
  Ptr<McpttTimer> tfp = GetOwner ()->GetTfp8 ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&> postTimerExpCb = GetPostTimerExpCb ();
  Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&> preTimerExpCb = GetPreTimerExpCb ();

  if (!preTimerExpCb.IsNull ())
    {
      preTimerExpCb (*this, *tfp);
    }

  McpttCallTypeMachinePrivate::ExpiryOfTfp8 ();

  if (!postTimerExpCb.IsNull ())
    {
      postTimerExpCb (*this, *tfp);
    }
}

void
TestCallTypeMachinePrivate::Start (void)
{
  McpttCallTypeMachinePrivate::Start ();
}

void
TestCallTypeMachinePrivate::ChangeState (Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttCallTypeMachinePrivateState> oldState = GetState ();
  Callback<void, const TestCallTypeMachinePrivate&, Ptr<McpttCallTypeMachinePrivateState>, Ptr<McpttCallTypeMachinePrivateState> > stateChangeCb = GetStateChangeCb ();

  McpttCallTypeMachinePrivate::ChangeState (newState);

  if (!stateChangeCb.IsNull ())
    {
      stateChangeCb (*this, oldState, newState);
    }
}

TypeId
TestCallTypeMachinePrivate::GetInstanceTypeId (void) const
{
  return TestCallTypeMachinePrivate::GetTypeId ();
}

Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&>
TestCallTypeMachinePrivate::GetPostRxCb (void) const
{
  return m_postRxCb;
}

Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&>
TestCallTypeMachinePrivate::GetPostTimerExpCb (void) const
{
  return m_postTimerExpCb;
}


Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&>
TestCallTypeMachinePrivate::GetPostTxCb (void) const
{
  return m_postTxCb;
}

Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&>
TestCallTypeMachinePrivate::GetPreRxCb (void) const
{
  return m_preRxCb;
}

Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&>
TestCallTypeMachinePrivate::GetPreTimerExpCb (void) const
{
  return m_preTimerExpCb;
}

Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&>
TestCallTypeMachinePrivate::GetPreTxCb (void) const
{
  return m_preTxCb;
}

Callback<void, const TestCallTypeMachinePrivate&, Ptr<McpttCallTypeMachinePrivateState>, Ptr<McpttCallTypeMachinePrivateState> >
TestCallTypeMachinePrivate::GetStateChangeCb (void) const
{
  return m_stateChangeCb;
}

void
TestCallTypeMachinePrivate::SetPostRxCb (const Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&>  postRxCb)
{
  m_postRxCb = postRxCb;
}

void
TestCallTypeMachinePrivate::SetPostTimerExpCb (const Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&>  timerExpCb)
{
  m_postTimerExpCb = timerExpCb;
}

void
TestCallTypeMachinePrivate::SetPostTxCb (const Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&>  postTxCb)
{
  m_postTxCb = postTxCb;
}

void
TestCallTypeMachinePrivate::SetPreRxCb (const Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&>  preRxCb)
{
  m_preRxCb = preRxCb;
}

void
TestCallTypeMachinePrivate::SetPreTimerExpCb (const Callback<void, const TestCallTypeMachinePrivate&, const McpttTimer&>  timerExpCb)
{
  m_preTimerExpCb = timerExpCb;
}

void
TestCallTypeMachinePrivate::SetPreTxCb (const Callback<void, const TestCallTypeMachinePrivate&, const McpttCallMsg&>  preTxCb)
{
  m_preTxCb = preTxCb;
}

void
TestCallTypeMachinePrivate::SetStateChangeTestCb (const Callback<void, const TestCallTypeMachinePrivate&, Ptr<McpttCallTypeMachinePrivateState>, Ptr<McpttCallTypeMachinePrivateState> >  stateChangeCb)
{
  m_stateChangeCb = stateChangeCb;
}

McpttCallTypePrivateEnterPrivateCall::McpttCallTypePrivateEnterPrivateCall (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueARxAccept (false),
    m_ueAQ0ToQ1 (false),
    m_ueBRxReq (false),
    m_ueBRxAcceptAck (false),
    m_ueBQ0ToQ1 (false)
{ }

void
McpttCallTypePrivateEnterPrivateCall::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (McpttCallMachinePrivate::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttCallMachinePrivate> ueACallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachinePrivate> ueBCallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (1));

  Ptr<TestCallTypeMachinePrivate> ueATypeMachine = CreateObject<TestCallTypeMachinePrivate> ();
  Ptr<TestCallTypeMachinePrivate> ueBTypeMachine = CreateObject<TestCallTypeMachinePrivate> ();

  ueACallMachine->SetTypeMachine (ueATypeMachine);
  ueBCallMachine->SetTypeMachine (ueBTypeMachine);

  Time startTime = Seconds (2.1);

  // UE A
  ueACallMachine->SetAttribute ("TFP1", TimeValue (MilliSeconds (240)));
  ueACallMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));

  ueATypeMachine->SetState (McpttCallTypeMachinePrivateStateQ0::GetInstance ());
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateEnterPrivateCall::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateEnterPrivateCall::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateEnterPrivateCall::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateEnterPrivateCall::UeATimerExpCb, this));

  // UE B
  ueBCallMachine->SetAttribute ("TFP4", TimeValue (MilliSeconds (240)));

  ueBTypeMachine->SetState (McpttCallTypeMachinePrivateStateQ0::GetInstance ());
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateEnterPrivateCall::UeBRxCb, this));
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateEnterPrivateCall::UeBTxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateEnterPrivateCall::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateEnterPrivateCall::UeBTimerExpCb, this));

  Simulator::Schedule (startTime, &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
McpttCallTypePrivateEnterPrivateCall::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, true, "UE A never received " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQ0ToQ1, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, true, "UE B never received " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBQ0ToQ1, true, "UE B never made the transition from " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
}

void
McpttCallTypePrivateEnterPrivateCall::UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypePrivateEnterPrivateCall::UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ0ToQ1, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      m_ueARxAccept = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateEnterPrivateCall::UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ0ToQ1, false, "UE already never made the transition form " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      m_ueAQ0ToQ1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldState << " to " << newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateEnterPrivateCall::UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypePrivateEnterPrivateCall::UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypePrivateEnterPrivateCall::UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ0ToQ1, false, "UE B already made the transition from " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ0::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
      m_ueBRxReq = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateAcceptAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ0ToQ1, true, "UE B never made the transition from " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      m_ueBRxAcceptAck = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateEnterPrivateCall::UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypePrivateEnterPrivateCall::UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ0ToQ1, false, "UE B already made the transition from " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      m_ueBQ0ToQ1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B state transition from " << oldState << " to " << newState << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

McpttCallTypePrivateEnterPrivateEmergencyCall::McpttCallTypePrivateEnterPrivateEmergencyCall (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueARxAccept (false),
    m_ueAQ0ToQ2 (false),
    m_ueBRxReq (false),
    m_ueBRxAcceptAck (false),
    m_ueBQ0ToQ2 (false)
{ }

void
McpttCallTypePrivateEnterPrivateEmergencyCall::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (McpttCallMachinePrivate::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttCallMachinePrivate> ueACallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachinePrivate> ueBCallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (1));

  Ptr<TestCallTypeMachinePrivate> ueATypeMachine = CreateObject<TestCallTypeMachinePrivate> ();
  Ptr<TestCallTypeMachinePrivate> ueBTypeMachine = CreateObject<TestCallTypeMachinePrivate> ();

  ueACallMachine->SetTypeMachine (ueATypeMachine);
  ueBCallMachine->SetTypeMachine (ueBTypeMachine);

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueACallMachine->GetEmergMachine ();

  Time startTime = Seconds (2.1);

  // UE A
  ueACallMachine->SetAttribute ("TFP1", TimeValue (MilliSeconds (240)));
  ueACallMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));

  ueAEmergMachine->SetStarted (true);
  ueAEmergMachine->SetEmerg (true);

  ueATypeMachine->SetState (McpttCallTypeMachinePrivateStateQ0::GetInstance ());
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateEnterPrivateEmergencyCall::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateEnterPrivateEmergencyCall::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateEnterPrivateEmergencyCall::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateEnterPrivateEmergencyCall::UeATimerExpCb, this));

  // UE B
  ueBCallMachine->SetAttribute ("TFP4", TimeValue (MilliSeconds (240)));

  ueBTypeMachine->SetState (McpttCallTypeMachinePrivateStateQ0::GetInstance ());
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateEnterPrivateEmergencyCall::UeBRxCb, this));
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateEnterPrivateEmergencyCall::UeBTxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateEnterPrivateEmergencyCall::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateEnterPrivateEmergencyCall::UeBTimerExpCb, this));

  Simulator::Schedule (startTime, &McpttPttApp::TakePushNotification, ueAPttApp);
}

void
McpttCallTypePrivateEnterPrivateEmergencyCall::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, true, "UE A never received " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQ0ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, true, "UE B never received " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBQ0ToQ2, true, "UE B never made the transition from " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
}

void
McpttCallTypePrivateEnterPrivateEmergencyCall::UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypePrivateEnterPrivateEmergencyCall::UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ0ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      m_ueARxAccept = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), true, "UE A timer " << tfp8->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateEnterPrivateEmergencyCall::UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ0ToQ2, false, "UE already never made the transition form " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      m_ueAQ0ToQ2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), true, "UE A timer " << tfp8->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateEnterPrivateEmergencyCall::UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypePrivateEnterPrivateEmergencyCall::UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypePrivateEnterPrivateEmergencyCall::UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ0ToQ2, false, "UE B already made the transition from " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ0::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << tfp8->GetId () << " is running.");
      m_ueBRxReq = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateAcceptAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ0ToQ2, true, "UE B never made the transition from " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ2::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), true, "UE B timer " << tfp8->GetId () << " is not running.");
      m_ueBRxAcceptAck = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateEnterPrivateEmergencyCall::UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypePrivateEnterPrivateEmergencyCall::UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ0::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxAcceptAck, false, "UE B already received " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ0ToQ2, false, "UE B already made the transition from " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      m_ueBQ0ToQ2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), true, "UE B timer " << tfp8->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

McpttCallTypePrivateUpgradeCall1::McpttCallTypePrivateUpgradeCall1 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAQ1ToQ2 (false),
    m_ueARxAccept (false),
    m_ueATxReq (false),
    m_ueATxAcceptAck (false),
    m_ueBQ1ToQ2 (false),
    m_ueBRxReq (false),
    m_ueBTxAccept (false)
{ }

void
McpttCallTypePrivateUpgradeCall1::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (McpttCallMachinePrivate::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttCall> ueACall = ueAPttApp->GetSelectedCall ();
  Ptr<McpttCall> ueBCall = ueBPttApp->GetSelectedCall ();

  Ptr<McpttCallMachinePrivate> ueACallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachinePrivate> ueBCallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (1));

  Ptr<McpttTimer> ueATfp5 = ueACallMachine->GetTfp5 ();
  Ptr<McpttTimer> ueBTfp5 = ueBCallMachine->GetTfp5 ();

  Ptr<TestCallTypeMachinePrivate> ueATypeMachine = CreateObject<TestCallTypeMachinePrivate> ();
  Ptr<TestCallTypeMachinePrivate> ueBTypeMachine = CreateObject<TestCallTypeMachinePrivate> ();

  ueACallMachine->SetTypeMachine (ueATypeMachine);
  ueBCallMachine->SetTypeMachine (ueBTypeMachine);

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueACallMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBCallMachine->GetEmergMachine ();

  uint16_t callId = 1;
  Time startTime = Seconds (2.1);
  Time upgradeTime = Seconds (2.2);
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
  ueACallMachine->SetCallId (callId);
  ueACallMachine->SetCallerId (callerId);
  ueACallMachine->SetCalleeId (calleeId);
  ueACallMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueACallMachine->SetSdp (sdp);
  ueACallMachine->SetAttribute ("TFP1", TimeValue (MilliSeconds (240)));
  ueACallMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::PRIVATE);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  ueATypeMachine->SetState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateUpgradeCall1::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateUpgradeCall1::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateUpgradeCall1::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateUpgradeCall1::UeATimerExpCb, this));

  // UE B
  ueBCallMachine->SetCallId (callId);
  ueBCallMachine->SetCallerId (callerId);
  ueBCallMachine->SetCalleeId (calleeId);
  ueBCallMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueBCallMachine->SetSdp (sdp);
  ueBCallMachine->SetAttribute ("TFP4", TimeValue (MilliSeconds (240)));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::PRIVATE);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  ueBTypeMachine->SetState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateUpgradeCall1::UeBRxCb, this));
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateUpgradeCall1::UeBTxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateUpgradeCall1::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateUpgradeCall1::UeBTimerExpCb, this));

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfp5);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfp5);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (upgradeTime, &McpttPttApp::UpgradeCallType, ueAPttApp, McpttCallMsgFieldCallType::EMERGENCY_PRIVATE);
}

void
McpttCallTypePrivateUpgradeCall1::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, true, "UE A never received " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, true, "UE A never sent " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBQ1ToQ2, true, "UE B never made the transition from " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
}

void
McpttCallTypePrivateUpgradeCall1::UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
}

void
McpttCallTypePrivateUpgradeCall1::UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, true, "UE A never sent " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
      m_ueARxAccept = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), true, "UE A timer " << tfp8->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall1::UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, false, "UE already never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, false, "UE A already sent " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
      m_ueAQ1ToQ2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << tfp1->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall1::UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, false, "UE already never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, false, "UE A already sent " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, false, "UE A already sent " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
      m_ueATxReq = true;
    }
  else if (msg.IsA (McpttCallMsgPrivateAcceptAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueARxAccept, false, "UE A already received " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, true, "UE never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, true, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxAcceptAck, false, "UE A already sent " << McpttCallMsgPrivateAcceptAck::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << tfp1->GetId () << " is not running.");
      m_ueATxAcceptAck = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall1::UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypePrivateUpgradeCall1::UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ1ToQ2, true, "UE B never made the transition from " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      m_ueBRxReq = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ2::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), true, "UE B timer " << tfp8->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall1::UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateAccept::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, false, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ1ToQ2, false, "UE B already made the transition from " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      m_ueBTxAccept = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall1::UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxAccept, true, "UE B never sent " << McpttCallMsgPrivateAccept::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ1ToQ2, false, "UE B already made the transition from " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      m_ueBQ1ToQ2 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), true, "UE B timer " << tfp8->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

McpttCallTypePrivateUpgradeCall2::McpttCallTypePrivateUpgradeCall2 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAExpTfp1 (false),
    m_ueAQ1ToQ2 (false),
    m_ueAQ2ToQ1 (false),
    m_ueARxReject (false),
    m_ueATxReq (0),
    m_ueBRxReq (false),
    m_ueBTxReject (false)
{ }

void
McpttCallTypePrivateUpgradeCall2::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (McpttCallMachinePrivate::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttTestCall> ueACall = ueAPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();
  Ptr<McpttTestCall> ueBCall = ueBPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();

  Ptr<McpttCallMachinePrivate> ueACallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachinePrivate> ueBCallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (1));

  Ptr<McpttTimer> ueATfp5 = ueACallMachine->GetTfp5 ();
  Ptr<McpttTimer> ueBTfp5 = ueBCallMachine->GetTfp5 ();

  Ptr<TestCallTypeMachinePrivate> ueATypeMachine = CreateObject<TestCallTypeMachinePrivate> ();
  Ptr<TestCallTypeMachinePrivate> ueBTypeMachine = CreateObject<TestCallTypeMachinePrivate> ();

  ueACallMachine->SetTypeMachine (ueATypeMachine);
  ueBCallMachine->SetTypeMachine (ueBTypeMachine);

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueACallMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBCallMachine->GetEmergMachine ();

  uint16_t callId = 1;
  Time startTime = Seconds (2.1);
  Time upgradeTime = Seconds (2.2);
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
  ueACallMachine->SetCallId (callId);
  ueACallMachine->SetCallerId (callerId);
  ueACallMachine->SetCalleeId (calleeId);
  ueACallMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueACallMachine->SetSdp (sdp);
  ueACallMachine->SetAttribute ("TFP1", TimeValue (MilliSeconds (240)));
  ueACallMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::PRIVATE);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  ueATypeMachine->SetState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateUpgradeCall2::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateUpgradeCall2::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateUpgradeCall2::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateUpgradeCall2::UeATimerExpCb, this));

  // UE B
  ueBCall->AddDropper (Create<McpttMsgDropperImpl> (2, McpttCallMsgPrivateSetupReq::GetTypeId ()));

  ueBCallMachine->SetCallId (callId);
  ueBCallMachine->SetCallerId (callerId);
  ueBCallMachine->SetCalleeId (calleeId);
  ueBCallMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueBCallMachine->SetSdp (sdp);
  ueBCallMachine->SetAttribute ("TFP4", TimeValue (MilliSeconds (240)));
  ueBCallMachine->SetAttribute ("Failure", BooleanValue (true));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::PRIVATE);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  ueBTypeMachine->SetState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateUpgradeCall2::UeBRxCb, this));
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateUpgradeCall2::UeBTxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateUpgradeCall2::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateUpgradeCall2::UeBTimerExpCb, this));

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfp5);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfp5);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (upgradeTime, &McpttPttApp::UpgradeCallType, ueAPttApp, McpttCallMsgFieldCallType::EMERGENCY_PRIVATE);
}

void
McpttCallTypePrivateUpgradeCall2::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp1, true, "UE timer TFP1 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxReject, true, "UE A never received " << McpttCallMsgPrivateReject::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, 2, "UE A never sent 2 " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, true, "UE B never received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxReject, true, "UE B never sent " << McpttCallMsgPrivateReject::GetTypeId () << ".");
}

void
McpttCallTypePrivateUpgradeCall2::UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (timer.GetId () == tfp1->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp1, false, "UE timer TFP1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, false, "UE A already made the transition form " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReject, false, "UE A already received " << McpttCallMsgPrivateReject::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, 2, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      m_ueAExpTfp1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << tfp1->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 2, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall2::UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateReject::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp1, true, "UE timer TFP1 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReject, false, "UE A already received " << McpttCallMsgPrivateReject::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, 2, "UE A never sent 1 " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      m_ueARxReject = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 2, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall2::UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp1, false, "UE timer TFP1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, false, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, false, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReject, false, "UE A already received " << McpttCallMsgPrivateReject::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, 1, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << tfp1->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
      m_ueAQ1ToQ2 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp1, true, "UE timer TFP1 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, false, "UE A already made the transition form " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReject, false, "UE A already received " << McpttCallMsgPrivateReject::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, 2, "UE A never sent 2 " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 2, "UE A counter " << cfp1->GetId () << " is not two.");
      m_ueAQ2ToQ1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall2::UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      if (m_ueATxReq == 0)
        {
          NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
          NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, false, "UE A already made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
        }
      else if (m_ueATxReq == 1)
        {
          NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 2, "UE A counter " << cfp1->GetId () << " is not two.");
          NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
        }
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp1, false, "UE timer TFP1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, false, "UE A already made the transition form " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxReject, false, "UE A already received " << McpttCallMsgPrivateReject::GetTypeId () << ".");
      m_ueATxReq += 1;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall2::UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypePrivateUpgradeCall2::UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReject, true, "UE B never sent " << McpttCallMsgPrivateReject::GetTypeId () << ".");
      m_ueBRxReq = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall2::UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateReject::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxReq, false, "UE B already received " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxReject, false, "UE B never sent " << McpttCallMsgPrivateReject::GetTypeId () << ".");
      m_ueBTxReject = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall2::UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
}

McpttCallTypePrivateUpgradeCall3::McpttCallTypePrivateUpgradeCall3 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAExpTfp1 (0),
    m_ueAQ1ToQ2 (false),
    m_ueAQ2ToQ0 (false),
    m_ueATxReq (0)
{ }

void
McpttCallTypePrivateUpgradeCall3::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (McpttCallMachinePrivate::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttTestCall> ueACall = ueAPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();
  Ptr<McpttTestCall> ueBCall = ueBPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();

  Ptr<McpttCallMachinePrivate> ueACallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachinePrivate> ueBCallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (1));

  Ptr<McpttTimer> ueATfp5 = ueACallMachine->GetTfp5 ();
  Ptr<McpttTimer> ueBTfp5 = ueBCallMachine->GetTfp5 ();

  Ptr<TestCallTypeMachinePrivate> ueATypeMachine = CreateObject<TestCallTypeMachinePrivate> ();
  Ptr<TestCallTypeMachinePrivate> ueBTypeMachine = CreateObject<TestCallTypeMachinePrivate> ();

  ueACallMachine->SetTypeMachine (ueATypeMachine);
  ueBCallMachine->SetTypeMachine (ueBTypeMachine);

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueACallMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBCallMachine->GetEmergMachine ();

  uint16_t callId = 1;
  Time startTime = Seconds (2.1);
  Time upgradeTime = Seconds (2.2);
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
  ueACallMachine->SetCallId (callId);
  ueACallMachine->SetCallerId (callerId);
  ueACallMachine->SetCalleeId (calleeId);
  ueACallMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueACallMachine->SetSdp (sdp);
  ueACallMachine->SetAttribute ("TFP1", TimeValue (MilliSeconds (240)));
  ueACallMachine->SetAttribute ("CFP1", UintegerValue (2));
  ueACallMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));

  ueAEmergMachine->SetEmerg (false);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::PRIVATE);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  ueATypeMachine->SetState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateUpgradeCall3::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateUpgradeCall3::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateUpgradeCall3::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateUpgradeCall3::UeATimerExpCb, this));

  // UE B
  ueBCall->AddDropper (Create<McpttMsgDropperImpl> (4, McpttCallMsgPrivateSetupReq::GetTypeId ()));

  ueBCallMachine->SetCallId (callId);
  ueBCallMachine->SetCallerId (callerId);
  ueBCallMachine->SetCalleeId (calleeId);
  ueBCallMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueBCallMachine->SetSdp (sdp);
  ueBCallMachine->SetAttribute ("TFP4", TimeValue (MilliSeconds (240)));
  ueBCallMachine->SetAttribute ("Failure", BooleanValue (true));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::PRIVATE);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::PRIVATE));
  ueBTypeMachine->SetState (McpttCallTypeMachinePrivateStateQ1::GetInstance ());
  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateUpgradeCall3::UeBRxCb, this));
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateUpgradeCall3::UeBTxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateUpgradeCall3::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateUpgradeCall3::UeBTimerExpCb, this));

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfp5);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfp5);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (upgradeTime, &McpttPttApp::UpgradeCallType, ueAPttApp, McpttCallMsgFieldCallType::EMERGENCY_PRIVATE);
}

void
McpttCallTypePrivateUpgradeCall3::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp1, 2, "UE timer TFP1 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ0, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, 2, "UE A never sent 2 " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
}

void
McpttCallTypePrivateUpgradeCall3::UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (timer.GetId () == tfp1->GetId ())
    {
      if (m_ueAExpTfp1 == 0)
        {
          NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << tfp1->GetId () << " is not running.");
          NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ0, false, "UE A already made the transition form " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
        }
      else if (m_ueAExpTfp1 == 1)
        {
          NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
          NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ0, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ0::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
        }
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, 2, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      m_ueAExpTfp1 += 1;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 2, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall3::UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypePrivateUpgradeCall3::UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp1, 0, "UE timer TFP1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, false, "UE A already made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ0, false, "UE A already made the transition form " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, 1, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), true, "UE A timer " << tfp1->GetId () << " is not running.");
      NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
      m_ueAQ1ToQ2 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ0::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp1, 1, "UE timer TFP1 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ0, false, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxReq, 2, "UE A never sent " << McpttCallMsgPrivateSetupReq::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
      NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 2, "UE A counter " << cfp1->GetId () << " is not two.");
      m_ueAQ2ToQ0 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall3::UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateSetupReq::GetTypeId ()))
    {
      if (m_ueATxReq == 0)
        {
          NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
          NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, false, "UE A already made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
        }
      else if (m_ueATxReq == 1)
        {
          NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 2, "UE A counter " << cfp1->GetId () << " is not two.");
          NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ2, true, "UE A never made the transition form " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
        }
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp1, 0, "UE timer TFP1 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ0, false, "UE A already made the transition form " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
      m_ueATxReq += 1;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateUpgradeCall3::UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypePrivateUpgradeCall3::UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
}

void
McpttCallTypePrivateUpgradeCall3::UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypePrivateUpgradeCall3::UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
}

McpttCallTypePrivateDowngradeCall1::McpttCallTypePrivateDowngradeCall1 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAExpTfp6 (false),
    m_ueAQ2ToQ1 (false),
    m_ueARxCancelAck (false),
    m_ueATxCancel (0),
    m_ueBQ2ToQ1 (false),
    m_ueBRxCancel (false),
    m_ueBTxCancelAck (false)
{ }

void
McpttCallTypePrivateDowngradeCall1::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (McpttCallMachinePrivate::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttTestCall> ueACall = ueAPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();
  Ptr<McpttTestCall> ueBCall = ueBPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();

  Ptr<McpttCallMachinePrivate> ueACallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachinePrivate> ueBCallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (1));

  Ptr<McpttTimer> ueATfp5 = ueACallMachine->GetTfp5 ();
  Ptr<McpttTimer> ueATfp8 = ueACallMachine->GetTfp8 ();
  Ptr<McpttTimer> ueBTfp5 = ueBCallMachine->GetTfp5 ();
  Ptr<McpttTimer> ueBTfp8 = ueBCallMachine->GetTfp8 ();

  Ptr<TestCallTypeMachinePrivate> ueATypeMachine = CreateObject<TestCallTypeMachinePrivate> ();
  Ptr<TestCallTypeMachinePrivate> ueBTypeMachine = CreateObject<TestCallTypeMachinePrivate> ();

  ueACallMachine->SetTypeMachine (ueATypeMachine);
  ueBCallMachine->SetTypeMachine (ueBTypeMachine);

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueACallMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBCallMachine->GetEmergMachine ();

  uint16_t callId = 1;
  Time startTime = Seconds (2.1);
  Time downgradeTime = Seconds (2.2);
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
  ueACallMachine->SetCallId (callId);
  ueACallMachine->SetCallerId (callerId);
  ueACallMachine->SetCalleeId (calleeId);
  ueACallMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueACallMachine->SetSdp (sdp);
  ueACallMachine->SetAttribute ("TFP6", TimeValue (MilliSeconds (240)));
  ueACallMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));

  ueAEmergMachine->SetEmerg (true);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
  ueATypeMachine->SetState (McpttCallTypeMachinePrivateStateQ2::GetInstance ());
  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateDowngradeCall1::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateDowngradeCall1::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateDowngradeCall1::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateDowngradeCall1::UeATimerExpCb, this));

  // UE B
  ueBCall->AddDropper (Create<McpttMsgDropperImpl> (2, McpttCallMsgPrivateEmergCancel::GetTypeId ()));

  ueBCallMachine->SetCallId (callId);
  ueBCallMachine->SetCallerId (callerId);
  ueBCallMachine->SetCalleeId (calleeId);
  ueBCallMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueBCallMachine->SetSdp (sdp);

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
  ueBTypeMachine->SetState (McpttCallTypeMachinePrivateStateQ2::GetInstance ());
  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateDowngradeCall1::UeBRxCb, this));
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateDowngradeCall1::UeBTxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateDowngradeCall1::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateDowngradeCall1::UeBTimerExpCb, this));

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfp5);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfp8);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfp5);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfp8);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (downgradeTime, &McpttPttApp::DowngradeCallType, ueAPttApp);
}

void
McpttCallTypePrivateDowngradeCall1::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp6, true, "UE A timer TFP6 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, true, "UE A never made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueARxCancelAck, true, "UE A never received " << McpttCallMsgPrivateEmergCancelAck::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxCancel, 2, "UE A never sent 2 " << McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBQ2ToQ1, true, "UE B never made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBRxCancel, true, "UE B never received " <<  McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBTxCancelAck, true, "UE B never sent " << McpttCallMsgPrivateEmergCancelAck::GetTypeId () << ".");

}

void
McpttCallTypePrivateDowngradeCall1::UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (timer.GetId () == tfp6->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp6, false, "UE A timer TFP6 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, true, "UE A never made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxCancelAck, false, "UE A already received " << McpttCallMsgPrivateEmergCancelAck::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxCancel, 2, "UE A never sent 2 " << McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");
      m_ueAExpTfp6 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), true, "UE A timer " << tfp6->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 2, "UE A counter " << cfp6->GetId () << " is not two.");
}

void
McpttCallTypePrivateDowngradeCall1::UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateEmergCancelAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp6, true, "UE A timer TFP6 never expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, true, "UE A never made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxCancelAck, false, "UE A already received " << McpttCallMsgPrivateEmergCancelAck::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxCancel, 2, "UE A never sent 2 " << McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");
      m_ueARxCancelAck = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 2, "UE A counter " << cfp6->GetId () << " is not two.");
}

void
McpttCallTypePrivateDowngradeCall1::UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp6, false, "UE A timer TFP6 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, false, "UE A already made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxCancelAck, false, "UE A already received " << McpttCallMsgPrivateEmergCancelAck::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxCancel, 1, "UE A never sent " << McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");
      m_ueAQ2ToQ1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), true, "UE A timer " << tfp6->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateDowngradeCall1::UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateEmergCancel::GetTypeId ()))
    {
      if (m_ueATxCancel == 0)
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, false, "UE A already made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), true, "UE A timer " << tfp8->GetId () << " is not running.");
        }
      else if (m_ueATxCancel == 1)
        {
          NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, true, "UE A never made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
          NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
        }
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp6, false, "UE A timer TFP6 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueARxCancelAck, false, "UE A already received " << McpttCallMsgPrivateEmergCancelAck::GetTypeId () << ".");
      m_ueATxCancel += 1;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateDowngradeCall1::UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypePrivateDowngradeCall1::UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateEmergCancel::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ2ToQ1, true, "UE B never made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxCancel, false, "UE B already received " <<  McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxCancelAck, true, "UE B never sent " << McpttCallMsgPrivateEmergCancelAck::GetTypeId () << ".");
      m_ueBRxCancel = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateDowngradeCall1::UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateEmergCancelAck::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ2ToQ1, false, "UE B already made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxCancel, false, "UE B already received " <<  McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxCancelAck, false, "UE B already sent " << McpttCallMsgPrivateEmergCancelAck::GetTypeId () << ".");
      m_ueBTxCancelAck = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ2::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), true, "UE B timer " << tfp8->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateDowngradeCall1::UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ2ToQ1, false, "UE B already made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBRxCancel, false, "UE B already received " <<  McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueBTxCancelAck, true, "UE B never sent " << McpttCallMsgPrivateEmergCancelAck::GetTypeId () << ".");
      m_ueBQ2ToQ1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

McpttCallTypePrivateDowngradeCall2::McpttCallTypePrivateDowngradeCall2 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAExpTfp6 (false),
    m_ueAQ1ToQ0 (false),
    m_ueAQ2ToQ1 (false),
    m_ueATxCancel (false)
{ }

void
McpttCallTypePrivateDowngradeCall2::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (McpttCallMachinePrivate::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttTestCall> ueACall = ueAPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();
  Ptr<McpttTestCall> ueBCall = ueBPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();

  Ptr<McpttCallMachinePrivate> ueACallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachinePrivate> ueBCallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (1));

  Ptr<McpttTimer> ueATfp5 = ueACallMachine->GetTfp5 ();
  Ptr<McpttTimer> ueATfp8 = ueACallMachine->GetTfp8 ();
  Ptr<McpttTimer> ueBTfp5 = ueBCallMachine->GetTfp5 ();
  Ptr<McpttTimer> ueBTfp8 = ueBCallMachine->GetTfp8 ();

  Ptr<TestCallTypeMachinePrivate> ueATypeMachine = CreateObject<TestCallTypeMachinePrivate> ();
  Ptr<TestCallTypeMachinePrivate> ueBTypeMachine = CreateObject<TestCallTypeMachinePrivate> ();

  ueACallMachine->SetTypeMachine (ueATypeMachine);
  ueBCallMachine->SetTypeMachine (ueBTypeMachine);

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueACallMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBCallMachine->GetEmergMachine ();

  uint16_t callId = 1;
  Time startTime = Seconds (2.1);
  Time downgradeTime = Seconds (2.2);
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
  ueACallMachine->SetCallId (callId);
  ueACallMachine->SetCallerId (callerId);
  ueACallMachine->SetCalleeId (calleeId);
  ueACallMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueACallMachine->SetSdp (sdp);
  ueACallMachine->SetAttribute ("TFP6", TimeValue (MilliSeconds (240)));
  ueACallMachine->SetAttribute ("CFP6", UintegerValue (1));
  ueACallMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));

  ueAEmergMachine->SetEmerg (true);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
  ueATypeMachine->SetState (McpttCallTypeMachinePrivateStateQ2::GetInstance ());
  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateDowngradeCall2::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateDowngradeCall2::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateDowngradeCall2::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateDowngradeCall2::UeATimerExpCb, this));

  // UE B
  ueBCall->AddDropper (Create<McpttMsgDropperImpl> (2, McpttCallMsgPrivateEmergCancel::GetTypeId ()));

  ueBCallMachine->SetCallId (callId);
  ueBCallMachine->SetCallerId (callerId);
  ueBCallMachine->SetCalleeId (calleeId);
  ueBCallMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueBCallMachine->SetSdp (sdp);

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
  ueBTypeMachine->SetState (McpttCallTypeMachinePrivateStateQ2::GetInstance ());
  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateDowngradeCall2::UeBRxCb, this));
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateDowngradeCall2::UeBTxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateDowngradeCall2::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateDowngradeCall2::UeBTimerExpCb, this));

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfp5);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfp8);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfp5);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfp8);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
  Simulator::Schedule (downgradeTime, &McpttPttApp::DowngradeCallType, ueAPttApp);
}

void
McpttCallTypePrivateDowngradeCall2::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp6, true, "UE A timer TFP6 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ0, true, "UE A never made transition from " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, true, "UE A never made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueATxCancel, true, "UE A never sent " << McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");

}

void
McpttCallTypePrivateDowngradeCall2::UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (timer.GetId () == tfp6->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp6, false, "UE A timer TFP6 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ0, true, "UE A never made transition from " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, true, "UE A never made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxCancel, true, "UE A never sent " << McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");
      m_ueAExpTfp6 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ0::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not two.");
}

void
McpttCallTypePrivateDowngradeCall2::UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypePrivateDowngradeCall2::UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp6, false, "UE A timer TFP6 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ0, false, "UE A already made transition from " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, false, "UE A already made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxCancel, true, "UE A never sent " << McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), true, "UE A timer " << tfp6->GetId () << " is not running.");
      m_ueAQ2ToQ1 = true;
    }
  else if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ()
           && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ0::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp6, false, "UE A timer TFP6 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ0, false, "UE A already made transition from " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, true, "UE A already made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxCancel, true, "UE A never sent " << McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");
      NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
      m_ueAQ1ToQ0 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateDowngradeCall2::UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (msg.IsA (McpttCallMsgPrivateEmergCancel::GetTypeId ()))
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp6, false, "UE A timer TFP6 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ1ToQ0, false, "UE A already made transition from " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ0::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, false, "UE A already made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      NS_TEST_ASSERT_MSG_EQ (m_ueATxCancel, false, "UE A already sent " << McpttCallMsgPrivateEmergCancel::GetTypeId () << ".");
      m_ueATxCancel = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ2::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), true, "UE A timer " << tfp8->GetId () << " is not running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateDowngradeCall2::UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " unexpectedly expired.");
}

void
McpttCallTypePrivateDowngradeCall2::UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
}

void
McpttCallTypePrivateDowngradeCall2::UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypePrivateDowngradeCall2::UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
}

McpttCallTypePrivateDowngradeCall3::McpttCallTypePrivateDowngradeCall3 (const std::string& name, Ptr<McpttTestCaseConfig>  config)
  : McpttTestCase (name, config),
    m_ueAExpTfp8 (false),
    m_ueAQ2ToQ1 (false),
    m_ueBExpTfp8 (false),
    m_ueBQ2ToQ1 (false)
{ }

void
McpttCallTypePrivateDowngradeCall3::Configure (void)
{
  GetConfig ()->SetAppCount (2);
  GetConfig ()->SetCallTid (McpttCallMachinePrivate::GetTypeId ());

  McpttTestCase::Configure ();

  Ptr<McpttPttApp> ueAPttApp = GetApp (0);
  Ptr<McpttPttApp> ueBPttApp = GetApp (1);

  Ptr<McpttTestCall> ueACall = ueAPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();
  Ptr<McpttTestCall> ueBCall = ueBPttApp->GetSelectedCall ()->GetObject<McpttTestCall> ();

  Ptr<McpttCallMachinePrivate> ueACallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (0));
  Ptr<McpttCallMachinePrivate> ueBCallMachine = DynamicCast<McpttCallMachinePrivate, McpttCallMachine> (GetCallMachine (1));

  Ptr<McpttTimer> ueATfp5 = ueACallMachine->GetTfp5 ();
  Ptr<McpttTimer> ueATfp8 = ueACallMachine->GetTfp8 ();
  Ptr<McpttTimer> ueBTfp5 = ueBCallMachine->GetTfp5 ();
  Ptr<McpttTimer> ueBTfp8 = ueBCallMachine->GetTfp8 ();

  Ptr<TestCallTypeMachinePrivate> ueATypeMachine = CreateObject<TestCallTypeMachinePrivate> ();
  Ptr<TestCallTypeMachinePrivate> ueBTypeMachine = CreateObject<TestCallTypeMachinePrivate> ();

  ueACallMachine->SetTypeMachine (ueATypeMachine);
  ueBCallMachine->SetTypeMachine (ueBTypeMachine);

  Ptr<McpttEmergAlertMachineBasic> ueAEmergMachine = ueACallMachine->GetEmergMachine ();
  Ptr<McpttEmergAlertMachineBasic> ueBEmergMachine = ueBCallMachine->GetEmergMachine ();

  uint16_t callId = 1;
  Time startTime = Seconds (2.1);
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
  ueACallMachine->SetCallId (callId);
  ueACallMachine->SetCallerId (callerId);
  ueACallMachine->SetCalleeId (calleeId);
  ueACallMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueACallMachine->SetSdp (sdp);
  ueACallMachine->SetAttribute ("TFP8", TimeValue (Seconds (1)));
  ueACallMachine->SetAttribute ("TargetId", UintegerValue (ueBPttApp->GetUserId ()));

  ueAEmergMachine->SetEmerg (true);
  ueAEmergMachine->SetStarted (true);

  ueATypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE);
  ueATypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
  ueATypeMachine->SetState (McpttCallTypeMachinePrivateStateQ2::GetInstance ());
  ueATypeMachine->SetStarted (true);
  ueATypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateDowngradeCall3::UeATxCb, this));
  ueATypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateDowngradeCall3::UeARxCb, this));
  ueATypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateDowngradeCall3::UeAStateChangeCb, this));
  ueATypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateDowngradeCall3::UeATimerExpCb, this));

  // UE B
  ueBCall->AddDropper (Create<McpttMsgDropperImpl> (2, McpttCallMsgPrivateEmergCancel::GetTypeId ()));

  ueBCallMachine->SetCallId (callId);
  ueBCallMachine->SetCallerId (callerId);
  ueBCallMachine->SetCalleeId (calleeId);
  ueBCallMachine->SetCommMode (McpttCallMsgFieldCommMode (commMode));
  ueBCallMachine->SetSdp (sdp);
  ueBCallMachine->SetAttribute ("TFP8", TimeValue (Seconds (1)));

  ueBEmergMachine->SetEmerg (false);
  ueBEmergMachine->SetStarted (true);

  ueBTypeMachine->SetCallType (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE);
  ueBTypeMachine->SetPriority (McpttCallMsgFieldCallType::GetCallTypePriority (McpttCallMsgFieldCallType::EMERGENCY_PRIVATE));
  ueBTypeMachine->SetState (McpttCallTypeMachinePrivateStateQ2::GetInstance ());
  ueBTypeMachine->SetStarted (true);
  ueBTypeMachine->SetPostRxCb (MakeCallback (&McpttCallTypePrivateDowngradeCall3::UeBRxCb, this));
  ueBTypeMachine->SetPostTxCb (MakeCallback (&McpttCallTypePrivateDowngradeCall3::UeBTxCb, this));
  ueBTypeMachine->SetStateChangeTestCb (MakeCallback (&McpttCallTypePrivateDowngradeCall3::UeBStateChangeCb, this));
  ueBTypeMachine->SetPostTimerExpCb (MakeCallback (&McpttCallTypePrivateDowngradeCall3::UeBTimerExpCb, this));

  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfp5);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueATfp8);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfp5);
  Simulator::Schedule (startTime, &McpttTimer::Start, ueBTfp8);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueACall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueACall, grpAddress.Get (), speechPort);
  Simulator::Schedule (startTime, &McpttCall::OpenFloorChannel, ueBCall, grpAddress.Get (), floorPort);
  Simulator::Schedule (startTime, &McpttCall::OpenMediaChannel, ueBCall, grpAddress.Get (), speechPort);
}

void
McpttCallTypePrivateDowngradeCall3::Execute (void)
{
  NS_LOG_LOGIC ("Starting " << GetName () << "...");

  Simulator::Run ();
  Simulator::Destroy ();

  NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp8, true, "UE A timer TFP8 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, true, "UE A never made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfp8, true, "UE B timer TFP8 never expired.");
  NS_TEST_ASSERT_MSG_EQ (m_ueBQ2ToQ1, true, "UE B never made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");

}

void
McpttCallTypePrivateDowngradeCall3::UeATimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (timer.GetId () == tfp8->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp8, false, "UE A timer TFP8 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, true, "UE A never made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      m_ueAExpTfp8 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE A timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE A in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateDowngradeCall3::UeARxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "Unexpected message recived " << msg << ".");
}

void
McpttCallTypePrivateDowngradeCall3::UeAStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueAExpTfp8, false, "UE A timer TFP8 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueAQ2ToQ1, false, "UE A already made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      m_ueAQ2ToQ1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE A state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE A timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE A timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE A timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE A counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE A counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateDowngradeCall3::UeATxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE A sent unexpected " << msg.GetInstanceTypeId () << " message.");
}

void
McpttCallTypePrivateDowngradeCall3::UeBTimerExpCb (const TestCallTypeMachinePrivate& machine, const McpttTimer& timer)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (timer.GetId () == tfp8->GetId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfp8, false, "UE B timer TFP8 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ2ToQ1, true, "UE B never made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      m_ueBExpTfp8 = true;
      Stop ();
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (false, true, "UE B timer " << timer << " expired.");
    }

  NS_TEST_ASSERT_MSG_EQ (state->GetInstanceStateId (), McpttCallTypeMachinePrivateStateQ1::GetStateId (), "UE B in state " << state->GetInstanceStateId () << " not " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

void
McpttCallTypePrivateDowngradeCall3::UeBRxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B received unexpected message: " << msg << ".");
}

void
McpttCallTypePrivateDowngradeCall3::UeBTxCb (const TestCallTypeMachinePrivate& machine, const McpttCallMsg& msg)
{
  NS_TEST_ASSERT_MSG_EQ (true, false, "UE B sent unexpected message: " << msg.GetTypeId () << ".");
}

void
McpttCallTypePrivateDowngradeCall3::UeBStateChangeCb (const TestCallTypeMachinePrivate& machine, Ptr<McpttCallTypeMachinePrivateState> oldState, Ptr<McpttCallTypeMachinePrivateState> newState)
{
  Ptr<McpttTimer> tfp1 = machine.GetOwner ()->GetTfp1 ();
  Ptr<McpttTimer> tfp6 = machine.GetOwner ()->GetTfp6 ();
  Ptr<McpttTimer> tfp8 = machine.GetOwner ()->GetTfp8 ();
  Ptr<McpttCounter> cfp1 = machine.GetOwner ()->GetCfp1 ();
  Ptr<McpttCounter> cfp6 = machine.GetOwner ()->GetCfp6 ();
  Ptr<McpttCallTypeMachinePrivateState> state = machine.GetState ();

  if (oldState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ2::GetStateId ()
      && newState->GetInstanceStateId () == McpttCallTypeMachinePrivateStateQ1::GetStateId ())
    {
      NS_TEST_ASSERT_MSG_EQ (m_ueBExpTfp8, false, "UE B timer TFP8 already expired.");
      NS_TEST_ASSERT_MSG_EQ (m_ueBQ2ToQ1, false, "UE B already made transition from " << McpttCallTypeMachinePrivateStateQ2::GetStateId () << " to " << McpttCallTypeMachinePrivateStateQ1::GetStateId () << ".");
      m_ueBQ2ToQ1 = true;
    }
  else
    {
      NS_TEST_ASSERT_MSG_EQ (true, false, "UE B state transition from " << oldState->GetInstanceStateId () << " to " << newState->GetInstanceStateId () << ".");
    }

  NS_TEST_ASSERT_MSG_EQ (tfp1->IsRunning (), false, "UE B timer " << tfp1->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp6->IsRunning (), false, "UE B timer " << tfp6->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (tfp8->IsRunning (), false, "UE B timer " << tfp8->GetId () << " is running.");
  NS_TEST_ASSERT_MSG_EQ (cfp1->GetValue (), 1, "UE B counter " << cfp1->GetId () << " is not one.");
  NS_TEST_ASSERT_MSG_EQ (cfp6->GetValue (), 1, "UE B counter " << cfp6->GetId () << " is not one.");
}

McpttCallTypePrivateControlTestSuite::McpttCallTypePrivateControlTestSuite (void)
  : TestSuite ("mcptt-call-type-control-private", TestSuite::UNIT)
{
  AddTestCase (new McpttCallTypePrivateEnterPrivateCall (), TestCase::QUICK);
  AddTestCase (new McpttCallTypePrivateEnterPrivateEmergencyCall (), TestCase::QUICK);
  AddTestCase (new McpttCallTypePrivateUpgradeCall1 (), TestCase::QUICK);
  AddTestCase (new McpttCallTypePrivateUpgradeCall2 (), TestCase::QUICK);
  AddTestCase (new McpttCallTypePrivateUpgradeCall3 (), TestCase::QUICK);
  AddTestCase (new McpttCallTypePrivateDowngradeCall1 (), TestCase::QUICK);
  AddTestCase (new McpttCallTypePrivateDowngradeCall2 (), TestCase::QUICK);
  AddTestCase (new McpttCallTypePrivateDowngradeCall3 (), TestCase::QUICK);
}

} // namespace tests
} // namespace psc
} // namespace ns3

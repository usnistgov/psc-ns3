/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#include "ns3/lte-sl-pc5-signalling-header.h"
#include "ns3/packet.h"
#include <string>
#include <ns3/log.h>
#include <ns3/test.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TestLteSlPc5SignallingHeader");


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief LTE SL PC5 Signalling Message Type test case.
 */
class LteSlPc5SignallingMessageTypeTestCase : public TestCase
{
public:
  static std::string BuildNameString (LteSlPc5SignallingMessageType nlpc5smt);
  LteSlPc5SignallingMessageTypeTestCase (LteSlPc5SignallingMessageType nlpc5smt);
private:
  virtual void DoRun (void);
  LteSlPc5SignallingMessageType m_nlpc5smt; ///< LteSlPc5SignallingMessageType
};

/**
   * Build name string function
   *
   * \param nlpc5smt LteSlPc5SignallingMessageType
   * \returns the name string
   */
std::string LteSlPc5SignallingMessageTypeTestCase::BuildNameString (LteSlPc5SignallingMessageType nlpc5smt)
{
  std::ostringstream oss;
  oss << "TestCase:LteSlPc5SignallingMessageTypeTestCase, " << nlpc5smt;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param nlpc5smt LteSlPc5SignallingMessageType
   */
LteSlPc5SignallingMessageTypeTestCase::LteSlPc5SignallingMessageTypeTestCase (LteSlPc5SignallingMessageType nlpc5smt)
  : TestCase (BuildNameString (nlpc5smt)),
  m_nlpc5smt (nlpc5smt)
{
}

void LteSlPc5SignallingMessageTypeTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_nlpc5smt));

  LteSlPc5SignallingMessageType nlpc5smt_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_nlpc5smt);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (nlpc5smt_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_nlpc5smt.GetMessageType () == nlpc5smt_peek.GetMessageType ()), true, "Input and Peeked msgType are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Communication Request test case.
 */
class DirectCommunicationRequestTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectCommunicationRequest ndcrq);
  DirectCommunicationRequestTestCase (DirectCommunicationRequest ndcrq);
private:
  virtual void DoRun (void);
  DirectCommunicationRequest m_ndcrq; ///< DirectCommunicationRequest
};

/**
   * Build name string function
   *
   * \param ndcrq DirectCommunicationRequest
   * \returns the name string
   */
std::string DirectCommunicationRequestTestCase::BuildNameString (DirectCommunicationRequest ndcrq)
{
  std::ostringstream oss;
  oss << "TestCase:DirectCommunicationRequestTestCase, " << ndcrq;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndcrq DirectCommunicationRequest
   */
DirectCommunicationRequestTestCase::DirectCommunicationRequestTestCase (DirectCommunicationRequest ndcrq)
  : TestCase (BuildNameString (ndcrq)),
  m_ndcrq (ndcrq)
{
}

void DirectCommunicationRequestTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndcrq));

  DirectCommunicationRequest ndcrq_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndcrq);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndcrq_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetMessageIdentifier () == ndcrq_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetSequenceNumber () == ndcrq_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetUserInfo () == ndcrq_peek.GetUserInfo ()), true, "Input and Peeked User Info are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetUserInfoType () == ndcrq_peek.GetUserInfoType ()), true, "Input and Peeked User Info Type are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetIpAddressConfig () == ndcrq_peek.GetIpAddressConfig ()), true, "Input and Peeked IP Address Config are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetMaximumInactivityPeriod () == ndcrq_peek.GetMaximumInactivityPeriod ()), true, "Input and Peeked Maximum Inactivity Period are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetNonce1 () == ndcrq_peek.GetNonce1 ()), true, "Input and Peeked Nonce1 are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetUeSecurityCapabilities () == ndcrq_peek.GetUeSecurityCapabilities ()), true, "Input and Peeked UE Security Capabilities are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetMsbKdId () == ndcrq_peek.GetMsbKdId ()), true, "Input and Peeked MSB Kd ID are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetKdId () == ndcrq_peek.GetKdId ()), true, "Input and Peeked Kd ID are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetRelayServiceCode () == ndcrq_peek.GetRelayServiceCode ()), true, "Input and Peeked Relay Service Code are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetSignature () == ndcrq_peek.GetSignature ()), true, "Input and Peeked Signature are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrq.GetLinkLocalIpv6Address () == ndcrq_peek.GetLinkLocalIpv6Address ()), true, "Input and Peeked Link Local IPv6 Address are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Communication Accept test case.
 */
class DirectCommunicationAcceptTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectCommunicationAccept ndca);
  DirectCommunicationAcceptTestCase (DirectCommunicationAccept ndca);
private:
  virtual void DoRun (void);
  DirectCommunicationAccept m_ndca; ///< DirectCommunicationAccept
};

/**
   * Build name string function
   *
   * \param ndca DirectCommunicationAccept
   * \returns the name string
   */
std::string DirectCommunicationAcceptTestCase::BuildNameString (DirectCommunicationAccept ndca)
{
  std::ostringstream oss;
  oss << "TestCase:DirectCommunicationAcceptTestCase, " << ndca;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndca DirectCommunicationAccept
   */
DirectCommunicationAcceptTestCase::DirectCommunicationAcceptTestCase (DirectCommunicationAccept ndca)
  : TestCase (BuildNameString (ndca)),
  m_ndca (ndca)
{
}

void DirectCommunicationAcceptTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndca));

  DirectCommunicationAccept ndca_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndca);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndca_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndca.GetMessageIdentifier () == ndca_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndca.GetSequenceNumber () == ndca_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndca.GetIpAddressConfig () == ndca_peek.GetIpAddressConfig ()), true, "Input and Peeked IP Address Config are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndca.GetLinkLocalIpv6Address () == ndca_peek.GetLinkLocalIpv6Address ()), true, "Input and Peeked Link Local IPv6 Address are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Communication Reject test case.
 */
class DirectCommunicationRejectTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectCommunicationReject ndcrj);
  DirectCommunicationRejectTestCase (DirectCommunicationReject ndcrj);
private:
  virtual void DoRun (void);
  DirectCommunicationReject m_ndcrj; ///< DirectCommunicationReject
};

/**
   * Build name string function
   *
   * \param ndcrj DirectCommunicationReject
   * \returns the name string
   */
std::string DirectCommunicationRejectTestCase::BuildNameString (DirectCommunicationReject ndcrj)
{
  std::ostringstream oss;
  oss << "TestCase:DirectCommunicationRejectTestCase, " << ndcrj;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndcrj DirectCommunicationReject
   */
DirectCommunicationRejectTestCase::DirectCommunicationRejectTestCase (DirectCommunicationReject ndcrj)
  : TestCase (BuildNameString (ndcrj)),
  m_ndcrj (ndcrj)
{
}

void DirectCommunicationRejectTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndcrj));

  DirectCommunicationReject ndcrj_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndcrj);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndcrj_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndcrj.GetMessageIdentifier () == ndcrj_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrj.GetSequenceNumber () == ndcrj_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcrj.GetPc5SignallingCauseValue () == ndcrj_peek.GetPc5SignallingCauseValue ()), true, "Input and Peeked PC5 Signalling Cause Value are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Communication Keepalive test case.
 */
class DirectCommunicationKeepaliveTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectCommunicationKeepalive ndck);
  DirectCommunicationKeepaliveTestCase (DirectCommunicationKeepalive ndck);
private:
  virtual void DoRun (void);
  DirectCommunicationKeepalive m_ndck; ///< DirectCommunicationKeepalive
};

/**
   * Build name string function
   *
   * \param ndck DirectCommunicationKeepalive
   * \returns the name string
   */
std::string DirectCommunicationKeepaliveTestCase::BuildNameString (DirectCommunicationKeepalive ndck)
{
  std::ostringstream oss;
  oss << "TestCase:DirectCommunicationKeepaliveTestCase, " << ndck;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndck DirectCommunicationKeepalive
   */
DirectCommunicationKeepaliveTestCase::DirectCommunicationKeepaliveTestCase (DirectCommunicationKeepalive ndck)
  : TestCase (BuildNameString (ndck)),
  m_ndck (ndck)
{
}

void DirectCommunicationKeepaliveTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndck));

  DirectCommunicationKeepalive ndck_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndck);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndck_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndck.GetMessageIdentifier () == ndck_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndck.GetSequenceNumber () == ndck_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndck.GetKeepaliveCounter () == ndck_peek.GetKeepaliveCounter ()), true, "Input and Peeked Keepalive Counter are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndck.GetMaximumInactivityPeriod () == ndck_peek.GetMaximumInactivityPeriod ()), true, "Input and Peeked Maximum Inactivity Period are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Communication Keepalive Ack test case.
 */
class DirectCommunicationKeepaliveAckTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectCommunicationKeepaliveAck ndcka);
  DirectCommunicationKeepaliveAckTestCase (DirectCommunicationKeepaliveAck ndcka);
private:
  virtual void DoRun (void);
  DirectCommunicationKeepaliveAck m_ndcka; ///< DirectCommunicationKeepaliveAck
};

/**
   * Build name string function
   *
   * \param ndcka DirectCommunicationKeepaliveAck
   * \returns the name string
   */
std::string DirectCommunicationKeepaliveAckTestCase::BuildNameString (DirectCommunicationKeepaliveAck ndcka)
{
  std::ostringstream oss;
  oss << "TestCase:DirectCommunicationKeepaliveAckTestCase, " << ndcka;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndcka DirectCommunicationKeepaliveAck
   */
DirectCommunicationKeepaliveAckTestCase::DirectCommunicationKeepaliveAckTestCase (DirectCommunicationKeepaliveAck ndcka)
  : TestCase (BuildNameString (ndcka)),
  m_ndcka (ndcka)
{
}

void DirectCommunicationKeepaliveAckTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndcka));

  DirectCommunicationKeepaliveAck ndcka_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndcka);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndcka_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndcka.GetMessageIdentifier () == ndcka_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcka.GetSequenceNumber () == ndcka_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcka.GetKeepaliveCounter () == ndcka_peek.GetKeepaliveCounter ()), true, "Input and Peeked Keepalive Counter are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Communication Release test case.
 */
class DirectCommunicationReleaseTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectCommunicationRelease ndcr);
  DirectCommunicationReleaseTestCase (DirectCommunicationRelease ndcr);
private:
  virtual void DoRun (void);
  DirectCommunicationRelease m_ndcr; ///< DirectCommunicationRelease
};

/**
   * Build name string function
   *
   * \param ndcr DirectCommunicationRelease
   * \returns the name string
   */
std::string DirectCommunicationReleaseTestCase::BuildNameString (DirectCommunicationRelease ndcr)
{
  std::ostringstream oss;
  oss << "TestCase:DirectCommunicationReleaseTestCase, " << ndcr;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndcr DirectCommunicationRelease
   */
DirectCommunicationReleaseTestCase::DirectCommunicationReleaseTestCase (DirectCommunicationRelease ndcr)
  : TestCase (BuildNameString (ndcr)),
  m_ndcr (ndcr)
{
}

void DirectCommunicationReleaseTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndcr));

  DirectCommunicationRelease ndcr_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndcr);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndcr_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndcr.GetMessageIdentifier () == ndcr_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcr.GetSequenceNumber () == ndcr_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcr.GetReleaseReason () == ndcr_peek.GetReleaseReason ()), true, "Input and Peeked Release Reason are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Communication Release Accept test case.
 */
class DirectCommunicationReleaseAcceptTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectCommunicationReleaseAccept ndcra);
  DirectCommunicationReleaseAcceptTestCase (DirectCommunicationReleaseAccept ndcra);
private:
  virtual void DoRun (void);
  DirectCommunicationReleaseAccept m_ndcra; ///< DirectCommunicationReleaseAccept
};

/**
   * Build name string function
   *
   * \param ndcra DirectCommunicationReleaseAccept
   * \returns the name string
   */
std::string DirectCommunicationReleaseAcceptTestCase::BuildNameString (DirectCommunicationReleaseAccept ndcra)
{
  std::ostringstream oss;
  oss << "TestCase:DirectCommunicationReleaseAcceptTestCase, " << ndcra;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndcra DirectCommunicationReleaseAccept
   */
DirectCommunicationReleaseAcceptTestCase::DirectCommunicationReleaseAcceptTestCase (DirectCommunicationReleaseAccept ndcra)
  : TestCase (BuildNameString (ndcra)),
  m_ndcra (ndcra)
{
}

void DirectCommunicationReleaseAcceptTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndcra));

  DirectCommunicationReleaseAccept ndcra_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndcra);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndcra_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndcra.GetMessageIdentifier () == ndcra_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndcra.GetSequenceNumber () == ndcra_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Security Mode Command test case.
 */
class DirectSecurityModeCommandTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectSecurityModeCommand ndsmcm);
  DirectSecurityModeCommandTestCase (DirectSecurityModeCommand ndsmcm);
private:
  virtual void DoRun (void);
  DirectSecurityModeCommand m_ndsmcm; ///< DirectSecurityModeCommand
};

/**
   * Build name string function
   *
   * \param ndsmcm DirectSecurityModeCommand
   * \returns the name string
   */
std::string DirectSecurityModeCommandTestCase::BuildNameString (DirectSecurityModeCommand ndsmcm)
{
  std::ostringstream oss;
  oss << "TestCase:DirectSecurityModeCommandTestCase, " << ndsmcm;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndsmcm DirectSecurityModeCommand
   */
DirectSecurityModeCommandTestCase::DirectSecurityModeCommandTestCase (DirectSecurityModeCommand ndsmcm)
  : TestCase (BuildNameString (ndsmcm)),
  m_ndsmcm (ndsmcm)
{
}

void DirectSecurityModeCommandTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndsmcm));

  DirectSecurityModeCommand ndsmcm_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndsmcm);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndsmcm_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetMessageIdentifier () == ndsmcm_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetSequenceNumber () == ndsmcm_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetUeSecurityCapabilities () == ndsmcm_peek.GetUeSecurityCapabilities ()), true, "Input and Peeked UE Security Capabilities are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetNonce2 () == ndsmcm_peek.GetNonce2 ()), true, "Input and Peeked Nonce2 are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetChosenAlgorithmsIntegrity () == ndsmcm_peek.GetChosenAlgorithmsIntegrity ()), true, "Input and Peeked Chosen Algorithms Integrity are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetChosenAlgorithmsCyphering () == ndsmcm_peek.GetChosenAlgorithmsCyphering ()), true, "Input and Peeked Chosen Algorithms Cyphering are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetLsb () == ndsmcm_peek.GetLsb ()), true, "Input and Peeked LSB are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetMsb () == ndsmcm_peek.GetMsb ()), true, "Input and Peeked MSB are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetKdFreshness () == ndsmcm_peek.GetKdFreshness ()), true, "Input and Peeked Kd Freshness are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetGpi () == ndsmcm_peek.GetGpi ()), true, "Input and Peeked GPI are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetUserInfo () == ndsmcm_peek.GetUserInfo ()), true, "Input and Peeked User Info are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetUserInfoType () == ndsmcm_peek.GetUserInfoType ()), true, "Input and Peeked User Info Type are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetSignature () == ndsmcm_peek.GetSignature ()), true, "Input and Peeked Signature are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcm.GetEncryptedPayload () == ndsmcm_peek.GetEncryptedPayload ()), true, "Input and Peeked Encrypted Payload are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Security Mode Complete test case.
 */
class DirectSecurityModeCompleteTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectSecurityModeComplete ndsmcp);
  DirectSecurityModeCompleteTestCase (DirectSecurityModeComplete ndsmcp);
private:
  virtual void DoRun (void);
  DirectSecurityModeComplete m_ndsmcp; ///< DirectSecurityModeComplete
};

/**
   * Build name string function
   *
   * \param ndsmcp DirectSecurityModeComplete
   * \returns the name string
   */
std::string DirectSecurityModeCompleteTestCase::BuildNameString (DirectSecurityModeComplete ndsmcp)
{
  std::ostringstream oss;
  oss << "TestCase:DirectSecurityModeCompleteTestCase, " << ndsmcp;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndsmcp DirectSecurityModeComplete
   */
DirectSecurityModeCompleteTestCase::DirectSecurityModeCompleteTestCase (DirectSecurityModeComplete ndsmcp)
  : TestCase (BuildNameString (ndsmcp)),
  m_ndsmcp (ndsmcp)
{
}

void DirectSecurityModeCompleteTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndsmcp));

  DirectSecurityModeComplete ndsmcp_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndsmcp);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndsmcp_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcp.GetMessageIdentifier () == ndsmcp_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcp.GetSequenceNumber () == ndsmcp_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmcp.GetLsb () == ndsmcp_peek.GetLsb ()), true, "Input and Peeked LSB are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Security Mode Reject test case.
 */
class DirectSecurityModeRejectTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectSecurityModeReject ndsmrj);
  DirectSecurityModeRejectTestCase (DirectSecurityModeReject ndsmrj);
private:
  virtual void DoRun (void);
  DirectSecurityModeReject m_ndsmrj; ///< DirectSecurityModeReject
};

/**
   * Build name string function
   *
   * \param ndsmrj DirectSecurityModeReject
   * \returns the name string
   */
std::string DirectSecurityModeRejectTestCase::BuildNameString (DirectSecurityModeReject ndsmrj)
{
  std::ostringstream oss;
  oss << "TestCase:DirectSecurityModeRejectTestCase, " << ndsmrj;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndsmrj DirectSecurityModeReject
   */
DirectSecurityModeRejectTestCase::DirectSecurityModeRejectTestCase (DirectSecurityModeReject ndsmrj)
  : TestCase (BuildNameString (ndsmrj)),
  m_ndsmrj (ndsmrj)
{
}

void DirectSecurityModeRejectTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndsmrj));

  DirectSecurityModeReject ndsmrj_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndsmrj);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndsmrj_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndsmrj.GetMessageIdentifier () == ndsmrj_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmrj.GetSequenceNumber () == ndsmrj_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmrj.GetPc5SignallingCauseValue () == ndsmrj_peek.GetPc5SignallingCauseValue ()), true, "Input and Peeked PC5 Signalling Cause Value are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmrj.GetRand () == ndsmrj_peek.GetRand ()), true, "Input and Peeked RAND are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndsmrj.GetAuts () == ndsmrj_peek.GetAuts ()), true, "Input and Peeked AUTS are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Rekeying Request test case.
 */
class DirectRekeyingRequestTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectRekeyingRequest ndrrq);
  DirectRekeyingRequestTestCase (DirectRekeyingRequest ndrrq);
private:
  virtual void DoRun (void);
  DirectRekeyingRequest m_ndrrq; ///< DirectRekeyingRequest
};

/**
   * Build name string function
   *
   * \param ndrrq DirectRekeyingRequest
   * \returns the name string
   */
std::string DirectRekeyingRequestTestCase::BuildNameString (DirectRekeyingRequest ndrrq)
{
  std::ostringstream oss;
  oss << "TestCase:DirectRekeyingRequestTestCase, " << ndrrq;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndrrq DirectRekeyingRequest
   */
DirectRekeyingRequestTestCase::DirectRekeyingRequestTestCase (DirectRekeyingRequest ndrrq)
  : TestCase (BuildNameString (ndrrq)),
  m_ndrrq (ndrrq)
{
}

void DirectRekeyingRequestTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndrrq));

  DirectRekeyingRequest ndrrq_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndrrq);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndrrq_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndrrq.GetMessageIdentifier () == ndrrq_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndrrq.GetSequenceNumber () == ndrrq_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndrrq.GetUeSecurityCapabilities () == ndrrq_peek.GetUeSecurityCapabilities ()), true, "Input and Peeked UE Security Capabilities are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndrrq.GetNonce1 () == ndrrq_peek.GetNonce1 ()), true, "Input and Peeked Nonce1 are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndrrq.GetMsb () == ndrrq_peek.GetMsb ()), true, "Input and Peeked MSB are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndrrq.GetAuthFlag () == ndrrq_peek.GetAuthFlag ()), true, "Input and Peeked Auth Flag are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndrrq.GetPrukId () == ndrrq_peek.GetPrukId ()), true, "Input and Peeked PRUK ID are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Rekeying Response test case.
 */
class DirectRekeyingResponseTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectRekeyingResponse ndrrs);
  DirectRekeyingResponseTestCase (DirectRekeyingResponse ndrrs);
private:
  virtual void DoRun (void);
  DirectRekeyingResponse m_ndrrs; ///< DirectRekeyingResponse
};

/**
   * Build name string function
   *
   * \param ndrrs DirectRekeyingResponse
   * \returns the name string
   */
std::string DirectRekeyingResponseTestCase::BuildNameString (DirectRekeyingResponse ndrrs)
{
  std::ostringstream oss;
  oss << "TestCase:DirectRekeyingResponseTestCase, " << ndrrs;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndrrs DirectRekeyingResponse
   */
DirectRekeyingResponseTestCase::DirectRekeyingResponseTestCase (DirectRekeyingResponse ndrrs)
  : TestCase (BuildNameString (ndrrs)),
  m_ndrrs (ndrrs)
{
}

void DirectRekeyingResponseTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndrrs));

  DirectRekeyingResponse ndrrs_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndrrs);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndrrs_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndrrs.GetMessageIdentifier () == ndrrs_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndrrs.GetSequenceNumber () == ndrrs_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Direct Rekeying Trigger test case.
 */
class DirectRekeyingTriggerTestCase : public TestCase
{
public:
  static std::string BuildNameString (DirectRekeyingTrigger ndrt);
  DirectRekeyingTriggerTestCase (DirectRekeyingTrigger ndrt);
private:
  virtual void DoRun (void);
  DirectRekeyingTrigger m_ndrt; ///< DirectRekeyingTrigger
};

/**
   * Build name string function
   *
   * \param ndrt DirectRekeyingTrigger
   * \returns the name string
   */
std::string DirectRekeyingTriggerTestCase::BuildNameString (DirectRekeyingTrigger ndrt)
{
  std::ostringstream oss;
  oss << "TestCase:DirectRekeyingTriggerTestCase, " << ndrt;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param ndrt DirectRekeyingTrigger
   */
DirectRekeyingTriggerTestCase::DirectRekeyingTriggerTestCase (DirectRekeyingTrigger ndrt)
  : TestCase (BuildNameString (ndrt)),
  m_ndrt (ndrt)
{
}

void DirectRekeyingTriggerTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ndrt));

  DirectRekeyingTrigger ndrt_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_ndrt);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (ndrt_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_ndrt.GetMessageIdentifier () == ndrt_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_ndrt.GetSequenceNumber () == ndrt_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Remote UE Info Request test case.
 */
class RemoteUeInfoRequestTestCase : public TestCase
{
public:
  static std::string BuildNameString (RemoteUeInfoRequest nruirq);
  RemoteUeInfoRequestTestCase (RemoteUeInfoRequest nruirq);
private:
  virtual void DoRun (void);
  RemoteUeInfoRequest m_nruirq; ///< RemoteUeInfoRequest
};

/**
   * Build name string function
   *
   * \param nruirq RemoteUeInfoRequest
   * \returns the name string
   */
std::string RemoteUeInfoRequestTestCase::BuildNameString (RemoteUeInfoRequest nruirq)
{
  std::ostringstream oss;
  oss << "TestCase:RemoteUeInfoRequestTestCase, " << nruirq;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param nruirq RemoteUeInfoRequest
   */
RemoteUeInfoRequestTestCase::RemoteUeInfoRequestTestCase (RemoteUeInfoRequest nruirq)
  : TestCase (BuildNameString (nruirq)),
  m_nruirq (nruirq)
{
}

void RemoteUeInfoRequestTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_nruirq));

  RemoteUeInfoRequest nruirq_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_nruirq);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (nruirq_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_nruirq.GetMessageIdentifier () == nruirq_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_nruirq.GetSequenceNumber () == nruirq_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_nruirq.GetRemoteUeInfoType () == nruirq_peek.GetRemoteUeInfoType ()), true, "Input and Peeked Remote UE Info Type are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Remote UE Info Response test case.
 */
class RemoteUeInfoResponseTestCase : public TestCase
{
public:
  static std::string BuildNameString (RemoteUeInfoResponse nruirs);
  RemoteUeInfoResponseTestCase (RemoteUeInfoResponse nruirs);
private:
  virtual void DoRun (void);
  RemoteUeInfoResponse m_nruirs; ///< RemoteUeInfoResponse
};

/**
   * Build name string function
   *
   * \param nruirs RemoteUeInfoResponse
   * \returns the name string
   */
std::string RemoteUeInfoResponseTestCase::BuildNameString (RemoteUeInfoResponse nruirs)
{
  std::ostringstream oss;
  oss << "TestCase:RemoteUeInfoResponseTestCase, " << nruirs;
  return oss.str ();
}

/**
   * Constructor
   *
   * \param nruirs RemoteUeInfoResponse
   */
RemoteUeInfoResponseTestCase::RemoteUeInfoResponseTestCase (RemoteUeInfoResponse nruirs)
  : TestCase (BuildNameString (nruirs)),
  m_nruirs (nruirs)
{
}

void RemoteUeInfoResponseTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_nruirs));

  RemoteUeInfoResponse nruirs_peek;

  Ptr<Packet> p = Create<Packet>();
  p->AddHeader (m_nruirs);

  Ptr<Packet> p_peek = Create<Packet>();
  p_peek = p;
  p_peek->PeekHeader (nruirs_peek);

  NS_TEST_EXPECT_MSG_EQ ((m_nruirs.GetMessageIdentifier () == nruirs_peek.GetMessageIdentifier ()), true, "Input and Peeked Message Identifier are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_nruirs.GetSequenceNumber () == nruirs_peek.GetSequenceNumber ()), true, "Input and Peeked Sequence Number are not equal");
  NS_TEST_EXPECT_MSG_EQ ((m_nruirs.GetImei () == nruirs_peek.GetImei ()), true, "Input and Peeked IMEI are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief LTE PC5 Signalling Header test suite.
 */
class LtePc5SignallingHeaderTestSuite : public TestSuite
{
public:
  LtePc5SignallingHeaderTestSuite ();
};

LtePc5SignallingHeaderTestSuite::LtePc5SignallingHeaderTestSuite ()
  : TestSuite ("lte-pc5-signalling-header", UNIT)
{
  uint64_t userInfo = 0x12345678;
  std::vector<uint8_t> nonce1;
  nonce1.assign (4,2);
  std::vector<uint8_t> linkLocalIpv6Address;
  linkLocalIpv6Address.assign (4,3);
  std::vector<uint8_t> signature;
  signature.assign (4,4);
  std::vector<uint8_t> rand;
  rand.assign (4,5);
  std::vector<uint8_t> auts;
  auts.assign (4,6);
  std::vector<uint8_t> prukId;
  prukId.assign (4,7);
  //std::vector<uint8_t> imeidigits; imeidigits.assign(15,8);
  std::vector<uint8_t> nonce2;
  nonce2.assign (4,9);
  std::vector<uint8_t> kdFreshness;
  kdFreshness.assign (4,10);
  std::vector<uint8_t> gpi;
  gpi.assign (4,11);
  std::vector<uint8_t> encryptedPayload;
  encryptedPayload.assign (4,12);
  uint32_t kdId = 13, relayServiceCode = 14, keepaliveCounter = 15, maxInactivityPeriod = 16;
  uint8_t ipAddressConfig = 0, pc5SignallingCauseValue = 4, releaseReason = 3, authFlag = 1, remoteUeInfoType = 2;
  uint16_t lsb = 0, msb = 0;
  uint64_t imei = 0x12345678;

  //Test cases for LteSlPc5SignallingMessageType class
  LteSlPc5SignallingMessageType nlpc5smt;

  AddTestCase (new LteSlPc5SignallingMessageTypeTestCase (nlpc5smt),TestCase::QUICK);     //Default values

  nlpc5smt.SetMessageType (1);
  AddTestCase (new LteSlPc5SignallingMessageTypeTestCase (nlpc5smt),TestCase::QUICK);     //Set MessageType


  //Test cases for DirectCommunicationRequest class
  DirectCommunicationRequest ndcrq, ndcrq1, ndcrq2, ndcrq3, ndcrq4, ndcrq5, ndcrq6;

  AddTestCase (new DirectCommunicationRequestTestCase (ndcrq),TestCase::QUICK);     //Default values

  ndcrq1.SetUserInfo (3,userInfo);
  AddTestCase (new DirectCommunicationRequestTestCase (ndcrq1),TestCase::QUICK);     //userInfo set

  ndcrq2.SetNonce1 (nonce1);
  AddTestCase (new DirectCommunicationRequestTestCase (ndcrq2),TestCase::QUICK);     //nonce1 set

  ndcrq3.SetLinkLocalIpv6Address (linkLocalIpv6Address);
  AddTestCase (new DirectCommunicationRequestTestCase (ndcrq3),TestCase::QUICK);     //linkLocalIpv6Address set

  ndcrq4.SetSignature (signature);
  AddTestCase (new DirectCommunicationRequestTestCase (ndcrq4),TestCase::QUICK);     //signature set

  ndcrq5.SetKdId (kdId);
  ndcrq5.SetRelayServiceCode (relayServiceCode);
  ndcrq5.SetSignature (signature);
  ndcrq5.SetLinkLocalIpv6Address (linkLocalIpv6Address);
  AddTestCase (new DirectCommunicationRequestTestCase (ndcrq5),TestCase::QUICK);     //kdId, relayServiceCode, signature, linkLocalIpv6Address set

  ndcrq6.SetUserInfo (3,userInfo);
  ndcrq6.SetNonce1 (nonce1);
  ndcrq6.SetKdId (kdId);
  ndcrq6.SetRelayServiceCode (relayServiceCode);
  ndcrq6.SetSignature (signature);
  ndcrq6.SetLinkLocalIpv6Address (linkLocalIpv6Address);
  AddTestCase (new DirectCommunicationRequestTestCase (ndcrq6),TestCase::QUICK);     //userinfo, nonce1, kdId, relayServiceCode, signature, linkLocalIpv6Address set


  //Test cases for DirectCommunicationAccept class
  DirectCommunicationAccept ndca, ndca1, ndca2, ndca3;

  AddTestCase (new DirectCommunicationAcceptTestCase (ndca),TestCase::QUICK);     //Default values


  ndca1.SetIpAddressConfig (ipAddressConfig);
  AddTestCase (new DirectCommunicationAcceptTestCase (ndca1),TestCase::QUICK);     //ipAddressConfig set

  ndca2.SetLinkLocalIpv6Address (linkLocalIpv6Address);
  AddTestCase (new DirectCommunicationAcceptTestCase (ndca2),TestCase::QUICK);     //linkLocalIpv6Address set

  ndca3.SetIpAddressConfig (ipAddressConfig);
  ndca3.SetLinkLocalIpv6Address (linkLocalIpv6Address);
  AddTestCase (new DirectCommunicationAcceptTestCase (ndca3),TestCase::QUICK);     //ipAddressConfig, linkLocalIpv6Address set


  //Test cases for DirectCommunicationReject class
  DirectCommunicationReject ndcrj, ndcrj1;

  AddTestCase (new DirectCommunicationRejectTestCase (ndcrj),TestCase::QUICK);     //Default values

  ndcrj1.SetPc5SignallingCauseValue (pc5SignallingCauseValue);
  AddTestCase (new DirectCommunicationRejectTestCase (ndcrj1),TestCase::QUICK);     //pc5SignallingCauseValue set


  //Test cases for DirectCommunicationKeepalive class
  DirectCommunicationKeepalive ndck, ndck1, ndck2, ndck3;

  AddTestCase (new DirectCommunicationKeepaliveTestCase (ndck),TestCase::QUICK);     //Default values

  ndck1.SetKeepaliveCounter (keepaliveCounter);
  AddTestCase (new DirectCommunicationKeepaliveTestCase (ndck1),TestCase::QUICK);     //keepaliveCounter set

  ndck2.SetMaximumInactivityPeriod (maxInactivityPeriod);
  AddTestCase (new DirectCommunicationKeepaliveTestCase (ndck2),TestCase::QUICK);     //maxInactivityPeriod set

  ndck3.SetKeepaliveCounter (keepaliveCounter);
  ndck3.SetMaximumInactivityPeriod (maxInactivityPeriod);
  AddTestCase (new DirectCommunicationKeepaliveTestCase (ndck3),TestCase::QUICK);     //keepaliveCounter, maxInactivityPeriod set


  //Test cases for DirectCommunicationKeepaliveAck class
  DirectCommunicationKeepaliveAck ndcka, ndcka1;

  AddTestCase (new DirectCommunicationKeepaliveAckTestCase (ndcka),TestCase::QUICK);     //Default values

  ndcka1.SetKeepaliveCounter (keepaliveCounter);
  AddTestCase (new DirectCommunicationKeepaliveAckTestCase (ndcka1),TestCase::QUICK);     //keepaliveCounter set


  //Test cases for DirectCommunicationRelease class
  DirectCommunicationRelease ndcr, ndcr1;

  AddTestCase (new DirectCommunicationReleaseTestCase (ndcr),TestCase::QUICK);     //Default values

  ndcr1.SetReleaseReason (releaseReason);
  AddTestCase (new DirectCommunicationReleaseTestCase (ndcr1),TestCase::QUICK);     //releaseReason set


  //Test cases for DirectCommunicationReleaseAccept class
  DirectCommunicationReleaseAccept ndcra;

  AddTestCase (new DirectCommunicationReleaseAcceptTestCase (ndcra),TestCase::QUICK);     //Default values


  //Test cases for DirectSecurityModeCommand class
  DirectSecurityModeCommand ndsmcm, ndsmcm1, ndsmcm2, ndsmcm3, ndsmcm4, ndsmcm5, ndsmcm6, ndsmcm7, ndsmcm8;

  AddTestCase (new DirectSecurityModeCommandTestCase (ndsmcm),TestCase::QUICK);     //Default values

  ndsmcm1.SetNonce2 (nonce2);
  AddTestCase (new DirectSecurityModeCommandTestCase (ndsmcm1),TestCase::QUICK);     //nonce2 set

  ndsmcm2.SetMsb (msb);
  AddTestCase (new DirectSecurityModeCommandTestCase (ndsmcm2),TestCase::QUICK);     //msb set

  ndsmcm3.SetKdFreshness (kdFreshness);
  AddTestCase (new DirectSecurityModeCommandTestCase (ndsmcm3),TestCase::QUICK);     //kdFreshness set

  ndsmcm4.SetGpi (gpi);
  AddTestCase (new DirectSecurityModeCommandTestCase (ndsmcm4),TestCase::QUICK);     //gpi set

  ndsmcm5.SetUserInfo (3,userInfo);
  AddTestCase (new DirectSecurityModeCommandTestCase (ndsmcm5),TestCase::QUICK);     //userInfo set

  ndsmcm6.SetSignature (signature);
  AddTestCase (new DirectSecurityModeCommandTestCase (ndsmcm6),TestCase::QUICK);     //signature set

  ndsmcm7.SetEncryptedPayload (encryptedPayload);
  AddTestCase (new DirectSecurityModeCommandTestCase (ndsmcm7),TestCase::QUICK);     //encryptedPayload set

  ndsmcm8.SetNonce2 (nonce2);
  ndsmcm8.SetMsb (msb);
  ndsmcm8.SetKdFreshness (kdFreshness);
  ndsmcm8.SetGpi (gpi);
  ndsmcm8.SetUserInfo (3,userInfo);
  ndsmcm8.SetSignature (signature);
  ndsmcm8.SetEncryptedPayload (encryptedPayload);
  AddTestCase (new DirectSecurityModeCommandTestCase (ndsmcm8),TestCase::QUICK);     //encryptedPayload set


  //Test cases for DirectSecurityModeComplete class
  DirectSecurityModeComplete ndsmcp, ndsmcp1;

  AddTestCase (new DirectSecurityModeCompleteTestCase (ndsmcp),TestCase::QUICK);     //Default values

  ndsmcp1.SetLsb (lsb);
  AddTestCase (new DirectSecurityModeCompleteTestCase (ndsmcp1),TestCase::QUICK);     //lsb set


  //Test cases for DirectSecurityModeReject class
  DirectSecurityModeReject ndsmrj, ndsmrj1, ndsmrj2, ndsmrj3, ndsmrj4;

  AddTestCase (new DirectSecurityModeRejectTestCase (ndsmrj),TestCase::QUICK);     //Default values

  ndsmrj1.SetPc5SignallingCauseValue (pc5SignallingCauseValue);
  AddTestCase (new DirectSecurityModeRejectTestCase (ndsmrj1),TestCase::QUICK);     //pc5SignallingCauseValue set

  ndsmrj2.SetRand (rand);
  AddTestCase (new DirectSecurityModeRejectTestCase (ndsmrj2),TestCase::QUICK);     //rand set

  ndsmrj3.SetAuts (auts);
  AddTestCase (new DirectSecurityModeRejectTestCase (ndsmrj3),TestCase::QUICK);     //auts set

  ndsmrj4.SetPc5SignallingCauseValue (pc5SignallingCauseValue);
  ndsmrj4.SetRand (rand);
  ndsmrj4.SetAuts (auts);
  AddTestCase (new DirectSecurityModeRejectTestCase (ndsmrj4),TestCase::QUICK);     //pc5SignallingCauseValue, rand, auts set


  //Test cases for DirectRekeyingRequest class
  DirectRekeyingRequest ndrrq, ndrrq1, ndrrq2, ndrrq3, ndrrq4;

  AddTestCase (new DirectRekeyingRequestTestCase (ndrrq),TestCase::QUICK);     //Default values

  ndrrq1.SetNonce1 (nonce1);
  AddTestCase (new DirectRekeyingRequestTestCase (ndrrq1),TestCase::QUICK);     //nonce1 set

  ndrrq2.SetAuthFlag (authFlag);
  AddTestCase (new DirectRekeyingRequestTestCase (ndrrq2),TestCase::QUICK);     //authFlag set

  ndrrq3.SetPrukId (prukId);
  AddTestCase (new DirectRekeyingRequestTestCase (ndrrq3),TestCase::QUICK);     //prukId set

  ndrrq4.SetNonce1 (nonce1);
  ndrrq4.SetAuthFlag (authFlag);
  ndrrq4.SetPrukId (prukId);
  AddTestCase (new DirectRekeyingRequestTestCase (ndrrq4),TestCase::QUICK);     //nonce1, authFlag, prukId set


  //Test cases for DirectRekeyingResponse class
  DirectRekeyingResponse ndrrs;

  AddTestCase (new DirectRekeyingResponseTestCase (ndrrs),TestCase::QUICK);     //Default values


  //Test cases for DirectRekeyingTrigger class
  DirectRekeyingTrigger ndrt;

  AddTestCase (new DirectRekeyingTriggerTestCase (ndrt),TestCase::QUICK);     //Default values


  //Test cases for RemoteUeInfoRequest class
  RemoteUeInfoRequest nruirq, nruirq1;

  AddTestCase (new RemoteUeInfoRequestTestCase (nruirq),TestCase::QUICK);     //Default values

  nruirq1.SetRemoteUeInfoType (remoteUeInfoType);
  AddTestCase (new RemoteUeInfoRequestTestCase (nruirq1),TestCase::QUICK);     //remoteUeInfoType set


  //Test cases for RemoteUeInfoResponse class
  RemoteUeInfoResponse nruirs, nruirs1;

  AddTestCase (new RemoteUeInfoResponseTestCase (nruirs),TestCase::QUICK);     //Default values

  nruirs1.SetImei (imei);
  AddTestCase (new RemoteUeInfoResponseTestCase (nruirs1),TestCase::QUICK);     //imeidigits set
}

static LtePc5SignallingHeaderTestSuite staticLtePc5SignallingHeaderTestSuite;

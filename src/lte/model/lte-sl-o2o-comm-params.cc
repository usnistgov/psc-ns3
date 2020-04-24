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


#include "lte-sl-o2o-comm-params.h"

#include <ns3/log.h>
#include <ns3/object-map.h>
#include <ns3/object-factory.h>
#include <ns3/simulator.h>
#include <ns3/nstime.h>
#include "ns3/timer.h"
#include <ns3/uinteger.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlO2oCommParams");

NS_OBJECT_ENSURE_REGISTERED (LteSlO2oCommParams);

LteSlO2oCommParams::LteSlO2oCommParams ()
{
  NS_LOG_FUNCTION (this);

  m_relay_dT4111 = 0;
  m_relay_dT4103 = 0;
  m_relay_dT4108 = 0;
  m_relay_dTRUIR = 0;

  m_relay_T4111 = new Timer ();
  m_relay_T4103 = new Timer ();
  m_relay_T4108 = new Timer ();
  m_relay_TRUIR = new Timer ();

  m_remote_dT4100 = 0;
  m_remote_dT4101 = 0;
  m_remote_dT4102 = 0;
  m_remote_dT4103 = 0;

  m_remote_T4100 = new Timer ();
  m_remote_T4101 = new Timer ();
  m_remote_T4102 = new Timer ();
  m_remote_T4103 = new Timer ();

  m_contextId.contextId = 0;
  m_contextId.peerL2Id = 0;

  m_relay_DCR_count = 0;
  m_relay_RUIR_count = 0;
  m_remote_DCR_count = 0;
  m_remote_DCRq_count = 0;
  m_remote_DCK_count = 0;
  m_remote_ka_count = 0;

  m_stateString.push_back ("RELAY_IDLE");
  m_stateString.push_back ("RELAY_SETUP_REQUEST");
  m_stateString.push_back ("RELAY_SECURE_ESTABLISHED");
  m_stateString.push_back ("RELAY_INIT_RELEASE");
  m_stateString.push_back ("REMOTE_IDLE");
  m_stateString.push_back ("REMOTE_INIT_SETUP");
  m_stateString.push_back ("REMOTE_SECURE_ESTABLISHED");
  m_stateString.push_back ("REMOTE_INIT_RELEASE");

  m_security_mode_id = 0;
}

LteSlO2oCommParams::~LteSlO2oCommParams ()
{
  NS_LOG_FUNCTION (this);
}

void
LteSlO2oCommParams::DoDispose ()
{
  NS_LOG_FUNCTION (this);

  ClearTimers ();
  delete m_relay_T4111;
  delete m_relay_T4103;
  delete m_relay_T4108;
  delete m_relay_TRUIR;
  delete m_remote_T4100;
  delete m_remote_T4101;
  delete m_remote_T4102;
  delete m_remote_T4103;
}

void
LteSlO2oCommParams::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
LteSlO2oCommParams::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LteSlO2oCommParams")
    .SetParent<Object> ()
    .AddConstructor<LteSlO2oCommParams> ()
    .AddAttribute ("RelayT4111",
                   "Duration of Timer 4111 (Wait for Direct Security Mode Complete) for Relay UE in milliseconds",
                   UintegerValue (200),
                   MakeUintegerAccessor (&LteSlO2oCommParams::SetRelayT4111Duration),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RelayT4103",
                   "Duration of Timer 4103 (Wait for Direct Communication Release Accept) for Relay UE in milliseconds",
                   UintegerValue (100),
                   MakeUintegerAccessor (&LteSlO2oCommParams::SetRelayT4103Duration),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RelayT4108",
                   "Duration of Timer 4108 (Wait for Direct Communication Keepalive or PC5 Data) for Relay UE in milliseconds",
                   UintegerValue (2000),
                   MakeUintegerAccessor (&LteSlO2oCommParams::SetRelayT4108Duration),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RelayTRUIR",
                   "Duration of Remote UE Info Request Timer (Wait for Remote UE Info Response) for Relay UE in milliseconds",
                   UintegerValue (100),
                   MakeUintegerAccessor (&LteSlO2oCommParams::SetRelayTRUIRDuration),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RelayDcrMaximum",
                   "Maximum number of Direct Communication Release retransmissions for Relay UE",
                   UintegerValue (3),
                   MakeUintegerAccessor (&LteSlO2oCommParams::m_relay_DCR_maximum),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("RelayRuirMaximum",
                   "Maximum number of Remote UE Info Request retransmissions for Relay UE",
                   UintegerValue (3),
                   MakeUintegerAccessor (&LteSlO2oCommParams::m_relay_RUIR_maximum),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("RemoteT4100",
                   "Duration of Timer 4100 (Wait for Direct Communication Accept) for Remote UE in milliseconds",
                   UintegerValue (400),
                   MakeUintegerAccessor (&LteSlO2oCommParams::SetRemoteT4100Duration),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RemoteT4101",
                   "Duration of Timer 4101 (Wait for Direct Communication Keepalive Ack or PC5 Data for Direct Communication Keepalive retransmission) for Remote UE in milliseconds",
                   UintegerValue (100),
                   MakeUintegerAccessor (&LteSlO2oCommParams::SetRemoteT4101Duration),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RemoteT4102",
                   "Duration of Timer 4102 (Wait for Direct Communication Keepalive transmission) for Remote UE in milliseconds",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&LteSlO2oCommParams::SetRemoteT4102Duration),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RemoteT4103",
                   "Duration of Timer 4103 (Wait for Direct Communication Release Accept) for Remote UE in milliseconds",
                   UintegerValue (100),
                   MakeUintegerAccessor (&LteSlO2oCommParams::SetRemoteT4103Duration),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("RemoteDcrMaximum",
                   "Maximum number of Direct Communication Release retransmissions for Remote UE",
                   UintegerValue (3),
                   MakeUintegerAccessor (&LteSlO2oCommParams::m_remote_DCR_maximum),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("RemoteDcrqMaximum",
                   "Maximum number of Direct Communication Request retransmissions for Remote UE",
                   UintegerValue (3),
                   MakeUintegerAccessor (&LteSlO2oCommParams::m_remote_DCRq_maximum),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("RemoteDckMaximum",
                   "Maximum number of Direct Communication Keepalive retransmissions for Remote UE",
                   UintegerValue (3),
                   MakeUintegerAccessor (&LteSlO2oCommParams::m_remote_DCK_maximum),
                   MakeUintegerChecker<uint32_t>())
  ;
  return tid;
}

void
LteSlO2oCommParams::SetContextId (LteSlPc5ContextId contextId)
{
  NS_LOG_FUNCTION (this << contextId.peerL2Id << contextId.peerL2Id);
  m_contextId = contextId;
}

LteSlO2oCommParams::LteSlPc5ContextId
LteSlO2oCommParams::GetContextId ()
{
  NS_LOG_FUNCTION (this);
  return m_contextId;
}

void
LteSlO2oCommParams::ClearTimers ()
{
  NS_LOG_FUNCTION (this);

  if (GetRemoteT4100 ()->IsRunning ())
    {
      GetRemoteT4100 ()->Cancel ();
    }
  if (GetRemoteT4101 ()->IsRunning ())
    {
      GetRemoteT4101 ()->Cancel ();
    }
  if (GetRemoteT4102 ()->IsRunning ())
    {
      GetRemoteT4102 ()->Cancel ();
    }
  if (GetRemoteT4103 ()->IsRunning ())
    {
      GetRemoteT4103 ()->Cancel ();
    }
  if (GetRelayT4111 ()->IsRunning ())
    {
      GetRelayT4111 ()->Cancel ();
    }
  if (GetRelayT4103 ()->IsRunning ())
    {
      GetRelayT4103 ()->Cancel ();
    }
  if (GetRelayT4108 ()->IsRunning ())
    {
      GetRelayT4108 ()->Cancel ();
    }
  if (GetRelayTRUIR ()->IsRunning ())
    {
      GetRelayTRUIR ()->Cancel ();
    }
}

void
LteSlO2oCommParams::SetRelayT4111Duration (uint32_t duration)
{
  NS_LOG_FUNCTION (this << duration);

  m_relay_dT4111 = duration;
  m_relay_T4111->SetDelay (MilliSeconds (m_relay_dT4111));
}

void
LteSlO2oCommParams::SetRelayT4103Duration (uint32_t duration)
{
  NS_LOG_FUNCTION (this << duration);

  m_relay_dT4103 = duration;
  m_relay_T4103->SetDelay (MilliSeconds (m_relay_dT4103));
}

void
LteSlO2oCommParams::SetRelayT4108Duration (uint32_t duration)
{
  NS_LOG_FUNCTION (this << duration);

  m_relay_dT4108 = duration;
  m_relay_T4108->SetDelay (MilliSeconds (m_relay_dT4108));
}

void
LteSlO2oCommParams::SetRelayTRUIRDuration (uint32_t duration)
{
  NS_LOG_FUNCTION (this << duration);

  m_relay_dTRUIR = duration;
  m_relay_TRUIR->SetDelay (MilliSeconds (m_relay_dTRUIR));
}

Timer*
LteSlO2oCommParams::GetRelayT4111 ()
{
  return m_relay_T4111;
}

Timer*
LteSlO2oCommParams::GetRelayT4103 ()
{
  return m_relay_T4103;
}

Timer*
LteSlO2oCommParams::GetRelayT4108 ()
{
  return m_relay_T4108;
}

Timer*
LteSlO2oCommParams::GetRelayTRUIR ()
{
  return m_relay_TRUIR;
}

void
LteSlO2oCommParams::SetRemoteT4100Duration (uint32_t duration)
{
  NS_LOG_FUNCTION (this << duration);

  m_remote_dT4100 = duration;
  m_remote_T4100->SetDelay (MilliSeconds (m_remote_dT4100));
}

void
LteSlO2oCommParams::SetRemoteT4101Duration (uint32_t duration)
{
  NS_LOG_FUNCTION (this << duration);

  m_remote_dT4101 = duration;
  m_remote_T4101->SetDelay (MilliSeconds (m_remote_dT4101));
}

void
LteSlO2oCommParams::SetRemoteT4102Duration (uint32_t duration)
{
  NS_LOG_FUNCTION (this << duration);

  m_remote_dT4102 = duration;
  m_remote_T4102->SetDelay (MilliSeconds (m_remote_dT4102));

}

void
LteSlO2oCommParams::SetRemoteT4103Duration (uint32_t duration)
{
  NS_LOG_FUNCTION (this << duration);

  m_remote_dT4103 = duration;
  m_remote_T4103->SetDelay (MilliSeconds (m_remote_dT4103));
}

Timer*
LteSlO2oCommParams::GetRemoteT4100 ()
{
  return m_remote_T4100;
}

Timer*
LteSlO2oCommParams::GetRemoteT4101 ()
{
  return m_remote_T4101;
}

Timer*
LteSlO2oCommParams::GetRemoteT4102 ()
{
  return m_remote_T4102;
}

Timer*
LteSlO2oCommParams::GetRemoteT4103 ()
{
  return m_remote_T4103;
}

uint32_t
LteSlO2oCommParams::GetRelayDcrMax () const
{
  return m_relay_DCR_maximum;
}

uint32_t
LteSlO2oCommParams::GetRelayRuirMax () const
{
  return m_relay_RUIR_maximum;
}

uint32_t
LteSlO2oCommParams::GetRemoteDcrMax () const
{
  return m_remote_DCR_maximum;
}

uint32_t
LteSlO2oCommParams::GetRemoteDcrqMax () const
{
  return m_remote_DCRq_maximum;
}

uint32_t
LteSlO2oCommParams::GetRemoteDckMax () const
{
  return m_remote_DCK_maximum;
}

uint16_t
LteSlO2oCommParams::GetRelayDcrCount () const
{
  return m_relay_DCR_count;
}

void
LteSlO2oCommParams::SetRelayDcrCount (uint16_t value)
{
  m_relay_DCR_count = value;
}

uint16_t
LteSlO2oCommParams::GetRelayRuirCount () const
{
  return m_relay_RUIR_count;
}

void
LteSlO2oCommParams::SetRelayRuirCount (uint16_t value)
{
  m_relay_RUIR_count = value;
}

uint16_t
LteSlO2oCommParams::GetRemoteDcrCount () const
{
  return m_remote_DCR_count;
}

void
LteSlO2oCommParams::SetRemoteDcrCount (uint16_t value)
{
  m_remote_DCR_count = value;
}

uint16_t
LteSlO2oCommParams::GetRemoteDcrqCount () const
{
  return m_remote_DCRq_count;
}

void
LteSlO2oCommParams::SetRemoteDcrqCount (uint16_t value)
{
  m_remote_DCRq_count = value;
}

uint16_t
LteSlO2oCommParams::GetRemoteDckCount () const
{
  return m_remote_DCK_count;
}

void
LteSlO2oCommParams::SetRemoteDckCount (uint16_t value)
{
  m_remote_DCK_count = value;
}

uint16_t
LteSlO2oCommParams::GetRemoteKaCount () const
{
  return m_remote_ka_count;
}

void
LteSlO2oCommParams::SetRemoteKaCount (uint16_t value)
{
  m_remote_ka_count = value;
}

DirectCommunicationRequest
LteSlO2oCommParams::GetDcrqRetrans () const
{
  return m_dcrq_retrans;
}

void
LteSlO2oCommParams::SetDcrqRetrans (DirectCommunicationRequest value)
{
  m_dcrq_retrans = value;
}

DirectCommunicationRelease
LteSlO2oCommParams::GetDcrRetrans () const
{
  return m_dcr_retrans;
}

void
LteSlO2oCommParams::SetDcrRetrans (DirectCommunicationRelease value)
{
  m_dcr_retrans = value;
}

DirectCommunicationKeepalive
LteSlO2oCommParams::GetDckRetrans () const
{
  return m_dck_retrans;
}

void
LteSlO2oCommParams::SetDckRetrans (DirectCommunicationKeepalive value)
{
  m_dck_retrans = value;
}

RemoteUeInfoRequest
LteSlO2oCommParams::GetRuirqRetrans () const
{
  return m_ruirq_retrans;
}

void
LteSlO2oCommParams::SetRuirqRetrans (RemoteUeInfoRequest value)
{
  m_ruirq_retrans = value;
}

DirectSecurityModeCommand
LteSlO2oCommParams::GetDsmcmRetrans () const
{
  return m_dsmcm_retrans;
}

void
LteSlO2oCommParams::SetDsmcmRetrans (DirectSecurityModeCommand value)
{
  m_dsmcm_retrans = value;
}

void
LteSlO2oCommParams::SetDsmcmRetransLsb (uint8_t lsb)
{
  m_dsmcm_retrans.SetLsb (lsb);
}

LteSlO2oCommParams::DirectSecurityModeState
LteSlO2oCommParams::GetSecurityModeState () const
{
  return m_security_mode_state;
}

void
LteSlO2oCommParams::SetSecurityModeState (LteSlO2oCommParams::DirectSecurityModeState value)
{
  m_security_mode_state = value;
}

uint8_t
LteSlO2oCommParams::GetSecurityModeId () const
{
  return m_security_mode_id;
}

void
LteSlO2oCommParams::SetSecurityModeId (uint8_t value)
{
  m_security_mode_id = value;
}

LteSlO2oCommParams::UeO2OCommState
LteSlO2oCommParams::GetState () const
{
  return m_state;
}

void
LteSlO2oCommParams::SetState (LteSlO2oCommParams::UeO2OCommState value)
{
  m_state = value;
}

std::string
LteSlO2oCommParams::GetStateString (LteSlO2oCommParams::UeO2OCommState state)
{
  return m_stateString[state];
}




} // namespace ns3

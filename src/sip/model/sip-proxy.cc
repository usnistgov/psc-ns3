/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright 2020 University of Washington
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/object.h"
#include "ns3/log.h"
#include "sip-proxy.h"
#include "sip-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SipProxy");

namespace sip {

NS_OBJECT_ENSURE_REGISTERED (SipProxy);

TypeId
SipProxy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::sip::SipProxy")
    .SetParent<SipElement> ()
    .SetGroupName ("Sip")
    .AddConstructor<SipProxy> ()
    .AddAttribute ("ProxyInviteTransactionTimeout",
                   "Timer C timeout value",
                   TimeValue (Minutes (4)), // > 3 minutes per RFC 3261
                   MakeTimeAccessor (&SipProxy::m_proxyInviteTransactionTimeout),
                   MakeTimeChecker ())
  ;
  return tid;
}

SipProxy::SipProxy ()
  : SipElement ()
{
  NS_LOG_FUNCTION (this);
}

SipProxy::~SipProxy ()
{
  NS_LOG_FUNCTION (this);
}

void
SipProxy::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  SipElement::DoDispose ();
}

void
SipProxy::SendResponse (Ptr<Packet> p, const Address& addr, uint16_t statusCode, uint32_t from, uint32_t to, uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
{
  NS_LOG_FUNCTION (p << addr << statusCode << from << to << callId);
  TransactionId tid = GetTransactionId (callId, from, to);
  DialogId did = GetDialogId (callId, from, to);
  auto it = GetDialogs ().find (did);
  NS_ASSERT_MSG (it != GetDialogs ().end (), "Dialog not found");
  it->second.m_sendCallback = sendCallback;
  if (statusCode == 100)
    {
      SetDialogState (did, DIALOG_PROCEEDING);
      SetTransactionState (tid, TRANSACTION_PROCEEDING);
      ScheduleTimerC (tid);
    }
  else if (statusCode == 200)
    {
      if (it->second.m_state == DIALOG_TRYING || it->second.m_state == DIALOG_PROCEEDING)
        {
          SetDialogState (did, DIALOG_CONFIRMED);
          SetTransactionState (tid, TRANSACTION_COMPLETED);
          CancelTimerC (tid);
        }
      else if (it->second.m_state == DIALOG_TERMINATED)
        {
          SetTransactionState (tid, TRANSACTION_COMPLETED);
          ScheduleTimerJ (tid);  // Start Timer J (to transition to TERMINATED)
        }
      else
        {
          NS_FATAL_ERROR ("SendResponse 200 unsupported from state " << DialogStateToString (it->second.m_state));
        }
    }
  else if (statusCode == 408)
    {
      SetDialogState (did, DIALOG_TERMINATED);
      SetTransactionState (tid, TRANSACTION_FAILED);
      CancelTimerC (tid);
    }
  SipHeader header;
  header.SetMessageType (SipHeader::SIP_RESPONSE);
  header.SetStatusCode (statusCode);
  header.SetFrom (from);
  header.SetTo (to);
  header.SetCallId (callId);
  p->AddHeader (header);
  sendCallback (p, addr, header);
  m_txTrace (p, header);
}

void
SipProxy::ScheduleTimerC (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = GetTransactions ().find (id);
  NS_ASSERT_MSG (transIt != GetTransactions ().end (), "Transaction not found");
  transIt->second.m_timerC.SetFunction (&SipProxy::HandleTimerC, this);
  transIt->second.m_timerC.SetArguments (id);
  transIt->second.m_timerC.Schedule (m_proxyInviteTransactionTimeout);
}

void
SipProxy::CancelTimerC (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = GetTransactions ().find (id);
  NS_ASSERT_MSG (transIt != GetTransactions ().end (), "Transaction not found");
  transIt->second.m_timerC.Cancel ();
}

void
SipProxy::HandleTimerC (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto eventIt = GetEventCallbacks ().find (std::get<0> (id));
  NS_ASSERT_MSG (eventIt != GetEventCallbacks ().end (), "CallID not found");
  auto transIt = GetTransactions ().find (id);
  NS_ASSERT_MSG (transIt != GetTransactions ().end (), "Transaction not found");
  NS_ASSERT_MSG (transIt->second.m_state == TRANSACTION_PROCEEDING, "Transaction not in PROCEEDING");
  // Notify user and let user handle this event
  eventIt->second (TIMER_C_EXPIRED, transIt->second.m_state);
}

} // namespace sip

} // namespace ns3

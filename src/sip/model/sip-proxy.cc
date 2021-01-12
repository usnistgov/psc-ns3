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
  static TypeId tid = TypeId ("ns3::SipProxy")
    .SetParent<SipElement> ()
    .SetGroupName ("Sip")
    .AddConstructor<SipProxy> ()
    .AddAttribute ("ProxyInviteTransactionTimeout",
                   "Timer C timeout value",
                   TimeValue (Minutes (5)), // > 3 minutes
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
SipProxy::ScheduleTimerC (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  transIt->second.m_timerC.SetFunction (&SipProxy::HandleTimerC, this);
  transIt->second.m_timerC.SetArguments (id);
  transIt->second.m_timerC.Schedule (m_proxyInviteTransactionTimeout);
}

void
SipProxy::CancelTimerC (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  transIt->second.m_timerC.Cancel ();
}

void
SipProxy::HandleTimerC (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  DialogId did = GetDialogId (std::get<0> (id), std::get<1> (id), std::get<2> (id));
  auto eventIt = m_eventCallbacks.find (std::get<0> (id));
  NS_ASSERT_MSG (eventIt != m_eventCallbacks.end (), "CallID not found");
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  NS_ASSERT_MSG (transIt->second.m_state == TransactionState::PROCEEDING, "Transaction not in PROCEEDING");
  eventIt->second (TIMER_C_EXPIRED, transIt->second.m_state);
  // Send CANCEL to client
  // Fail the transaction
  SetTransactionState (id, TransactionState::FAILED);
  SetDialogState (did, DialogState::TERMINATED);
}

} // namespace sip

} // namespace ns3

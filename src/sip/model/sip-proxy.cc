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

#include <tuple>
#include <utility>
#include "ns3/object.h"
#include "ns3/callback.h"
#include "ns3/traced-callback.h"
#include "ns3/log.h"
#include "ns3/socket.h"
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
    .SetParent<Object> ()
    .SetGroupName ("Sip")
    .AddConstructor<SipProxy> ()
    .AddTraceSource ("TxTrace", "The trace for capturing transmitted messages",
                     MakeTraceSourceAccessor (&SipProxy::m_txTrace),
                     "ns3::sip::SipProxy::TxRxTracedCallback")
    .AddTraceSource ("RxTrace", "The trace for capturing received messages",
                     MakeTraceSourceAccessor (&SipProxy::m_rxTrace),
                     "ns3::sip::SipProxy::TxRxTracedCallback")
    .AddTraceSource ("DialogState", "Trace of Dialog state changes",
                     MakeTraceSourceAccessor (&SipProxy::m_dialogTrace),
                     "ns3::sip::SipProxy::DialogStateTracedCallback")
    .AddTraceSource ("TransactionState", "Trace of Transaction state changes",
                     MakeTraceSourceAccessor (&SipProxy::m_transactionTrace),
                     "ns3::sip::SipProxy::TransactionStateTracedCallback")
  ;
  return tid;
}

SipProxy::SipProxy ()
  : m_defaultSendCallback (MakeNullCallback<void, Ptr<Packet>, const Address&, const SipHeader&> ())
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
  m_receiveCallbacks.clear ();
  m_eventCallbacks.clear ();
  m_dialogs.clear ();
  m_defaultSendCallback = MakeNullCallback<void, Ptr<Packet>, const Address&, const SipHeader&> ();
}

std::string
SipProxy::TransactionStateToString (TransactionState state)
{
  switch (state)
    {
      case TransactionState:: IDLE:
        return "IDLE";
      case TransactionState:: CALLING:
        return "CALLING";
      case TransactionState:: TRYING:
        return "TRYING";
      case TransactionState:: PROCEEDING:
        return "PROCEEDING";
      case TransactionState:: COMPLETED:
        return "COMPLETED";
      case TransactionState:: CONFIRMED:
        return "CONFIRMED";
      case TransactionState:: TERMINATED:
        return "TERMINATED";
      case TransactionState:: FAILED:
        return "FAILED";
      default:
        return "Unrecognized state";
    }
}

std::string
SipProxy::DialogStateToString (DialogState state)
{
  switch (state)
    {
      case DialogState:: UNINITIALIZED:
        return "UNINITIALIZED";
      case DialogState:: TRYING:
        return "TRYING";
      case DialogState:: PROCEEDING:
        return "PROCEEDING";
      case DialogState:: EARLY:
        return "EARLY";
      case DialogState:: CONFIRMED:
        return "CONFIRMED";
      case DialogState:: TERMINATED:
        return "TERMINATED";
      default:
        return "Unrecognized state";
    }
}

void
SipProxy::SendInvite (Ptr<Packet> p, const Address& addr, uint32_t requestUri, uint32_t from, uint32_t to, uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
{
  NS_LOG_FUNCTION (p << addr << requestUri << from << to << callId);
  CreateTransaction (callId, from, to);
  SetTransactionState (callId, from, to, TransactionState::CALLING);
  SipHeader header;
  header.SetMessageType (SipHeader::SIP_REQUEST);
  header.SetMethod (SipHeader::INVITE);
  header.SetRequestUri (requestUri);
  header.SetFrom (from);
  header.SetTo (to);
  header.SetCallId (callId);
  p->AddHeader (header);
  sendCallback (p, addr, header);
  m_txTrace (p, header);
}

void
SipProxy::SendBye (Ptr<Packet> p, const Address& addr, uint32_t requestUri, uint32_t from, uint32_t to, uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
{
  NS_LOG_FUNCTION (p << addr << requestUri << from << to << callId);
  auto dialogIt = m_dialogs.find (callId);
  if (dialogIt->second.m_state != DialogState::TERMINATED)
    {
      SetDialogState (callId, DialogState::TERMINATED);
    }
  dialogIt->second.m_sendCallback = sendCallback;
  if (!TransactionExists (callId, from, to))
    {
      CreateTransaction (callId, from, to);
    }
  SetTransactionState (callId, from, to, TransactionState::TRYING);
  SipHeader header;
  header.SetMessageType (SipHeader::SIP_REQUEST);
  header.SetMethod (SipHeader::BYE);
  header.SetRequestUri (requestUri);
  header.SetFrom (from);
  header.SetTo (to);
  header.SetCallId (callId);
  p->AddHeader (header);
  sendCallback (p, addr, header);
  m_txTrace (p, header);
}

void
SipProxy::SendResponse (Ptr<Packet> p, const Address& addr, uint16_t statusCode, uint32_t from, uint32_t to, uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
{
  NS_LOG_FUNCTION (p << statusCode << from << to << callId);
  auto dialogIt = m_dialogs.find (callId);
  if (dialogIt == m_dialogs.end ())
    {
      NS_FATAL_ERROR ("Dialog does not exist for callId");
    }
  // The send callback may have been set to the default send callback earlier
  // upon receipt of a request.  Update it here with the provided callback.
  dialogIt->second.m_sendCallback = sendCallback;
  if (!TransactionExists (callId, from, to))
    {
      NS_FATAL_ERROR ("Transaction does not exist for callId");
    }
  if (statusCode == 100)
    {
      if (dialogIt->second.m_state == DialogState::TRYING)
        {
          NS_LOG_DEBUG ("Most likely responding to INVITE with a 100 Trying");
          SetDialogState (callId, DialogState::PROCEEDING);
          SetTransactionState (callId, from, to, TransactionState::PROCEEDING);
        }
      else
        {
          NS_FATAL_ERROR ("Sending 100 from state other than TRYING is not implemented");
        }
    }
  else if (statusCode == 200)
    {
      if (dialogIt->second.m_state == DialogState::TRYING || dialogIt->second.m_state == DialogState::PROCEEDING)
        {
          NS_LOG_DEBUG ("200 Response could be in response to INVITE or BYE");
          SetTransactionState (callId, from, to, TransactionState::PROCEEDING);
        }
      std::tuple<uint16_t, uint32_t, uint32_t> key (callId, from, to);
      auto transactionIt = m_transactions.find (key);
      if (transactionIt->second == TransactionState::TRYING || transactionIt->second == TransactionState::PROCEEDING)
        {
          SetTransactionState (callId, from, to, TransactionState::COMPLETED);
          // Start Timer J
        }
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
SipProxy::Receive (Ptr<Packet> p, Address from)
{
  NS_LOG_FUNCTION (this << p << from);
  SipHeader sipHeader;
  p->RemoveHeader (sipHeader);
  m_rxTrace (p, sipHeader);
  auto eventIt = m_eventCallbacks.find (sipHeader.GetCallId ());
  auto receiveIt = m_receiveCallbacks.find (sipHeader.GetCallId ());
  if (eventIt == m_eventCallbacks.end () || receiveIt == m_receiveCallbacks.end ())
    {
      NS_FATAL_ERROR ("CallId does not have callbacks set");
    }
  if (sipHeader.GetMessageType () == SipHeader::SIP_RESPONSE)
    {
      if (sipHeader.GetStatusCode () == 100)
        { 
          NS_LOG_DEBUG ("Received 100 Trying for call ID " << sipHeader.GetCallId ());
          eventIt->second (TRYING_RECEIVED, TransactionState::PROCEEDING);
          SetDialogState (sipHeader.GetCallId (), DialogState::PROCEEDING);
          SetTransactionState (sipHeader.GetCallId (), sipHeader.GetFrom (), sipHeader.GetTo (), TransactionState::PROCEEDING);
          // Stop timer A
        }
      else if (sipHeader.GetStatusCode () == 200)
        {
          NS_LOG_DEBUG ("Received 200 OK for call ID " << sipHeader.GetCallId ());
          auto dialogIt = m_dialogs.find (sipHeader.GetCallId ());
          NS_ABORT_MSG_IF (dialogIt == m_dialogs.end (), "Dialog not found");
          if (dialogIt->second.m_state == DialogState::TRYING || dialogIt->second.m_state == DialogState::PROCEEDING || dialogIt->second.m_state == DialogState::CONFIRMED)
            {
              if (dialogIt->second.m_state != DialogState::CONFIRMED)
                {
                  SetDialogState (sipHeader.GetCallId (), DialogState::CONFIRMED);
                }
              SetTransactionState (sipHeader.GetCallId (), sipHeader.GetFrom (), sipHeader.GetTo (), TransactionState::TERMINATED);
              // Deliver the packet since the OK may have SDP information
              receiveIt->second (p, sipHeader, TransactionState::TERMINATED);
              NS_LOG_DEBUG ("Send ACK for call ID " << sipHeader.GetCallId ());
              Ptr<Packet> packet = Create<Packet> ();
              SipHeader header;
              header.SetMessageType (SipHeader::SIP_REQUEST);
              header.SetMethod (SipHeader::ACK);
              header.SetRequestUri (sipHeader.GetRequestUri ());
              header.SetFrom (sipHeader.GetFrom ());
              header.SetTo (sipHeader.GetTo ());
              header.SetCallId (sipHeader.GetCallId ());
              packet->AddHeader (header);
              // ACK to the source address of the incoming packet.
              dialogIt->second.m_sendCallback (packet, from, header);
              m_txTrace (packet, header);
            }
          else if (dialogIt->second.m_state == DialogState::TERMINATED)
            {
              NS_LOG_DEBUG ("No ACK needed for 200 OK response to BYE");
              SetTransactionState (sipHeader.GetCallId (), sipHeader.GetFrom (), sipHeader.GetTo (), TransactionState::COMPLETED);
              // Deliver the packet, although the OK of BYE should not include SDP
              receiveIt->second (p, sipHeader, TransactionState::COMPLETED);
              // Set Timer K to transition to TERMINATED
            }
          else
            {
              NS_FATAL_ERROR ("Received 200 OK in unexpected state " << SipProxy::DialogStateToString (dialogIt->second.m_state));
            }
        }
    }
  else if (sipHeader.GetMessageType () == SipHeader::SIP_REQUEST)
    {
      if (sipHeader.GetMethod () == SipHeader::INVITE)
        { 
          NS_LOG_DEBUG ("Received INVITE for call ID " << sipHeader.GetCallId ());
          // The first INVITE for a call ID should create the Dialog for it
          if (!DialogExists (sipHeader.GetCallId ()))
            {
              NS_LOG_DEBUG ("Creating dialog for call ID " << sipHeader.GetCallId ());
              CreateDialog (sipHeader.GetCallId (), m_defaultSendCallback);
              SetDialogState (sipHeader.GetCallId (), DialogState::TRYING);
              CreateTransaction (sipHeader.GetCallId (), sipHeader.GetFrom (), sipHeader.GetTo ());
              SetTransactionState (sipHeader.GetCallId (), sipHeader.GetFrom (), sipHeader.GetTo (), TransactionState::TRYING);
              receiveIt->second (p, sipHeader, TransactionState::TRYING);
            }
          else
            {
              NS_FATAL_ERROR ("TODO:  Handle multiple INVITES at proxy");
            }
        }
      else if (sipHeader.GetMethod () == SipHeader::BYE)
        { 
          NS_LOG_DEBUG ("Received BYE for call ID " << sipHeader.GetCallId ());
          SetDialogState (sipHeader.GetCallId (), DialogState::TERMINATED);
          if (!TransactionExists (sipHeader.GetCallId (), sipHeader.GetFrom (), sipHeader.GetTo ()))
            {
              CreateTransaction (sipHeader.GetCallId (), sipHeader.GetFrom (), sipHeader.GetTo ());
            }
          SetTransactionState (sipHeader.GetCallId (), sipHeader.GetFrom (), sipHeader.GetTo (), TransactionState::TRYING);
          receiveIt->second (p, sipHeader, TransactionState::TRYING);
        }
      else if (sipHeader.GetMethod () == SipHeader::ACK)
        { 
          NS_LOG_DEBUG ("Received ACK for call ID " << sipHeader.GetCallId ());
          SetTransactionState (sipHeader.GetCallId (), sipHeader.GetFrom (), sipHeader.GetTo (), TransactionState::CONFIRMED);
          eventIt->second (ACK_RECEIVED, TransactionState::CONFIRMED);
          // Stop Timer H
          // Start Timer I (to absorb any acks)
        }
    }
}

void
SipProxy::SetServerCallbacks (uint16_t callId, Callback<void, Ptr<Packet>, const SipHeader&, TransactionState> receiveCallback, Callback<void, const char*, TransactionState> eventCallback)
{
  NS_LOG_FUNCTION (this << callId);

  auto element = m_receiveCallbacks.find (callId);
  if (element == m_receiveCallbacks.end ())
    {
      m_receiveCallbacks.emplace (callId, receiveCallback);
    }
  else
    {
      NS_FATAL_ERROR ("CallId has already configured a receive callback");
    }
  auto element2 = m_eventCallbacks.find (callId);
  if (element2 == m_eventCallbacks.end ())
    {
      m_eventCallbacks.emplace (callId, eventCallback);
    }
  else
    {
      NS_FATAL_ERROR ("CallId has already configured an event callback");
    }
}

void
SipProxy::SetDefaultSendCallback (Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
{
  NS_LOG_FUNCTION (this);
  m_defaultSendCallback = sendCallback;
}

void
SipProxy::CreateDialog (uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
{
  NS_LOG_FUNCTION (this << callId);
  Dialog dialog (callId, sendCallback, DialogState::UNINITIALIZED);
  std::pair<std::unordered_map<uint16_t, Dialog>::iterator, bool> returnValue;
  returnValue = m_dialogs.emplace (callId, dialog);
  NS_ABORT_MSG_UNLESS (returnValue.second, "Emplace SipProxy Dialog failed");
}

bool
SipProxy::DialogExists (uint16_t callId) const
{
  auto dialogIt = m_dialogs.find (callId);
  if (dialogIt == m_dialogs.end ())
    {
      return false;
    }
  else
    {
      return true;
    }
}

void
SipProxy::SetDialogState (uint16_t callId, DialogState state)
{
  NS_LOG_FUNCTION (this << callId << DialogStateToString (state));
  auto dialogIt = m_dialogs.find (callId);
  NS_ABORT_MSG_IF (dialogIt == m_dialogs.end (), "Dialog not found");
  dialogIt->second.m_state = state;
  m_dialogTrace (callId, state);
}

void
SipProxy::CreateTransaction (uint16_t callId, uint32_t from, uint32_t to)
{
  NS_LOG_FUNCTION (this << callId << from << to);
  std::tuple<uint16_t, uint32_t, uint32_t> key (callId, from, to);
  auto transactionIt = m_transactions.find (key);
  if (transactionIt == m_transactions.end ())
    {
      std::pair<std::unordered_map<std::tuple<uint16_t, uint32_t, uint32_t>, TransactionState, TupleHash>::iterator, bool> returnValue;
      returnValue = m_transactions.emplace (key, TransactionState::IDLE);
      NS_ABORT_MSG_UNLESS (returnValue.second, "Emplace SipProxy Transaction failed");
    }
  else
    {
      transactionIt->second = TransactionState::IDLE;
    }
}

bool
SipProxy::TransactionExists (uint16_t callId, uint32_t from, uint32_t to) const
{
  std::tuple<uint16_t, uint32_t, uint32_t> key (callId, from, to);
  auto transactionIt = m_transactions.find (key);
  if (transactionIt == m_transactions.end ())
    {
      return false;
    }
  else
    {
      return true;
    }
}

void
SipProxy::SetTransactionState (uint16_t callId, uint32_t from, uint32_t to, TransactionState state)
{
  NS_LOG_FUNCTION (this << callId << from << to << TransactionStateToString (state));
  std::tuple<uint16_t, uint32_t, uint32_t> key (callId, from, to);
  auto transactionIt = m_transactions.find (key);
  NS_ABORT_MSG_IF (transactionIt == m_transactions.end (), "Transaction not found");
  transactionIt->second = state;
  m_transactionTrace (callId, from, to, state);
}

std::ostream& operator << (std::ostream& os, const SipProxy::DialogState& state)
{
   os << static_cast<uint16_t> (state);
   return os;
}

std::ostream& operator << (std::ostream& os, const SipProxy::TransactionState& state)
{
   os << static_cast<uint16_t> (state);
   return os;
}

} // namespace sip

} // namespace ns3

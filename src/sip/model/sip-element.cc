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

#include <sstream>
#include "ns3/object.h"
#include "ns3/log.h"
#include "ns3/socket.h"
#include "ns3/boolean.h"
#include "ns3/nstime.h"
#include "sip-element.h"
#include "sip-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SipElement");

namespace sip {

NS_OBJECT_ENSURE_REGISTERED (SipElement);

TypeId
SipElement::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::sip::SipElement")
    .SetParent<Object> ()
    .SetGroupName ("Sip")
    .AddConstructor<SipElement> ()
    .AddAttribute ("ReliableTransport",
                   "Whether the transport is reliable (TCP, SCTP) or unreliable (UDP)",
                   BooleanValue (false),
                   MakeBooleanAccessor (&SipElement::m_reliableTransport),
                   MakeBooleanChecker ())
    .AddAttribute ("T1",
                   "RTT Estimate",
                   TimeValue (MilliSeconds (500)), // RFC 3261 default
                   MakeTimeAccessor (&SipElement::m_t1),
                   MakeTimeChecker ())
    .AddAttribute ("T2",
                   "Maximum retransmit interval for non-INVITE requests and INVITE responses",
                   TimeValue (Seconds (4)), // RFC 3261 default
                   MakeTimeAccessor (&SipElement::m_t2),
                   MakeTimeChecker ())
    .AddAttribute ("T4",
                   "Maximum duration a message will remain in the network",
                   TimeValue (Seconds (5)), // RFC 3261 default
                   MakeTimeAccessor (&SipElement::m_t4),
                   MakeTimeChecker ())
    .AddTraceSource ("TxTrace", "The trace for capturing transmitted messages",
                     MakeTraceSourceAccessor (&SipElement::m_txTrace),
                     "ns3::sip::SipElement::TxRxTracedCallback")
    .AddTraceSource ("RxTrace", "The trace for capturing received messages",
                     MakeTraceSourceAccessor (&SipElement::m_rxTrace),
                     "ns3::sip::SipElement::TxRxTracedCallback")
    .AddTraceSource ("DialogState", "Trace of Dialog state changes",
                     MakeTraceSourceAccessor (&SipElement::m_dialogTrace),
                     "ns3::sip::SipElement::DialogStateTracedCallback")
    .AddTraceSource ("TransactionState", "Trace of Transaction state changes",
                     MakeTraceSourceAccessor (&SipElement::m_transactionTrace),
                     "ns3::sip::SipElement::TransactionStateTracedCallback")
  ;
  return tid;
}

SipElement::SipElement ()
  : m_defaultSendCallback (MakeNullCallback<void, Ptr<Packet>, const Address&, const SipHeader&> ())
{
  NS_LOG_FUNCTION (this);
}

SipElement::~SipElement ()
{
  NS_LOG_FUNCTION (this);
  m_dialogs.clear ();
  m_transactions.clear ();
}

void
SipElement::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_receiveCallbacks.clear ();
  m_eventCallbacks.clear ();
  m_defaultSendCallback = MakeNullCallback<void, Ptr<Packet>, const Address&, const SipHeader&> ();
}

std::string
SipElement::TransactionStateToString (TransactionState state)
{
  switch (state)
    {
      case TRANSACTION_IDLE:
        return "IDLE";
      case TRANSACTION_CALLING:
        return "CALLING";
      case TRANSACTION_TRYING:
        return "TRYING";
      case TRANSACTION_PROCEEDING:
        return "PROCEEDING";
      case TRANSACTION_COMPLETED:
        return "COMPLETED";
      case TRANSACTION_CONFIRMED:
        return "CONFIRMED";
      case TRANSACTION_TERMINATED:
        return "TERMINATED";
      case TRANSACTION_FAILED:
        return "FAILED";
      default:
        return "Unrecognized state";
    }
}

std::string
SipElement::DialogStateToString (DialogState state)
{
  switch (state)
    {
      case DIALOG_UNINITIALIZED:
        return "UNINITIALIZED";
      case DIALOG_TRYING:
        return "TRYING";
      case DIALOG_PROCEEDING:
        return "PROCEEDING";
      case DIALOG_EARLY:
        return "EARLY";
      case DIALOG_CONFIRMED:
        return "CONFIRMED";
      case DIALOG_TERMINATED:
        return "TERMINATED";
      default:
        return "Unrecognized state";
    }
}

void
SipElement::SendInvite (Ptr<Packet> p, const Address& addr, uint32_t requestUri, uint32_t from, uint32_t to, uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
{
  NS_LOG_FUNCTION (p << addr << requestUri << from << to << callId);

  CreateDialog (GetDialogId (callId, from, to), sendCallback);
  SetDialogState (GetDialogId (callId, from, to), DIALOG_TRYING);
  TransactionId tid = GetTransactionId (callId, from, to);
  CreateTransaction (tid, sendCallback);
  SetTransactionState (tid, TRANSACTION_CALLING);
  SipHeader header;
  header.SetMessageType (SipHeader::SIP_REQUEST);
  header.SetMethod (SipHeader::INVITE);
  header.SetRequestUri (requestUri);
  header.SetFrom (from);
  header.SetTo (to);
  header.SetCallId (callId);
  p->AddHeader (header);
  CacheTransactionPacket (tid, p, addr, header);
  sendCallback (p, addr, header);
  m_txTrace (p, header);
  // Start timers A and B
  uint32_t backoff = 1;
  ScheduleTimerA (tid, backoff);
  ScheduleTimerB (tid);
}

void
SipElement::SendBye (Ptr<Packet> p, const Address& addr, uint32_t requestUri, uint32_t from, uint32_t to, uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
{
  NS_LOG_FUNCTION (p << addr << requestUri << from << to << callId);
  TransactionId tid = GetTransactionId (callId, from, to);
  DialogId did = GetDialogId (callId, from, to);
  auto it = m_dialogs.find (did);
  NS_ASSERT_MSG (it != m_dialogs.end (), "Dialog not found");
  it->second.m_sendCallback = sendCallback;
  SetDialogState (did, DIALOG_TERMINATED);
  if (TransactionExists (tid))
    {
      SetTransactionState (tid, TRANSACTION_TRYING);
    }
  else
    {
      CreateTransaction (tid, sendCallback);
      SetTransactionState (tid, TRANSACTION_TRYING);
    }
  SipHeader header;
  header.SetMessageType (SipHeader::SIP_REQUEST);
  header.SetMethod (SipHeader::BYE);
  header.SetRequestUri (requestUri);
  header.SetFrom (from);
  header.SetTo (to);
  header.SetCallId (callId);
  p->AddHeader (header);
  CacheTransactionPacket (tid, p, addr, header);
  sendCallback (p, addr, header);
  m_txTrace (p, header);
  uint32_t backoff = 1;
  ScheduleTimerE (tid, backoff);
  ScheduleTimerF (tid);
}

void
SipElement::SendResponse (Ptr<Packet> p, const Address& addr, uint16_t statusCode, uint32_t from, uint32_t to, uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
{
  NS_LOG_FUNCTION (p << addr << statusCode << from << to << callId);
  TransactionId tid = GetTransactionId (callId, from, to);
  DialogId did = GetDialogId (callId, from, to);
  auto it = m_dialogs.find (did);
  NS_ASSERT_MSG (it != m_dialogs.end (), "Dialog not found");
  it->second.m_sendCallback = sendCallback;
  if (statusCode == 100)
    {
      SetDialogState (did, DIALOG_PROCEEDING);
      SetTransactionState (tid, TRANSACTION_PROCEEDING);
    }
  else if (statusCode == 200)
    {
      if (it->second.m_state == DIALOG_TRYING || it->second.m_state == DIALOG_PROCEEDING)
        {
          SetDialogState (did, DIALOG_CONFIRMED);
          SetTransactionState (tid, TRANSACTION_COMPLETED);
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
SipElement::Receive (Ptr<Packet> p, Address from)
{
  NS_LOG_FUNCTION (this << p << from);
  SipHeader sipHeader;
  p->RemoveHeader (sipHeader);
  m_rxTrace (p, sipHeader);
  TransactionId tid = GetTransactionId (sipHeader.GetCallId (), sipHeader.GetFrom (), sipHeader.GetTo ());
  NS_LOG_DEBUG ("Receive packet for TransactionId " << TransactionIdToString (tid));
  DialogId did = GetDialogId (sipHeader.GetCallId (), sipHeader.GetFrom (), sipHeader.GetTo ());
  auto eventIt = m_eventCallbacks.find (sipHeader.GetCallId ());
  auto receiveIt = m_receiveCallbacks.find (sipHeader.GetCallId ());
  if (eventIt == m_eventCallbacks.end () || receiveIt == m_receiveCallbacks.end ())
    {
      NS_FATAL_ERROR ("CallId " << sipHeader.GetCallId () << " does not have callbacks set");
    }
  if (sipHeader.GetMessageType () == SipHeader::SIP_RESPONSE)
    {
      if (sipHeader.GetStatusCode () == 100)
        {
          NS_LOG_DEBUG ("Received 100 Trying for call ID " << sipHeader.GetCallId ());
          eventIt->second (TRYING_RECEIVED, TRANSACTION_PROCEEDING);
          SetDialogState (did, DIALOG_PROCEEDING);
          SetTransactionState (tid, TRANSACTION_PROCEEDING);
          CancelTimerA (tid);
          CancelTimerB (tid);
          FreeTransactionPacket (tid);
        }
      else if (sipHeader.GetStatusCode () == 200)
        {
          NS_LOG_DEBUG ("Received 200 OK for call ID " << sipHeader.GetCallId ());
          auto dialogIt = m_dialogs.find (did);
          if (dialogIt->second.m_state == DIALOG_TRYING || dialogIt->second.m_state == DIALOG_PROCEEDING)
            {
              SetDialogState (did, DIALOG_CONFIRMED);
              CancelTimerA (tid);
              CancelTimerB (tid);
              SetTransactionState (tid, TRANSACTION_TERMINATED);
              FreeTransactionPacket (tid);
              // Deliver the packet since the OK may have SDP information
              receiveIt->second (p, sipHeader, TRANSACTION_TERMINATED);
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
          else if (dialogIt->second.m_state == DIALOG_CONFIRMED)
            {
              // The transaction should be already terminated, but possibly the
              // ACK was lost.
              NS_LOG_DEBUG ("Resend ACK for call ID " << sipHeader.GetCallId ());
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
          else if (dialogIt->second.m_state == DIALOG_TERMINATED)
            {
              NS_LOG_DEBUG ("No ACK needed for 200 OK response to BYE");
              SetTransactionState (tid, TRANSACTION_COMPLETED);
              // Deliver the packet, although the OK of BYE should not include SDP
              receiveIt->second (p, sipHeader, TRANSACTION_COMPLETED);
              CancelTimerE (tid);
              CancelTimerF (tid);
              ScheduleTimerK (tid);  // Start Timer J (to transition to TERMINATED)
            }
          else
            {
              NS_FATAL_ERROR ("Received 200 OK in unexpected state");
            }
        }
      else if (sipHeader.GetStatusCode () == 408)
        {
          NS_LOG_DEBUG ("Received 408 Request Timeout for call ID " << sipHeader.GetCallId ());
          CancelTimerA (tid);
          CancelTimerB (tid);
          FreeTransactionPacket (tid);
          SetDialogState (did, DIALOG_TERMINATED);
          SetTransactionState (tid, TRANSACTION_FAILED);
          // Deliver the packet in case SDP information is present
          receiveIt->second (p, sipHeader, TRANSACTION_FAILED);
        }
      else
        {
          NS_LOG_DEBUG ("Received unknown response " << sipHeader.GetStatusCode () << " for call ID " << sipHeader.GetCallId ());
        }
    }
  else if (sipHeader.GetMessageType () == SipHeader::SIP_REQUEST)
    {
      if (sipHeader.GetMethod () == SipHeader::INVITE)
        {
          NS_LOG_DEBUG ("Received INVITE for call ID " << sipHeader.GetCallId ());
          auto dialogIt = m_dialogs.find (did);
          if (dialogIt == m_dialogs.end ())
            {
              NS_LOG_DEBUG ("Creating dialog for call ID " << sipHeader.GetCallId ());
              CreateDialog (did, m_defaultSendCallback);
              SetDialogState (did, DIALOG_TRYING);
              CreateTransaction (tid, m_defaultSendCallback);
              SetTransactionState (tid, TRANSACTION_TRYING);
              receiveIt->second (p, sipHeader, TRANSACTION_TRYING);
            }
          else
            {
              NS_LOG_DEBUG ("Dialog already exists; ignoring possible retransmission");
            }
        }
      else if (sipHeader.GetMethod () == SipHeader::BYE)
        {
          NS_LOG_DEBUG ("Received BYE for call ID " << sipHeader.GetCallId ());
          SetDialogState (did, DIALOG_TERMINATED);
          if (TransactionExists (tid))
            {
              auto transIt = m_transactions.find (tid);
              if (transIt->second.m_timerI.IsRunning ())
                {
                  // BYE may arrive to a UAS still in CONFIRMED state
                  NS_LOG_DEBUG ("Cancelling Timer I (running)");
                  transIt->second.m_timerI.Cancel ();
                }
            }
          else
            {
              CreateTransaction (tid, m_defaultSendCallback);
            }
          SetTransactionState (tid, TRANSACTION_TRYING);
          receiveIt->second (p, sipHeader, TRANSACTION_TRYING);
        }
      else if (sipHeader.GetMethod () == SipHeader::ACK)
        {
          NS_LOG_DEBUG ("Received ACK for call ID " << sipHeader.GetCallId ());
          eventIt->second (ACK_RECEIVED, TRANSACTION_CONFIRMED);
          SetTransactionState (tid, TRANSACTION_CONFIRMED);
          ScheduleTimerI (tid);  // Start Timer I (to absorb further ACKs)
        }
    }
}

void
SipElement::SetCallbacks (uint16_t callId, Callback<void, Ptr<Packet>, const SipHeader&, TransactionState> receiveCallback, Callback<void, const char*, TransactionState> eventCallback)
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
SipElement::SetDefaultSendCallback (Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
{
  NS_LOG_FUNCTION (this);
  m_defaultSendCallback = sendCallback;
}

// Protected members

std::string
SipElement::DialogIdToString (DialogId id) const
{
  std::stringstream ss;
  ss << "(" << std::get<0> (id) << "," << std::get<1> (id) << "," << std::get<2> (id) << ")";
  return ss.str ();
}

SipElement::DialogId
SipElement::GetDialogId (uint16_t callId, uint32_t uriA, uint32_t uriB) const
{
  if (uriA < uriB)
    {
      return DialogId (callId, uriA, uriB);
    }
  else
    {
      return DialogId (callId, uriB, uriA);
    }
}

void
SipElement::CreateDialog (DialogId id, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
{
  NS_LOG_FUNCTION (this << DialogIdToString (id));
  Dialog dialog (std::get<0> (id), sendCallback, DIALOG_UNINITIALIZED);
  std::pair<std::unordered_map<DialogId, Dialog, TupleHash>::iterator, bool> returnValue;
  returnValue = m_dialogs.emplace (id, dialog);
  NS_ABORT_MSG_UNLESS (returnValue.second, "Emplace SipElement Dialog failed");
}

bool
SipElement::DialogExists (DialogId id) const
{
  auto dialogIt = m_dialogs.find (id);
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
SipElement::SetDialogState (DialogId id, DialogState state)
{
  NS_LOG_FUNCTION (this << DialogIdToString (id) << DialogStateToString (state));
  auto dialogIt = m_dialogs.find (id);
  NS_ABORT_MSG_IF (dialogIt == m_dialogs.end (), "Dialog not found");
  dialogIt->second.m_state = state;
  m_dialogTrace (std::get<0> (id), std::get<1> (id), std::get<2> (id), state);
}

std::string
SipElement::TransactionIdToString (TransactionId id) const
{
  std::stringstream ss;
  ss << "(" << std::get<0> (id) << "," << std::get<1> (id) << "," << std::get<2> (id) << ")";
  return ss.str ();
}

SipElement::TransactionId
SipElement::GetTransactionId (uint16_t callId, uint32_t from, uint32_t to) const
{
  return TransactionId (callId, from, to);
}

void
SipElement::CreateTransaction (TransactionId id, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  if (transIt == m_transactions.end ())
    {
      std::pair<std::unordered_map<TransactionId, Transaction, TupleHash>::iterator, bool> returnValue;
      returnValue = m_transactions.emplace (id, Transaction (std::get<0> (id), sendCallback));
      NS_ABORT_MSG_UNLESS (returnValue.second, "Emplace SipElement Transaction failed");
    }
  else
    {
      if (transIt->second.m_state != TRANSACTION_IDLE)
        {
          if (transIt->second.m_timerI.IsRunning ())
            {
              NS_LOG_DEBUG ("Cancelling Timer I (running)");
              transIt->second.m_timerI.Cancel ();
            }
          if (transIt->second.m_timerJ.IsRunning ())
            {
              NS_LOG_DEBUG ("Cancelling Timer J (running)");
              transIt->second.m_timerJ.Cancel ();
            }
          if (transIt->second.m_timerK.IsRunning ())
            {
              NS_LOG_DEBUG ("Cancelling Timer K (running)");
              transIt->second.m_timerK.Cancel ();
            }
        }
      transIt->second.m_state = TRANSACTION_IDLE;
    }
}

bool
SipElement::TransactionExists (TransactionId id) const
{
  auto transIt = m_transactions.find (id);
  if (transIt == m_transactions.end ())
    {
      return false;
    }
  else
    {
      return true;
    }
}

void
SipElement::SetTransactionState (TransactionId id, TransactionState state)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id) << TransactionStateToString (state));
  auto transIt = m_transactions.find (id);
  NS_ABORT_MSG_IF (transIt == m_transactions.end (), "Transaction not found");
  transIt->second.m_state = state;
  m_transactionTrace (std::get<0> (id), std::get<1> (id), std::get<2> (id), state);
}

void
SipElement::CacheTransactionPacket (TransactionId id, Ptr<const Packet> p, const Address& addr, const SipHeader& hdr)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  NS_ABORT_MSG_IF (transIt == m_transactions.end (), "Transaction not found");
  transIt->second.m_packet = p->Copy ();
  transIt->second.m_address = addr;
  transIt->second.m_sipHeader = hdr;
}

Ptr<const Packet>
SipElement::GetTransactionPacket (TransactionId id)
{
  auto transIt = m_transactions.find (id);
  NS_ABORT_MSG_IF (transIt == m_transactions.end (), "Transaction not found");
  return transIt->second.m_packet;
}

void
SipElement::FreeTransactionPacket (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  if (transIt != m_transactions.end ())
    {
      transIt->second.m_packet = 0;
    }
}

void
SipElement::ScheduleTimerA (TransactionId id, uint32_t backoff)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id) << backoff);
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  transIt->second.m_timerA.SetFunction (&SipElement::HandleTimerA, this);
  transIt->second.m_timerA.SetArguments (id, backoff);
  transIt->second.m_timerA.Schedule (backoff * m_t1);
}

void
SipElement::CancelTimerA (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  transIt->second.m_timerA.Cancel ();
}

void
SipElement::ScheduleTimerB (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  transIt->second.m_timerB.SetFunction (&SipElement::HandleTimerB, this);
  transIt->second.m_timerB.SetArguments (id);
  transIt->second.m_timerB.Schedule (64 * m_t1);
}

void
SipElement::CancelTimerB (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  transIt->second.m_timerB.Cancel ();
}

void
SipElement::ScheduleTimerE (TransactionId id, uint32_t backoff)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id) << backoff);
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  transIt->second.m_timerE.SetFunction (&SipElement::HandleTimerE, this);
  transIt->second.m_timerE.SetArguments (id, backoff);
  transIt->second.m_timerE.Schedule (backoff * m_t1);
}

void
SipElement::CancelTimerE (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  transIt->second.m_timerE.Cancel ();
}

void
SipElement::ScheduleTimerF (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  transIt->second.m_timerF.SetFunction (&SipElement::HandleTimerF, this);
  transIt->second.m_timerF.SetArguments (id);
  transIt->second.m_timerF.Schedule (64 * m_t1);
}

void
SipElement::CancelTimerF (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  transIt->second.m_timerF.Cancel ();
}

void
SipElement::ScheduleTimerI (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  NS_ASSERT_MSG (transIt->second.m_state == TRANSACTION_CONFIRMED, "Transaction not in CONFIRMED");
  transIt->second.m_timerI.SetFunction (&SipElement::HandleTimerI, this);
  transIt->second.m_timerI.SetArguments (id);
  if (m_reliableTransport)
    {
      transIt->second.m_timerI.Schedule (Seconds (0));
    }
  else
    {
      transIt->second.m_timerI.Schedule (m_t4);
    }
}

void
SipElement::ScheduleTimerJ (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  NS_ASSERT_MSG (transIt->second.m_state == TRANSACTION_COMPLETED, "Transaction not in COMPLETED");
  transIt->second.m_timerJ.SetFunction (&SipElement::HandleTimerJ, this);
  transIt->second.m_timerJ.SetArguments (id);
  if (m_reliableTransport)
    {
      transIt->second.m_timerJ.Schedule (Seconds (0));
    }
  else
    {
      transIt->second.m_timerJ.Schedule (64 * m_t1);
    }
}

void
SipElement::ScheduleTimerK (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  NS_ASSERT_MSG (transIt->second.m_state == TRANSACTION_COMPLETED, "Transaction not in COMPLETED");
  transIt->second.m_timerK.SetFunction (&SipElement::HandleTimerK, this);
  transIt->second.m_timerK.SetArguments (id);
  if (m_reliableTransport)
    {
      transIt->second.m_timerK.Schedule (Seconds (0));
    }
  else
    {
      transIt->second.m_timerK.Schedule (m_t4);
    }
}

void
SipElement::HandleTimerA (TransactionId id, uint32_t backoff)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id) << backoff);
  auto eventIt = m_eventCallbacks.find (std::get<0> (id));
  NS_ASSERT_MSG (eventIt != m_eventCallbacks.end (), "CallID not found");
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  NS_ASSERT_MSG (transIt->second.m_state == TRANSACTION_CALLING, "Transaction not in CALLING");
  eventIt->second (TIMER_A_EXPIRED, transIt->second.m_state);
  // Resend the cached packet
  transIt->second.m_sendCallback (transIt->second.m_packet, transIt->second.m_address, transIt->second.m_sipHeader);
  // Double the backoff as a multiplier of T1, and reschedule
  backoff = backoff << 1;
  ScheduleTimerA (id, backoff);
}

void
SipElement::HandleTimerB (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  DialogId did = GetDialogId (std::get<0> (id), std::get<1> (id), std::get<2> (id));
  auto eventIt = m_eventCallbacks.find (std::get<0> (id));
  NS_ASSERT_MSG (eventIt != m_eventCallbacks.end (), "CallID not found");
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  NS_ASSERT_MSG (transIt->second.m_state == TRANSACTION_CALLING, "Transaction not in CALLING");
  // Cancel timer A and fail the transaction
  CancelTimerA (id);
  SetTransactionState (id, TRANSACTION_FAILED);
  SetDialogState (did, DIALOG_TERMINATED);
  // Notify user that Timer B expired
  eventIt->second (TIMER_B_EXPIRED, transIt->second.m_state);
}

void
SipElement::HandleTimerE (TransactionId id, uint32_t backoff)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id) << backoff);
  auto eventIt = m_eventCallbacks.find (std::get<0> (id));
  NS_ASSERT_MSG (eventIt != m_eventCallbacks.end (), "CallID not found");
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  NS_ASSERT_MSG (transIt->second.m_state == TRANSACTION_TRYING, "Transaction not in TRYING");
  eventIt->second (TIMER_E_EXPIRED, transIt->second.m_state);
  // Resend the cached packet
  transIt->second.m_sendCallback (transIt->second.m_packet, transIt->second.m_address, transIt->second.m_sipHeader);
  // Double the backoff as a multiplier of T1, and reschedule
  backoff = backoff << 1;
  ScheduleTimerE (id, backoff);
}

void
SipElement::HandleTimerF (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto eventIt = m_eventCallbacks.find (std::get<0> (id));
  NS_ASSERT_MSG (eventIt != m_eventCallbacks.end (), "CallID not found");
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  NS_ASSERT_MSG (transIt->second.m_state == TRANSACTION_TRYING, "Transaction not in TRYING");
  eventIt->second (TIMER_F_EXPIRED, transIt->second.m_state);
  // Cancel timer E and fail the transaction
  CancelTimerE (id);
  SetTransactionState (id, TRANSACTION_FAILED);
}

void
SipElement::HandleTimerI (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto eventIt = m_eventCallbacks.find (std::get<0> (id));
  NS_ASSERT_MSG (eventIt != m_eventCallbacks.end (), "CallID not found");
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  NS_ASSERT_MSG (transIt->second.m_state == TRANSACTION_CONFIRMED, "Transaction not in CONFIRMED");
  eventIt->second (TIMER_I_EXPIRED, transIt->second.m_state);
  SetTransactionState (id, TRANSACTION_TERMINATED);
}

void
SipElement::HandleTimerJ (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto eventIt = m_eventCallbacks.find (std::get<0> (id));
  NS_ASSERT_MSG (eventIt != m_eventCallbacks.end (), "CallID not found");
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  NS_ASSERT_MSG (transIt->second.m_state == TRANSACTION_COMPLETED, "Transaction not in COMPLETED");
  eventIt->second (TIMER_J_EXPIRED, transIt->second.m_state);
  SetTransactionState (id, TRANSACTION_TERMINATED);
}

void
SipElement::HandleTimerK (TransactionId id)
{
  NS_LOG_FUNCTION (this << TransactionIdToString (id));
  auto eventIt = m_eventCallbacks.find (std::get<0> (id));
  NS_ASSERT_MSG (eventIt != m_eventCallbacks.end (), "CallID not found");
  auto transIt = m_transactions.find (id);
  NS_ASSERT_MSG (transIt != m_transactions.end (), "Transaction not found");
  NS_ASSERT_MSG (transIt->second.m_state == TRANSACTION_COMPLETED, "Transaction not in COMPLETED");
  eventIt->second (TIMER_K_EXPIRED, transIt->second.m_state);
  SetTransactionState (id, TRANSACTION_TERMINATED);
}

std::unordered_map<SipElement::DialogId, SipElement::Dialog, SipElement::TupleHash>&
SipElement::GetDialogs (void)
{
  return m_dialogs;
}

std::unordered_map<SipElement::TransactionId, SipElement::Transaction, SipElement::TupleHash>&
SipElement::GetTransactions (void)
{
  return m_transactions;
}

std::unordered_map<uint16_t, Callback<void, const char*, SipElement::TransactionState> >&
SipElement::GetEventCallbacks (void)
{
  return m_eventCallbacks;
}

std::unordered_map<uint16_t, Callback<void, Ptr<Packet>, const SipHeader&, SipElement::TransactionState> >&
SipElement::GetReceiveCallbacks (void)
{
  return m_receiveCallbacks;
}

std::ostream& operator<< (std::ostream& os, const SipElement::DialogState& state)
{
  os << static_cast<uint16_t> (state);
  return os;
}

std::ostream& operator<< (std::ostream& os, const SipElement::TransactionState& state)
{
  os << static_cast<uint16_t> (state);
  return os;
}

} // namespace sip

} // namespace ns3

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

#ifndef SIP_PROXY_H
#define SIP_PROXY_H

#include <unordered_map>
#include <functional>
#include <tuple>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/packet.h>
#include <ns3/type-id.h>
#include <ns3/traced-callback.h>
#include <ns3/callback.h>

namespace ns3 {

class Socket;

namespace sip {

class SipHeader;

/**
 * \ingroup sip
 *
 * A SipProxy notionally represents a SIP Proxy on a server.  The model
 * does not distinguish between different variants of SIP proxies.
 * The SipProxy is the peer to the client-based SipAgent, and exists
 * primarily to manage transactions and dialogs for one or more calls.
 *
 * A SipProxy is not an ns3::Application but is instead owned by another
 * application.  It is neither started nor initialized like an
 * ns3::Application.  Other applications can request the SipProxy
 * to initiate transactions (requests and responses) and when the
 * transaction is completed (or if it fails), the client is notified
 * via a callback.  The SipProxy also sends out its packets over
 * a callback provided by the client.   Finally, the SipProxy receives
 * packets from an underlying channel via the Receive() method.
 */
class SipProxy : public Object
{
public:
  /**
   * \brief Construct a SIP proxy
   */
  SipProxy ();
  /**
   * \brief Destructor
   */
  virtual ~SipProxy ();
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * The states representing the progression of a SIP transaction
   * (based on Figures 7 and 8 of RFC 3261).
   * Invite-based:  IDLE->PROCEEDING->COMPLETED->CONFIRMED->TERMINATED
   * non-Invite-based:  IDLE->TRYING->PROCEEDING->COMPLETED->TERMINATED
   */
  enum class TransactionState
  {
    IDLE, // not in RFC 3261
    CALLING,
    TRYING,
    PROCEEDING,
    COMPLETED,
    CONFIRMED,
    TERMINATED,
    FAILED
  };
  /**
   * The states representing the progression of a SIP dialog
   * (based on Figure 3 of RFC 4235).
   */
  enum class DialogState
  {
    UNINITIALIZED,  // not in RFC 4235
    TRYING,
    PROCEEDING,
    EARLY,
    CONFIRMED,
    TERMINATED
  };
  /**
   * Return string corresponding to TransactionState value
   * \param state the state
   * \return string value for the state
   */
  static std::string TransactionStateToString (TransactionState state);
  /**
   * Return string corresponding to DialogState value
   * \param state the state
   * \return string value for the state
   */
  static std::string DialogStateToString (DialogState state);
  /**
   * \brief Start a transaction with a SIP INVITE
   *
   * \param p the packet to add a SIP header to
   * \param addr the address to send the packet to
   * \param requestUri the request URI
   * \param from the from field
   * \param to the to field
   * \param callId the callId
   * \param sendCallback the callback to use to send the messages
   */
  void SendInvite (Ptr<Packet> p, const Address& addr, uint32_t requestUri, uint32_t from, uint32_t to, uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback);
  /**
   * \brief Start a transaction with a SIP BYE
   *
   * \param p the packet to add a SIP header to
   * \param addr the address to send the packet to
   * \param requestUri the request URI
   * \param from the from field
   * \param to the to field
   * \param callId the callId
   * \param sendCallback the callback to use to send the messages
   */
  void SendBye (Ptr<Packet> p, const Address& addr, uint32_t requestUri, uint32_t from, uint32_t to, uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback);
  /**
   * \brief Send a SIP response with status code
   *
   * \param p the packet to add a SIP header to
   * \param addr the address to send the packet to
   * \param statusCode the status code
   * \param from the from field
   * \param to the to field
   * \param callId the callId
   * \param sendCallback the callback to use to send the messages
   */
  void SendResponse (Ptr<Packet> p, const Address& addr, uint16_t statusCode, uint32_t from, uint32_t to, uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback);
  /**
   * Receive and process an inbound SIP packet.  The SIP header should be the
   * next header to be deserialized.
   * \param p Packet (with SIP header) received
   * \param from IPv4 or IPv6 source address of packet
   */
  void Receive (Ptr<Packet> p, Address from);
  /**
   * Set a receive callback for receiving a packet on a specific call ID
   * and an event callback for notifying the server about events.
   *
   * This method can only be called once per call ID
   *
   * \param callId the call ID
   * \param receiveCallback the receive callback
   * \param eventCallback the event callback
   */
  void SetServerCallbacks (uint16_t callId, Callback<void, Ptr<Packet>, const SipHeader&, TransactionState> receiveCallback, Callback<void, const char*, TransactionState> eventCallback);
  /**
   * Set a default send callback.  This may be needed to respond to
   * incoming packets with a SIP response (such as 100 Trying) before
   * the process using SIP calls one of the methods that bundles a
   * send callback.
   *
   * \param sendCallback the send callback
   */
  void SetDefaultSendCallback (Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback);
  /**
   * TracedCallback signature for transmission and reception events
   * \param [in] p packet (with SIP header)
   * \param [in] s SIP header
   */
  typedef void (*TxRxTracedCallback)(Ptr<Packet> p, const SipHeader& s);

  /**
   * TracedCallback signature for dialog state events
   * \param [in] callId call ID
   * \param [in] state DialogState
   */
  typedef void (*DialogStateTracedCallback)(uint16_t callId, DialogState state)
;

  /**
   * TracedCallback signature for transaction state events
   * \param [in] callId call ID
   * \param [in] from From URI
   * \param [in] to To URI
   * \param [in] state DialogState
   */
  typedef void (*TransactionStateTracedCallback)(uint16_t callId, uint32_t from
, uint32_t to, TransactionState state);

  /**
   * Events to report; defined as C-style string literals
   */
 static constexpr const char* ACK_RECEIVED = "ACK received";
 static constexpr const char* TRYING_RECEIVED = "Trying received";

protected:
  void DoDispose (void);

private:
  /**
   * Structure to hold agent state regarding a dialog
   */
  struct Dialog
  {
    Dialog (uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback, DialogState state)
      : m_callId (callId),
        m_sendCallback (sendCallback),
        m_state (state)
    {}
    uint16_t m_callId;
    Callback<void, Ptr<Packet>, const Address&, const SipHeader&> m_sendCallback;
    DialogState m_state;
  };
  /**
   * Create a new Dialog object and store it in the internal map, with call ID
   * as a key.  If a Dialog already exists for the call ID, the program 
   * terminates.  The Dialog state will be set to UNINITIALIZED.
   *
   * \param callId Call ID
   * \param sendCallback Send Callback to use during the dialog
   */
  void CreateDialog (uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback);

  /**
   * Check if a dialog object exists for the call ID
   * \param callId Call ID
   * \return true if Dialog exists, false otherwise
   */
  bool DialogExists (uint16_t callId) const;

  /**
   * Set the Dialog state and notify the trace source.  If a Dialog for the
   * call ID does not exist, the program terminates.
   *
   * \param callId Call ID
   * \param state Dialog state
   */
  void SetDialogState (uint16_t callId, DialogState state);

  /**
   * Hash for std::unordered_map<std::tuple<uint16_t, uint32_t, uint32_t>, TransactionState>
   */
  struct TupleHash
  {
    std::size_t operator () (const std::tuple<uint16_t, uint32_t, uint32_t> &tuple) const
    {
      // shift uint16_t left by 16 bits and OR it with second value shifted
      // by 8 bits and OR with third value
      // Collisions should be avoided if the input values are close to zero
      return std::hash<std::size_t> () (static_cast<uint32_t> ((std::get<0> (tuple) << 16) | (std::get<1> (tuple) << 8) | std::get<2> (tuple)));
    }
  };
  /**
   * Create a new Transaction state entry and store it in the internal map,
   * with call ID as a key.  If a Transaction already exists, it is reused.
   * The Transaction state will be set to IDLE.
   *
   * \param callId Call ID
   * \param from From URI
   * \param to To URI
   */
  void CreateTransaction (uint16_t callId, uint32_t from, uint32_t to);
  
  /**
   * Check if a transaction object exists for the call ID and URIs
   * \param callId Call ID
   * \param from from URI
   * \param to to URI
   * \return true if Transaction exists, false otherwise
   */
  bool TransactionExists (uint16_t callId, uint32_t from, uint32_t to) const;

  /**
   * Set the Transaction state and notify the trace source.  If a Transaction
   * for the call ID does not exist, the program terminates.
   *
   * \param callId Call ID
   * \param from From URI
   * \param to To URI
   * \param state Dialog state
   */
  void SetTransactionState (uint16_t callId, uint32_t from, uint32_t to, TransactionState state);

  std::unordered_map<uint16_t, Dialog> m_dialogs;
  std::unordered_map<std::tuple<uint16_t, uint32_t, uint32_t>, TransactionState, TupleHash> m_transactions;
  std::unordered_map<uint16_t, Callback<void, Ptr<Packet>, const SipHeader&, TransactionState> > m_receiveCallbacks;
  std::unordered_map<uint16_t, Callback<void, const char*, TransactionState> > m_eventCallbacks;
  Callback<void, Ptr<Packet>, const Address&, const SipHeader&> m_defaultSendCallback;
  TracedCallback<Ptr<Packet>, const SipHeader&>  m_rxTrace;
  TracedCallback<Ptr<Packet>, const SipHeader&>  m_txTrace;
  TracedCallback<uint16_t, DialogState> m_dialogTrace;
  TracedCallback<uint16_t, uint32_t, uint32_t, TransactionState> m_transactionTrace;
};

std::ostream& operator << (std::ostream& os, const SipProxy::DialogState& state);

std::ostream& operator << (std::ostream& os, const SipProxy::TransactionState& state);

} // namespace sip

} // namespace ns3

#endif /* SIP_PROXY_H */

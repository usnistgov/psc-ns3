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

#ifndef SIP_ELEMENT_H
#define SIP_ELEMENT_H

#include <unordered_map>
#include <tuple>
#include <ns3/object.h>
#include <ns3/address.h>
#include <ns3/ptr.h>
#include <ns3/packet.h>
#include <ns3/type-id.h>
#include <ns3/traced-callback.h>
#include <ns3/callback.h>
#include <ns3/timer.h>
#include "sip-header.h"

namespace ns3 {

namespace sip {

/**
 * \ingroup sip
 *
 * A SipElement represents the base 'SIP element' defined in RFC 3261
 * upon which user agent clients and servers, stateless and stateful proxies,
 * and registrars are built.  In ns-3, it is the base class for
 * end systems (ns3::SipAgent) and proxies (ns3::SipProxy), implementing
 * the generic aspects of managing transactions and dialogs for one
 * or more calls.
 *
 * A SipElement is not an ns3::Application but is instead owned by another
 * application.  It is neither started nor initialized like an
 * ns3::Application.  Other applications can request the SipElement
 * to initiate transactions (requests and responses) and when the
 * transaction is completed (or if it fails), the client is notified
 * via a callback.  The SipElement also sends out its packets over
 * a callback provided by the client.   Finally, the SipElement receives
 * packets from an underlying channel via the Receive() method.
 */
class SipElement : public Object
{
public:
  /**
   * \brief Construct a SIP element
   */
  SipElement ();
  /**
   * \brief Destructor
   */
  virtual ~SipElement ();
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * The states representing the progression of a SIP transaction
   * (based on Figure 5 of RFC 3261).  This is a plain enum rather
   * than an enum class due to a limitation in PyBindGen.
   */
  enum TransactionState
  {
    TRANSACTION_IDLE,
    TRANSACTION_CALLING, // Initial client state for INVITE transaction
    TRANSACTION_TRYING,  // Initial client and server state, non-INVITE transaction
    TRANSACTION_PROCEEDING, // Server send 100, client receipt of 100
    TRANSACTION_COMPLETED,
    TRANSACTION_CONFIRMED,
    TRANSACTION_TERMINATED,
    TRANSACTION_FAILED
  };
  /**
   * The states representing the progression of a SIP dialog
   * (based on Figure 3 of RFC 4235).  This is a plain enum rather
   * than an enum class due to a limitation in PyBindGen.
   */
  enum DialogState
  {
    DIALOG_UNINITIALIZED,  // not in RFC 4235
    DIALOG_TRYING,  // entered before send or receipt of 100 Trying
    DIALOG_PROCEEDING, // entered after send or receipt of 100 Trying
    DIALOG_EARLY,  // not presently used
    DIALOG_CONFIRMED, // entered after send or receipt of 200 OK
    DIALOG_TERMINATED // entered after send or receipt of BYE
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
   * \brief Start a transaction to send a SIP INVITE
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
   * \brief Start a transaction to send a SIP BYE
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
  virtual void SendResponse (Ptr<Packet> p, const Address& addr, uint16_t statusCode, uint32_t from, uint32_t to, uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback);
  /**
   * Receive and process an inbound SIP packet.  The SIP header should be the
   * next header ready to be deserialized.
   * \param p Packet (with SIP header) received
   * \param from IPv4 or IPv6 source address of packet
   */
  void Receive (Ptr<Packet> p, Address from);
  /**
   * Set a receive callback for receiving a packet on a specific call ID
   * and an event callback for notifying the user about events.
   *
   * This method can only be called once per call ID
   *
   * \param callId the call ID
   * \param receiveCallback the receive callback
   * \param eventCallback the event callback
   */
  void SetCallbacks (uint16_t callId, Callback<void, Ptr<Packet>, const SipHeader&, TransactionState> receiveCallback, Callback<void, const char*, TransactionState> eventCallback);
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
   * \param [in] p packet received (without SIP header)
   * \param [in] s SIP header received
   */
  typedef void (*TxRxTracedCallback)(Ptr<Packet> p, const SipHeader& s);

  /**
   * TracedCallback signature for dialog state events
   * \param [in] callId call ID
   * \param [in] lowUri lower of the two URI
   * \param [in] highUri higher of the two URI
   * \param [in] state DialogState
   */
  typedef void (*DialogStateTracedCallback)(uint16_t callId, uint32_t lowUri, uint32_t highUri, DialogState state);

  /**
   * TracedCallback signature for transaction state events
   * \param [in] callId call ID
   * \param [in] from From URI
   * \param [in] to To URI
   * \param [in] state DialogState
   */
  typedef void (*TransactionStateTracedCallback)(uint16_t callId, uint32_t from, uint32_t to, TransactionState state);

  /**
   * Events to report; defined as C-style string literals
   */
  static constexpr const char* ACK_RECEIVED = "ACK received";
  static constexpr const char* TRYING_RECEIVED = "Trying received";
  static constexpr const char* REQUEST_TIMEOUT = "Request timeout";
  static constexpr const char* TIMER_A_EXPIRED = "Timer A expired";
  static constexpr const char* TIMER_B_EXPIRED = "Timer B expired";
  static constexpr const char* TIMER_C_EXPIRED = "Timer C expired";
  static constexpr const char* TIMER_E_EXPIRED = "Timer E expired";
  static constexpr const char* TIMER_F_EXPIRED = "Timer F expired";
  static constexpr const char* TIMER_I_EXPIRED = "Timer I expired";
  static constexpr const char* TIMER_J_EXPIRED = "Timer J expired";
  static constexpr const char* TIMER_K_EXPIRED = "Timer K expired";

protected:
  void DoDispose (void);

  /**
   * Dialog ID is a tuple with an ordered list of callId, URI(low), URI(high)
   * where low < high.  'low' and 'high' correspond to the From and To fields
   */
  typedef std::tuple<uint16_t, uint32_t, uint32_t> DialogId;
  /**
   * Return string corresponding to DialogId tuple
   * \param id the DialogId
   * \return string value for the DialogId
   */
  std::string DialogIdToString (DialogId id) const;
  /**
   * Return a properly formatted DialogId (a std::tuple) from a callID
   * and two URIs
   * \param callId call ID
   * \param uriA the first URI
   * \param uriB the second URI
   * \return corresponding DialogId
   */
  DialogId GetDialogId (uint16_t callId, uint32_t uriA, uint32_t uriB) const;
  /**
   * Structure to hold state regarding a dialog
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
   * Transaction ID is a tuple with an ordered list of callId, from, to
   */
  typedef std::tuple<uint16_t, uint32_t, uint32_t> TransactionId;
  /**
   * Return string corresponding to TransactionId tuple
   * \param id the DialogId
   * \return string value for the TransactionId
   */
  std::string TransactionIdToString (TransactionId id) const;
  /**
   * Return a properly formatted TransactionId (a std::tuple) from a callID
   * and two URIs
   * \param callId call ID
   * \param from the from URI
   * \param to the to URI
   * \return corresponding TransactionId
   */
  TransactionId GetTransactionId (uint16_t callId, uint32_t from, uint32_t to) const;
  /**
   * Structure to hold state regarding a transaction
   */
  struct Transaction
  {
    Transaction (uint16_t callId, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback)
      : m_callId (callId),
        m_sendCallback (sendCallback)
    {
      m_state = TRANSACTION_IDLE;
      m_packet = 0;
    }
    uint16_t m_callId;
    Callback<void, Ptr<Packet>, const Address&, const SipHeader&> m_sendCallback;
    TransactionState m_state;
    Ptr<Packet> m_packet;
    Address m_address;
    SipHeader m_sipHeader;
    Timer m_timerA {Timer::CANCEL_ON_DESTROY}; //!< Timer A
    Timer m_timerB {Timer::CANCEL_ON_DESTROY}; //!< Timer B
    Timer m_timerC {Timer::CANCEL_ON_DESTROY}; //!< Timer C
    Timer m_timerE {Timer::CANCEL_ON_DESTROY}; //!< Timer E
    Timer m_timerF {Timer::CANCEL_ON_DESTROY}; //!< Timer F
    Timer m_timerI {Timer::CANCEL_ON_DESTROY}; //!< Timer I
    Timer m_timerJ {Timer::CANCEL_ON_DESTROY}; //!< Timer J
    Timer m_timerK {Timer::CANCEL_ON_DESTROY}; //!< Timer K
  };
  /**
   * Create a new Dialog object and store it in the internal map, with call ID
   * and URIs as a key.  If a Dialog already exists for the key, the program
   * terminates.  The Dialog state will be set to UNINITIALIZED.
   *
   * \param id Dialog ID
   * \param sendCallback Send Callback to use during the dialog
   */
  void CreateDialog (DialogId id, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback);

  /**
   * Create a new Transaction object and store it in the internal map,
   * with the tuple (call ID, from, to) as a key.  If a Transaction
   * already exists, it is reused.  The Transaction state will be set to IDLE.
   *
   * \param id Transaction ID
   * \param sendCallback Send Callback to use during the dialog
   */
  void CreateTransaction (TransactionId id, Callback<void, Ptr<Packet>, const Address&, const SipHeader&> sendCallback);

  /**
   * Check if a dialog object exists for the call ID and peer
   * \param id Dialog ID
   * \return true if Dialog exists, false otherwise
   */
  bool DialogExists (DialogId id) const;

  /**
   * Set the Dialog state and notify the trace source.  If a Dialog for the
   * call ID and peer does not exist, the program terminates.
   *
   * \param id Dialog ID
   * \param state Dialog state
   */
  void SetDialogState (DialogId id, DialogState state);

  /**
   * Hash for std::unordered_map<std::tuple<uint16_t, uint32_t, uint32_t>, Transaction>
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
   * Check if a transaction object exists for the call ID and URIs
   * \param id Transaction ID
   * \return true if Transaction exists, false otherwise
   */
  bool TransactionExists (TransactionId id) const;

  /**
   * Set the Transaction state and notify the trace source.  If a Transaction
   * for the call ID does not exist, the program terminates.
   *
   * \param id Transaction ID
   * \param state Dialog state
   */
  void SetTransactionState (TransactionId id, TransactionState state);

  /**
   * Cache a Ptr<Packet> to use for any transaction retransmissions
   * with a tuple of (call ID, from, to) as a key.  This method will store
   * a deep copy of the provided Packet pointer.
   *
   * \param id TransactionId
   * \param pkt Ptr<const Packet> to cache
   * \param addr Address to cache
   * \param hdr SipHeader to cache
   */
  void CacheTransactionPacket (TransactionId id, Ptr<const Packet> pkt, const Address& addr, const SipHeader& hdr);

  /**
   * Retrieve a previously cached transaction packet for use in retransmission.
   * The client is responsible for performing a deep copy (Packet::Copy()) of
   * the returned pointer to const Packet.
   *
   * \param id TransactionId
   * \return pointer to transaction packet
   */
  Ptr<const Packet> GetTransactionPacket (TransactionId id);

  /**
   * Free the previously cached transaction packet.
   *
   * \param id TransactionId
   */
  void FreeTransactionPacket (TransactionId id);

  /**
   * Schedule Timer A (retransmit the INVITE)
   * \param id TransactionId
   * \param backoff Value to use for the T1 multiplier
   */
  void ScheduleTimerA (TransactionId id, uint32_t backoff);
  /**
   * Schedule Timer B (failure of INVITE to complete)
   * \param id Transaction ID
   */
  void ScheduleTimerB (TransactionId id);
  /**
   * Schedule Timer E (retransmit the non-INVITE)
   * \param id TransactionId
   * \param backoff Value to use for the T1 multiplier
   */
  void ScheduleTimerE (TransactionId id, uint32_t backoff);
  /**
   * Schedule Timer F (failure of non-INVITE to complete)
   * \param id Transaction ID
   */
  void ScheduleTimerF (TransactionId id);
  /**
   * Schedule Timer I (INVITE server transaction, absorb further ACKs)
   * \param id Transaction ID
   */
  void ScheduleTimerI (TransactionId id);
  /**
   * Schedule Timer J (non-INVITE server transaction)
   * \param id Transaction ID
   */
  void ScheduleTimerJ (TransactionId id);
  /**
   * Schedule Timer K (non-INVITE client transaction)
   * \param id Transaction ID
   */
  void ScheduleTimerK (TransactionId id);
  /**
   * \brief Cancel Timer A
   * \param id Transaction ID
   */
  void CancelTimerA (TransactionId id);
  /**
   * \brief Cancel Timer B
   * \param id Transaction ID
   */
  void CancelTimerB (TransactionId id);
  /**
   * \brief Cancel Timer E
   * \param id Transaction ID
   */
  void CancelTimerE (TransactionId id);
  /**
   * \brief Cancel Timer F
   * \param id Transaction ID
   */
  void CancelTimerF (TransactionId id);
  /**
   * Handle Timer A expiry (retransmit the INVITE)
   * \param id Transaction ID
   * \param backoff Last value used for the T1 multiplier
   */
  void HandleTimerA (TransactionId id, uint32_t backoff);
  /**
   * Handle Timer B expiry (failure of INVITE transaction)
   * \param id Transaction ID
   */
  void HandleTimerB (TransactionId id);
  /**
   * Handle Timer E expiry (retransmit the non-INVITE)
   * \param id Transaction ID
   * \param backoff Last value used for the T1 multiplier
   */
  void HandleTimerE (TransactionId id, uint32_t backoff);
  /**
   * Handle Timer F expiry (failure of non-INVITE transaction)
   * \param id Transaction ID
   */
  void HandleTimerF (TransactionId id);
  /**
   * Handle Timer I expiry (move state from CONFIRMED to TERMINATED)
   * \param id Transaction ID
   */
  void HandleTimerI (TransactionId id);
  /**
   * Handle Timer J expiry (server move state from COMPLETED to TERMINATED)
   * \param id Transaction ID
   */
  void HandleTimerJ (TransactionId id);
  /**
   * Handle Timer K expiry (client move state from COMPLETED to TERMINATED)
   * \param id Transaction ID
   */
  void HandleTimerK (TransactionId id);

  // Accessors for private members; these are exposed as non-const accessors
  // rather than simply making the member variables protected because
  // PyBindGen does not support protected member variables of this type
  std::unordered_map<DialogId, Dialog, TupleHash>& GetDialogs (void);
  std::unordered_map<TransactionId, Transaction, TupleHash>& GetTransactions (void);
  std::unordered_map<uint16_t, Callback<void, const char*, TransactionState> >& GetEventCallbacks (void);
  std::unordered_map<uint16_t, Callback<void, Ptr<Packet>, const SipHeader&, TransactionState> >& GetReceiveCallbacks (void);

  // PyBindGen does not complain about exposing the below as protected

  TracedCallback<Ptr<Packet>, const SipHeader&>  m_txTrace; //!< Trace send
  TracedCallback<Ptr<Packet>, const SipHeader&>  m_rxTrace; //!< Trace receive
  TracedCallback<uint16_t, uint32_t, uint32_t, DialogState> m_dialogTrace; //!< Trace dialog state change
  TracedCallback<uint16_t, uint32_t, uint32_t, TransactionState> m_transactionTrace; //!< Trace transaction state change

private:
  std::unordered_map<DialogId, Dialog, TupleHash> m_dialogs;
  std::unordered_map<TransactionId, Transaction, TupleHash> m_transactions;
  std::unordered_map<uint16_t, Callback<void, Ptr<Packet>, const SipHeader&, TransactionState> > m_receiveCallbacks;
  std::unordered_map<uint16_t, Callback<void, const char*, TransactionState> > m_eventCallbacks;
  Callback<void, Ptr<Packet>, const Address&, const SipHeader&> m_defaultSendCallback;

  bool m_reliableTransport; //!< reliable transport flag
  // timers
  Time m_t1; //!< Time T1 (RTT estimate)
  Time m_t2; //!< Time T2 (Max retransmit interval for non-INVITE requests and INVITE responses)
  Time m_t4; //!< Time T4 (Max duration a message will remain in the network)
};

std::ostream& operator<< (std::ostream& os, const SipElement::DialogState& state);
std::ostream& operator<< (std::ostream& os, const SipElement::TransactionState& state);

} // namespace sip

} // namespace ns3

#endif /* SIP_ELEMENT_H */

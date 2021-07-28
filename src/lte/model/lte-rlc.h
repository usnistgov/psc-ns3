/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 *
 * Author: Nicola Baldo <nbaldo@cttc.es>
 * Modified by: CTTC for NR Sidelink
 */

#ifndef LTE_RLC_H
#define LTE_RLC_H

#include <ns3/simple-ref-count.h>
#include <ns3/packet.h>
#include "ns3/uinteger.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/nstime.h"

#include "ns3/object.h"

#include "ns3/lte-rlc-sap.h"
#include "ns3/lte-mac-sap.h"
#include "ns3/nr-sl-mac-sap.h"
#include "ns3/nr-sl-rlc-sap.h"

namespace ns3 {


// class LteRlcSapProvider;
// class LteRlcSapUser;
// 
// class LteMacSapProvider;
// class LteMacSapUser;

/**
 * This abstract base class defines the API to interact with the Radio Link Control
 * (LTE_RLC) in LTE, see 3GPP TS 36.322
 *
 */
class LteRlc : public Object // SimpleRefCount<LteRlc>
{
  /// allow LteRlcSpecificLteMacSapUser class friend access
  friend class LteRlcSpecificLteMacSapUser;
  /// allow LteRlcSpecificLteRlcSapProvider<LteRlc> class friend access
  friend class LteRlcSpecificLteRlcSapProvider<LteRlc>;
  //NR Sidelink
  /// let the forwarder class access the protected and private members
  friend class MemberNrSlMacSapUser<LteRlc>;
  /// let the forwarder class access the protected and private members
  friend class MemberNrSlRlcSapProvider<LteRlc>;
public:
  LteRlc ();
  virtual ~LteRlc ();
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  /**
   *
   *
   * \param rnti
   */
  void SetRnti (uint16_t rnti);

  /**
   *
   *
   * \param lcId
   */
  void SetLcId (uint8_t lcId);

  /**
   *
   *
   * \param s the RLC SAP user to be used by this LTE_RLC
   */
  void SetLteRlcSapUser (LteRlcSapUser * s);

  /**
   *
   *
   * \return the RLC SAP Provider interface offered to the PDCP by this LTE_RLC
   */
  LteRlcSapProvider* GetLteRlcSapProvider ();

  /**
   *
   *
   * \param s the MAC SAP Provider to be used by this LTE_RLC
   */
  void SetLteMacSapProvider (LteMacSapProvider * s);

  /**
   *
   *
   * \return the MAC SAP User interface offered to the MAC by this LTE_RLC
   */
  LteMacSapUser* GetLteMacSapUser ();


  /**
   * TracedCallback signature for NotifyTxOpportunity events.
   *
   * \param [in] rnti C-RNTI scheduled.
   * \param [in] lcid The logical channel id corresponding to
   *             the sending RLC instance.
   * \param [in] bytes The number of bytes to transmit
   */
  typedef void (* NotifyTxTracedCallback)
    (uint16_t rnti, uint8_t lcid, uint32_t bytes);

  /**
   * TracedCallback signature for
   *
   * \param [in] rnti C-RNTI scheduled.
   * \param [in] lcid The logical channel id corresponding to
   *             the sending RLC instance.
   * \param [in] bytes The packet size.
   * \param [in] delay Delay since sender timestamp, in ns.
   */
  typedef void (* ReceiveTracedCallback)
    (uint16_t rnti, uint8_t lcid, uint32_t bytes, uint64_t delay);

  /// \todo MRE What is the sense to duplicate all the interfaces here???
  // NB to avoid the use of multiple inheritance
  
protected:
  // Interface forwarded by LteRlcSapProvider
  /**
   * Transmit PDCP PDU
   * 
   * \param p packet
   */
  virtual void DoTransmitPdcpPdu (Ptr<Packet> p) = 0;

  LteRlcSapUser* m_rlcSapUser; ///< RLC SAP user
  LteRlcSapProvider* m_rlcSapProvider; ///< RLC SAP provider

  // Interface forwarded by LteMacSapUser
  /**
   * Notify transmit opportunity
   *
   * \param params LteMacSapUser::TxOpportunityParameters
   */ 
  virtual void DoNotifyTxOpportunity (LteMacSapUser::TxOpportunityParameters params) = 0;
  /**
   * Notify HARQ delivery failure
   */ 
  virtual void DoNotifyHarqDeliveryFailure () = 0;
  /**
   * Receive PDU function
   *
   * \param params the LteMacSapUser::ReceivePduParameters
   */ 
  virtual void DoReceivePdu (LteMacSapUser::ReceivePduParameters params) = 0;

  LteMacSapUser* m_macSapUser; ///< MAC SAP user
  LteMacSapProvider* m_macSapProvider; ///< MAC SAP provider

  uint16_t m_rnti; ///< RNTI
  uint8_t m_lcid; ///< LCID

  /**
   * Used to inform of a PDU delivery to the MAC SAP provider
   */
  TracedCallback<uint16_t, uint8_t, uint32_t> m_txPdu;
  /**
   * Used to inform of a PDU reception from the MAC SAP user
   */
  TracedCallback<uint16_t, uint8_t, uint32_t, uint64_t> m_rxPdu;
  /**
   * The trace source fired when the RLC drops a packet before
   * transmission.
   */
  TracedCallback<Ptr<const Packet> > m_txDropTrace;

 //Sidelink
public:
  /// Channel type enumeration
  enum ChannelType
  {
    DEFAULT = 0,
    STCH
  };

  /**
   * \brief Set the NR Sidelik MAC SAP offered by MAC to RLC
   *
   * \param s the NR Sidelik MAC SAP provider interface offered by
   *          MAC by this RLC
   */
  void SetNrSlMacSapProvider (NrSlMacSapProvider * s);

  /**
   * \brief Get the NR Sidelik MAC SAP offered by this RLC
   *
   * \return the NR Sidelik MAC SAP user interface offered to the
   *         MAC by this RLC
   */
  NrSlMacSapUser* GetNrSlMacSapUser ();

  /**
   * \brief Get the NR Sidelik SAP offered by RLC to PDCP
   *
   * \return the NR Sidelink RLC SAP Provider interface offered by RLC to PDCP
   */
  NrSlRlcSapProvider* GetNrSlRlcSapProvider ();

  /**
   * \brief Set the NR Sidelik SAP offered by PDCP to RLC
   *
   * \param s the NR Sidelink SAP user interface offered by PDCP to RLC
   */
  void SetNrSlRlcSapUser (NrSlRlcSapUser* s);

  /**
   * \brief Set the RLC logical channel type
   *
   * Currently this method is only used for a
   * Sidelink logical channel.
   *
   * \param channelType the logical channel type
   */
  void SetRlcChannelType (LteRlc::ChannelType channelType);

  /**
   * \brief Sets the source L2 Id for sidelink identification of the RLC UM and PDCP entity
   * \param src The Sidelink source layer 2 id
   */
  void SetSourceL2Id (uint32_t src);

  /**
   * \brief Sets the destination L2 Id for sidelink identification of the RLC UM and PDCP entity
   * \param dst The Sidelink destination layer 2 id
   */
  void SetDestinationL2Id (uint32_t dst);

protected:
  /**
   * \brief Send a NR Sidelink PDCP PDU to the RLC for transmission
   *
   * This method is to be called when upper PDCP entity has a NR Sidelink PDCP
   * PDU ready to send
   *
   * \param params the NrSlTransmitPdcpPduParameters
   */
  virtual void DoTransmitNrSlPdcpPdu (const NrSlRlcSapProvider::NrSlTransmitPdcpPduParameters &params) = 0;

  /**
   * \brief Called by the MAC to notify the RLC that the scheduler granted a
   * transmission opportunity to this RLC instance.
   *
   * \param params the NrSlTxOpportunityParameters
   */
  virtual void DoNotifyNrSlTxOpportunity (const NrSlMacSapUser::NrSlTxOpportunityParameters &params) = 0;

  /**
   * \brief Called by the MAC to notify the RLC of the reception of a new PDU
   *
   * \param rxPduParams the NrSlReceiveRlcPduParameters
   */
  virtual void DoReceiveNrSlRlcPdu (NrSlMacSapUser::NrSlReceiveRlcPduParameters rxPduParams) = 0;

  NrSlMacSapProvider* m_nrSlMacSapProvider {nullptr}; //!< NR SL MAC SAP provider
  NrSlMacSapUser* m_nrSlMacSapUser; //!< NR SL MAC SAP user
  NrSlRlcSapProvider* m_nrSlRlcSapProvider; //!< SAP interface to receive calls from PDCP instance
  NrSlRlcSapUser* m_nrSlRlcSapUser {nullptr}; //!< SAP interface to call methods of PDCP instance
  uint32_t m_srcL2Id {0};  ///< Source L2 ID (24 bits)
  uint32_t m_dstL2Id {0};  ///< Destination L2 ID (24 bits)
  ChannelType m_channelType {LteRlc::DEFAULT}; ///< The logical channel type
};



/**
 * LTE_RLC Saturation Mode (SM): simulation-specific mode used for
 * experiments that do not need to consider the layers above the LTE_RLC.
 * The LTE_RLC SM, unlike the standard LTE_RLC modes, it does not provide
 * data delivery services to upper layers; rather, it just generates a
 * new LTE_RLC PDU whenever the MAC notifies a transmission opportunity.
 *
 */
class LteRlcSm : public LteRlc
{
public:
  LteRlcSm ();
  virtual ~LteRlcSm ();
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual void DoInitialize ();
  virtual void DoDispose ();

  virtual void DoTransmitPdcpPdu (Ptr<Packet> p);
  virtual void DoNotifyTxOpportunity (LteMacSapUser::TxOpportunityParameters txOpParams);
  virtual void DoNotifyHarqDeliveryFailure ();
  virtual void DoReceivePdu (LteMacSapUser::ReceivePduParameters rxPduParams);
  //NR Sidelink
protected:
  void DoTransmitNrSlPdcpPdu (const NrSlRlcSapProvider::NrSlTransmitPdcpPduParameters &params);
  void DoNotifyNrSlTxOpportunity (const NrSlMacSapUser::NrSlTxOpportunityParameters &params);
  void DoReceiveNrSlRlcPdu (NrSlMacSapUser::NrSlReceiveRlcPduParameters rxPduParams);



private:
  /// Report buffer status
  void ReportBufferStatus ();

};




// /**
//  * Implements LTE_RLC Transparent Mode (TM), see  3GPP TS 36.322
//  *
//  */
// class LteRlcTm : public LteRlc
// {
// public:
//   virtual ~LteRlcTm ();

// };


// /**
//  * Implements LTE_RLC Unacknowledged Mode (UM), see  3GPP TS 36.322
//  *
//  */
// class LteRlcUm : public LteRlc
// {
// public:
//   virtual ~LteRlcUm ();

// };

// /**
//  * Implements LTE_RLC Acknowledged Mode (AM), see  3GPP TS 36.322
//  *
//  */

// class LteRlcAm : public LteRlc
// {
// public:
//   virtual ~LteRlcAm ();
// };





} // namespace ns3

#endif // LTE_RLC_H

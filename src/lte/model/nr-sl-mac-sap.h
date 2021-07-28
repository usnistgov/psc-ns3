/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef NR_SL_MAC_SAP_H
#define NR_SL_MAC_SAP_H

#include <ns3/packet.h>

namespace ns3 {



/**
 * \ingroup lte
 *
 * Service Access Point (SAP) offered by the MAC to the RLC for NR Sidelink
 *
 * This is the NR SL MAC SAP Provider, i.e., the part of the SAP that contains
 * the MAC methods called by the RLC
 */
class NrSlMacSapProvider
{
public:
  virtual ~NrSlMacSapProvider ();

  /**
   * Parameters for NrSlMacSapProvider::TransmitNrSlRlcPdu
   *
   */
  struct NrSlRlcPduParameters
  {
    /**
     * \brief NrSlRlcPduParameters constructor
     * \param pdu Packet
     * \param rnti RNTI
     * \param lcId Logical Channel ID
     * \param layer Layer (used for MIMO)
     * \param harqProcessId HARQ process id
     * \param bwpId Bandwidth part id
     * \param srcL2Id Sidelink source L2 ID (24 bits)
     * \param dstL2Id Sidelink destination L2 ID (24 bits)
     */
    NrSlRlcPduParameters (Ptr<Packet> pdu, uint16_t rnti, uint8_t lcId,
                          uint8_t layer, uint8_t harqProcessId,
                          uint16_t bwpId, uint32_t srcL2Id,  uint32_t dstL2Id)
    {
      this->pdu = pdu;
      this->rnti = rnti;
      this->lcid = lcId;
      this->harqProcessId = harqProcessId;
      this->bwpId = bwpId;
      this->srcL2Id = srcL2Id;
      this->dstL2Id = dstL2Id;
    }

    Ptr<Packet> pdu;  //!<the RLC PDU
    uint16_t    rnti {std::numeric_limits <uint16_t>::max ()}; //!< the C-RNTI identifying the UE
    uint8_t     lcid {std::numeric_limits <uint8_t>::max ()}; //!< the logical channel id corresponding to the sending RLC instance
    uint8_t     layer {std::numeric_limits <uint8_t>::max ()}; //!< the layer value that was passed by the MAC in the call to NotifyNrSlTxOpportunity that generated this PDU
    uint8_t     harqProcessId {std::numeric_limits <uint8_t>::max ()}; //!< the HARQ process id that was passed by the MAC in the call to NotifyNrSlTxOpportunity that generated this PDU
    uint8_t bwpId {std::numeric_limits <uint8_t>::max ()}; //!< the component carrier id corresponding to the sending Mac instance */
    uint32_t srcL2Id {0};  //!< Source L2 ID (24 bits)
    uint32_t dstL2Id {0};  //!< Destination L2 ID (24 bits)
  };

  /**
   * \brief send an NR SL RLC PDU to the MAC for transmission. This method is
   * to be called as a response to NrSlMacSapUser::NotifyNrSlTxOpportunity
   *
   * \param params NrSlRlcPduParameters
   */
  virtual void TransmitNrSlRlcPdu (const NrSlRlcPduParameters &params) = 0;


  /**
   * Parameters for NrSlMacSapProvider::ReportNrSlBufferStatus
   */
  struct NrSlReportBufferStatusParameters
  {
    /**
     * \brief NrSlReportBufferStatusParameters constructor
     * \param rnti RNTI
     * \param lcId Logical Channel ID
     * \param txQueueSize The current size of the RLC transmission queue
     * \param txQueueHolDelay The Head Of Line delay of the transmission queue
     * \param retxQueueSize The current size of the RLC retransmission queue
     * \param retxQueueHolDelay The Head Of Line delay of the retransmission queue
     * \param statusPduSize The current size of the pending STATUS RLC  PDU message in bytes
     * \param srcL2Id Sidelink source L2 ID (24 bits)
     * \param dstL2Id Sidelink destination L2 ID (24 bits)
     */
    NrSlReportBufferStatusParameters (uint16_t rnti, uint8_t lcId,
                                      uint32_t txQueueSize, uint16_t txQueueHolDelay,
                                      uint32_t retxQueueSize, uint16_t retxQueueHolDelay,
                                      uint16_t statusPduSize, uint32_t srcL2Id,  uint32_t dstL2Id)
    {
      this->rnti = rnti;
      this->lcid = lcId;
      this->txQueueSize = txQueueSize;
      this->txQueueHolDelay = txQueueHolDelay;
      this->retxQueueSize = retxQueueSize;
      this->retxQueueHolDelay = retxQueueHolDelay;
      this->srcL2Id = srcL2Id;
      this->dstL2Id = dstL2Id;
    }

    uint16_t rnti {std::numeric_limits <uint16_t>::max ()};  //!< the C-RNTI identifying the UE
    uint8_t lcid {std::numeric_limits <uint8_t>::max ()};  //!< the logical channel id corresponding to the sending RLC instance
    uint32_t txQueueSize {0};  //!< the current size of the RLC transmission queue
    uint16_t txQueueHolDelay {std::numeric_limits <uint16_t>::max ()};  //!< the Head Of Line delay of the transmission queue
    uint32_t retxQueueSize {0};  //!< the current size of the RLC retransmission queue in bytes
    uint16_t retxQueueHolDelay {std::numeric_limits <uint16_t>::max ()};  //!< the Head Of Line delay of the retransmission queue
    uint16_t statusPduSize {0};  //!< the current size of the pending STATUS RLC  PDU message in bytes
    uint32_t srcL2Id {0};  //!< Source L2 ID (24 bits)
    uint32_t dstL2Id {0};  //!< Destination L2 ID (24 bits)
  };

  /**
   * \brief Report the RLC buffer status to the MAC
   *
   * \param params NrSlReportBufferStatusParameters
   */
  virtual void ReportNrSlBufferStatus (const NrSlReportBufferStatusParameters &params) = 0;
};

/**
 * \ingroup lte
 *
 * Template for the implementation of the NrSlMacSapProvider as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to UE MAC class, which are called by UE RLC
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlMacSapProvider : public NrSlMacSapProvider
{
public:
  /**
   * \brief Constructor
   *
   * \param mac the MAC class
   */
  MemberNrSlMacSapProvider (C* mac);

  // inherited from NrSlMacSapProvider
  virtual void TransmitNrSlRlcPdu (const NrSlRlcPduParameters &params);
  virtual void ReportNrSlBufferStatus (const NrSlReportBufferStatusParameters &params);

private:
  C* m_mac; ///< the MAC class
};


template <class C>
MemberNrSlMacSapProvider<C>::MemberNrSlMacSapProvider (C* mac)
  : m_mac (mac)
{
}

template <class C>
void MemberNrSlMacSapProvider<C>::TransmitNrSlRlcPdu (const NrSlRlcPduParameters &params)
{
  m_mac->DoTransmitNrSlRlcPdu (params);
}

template <class C>
void MemberNrSlMacSapProvider<C>::ReportNrSlBufferStatus (const NrSlReportBufferStatusParameters &params)
{
  m_mac->DoReportNrSlBufferStatus (params);
}



/**
 * Service Access Point (SAP) offered by the MAC to the RLC for NR Sidelink
 *
 * This is the NR Sidelink MAC SAP User, i.e., the part of the SAP that contains
 * the RLC methods called by the MAC
 */
class NrSlMacSapUser
{
public:
  virtual ~NrSlMacSapUser ();
  /**
   * Parameters for NrSlMacSapUser::NotifyNrSlTxOpportunity
   *
   */
  struct NrSlTxOpportunityParameters
  {
    /**
     * \brief NrSlTxOpportunityParameters constructor
     * \param bytes Bytes
     * \param rnti RNTI
     * \param lcId Logical Channel ID
     * \param layer Layer
     * \param harqId HarqID
     * \param bwpId Bandwidth part ID
     * \param srcL2Id Sidelink source L2 ID (24 bits)
     * \param dstL2Id Sidelink destination L2 ID (24 bits)
     */
    NrSlTxOpportunityParameters (uint32_t bytes, uint16_t rnti, uint8_t lcId,
                                 uint8_t layer, uint8_t harqId, uint8_t bwpId,
                                 uint32_t srcL2Id,  uint32_t dstL2Id)
    {
      this->bytes = bytes;
      this->rnti = rnti;
      this->lcid = lcId;
      this->layer = layer;
      this->harqId = harqId;
      this->bwpId = bwpId;
      this->srcL2Id = srcL2Id;
      this->dstL2Id = dstL2Id;
    }

    uint32_t bytes {0};  //!<the number of bytes to transmit
    uint16_t rnti {std::numeric_limits <uint16_t>::max ()}; //!< the C-RNTI identifying the UE
    uint8_t lcid {std::numeric_limits <uint8_t>::max ()}; //!< the logical channel id */
    uint8_t layer {std::numeric_limits <uint8_t>::max ()}; //!< the layer of transmission (MIMO)
    uint8_t harqId {std::numeric_limits <uint8_t>::max ()}; //!< the HARQ ID
    uint8_t bwpId {std::numeric_limits <uint8_t>::max ()}; //!<the bandwidth part id
    uint32_t srcL2Id {0};  //!<Source L2 ID (24 bits)
    uint32_t dstL2Id {0};  //!<Destination L2 ID (24 bits)
  };
  /**
   * \brief Called by the MAC to notify the RLC that the scheduler granted a
   * transmission opportunity to this RLC instance.
   *
   * \param params the NrSlTxOpportunityParameters
   */
  virtual void NotifyNrSlTxOpportunity (const NrSlTxOpportunityParameters &params) = 0;

  /**
   * Parameters for NrSlMacSapUser::ReceiveNrRlcSlPdu
   *
   */
  struct NrSlReceiveRlcPduParameters
  {
    /**
     * \brief NrSlReceiveRlcPduParameters constructor
     * \param p Packet
     * \param rnti RNTI
     * \param lcid Logical Channel ID
     * \param srcL2Id Sidelink source L2 ID
     * \param dstL2Id Sidelink destination L2 ID
     */
    NrSlReceiveRlcPduParameters (const Ptr<Packet> &p, uint16_t rnti, uint8_t lcid, uint32_t srcL2Id, uint32_t dstL2Id)
    {
      this->p = p;
      this->rnti = rnti;
      this->lcid = lcid;
      this->srcL2Id = srcL2Id;
      this->dstL2Id = dstL2Id;
    }
    Ptr<Packet> p;  //!< the RLC PDU to be received
    uint16_t rnti {std::numeric_limits <uint16_t>::max ()}; //!< the C-RNTI identifying the UE
    uint8_t lcid {std::numeric_limits <uint8_t>::max ()}; //!< the logical channel id */
    uint32_t srcL2Id {0};  //!< Source L2 ID (24 bits)
    uint32_t dstL2Id {0};  //!< Destination L2 ID (24 bits)
  };
  /**
   * \brief Called by the MAC to notify the RLC of the reception of a new PDU
   *
   * Can't make it const ref, see RlcUm::DoReceiveNrSlRlcPdu
   *
   * \param params the NrSlReceiveRlcPduParameters
   */
  virtual void ReceiveNrSlRlcPdu (NrSlReceiveRlcPduParameters params) = 0;

};

/**
 * \ingroup lte
 *
 * Template for the implementation of the NrSlMacSapUser as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to UE RLC class, which are called by UE MAC
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlMacSapUser : public NrSlMacSapUser
{
public:
  /**
   * \brief Constructor
   *
   * \param rlc the RLC class
   */
  MemberNrSlMacSapUser (C* rlc);

  // inherited from NrSlMacSapUser
  virtual void NotifyNrSlTxOpportunity (const NrSlTxOpportunityParameters &params);
  virtual void ReceiveNrSlRlcPdu (NrSlReceiveRlcPduParameters params);

private:
  C* m_rlc; ///< the RLC class
};


template <class C>
MemberNrSlMacSapUser<C>::MemberNrSlMacSapUser (C* rlc)
  : m_rlc (rlc)
{
}

template <class C>
void MemberNrSlMacSapUser<C>::NotifyNrSlTxOpportunity (const NrSlTxOpportunityParameters &params)
{
  m_rlc->DoNotifyNrSlTxOpportunity (params);
}

template <class C>
void MemberNrSlMacSapUser<C>::ReceiveNrSlRlcPdu (NrSlReceiveRlcPduParameters params)
{
  m_rlc->DoReceiveNrSlRlcPdu (params);
}

} // namespace ns3


#endif // NR_SL_MAC_SAP_H

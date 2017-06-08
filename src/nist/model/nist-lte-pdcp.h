/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011-2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Manuel Requena <manuel.requena@cttc.es>
 * Modified by: NIST
 */

#ifndef NIST_LTE_PDCP_H
#define NIST_LTE_PDCP_H

#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"

#include "ns3/object.h"

#include "ns3/nist-lte-pdcp-sap.h"
#include "ns3/nist-lte-rlc-sap.h"

namespace ns3 {

/**
 * LTE PDCP entity, see 3GPP TS 36.323
 */
class NistLtePdcp : public Object // SimpleRefCount<NistLtePdcp>
{
  friend class NistLtePdcpSpecificLteRlcSapUser;
  friend class NistLtePdcpSpecificLtePdcpSapProvider<NistLtePdcp>;
public:
  NistLtePdcp ();
  virtual ~NistLtePdcp ();
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
   * Sets the source L2 Id for sidelink identification of the PDCP entity
   * \param src
   */
  void SetSourceL2Id (uint32_t src);
   
  /**
   * Sets the source L2 Id for sidelink identification of the PDCP entity
   * \param src
   */
  void SetDestinationL2Id (uint32_t dst);
 
  /**
   *
   *
   * \param s the PDCP SAP user to be used by this LTE_PDCP
   */
  void SetNistLtePdcpSapUser (NistLtePdcpSapUser * s);

  /**
   *
   *
   * \return the PDCP SAP Provider interface offered to the RRC by this LTE_PDCP
   */
  NistLtePdcpSapProvider* GetNistLtePdcpSapProvider ();

  /**
   *
   *
   * \param s the RLC SAP Provider to be used by this LTE_PDCP
   */
  void SetNistLteRlcSapProvider (NistLteRlcSapProvider * s);

  /**
   *
   *
   * \return the RLC SAP User interface offered to the RLC by this LTE_PDCP
   */
  NistLteRlcSapUser* GetNistLteRlcSapUser ();

  static const uint16_t MAX_PDCP_SN = 4096;

  /**
   * NistStatus variables of the PDCP
   * 
   */
  struct NistStatus
  {
    uint16_t txSn; ///< TX sequence number
    uint16_t rxSn; ///< RX sequence number
  };

  /** 
   * 
   * \return the current status of the PDCP
   */
  NistStatus GetNistStatus ();

  /**
   * Set the status of the PDCP
   * 
   * \param s 
   */
  void SetNistStatus (NistStatus s);

  /**
   * TracedCallback for PDU transmission event.
   *
   * \param [in] rnti The C-RNTI identifying the UE.
   * \param [in] lcid The logical channel id corresponding to
   *             the sending RLC instance.
   * \param [in] size Packet size.
   */
  typedef void (* PduTxTracedCallback)
    (const uint16_t rnti, const uint8_t lcid, const uint32_t size);

  /**
   * TracedCallback signature for PDU receive event.
   *
   * \param [in] rnti The C-RNTI identifying the UE.
   * \param [in] lcid The logical channel id corresponding to
   *             the sending RLC instance.
   * \param [in] size Packet size.
   * \param [in] delay Delay since packet sent, in ns..
   */
  typedef void (* PduRxTracedCallback)
    (const uint16_t rnti, const uint8_t lcid,
     const uint32_t size, const uint64_t delay);

protected:
  // Interface provided to upper RRC entity
  virtual void DoTransmitPdcpSdu (Ptr<Packet> p);

  NistLtePdcpSapUser* m_pdcpSapUser;
  NistLtePdcpSapProvider* m_pdcpSapProvider;

  // Interface provided to lower RLC entity
  virtual void DoReceivePdu (Ptr<Packet> p);

  NistLteRlcSapUser* m_rlcSapUser;
  NistLteRlcSapProvider* m_rlcSapProvider;

  uint16_t m_rnti;
  uint8_t m_lcid;
  /* Additional identifier for sidelink */
  uint32_t m_srcL2Id;  /**< Source L2 ID (24 bits) */
  uint32_t m_dstL2Id;  /**< Destination L2 ID (24 bits) */
    
  /**
   * Used to inform of a PDU delivery to the RLC SAP provider.
   * The parameters are RNTI, LCID and bytes delivered
   */
  TracedCallback<uint16_t, uint8_t, uint32_t> m_txPdu;
  /**
   * Used to inform of a PDU reception from the RLC SAP user.
   * The parameters are RNTI, LCID, bytes delivered and delivery delay in nanoseconds. 
   */
  TracedCallback<uint16_t, uint8_t, uint32_t, uint64_t> m_rxPdu;

private:
  /**
   * State variables. See section 7.1 in TS 36.323
   */
  uint16_t m_txSequenceNumber;
  uint16_t m_rxSequenceNumber;

  /**
   * Constants. See section 7.2 in TS 36.323
   */
  static const uint16_t m_maxPdcpSn = 4095;

};


} // namespace ns3

#endif // NIST_LTE_PDCP_H

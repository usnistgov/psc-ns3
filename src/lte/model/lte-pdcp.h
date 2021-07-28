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
 * Modified by: CTTC for NR Sidelink
 */

#ifndef LTE_PDCP_H
#define LTE_PDCP_H

#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"

#include "ns3/object.h"

#include "ns3/lte-pdcp-sap.h"
#include "ns3/lte-rlc-sap.h"
#include "ns3/nr-sl-pdcp-sap.h"
#include "ns3/nr-sl-rlc-sap.h"

namespace ns3 {

/**
 * LTE PDCP entity, see 3GPP TS 36.323
 */
class LtePdcp : public Object // SimpleRefCount<LtePdcp>
{
  /// allow LtePdcpSpecificLteRlcSapUser class friend access
  friend class LtePdcpSpecificLteRlcSapUser;
  /// allow LtePdcpSpecificLtePdcpSapProvider<LtePdcp> class friend access
  friend class LtePdcpSpecificLtePdcpSapProvider<LtePdcp>;
  //NR Sidelink
  /// allow MemberNrSlPdcpSapProvider<LtePdcp> class friend access
  friend class MemberNrSlPdcpSapProvider<LtePdcp>;
  /// allow MemberNrSlRlcSapUser<LtePdcp> class friend access
  friend class MemberNrSlRlcSapUser<LtePdcp>;
public:
  LtePdcp ();
  virtual ~LtePdcp ();
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
   * \param s the PDCP SAP user to be used by this LTE_PDCP
   */
  void SetLtePdcpSapUser (LtePdcpSapUser * s);

  /**
   *
   *
   * \return the PDCP SAP Provider interface offered to the RRC by this LTE_PDCP
   */
  LtePdcpSapProvider* GetLtePdcpSapProvider ();

  /**
   *
   *
   * \param s the RLC SAP Provider to be used by this LTE_PDCP
   */
  void SetLteRlcSapProvider (LteRlcSapProvider * s);

  /**
   *
   *
   * \return the RLC SAP User interface offered to the RLC by this LTE_PDCP
   */
  LteRlcSapUser* GetLteRlcSapUser ();

  /// maximum PDCP SN
  static const uint16_t MAX_PDCP_SN = 4096;

  /**
   * Status variables of the PDCP
   */
  struct Status
  {
    uint16_t txSn; ///< TX sequence number
    uint16_t rxSn; ///< RX sequence number
  };

  /** 
   * 
   * \return the current status of the PDCP
   */
  Status GetStatus ();

  /**
   * Set the status of the PDCP
   * 
   * \param s 
   */
  void SetStatus (Status s);

  /**
   * TracedCallback for PDU transmission event.
   *
   * \param [in] rnti The C-RNTI identifying the UE.
   * \param [in] lcid The logical channel id corresponding to
   *             the sending RLC instance.
   * \param [in] size Packet size.
   */
  typedef void (* PduTxTracedCallback)
    (uint16_t rnti, uint8_t lcid, uint32_t size);

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
  /**
   * Interface provided to upper RRC entity
   *
   * \param params the TransmitPdcpSduParameters
   */
  virtual void DoTransmitPdcpSdu (LtePdcpSapProvider::TransmitPdcpSduParameters params);

  LtePdcpSapUser* m_pdcpSapUser; ///< PDCP SAP user
  LtePdcpSapProvider* m_pdcpSapProvider; ///< PDCP SAP provider

  /**
   * Interface provided to lower RLC entity
   *
   * \param p packet
   */
  virtual void DoReceivePdu (Ptr<Packet> p);

  LteRlcSapUser* m_rlcSapUser; ///< RLC SAP user 
  LteRlcSapProvider* m_rlcSapProvider; ///< RLC SAP provider

  uint16_t m_rnti; ///< RNTI
  uint8_t m_lcid; ///< LCID

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
  /**
   * State variables. See section 7.1 in TS 36.323
   */
  uint16_t m_rxSequenceNumber;

  /**
   * Constants. See section 7.2 in TS 36.323
   */
  static const uint16_t m_maxPdcpSn = 4095;

//NR Sidelink

public:
  /**
   * \brief Get the NR Sidelik SAP offered by PDCP to RRC
   *
   * \return the NR Sidelik UE PHY SAP provider interface offered by PDCP to RRC
   */
  NrSlPdcpSapProvider* GetNrSlPdcpSapProvider ();

  /**
   * \brief Set the NR Sidelik SAP offered by RRC to PDCP
   *
   * \param s the NR Sidelik PDCP SAP user interface offered by RRC to PDCP
   */
  void SetNrSlPdcpSapUser (NrSlPdcpSapUser *s);

  /**
   * \brief Set the NR Sidelik SAP offered by RLC to PDCP
   *
   * \param s the NR Sidelink RLC SAP Provider interface offered by RLC to PDCP
   */
  void SetNrSlRlcSapProvider (NrSlRlcSapProvider *s);

  /**
   * \brief Get the NR Sidelik SAP offered by PDCP to RLC
   *
   * \return the NR Sidelink SAP user interface offered by PDCP to RLC
   */
  NrSlRlcSapUser* GetNrSlRlcSapUser ();

  /**
   * \brief Send a RRC PDU to the RDCP for transmission
   *
   * This method is to be called when upper RRC entity has a NR SL RRC PDU
   * ready to send.
   *
   * \param params
   */
  void DoTransmitNrSlPdcpSdu (const NrSlPdcpSapProvider::NrSlTransmitPdcpSduParameters &params);

  /**
   * \brief Called by the RLC entity to notify the PDCP entity of the reception
   * of a new NR sidelink PDCP PDU
   *
   * \param p the PDCP PDU
   */
  void DoReceiveNrSlPdcpPdu (Ptr<Packet> p);

  /**
   * \brief Sets the source L2 Id for sidelink identification of the RLC and PDCP entity
   * \param src The Sidelink source layer 2 id
   */
  void SetSourceL2Id (uint32_t src);

  /**
   * \brief Sets the source L2 Id for sidelink identification of the RLC PDCP entity
   * \param dst The Sidelink destination layer 2 id
   */
  void SetDestinationL2Id (uint32_t dst);

private:
  /**
   * \brief Indicates if this PDCP is for a sidelink radio bearer
   *
   * \return true if the PDCP instance is for a sidelink radio bearer
   */
  bool IsSlRb ();
  uint32_t m_srcL2Id {0};  //!< Source L2 ID (24 bits)
  uint32_t m_dstL2Id {0};  //!< Destination L2 ID (24 bits)
  /**
   * Constants. See section 7.2 in TS 36.323
   */
  static const uint16_t m_maxPdcpSlSn = 65535;
  NrSlPdcpSapProvider *m_nrSlPdcpSapProvider; //!< SAP interface to receive calls from RRC instance
  NrSlPdcpSapUser *m_nrSlPdcpSapUser {nullptr}; //!< SAP interface to call the methods of RRC instance
  NrSlRlcSapProvider *m_nrSlRlcSapProvider {nullptr}; //!< SAP interface to call the methods of RLC instance
  NrSlRlcSapUser *m_nrSlRlcSapUser; //!< SAP interface to receive calls from RLC instance

};


} // namespace ns3

#endif // LTE_PDCP_H

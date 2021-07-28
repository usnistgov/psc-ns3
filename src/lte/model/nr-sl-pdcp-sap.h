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

#ifndef NR_SL_PDCP_SAP_H
#define NR_SL_PDCP_SAP_H

#include "ns3/packet.h"

namespace ns3 {

/**
 * \ingroup lte
 *
 * Service Access Point (SAP) offered by the PDCP entity to the RRC entity for
 * NR Sidelink
 *
 * This is the PDCP SAP Provider
 * (i.e. the part of the SAP that contains the PDCP methods called by the RRC)
 */
class NrSlPdcpSapProvider
{
public:
  virtual ~NrSlPdcpSapProvider ();

  /**
   * \brief Parameters for NrSlPdcpSapProvider::TransmitNrSlPdcpSdu
   */
  struct NrSlTransmitPdcpSduParameters
  {
    /**
     * \brief NrSlTransmitPdcpSduParameters constructor
     *
     * \param pdcpSdu The RRC PDU
     * \param rnti The C-RNTI identifying the UE
     * \param lcId The logical channel id corresponding to the sending RLC instance
     * \param sduType The SDU type for SLRB (3 bits)
     */
    NrSlTransmitPdcpSduParameters (Ptr<Packet> pdcpSdu, uint16_t rnti, uint8_t lcId, uint8_t sduType)
    {
      this->pdcpSdu = pdcpSdu;
      this->rnti = rnti;
      this->lcId = lcId;
      this->sduType = sduType;
    }

    Ptr<Packet> pdcpSdu;  //!< the RRC PDU
    uint16_t    rnti; //!< the C-RNTI identifying the UE
    uint8_t     lcId; //!< the logical channel id corresponding to the sending RLC instance
    uint8_t sduType {std::numeric_limits<uint8_t>::max ()}; //!< SDU type for SLRB (3 bits)
  };

  /**
   * \brief Send a RRC PDU to the RDCP for transmission
   * This method is to be called
   * when upper RRC entity has a NR SL RRC PDU ready to send
   * 
   * \param params 
   */
  virtual void TransmitNrSlPdcpSdu (const NrSlTransmitPdcpSduParameters &params) = 0;
};


/**
 * \ingroup lte
 *
 * Service Access Point (SAP) offered by the PDCP entity to the RRC entity for
 * NR Sidelink
 *
 * This is the PDCP SAP User
 * (i.e. the part of the SAP that contains the RRC methods called by the PDCP)
 */
class NrSlPdcpSapUser
{
public:
  virtual ~NrSlPdcpSapUser ();

  /**
   * PDCP SDU types (TS 36.323 v14 section 6.3.14)
   */
  enum PdcpSduType
  {
    IP_SDU   = 0,
    ARP_SDU  = 1,
    PC5_SIGNALING_SDU = 2,
    NON_IP_SDU = 3
  };

  /**
   * \brief Parameters for NrSlPdcpSapUser::ReceiveNrSlPdcpSdu
   */
  struct NrSlReceivePdcpSduParameters
  {
    /**
     * \brief NrSlReceivePdcpSduParameters constructor
     * \param pdcpSdu The PDCP SDU
     * \param rnti The C-RNTI identifying the UE
     * \param lcId The logical channel id corresponding to the sending RLC instance
     * \param srcL2Id Source L2 ID
     * \param dstL2Id Destination L2 ID
     * \param sduType SDU type for SLRB
     */
    NrSlReceivePdcpSduParameters (Ptr<Packet> pdcpSdu, uint16_t rnti, uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id, uint8_t sduType)
    {
      this->pdcpSdu = pdcpSdu;
      this->rnti = rnti;
      this->lcId = lcId;
      this->srcL2Id = srcL2Id;
      this->dstL2Id = dstL2Id;
      this->sduType = sduType;
    }

    Ptr<Packet> pdcpSdu;  //!< the RRC PDU
    uint16_t    rnti; //!< the C-RNTI identifying the UE
    uint8_t     lcId; //!< the logical channel id corresponding to the sending RLC instance
    uint32_t    srcL2Id {0};  //!< Source L2 ID (24 bits)
    uint32_t    dstL2Id {0};  //!< Destination L2 ID (24 bits)
    uint8_t sduType {std::numeric_limits<uint8_t>::max ()}; //!< SDU type for SLRB (3 bits)
  };

  /**
  * \brief Called by the PDCP entity to notify the RRC entity of the reception
  * of a new NR SL RRC PDU
  *
  * \param params
  */
  virtual void ReceiveNrSlPdcpSdu (const NrSlReceivePdcpSduParameters &params) = 0;
};


/**
 * \ingroup lte
 *
 * Template for the implementation of the NrSlPdcpSapProvider as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to the PDCP class, which are called by RRC
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlPdcpSapProvider : public NrSlPdcpSapProvider
{
public:
  /**
   * Constructor
   * 
   * \param pdcp PDCP
   */
  MemberNrSlPdcpSapProvider (C* pdcp);

  // Interface implemented from NrSlPdcpSapProvider
  virtual void TransmitNrSlPdcpSdu (const NrSlTransmitPdcpSduParameters &params);

private:
  MemberNrSlPdcpSapProvider ();
  C* m_pdcp; ///< the PDCP
};

template <class C>
MemberNrSlPdcpSapProvider<C>::MemberNrSlPdcpSapProvider (C* pdcp)
  : m_pdcp (pdcp)
{
}


template <class C>
void MemberNrSlPdcpSapProvider<C>::TransmitNrSlPdcpSdu (const NrSlTransmitPdcpSduParameters &params)
{
  m_pdcp->DoTransmitNrSlPdcpSdu (params);
}


/**
 * \ingroup lte
 *
 * Template for the implementation of the NrSlPdcpSapProvider as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to the RRC class, which are called by PDCP
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlPdcpSapUser : public NrSlPdcpSapUser
{
public:
  /**
   * \brief Constructor
   *
   * \param rrc RRC
   */
  MemberNrSlPdcpSapUser (C* rrc);

  // Interface implemented from NrSlPdcpSapUser
  virtual void ReceiveNrSlPdcpSdu (const NrSlReceivePdcpSduParameters &params);

private:
  MemberNrSlPdcpSapUser ();
  C* m_rrc; ///< RRC
};

template <class C>
MemberNrSlPdcpSapUser<C>::MemberNrSlPdcpSapUser (C* rrc)
  : m_rrc (rrc)
{
}

template <class C>
void MemberNrSlPdcpSapUser<C>::ReceiveNrSlPdcpSdu (const NrSlReceivePdcpSduParameters &params)
{
  m_rrc->DoReceiveNrSlPdcpSdu (params);
}


} // namespace ns3

#endif // NR_SL_PDCP_SAP_H

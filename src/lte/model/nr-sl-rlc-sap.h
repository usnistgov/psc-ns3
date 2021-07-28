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

#ifndef NR_SL_RLC_SAP_H
#define NR_SL_RLC_SAP_H

#include "ns3/packet.h"

namespace ns3 {

/**
 * \ingroup lte
 *
 * Service Access Point (SAP) offered by the UM-RLC and AM-RLC entities to the
 * PDCP entity for NR Sidelink
 *
 * This is the RLC SAP Provider
 * (i.e. the part of the SAP that contains the RLC methods called by the PDCP)
 */
class NrSlRlcSapProvider
{
public:
  virtual ~NrSlRlcSapProvider ();

  /**
   * \brief Parameters for NrSlRlcSapProvider::TransmitNrSlPdcpPdu
   */
  struct NrSlTransmitPdcpPduParameters
  {
    /**
     * \brief NrSlTransmitPdcpPduParameters constructor
     *
     * \param pdu The PDCP PDU
     * \param rnti The C-RNTI identifying the UE
     * \param lcId The logical channel id corresponding to the sending RLC instance
     * \param srcL2Id Source L2 ID (24 bits)
     * \param dstL2Id Destination L2 ID (24 bits)
     */
    NrSlTransmitPdcpPduParameters (Ptr<Packet> pdu, uint16_t rnti, uint8_t lcId,
                                   uint32_t srcL2Id,  uint32_t dstL2Id)
    {
      this->pdcpPdu = pdu;
      this->rnti = rnti;
      this->lcid = lcId;
      this->srcL2Id = srcL2Id;
      this->dstL2Id = dstL2Id;
    }

    Ptr<Packet> pdcpPdu;  //!< the PDCP PDU
    uint16_t    rnti {std::numeric_limits <uint16_t>::max ()}; //!< the C-RNTI identifying the UE
    uint8_t     lcid {std::numeric_limits <uint8_t>::max ()}; //!< the logical channel id corresponding to the sending RLC instance
    uint32_t    srcL2Id {0};  //!< Source L2 ID (24 bits)
    uint32_t    dstL2Id {0};  //!< Destination L2 ID (24 bits)
  };

  /**
   * \brief Send a NR Sidelink PDCP PDU to the RLC for transmission
   *
   * This method is to be called when upper PDCP entity has a NR Sidelink PDCP
   * PDU ready to send
   *
   * \param params the NrSlTransmitPdcpPduParameters
   */
  virtual void TransmitNrSlPdcpPdu (const NrSlTransmitPdcpPduParameters &params) = 0;
};


/**
 * \ingroup lte
 *
 * Service Access Point (SAP) offered by the UM-RLC and AM-RLC entities to the
 * PDCP entity for NR Sidelink
 *
 *
 * This is the RLC SAP User
 * (i.e. the part of the SAP that contains the PDCP methods called by the RLC)
 */
class NrSlRlcSapUser
{
public:
  virtual ~NrSlRlcSapUser ();

 /**
  * \brief Called by the RLC entity to notify the PDCP entity of the reception
  * of a new NR sidelink PDCP PDU
  *
  * \param p the PDCP PDU
  */
  virtual void ReceiveNrSlPdcpPdu (Ptr<Packet> p) = 0;
};


/**
 * \ingroup lte
 *
 * Template for the implementation of the NrSlRlcSapProvider as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to the RLC class, which are called by PDCP
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlRlcSapProvider : public NrSlRlcSapProvider
{
public:
  /**
   * Constructor
   *
   * \param rlc the RLC
   */
  MemberNrSlRlcSapProvider (C* rlc);

  /**
   * Interface implemented from NrSlRlcSapProvider
   * \param params the NrSlTransmitPdcpPduParameters
   */
  virtual void TransmitNrSlPdcpPdu (const NrSlTransmitPdcpPduParameters &params);

private:
  MemberNrSlRlcSapProvider ();
  C* m_rlc; //!< the RLC
};

template <class C>
MemberNrSlRlcSapProvider<C>::MemberNrSlRlcSapProvider (C* rlc)
  : m_rlc (rlc)
{
}


template <class C>
void MemberNrSlRlcSapProvider<C>::TransmitNrSlPdcpPdu (const NrSlTransmitPdcpPduParameters &params)
{
  m_rlc->DoTransmitNrSlPdcpPdu (params);
}


/**
 * \ingroup lte
 *
 * Template for the implementation of the NrSlRlcSapUser as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to the RLC class, which are called by PDCP
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlRlcSapUser : public NrSlRlcSapUser
{
public:
  /**
   * \brief Constructor
   *
   * \param pdcp the PDCP
   */
  MemberNrSlRlcSapUser (C* pdcp);

  // Interface implemented from NrSlRlcSapUser
  virtual void ReceiveNrSlPdcpPdu (Ptr<Packet> p);

private:
  MemberNrSlRlcSapUser ();
  C* m_pdcp; //!< the PDCP
};

template <class C>
MemberNrSlRlcSapUser<C>::MemberNrSlRlcSapUser (C* pdcp)
  : m_pdcp (pdcp)
{
}

template <class C>
void MemberNrSlRlcSapUser<C>::ReceiveNrSlPdcpPdu (Ptr<Packet> p)
{
  m_pdcp->DoReceiveNrSlPdcpPdu (p);
}


} // namespace ns3

#endif // NR_SL_RLC_SAP_H

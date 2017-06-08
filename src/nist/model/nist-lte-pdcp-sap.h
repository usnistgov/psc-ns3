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
 * Author: Manuel Requena <manuel.requena@cttc.es>
 * Modified by: NIST
 */

#ifndef NIST_LTE_PDCP_SAP_H
#define NIST_LTE_PDCP_SAP_H

#include "ns3/packet.h"

namespace ns3 {

/**
 * Service Access Point (SAP) offered by the PDCP entity to the RRC entity
 * See 3GPP 36.323 Packet Data Convergence Protocol (PDCP) specification
 *
 * This is the PDCP SAP Provider
 * (i.e. the part of the SAP that contains the PDCP methods called by the RRC)
 */
class NistLtePdcpSapProvider
{
public:
  virtual ~NistLtePdcpSapProvider ();

  /**
   * Parameters for NistLtePdcpSapProvider::TransmitPdcpSdu
   */
  struct NistTransmitPdcpSduParameters
  {
    Ptr<Packet> pdcpSdu;  /**< the RRC PDU */
    uint16_t    rnti; /**< the C-RNTI identifying the UE */
    uint8_t     lcid; /**< the logical channel id corresponding to the sending RLC instance */
    /* Additional identifier for sidelink */
    uint32_t    srcL2Id;  /**< Source L2 ID (24 bits) */
    uint32_t    dstL2Id;  /**< Destination L2 ID (24 bits) */
  };

  /**
   * Send a RRC PDU to the RDCP for transmission
   * This method is to be called
   * when upper RRC entity has a RRC PDU ready to send   
   * 
   * \param params 
   */
  virtual void TransmitPdcpSdu (NistTransmitPdcpSduParameters params) = 0;
};


/**
 * Service Access Point (SAP) offered by the PDCP entity to the RRC entity
 * See 3GPP 36.323 Packet Data Convergence Protocol (PDCP) specification
 *
 * This is the PDCP SAP User
 * (i.e. the part of the SAP that contains the RRC methods called by the PDCP)
 */
class NistLtePdcpSapUser
{
public:
  virtual ~NistLtePdcpSapUser ();

  /**
   * Parameters for NistLtePdcpSapUser::ReceivePdcpSdu
   */
  struct NistReceivePdcpSduParameters
  {
    Ptr<Packet> pdcpSdu;  /**< the RRC PDU */
    uint16_t    rnti; /**< the C-RNTI identifying the UE */
    uint8_t     lcid; /**< the logical channel id corresponding to the sending RLC instance */
    /* Additional identifier for sidelink */
    uint32_t    srcL2Id;  /**< Source L2 ID (24 bits) */
    uint32_t    dstL2Id;  /**< Destination L2 ID (24 bits) */    
  };

  /**
  * Called by the PDCP entity to notify the RRC entity of the reception of a new RRC PDU
  *
  * \param params
  */
  virtual void ReceivePdcpSdu (NistReceivePdcpSduParameters params) = 0;
};

///////////////////////////////////////

template <class C>
class NistLtePdcpSpecificLtePdcpSapProvider : public NistLtePdcpSapProvider
{
public:
  NistLtePdcpSpecificLtePdcpSapProvider (C* pdcp);

  // Interface implemented from NistLtePdcpSapProvider
  virtual void TransmitPdcpSdu (NistTransmitPdcpSduParameters params);

private:
  NistLtePdcpSpecificLtePdcpSapProvider ();
  C* m_pdcp;
};

template <class C>
NistLtePdcpSpecificLtePdcpSapProvider<C>::NistLtePdcpSpecificLtePdcpSapProvider (C* pdcp)
  : m_pdcp (pdcp)
{
}

template <class C>
NistLtePdcpSpecificLtePdcpSapProvider<C>::NistLtePdcpSpecificLtePdcpSapProvider ()
{
}

template <class C>
void NistLtePdcpSpecificLtePdcpSapProvider<C>::TransmitPdcpSdu (NistTransmitPdcpSduParameters params)
{
  m_pdcp->DoTransmitPdcpSdu (params.pdcpSdu);
}

///////////////////////////////////////

template <class C>
class NistLtePdcpSpecificLtePdcpSapUser : public NistLtePdcpSapUser
{
public:
  NistLtePdcpSpecificLtePdcpSapUser (C* rrc);

  // Interface implemented from NistLtePdcpSapUser
  virtual void ReceivePdcpSdu (NistReceivePdcpSduParameters params);

private:
  NistLtePdcpSpecificLtePdcpSapUser ();
  C* m_rrc;
};

template <class C>
NistLtePdcpSpecificLtePdcpSapUser<C>::NistLtePdcpSpecificLtePdcpSapUser (C* rrc)
  : m_rrc (rrc)
{
}

template <class C>
NistLtePdcpSpecificLtePdcpSapUser<C>::NistLtePdcpSpecificLtePdcpSapUser ()
{
}

template <class C>
void NistLtePdcpSpecificLtePdcpSapUser<C>::ReceivePdcpSdu (NistReceivePdcpSduParameters params)
{
  m_rrc->DoReceivePdcpSdu (params);
}


} // namespace ns3

#endif // NIST_LTE_PDCP_SAP_H

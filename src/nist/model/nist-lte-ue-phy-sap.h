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
 * Author: Marco Miozzo <mmiozzo@cttc.es>
 * Modified by: NIST
 */



#ifndef NIST_LTE_UE_PHY_SAP_H
#define NIST_LTE_UE_PHY_SAP_H

#include <ns3/packet.h>

namespace ns3 {

class NistLteControlMessage;

/**
* Service Access Point (SAP) offered by the UE-PHY to the UE-MAC
*
* This is the PHY SAP Provider, i.e., the part of the SAP that contains
* the PHY methods called by the MAC
*/
class NistLteUePhySapProvider
{
public:
  virtual ~NistLteUePhySapProvider ();

  /**
  * \brief Send the MAC PDU to the channel
  * \param p the MAC PDU to send
  * \return true if
  */
  virtual void SendMacPdu (Ptr<Packet> p) = 0;

  /**
  * \brief Send SendNistLteControlMessage (PDCCH map, CQI feedbacks) using the ideal control channel
  * \param msg the Ideal Control Message to send
  */
  virtual void SendNistLteControlMessage (Ptr<NistLteControlMessage> msg) = 0;

  /** 
   * send a preamble on the PRACH
   * 
   * \param prachId the ID of the preamble
   */
  virtual void SendRachPreamble (uint32_t prachId, uint32_t raRnti) = 0;

};


/**
* Service Access Point (SAP) offered by the PHY to the MAC
*
* This is the PHY SAP User, i.e., the part of the SAP that contains the MAC
* methods called by the PHY
*/
class NistLteUePhySapUser
{
public:
  virtual ~NistLteUePhySapUser ();


  /**
  * Called by the Phy to notify the MAC of the reception of a new PHY-PDU
  *
  * \param p
  */
  virtual void ReceivePhyPdu (Ptr<Packet> p) = 0;

  /**
  * \brief Trigger the start from a new frame (input from Phy layer)
  * \param frameNo frame number
  * \param subframeNo subframe number
  */
  virtual void SubframeIndication (uint32_t frameNo, uint32_t subframeNo) = 0;

  /**
  * \brief Receive SendNistLteControlMessage (PDCCH map, CQI feedbacks) using the ideal control channel
  * \param msg the Ideal Control Message to receive
  */
  virtual void ReceiveNistLteControlMessage (Ptr<NistLteControlMessage> msg) = 0;

  /**
  * Notify the MAC that the PHY changed of timing as consequence of a change of SyncRef
  * Adjust the MAC subframe indication
  * \param frameNo the current PHY frame number
  * \param subframeNo the current PHY subframe number
  */
  virtual void NotifyChangeOfTiming (uint32_t frameNo, uint32_t subframeNo) = 0;
};



} // namespace ns3


#endif // NIST_LTE_UE_PHY_SAP_H

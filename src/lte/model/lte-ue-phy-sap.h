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
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */



#ifndef LTE_UE_PHY_SAP_H
#define LTE_UE_PHY_SAP_H

#include <ns3/packet.h>

namespace ns3 {

class LteControlMessage;

/**
* Service Access Point (SAP) offered by the UE-PHY to the UE-MAC
*
* This is the PHY SAP Provider, i.e., the part of the SAP that contains
* the PHY methods called by the MAC
*/
class LteUePhySapProvider
{
public:
  virtual ~LteUePhySapProvider ();

  /**
  * \brief Send The MAC PDU to the channel
  * \param p The MAC PDU to send
  */
  virtual void SendMacPdu (Ptr<Packet> p) = 0;

  /**
  * \brief Send SendLteControlMessage (PDCCH map, CQI feedbacks) using the ideal control channel
  * \param msg The Ideal Control Message to send
  */
  virtual void SendLteControlMessage (Ptr<LteControlMessage> msg) = 0;

  /** 
   * Send a preamble on the PRACH
   * 
   * \param prachId The ID of the preamble
   * \param raRnti The RA RNTI
   */
  virtual void SendRachPreamble (uint32_t prachId, uint32_t raRnti) = 0;

  //Sidelink Communication

  /**
   * Set discovery announcement apps
   * \param apps The applications we are interested in announcing
   */
   virtual void AddDiscTxApps (std::list<uint32_t> apps) = 0;

  /**
   * Set discovery monitoring apps
   * \param apps The applications we are interested in monitoring
   */
   virtual void AddDiscRxApps (std::list<uint32_t> apps) = 0;

  /**
   * Set grant for discovery
   * \param resPsdch The resource to use in the discovery pool
   */
  virtual void SetDiscGrantInfo (uint8_t resPsdch) = 0;

};


/**
* Service Access Point (SAP) offered by the PHY to the MAC
*
* This is the PHY SAP User, i.e., the part of the SAP that contains the MAC
* methods called by the PHY
*/
class LteUePhySapUser
{
public:
  virtual ~LteUePhySapUser ();


  /**
   * Called by the Phy to notify the MAC of the reception of a new PHY-PDU
   *
   * \param p The packet
   */
  virtual void ReceivePhyPdu (Ptr<Packet> p) = 0;

  /**
   * \brief Trigger the start from a new frame (input from Phy layer)
   * \param frameNo The frame number
   * \param subframeNo The subframe number
   */
  virtual void SubframeIndication (uint32_t frameNo, uint32_t subframeNo) = 0;

  /**
   * \brief Receive SendLteControlMessage (PDCCH map, CQI feedbacks) using the ideal control channel
   * \param msg The Ideal Control Message to receive
   */
  virtual void ReceiveLteControlMessage (Ptr<LteControlMessage> msg) = 0;

  /**
   * Notify the MAC that the PHY changed of timing as consequence of a change of SyncRef
   * Adjust the MAC subframe indication
   *
   * \param frameNo The current PHY frame number
   * \param subframeNo The current PHY subframe number
   */
   virtual void NotifyChangeOfTiming (uint32_t frameNo, uint32_t subframeNo) = 0;

  /**
   * Notify the MAC that Sidelink is configured
   */
  virtual void NotifySidelinkEnabled () = 0;
};

} // namespace ns3


#endif // LTE_UE_PHY_SAP_H

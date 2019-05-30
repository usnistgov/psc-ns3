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
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#ifndef LTE_UE_CMAC_SAP_H
#define LTE_UE_CMAC_SAP_H

#include <ns3/packet.h>
#include <ns3/ff-mac-common.h>
#include <ns3/eps-bearer.h>
#include <ns3/lte-common.h>
#include <ns3/lte-sl-pool.h>
#include <ns3/lte-control-messages.h>

namespace ns3 {



class LteMacSapUser;

/**
 * Service Access Point (SAP) offered by the UE MAC to the UE RRC
 *
 * This is the MAC SAP Provider, i.e., the part of the SAP that contains the MAC methods called by the RRC
 */
class LteUeCmacSapProvider
{
public:
  virtual ~LteUeCmacSapProvider ();

  /// RachConfig structure
  struct RachConfig
  {
    uint8_t numberOfRaPreambles; ///< number of RA preambles
    uint8_t preambleTransMax; ///< preamble transmit maximum
    uint8_t raResponseWindowSize; ///< RA response window size
    uint8_t connEstFailCount; ///< the counter value for T300 timer expiration
  };

  /**
   * Configure RACH function
   *
   * \param rc The RACH config
   */
  virtual void ConfigureRach (RachConfig rc) = 0;

  /**
   * Tells the MAC to start a contention-based random access procedure,
   * e.g., to perform RRC connection establishment
   *
   */
  virtual void StartContentionBasedRandomAccessProcedure () = 0;

  /**
   * Tells the MAC to start a non-contention-based random access
   * procedure, e.g., as a consequence of handover
   *
   * \param rnti The RNTI
   * \param rapId The Random Access Preamble Identifier
   * \param prachMask The PRACK mask
   */
  virtual void StartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t rapId, uint8_t prachMask) = 0;


  /// LogicalChannelConfig structure
  struct LogicalChannelConfig
  {
    uint8_t priority; ///< priority
    uint16_t prioritizedBitRateKbps; ///< prioritize bit rate Kbps
    uint16_t bucketSizeDurationMs; ///< bucket size duration ms
    uint8_t logicalChannelGroup; ///< logical channel group
  };

  /**
   * Adds a new Logical Channel (LC)
   *
   * \param lcId The ID of the LC
   * \param lcConfig The LC configuration provided by the RRC
   * \param msu The corresponding LteMacSapUser
   */
  virtual void AddLc (uint8_t lcId, LogicalChannelConfig lcConfig, LteMacSapUser* msu) = 0;

  /**
   * Adds a new Logical Channel (LC) used for Sidelink
   *
   * \param lcId The ID of the LC
   * \param srcL2Id The L2 group id of the source
   * \param dstL2Id The L2 group id of the destination
   * \param lcConfig The LC configuration provided by the RRC
   * \param msu The corresponding LteMacSapUser
   */
  virtual void AddSlLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id, LogicalChannelConfig lcConfig, LteMacSapUser* msu) = 0;

  /**
   * Removes an existing LC
   *
   * \param lcId
   */
  virtual void RemoveLc (uint8_t lcId) = 0;

  /**
   * Removes an existing Sidelink LC
   *
   * \param lcId The LC ID
   * \param srcL2Id The L2 group id of the source
   * \param dstL2Id The L2 group id of the destination
   */
  virtual void RemoveSlLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id) = 0;

  /**
   * Resets the MAC
   *
   */
  virtual void Reset () = 0;

  /**
   * Adds a Sidelink transmission pool for the given destination
   *
   * \param dstL2Id The destination
   * \param pool The transmission pool
   */
  virtual void AddSlCommTxPool (uint32_t dstL2Id, Ptr<SidelinkTxCommResourcePool> pool) = 0;

  /**
   * Removes the Sidelink communication transmission pool for the given destination
   *
   * \param dstL2Id The destination
   */
  virtual void RemoveSlCommTxPool (uint32_t dstL2Id) = 0;

  /**
   * Sets the Sidelink communication receiving pools
   *
   * \param pools The list of Sidelink receiving pools
   */
  virtual void SetSlCommRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools) = 0;

  /**
   * Adds a new destination to listen for
   *
   * \param destination A destination (L2 ID) to listen for
   */
  virtual void AddSlDestination (uint32_t destination) = 0;

  /**
   * Removes a destination to listen for
   *
   * \param destination The destination (L2 ID) that is no longer of interest
   */
  virtual void RemoveSlDestination (uint32_t destination) = 0;

  /**
   * Sets a Sidelink discovery pool
   *
   * \param pool The transmission pool
   */
  virtual void SetSlDiscTxPool (Ptr<SidelinkTxDiscResourcePool> pool) = 0;

  /**
   * Removes the Sidelink discovery pool
   */
  virtual void RemoveSlDiscTxPool () = 0;

  /**
   * Sets the Sidelink discovery receiving pools
   *
   * \param pools The Sidelink discovery receiving pools
   */
  virtual void SetSlDiscRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools) = 0;

  /**
   * Sets the RNTI of the UE
   *
   * \param rnti The cell-specific UE identifier
   */
  virtual void SetRnti (uint16_t rnti) = 0;

  /**
   * \brief Notify MAC about the successful RRC connection
   * establishment.
   */
  virtual void NotifyConnectionSuccessful () = 0;

  /**
   * \brief A method call by UE RRC to communicate the IMSI to the UE MAC
   * \param imsi the IMSI of the UE
   */
  virtual void SetImsi (uint64_t imsi) = 0;

  //todo Check if we need AddLCPriority function
  /**
   *  added function to handle priority in UL scheduling
   *
   */
  // virtual void AddLCPriority (uint8_t rnti, uint8_t lcid ,uint8_t  priority) = 0;

};



/**
 * Service Access Point (SAP) offered by the UE MAC to the UE RRC
 *
 * This is the MAC SAP User, i.e., the part of the SAP that contains the RRC methods called by the MAC
 */
class LteUeCmacSapUser
{
public:
  virtual ~LteUeCmacSapUser ();

  /**
   *
   *
   * \param rnti the T-C-RNTI, which will eventually become the C-RNTI after contention resolution
   */
  virtual void SetTemporaryCellRnti (uint16_t rnti) = 0;

  /**
   * Notify the RRC that the MAC Random Access procedure completed successfully
   *
   */
  virtual void NotifyRandomAccessSuccessful () = 0;

  /**
   * Notify the RRC that the MAC Random Access procedure failed
   *
   */
  virtual void NotifyRandomAccessFailed () = 0;

  /**
   * Notify the RRC that the MAC has detected a new incoming flow for Sidelink reception
   *
   * \param lcId The logical channel id
   * \param srcL2Id Sidelink source L2 id
   * \param dstL2Id Sidelink destination L2 id
   */
  virtual void NotifySidelinkReception (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id) = 0;

  /**
   * Notify the RRC that the MAC has data to send in the PSSCH
   */
  virtual void NotifyMacHasSlDataToSend () = 0;
  /**
   * Notify the RRC that the MAC does not have data to send in the PSSCH
   */
  virtual void NotifyMacHasNoSlDataToSend () = 0;

  /**
   * Notify the RRC that the MAC has detected a new incoming flow for discovery reception
   *
   * \param p The packet containing the discovery message
   */
  virtual void NotifyDiscoveryReception (Ptr<Packet> p) = 0;
};




} // namespace ns3


#endif // LTE_UE_CMAC_SAP_H

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
 * Modified by: NIST
 */

#ifndef NIST_LTE_UE_CMAC_SAP_H
#define NIST_LTE_UE_CMAC_SAP_H

#include <ns3/packet.h>
#include <ns3/nist-ff-mac-common.h>
#include <ns3/nist-eps-bearer.h>
#include <ns3/nist-lte-common.h>
#include <ns3/nist-sl-pool.h>
#include <ns3/nist-lte-control-messages.h>

namespace ns3 {



class NistLteMacSapUser;

/**
 * Service Access Point (SAP) offered by the UE MAC to the UE RRC
 *
 * This is the MAC SAP Provider, i.e., the part of the SAP that contains the MAC methods called by the RRC
 */
class NistLteUeCmacSapProvider
{
public:
  virtual ~NistLteUeCmacSapProvider ();

  struct NistRachConfig
  {
    uint8_t numberOfRaPreambles;
    uint8_t preambleTransMax;
    uint8_t raResponseWindowSize;
  };
  
  virtual void ConfigureRach (NistRachConfig rc) = 0;

  /** 
   * tell the MAC to start a contention-based random access procedure,
   * e.g., to perform RRC connection establishment 
   * 
   */
  virtual void StartContentionBasedRandomAccessProcedure () = 0;

  /** 
   * tell the MAC to start a non-contention-based random access
   * procedure, e.g., as a consequence of handover
   * 
   * \param rnti
   * \param rapId Random Access Preamble Identifier
   * \param prachMask 
   */
  virtual void StartNonContentionBasedRandomAccessProcedure (uint16_t rnti, uint8_t rapId, uint8_t prachMask) = 0;


  struct NistLogicalChannelConfig
  {
    uint8_t priority;
    uint16_t prioritizedBitRateKbps;
    uint16_t bucketSizeDurationMs;
    uint8_t logicalChannelGroup;
  };
  
  /** 
   * add a new Logical Channel (LC) 
   * 
   * \param lcId the ID of the LC
   * \param lcConfig the LC configuration provided by the RRC
   * \param msu the corresponding NistLteMacSapUser
   */
  virtual void AddLc (uint8_t lcId, NistLogicalChannelConfig lcConfig, NistLteMacSapUser* msu) = 0;

  /**
   * add a new Logical Channel (LC) used for Sidelink communication
   *
   * \param lcId the ID of the LC
   * \param lcConfig the LC configuration provided by the RRC
   * \param msu the corresponding NistLteMacSapUser
   */
  virtual void AddLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id, NistLogicalChannelConfig lcConfig, NistLteMacSapUser* msu) = 0;
  
  /** 
   * remove an existing LC
   * 
   * \param lcId 
   */
  virtual void RemoveLc (uint8_t lcId) = 0;

  /**
   * remove an existing LC
   *
   * \param lcId
   */
  virtual void RemoveLc (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id) = 0;
  
  /** 
   * reset the MAC
   * 
   */
  virtual void Reset () = 0;

  /**
   * add a sidelink transmission pool for the given destination
   * \param dstL2Id the destination
   * \param pool the transmission pool
   */
  virtual void AddSlTxPool (uint32_t dstL2Id, Ptr<SidelinkTxCommResourcePool> pool) = 0;

  /**
   * remove the sidelink transmission pool for the given destination
   * \param dstL2Id the destination
   */
  virtual void RemoveSlTxPool (uint32_t dstL2Id) = 0;

  /**
   * set the sidelink receiving pools
   * \param pools the sidelink receiving pools
   */
  virtual void SetSlRxPools (std::list<Ptr<SidelinkRxCommResourcePool> > pools) = 0;

    /**
   * add a new destination to listen for
   * \param a destination to listen for
   */
  virtual void AddSlDestination (uint32_t destination) = 0;

  /**
   * remove a destination to listen for
   * \param destination the destination that is no longer of interest
   */
  virtual void RemoveSlDestination (uint32_t destination) = 0;

  /**
   * add a sidelink discovery pool 
   * \param res discovery resources
   * \param pool the transmission pool
   */  
  virtual void AddSlTxPool (Ptr<SidelinkTxDiscResourcePool> pool) = 0;

  /**
   * remove the sidelink discovery pool 
   */
  virtual void RemoveSlTxPool () = 0;

 /**
   * set the sidelink discovery receiving pools
   * \param pools the sidelink discovery receiving pools
   */
  virtual void SetSlRxPools (std::list<Ptr<SidelinkRxDiscResourcePool> > pools) = 0;
  
  /*
   * Push announcing applications to MAC 
   * \param apps applications to announce
   */
  virtual void ModifyDiscTxApps (std::list<uint32_t> apps) = 0;
 
  /*
   * Push monitoring applications to MAC 
   * \params apps applications to monitor
   */
  virtual void ModifyDiscRxApps (std::list<uint32_t> apps) = 0;


  /**
   * Sets the RNTI when not connected to eNodeB
   */
  virtual void SetRnti (uint16_t rnti) = 0;

  /**
   *  added function to handle priority in UL scheduling 
   *
   */
  virtual void AddLCPriority (uint8_t rnti, uint8_t lcid ,uint8_t  priority) = 0;

  
};



/**
 * Service Access Point (SAP) offered by the UE MAC to the UE RRC
 *
 * This is the MAC SAP User, i.e., the part of the SAP that contains the RRC methods called by the MAC
 */
class NistLteUeCmacSapUser
{
public:

  virtual ~NistLteUeCmacSapUser ();

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
   * Notify the RRC that the MAC has detected a new incoming flow for sidelink reception
   */
  virtual void NotifySidelinkReception (uint8_t lcId, uint32_t srcL2Id, uint32_t dstL2Id) = 0;

  /**
   * Notify the RRC that the MAC has data to send in the PSSCH
   */
  virtual void NotifyMacHasSlDataToSend () = 0;
  /**
   * Notify the RRC that the MAC does not have data to send in the PSSCH
   */
  virtual void NotifyMacHasNotSlDataToSend () = 0;

  /**
   * Notify the RRC that the MAC has detected a new incoming flow for discovery reception
   */
  virtual void NotifyDiscoveryReception (Ptr<NistLteControlMessage> msg) = 0;
};




} // namespace ns3


#endif // NIST_LTE_UE_CMAC_SAP_H

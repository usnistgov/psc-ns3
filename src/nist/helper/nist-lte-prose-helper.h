/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This software was developed at the National Institute of Standards and
 * Technology by employees of the Federal Government in the course of
 * their official duties. Pursuant to titleElement 17 Section 105 of the United
 * States Code this software is not subject to copyright protection and
 * is in the public domain.
 * NIST assumes no responsibility whatsoever for its use by other parties,
 * and makes no guarantees, expressed or implied, about its quality,
 * reliability, or any other characteristic.
 * 
 * We would appreciate acknowledgement if the software is used.
 * 
 * NIST ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
 * DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING
 * FROM THE USE OF THIS SOFTWARE.
 * 
 * Modified by: NIST
 */

#ifndef NIST_LTE_PROSE_HELPER_H
#define NIST_LTE_PROSE_HELPER_H

#include <ns3/nist-lte-helper.h>
#include <ns3/nist-lte-ue-net-device.h>
#include <ns3/nist-lte-hex-grid-enb-topology-helper.h>
#include <ns3/output-stream-wrapper.h>

namespace ns3 {



/**
 * This helper class allows to easily create a topology with eNBs
 * grouped in three-sector sites layed out on an hexagonal grid. The
 * layout is done row-wise. 
 *
 */
class NistLteProseHelper : public Object
{
public:
  /**
   * Enumeration of the different methods to compute D2D RSRP 
   */
  enum SrsrpMethod_t { SRSRP_EVAL, ///< Defined in TR 36.843
                       SRSRP_STD ///<standard function of S-RSRP defined in TS 136.214
  };


  NistLteProseHelper (void);
  virtual ~NistLteProseHelper (void);

  static TypeId GetTypeId (void);
  virtual void DoDispose (void);


  /** 
   * Set the LteHelper to be used to actually create the EnbNetDevices
   *
   * \note if no EpcHelper is ever set, then NistLteProseHelper will default
   * to creating an LTE-only simulation with no EPC, using LteRlcSm as
   * the RLC model, and without supporting any IP networking. In other
   * words, it will be a radio-level simulation involving only LTE PHY
   * and MAC and the FF Scheduler, with a saturation traffic model for
   * the RLC.
   * 
   * \param h a pointer to the EpcHelper to be used
   */
  void SetLteHelper (Ptr<NistLteHelper> h);

  /**
   * Removes an element from the container 
   * \param container The container of net devices
   * \param item The net device to remove
   * \return a container without the item
   */
  NetDeviceContainer RemoveNetDevice (NetDeviceContainer container, Ptr<NetDevice> item);

  /**
   * Associate UEs for group communication
   * \param txPower The transmit power used by the UEs
   * \param ulEarfcn The uplink frequency band
   * \param ulBandwidth The uplink bandwidth
   * \param ues The list of UEs deployed
   * \param rsrpThreshold minimum RSRP to connect a transmitter and receiver
   * \param ngroups Number of groups to create
   * \param nreceivers Number of receivers per group
   * \param compMethod The method to compute the SRSRP value
   * \return List of groups, with first NetDevice in the container being the transmitter
   */
  std::vector < NetDeviceContainer > AssociateForGroupcast (double txPower, double ulEarfcn, double ulBandwidth, NetDeviceContainer ues, double rsrpThreshold, int ngroups, int nreceivers, SrsrpMethod_t compMethod = NistLteProseHelper::SRSRP_STD);

 /**
   * Associate UEs for broadcast communication
   * \param txPower The transmit power used by the UEs
   * \param ulEarfcn The uplink frequency band
   * \param ulBandwidth The uplink bandwidth
   * \param ues The list of UEs deployed
   * \param rsrpThreshold minimum RSRP to connect a transmitter and receiver
   * \param ntransmitters Number of groups to create
   * \param compMethod The method to compute the SRSRP value
   * \return List of groups, with first NetDevice in the container being the transmitter
   */
  std::vector < NetDeviceContainer > AssociateForBroadcast (double txPower, double ulEarfcn, double ulBandwidth, NetDeviceContainer ues, double rsrpThreshold, uint32_t ntransmitters, SrsrpMethod_t compMethod = NistLteProseHelper::SRSRP_STD);

/**
 * Associate UEs for group communication with transmitters enabled to receive
 * \param txPower The transmit power used by the UEs
 * \param ulEarfcn The uplink frequency band
 * \param ulBandwidth The uplink bandwidth
 * \param ues The list of UEs deployed
 * \param rsrpThreshold minimum RSRP to connect a transmitter and receiver
 * \param ntransmitters Number of transmitters to create
 * \param compMethod The method to compute the SRSRP value
 * \return List of groups, with first NetDevice in the container being the transmitter
 */
std::vector < NetDeviceContainer > AssociateForBroadcastWithTxEnabledToReceive (double txPower, double ulEarfcn, double ulBandwidth, NetDeviceContainer ues, double rsrpThreshold, uint32_t ntransmitters, SrsrpMethod_t compMethod);

 /**
   * Associate UEs for broadcast communication
   * \param txPower The transmit power used by the UEs
   * \param ulEarfcn The uplink frequency band
   * \param ulBandwidth The uplink bandwidth
   * \param ues The list of UEs deployed
   * \param rsrpThreshold minimum RSRP to connect a transmitter and receiver
   * \param ntransmitters Number of groups to create
   * \param topologyHelper Pointer to the topology helper
   * \param compMethod The method to compute the SRSRP value
   * \return List of groups, with first NetDevice in the container being the transmitter
   */
  std::vector < NetDeviceContainer > AssociateForBroadcastWithWrapAround (double txPower, double ulEarfcn, double ulBandwidth, NetDeviceContainer ues, double rsrpThreshold, uint32_t ntransmitters, Ptr<NistLteHexGridEnbTopologyHelper> topologyHelper, SrsrpMethod_t compMethod = NistLteProseHelper::SRSRP_STD);

  /**
   * Prints the groups starting by the transmitter
   * \param groups The list of groups
   */
  void PrintGroups (std::vector < NetDeviceContainer > groups);
  
  /**
   * Prints the groups in a table format to a provided stream.
   * \param groups The list of groups
   * \param stream The output stream
   */
  void PrintGroups (std::vector < NetDeviceContainer > groups, Ptr< OutputStreamWrapper > stream);

  /**
   * Schedule the activation of a sidelink bearer
   * \param activationTime The time to setup the sidelink bearer
   * \param ues The list of UEs where the bearer must be activated
   * \param tft Traffic flow template for the bearer (i.e. multicast address and group)
   */
  void ActivateSidelinkBearer (Time activationTime, NetDeviceContainer ues, Ptr<NistSlTft> tft);

  /**
   * Activation of a sidelink bearer
   * \param ues The list of UEs where the bearer must be activated
   * \param tft Traffic flow template for the bearer (i.e. multicast address and group)
   */
  void DoActivateSidelinkBearer (NetDeviceContainer ues, Ptr<NistSlTft> tft);

private:

  Ptr<NistLteHelper> m_lteHelper;



};


} // namespace ns3



#endif // NIST_LTE_PROSE_HELPER_H

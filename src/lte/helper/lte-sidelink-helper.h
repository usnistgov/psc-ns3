/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#ifndef LTE_SIDELINK_HELPER_H
#define LTE_SIDELINK_HELPER_H


#include <ns3/lte-helper.h>
#include <ns3/lte-ue-net-device.h>
#include <ns3/output-stream-wrapper.h>
#include <ns3/lte-spectrum-value-helper.h>
#include <ns3/lte-3gpp-hex-grid-enb-topology-helper.h>
#include <ns3/sidelink-rsrp-calculator.h>

namespace ns3 {

/**
 * This helper class is provided to ease the burden of a user to
 * configure public safety scenarios involving Sidelink. In
 * particular, it uses other helper classes, e.g., LteHelper to create
 * groups for Sidelink broadcast or groupcast communication and
 * Lte3gppHexGridEnbTopologyHelper to associate UEs to a Sidelink group
 * or an eNB using wrap around method.
 *
 */

class LteSidelinkHelper : public Object
{
public:
  /**
   * Enumeration of the different methods to compute D2D RSRP
   */
  enum SrsrpMethod_t
  {
    SLRSRP_TX_PW,                    ///< S-RSRP (Sidelink RSRP) calculation method defined in TR 36.843
    SLRSRP_PSBCH                        ///< S-RSRP (Sidelink RSRP) calculation method defined in TS 36.214
  };

  LteSidelinkHelper (void);
  virtual ~LteSidelinkHelper (void);

  // inherited from Object
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);
  virtual void DoDispose (void);


  /**
   * Set the LteHelper to be used to actually create the EnbNetDevices
   *
   * \note if no EpcHelper is ever set, then LteSidelinkHelper will default
   * to creating an LTE-only simulation with no EPC, using LteRlcSm as
   * the RLC model, and without supporting any IP networking. In other
   * words, it will be a radio-level simulation involving only LTE PHY
   * and MAC and the FF Scheduler, with a saturation traffic model for
   * the RLC.
   *
   * \param h A pointer to the EpcHelper to be used
   */
  void SetLteHelper (Ptr<LteHelper> h);

  /**
   * Removes an element from the container
   * \param container The container of net devices
   * \param item The net device to remove
   * \return A container without the item
   */
  NetDeviceContainer RemoveNetDevice (NetDeviceContainer container, Ptr<NetDevice> item);

  /**
   * Associate UEs for group communication
   * \param txPower The transmit power used by the UEs
   * \param ulEarfcn The uplink frequency band
   * \param ulBandwidth The uplink bandwidth
   * \param ues The list of UEs deployed
   * \param rsrpThreshold The minimum RSRP to connect a transmitter and receiver
   * \param nGroups The number of groups to create
   * \param nReceivers The number of receivers per group
   * \param compMethod The method to compute the SRSRP value
   * \return The list of groups, with first NetDevice in the container being the transmitter
   */
  std::vector < NetDeviceContainer > AssociateForGroupcast (double txPower, double ulEarfcn, double ulBandwidth, NetDeviceContainer ues, double rsrpThreshold, int nGroups, int nReceivers, SrsrpMethod_t compMethod = LteSidelinkHelper::SLRSRP_PSBCH);

  /**
   * Associate UEs for broadcast communication
   * \param txPower The transmit power used by the UEs
   * \param ulEarfcn The uplink frequency band
   * \param ulBandwidth The uplink bandwidth
   * \param ues The list of UEs deployed
   * \param rsrpThreshold minimum RSRP to connect a transmitter and receiver
   * \param nTransmitters Number of groups to create
   * \param compMethod The method to compute the SRSRP value
   * \return The list of groups, with first NetDevice in the container being the transmitter
   */
  std::vector < NetDeviceContainer > AssociateForBroadcast (double txPower, double ulEarfcn, double ulBandwidth, NetDeviceContainer ues, double rsrpThreshold, uint32_t nTransmitters, SrsrpMethod_t compMethod = LteSidelinkHelper::SLRSRP_PSBCH);

  /**
   * Associate UEs for group communication with transmitters enabled to receive
   * \param txPower The transmit power used by the UEs
   * \param ulEarfcn The uplink frequency band
   * \param ulBandwidth The uplink bandwidth
   * \param ues The list of UEs deployed
   * \param rsrpThreshold The minimum RSRP to connect a transmitter and receiver
   * \param nTransmitters The number of transmitters to create
   * \param compMethod The method to compute the SRSRP value
   * \return The list of groups, with first NetDevice in the container being the transmitter
   */
  std::vector < NetDeviceContainer > AssociateForBroadcastWithTxEnabledToReceive (double txPower, double ulEarfcn, double ulBandwidth, NetDeviceContainer ues, double rsrpThreshold, uint32_t nTransmitters, SrsrpMethod_t compMethod);

  /**
   * Associate UEs for broadcast communication
   * \param txPower The transmit power used by the UEs
   * \param ulEarfcn The uplink frequency band
   * \param ulBandwidth The uplink bandwidth
   * \param ues The list of UEs deployed
   * \param rsrpThreshold The minimum RSRP to connect a transmitter and receiver
   * \param nTransmitters The number of groups to create
   * \param topologyHelper The pointer to the topology helper
   * \param compMethod The method to compute the SRSRP value
   * \return The list of groups, with first NetDevice in the container being the transmitter
   */
  std::vector < NetDeviceContainer > AssociateForBroadcastWithWrapAround (double txPower, double ulEarfcn, double ulBandwidth, NetDeviceContainer ues, double rsrpThreshold, uint32_t nTransmitters, Ptr<Lte3gppHexGridEnbTopologyHelper> topologyHelper, SrsrpMethod_t compMethod = LteSidelinkHelper::SLRSRP_PSBCH);

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
   * \param tft The traffic flow template for the bearer (i.e. multicast address and group)
   */
  void ActivateSidelinkBearer (Time activationTime, NetDeviceContainer ues, Ptr<LteSlTft> tft);

  /**
   * Activation of a sidelink bearer
   * \param ues The list of UEs where the bearer must be activated
   * \param tft The traffic flow template for the bearer (i.e. multicast address and group)
   */
  void DoActivateSidelinkBearer (NetDeviceContainer ues, Ptr<LteSlTft> tft);

   /**
   * Starts discovery process for given applications depending on the interest (monitoring or announcing)
   * \param ueDevice the targeted device
   * \param appCodes app code payloads to be added
   * \param role UE role (discovered or discoveree)
   */
  void StartDiscoveryApps (Ptr<NetDevice> ueDevice, std::list<uint64_t> appCodes, LteSlUeRrc::DiscoveryRole role);
  
  /**
   * Stops discovery process for given applications depending on the interest (monitoring or announcing)
   * \param ueDevice the targeted device
   * \param appCodes app code payloads to be removed
   * \param role UE role (discovered or discoveree)
   */
  void StopDiscoveryApps (Ptr<NetDevice> ueDevice, std::list<uint64_t> appCodes, LteSlUeRrc::DiscoveryRole role);

  /**
   * Starts UE-to-Network relay process
   * \param ueDevice the targeted device
   * \param serviceCode relay service code to use
   * \param model discovery model (A or B)
   * \param role UE role (remote UE or relay node)
   */
  void StartRelayService (Ptr<NetDevice> ueDevice, uint32_t serviceCode, LteSlUeRrc::DiscoveryModel model, LteSlUeRrc::RelayRole role); 

  /**
   * Stops UE-to-Network relay process
   * \param ueDevice the targeted device
   * \param serviceCode relay service code to use
   */
  void StopRelayService (Ptr<NetDevice> ueDevice, uint32_t serviceCode);

  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this model.  Return the number of streams (possibly zero) that
   * have been assigned.
   *
   * \param stream The first stream index to use
   * \return The number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);

private:
  Ptr<LteHelper> m_lteHelper; ///< Pointer to the LteHelper
  Ptr<UniformRandomVariable> m_uniformRandomVariable; ///< Provides uniform random variables
};


} // namespace ns3


#endif // LTE_SIDELINK_HELPER_H

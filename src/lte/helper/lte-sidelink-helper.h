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
#include <ns3/ipv6-address-helper.h>
#include <map>


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

  /**
   * Structure to be used as key to IPv6 address configurations of one-to-one sidelink link
   */
  struct LteSlO2OIpv6AddressKey
  {
    uint32_t relayL2Id; ///< Relay UE layer 2 ID
    uint32_t remoteL2Id; ///< Remote UE layer 2 ID
  };

  /**
   * Structure to be used as value to IPv6 address configurations of one-to-one sidelink link
   */
  struct LteSlO2OIpv6AddressValue
  {
    Ipv6AddressHelper ipv6ah; ///< IPv6 address helper used to assign addresses
    Ipv6Address relay_address = Ipv6Address::GetOnes(); ///< Address assigned for relay UE (or all 1s if not initialized)
    Ipv6Address remote_address = Ipv6Address::GetOnes(); ///< Address assigned for remote UE (or all 1s if not initialized)
  };

  /**
   * Less than operator
   *
   * \param l first SidelinkLcIdentifier
   * \param r second SidelinkLcIdentifier
   * \returns true if first SidelinkLcIdentifier parameter values are less than the second SidelinkLcIdentifier parameters"
   */
  friend bool operator < (const LteSlO2OIpv6AddressKey &l, const LteSlO2OIpv6AddressKey &r)
  {
    return (l.relayL2Id < r.relayL2Id) || (l.relayL2Id == r.relayL2Id && l.remoteL2Id < r.remoteL2Id);
  }

  LteSidelinkHelper (void);
  virtual ~LteSidelinkHelper (void);

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
  void StartDiscoveryApps (Ptr<NetDevice> ueDevice, std::list<uint32_t> appCodes, LteSlUeRrc::DiscoveryRole role);
  
  /**
   * Stops discovery process for given applications depending on the interest (monitoring or announcing)
   * \param ueDevice the targeted device
   * \param appCodes app code payloads to be removed
   * \param role UE role (discovered or discoveree)
   */
  void StopDiscoveryApps (Ptr<NetDevice> ueDevice, std::list<uint32_t> appCodes, LteSlUeRrc::DiscoveryRole role);

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

  /**
   * Sets the prefix for assigning IPv6 addresses for UE-to-Network relay
   * The prefix length needs to be less that 64 bits
   * \param prefix The prefix to use
   * \param network The network address
   */
  void SetIpv6BaseForRelayCommunication (Ipv6Address network, Ipv6Prefix prefix);

  /**
   * Returns the IPv6 network address used for UE-to-Network Relay
   *
   * \return The UE-to-Network Relay IPv6 network address
   */
  Ipv6Address GetIpv6NetworkForRelayCommunication () const;

  /**
   * Returns the IPv6 prefix used for UE-to-Network Relay
   *
   * \return The UE-to-Network Relay IPv6 prefix
   */
  Ipv6Prefix GetIpv6PrefixForRelayCommunication () const;

  /**
   * Assign and return an IPv6 address for relay communication
   * The function is expected to be called by each node (relay and remote)
   * \param dev The pointer to the NetDevice
   * \param relayL2Id The relay L2 ID
   * \param remoteL2Id The remote L2 ID
   * \param role The role (RelayUE or Remote UE)
   * \return The container with interface information
   */
  Ipv6InterfaceContainer AssignIpv6AddressForRelayCommunication (Ptr<NetDevice> dev, uint32_t relayL2Id, uint32_t remoteL2Id, LteSlUeRrc::RelayRole role);

  /**
   * Returns the Relay UE IPv6 address after searching in the m_ipv6AddressHelperMap
   * If not found, return all zero IPv6 address
   * \param relayL2Id The relay L2 ID
   * \param remoteL2Id The remote L2 ID
   * \param role The role (RelayUE or Remote UE)
   *
   * \return The Relay IPv6 address
   */
  Ipv6Address GetRelayIpv6AddressFromMap (uint32_t relayL2Id, uint32_t remoteL2Id, LteSlUeRrc::RelayRole role);
  
  /**
   * Returns the self IPv6 address after searching in the m_ipv6AddressHelperMap
   * If not found, return all zero IPv6 address
   * \param relayL2Id The relay L2 ID
   * \param remoteL2Id The remote L2 ID
   * \param role The role (RelayUE or Remote UE)
   *
   * \return The self IPv6 address
   */
  Ipv6Address GetSelfIpv6AddressFromMap (uint32_t relayL2Id, uint32_t remoteL2Id, LteSlUeRrc::RelayRole role);

  /**
   * Informs the PGW/SGW of a new Remote UE connected to a relay UE
   *
   * \param relayImsi The IMSI of the relay node
   * \param ueImsi The IMSI of the remote UE
   * \param ipv6Prefix The /64 prefix assigned to the remote UE
   */
  void RemoteUeContextConnected (uint64_t relayImsi, uint64_t ueImsi, uint8_t ipv6Prefix[8]);

  /**
   * Informs the PGW/SGW of a new Remote UE connected to a relay UE
   *
   * \param relayImsi The IMSI of the relay node
   * \param ueImsi The IMSI of the remote UE
   * \param ipv6Prefix The /64 prefix that was assigned to the remote UE
   */
  void RemoteUeContextDisconnected (uint64_t relayImsi, uint64_t ueImsi, uint8_t ipv6Prefix[8]);

  /**
   * Returns a Mode 2 (UE_SELECTED) Sidelink communication resource pool.
   * The pool is configured with default values to provide a working example
   * and ease scenario configuration
   *
   * \return A Sidelink communication pool configured with default values
   */
  LteRrcSap::SlCommTxResourcesSetup GetDefaultSlCommTxResourcesSetupUeSelected () const;

  /**
   * Returns a Mode 1 (SCHEDULED) Sidelink communication resource pool.
   * The pool is configured with default values to provide a working example
   * and ease scenario configuration
   *
   * \return A Sidelink communication pool configured with default values
   */
  LteRrcSap::SlCommTxResourcesSetup GetDefaultSlCommTxResourcesSetupScheduled () const;

  /**
   * Returns a Mode 2 (UE_SELECTED) Sidelink discovery resource pool.
   * The pool is configured with default values to provide a working example
   * and ease scenario configuration
   *
   * \return A Sidelink discovery pool configured with default values
   */
  LteRrcSap::SlDiscTxResourcesSetup GetDefaultSlDiscTxResourcesSetupUeSelected () const;

  /**
   * Returns a default configuration for the UE-to-Network Relay parameters
   * to be broadcasted by the eNodeB on the SIB 19. This configuration is
   * necessary for Relay UE (re)selection and the configured default values
   * provide a working example to ease scenario configuration
   *
   * \return A SIB 19 UE-to-Network Relay configuration
   */
  LteRrcSap::Sib19DiscConfigRelay GetDefaultSib19DiscConfigRelay () const;

  /**
   * Configures a static route from the PGW towards to the UE-to-Network Relay
   * sub-net
   *
   * \param pgw A pointer to the PGW node
   */
  void ConfigurePgwToUeToNetworkRelayRoute (Ptr<Node> pgw);

  /**
   * Returns a Sidelink communication pool list for preconfiguration. The list
   * contains one pool, which is configured with default values to provide a
   * working example and ease scenario configuration
   *
   * \return A Sidelink communication pool list for preconfiguration
   */
  LteRrcSap::SlPreconfigCommPoolList GetDefaultSlPreconfigCommPoolList () const;


  /**
   * Returns a Sidelink discovery pool list for preconfiguration. The list
   * contains one pool, which is configured with default values to provide a
   * working example and ease scenario configuration
   *
   * \return A Sidelink discovery pool list for preconfiguration
   */
  LteRrcSap::SlPreconfigDiscPoolList GetDefaultSlPreconfigDiscPoolList () const;

  /**
   * Returns a UE-to-Network Relay preconfiguration. This configuration is
   * necessary for Relay UE (re)selection and the configured default values
   * provide a working example to ease scenario configuration
   *
   * \return A UE-to-Network Relay preconfiguration
   */
  LteRrcSap::SlPreconfigRelay GetDefaultSlPreconfigRelay () const;

private:
  Ptr<LteHelper> m_lteHelper; ///< Provides access to LTE helper
  Ptr<UniformRandomVariable> m_uniformRandomVariable; ///< Provides uniform random variables

  Ipv6AddressHelper m_relayIpv6ah; ///< Address helper for assigning addresses for UE relay operations
  Ipv6Prefix m_ipv6AddressRelayPrefix; ///< Prefix from which addresses are assigned for UE relay operations
  Ipv6Address m_ipv6AddressRelayNetwork; ///< Base network to assign IPv6 address helper
  std::map <LteSlO2OIpv6AddressKey, LteSlO2OIpv6AddressValue> m_ipv6AddressValueMap; ///< Provides mapping between pairs of L2 addresses and IPv6 address helper, IPv6 addresses and required booleans
};


} // namespace ns3


#endif // LTE_SIDELINK_HELPER_H

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


#ifndef NR_SL_PROSE_HELPER_H
#define NR_SL_PROSE_HELPER_H

#include <ns3/object.h>
#include <ns3/net-device-container.h>
#include <ns3/lte-rrc-sap.h>

#include <ns3/nr-sl-helper.h>
#include <ns3/nr-sl-ue-prose.h>

#include <ns3/nr-sl-discovery-trace.h>

namespace ns3 {

class EpcTft;
class EpsBearer;
class NrPointToPointEpcHelper;
class NrUeNetDevice;


class NrSlProseHelper : public Object
{

/**
 * \brief Class to help in the configuration of the Proximity Service (ProSe)
 *        functionalities
 */
public:
  /**
   * \brief Constructor
   */
  NrSlProseHelper (void);
  /**
   * \brief Destructor
   */
  virtual ~NrSlProseHelper (void);
  /**
   * \brief GetTypeId, inherited from Object
   *
   * \returns The TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Set EPC helper
   *
   * \param epcHelper Ptr of type NrPointToPointEpcHelper
   */
  void SetEpcHelper (const Ptr<NrPointToPointEpcHelper> &epcHelper);
  /**
   * \brief Prepare UE for ProSe
   *
   * This method installs the ProSe layer in the UE(s) in the container
   *
   * \param c The NetDeviceContainer
   */
  void PrepareUesForProse (NetDeviceContainer c);
  /**
   * \brief Prepare UE for Unicast ProSe Direct Communication
   *
   * This method configures the UE(s) in the container to be able to do unicast
   * ProSe direct communication
   *
   * \param c The \c NetDeviceContainer
   */
  void PrepareUesForUnicast (NetDeviceContainer c);

  /**
   * \brief Establish a 5G ProSe direct link between the two UEs using the real
   *        protocol
   *
   * This method schedules the creation of the direct link instances in both UEs
   * participating in the direct link. Then, the ProSe layer configures the direct
   * link instances and starts the establishment procedure in the initiating UE.
   * A real protocol means that PC5-S messages used for establishing and
   * maintaining the direct link connection go through the protocol stack, and
   * are transmitted in SL-SRBs and sent over the SL
   *
   * \param time The time at which the direct link instances should be created
   * \param initUe The UE initiating the establishment procedure
   * \param initUeIp The IP address used by the initiating UE
   * \param initSlInfo the traffic profile parameters to be used for the sidelink data radio bearer on the initiating UE
   * \param trgtUE The peer UE
   * \param trgtUeIp The IP address used by the target UE
   * \param trgtSlInfo the traffic profile parameters to be used for the sidelink data radio bearer on the target UE
   */
  void EstablishRealDirectLink (Time time,
		                        Ptr<NetDevice> initUe, Ipv4Address initUeIp, struct SidelinkInfo& initSlInfo,
		                        Ptr<NetDevice> trgtUe, Ipv4Address trgtUeIp, struct SidelinkInfo& trgtSlInfo);

  /**
   * \brief Establish a 5G ProSe L3 UE-to-Network (U2N) relay connection between
   *        two UEs (a remote UE and a relay UE)
   *
   * This method schedules the creation of the corresponding direct link instances
   * in both UEs participating in the U2N relay connection (Remote UE is the
   * initiating UE of the direct link and relay UE is the target UE). Then, the
   * ProSe layer configures the direct link instances and starts the establishment
   * procedure in the remote UE.
   *
   * \param t The time at which the L3 U2N connection procedure should start
   * \param remoteUe The remote UE of the connection
   * \param remoteUeIp The IPv4 address used by the remote UE
   * \param remoteUeSlInfo the traffic profile parameters to be used for the sidelink data radio bearer on the remote UE
   * \param relayUE The relay UE
   * \param relayUeIp The IPv4 address used by the relay UE
   * \param relayUeSlInfo the traffic profile parameters to be used for the sidelink data radio bearer on the relay UE
   * \param relayServiceCode the relay service code associated to this direct link
   */
  void EstablishL3UeToNetworkRelayConnection (Time t,
		                                      Ptr<NetDevice> remoteUe, Ipv4Address remoteUeIp, struct SidelinkInfo& remoteUeSlInfo,
                                              Ptr<NetDevice> relayUe, Ipv4Address relayUeIp, struct SidelinkInfo& relayUeSlInfo,
                                              uint32_t relayServiceCode);

  /**
   * \brief Install configuration on the UEs that will act as L3 UE-to-Network (U2N)
   *        relay UEs
   *
   *  This method activates the EPS bearer to be used for relaying traffic on
   *  each relay UE device, and internally sets the pointer to the EpcHelper in
   *  the ProSe layer. The EpcHelper will be used by the ProSe layer to configure
   *  the data path in the EpcPgwApplication when a remote UE successfully connects
   *  to the relay UE
   *
   * \param ueDevices the devices in which the L3 U2N relay configuration will be installed
   * \param relayServiceCodes the relay service codes to which the configuration will be associated
   * \param bearer the EPS bearer to be used for relaying traffic
   * \param tft the traffic flow template to be used for relaying traffic
   */
  void ConfigureL3UeToNetworkRelay (NetDeviceContainer ueDevices,
                                    std::set<uint32_t> relayServiceCodes,
                                    EpsBearer bearer, Ptr<EpcTft> tft);

  /**
   * Starts discovery process for given applications depending on the interest (monitoring or announcing)
   * \param ueDevice the targeted device
   * \param appCode application code to be added
   * \param dstL2Id destination layer 2 ID to be set for this appCode
   * \param role UE role (discovered or discoveree)
   */
  void StartDiscoveryApp (Ptr<NetDevice> ueDevice, uint32_t appCode, uint32_t dstL2Id, NrSlUeProse::DiscoveryRole role);

  /**
   * Stops discovery process for given applications 
   * \param ueDevice the targeted device
   * \param appCode application code to be removed
   * \param role UE role (discovered or discoveree)
   */
  void StopDiscoveryApp (Ptr<NetDevice> ueDevice, uint32_t appCode, NrSlUeProse::DiscoveryRole role);
  
  /**
   * Starts discovery process for given applications depending on the interest (monitoring or announcing)
   * \param ueDevice the targeted device
   * \param appCodes application code to be added
   * \param dstL2Ids destination layer 2 IDs to be set for each appCode
   * \param role UE role (discovered or discoveree)
   */
  void StartDiscovery (Ptr<NetDevice> ueDevice, std::list<uint32_t> appCodes, std::list<uint32_t> dstL2Ids, NrSlUeProse::DiscoveryRole role);
  
  /**
   * Stops discovery process for given applications 
   * \param ueDevice the targeted device
   * \param appCodes application codes to be removed
   * \param role UE role (discovered or discoveree)
   */
  void StopDiscovery (Ptr<NetDevice> ueDevice, std::list<uint32_t> appCodes, NrSlUeProse::DiscoveryRole role);

  /**
   * Starts relay discovery process depending on the interest (relay or remote)
   * \param ueDevice the targeted device
   * \param relayCode relay code 
   * \param dstL2Ids destination layer 2 ID
   * \param model UE model (A or B)
   * \param role UE role (relay or remote)
   */
  void StartRelayDiscovery (Ptr<NetDevice> ueDevice, uint32_t relayCode, uint32_t dstL2Id, NrSlUeProse::DiscoveryModel model, NrSlUeProse::DiscoveryRole role);
  
  /**
   * Stops relay discovery process for given code
   * \param ueDevice the targeted device
   * \param relayCode relay code to be removed
   * \param role UE role (relay or remote)
   */
  void StopRelayDiscovery (Ptr<NetDevice> ueDevice, uint32_t relayCode, NrSlUeProse::DiscoveryRole role);

   /**
   * Enable trace sinks for ProSe discovery
   */
  void EnableDiscoveryTraces (void);

protected:
  /**
   * \brief \c DoDispose method inherited from \c Object
   */
  virtual void DoDispose (void) override;

private:
  /**
  * \brief Prepare Single UE for ProSe
  *
  *  Install ProSe layer in the device and connect corresponding SAPs
  *
  * \param nrUeDev The Ptr to NR UE NetDevice
  */
  void PrepareSingleUeForProse (Ptr<NrUeNetDevice> nrUeDev);
  /**
  * \brief  Prepare UE for Unicast ProSe Direct Communication
  *
  * \param nrUeDev The Ptr to NR UE NetDevice
  */
  void PrepareSingleUeForUnicast (Ptr<NrUeNetDevice> nrUeDev);

  Ptr<NrPointToPointEpcHelper> m_epcHelper; //!< pointer to the EPC helper

  Ptr<NrSlDiscoveryTrace> m_discoveryTrace; //!< Container of discovery traces.
};

}

#endif /* NR_SL_PROSE_HELPER_H */


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

#ifndef LTE_SL_UE_CONTROLLER_CAMAD_2019_H
#define LTE_SL_UE_CONTROLLER_CAMAD_2019_H

#include <ns3/lte-ue-net-device.h>
#include <ns3/lte-sl-ue-net-device.h>
#include <ns3/lte-sidelink-helper.h>
#include "ns3/internet-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/random-variable-stream.h"

#include <ns3/lte-sl-ue-controller.h>
#include <ns3/lte-sl-ue-ctrl-sap.h>

namespace ns3 {

/**
 * \brief Implementation of the UE sidelink controller used for the
 * evaluations in the paper “System Level Evaluation of UE-to-Network Relays
 * in D2D-Enabled LTE Networks,” in the procedings of 2019 IEEE 24th
 * International Workshop on Computer Aided Modeling and Design of
 * Communication Links and Networks (CAMAD 2019).
 *
 */
class LteSlUeControllerCamad2019 : public LteSlUeController
{

public:
  LteSlUeControllerCamad2019 ();
  virtual ~LteSlUeControllerCamad2019 ();

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();

  virtual void SetLteSlUeCtrlSapUser (LteSlUeCtrlSapUser* s);
  virtual LteSlUeCtrlSapProvider* GetLteSlUeCtrlSapProvider ();

  /// let the forwarder class access the protected and private members
  friend class MemberLteSlUeCtrlSapProvider<LteSlUeControllerCamad2019>;

  /**
   * Sets the name of the simulation campaign
   * \param campaign The name of the simulation campaign
   */
  void SetCampaign (std::string campaign);
  /**
   * Set the start of the relay service
   * \param t The start of the relay service
   */
  void SetStartRelayServiceTime (Time t);

  /**
   * Set the discovery period duration
   * \param period The discovery period duration
   */
  void SetDiscoveryPeriodDuration (uint32_t period);

  /**
   * Sets the number of relays to discover
   * \param nRelays The number of relays to discover
   */
  void SetNumberProseRelaysToDiscover (uint32_t nRelays);

  /**
   * TracedCallback signature for discovering events.
   * \param [in] remoteId
   * \param [in] relayId
   * \param [in] timeToDis
   * \param [in] nPeriodstoDisc
   */
  typedef void (* DiscoveryEventTracedCallback)(const uint32_t remoteId, const uint32_t relayId,
                                                const double timeToDisc, const uint32_t nPeriodstoDisc);

  /**
   * TracedCallback signature for connection start events.
   * \param [in] remoteId
   * \param [in] relayId
   * \param [in] connectionCount
   */
  typedef void (* RemoteConnectionStartTracedCallback)(const uint32_t remoteId, const uint32_t relayId,
                                                       const uint32_t connectionCount);

  /**
   * TracedCallback signature for connection establised events.
   * \param [in] remoteId
   * \param [in] relayId
   * \param [in] connectionCount
   * \param [in] timeToConnect
   */
  typedef void (* RemoteConnectionEstablishedTracedCallback)(const uint32_t remoteId, const uint32_t relayId,
                                                             const uint32_t connectionCount, const double timeToConnect);
private:
  LteSlUeCtrlSapUser* m_slUeCtrlSapUser; ///< Controller SAP user
  LteSlUeCtrlSapProvider* m_slUeCtrlSapProvider; ///< Controller SAP provider

protected:
  // inherited from Object
  virtual void DoDispose ();


  // inherited from LteSlUeController
  virtual void DoRecvRelayServiceDiscovery (uint32_t serviceCode, uint64_t announcerInfo, uint32_t proseRelayUeId, uint8_t statusIndicator);
  virtual void DoPc5ConnectionStarted (uint32_t remoteUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role);
  virtual void DoPc5SecuredEstablished (uint32_t remoteUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role);
  virtual void DoPc5ConnectionTerminated (uint32_t remoteUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role);
  virtual void DoPc5ConnectionAborted (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role, LteSlO2oCommParams::UeO2ORejectReason reason);
  virtual void DoRecvRemoteUeReport (uint64_t localImsi, uint32_t peerUeId, uint64_t remoteImsi);
  virtual uint64_t DoRelayUeSelection (std::map <uint64_t, double> validRelays, uint32_t serviceCode, uint64_t currentRelayId );


  /** The UE netdevice to access different layers of the UE stack */
  Ptr <LteUeNetDevice> m_netDevice;

  /** The helper to assist with address configuration */
  Ptr <LteSidelinkHelper> m_lteSidelinkHelper;

  /** Map the relay UE layer 2 ID and the NetDevice used to communicate */
  std::map < uint32_t, Ptr<LteSlUeNetDevice> > m_lteSlUeNetDeviceMap;

  /** Probability (in %) to connect to a newly discovery relay if not already connected */
  uint16_t m_relayDiscProb;

  /** Random variable to decide initial relay connection */
  Ptr<UniformRandomVariable> m_relayDiscProbRndVar;

  /** Indicate the relay UE L2 ID the node is trying to connect */
  uint32_t m_connectingRelayUeId;

  /** CAMAD 2019 */
  std::string m_campaign;

  /** Discovery Campaign **/

  /**
   * Map the Relay UE ID of the discovered Relay UEs to the time they were discovered
   */
  std::map <uint32_t, Time> m_dicoveredRelayUeIds;

  /**
   * Time the UE started the Relay service
   */
  Time m_startRelayServiceTime;

  /**
   * The discovery period duration in ms
   */
  uint32_t m_discPeriodDuration;

  /**
   * The number of Relay UEs to discover
   */
  uint32_t m_nProseRelaysToDiscover;

  /**
   * Trace fired when the Remote UE discovered all the Relay UEs to discover
   */
  TracedCallback<uint32_t, uint32_t, double, uint32_t> m_traceDiscoversAllRelays;


  /** Connection Campaign **/

  /**
   * Time the remote UE started the connection with the relay UE
   */
  Time m_remoteStartedConnectionTime;

  /**
   * Number of connection
   */
  uint32_t m_connectionCount;

  /**
   * Maximum number of connection attempts
   */
  uint32_t m_maxConnectionAttempts;

  /**
   * Indicates if the remote UE connected to the relay UE
   */
  bool m_haveConnected;

  /**
   * Traces fired when the connection starts
   */
  TracedCallback<uint32_t, uint32_t, uint32_t> m_traceRemoteConnectionStart;

  /**
   * Traces fired when the connection is established
   */
  TracedCallback<uint32_t, uint32_t, uint32_t, double> m_traceRemoteConnectionEstablished;

}; // end of class LteSlUeController

} // namespace ns3

#endif /* LTE_SL_UE_CONTROLLER_CAMAD_2019_H */

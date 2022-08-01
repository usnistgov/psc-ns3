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

#ifndef NR_SL_UE_PROSE_H
#define NR_SL_UE_PROSE_H

#include "nr-sl-ue-service.h"
#include <ns3/nr-sl-ue-svc-rrc-sap.h>
#include <ns3/nr-sl-ue-svc-nas-sap.h>
#include <ns3/nr-sl-ue-prose-dir-lnk-sap.h>
#include <unordered_map>
#include <ns3/nr-sl-ue-prose-direct-link.h>
#include <ns3/traced-callback.h>
#include <ns3/nr-sl-discovery-header.h>


namespace ns3 {

class NrPointToPointEpcHelper;


/**
 * Store information of the active direct link instance
 * * \ingroup lte
 */
class NrSlUeProseDirLinkContext : public Object
{

public:
  NrSlUeProseDirLinkContext (void);
  virtual ~NrSlUeProseDirLinkContext (void);
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  Ptr<NrSlUeProseDirectLink> m_link; ///< Pointer to the direct link instance
  NrSlUeProseDirLnkSapProvider*  m_nrSlUeProseDirLnkSapProvider; ///< The pointer to the SAP in which the direct link receives calls from the ProSe layer
  NrSlUeProseDirLnkSapUser::DirectLinkIpInfo m_ipInfo; ///< The IP addresses used by the peers in the link
  bool m_hasPendingSlDrb; ///< Flag to indicate if the UE is having a SL-DRB pending for activation
  bool m_hasActiveSlDrb; ///< Flag to indicate if the UE has an active SL-DRB
  uint32_t m_relayServiceCode; ///< the relay service code associated to this direct link

};

/**
 * \ingroup lte
 *
 * This class implements an NR SL service layer providing Proximity Services (ProSe)
 */
class NrSlUeProse : public NrSlUeService
{

  /// allow MemberNrSlUeSvcRrcSapUser<NrSlUeProse> class friend access
  friend class MemberNrSlUeSvcRrcSapUser<NrSlUeProse>;
  /// allow MemberNrSlUeSvcNasSapUser<NrSlUeProse> class friend access
  friend class MemberNrSlUeSvcNasSapUser<NrSlUeProse>;
  /// allow MemberNrSlUeProseDirLnkSapUser<NrSlUeProse> class friend access
  friend class MemberNrSlUeProseDirLnkSapUser<NrSlUeProse>;

public:
  NrSlUeProse ();
  virtual ~NrSlUeProse (void);

  ///< The types of discovery role
  enum DiscoveryRole
  {
    Monitoring = 0, ///< Model A: The UE receiving discovery messages
    Announcing,     ///< Model A: The UE sending discovery messages
    Discoveree,     ///< Model B: The UE responding to requests
    Discoverer,     ///< Model B: The UE sending requests
    RemoteUE,
    RelayUE
  };

  ///< The discovery models supported
  enum DiscoveryModel
  {
    ModelA = 0,     ///< announce
    ModelB          ///< request/response
  };

  ///< Information for application discovery
  struct DiscoveryInfo
  {
    DiscoveryModel model; ///< discovery model used
    DiscoveryRole role; ///< role in the discovery 
    uint32_t appCode; ///< application code 
    uint32_t dstL2Id; ///< destination L2 ID
  };


protected:
  virtual void DoDispose ();

public:

  /**
   *  \brief Register this type.
   *
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Get the pointer of the UE service layer SAP User interface
   *        offered to UE RRC by this class
   *
   * \return the pointer of type NrSlUeSvcRrcSapUser
   */
  NrSlUeSvcRrcSapUser* GetNrSlUeSvcRrcSapUser ();
  /**
   * \brief Set the pointer for the UE service layer SAP Provider interface
   *        offered to this class by UE RRC class
   *
   * \param s the pointer of type NrSlUeSvcRrcSapProvider
   */
  void SetNrSlUeSvcRrcSapProvider (NrSlUeSvcRrcSapProvider* s);

  /**
   * \brief Get the pointer of the UE service layer SAP User interface
   *        offered to EPC UE NAS by this class
   *
   * \return the pointer of type NrSlUeSvcNasSapUser
   */
  NrSlUeSvcNasSapUser* GetNrSlUeSvcNasSapUser ();
  /**
   * \brief Set the pointer for the UE service layer SAP Provider interface
   *        offered to this class by EPC UE NAS class
   *
   * \param s the pointer of type NrSlUeSvcNasSapProvider
   */
  void SetNrSlUeSvcNasSapProvider (NrSlUeSvcNasSapProvider* s);

  /**
   * \brief Get the pointer of the ProSe Direct Link SAP User interface
   *        offered to the Prose Direct Links by this class
   *
   * \return the pointer of type NrSlUeProseDirLnkSapUser
   */
  NrSlUeProseDirLnkSapUser* GetNrSlUeProseDirLnkSapUser ();

  /**
   * \brief Configure the parameters required by the UE to perform ProSe
   *        unicast communication
   */
  void ConfigureUnicast ();

  /**
   * \brief Configure the monitoring of layer 2 IDs required by the UE to
   *        perform ProSe discovery
   *
   * \param dstL2Id a destination layer 2 ID to monitor
   */
  void ConfigureL2IdMonitoringForDiscovery (uint32_t dstL2Id);

  /**
   * \brief Add a new direct link connection with the given peer if possible
   *
   * \param selfL2Id the layer 2 ID of this UE
   * \param selfIp the IPv4 address used by this UE
   * \param peerL2Id the layer 2 UD of the peer UE
   * \param isInitiating flag indicating if the UE is initiating the procedure (true)
   *                     or adding the link as result of a request by the peer UE (false)
   * \param isRelayConn flag indicating if the direct link is for a relay connection (true)
   * \param relayServiceCode the relay service code associated to this direct link
   */
  void AddDirectLinkConnection (uint32_t selfL2Id, Ipv4Address selfIp, uint32_t peerL2Id,
                                bool isInitiating, bool isRelayConn,  uint32_t relayServiceCode);
  /**
   * Map to store direct link context instances indexed by direct link id
   */
  typedef std::unordered_map <uint32_t, Ptr<NrSlUeProseDirLinkContext> > NrSlDirectLinkContextMapPerPeerL2Id;

  /**
   * Map to keep track of the active SL-SRBs. A bit set of 4 bit representing each of the
   * 4 LcIds of the SL-SRBs is stored per peerL2Id.
   * Bit set to 1 means the SL-SRB of the corresponding LcId is active
   */
  typedef std::unordered_map <uint32_t, std::bitset<4> > NrSlSingalingRadioBearersPerPeerL2Id;

  /**
   * Parameters for L3 UE-to-Network relay configuration
   */
  struct NrSlL3U2nServiceConfiguration
  {
    uint8_t relayDrbId; //!< the UL data radio bearer ID to be used for L3 UE-to-Network relay
  };

  /**
   * Map to store L3 UE-to-Network relay configurations indexed by relay service code
   */
  typedef std::unordered_map <uint32_t, NrSlL3U2nServiceConfiguration > NrSlL3U2nRelayServices;


  /**
   * \brief Add an L3 UE-to-Network relay service that the UE in the role of relay UE will provide.
   *
   * \param relayServiceCode the relay service code associated to the service
   * \param config the parameters associated to the service
   */
  void AddL3U2nRelayServiceConfiguration (uint32_t relayServiceCode, NrSlL3U2nServiceConfiguration config);

  /**
   * \brief Set the IMSI used by the UE
   *
   * \param imsi the IMSI of the UE
   */
  void SetImsi (uint64_t imsi);

  /**
   * \brief Set the Layer 2 ID used by the UE
   *
   * \param l2Id the Layer 2 ID  of the UE
   */
  void SetL2Id (uint32_t l2Id);

  /**
   * \brief Set EPC helper
   *
   * \param epcHelper Ptr of type NrPointToPointEpcHelper
   */
  void SetEpcHelper (const Ptr<NrPointToPointEpcHelper> &epcHelper);

  /**
    * TracedCallback signature for transmission and reception of PC5-S messages.
    *
    * \param [in] srcL2Id the layer 2 ID of the source of the message
    * \param [in] dstL2Id the layer 2 ID of the destination of the message
    * \param [in] isTx flag indicating if the trace was called upon a transmission
    * \param [in] p the PC5-S message
    */
  typedef void (* PC5SignallingPacketTracedCallback)(uint32_t srcL2Id, uint32_t dstL2Id, bool isTx, Ptr<Packet> p);
  
  /**
    * TracedCallback signature for transmission of discovery messages.
    *
    * \param [in] senderL2Id the layer 2 ID of the source of the message
    * \param [in] receiverL2Id the layer 2 ID of the destination of the message
    * \param [in] discHeader the discovery message
    */
  typedef void (* DiscoveryTraceTracedCallback)(uint32_t senderL2Id, uint32_t receiverL2Id, NrSlDiscoveryHeader discHeader);
  
  /**
   * \brief Add discovery application
   * Add payload depending on the interest (monitoring or announcing)
   * \param appCode application code to be announced or monitored
   * \param dstL2Id destination layer 2 ID to be set for this appCode
   * \param role Indicates if announcing or monitoring
   */
  void AddDiscoveryApp (uint32_t appCode, uint32_t dstL2Id, DiscoveryRole role);

  /**
   * \brief Remove Sidelink discovery applications
   * Remove application from discovery map
   * \param appCode application code to be removed
   * \param role Indicates if announcing or monitoring
   */
  void RemoveDiscoveryApp (uint32_t appCode, DiscoveryRole role);

  /**
   * Checks if the given app must be discovered
   * \param msgType The message type
   * \param appCode The application code
   * \return true if the node is monitoring for this message type and appCode
   */
  bool IsMonitoringApp (uint8_t msgType, uint32_t appCode);

  /**
   * \brief Send discovery message
   * \param appCode Application code
   * \param dstL2Id destination L2 ID
   */
  void SendDiscovery (uint32_t appCode, uint32_t dstL2Id);

  /**
   * \brief Add discovery relay
   * Add relay code depending on the interest (relay or remote)
   * \param relayCode relay code to consider
   * \param dstL2Id destination layer 2 ID 
   * \param model can be model A or model B
   * \param role Indicates if the UE acts as relay or remote
   */
  void AddRelayDiscovery (uint32_t relayCode, uint32_t dstL2Id, DiscoveryModel model, DiscoveryRole role);

  /**
   * \brief Remove Sidelink discovery relay
   * Remove relay code from list
   * \param relayCode relay code
   * \param role role can be relay or remote
   */
  void RemoveRelayDiscovery (uint32_t relayCode, DiscoveryRole role);

  /**
   * Indicates if the device is monitoring messages for the given relay code
   * \param msgType The message type received
   * \param relayCode relay code to use
   * \return true if the node is monitoring for this message type and relayCode
   */
  bool IsMonitoringRelay (uint8_t msgType, uint32_t relayCode);

  /**
   * \brief Initiate relay discovery
   * \param relayCode relay code to consider
   * \param dstL2Id destination layer 2 ID 
   */
  void SendRelayDiscovery (uint32_t relayCode, uint32_t dstL2Id);
  
  /**
   * Map to keep track of the active SL Discovery RBs. 
   * A bit set of 4 bits representing each of the 4 LcIds of the SL-SRBs is stored per peerL2Id.
   * Bit set to 1 means the SL-SRB of the corresponding LcId is active
   */
  typedef std::list <uint32_t> NrSlDiscoveryRadioBearers;

  ///< Frequency of Discovery messages in seconds
  Time m_discoveryInterval;


private:
  //NrSlUeSvcRrcSapUser methods
  void DoReceiveNrSlSignalling (Ptr<Packet> packet, uint32_t srcL2Id);
  void DoNotifySvcNrSlDataRadioBearerActivated (uint32_t peerL2Id);
  void DoReceiveNrSlDiscovery (Ptr<Packet> packet, uint32_t srcL2Id);

  //NrSlUeProseDirLnkSapUser methods
  void DoSendNrSlPc5SMessage (Ptr<Packet> packet, uint32_t dstL2Id,  uint8_t lcId);
  void DoNotifyChangeOfDirectLinkState (uint32_t peerL2Id, NrSlUeProseDirLnkSapUser::ChangeOfStateNotification info);
  void DoSendNrSlDiscovery (Ptr<Packet> packet, uint32_t dstL2Id);

  /**
   * Trace information upon transmission and reception of PC5-S messages
   */
  TracedCallback< uint32_t, uint32_t, bool, Ptr<Packet> > m_pc5SignallingPacketTrace;


  /**
   * Track the transmission of discovery message
   * Exporting sender L2 ID, receiver L2 ID, transmission or not flag, and discovery message.
   */
  TracedCallback<uint32_t, uint32_t, bool, NrSlDiscoveryHeader> m_discoveryTrace;

  //SAP pointers
  NrSlUeSvcNasSapUser* m_nrSlUeSvcNasSapUser; ///< NR SL UE SERVICE NAS SAP user
  NrSlUeSvcNasSapProvider* m_nrSlUeSvcNasSapProvider {nullptr}; ///< NR SL UE SERVICE NAS SAP Provider

  NrSlUeSvcRrcSapUser* m_nrSlUeSvcRrcSapUser; ///< NR SL UE SERVICE SAP User
  NrSlUeSvcRrcSapProvider* m_nrSlUeSvcRrcSapProvider {nullptr}; ///< NR SL UE SERVICE SAP Provider

  NrSlUeProseDirLnkSapUser * m_nrSlUeProseDirLnkSapUser; ///< ProSe Direct Link SAP User

  //Class internal private methods and member variables
  NrSlDirectLinkContextMapPerPeerL2Id m_unicastDirectLinks;  ///< Active direct link contexts map
  NrSlSingalingRadioBearersPerPeerL2Id m_activeSlSrbs;  ///< Active SL signalling radio bearers
  NrSlL3U2nRelayServices m_l3U2nRelayProvidedSvcs; ///< UE-to-Network Relay services provided by the UE

  uint64_t m_imsi;  //!< the IMSI used by the UE
  uint32_t m_l2Id; ///< the L2Id used by this UE

  Ptr<NrPointToPointEpcHelper> m_epcHelper; //!< pointer to the EPC helper

  ///< List of active discovery RBs
  NrSlDiscoveryRadioBearers m_activeSlDiscoveryRbs;

   ///< list of IDs of applications to announce/response
  std::map <uint32_t, DiscoveryInfo> m_discoveryMap;

  ///< List of relay codes 
  std::map <uint32_t, DiscoveryInfo> m_relayMap;

  /**
   * \brief Creates the TFT and instructs the activation of the corresponding
   *        data radior bearer for a direct link
   *
   * \param peerL2Id the L2 ID of the peer UE in the link
   * \param ipInfo the IP configuration to use
   */
  void ActivateDirectLinkDataRadioBearer (uint32_t peerL2Id, NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo);

  /**
   * \brief Instruct the (re)configuration of the corresponding data radio
   * bearer when the UE is participating in a UE-to-Network relay service
   *
   * \param peerL2Id the L2 ID of the peer UE in the link
   * \param relayInfo the UE-to-Network relay configuration to use
   * \param ipInfo the IP configuration to use
   */
  void ConfigureDataRadioBearersForU2nRelay (uint32_t peerL2Id, NrSlUeProseDirLnkSapUser::DirectLinkRelayInfo relayInfo, NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo);

};//end of NrSlUeProse class definition

} //namespace ns3

#endif /* NR_SL_UE_PROSE_H */

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

#ifndef LTE_SL_UE_RRC_H
#define LTE_SL_UE_RRC_H

#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/traced-callback.h>
#include <ns3/event-id.h>

#include <ns3/lte-rrc-sap.h>
#include <ns3/lte-sl-pool.h>
#include <ns3/lte-sl-header.h>
#include <ns3/lte-sl-o2o-comm-params.h>
#include <ns3/lte-radio-bearer-info.h>
#include <ns3/random-variable-stream.h>

#include <map>
#include <set>
#include <vector>

namespace ns3 {

class LteUeRrc;
class LteSlUeController;
class LteSlUeCtrlSapUser;
class LteSlUeCtrlSapProvider;

/**
 * \ingroup lte
 * Manages Sidelink information for this UE
 */
class LteSlUeRrc : public Object
{
  /// allow LteUeRrc class friend access
  friend class LteUeRrc;

  /**
   * Structure to store Sidelink configuration for a given plmnid
   */
  struct LteSlCellConfiguration
  {
    uint16_t cellId; ///< Cell id
    bool haveSib18; ///< Have Sib18 ? May need to be replaced with another struct containing frame information
    LteRrcSap::SystemInformationBlockType18 sib18; ///< SIB18
    bool haveSib19; ///< Have SIB19 ?
    LteRrcSap::SystemInformationBlockType19 sib19; ///< SIB 19
  };

public:
  LteSlUeRrc ();
  virtual ~LteSlUeRrc (void);

  /**
   * The types of discovery role
   */
  enum DiscoveryRole
  {
    Monitoring = 0,
    Announcing,
    Discoveree,     ///< The UE responding to requests
    Discoverer      ///< The UE sending requests
  };

  /**
   * The discovery models supported
   */
  enum DiscoveryModel
  {
    ModelA = 0,     //announce
    ModelB          //request/response
  };

  /**
   * The role of the UE in the UE-to-Network relay process
   */
  enum RelayRole
  {
    RemoteUE = 0,
    RelayUE
  };

  /**
   * Information for application discovery
   */
  struct AppServiceInfo
  {
    DiscoveryModel model; ///< discovery model used
    DiscoveryRole role; ///< role in the discovery 
    uint32_t appCode; ///< application code 
    EventId txTimer; ///< timer for transmitting announcement or requests
    Time lastRspTimestamp; ///< time of the last response, used to avoid sending multiple times in the same sidelink period
  };

  /**
   * Information for relay service discovery
   */
  struct RelayServiceInfo
  {
    DiscoveryModel model; ///< discovery model used
    RelayRole role; ///< role in the relay operation
    uint32_t serviceCode; ///< relay service code
    EventId txTimer; ///< timer for transmitting announcement or requests
    Time lastRspTimestamp; ///< time of the last response, used to avoid sending multiple times in the same sidelink period
  };

  /**
   * \brief makes a copy of the sidelink configuration
   * \return a copy of the sidelink configuration
   */
  Ptr<LteSlUeRrc> Copy ();

  //Controller SAPs
  /**
   * Set the Provider part of the LteSlUeCtrlSap that this SL UE RRC entity will
   * interact with
   *
   * \param s
   */
  virtual void SetLteSlUeCtrlSapProvider (LteSlUeCtrlSapProvider* s);

  /**
   *
   * \return the User part of the LteSlUeCtrlSap
   *
   */
  virtual LteSlUeCtrlSapUser* GetLteSlUeCtrlSapUser ();

/// inherited from Object
protected:
  virtual void DoInitialize ();
  virtual void DoDispose ();
public:
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Set the Sidelink communication enabled flag
   * \param status True to enable. False to disable
   */
  void SetSlEnabled (bool status);
  /**
   * \brief Is Sidelink enabled function
   * \return True if Sidelink communication is enabled, false otherwise
   */
  bool IsSlEnabled ();
  /**
   * \brief Set the Sidelink discovery enabled flag
   * \param status True to enable. False to disable
   */
  void SetDiscEnabled (bool status);
  /**
   * \brief Is Sidelink discovery enabled function
   * \return True if Sidelink discovery is enabled, false otherwise
   */
  bool IsDiscEnabled ();
  /**
   * \brief Set Sidelink pre-configuration function
   * \param preconfiguration LteRrcSap::SlPreconfiguration
   */
  void SetSlPreconfiguration (LteRrcSap::SlPreconfiguration preconfiguration);
  /**
   * \brief Get Sidelink pre-configuration function
   * \return LteRrcSap::SlPreconfiguration
   */
  LteRrcSap::SlPreconfiguration GetSlPreconfiguration ();
  /**
   * \brief Set Sidelink source layer 2 id function
   * \param src The Sidelink layer 2 id of the source
   */
  void SetSourceL2Id (uint32_t src);
  /**
   * \brief Get Sidelink source layer 2 id function
   * \return The Sidelink layer 2 id of the source
   */
  uint32_t GetSourceL2Id ();
  /**
   * \brief Set Sidelink discovery Tx resources function
   * \param numDiscTxRes Number of resources the UE requires every discovery period
   */
  void SetDiscTxResources (uint8_t numDiscTxRes);
  /**
   * \brief Set discovery interested frequency
   * \param ulEarfcn The frequency that the UE is supposed to monitor for discovery announcements
   */
  void SetDiscInterFreq (uint16_t ulEarfcn);
  /**
   * \brief Get discovery Tx resources function
   * \return Number of resources the UE requires every discovery period
   */
  uint8_t GetDiscTxResources ();
  /**
   * \brief Get discovery inter frequency function
   * \return The frequency that the UE is supposed to monitor for discovery announcements
   */
  uint16_t GetDiscInterFreq ();
  /**
   * \brief Is Tx interested function
   * Indicates if the UE is interested in sidelink transmission
   * \return True if the UE is interested in sidelink transmission
   */
  bool IsTxInterested ();
  /**
   * Indicates if the UE is interested in sidelink transmission
   * \return True if the UE is interested in sidelink transmission
   */
  bool IsRxInterested ();
  /**
   * \brief Is announcing monitoring function
   * Indicates if the UE is interested in discovery Monitoring
   * \return True if the UE is interested in discovery Monitoring
   */
  bool IsMonitoringInterested ();
  /**
   * \brief Is announcing interested function
   * Indicates if the UE is interested in discovery Announcing
   * \return True if the UE is interested in discovery Announcing
   */
  bool IsAnnouncingInterested ();
  /**
   * Schedule transmission of announcements/requests
   */
  void StartAnnouncing ();
  /**
   * \brief Get Tx destination function
   * Returns the list of Sidelink communication destinations this UE is interested in transmitting to
   * \return the list of destinations
   */
  std::list <uint32_t> GetTxDestinations ();
  /**
   * \brief Add Sidelink radio bearer function
   * Attempts to add a sidelink radio bearer
   * \param slb LteSidelinkRadioBearerInfo pointer
   * \return True if the sidelink was successfully added, else false such as when
   * an identical bearer already exists
   */
  bool AddSidelinkRadioBearer (Ptr<LteSidelinkRadioBearerInfo> slb);
  /**
   * \brief Delete Sidelink radio bearer function
   * Deletes a sidelink radio bearer
   * \param src Sidelink source layer 2 id
   * \param group The group identifying the radio bearer to delete
   * \return true if the bearer was successfully deleted
   */
  bool DeleteSidelinkRadioBearer (uint32_t src, uint32_t group);
  /**
   * \brief Get Sidelink radio bearer function
   * \param src Sidelink source layer 2 id
   * \param group Sidelink group id
   * \return The radio bearer for the given group
   */
  Ptr<LteSidelinkRadioBearerInfo> GetSidelinkRadioBearer (uint32_t src, uint32_t group);
  /**
   * \brief Get Sidelink radio bearer function
   * \param group Sidelink group id
   * \return The transmit radio bearer for the given group
   */
  Ptr<LteSidelinkRadioBearerInfo> GetSidelinkRadioBearer (uint32_t group);
  /**
   * \brief Add discovery applications
   * Add payloads depending on the interest (monitoring or announcing)
   * \param payloads payloads to be added
   * \param role Indicates if announcing or monitoring
   */
  void StartDiscoveryApps (std::list<uint32_t> payloads, DiscoveryRole role);

  /**
   * \brief Remove Sidelink discovery applications
   * Remove applications depending on the interest (monitoring or announcing)   *
   * \param payloads payloads to be removed
   * \param role Indicates if announcing or monitoring
   */
  void StopDiscoveryApps (std::list<uint32_t> payloads, DiscoveryRole role);
  /**
   * \brief Record transmission time of the Sidelink UE information
   */
  void RecordTransmissionOfSidelinkUeInformation ();
  /**
   * \brief Get time since last transmission of the SidelinkUeInformation
   * \return The time elapse since the transmission of the last SidelinkUeInformation
   */
  double GetTimeSinceLastTransmissionOfSidelinkUeInformation ();
  /**
   * \brief Get next LCID for setting up SLRB towards the given destination
   * \param dstL2Id The destination layer 2 ID
   * \return the next available LCID
   */
  uint8_t GetNextLcid (uint32_t dstL2Id);
  /**
   * \brief Is cell broadcasting SIB 18 function
   * Indicates if cell is broadcasting SIB 18
   * \param cellId The cell ID
   * \return true if the SIB 18 was received from the cell
   */
  bool IsCellBroadcastingSIB18 (uint16_t cellId);
  /**
   * \brief Is cell broadcasting SIB 19 function
   * Indicates if cell is broadcasting SIB 19
   * \param cellId The cell ID
   * \return true if the SIB 19 was received from the cell
   */
  bool IsCellBroadcastingSIB19 (uint16_t cellId);
  /**
   * Checks if the given app must be discovered
   * \param msgType The message type
   * \param appCode The application code
   * \return true if the node is monitoring for this message type and appCode
   */
  bool IsMonitoringApp (uint8_t msgType, uint32_t appCode);

  /**
   * Checks if the given app must be announced
   * \param appCode The application code
   * \return true if the UE is announcing the given application code
   */
  bool IsAnnouncingApp (uint32_t appCode);

  /**
   * Set active discovery pool
   * \param pool transmission pool
   */
  void SetActiveTxDiscoveryPool (Ptr<SidelinkTxDiscResourcePool> pool);

  /**
   * Get active discovery pool
   * \return active discovery pool
   */
  Ptr<SidelinkTxDiscResourcePool> GetActiveTxDiscoveryPool ();

  /**
   * Trigger transmission of application discovery message
   * \param appCode The Prose Application Code
   */
  void TransmitApp (uint32_t appCode);

  /**
   * Starts UE-to-Network relay process
   * \param serviceCode relay service code to use
   * \param model discovery model (A or B)
   * \param role UE role (remote UE or relay node)
   */
  void StartRelayService (uint32_t serviceCode, LteSlUeRrc::DiscoveryModel model, LteSlUeRrc::RelayRole role);

  /**
   * Stops UE-to-Network relay process
   * \param serviceCode relay service code to use
   */
  void StopRelayService (uint32_t serviceCode);

  /**
   * Trigger transmission of a relay service discovery message
   * \param serviceCode relay service code to use
   */
  void TransmitRelayMessage (uint32_t serviceCode);

  /**
   * Indicates if the device is monitoring messages for the given service code
   * \param msgType The message type received
   * \param serviceCode relay service code to use
   * \return true if the UE is monitoring for this message type and service code
   */
  bool IsMonitoringRelayServiceCode (uint8_t msgType, uint32_t serviceCode);

  /**
   * Start of One to One Relay Direct Communication
   * \param serviceCode The relay service code
   * \param proseRelayUeId The layer 2 ID for the relay node
   */
  void StartRelayDirectCommunication (uint32_t serviceCode, uint32_t proseRelayUeId);

  /**
   * TracedCallback signature for reception of PC5 Signaling message.
   *
   * \param [in] p
   */
  typedef void (* PC5SignalingPacketTracedCallback)(uint32_t srcL2Id, uint32_t dstL2Id, Ptr<Packet> p);

  /**
   * Evaluate Relay UE (re)selection upon reception of the L3 SD-RSRP measurement
   * report
   *
   * \param validRelays the list of valid detected Relay UEs and their respective SD-RSRP values
   * indexed by the Relay UE Ids and the service code
   */
  void RelayUeSelection (std::map <std::pair <uint64_t, uint32_t>, double> validRelays);

protected:
  /**
   * Notification of the received application discovery message
   * \param msgType The message type
   * \param appCode The application code
   */
  void RecvApplicationServiceDiscovery (uint8_t msgType, uint32_t appCode);

  /**
   * Notification of the received relay discovery announcement/response message
   * \param serviceCode The relay service code
   * \param announcerInfo The announcer info included in the discovery message
   * \param proseRelayUeId The layer 2 ID for the relay node
   * \param statusIndicator The status field
   */
  void RecvRelayServiceDiscovery (uint32_t serviceCode, uint64_t announcerInfo, uint32_t proseRelayUeId, uint8_t statusIndicator);

  /**
   * Notification of the received relay discovery request message
   * \param serviceCode The relay service code
   * \param discovererInfo The announcer info included in the discovery message
   * \param urdsComposition The URDS composition field
   * \param proseRelayUeId The layer 2 ID for the relay node
   */
  void RecvRelayServiceDiscovery (uint32_t serviceCode, uint64_t discovererInfo,uint8_t urdsComposition, uint32_t proseRelayUeId);

  /**
   * Indicate that a PC5 Data Message was received
   * \param srcL2Id The Source Layer 2 ID
   * \param dstL2Id The Destination Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void RecvPc5DataMessage (uint32_t srcL2Id, uint32_t dstL2Id, Ptr<Packet> pdcpSdu);

  /**
   * Receive PC5 Signaling Message in One to One Relay Direct Communication State Machine
   * \param srcL2Id The Source Layer 2 ID
   * \param dstL2Id The Destination Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void RecvPc5SignalingMessage (uint32_t srcL2Id, uint32_t dstL2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Communication Request message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectCommunicationRequest (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Communication Accept message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectCommunicationAccept (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Communication Reject message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectCommunicationReject (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Communication Keepalive message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectCommunicationKeepalive (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Communication Keepalive Acknowledgment message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectCommunicationKeepaliveAck (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Communication Release message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectCommunicationRelease (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Communication Release Accept message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectCommunicationReleaseAccept (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Security Mode Command message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectSecurityModeCommand (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Security Mode Complete message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectSecurityModeComplete (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Security Mode Reject message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectSecurityModeReject (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Rekeying Request message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectRekeyingRequest (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Rekeying Response message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectRekeyingResponse (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Direct Rekeying Trigger message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessDirectRekeyingTrigger (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Remote UE Info Request message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessRemoteUeInfoRequest (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Process received Remote UE Info Response message in One to One Relay Direct Communication State Machine
   * \param L2Id The Layer 2 ID
   * \param pdcpSdu The received PDCP SDU packet
   */
  void ProcessRemoteUeInfoResponse (uint32_t L2Id, Ptr<Packet> pdcpSdu);

  /**
   * Called upon expiry of Timer 4100 in One to One Relay Direct Communication State Machine
   * \param cId The context associated with the state machine
   */
  void Timer4100Expiry (LteSlO2oCommParams::LteSlPc5ContextId cId);

  /**
   * Called upon expiry of Timer 4111 in One to One Relay Direct Communication State Machine
   * \param cId The context associated with the state machine
   */
  void Timer4111Expiry (LteSlO2oCommParams::LteSlPc5ContextId cId);

  /**
   * Called upon expiry of Timer 4108 in One to One Relay Direct Communication State Machine
   * \param cId The context associated with the state machine
   */
  void Timer4108Expiry (LteSlO2oCommParams::LteSlPc5ContextId cId);

  /**
   * Called upon expiry of Timer 4103 in One to One Relay Direct Communication State Machine
   * \param cId The context associated with the state machine
   */
  void Timer4103Expiry (LteSlO2oCommParams::LteSlPc5ContextId cId);

  /**
   * Called upon expiry of Timer 4102 in One to One Relay Direct Communication State Machine
   * \param cId The context associated with the state machine
   */
  void Timer4102Expiry (LteSlO2oCommParams::LteSlPc5ContextId cId);

  /**
   * Called upon expiry of Timer 4101 in One to One Relay Direct Communication State Machine
   * \param cId The context associated with the state machine
   */
  void Timer4101Expiry (LteSlO2oCommParams::LteSlPc5ContextId cId);

  /**
   * Called upon expiry of Timer RUIR in One to One Relay Direct Communication State Machine
   * \param cId The context associated with the state machine
   */
  void TimerRUIRExpiry (LteSlO2oCommParams::LteSlPc5ContextId cId);

  /**
   * Indicates if the proseUeId is a known peer for one to one communication
   * \param proseUeId The layer 2 ID to check
   * \return true if the proseUeId is a peer for one to one communication
   */
  bool IsOneToOnePeer (uint32_t proseUeId);

  /**
   * Notification that the bearer with the given peer has been established
   * \param proseUeId The layer 2 ID of the peer node
   */
  void NotifySidelinkRadioBearerActivated (uint32_t proseUeId);

  /**
   * Update timers associated with keep alive procedures
   * \param context The link context
   */
  void UpdateKeepAlive (Ptr<LteSlO2oCommParams> context);

  /**
   * Release one-to-one communication for the given reason
   * \param context The link context
   * \param reason The reason for releasing the connection
   */
  void ReleaseO2OConnection (Ptr<LteSlO2oCommParams> context, LteSlO2oCommParams::UeO2OReleaseReason reason);

  /**
   * Gets the context for processing the packet received from a peer node
   * \param peerUeId The remote peer that sent the packet
   * \param contextId The context ID
   * \return the context if known
   */
  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> >::iterator GetO2OContext (uint32_t peerUeId, uint32_t contextId);

  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this class. Return the number of stream indices assigned.
   *
   * \param stream first stream index to use
   * \return the number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);

private:
  /**
   * Reference to the RRC layer
   */
  Ptr<LteUeRrc> m_rrc;
  /**
   * Indicates if sidelink is enabled
   */
  bool m_slEnabled;
  /**
   * Indicates if discovery is enabled
   */
  bool m_discEnabled;
  /**
   * The preconfiguration for out of coverage scenarios
   */
  LteRrcSap::SlPreconfiguration m_preconfiguration;
  /**
   * Map between cell ID and sidelink configuration
   */
  std::map <uint16_t, LteSlCellConfiguration> m_slMap;
  /**
   * Map between source, destination, and radio bearer for transmissions
   */
  std::map <uint32_t, std::map <uint32_t, std::list < Ptr<LteSidelinkRadioBearerInfo> > > > m_slrbMap;
  /**
   * The time the last SidelinkUeInformation was sent
   */
  Time m_lastSidelinkUeInformationTime;
  /**
   * The source layer 2 ID to be used
   */
  uint32_t m_sourceL2Id;
  /**
   * List of groups for which the UE is interested in receiving and may or may not have
   * radio bearers yet
   */
  std::list<uint32_t> m_rxGroup;
  /**
   * List of l2 ID for which the UE having one-to-one direct communication
   */
  std::list<uint32_t> m_oneToOnePeer;
  /**
   * List of Sidelink communication receiving pools
   */
  std::vector< std::pair <LteRrcSap::SlCommResourcePool, Ptr<SidelinkRxCommResourcePool> > > rxPools; ///<
  /**
   * list of IDs of applications to announce/response
   */
  std::map <uint32_t, AppServiceInfo> m_appServicesMap;
  /**
   * list of relay services used by this device
   */
  std::map <uint32_t, RelayServiceInfo> m_relayServicesMap;
  /**
   * Active Tx pool for discovery
   */
  Ptr<SidelinkTxDiscResourcePool> m_activeDiscTxPool;
  /**
   * List of discovery receiving pools
   */
  std::vector< std::pair <LteRrcSap::SlDiscResourcePool, Ptr<SidelinkRxDiscResourcePool> > > monitorPools;
  /**
   * discTxResourceReq : number of resources the UE requires every discovery period
   * for transmitting sidelink direct discovery announcement
   * i.e. number of separate discovery message(s) the UE wants to transmit every discovery period
   * (to be set in the scenario)
   */
  uint8_t m_discTxResources;
  /**
   * frequency that the UE is supposed to monitor for discovery announcements
   */
  uint16_t m_discInterFreq;

  Ptr<UniformRandomVariable> m_rand; ///< Uniform random variable

  /**
   * One to One Communication Contexts for this particular UE
   */

  std::map< LteSlO2oCommParams::LteSlPc5ContextId, Ptr<LteSlO2oCommParams> > m_o2oCommContexts;
  /**
   * Map of L2Id and corresponding IMSI for this particular UE
   */
  std::map< uint32_t, uint64_t > m_l2Id2ImsiMap;

  /**
   * Logic for sidelink
   */
  Ptr<LteSlUeController> m_controller;

  /**
   * Frequency of Model B Relay Discovery Solicitation messages (number of discovery periods)
   */
  uint16_t m_relaySolFreq;

  /**
   * The `m_pc5SignalingPacketTrace` trace source. Track the reception of PC5 Signaling message
   * Exporting PC5 Signaling packet.
   */
  TracedCallback< uint32_t, uint32_t, Ptr<Packet> > m_pc5SignalingPacketTrace;

  /**
   * Sequence number for PC5 signaling messages
   */
  uint32_t m_pc5SignalingSeqNum;

  /**
   * Activate or deactivate Remote UE information request procedure
   */
  bool m_isRuirqEnabled;

  LteSlUeCtrlSapUser* m_slUeCtrlSapUser; ///< Controller SAP user
  LteSlUeCtrlSapProvider* m_slUeCtrlSapProvider; ///< Controller SAP provider

};     //end of LteSlUeRrc'class

} // namespace ns3

#endif // LTE_SL_UE_RRC_H

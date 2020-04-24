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

#ifndef LTE_SL_O2O_COMM_PARAMS_H
#define LTE_SL_O2O_COMM_PARAMS_H

#include <ns3/object.h>
#include <ns3/nstime.h>
#include "ns3/timer.h"
#include <ns3/lte-sl-pc5-signalling-header.h>

namespace ns3 {

/**
 * This class implements the state machine of a one-to-one direct communication
 * link.
 *
 */

class LteSlO2oCommParams : public Object
{

public:
  /**
   * Structure to identify and link context
   * We use the sequence number of the connection request to identify each new request
   * That number is then used in all subsequent packets to "simulate" the fact that
   * packets are secured once the link is established.
   */
  struct LteSlPc5ContextId
  {
    uint32_t peerL2Id; ///< The L2 ID of the peer node
    uint32_t contextId; ///< The identifier for a given link with peer node

    /**
     * Comparison operator
     *
     * \param lhs first context
     * \param rhs second context
     * \returns true if the first context has lower value than the second
     */
    friend bool operator < (const LteSlPc5ContextId& lhs, const LteSlPc5ContextId& rhs)
    {
      return lhs.peerL2Id < rhs.peerL2Id || ( lhs.peerL2Id == rhs.peerL2Id && lhs.contextId < rhs.contextId);
    }
  };

  /**
   * The state of the direct security mode procedure
   */
  enum DirectSecurityModeState
  {
    EMPTY = 0,
    COMMANDED,
    COMPLETED,
  };

  /**
   * The Relay or Remote UE states
   */
  enum UeO2OCommState
  {
    RELAY_IDLE = 0,
    RELAY_SETUP_REQUEST,
    RELAY_SECURE_ESTABLISHED,
    RELAY_INIT_RELEASE,
    REMOTE_IDLE,
    REMOTE_INIT_SETUP,
    REMOTE_SECURE_ESTABLISHED,
    REMOTE_INIT_RELEASE
  };
  /**
   * Constructor
   */
  LteSlO2oCommParams ();

  /**
   * Destructor
   */
  virtual ~LteSlO2oCommParams ();

/**
 * Clears all pending timers
 */
  void ClearTimers ();

// inherited from Object
private:
  virtual void DoInitialize (void);
  virtual void DoDispose (void);

  /**
   * The duration value (in ms) for T4111 timer
   */
  uint32_t m_relay_dT4111;

  /**
   * The duration value (in ms) for T4103 timer
   */
  uint32_t m_relay_dT4103;

  /**
   * The duration value (in ms) for T4108 timer
   */
  uint32_t m_relay_dT4108;

  /**
   * The duration value (in ms) for TRUIR timer
   */
  uint32_t m_relay_dTRUIR;

  /**
   * T4111 timer
   */
  Timer* m_relay_T4111;

  /**
   * T4103 timer
   */
  Timer* m_relay_T4103;

  /**
   * T4108 timer
   */
  Timer* m_relay_T4108;

  /**
   * TRUIR timer
   */
  Timer* m_relay_TRUIR;

  /**
   * The duration value (in ms) for T4100 timer
   */
  uint32_t m_remote_dT4100;

  /**
   * The duration value (in ms) for T4101 timer
   */
  uint32_t m_remote_dT4101;

  /**
   * The duration value (in ms) for T4102 timer
   */
  uint32_t m_remote_dT4102;
  /**
   * The duration value (in ms) for T4103 timer
   */
  uint32_t m_remote_dT4103;

  /**
   * T4100 timer
   */
  Timer* m_remote_T4100;

  /**
   * T4101 timer
   */
  Timer* m_remote_T4101;

  /**
   * T4102 timer
   */
  Timer* m_remote_T4102;

  /**
   * T4103 timer
   */
  Timer* m_remote_T4103;

  /**
   * The maximum values for DCR counter
   */
  uint32_t m_relay_DCR_maximum;

  /**
   * The maximum values for RUIR counter
   */
  uint32_t m_relay_RUIR_maximum;

  /**
   * The maximum values for DCR counter
   */
  uint32_t m_remote_DCR_maximum;

  /**
   * The maximum values for DCRq counter
   */
  uint32_t m_remote_DCRq_maximum;

  /**
   * The maximum values for DCK counter
   */
  uint32_t m_remote_DCK_maximum;

  /**
   * The DCR counter for relay UE
   */
  uint16_t m_relay_DCR_count;

  /**
   * The RUIR counter
   */
  uint16_t m_relay_RUIR_count;

  /**
   * The DCR counter for remote UE
   */
  uint16_t m_remote_DCR_count;

  /**
   * The DCRq counter
   */
  uint16_t m_remote_DCRq_count;

  /**
   * The DCK counter
   */
  uint16_t m_remote_DCK_count;

  /**
   * The KA counter
   */
  uint16_t m_remote_ka_count;

  /**
   * Contains the copy of DirectCommunicationRequest for further retransmissions
   */
  DirectCommunicationRequest m_dcrq_retrans;

  /**
   * Contains the copy of DirectCommunicationRelease for further retransmissions
   */
  DirectCommunicationRelease m_dcr_retrans;

  /**
   * Contains the copy of DirectCommunicationKeepalive for further retransmissions
   */
  DirectCommunicationKeepalive m_dck_retrans;

  /**
   * Contains the copy of RemoteUeInfoRequest for further retransmissions
   */
  RemoteUeInfoRequest m_ruirq_retrans;

  /**
   * Contains the copy of DirectSecurityModeCommand for further retransmissions
   */
  DirectSecurityModeCommand m_dsmcm_retrans;

  /**
   * Flag to identify if the direct security mode control state of this context
   */
  DirectSecurityModeState m_security_mode_state;

  /**
   * ID to identify the security mode control related to a request
   */
  uint8_t m_security_mode_id;

  /**
   * The context identifier
   */
  LteSlPc5ContextId m_contextId;

  /**
   * The current state of the remote or relay UE
   */
  UeO2OCommState m_state;

  /**
   * String representation of the current state
   */
  std::vector <std::string> m_stateString;

public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * The possible reasons for rejecting a request from a remote UE
   */
  enum UeO2ORejectReason
  {
    COMM_NOT_ALLOWED = 1,
    AUTH_FAILURE,
    CONFLICT_L2_ID,
    LACK_OF_RESOURCES,
    IP_MISMATCH,
    OTHER_ERRORS
  };

  /**
   * The possible reasons for releasing a link
   */
  enum UeO2OReleaseReason
  {
    COMM_NO_LONGER_NEEDED = 1,
    COMM_NO_LONGER_ALLOWED,
    COMM_NO_LONGER_AVAILABLE
  };

  /**
   * Set the duration value (in ms) for T4111 timer
   *
   * \param duration the duration value (in ms) for T4111 timer
   */
  void SetRelayT4111Duration (uint32_t duration);


  /**
   * Set the duration value (in ms) for T4103 timer
   *
   * \param duration the duration value (in ms) for T4103 timer
   */
  void SetRelayT4103Duration (uint32_t duration);


  /**
   * Set the duration value (in ms) for T4108 timer
   *
   * \param duration the duration value (in ms) for T4108 timer
   */
  void SetRelayT4108Duration (uint32_t duration);


  /**
   * Set the duration value (in ms) for TRUIR timer
   *
   * \param duration the duration value (in ms) for TRUIR timer
   */
  void SetRelayTRUIRDuration (uint32_t duration);

  /**
   *
   * \return T4111 timer for Relay UE
   */
  Timer* GetRelayT4111 (void);

  /**
   *
   * \return T4103 timer for Relay UE
   */
  Timer* GetRelayT4103 (void);

  /**
   *
   * \return T4108 timer for Relay UE
   */
  Timer* GetRelayT4108 (void);

  /**
   *
   * \return TRUIR timer for Relay UE
   */
  Timer* GetRelayTRUIR (void);

  /**
   * Set the duration value (in ms) for T4100 timer
   *
   * \param duration the duration value (in ms) for T4100 timer
   */
  void SetRemoteT4100Duration (uint32_t duration);

  /**
   * Set the duration value (in ms) for T4101 timer
   *
   * \param duration the duration value (in ms) for T4101 timer
   */
  void SetRemoteT4101Duration (uint32_t duration);

  /**
   * Set the duration value (in ms) for T4102 timer
   *
   * \param duration the duration value (in ms) for T4102 timer
   */
  void SetRemoteT4102Duration (uint32_t duration);

  /**
   * Set the duration value (in ms) for T4103 timer
   *
   * \param duration the duration value (in ms) for T4103 timer
   */
  void SetRemoteT4103Duration (uint32_t duration);

  /**
   *
   * \return T4100 timer for Remote UE
   */
  Timer* GetRemoteT4100 (void);

  /**
   *
   * \return T4101 timer for Remote UE
   */
  Timer* GetRemoteT4101 (void);

  /**
   *
   * \return T4102 timer for Remote UE
   */
  Timer* GetRemoteT4102 (void);

  /**
   *
   * \return T4103 timer for Remote UE
   */
  Timer* GetRemoteT4103 (void);

  /**
   *
   * \return RelayDcrMaximum The maximum values for DCR counter
   */
  uint32_t GetRelayDcrMax (void) const;

  /**
   *
   * \return RelayRuirMaximum he maximum values for RUIR counter
   */
  uint32_t GetRelayRuirMax (void) const;

  /**
   *
   * \return RemoteDcrMaximum The maximum values for DCR counter
   */
  uint32_t GetRemoteDcrMax (void) const;

  /**
   *
   * \return RemoteDcrqMaximum The maximum values for DCRq counter
   */
  uint32_t GetRemoteDcrqMax (void) const;

  /**
   *
   * \return RemoteDckMaximum  The maximum values for DCK counter
   */
  uint32_t GetRemoteDckMax (void) const;

  /**
   *
   * \return relay_DCR_count The DCR counter for relay UE
   */
  uint16_t GetRelayDcrCount (void) const;
  /**
   *
   * \param value The DCR counter value to set for relay UE
   */
  void SetRelayDcrCount (uint16_t value);

  /**
   *
   * \return relay_RUIR_count The RUIR counter
   */
  uint16_t GetRelayRuirCount (void) const;
  /**
   *
   * \param value The RUIR counter value to set for relay UE
   */
  void SetRelayRuirCount (uint16_t value);

  /**
   *
   * \return remote_DCR_count The DCR counter for remote UE
   */
  uint16_t GetRemoteDcrCount (void) const;
  /**
   *
   * \param value The DCR counter value to set for remote UE
   */
  void SetRemoteDcrCount (uint16_t value);

  /**
   *
   * \return remote_DCRq_count The DCR counter for remote UE
   */
  uint16_t GetRemoteDcrqCount (void) const;
  /**
   *
   * \param value The DCRq counter value to set for remote UE
   */
  void SetRemoteDcrqCount (uint16_t value);

  /**
   *
   * \return remote_DCK_count The DCK counter for remote UE
   */
  uint16_t GetRemoteDckCount (void) const;
  /**
   *
   * \param value The DCK counter value to set for remote UE
   */
  void SetRemoteDckCount (uint16_t value);

  /**
   *
   * \return remote_ka_count The KA counter for remote UE
   */
  uint16_t GetRemoteKaCount (void) const;
  /**
   *
   * \param value The KA counter value to set for remote UE
   */
  void SetRemoteKaCount (uint16_t value);

  /**
   *
   * \return dcrq_retrans the copy of DirectCommunicationRequest
   */
  DirectCommunicationRequest GetDcrqRetrans (void) const;
  /**
   *
   * \param value the copy of DirectCommunicationRequest for further retransmissions
   */
  void SetDcrqRetrans (DirectCommunicationRequest value);

  /**
   *
   * \return dcr_retrans the copy of DirectCommunicationRelease
   */
  DirectCommunicationRelease GetDcrRetrans (void) const;

  /**
   *
   * \param value the copy of DirectCommunicationRelease for further retransmissions
   */
  void SetDcrRetrans (DirectCommunicationRelease value);

  /**
   *
   * \return dck_retrans the copy of DirectCommunicationKeepalive
   */
  DirectCommunicationKeepalive GetDckRetrans (void) const;
  /**
   *
   * \param value the copy of DirectCommunicationKeepalive for further retransmissions
   */
  void SetDckRetrans (DirectCommunicationKeepalive value);

  /**
   *
   * \return ruirq_retrans the copy of RemoteUeInfoRequest
   */
  RemoteUeInfoRequest GetRuirqRetrans (void) const;
  /**
   *
   * \param value the copy of RemoteUeInfoRequest for further retransmissions
   */
  void SetRuirqRetrans (RemoteUeInfoRequest value);

  /**
   *
   * \return dsmcm_retrans the copy of DirectSecurityModeCommand
   */
  DirectSecurityModeCommand GetDsmcmRetrans (void) const;
  /**
   *
   * \param value the copy of DirectSecurityModeCommand for further retransmissions
   */
  void SetDsmcmRetrans (DirectSecurityModeCommand value);
  /**
   *
   * \param lsb LSB to be set on the copy of DirectSecurityModeCommand for further retransmissions
   */
  void SetDsmcmRetransLsb (uint8_t lsb);

  /**
    *
    * \return security_mode_state the flag to identify if the direct security mode control state of this context
    */
  DirectSecurityModeState GetSecurityModeState (void) const;
  /**
   *
   * \param value the flag value to be set to identify if the direct security mode control state of this context
   */
  void SetSecurityModeState (DirectSecurityModeState value);

  /**
    *
    * \return security_mode_id ID to identify the security mode control related to a request
    */
  uint8_t GetSecurityModeId (void) const;

  /**
   *
   * \param value the ID to be set to identify the security mode control related to a request
   */
  void SetSecurityModeId (uint8_t value);

  /**
   * Set the context ID
   * \param contextId The context ID
   */
  void SetContextId (LteSlPc5ContextId contextId);

  /**
   * Get the context ID
   * \return The context ID
   */
  LteSlPc5ContextId GetContextId ();

  /**
   * Return the current state
   * \return state The current state of the remote or relay UE
   */
  UeO2OCommState GetState (void) const;

  /**
   * Set the current state
   * \param value The current state of the remote or relay UE to be set
   */
  void SetState (UeO2OCommState value);

  /**
   * Return the string representation of the state value given as argument
   * \param state The state value
   * \return String representation of the state given as argument
   */
  std::string GetStateString (UeO2OCommState state);

};

} // namespace ns3

#endif // LTE_SL_O2O_COMM_PARAMS_H

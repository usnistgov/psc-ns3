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


#ifndef LTE_SL_UE_CTRL_SAP_H
#define LTE_SL_UE_CTRL_SAP_H

#include <ns3/object.h>
#include <ns3/lte-sl-o2o-comm-params.h>

#include <ns3/lte-sl-ue-rrc.h>

namespace ns3 {

/**
 * \brief Service Access Point (SAP) offered by the Sidelink UE Controller
 *        instance to the Sidelink UE RRC instance.
 *
 * This is the *LteSlUeCtrlSapProvider*, i.e., the part of the SAP
 * that contains the controller methods called by the Sidelink UE RRC
 * instance.
 */
class LteSlUeCtrlSapProvider
{
public:
  virtual ~LteSlUeCtrlSapProvider ();

  /**
   * \brief Indication that a relay service discovery message was received
   *
   * \param serviceCode The relay service code
   * \param announcerInfo The announcer info included in the discovery message
   * \param proseRelayUeId The layer 2 ID for the relay node
   * \param statusIndicator The status field
   */
  virtual void RecvRelayServiceDiscovery (uint32_t serviceCode, uint64_t announcerInfo, uint32_t proseRelayUeId, uint8_t statusIndicator) = 0;

  /**
   * \brief Indication that PC5 connection has started
   *
   * \param peerUeId The peer node L2ID
   * \param selfUeId The local node L2ID
   * \param role The role of this node (remote or relay)
   */
  virtual void Pc5ConnectionStarted (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role) = 0;

  /**
   * \brief Indication that PC5 signaling is complete and connection is established
   *
   * \param peerUeId The peer node L2ID
   * \param selfUeId The self node L2ID
   * \param role The role of this node (remote or relay)
   */
  virtual void Pc5SecuredEstablished (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role) = 0;

  /**
   * \brief Indication that PC5 connection has been terminated
   *
   * \param peerUeId The peer node L2ID
   * \param selfUeId The local node L2ID
   * \param role The role of this node (remote or relay)
   */
  virtual void Pc5ConnectionTerminated (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role) = 0;

  /**
   * \brief Indication that PC5 connection has been aborted
   *
   * \param peerUeId The peer node L2ID
   * \param selfUeId The local node L2ID
   * \param role The role of this node (remote or relay)
   * \param reason The reason for aborting connection
   */
  virtual void Pc5ConnectionAborted (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role, LteSlO2oCommParams::UeO2ORejectReason reason) = 0;

  /**
   * \brief Indication that the remote UE report was received
   *
   * \param localImsi The local node IMSI value
   * \param peerUeId The peer node L2ID
   * \param remoteImsi The remote UE IMSI value
   */
  virtual void RecvRemoteUeReport (uint64_t localImsi, uint32_t peerUeId, uint64_t remoteImsi) = 0;


  /**
   * \brief Implementation of the Relay UE (re)selection algorithm
   *
   * \param validRelays the list of valid Relay UEs detected in the proximity offering the service code
   * \param serviceCode the relay service code
   * \param currentRelayId the Id of the Relay UE to which the UE is currently connected for the service code (Zero if none)
   *
   * \return selectedRelayId the Id of the selected Realy UE
   */
  virtual uint64_t RelayUeSelection (std::map <uint64_t, double> validRelays, uint32_t serviceCode, uint64_t currentRelayId) = 0;

}; // end of class LteSlUeCtrlSapProvider

/**
 * \brief Service Access Point (SAP) offered by the Sidelink UE RRC instance to the
 *        Sidelink UE Controller instance.
 *
 * This is the *LteSlUeCtrlSapUser*, i.e., the part of the SAP that
 * contains the Sidelink UE RRC methods called by the Sidelink UE Controller instance.
 */
class LteSlUeCtrlSapUser
{
public:
  virtual ~LteSlUeCtrlSapUser ();

}; // end of class LteSlUeCtrlSapUser

/**
 * \brief Template for the implementation of the LteSlUeCtrlSapProvider
 *        as a member of an owner class of type C to which all methods are
 *        forwarded.
 */
template <class C>
class MemberLteSlUeCtrlSapProvider : public LteSlUeCtrlSapProvider
{
public:
  /**
   * Constructor
   *
   * \param owner the owner class
   */
  MemberLteSlUeCtrlSapProvider (C* owner);

  // inherited from LteSlUeCtrlSapProvider
  virtual void RecvRelayServiceDiscovery (uint32_t serviceCode, uint64_t announcerInfo, uint32_t proseRelayUeId, uint8_t statusIndicator);
  virtual void Pc5ConnectionStarted (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role);
  virtual void Pc5SecuredEstablished (uint32_t remoteUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role);
  virtual void Pc5ConnectionTerminated (uint32_t remoteUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role);
  virtual void Pc5ConnectionAborted (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role, LteSlO2oCommParams::UeO2ORejectReason reason);
  virtual void RecvRemoteUeReport (uint64_t localImsi, uint32_t peerUeId, uint64_t remoteImsi);
  virtual uint64_t RelayUeSelection (std::map <uint64_t, double> validRelays, uint32_t serviceCode, uint64_t currentRelayId);

private:
  MemberLteSlUeCtrlSapProvider ();
  C* m_owner; ///< the owner class

}; // end of class MemberLteSlUeCtrlSapProvider


template <class C>
MemberLteSlUeCtrlSapProvider<C>::MemberLteSlUeCtrlSapProvider (C* owner)
  : m_owner (owner)
{
}

template <class C>
void
MemberLteSlUeCtrlSapProvider<C>::RecvRelayServiceDiscovery (uint32_t serviceCode, uint64_t announcerInfo, uint32_t proseRelayUeId, uint8_t statusIndicator)
{
  return m_owner->DoRecvRelayServiceDiscovery (serviceCode, announcerInfo, proseRelayUeId, statusIndicator);
}

template <class C>
void
MemberLteSlUeCtrlSapProvider<C>::Pc5ConnectionStarted (uint32_t remoteUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role)
{
  return m_owner->DoPc5ConnectionStarted (remoteUeId, selfUeId, role);
}

template <class C>
void
MemberLteSlUeCtrlSapProvider<C>::Pc5SecuredEstablished (uint32_t remoteUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role)
{
  return m_owner->DoPc5SecuredEstablished (remoteUeId, selfUeId, role);
}

template <class C>
void
MemberLteSlUeCtrlSapProvider<C>::Pc5ConnectionTerminated (uint32_t remoteUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role)
{
  return m_owner->DoPc5ConnectionTerminated (remoteUeId, selfUeId, role);
}

template <class C>
void
MemberLteSlUeCtrlSapProvider<C>::Pc5ConnectionAborted (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role, LteSlO2oCommParams::UeO2ORejectReason reason)
{
  return m_owner->DoPc5ConnectionAborted (peerUeId, selfUeId, role, reason);
}

template <class C>
void
MemberLteSlUeCtrlSapProvider<C>::RecvRemoteUeReport (uint64_t localImsi, uint32_t peerUeId, uint64_t remoteImsi)
{
  return m_owner->DoRecvRemoteUeReport (localImsi, peerUeId, remoteImsi);
}

template <class C>
uint64_t
MemberLteSlUeCtrlSapProvider<C>::RelayUeSelection (std::map <uint64_t, double> validRelays, uint32_t serviceCode, uint64_t currentRelayId)
{
  return m_owner->DoRelayUeSelection (validRelays, serviceCode, currentRelayId);
}

/**
 * \brief Template for the implementation of the LteSlUeCtrlSapUser
 *        as a member of an owner class of type C to which all methods are
 *        forwarded.
 */
template <class C>
class MemberLteSlUeCtrlSapUser : public LteSlUeCtrlSapUser
{
public:
  /**
   * Constructor
   *
   * \param owner the owner class
   */
  MemberLteSlUeCtrlSapUser (C* owner);

private:
  MemberLteSlUeCtrlSapUser ();
  C* m_owner; ///< the owner class

}; // end of class LteSlUeCtrlSapUser

template <class C>
MemberLteSlUeCtrlSapUser<C>::MemberLteSlUeCtrlSapUser (C* owner)
  : m_owner (owner)
{
}

} // end of namespace ns3


#endif /* LTE_SL_UE_CTRL_SAP_H */

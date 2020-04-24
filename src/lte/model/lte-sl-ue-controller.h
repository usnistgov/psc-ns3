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

#ifndef LTE_SL_UE_CONTROLLER_H
#define LTE_SL_UE_CONTROLLER_H


#include <ns3/object.h>
#include <ns3/lte-sl-o2o-comm-params.h>
#include "lte-sl-o2o-comm-params.h"

#include <ns3/lte-sl-ue-rrc.h>

namespace ns3 {
class LteUeRrc;
class LteSlUeCtrlSapUser;
class LteSlUeCtrlSapProvider;

/**
 * \brief The abstract base class of a UE sidelink controller.
 *
 */
class LteSlUeController : public Object
{

public:
  LteSlUeController ();
  virtual ~LteSlUeController ();

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();

  /**
   * \brief Set the "user" part of the LteSlUeCtrlSap interface that
   *        the controller instance will interact with.
   * \param s a reference to the "user" part of the interface
   *
   */
  virtual void SetLteSlUeCtrlSapUser (LteSlUeCtrlSapUser* s) = 0;

  /**
   * \brief Export the "provider" part of the LteSlUeCtrlSap interface.
   * \return the reference to the "provider" part of the interface
   */
  virtual LteSlUeCtrlSapProvider* GetLteSlUeCtrlSapProvider () = 0;

protected:
  // inherited from Object
  virtual void DoDispose ();

  /**
   * \brief Implementation of LteSlUeCtrlSapProvider::RecvRelayServiceDiscovery
   *
   * \param serviceCode The relay service code
   * \param announcerInfo The announcer info included in the discovery message
   * \param proseRelayUeId The layer 2 ID for the relay node
   * \param statusIndicator The status field
   */
  virtual void DoRecvRelayServiceDiscovery (uint32_t serviceCode, uint64_t announcerInfo, uint32_t proseRelayUeId, uint8_t statusIndicator) = 0;

  /**
     * \brief Implementation of LteSlUeCtrlSapProvider::Pc5ConnectionStarted
     *
     * \param peerUeId The peer node L2ID
     * \param selfUeId The local node L2ID
     * \param role The role of this node (remote or relay)
     */
  virtual void DoPc5ConnectionStarted (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role) = 0;

  /**
   * \brief Implementation of LteSlUeCtrlSapProvider::Pc5SecuredEstablished
   *
   * \param peerUeId The peer node L2ID
   * \param selfUeId The self node L2ID
   * \param role The role of this node (remote or relay)
   */
  virtual void DoPc5SecuredEstablished (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role) = 0;

  /**
   * \brief Implementation of LteSlUeCtrlSapProvider::Pc5ConnectionTerminated
   *
   * \param peerUeId The peer node L2ID
   * \param selfUeId The local node L2ID
   * \param role The role of this node (remote or relay)
   */
  virtual void DoPc5ConnectionTerminated (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role) = 0;

  /**
   * \brief Implementation of LteSlUeCtrlSapProvider::Pc5ConnectionAborted
   *
   * \param peerUeId The peer node L2ID
   * \param selfUeId The local node L2ID
   * \param role The role of this node (remote or relay)
   * \param reason The reason for aborting connection
   */
  virtual void DoPc5ConnectionAborted (uint32_t peerUeId, uint32_t selfUeId, LteSlUeRrc::RelayRole role, LteSlO2oCommParams::UeO2ORejectReason reason) = 0;

  /**
   * \brief Implementation of LteSlUeCtrlSapProvider::RecvRemoteUeReport
   *
   * \param localImsi The local node IMSI value
   * \param peerUeId The peer node L2ID
   * \param remoteImsi The remote UE IMSI value
   */
  virtual void DoRecvRemoteUeReport (uint64_t localImsi, uint32_t peerUeId, uint64_t remoteImsi) = 0;

  /**
   * \brief Implementation of LteSlUeCtrlSapProvider::RelayUeSelection
   *
   * \param validRelays the list of valid Relay UEs detected in the proximity offering the service code
   * \param serviceCode the relay service code
   * \param currentRelayId the Id of the Realy UE to which the UE is currently connected for the service code (Zero if none)
   * \return selectedRelayId the Id of the selected Realy UE
   */
  virtual uint64_t DoRelayUeSelection (std::map <uint64_t, double> validRelays, uint32_t serviceCode, uint64_t currentRelayId) = 0;

}; // end of class LteSlUeController

} // namespace ns3

#endif /* LTE_SL_UE_CONTROLLER_H */

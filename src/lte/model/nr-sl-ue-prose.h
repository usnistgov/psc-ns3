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
  bool m_hasPendingSlDrb;
  bool m_hasActiveSlDrb;

};


/**
 * \ingroup lte
 *
 * This class implements a NR SL service layer providing Proximity Services (ProSe)
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
   * \brief Add a new direct link connection with the given peer if possible
   *
   * \param selfL2Id the layer 2 ID of this UE
   * \param selfIp the IPv4 address used by this UE
   * \param peerL2Id the layer 2 UD of the peer UE
   * \param isInitiating flag indicating if the UE is initiating the procedure (true)
   *                     or adding the link as result of a request by the peer UE (false)
   * \param isRelayConn flag indicating if the direct link is for a relay connection (true)
   *
   */
  void AddDirectLinkConnection (uint32_t selfL2Id, Ipv4Address selfIp, uint32_t peerL2Id, bool isInitiating, bool isRelayConn);


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
   * \brief Set the UL data radio bearer ID to be used for L3 UE-to-Network relay
   *
   * \param drbId the data radio bearer ID
   */
  void SetU2nRelayDrbId (uint8_t drbId);
  /**
   * \brief Set the IMSI used by the UE
   *
   * \param imsi the IMSI of the UE
   */
  void SetImsi (uint64_t imsi);
  /**
   * \brief Set EPC helper
   *
   * \param epcHelper Ptr of type NrPointToPointEpcHelper
   */
  void SetEpcHelper (const Ptr<NrPointToPointEpcHelper> &epcHelper);

private:
  //NrSlUeSvcRrcSapUser methods
  void DoReceiveNrSlSignalling (Ptr<Packet> packet, uint32_t srcL2Id);
  void DoNotifySvcNrSlDataRadioBearerActivated (uint32_t peerL2Id);

  //NrSlUeProseDirLnkSapUser methods
  void DoSendNrSlPc5SMessage (Ptr<Packet> packet, uint32_t dstL2Id,  uint8_t lcId);
  void DoNotifyChangeOfDirectLinkState (uint32_t peerL2Id, NrSlUeProseDirLnkSapUser::ChangeOfStateNotification info);

  //SAP pointers
  NrSlUeSvcNasSapUser* m_nrSlUeSvcNasSapUser; ///< NR SL UE SERVICE NAS SAP user
  NrSlUeSvcNasSapProvider* m_nrSlUeSvcNasSapProvider {nullptr}; ///< NR SL UE SERVICE NAS SAP Provider

  NrSlUeSvcRrcSapUser* m_nrSlUeSvcRrcSapUser; ///< NR SL UE SERVICE SAP User
  NrSlUeSvcRrcSapProvider* m_nrSlUeSvcRrcSapProvider {nullptr}; ///< NR SL UE SERVICE SAP Provider

  NrSlUeProseDirLnkSapUser * m_nrSlUeProseDirLnkSapUser; ///< ProSe Direct Link SAP User

  //Class internal private methods and member variables
  NrSlDirectLinkContextMapPerPeerL2Id m_unicastDirectLinks;
  NrSlSingalingRadioBearersPerPeerL2Id m_activeSlSrbs;

  void ActivateDirectLinkDataRadioBearer (uint32_t peerL2Id, NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo);
  void ConfigureDataRadioBearersForU2NRelay (uint32_t peerL2Id, enum NrSlUeProseDirLnkSapUser::U2nRole role, NrSlUeProseDirLnkSapUser::DirectLinkIpInfo ipInfo);

  uint8_t m_relayDrbId; //!< the UL data radio bearer ID to be used for L3 UE-to-Network relay
  uint64_t m_imsi;  //!< the IMSI used by the UE

  Ptr<NrPointToPointEpcHelper> m_epcHelper; //!< pointer to the EPC helper


};//end of NrSlUeProse class definition

} //namespace ns3

#endif /* NR_SL_UE_PROSE_H */

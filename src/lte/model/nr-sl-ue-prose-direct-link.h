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

#ifndef NR_SL_UE_PROSE_DIRECT_LINK_H
#define NR_SL_UE_PROSE_DIRECT_LINK_H

#include <ns3/object.h>
#include <ns3/nr-sl-ue-prose-dir-lnk-sap.h>
#include <ns3/nr-sl-pc5-signalling-header.h>
#include <ns3/timer.h>
#include <ns3/tag.h>

#include <unordered_map>
#include <vector>


namespace ns3 {

/**
 * \ingroup lte
 *
 * Packet tag used to exchange IPv4 address information between peer UEs of the
 * direct link during establishment procedure.
 */
class Ipv4AddrTag : public Tag
{
public:
  Ipv4AddrTag ();

  /**
   * \brief Set the tag's address
   *
   * \param addr the address
   */
  void SetAddress (Ipv4Address addr);

  /**
   * \brief Get the tag's address
   *
   * \returns the address
   */
  Ipv4Address GetAddress (void) const;

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

private:
  Ipv4Address m_addr;     //!< address

};

/**
 * \ingroup lte
 *
 * \brief NR ProSe Direct Link with a peer UE
 *
 * This class implements the NR ProSe direct link with a peer UE. It stores all
 * the information corresponding to the direct link in this UE and executes the
 * the different protocols depending on the UE role in the direct link.
 */
class NrSlUeProseDirectLink : public Object
{

  /// allow MemberNrSlUeProseDirLnkSapProvider<NrSlUeProseDirectLink> class friend access
  friend class MemberNrSlUeProseDirLnkSapProvider<NrSlUeProseDirectLink>;

protected:
  virtual void DoDispose ();

public:
  NrSlUeProseDirectLink ();
  virtual ~NrSlUeProseDirectLink (void);
  static TypeId GetTypeId (void);

  /**
   * \brief Set parameters of the direct link
   *
   * \param selfL2Id layer 2 ID of this UE
   * \param peerL2Id layer 2 ID of the peer UE
   * \param isInitiating true if the UE is the one initiating the direct link establishment
   * \param isRelayConn true if the direct link is part of a relay connection
   * \param relayServiceCode the relay service code associated to this direct link
   * \param selfIp the Ipv4 address this UE is using
   */
  void SetParameters (uint32_t selfL2Id,
                      uint32_t peerL2Id,
                      bool isInitiating,
                      bool isRelayConn,
                      uint32_t relayServiceCode,
                      Ipv4Address selfIp);

  /**
   * \brief Get the pointer of the ProSe Direct Link SAP provider interface
   *        offered to the Prose layer by this class
   *
   * \return the pointer of type NrSlUeProseDirLnkSapProvider
   */
  NrSlUeProseDirLnkSapProvider* GetNrSlUeProseDirLnkSapProvider ();

  /**
   * \brief Set the pointer for the Prose Direct Link SAP User interface
   *        offered to this class by the ProSe layer class
   *
   * \param s the pointer of type m_nrSlUeProseDirLnkSapUser
   */
  void SetNrSlUeProseDirLnkSapUser (NrSlUeProseDirLnkSapUser* s);

  /**
   * \brief Start the ProSe direct link establishment procedure
   *
   */
  void StartConnectionEstablishment ();

  enum DirectLinkState
  {
    INIT = 0,
    ESTABLISHING,
    ESTABLISHED,
    RELEASING,
    RELEASED,
    NUM_STATES
  };

  /**
   * \brief Parameters for the ProSe direct link establishment procedure
   *        to be use by a initiating UE
   *
   */
  struct ProcessDirectLinkEstablishmentParams
  {
    Timer* t5080;                                     ///< Timer T5080 (establishment request retransmission)
    uint32_t rtxCounter;                              ///< request retransmission counter
    uint32_t rtxMax;                                  ///< maximum number of request retransmissions
    ProseDirectLinkEstablishmentRequest rqMsgCopy;    ///< copy of the establishment request message used for rtxs
  };

private:

  //NrSlUeProseDirLink SAPs
  NrSlUeProseDirLnkSapUser* m_nrSlUeProseDirLnkSapUser {nullptr};         ///< ProSe Direct Link SAP user
  NrSlUeProseDirLnkSapProvider* m_nrSlUeProseDirLnkSapProvider {nullptr}; ///< ProSe Direct Link SAP provider

  //NrSlUeProseDirLnkSapProvider methods
  void DoReceiveNrSlPc5Message (Ptr<Packet> packet);

  //Class internal private methods and member variables

  /**
    * \brief Change the state of this direct link
    *
    * \param newState the state to which the link is changing to
    */
  void SwitchToState (DirectLinkState newState);

  /**
   * \brief Send a NR SL PC5-S message
   *
   * When the direct link is real, this function will pass the message to the
   * ProSe layer to be sent on the appropriate SL-SRB and over the SL.
   *
   * \param packet the NR SL PC5-S message
   * \param dstL2Id the layer 2 ID of the peer UE
   * \param lcId the logical channel ID of the logical channel to be used to
   *             send the message if the direct link is real
   */
  void SendNrSlPc5SMessage (Ptr<Packet> packet, uint32_t dstL2Id,  uint8_t lcId);

  void ProcessDirectLinkEstablishmentRequest (Ptr<Packet> packet); ///< Process a DirectLink Establishment Request message
  void ProcessDirectLinkEstablishmentAccept (Ptr<Packet> packet);  ///< Process a DirectLink Establishment Accept message
  void ProcessDirectLinkEstablishmentReject (Ptr<Packet> packet);  ///< Process a DirectLink Establishment Reject message

  void SendDirectLinkEstablishmentRequest (); ///< Send a DirectLink Establishment Request message
  void SendDirectLinkEstablishmentAccept ();  ///< Send a DirectLink Establishment Accept message
  void SendDirectLinkEstablishmentReject (uint8_t cause);  ///< Send a DirectLink Establishment Reject message
  void RetransmitDirectLinkEstablishmentRequest (); ///< Retransmit a DirectLink Establishment Request message

  //IDs
  uint32_t m_selfL2Id; ///< L2Id of the UE where this direct link object is installed
  uint32_t m_peerL2Id; ///< L2Id of the peer UE for this direct link

  bool m_isInitiating; ///< Indicates if the UE is the initiating UE of the direct link
  bool m_isRelayConn;  ///< Indicates if the direct link is part of a relay connection

  uint32_t m_relayServiceCode; ///< The relay service code associated with this direct link

  DirectLinkState m_state; ///< State of this direct link

  NrPc5SignallingHeaderSequenceNumber m_pc5SigMsgSeqNum; ///< Unique sequence number generator for PC5-S messages to be transmitted

  NrSlUeProseDirLnkSapUser::DirectLinkIpInfo m_ipInfo; ///< IP configuration

  //Parameters and functions depending on the procedure

  //Establishment procedure
  ProcessDirectLinkEstablishmentParams m_pdlEsParam; ///< Parameters of the establishment procedure
  void SetT5080 (Time t); ///< Set the value of the timer T5080 (Establishment Request Retransmission)
  void SetPdlEsRqRtxMax (uint32_t max);  ///< Set the maximum number of Establishment Request Retransmissions
  void T5080Expiry ();

};//end of NrSlUeProseDirectLink

} // namespace ns3

#endif // NR_SL_UE_PROSE_DIRECT_LINK_H

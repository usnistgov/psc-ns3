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

#ifndef NR_SL_PC5_SIGNALLING_HEADER_H
#define NR_SL_PC5_SIGNALLING_HEADER_H

#include "ns3/header.h"
#include <stdint.h>
#include <vector>
#include "ns3/string.h"


namespace ns3 {

/**
 * \ingroup lte
 * \brief To peek at the first byte of the received PC5 Signaling message and
 *        identify the type
 */

class NrSlPc5SignallingMessageType : public Header
{
public:
  /**
    * Default constructor
    */
  NrSlPc5SignallingMessageType ();
  /**
   * Default destructor
   */
  ~NrSlPc5SignallingMessageType ();

  /**
  * Get the type ID.
  *
  * \return the object TypeId
  */
  static TypeId GetTypeId (void);

  /**
   * Get the TypeId for this instance
   *
   * \return the TypeId for this instance
   */
  TypeId GetInstanceTypeId (void) const;

  /**
   * Print a string representation of this header to the
   * std::ostream object passed as a parameter
   *
   * \param os The ostream where to print
   */
  void Print (std::ostream &os) const;

  /**
   * Get the size of the serialized version of this header
   *
   * \return The size of the serialized version of this header
   */
  uint32_t GetSerializedSize (void) const;

  /**
   * Write in the Iterator object passed as a parameter the
   * serialized version of this header
   *
   * \param start The Iterator for the Buffer where to write
   */
  void Serialize (Buffer::Iterator start) const;

  /**
   * Read the provided Buffer and deserialize a header from there
   *
   * \param start The Iterator for the Buffer where to read
   * \return The size of the deserialized header
   */
  uint32_t Deserialize (Buffer::Iterator start);

  /**
   * Get the Message Type
   *
   * \return the Message Type
  */
  uint8_t GetMessageType ();

  /**
   * Set the Message Type
   *
   * \param msgType Message Type
   */
  void SetMessageType (uint8_t msgType);

  /**
   * Get the Message Acronym
   *
   * \return the Message Acronym
  */
  std::string GetMessageAcronym (void) const;

  /**
   * Get the Message Name
   *
   * \return the Message Name
  */
  std::string GetMessageName (void) const;

  /**
   * The different types of PC5 signaling messages
   */
  enum PC5SignallingMessageType
  {
    ProseDirectLinkEstablishmentRequest = 1,
    ProseDirectLinkEstablishmentAccept,
    ProseDirectLinkEstablishmentReject
  };

private:
  /**
   * Message type
   */
  uint8_t m_msgType;
};


/**
 * \ingroup nist
 * \brief sequence number for PC5 signaling headers
 *
 * This class provides a sequential packet number
 */

class NrPc5SignallingHeaderSequenceNumber
{
public:
  /**
   * Variable with the last sequence number generated
   */
  static uint8_t s_seqNum;
  /**
   * Return a new sequence number
   * \return A new sequence number value
   */
  static uint8_t GenerateSeqNum ()
  {
    return ++s_seqNum;
  }
};


/**
 * \ingroup lte
 *
 * \brief Header of the ProSe direct link establishment request message
 *        (as per TS 24.554 V0.4.0 (2021-08) Section 10.3.1)
 */
class ProseDirectLinkEstablishmentRequest : public Header
{
public:
  /**
   * Default constructor
    */
  ProseDirectLinkEstablishmentRequest ();
  /**
   * Default destructor
   */
  ~ProseDirectLinkEstablishmentRequest ();

  /**
  * Get the type ID.
  *
  * \return the object TypeId
  */
  static TypeId GetTypeId (void);

  /**
   * Get the TypeId for this instance
   *
   * \return the TypeId for this instance
   */
  TypeId GetInstanceTypeId (void) const;

  /**
   * Print a string representation of this header to the
   * std::ostream object passed as a parameter
   *
   * \param os The ostream where to print
   */
  void Print (std::ostream &os) const;

  /**
   * Get the size of the serialized version of this header
   *
   * \return The size of the serialized version of this header
   */
  uint32_t GetSerializedSize (void) const;

  /**
   * Write in the Iterator object passed as a parameter the
   * serialized version of this header
   *
   * \param start The Iterator for the Buffer where to write
   */
  void Serialize (Buffer::Iterator start) const;

  /**
   * Read the provided Buffer and deserialize a header from there
   *
   * \param start The Iterator for the Buffer where to read
   * \return The size of the deserialized header
   */
  uint32_t Deserialize (Buffer::Iterator start);


  /**
   * Function to set all the fields of the header
   *
   * \param seq The sequence number of the packet
   * \param proseAppIds the identifiers of the ProSe application
   * \param srcUserInfo the UE application layer ID
   * \param secCapabilities indicates which security algorithms are supported by the UE.
   * \param ueSigSecPolicy the UE PC5 unicast signalling security policy
   * \param keyEsInfoContainer the key establishment information container
   * \param nonce1 indicates the nonce value generated by the UE.
   * \param msb carries the 8 most significant bits of the Knrp-sess ID.
   * \param tgtUserInfo target UE's application layer ID from upper layers or if the destination layer-2 ID is the unicast layer-2 ID of target UE
   * \param knrpId carries the identity of the Knrp of the target UE.
   * \param relayServiceCode parameter that identifies a connectivity service the UE-to-Network relay provides.
   */
  void SetParameters (uint8_t seq, std::vector<uint32_t> proseAppIds, uint32_t srcUserInfo,
                      std::vector<uint8_t> secCapabilities, uint8_t ueSigSecPolicy, uint8_t keyEsInfoContainer,
                      std::vector<uint8_t> nonce1, uint8_t msb, uint32_t tgtUserInfo, uint32_t knrpId,
                      uint32_t relayServiceCode);

  /**
   * Get the Message Identifier
   *
   * \return the Message Identifier
   */
  uint8_t GetMessageIdentifier ();

  /**
   * Get the Sequence Number
   *
   * \return the Sequence Number
   */
  uint16_t GetSequenceNumber ();

  /**
   * Set the Sequence Number
   * \param seq The sequence number of the packet
   */
  void SetSequenceNumber (uint8_t seq);

  /**
   * Set the identifiers of the ProSe application
   *
   * \param proseAppIds the identifiers of the ProSe applications
   */
  void SetProseApplicationIds (std::vector<uint32_t> proseAppIds);

  /**
    * Get the identifiers of the ProSe application
    *
    * \return the identifiers of the ProSe applications
    */
  std::vector<uint32_t> GetProseApplicationIds ();

  /**
   * Set the UE application layer ID
   *
   * \param srcUserInfo the UE application layer ID
   */
  void SetSourceUserInfo (uint32_t srcUserInfo);

  /**
    * Get the UE application layer ID
    *
    * \return the UE application layer ID
    */
  uint32_t GetSourceUserInfo ();

  /**
   * Set the supported security algorithms
   *
   * \param secCapabilities the supported security algorithms
   */
  void SetUeSecurityCapabilities ( std::vector<uint8_t> secCapabilities);

  /**
  * Get the supported security algorithms
  *
  * \return the supported security algorithms
  */
  std::vector<uint8_t> GetUeSecurityCapabilities ();

  /**
    * Set the UE PC5 unicast signalling security policy
    *
    * \param ueSigSecPolicy the UE PC5 unicast signalling security policy
    */
  void SetUeSignallingSecurityPolicy (uint8_t ueSigSecPolicy);

  /**
   * Get the UE PC5 unicast signalling security policy
   *
   * \return the UE PC5 unicast signalling security policy
   */
  uint8_t GetUeSignallingSecurityPolicy ();


  /**
   * Set the key establishment information container
   *
   * \param keyEsInfoContainer the key establishment information container
   */
  void SetKeyEstablishmentInfoContainer (uint8_t keyEsInfoContainer);

  /**
   * Get the key establishment information container
   *
   * \return the key establishment information container
   */
  uint8_t GetKeyEstablishmentInfoContainer ();


  /**
   * Set the nonce1 value
   *
   * \param nonce1 the nonce value
   */
  void SetNonce1 (std::vector<uint8_t> nonce1);

  /**
  * Get the nonce1 value
  *
  * \return the nonce1 value
  */
  std::vector<uint8_t> GetNonce1 ();


  /**
   * Set the 8 most significant bits of the KD-sess ID
   *
   * \param msb the 8 most significant bits of the KD-sess ID
   */
  void SetMsbKdId (uint8_t msb);

  /**
   * Get the 8 most significant bits of the KD-sess ID
   *
   * \return the 8 most significant bits of the KD-sess ID
   */
  uint8_t GetMsbKdId ();


  /**
   * Set the UE application layer ID of the target UE
   *
   * \param tgtUserInfo the UE application layer ID of the target UE
   */
  void SetTargetUserInfo (uint32_t tgtUserInfo);

  /**
    * Get the UE application layer ID of the target UE
    *
    * \return the UE application layer ID of the target UE
    */
  uint32_t GetTargetUserInfo ();


  /**
   * Set the identity of the Knrp
   *
   * \param kdId the identity of the Knrp
   */
  void SetKnrpId (uint32_t kdId);

  /**
   * Get the identity of the Knrp
   *
   * \return the identity of the Knrp
   */
  uint32_t GetKnrpId ();

  /**
   * Set the UE-to-Network relay service code
   *
   * \param relayServiceCode the UE-to-Network relay service code
   */
  void SetRelayServiceCode (uint32_t relayServiceCode);

  /**
   * Get the UE-to-Network relay service code
   *
   * \return the UE-to-Network relay service code
   */
  uint32_t GetRelayServiceCode ();

private:

  // Mandatory IEs
  uint8_t m_msgId;   ///< Message identity
  uint8_t m_seqNum;  ///< Sequence number
  std::vector<uint32_t> m_proseAppIds;   ///< ProSe application identifier contents (4 octets each ID, min 1 ID) No max defined in standard ATM
  uint32_t m_srcUserInfo;   ///< Source user info contents (Format 'Application layer ID', assumed with min 4 octets) No min defined in standard ATM
  std::vector<uint8_t> m_secCapabilities;    ///< UE security capabilities contents (min 2 octets, max 8 octets)
  uint8_t m_ueSigSecPolicy; ///< UE PC5 unicast signalling security policy (1 octet)

  // Optional IEs
  uint8_t m_keyEsInfoContainer; ///< Key establishment information container contents (using min of 1 octets)
  bool m_hasKeyEsInfoContainer; ///< Flag indicating if the Key establishment information container is present
  std::vector<uint8_t> m_nonce1; ///< Nonce_1 (128 bit = 16 octets)
  bool m_hasNonce1;  ///< Flag indicating if Nonce_1 is present
  uint8_t m_msb;  ///< Most significant bits of the KNRP-sess ID contents (1 octect)
  bool m_hasMsb;  ///< Flag indicating if the most significant bits of the KNRP-sess ID is present
  uint32_t m_tgtUserInfo;  ///< Target user info contents (Format 'Application layer ID', assumed with min 4 octets) No min defined in standard ATM
  bool m_hasTgtUserInfo; ///< Flag indicating if the target user info is present
  uint32_t m_knrpId; ///< Knrp ID contents (32 bits = 4 octets)
  bool m_hasKnrpId; ///< Flag indicating if
  uint32_t m_relayServiceCode; ///< Relay service code (24 bits = 3 octets, Only 24 bits out of the 32 of the variable are serialized)
  bool m_hasRelayServiceCode; ///< Flag indicating if Relay service code is present

};

/**
 * \ingroup lte
 *
 * \brief Header of the ProSe direct link establishment accept message
 *        (as per TS 24.554 V0.4.0 (2021-08) Section 10.3.2)
 *
 */
class ProseDirectLinkEstablishmentAccept : public Header
{
public:
  /**
   * Default constructor
   */
  ProseDirectLinkEstablishmentAccept ();
  /**
   * Default destructor
   */
  ~ProseDirectLinkEstablishmentAccept ();

  /**
  * Get the type ID.
  *
  * \return the object TypeId
  */
  static TypeId GetTypeId (void);

  /**
   * Get the TypeId for this instance
   *
   * \return the TypeId for this instance
   */
  TypeId GetInstanceTypeId (void) const;

  /**
   * Print a string representation of this header to the
   * std::ostream object passed as a parameter
   *
   * \param os The ostream where to print
   */
  void Print (std::ostream &os) const;

  /**
   * Get the size of the serialized version of this header
   *
   * \return The size of the serialized version of this header
   */
  uint32_t GetSerializedSize (void) const;

  /**
   * Write in the Iterator object passed as a parameter the
   * serialized version of this header
   *
   * \param start The Iterator for the Buffer where to write
   */
  void Serialize (Buffer::Iterator start) const;

  /**
   * Read the provided Buffer and deserialize a header from there
   *
   * \param start The Iterator for the Buffer where to read
   * \return The size of the deserialized header
   */
  uint32_t Deserialize (Buffer::Iterator start);


  /**
   * Function to set all the fields of the header
   *
   * \param seq The sequence number of the packet
   * \param srcUserInfo Source user info contents
   * \param qosFlowDescriptions PC5 QoS flow descriptions
   * \param userPlaneSecConfig configuration of UE PC5 unicast user plane security protection
   * \param ipAddressConfig IP address config to indicate the configuration options for IP address used by the UE over this direct link.
   * \param linkLocalIpv6Address contains a link-local IPv6 address.
   */
  void SetParameters (
    uint8_t seq,
    uint32_t srcUserInfo,
    std::vector<uint8_t> qosFlowDescriptions,
    uint8_t userPlaneSecConfig,
    uint8_t ipAddressConfig,
    std::vector<uint8_t> linkLocalIpv6Address);

  /**
   * Get the Message Identifier
   *
   * \return the Message Identifier
   */
  uint8_t GetMessageIdentifier ();

  /**
   * Get the Sequence Number
   *
   * \return the Sequence Number
   */
  uint16_t GetSequenceNumber ();

  /**
   * Set the Sequence Number
   * \param seq The sequence number of the packet
   */
  void SetSequenceNumber (uint8_t seq);

  /**
    * Set the UE application layer ID
    *
    * \param srcUserInfo the UE application layer ID
    */
  void SetSourceUserInfo (uint32_t srcUserInfo);

  /**
    * Get the UE application layer ID
    *
    * \return the UE application layer ID
    */
  uint32_t GetSourceUserInfo ();

  /**
   * Set PC5 QoS flow descriptions
   *
   * \param qosFlowDescriptions PC5 QoS flow descriptions
   */
  void SetPc5QoSFlowDescriptions (std::vector<uint8_t> qosFlowDescriptions);

  /**
   * Get PC5 QoS flow descriptions
   *
   * \return the PC5 QoS flow descriptions
   */
  std::vector<uint8_t> GetPc5QoSFlowDescriptions ();


  /**
   * Set the configuration of UE PC5 unicast user plane security protection
   * \param userPlaneSecConfig configuration of UE PC5 unicast user plane security protection
   */
  void SetUserPlaneSecurityProtectionConfiguration (uint8_t userPlaneSecConfig);

  /**
   * Get the configuration of UE PC5 unicast user plane security protection
   *
   * \return the configuration of UE PC5 unicast user plane security protection
   */
  uint16_t GetUserPlaneSecurityProtectionConfiguration ();


  /**
  * Set the IP address config
  *
  * \param ipAddressConfig the IP address config
  */
  void SetIpAddressConfig (uint8_t ipAddressConfig);

  /**
    * Get the IP address config
    *
    * \return the IP address config
    */
  uint8_t GetIpAddressConfig ();


  /**
   * Set link-local IPv6 address
   *
   * \param linkLocalIpv6Address link-local IPv6 address
   */
  void SetLinkLocalIpv6Address (std::vector<uint8_t> linkLocalIpv6Address);

  /**
   * Get link-local IPv6 address
   *
   * \return link-local IPv6 address
   */
  std::vector<uint8_t> GetLinkLocalIpv6Address ();

private:

  //Mandatory IEs
  uint8_t m_msgId;   ///< Message identity
  uint8_t m_seqNum;  ///< Sequence number
  uint32_t m_srcUserInfo;   ///< Source user info contents (Format 'Application layer ID', assumed with min 4 octets) No min defined in standard ATM
  std::vector<uint8_t> m_qosFlowDescriptions; ///< PC5 QoS flow descriptions contents (complex structure, min 6 octets)
  uint8_t m_userPlaneSecConfig; ///< Configuration of UE PC5 unicast user plane security protection contents (1 octet)

  // Optional IEs
  bool m_hasIpAddressConfig; ///< Flag indicating if the IP address configuration  is present
  uint8_t m_ipAddressConfig; ///< IP address configuration contents (1 octet)
  bool m_hasLinkLocalIpv6Address; ///< Flag indicating if the link-local IPv6 address is present
  std::vector<uint8_t> m_linkLocalIpv6Address;  ///< link-local IPv6 address contents (16 octets)

};

/**
 * \ingroup lte
 *
 * \brief Header of the ProSe direct link establishment reject message
 *        (as per TS 24.554 V0.4.0 (2021-08) Section 10.3.3)
 */
class ProseDirectLinkEstablishmentReject : public Header
{
public:
  /**
    * Default constructor
    */
  ProseDirectLinkEstablishmentReject ();
  /**
   * Default destructor
   */
  ~ProseDirectLinkEstablishmentReject ();

  /**
  * Get the type ID.
  *
  * \return the object TypeId
  */
  static TypeId GetTypeId (void);

  /**
   * Get the TypeId for this instance
   *
   * \return the TypeId for this instance
   */
  TypeId GetInstanceTypeId (void) const;

  /**
   * Print a string representation of this header to the
   * std::ostream object passed as a parameter
   *
   * \param os The ostream where to print
   */
  void Print (std::ostream &os) const;

  /**
   * Get the size of the serialized version of this header
   *
   * \return The size of the serialized version of this header
   */
  uint32_t GetSerializedSize (void) const;

  /**
   * Write in the Iterator object passed as a parameter the
   * serialized version of this header
   *
   * \param start The Iterator for the Buffer where to write
   */
  void Serialize (Buffer::Iterator start) const;

  /**
   * Read the provided Buffer and deserialize a header from there
   *
   * \param start The Iterator for the Buffer where to read
   * \return The size of the deserialized header
   */
  uint32_t Deserialize (Buffer::Iterator start);


  /**
   * Get the Message Identifier
   *
   * \return the Message Identifier
   */
  uint8_t GetMessageIdentifier ();

  /**
   * Get the Sequence Number
   *
   * \return the Sequence Number
   */
  uint16_t GetSequenceNumber ();

  /**
   * Set the Sequence Number
   * \param seq The sequence number of the packet
   */
  void SetSequenceNumber (uint8_t seq);

  /**
   * Set the PC5 Signaling Protocol Cause
   *
   * \param pc5SignallingProtocolCause the PC5 Signaling Protocol Cause
   */
  void SetPc5SignallingProtocolCause (uint8_t pc5SignallingProtocolCause);

  /**
   * Get the PC5 Signaling Protocol Cause
   *
   * \return the PC5 Signaling Protocol Cause
   */
  uint8_t GetPc5SignallingProtocolCause ();

private:

  uint8_t m_msgId;   ///< message identifier
  uint8_t m_seqNum;  ///< sequence number
  uint8_t m_pc5SignallingProtocolCause;  ///< pc5 signalling protocol cause value

};


}// namespace ns3

#endif //NR_SL_PC5_SIGNALLING_HEADER_H

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
 * TODO: The following messages are a C&P of the LTE messages. These were used
 * for initial development of the protocol framework. They will be replaced
 * with the NR messages once fully available in the standard.
 */

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
  static uint64_t s_seqNum;
  /**
   * Return a new sequence number
   * \return A new sequence number value
   */
  static uint64_t GenerateSeqNum ()
  {
    return ++s_seqNum;
  }
};


/**
 * \ingroup lte
 *
 * \brief Header of the ProSe direct link establishment request message
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
   * \param userInfoType user info type.
   * \param userInfo user info to provide either a User Info received from upper layers, the PRUK ID received from the PKMF that the remote UE wants to use to connect to a relay, or the IMSI of the remote UE using this direct link.
   * \param ipAddressConfig IP address config to indicate the configuration options for IP address used by the UE over this direct link.
   * \param maxInactivityPeriod indicates the maximum inactivity period of the requesting UE over the direct link.
   * \param nonce1 indicates the nonce value generated by the UE which initiated the direct link setup procedure or direct link rekeying procedure.
   * \param ueSecurityCapabilities indicates which security algorithms are supported by the UE.
   * \param msb carries the 8 most significant bits of the KD-sess ID.
   * \param kdId carries the identity of the KD held by a UE.
   * \param relayServiceCode parameter that identifies a connectivity service the UE-to-Network relay provides.
   * \param signature indicates the ECCSI signature calculated based on information exchanged during the direct link setup.
   * \param linkLocalIpv6Address contains a link-local IPv6 address.
   */
  void SetParameters (uint8_t userInfoType, uint64_t userInfo, uint8_t ipAddressConfig, uint32_t maxInactivityPeriod, std::vector<uint8_t> nonce1, uint16_t ueSecurityCapabilities, uint8_t msb, int32_t kdId, uint32_t relayServiceCode, std::vector<uint8_t> signature, std::vector<uint8_t> linkLocalIpv6Address);


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
  void SetSequenceNumber (uint32_t seq);

  /**
   * Set the User Info
   *
   * \param userInfoType User Info Type
   * \param userInfo User Info
   */
  void SetUserInfo (uint8_t userInfoType, uint64_t userInfo);

  /**
   * Get the User Info
   *
   * \return userInfo User Info
   */
  uint64_t GetUserInfo ();

  /**
   * Get the User Info Type
   *
   * \return userInfoType User Info Type
   */
  uint8_t GetUserInfoType ();

  /**
   * Set the User Info to be the IMSI
   *
   * \param imsi the IMSI
   */
  void SetImsi (uint64_t imsi);

  /**
   * Get the IMSI from the User Info
   *
   * \return imsi the IMSI
   */
  uint64_t GetImsi ();

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
   * Set the maximum inactivity period
   *
   * \param maxInactivityPeriod the maximum inactivity period
   */
  void SetMaximumInactivityPeriod (uint32_t maxInactivityPeriod);

  /**
   * Get the maximum inactivity period
   *
   * \return the maximum inactivity period
   */
  uint32_t GetMaximumInactivityPeriod ();

  /**
   * Set the nonce value
   *
   * \param nonce1 the nonce value
   */
  void SetNonce1 (std::vector<uint8_t> nonce1);

  /**
  * Get the nonce value
  *
  * \return the nonce value
  */
  std::vector<uint8_t> GetNonce1 ();

  /**
   * Set the supported security algorithms
   *
   * \param ueSecurityCapabilities the supported security algorithms
   */
  void SetUeSecurityCapabilities (uint16_t ueSecurityCapabilities);

  /**
  * Get the supported security algorithms
  *
  * \return the supported security algorithms
  */
  uint16_t GetUeSecurityCapabilities ();

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
   * Set the identity of the KD held by a UE
   *
   * \param kdId the identity of the KD held by a UE.
   */
  void SetKdId (uint32_t kdId);

  /**
   * Get the identity of the KD held by a UE
   *
   * \return the identity of the KD held by a UE.
   */
  uint32_t GetKdId ();

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

  /**
   * Set the ECCSI signature
   *
   * \param signature the ECCSI signature
   */
  void SetSignature (std::vector<uint8_t> signature);

  /**
   * Get the ECCSI signature
   *
   * \return the ECCSI signature
   */
  std::vector<uint8_t> GetSignature ();

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
  /**
   * message identifier
   */
  uint8_t m_msgId;

  /**
   * sequence number
   */
  uint16_t m_seqNum;

  /**
   * user info
   */
  uint64_t m_userInfo;

  /**
   * user info type
   */
  uint8_t m_userInfoType;

  /**
   * ip adress config
   */
  uint8_t m_ipAddressConfig;

  /**
   * maximum inactivity period
   */
  uint32_t m_maxInactivityPeriod;

  /**
   * nonce
   */
  std::vector<uint8_t> m_nonce1;

  /**
   * ue security  capabilities
   */
  uint16_t m_ueSecurityCapabilities;

  /**
   * most significant bits of the KD-sess ID
   */
  uint8_t m_msb;

  /**
  * optinal KD ID
  */
  bool m_hasKdId;

  /**
   * identity of the KD
   */
  int32_t m_kdId;

  /**
   * optinal relay service code
   */
  bool m_hasRelayServiceCode;

  /**
   * relay service code
   */
  uint32_t m_relayServiceCode;

  /**
   * optinal signature
   */
  bool m_hasSignature;

  /**
   * signature
   */
  std::vector<uint8_t> m_signature;

  /**
   * optinal link-local IPv6 address
   */
  bool m_hasLinkLocalIpv6Address;

  /**
   * link-local IPv6 address
   */
  std::vector<uint8_t> m_linkLocalIpv6Address;
};

/**
 * \ingroup lte
 *
 * \brief Header of the ProSe direct link establishment accept message
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
   * \param ipAddressConfig IP address config to indicate the configuration options for IP address used by the UE over this direct link.
   * \param linkLocalIpv6Address contains a link-local IPv6 address.
   */
  void SetParameters (uint8_t ipAddressConfig, std::vector<uint8_t> linkLocalIpv6Address);

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
  void SetSequenceNumber (uint32_t seq);

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
  /**
   * message identifier
   */
  uint8_t m_msgId;

  /**
   * sequence number
   */
  uint16_t m_seqNum;

  /**
   * ip adress config
   */
  uint8_t m_ipAddressConfig;

  /**
   * optinal link-local IPv6 address
   */
  bool m_hasLinkLocalIpv6Address;

  /**
   * link-local IPv6 address
   */
  std::vector<uint8_t> m_linkLocalIpv6Address;


};

/**
 * \ingroup lte
 *
 * \brief Header of the ProSe direct link establishment reject message
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
  void SetSequenceNumber (uint32_t seq);

  /**
   * Set the PC5 Signaling Protocol Cause Value
   *
   * \param pc5SignallingCauseValue the PC5 Signaling Protocol Cause Value
   */
  void SetPc5SignallingCauseValue (uint8_t pc5SignallingCauseValue);

  /**
   * Get the PC5 Signaling Protocol Cause Value
   *
   * \return the PC5 Signaling Protocol Cause Value
   */
  uint8_t GetPc5SignallingCauseValue ();

private:
  /**
   * message identifier
   */
  uint8_t m_msgId;

  /**
   * sequence number
   */
  uint16_t m_seqNum;

  /**
   * pc5 signalling protocol cause value
   */
  uint8_t m_pc5SignallingCauseValue;
};


}// namespace ns3

#endif //NR_SL_PC5_SIGNALLING_HEADER_H

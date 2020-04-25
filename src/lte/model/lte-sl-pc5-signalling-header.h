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

#ifndef LTE_SL_PC5_SIGNALLING_HEADER_H
#define LTE_SL_PC5_SIGNALLING_HEADER_H

#include "ns3/header.h"
#include <stdint.h>
#include <vector>
#include "ns3/string.h"

namespace ns3 {

/**
 * \ingroup nist
 * \brief To peek at the first byte of the received PC5 Signaling message and identify the type
 */

class LteSlPc5SignallingMessageType : public Header
{
public:
  /**
    * Default constructor
    */
  LteSlPc5SignallingMessageType ();
  /**
   * Default destructor
   */
  ~LteSlPc5SignallingMessageType ();

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
    DirectCommunicationRequest = 1,
    DirectCommunicationAccept,
    DirectCommunicationReject,
    DirectCommunicationKeepalive,
    DirectCommunicationKeepaliveAck,
    DirectCommunicationRelease,
    DirectCommunicationReleaseAccept,
    DirectSecurityModeCommand = 12,
    DirectSecurityModeComplete,
    DirectSecurityModeReject,
    DirectRekeyingRequest,
    DirectRekeyingResponse,
    DirectRekeyingTrigger,
    RemoteUeInfoRequest,
    RemoteUeInfoResponse
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

class LtePc5SignallingHeaderSequenceNumber
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
 * \ingroup nist
 * \brief Brief headers for PC5 signaling messages
 *
 * This class has fields corresponding to a DIRECT_COMMUNICATION_REQUEST message :
 * DIRECT_COMMUNICATION_REQUEST message identity
 * Sequence Number
 * User Info
 * IP address config
 * Maximum Inactivity Period
 * Nonce_1
 * UE Security Capabilities
 * MSB of KDsess ID
 * Kd ID
 * Relay Service Code
 * Signature
 * Link Local IPv6 Address
 *
 */
class DirectCommunicationRequest : public Header
{
public:
  /**
   * Default constructor
    */
  DirectCommunicationRequest ();
  /**
   * Default destructor
   */
  ~DirectCommunicationRequest ();

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

/*
 * This class has fields corresponding to a DIRECT_COMMUNICATION_ACCEPT message :
 * DIRECT_COMMUNICATION_ACCEPT message identity
 * Sequence Number
 * IP address config
 * Link Local IPv6 Address
 *
 */
class DirectCommunicationAccept : public Header
{
public:
  /**
   * Default constructor
   */
  DirectCommunicationAccept ();
  /**
   * Default destructor
   */
  ~DirectCommunicationAccept ();

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

/*
 * This class has fields corresponding to a DIRECT_COMMUNICATION_REJECT message :
 * DIRECT_COMMUNICATION_REJECT message identity
 * Sequence Number
 * PC5 Signalling Cause Value
 *
 */
class DirectCommunicationReject : public Header
{
public:
  /**
    * Default constructor
    */
  DirectCommunicationReject ();
  /**
   * Default destructor
   */
  ~DirectCommunicationReject ();

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


/*
 * This class has fields corresponding to a DIRECT_COMMUNICATION_KEEPALIVE message :
 * DIRECT_COMMUNICATION_KEEPALIVE message identity
 * Sequence Number
 * Keepalive Counter
 * Maximum Inactivity Period
 *
 */
class DirectCommunicationKeepalive : public Header
{
public:
  /**
   * Default constructor
   */
  DirectCommunicationKeepalive ();
  /**
   * Default destructor
   */
  ~DirectCommunicationKeepalive ();

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
   * \param keepaliveCounter contains a 32-bit counter used for the direct link keepalive procedure.
   * \param maxInactivityPeriod indicates the maximum inactivity period of the requesting UE over the direct link.
   */
  void SetParameters (uint32_t keepaliveCounter, uint32_t maxInactivityPeriod);

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
   * Set the Keepalive Counter
   *
   * \param keepaliveCounter the Keepalive Counter
   */
  void SetKeepaliveCounter (uint32_t keepaliveCounter);

  /**
   * Get the Keepalive Counter
   *
   * \return the Keepalive Counter
   */
  uint32_t GetKeepaliveCounter ();

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
   * keepalive counter
   */
  uint32_t m_keepaliveCounter;

  /**
   * optional maximum inactivity period
   */
  bool m_hasMaxInactivityPeriod;

  /**
  * maximum inactivity period
  */
  uint32_t m_maxInactivityPeriod;

};

/*
 * This class has fields corresponding to a DIRECT_COMMUNICATION_KEEPALIVE_ACK message :
 * DIRECT_COMMUNICATION_KEEPALIVE_ACK message identity
 * Sequence Number
 * Keepalive Counter
 * Maximum Inactivity Period
 *
 */
class DirectCommunicationKeepaliveAck : public Header
{
public:
  /**
   * Default constructor
   */
  DirectCommunicationKeepaliveAck ();
  /**
   * Default destructor
   */
  ~DirectCommunicationKeepaliveAck ();

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
   * Set the Keepalive Counter
   *
   * \param keepaliveCounter the Keepalive Counter
   */
  void SetKeepaliveCounter (uint32_t keepaliveCounter);

  /**
   * Get the Keepalive Counter
   *
   * \return the Keepalive Counter
   */
  uint32_t GetKeepaliveCounter ();

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
   * keepalive counter
   */
  uint32_t m_keepaliveCounter;

};

/*
 * This class has fields corresponding to a DIRECT_COMMUNICATION_RELEASE message :
 * DIRECT_COMMUNICATION_RELEASE message identity
 * Sequence Number
 * Release Reason
 *
 */
class DirectCommunicationRelease : public Header
{
public:
  /**
    * Default constructor
    */
  DirectCommunicationRelease ();
  /**
   * Default destructor
   */
  ~DirectCommunicationRelease ();

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
   * Set the Release Reason
   *
   * \param releaseReason
   */
  void SetReleaseReason (uint8_t releaseReason);

  /**
   * Get the Release Reason
   *
   * \return the Release Reason
   */
  uint8_t GetReleaseReason ();

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
   * release reason
   */
  uint8_t m_releaseReason;


};

/*
 * This class has fields corresponding to a DIRECT_COMMUNICATION_RELEASE_ACCEPT message :
 * DIRECT_COMMUNICATION_RELEASE_ACCEPT message identity
 * Sequence Number
 *
 */
class DirectCommunicationReleaseAccept : public Header
{
public:
  /**
    * Default constructor
    */
  DirectCommunicationReleaseAccept ();
  /**
   * Default destructor
   */
  ~DirectCommunicationReleaseAccept ();

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

private:
  /**
   * message identifier
   */
  uint8_t m_msgId;

  /**
   * sequence number
   */
  uint16_t m_seqNum;

};


/*
 * This class has fields corresponding to a DIRECT_SECURITY_MODE_COMMAND message :
 * DIRECT_SECURITY_MODE_COMMAND message identity
 * Sequence Number
 * UE Security Capabilities
 * Nonce 2
 * Chosen Algorithms
 * LSB of Kdsess ID
 * MSB of Kd ID
 * Kd Freshness
 * GPI
 * User Info
 * Signature
 * Encrypted Payload
 *
 */
class DirectSecurityModeCommand : public Header
{
public:
  /**
    * Default constructor
    */
  DirectSecurityModeCommand ();
  /**
   * Default destructor
   */
  ~DirectSecurityModeCommand ();

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
  * \param ueSecurityCapabilities indicate which security algorithms are supported by the UE.
  * \param nonce2 indicate the nonce value generated by the UE which initiated the direct security mode control procedure.
  * \param chosenAlgorithmsIntegrity indicate the algorithms to be used for integrity protection.
  * \param chosenAlgorithmCyphering indicate the algorithms to be used for ciphering protection.
  * \param lsb carry the 16 least significant bits of the KD ID.
  * \param msb carry the 16 most significant bits of the KD ID.
  * \param kdFreshness indicate the nonce value generated by initiating PKMF to ensure that any calculated KD is fresh.
  * \param gpi include the GBA Push Information.
  * \param userInfoType provide the user info type.
  * \param userInfo provide either the User Info received from upper layers identifying the user which is using this direct link, the PRUK ID received from the PKMF that the remote UE wants to use to connect to a relay using this direct link, or the IMSI of the remote UE using this direct link.
  * \param signature indicate the ECCSI signature calculated based on information exchanged during the direct link setup.
  * \param encryptedPayload to indicate the encrypted data encapsulating the shared secret key to be used for the established link.
  */
  void SetParameters (uint16_t ueSecurityCapabilities, std::vector<uint8_t> nonce2, uint8_t chosenAlgorithmsIntegrity, uint8_t chosenAlgorithmCyphering, uint8_t lsb, uint16_t msb, std::vector<uint8_t> kdFreshness, std::vector<uint8_t> gpi,  uint8_t userInfoType, uint64_t userInfo, std::vector<uint8_t> signature, std::vector<uint8_t> encryptedPayload);


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
   * Get UeSecurityCapabilities
   *
   * \return the UE Security Capabilities
   */
  uint16_t GetUeSecurityCapabilities ();

  /**
   * Set UeSecurityCapabilities
   *
   * \param ueSecurityCapabilities the UE Security Capabilities
   */
  void SetUeSecurityCapabilities (uint16_t ueSecurityCapabilities);

  /**
   * Get Nonce 2
   *
   * \return the nonce 2
   */
  std::vector<uint8_t> GetNonce2 ();

  /**
   * Set Nonce 2
   *
   * \param nonce2 Nonce 2
   */
  void SetNonce2 ( std::vector<uint8_t> nonce2);

  /**
   * Get Chosen Algorithms integrity
   *
   * \return the chosen algorithms integrity
   */
  uint8_t GetChosenAlgorithmsIntegrity ();

  /**
   * Get Chosen Algorithms Cyphering
   *
   * \return the chosen algorithms Cyphering
   */
  uint8_t GetChosenAlgorithmsCyphering ();

  /**
   * Set Chosen Algorithms
   *
   * \param chosenAlgorithmsIntegrity the chosen algorithms for integrity
       * \param chosenalgorithmsCyphering the chosen algorithms for cyphering
   */
  void SetChosenAlgorithms (uint8_t chosenAlgorithmsIntegrity, uint8_t chosenalgorithmsCyphering);

  /**
   * Get LSB
   *
   * \return LSB
   */
  uint8_t GetLsb ();

  /**
   * Set LSB
   *
   * \param lsb LSB
   */
  void SetLsb (uint8_t lsb);

  /**
   * Get MSB
   *
   * \return MSB
   */
  uint16_t GetMsb ();

  /**
   * Set MSB
   *
   * \param msb MSB
   */
  void SetMsb (uint16_t msb);

  /**
   * Get kd Fresheness
   *
   * \return kd Freshness
   */
  std::vector<uint8_t> GetKdFreshness ();

  /**
   * Set kd Fresheness
   *
   * \param kdFreshness kd Freshness
   */
  void SetKdFreshness (std::vector<uint8_t> kdFreshness);

  /**
   * Get GPI
   *
   * \return GPI
   */
  std::vector<uint8_t> GetGpi ();

  /**
   * Set GPI
   *
   * \param gpi GPI
   */
  void SetGpi (std::vector<uint8_t> gpi);

  /**
   * Get user info
   *
   * \return user info
   */
  uint64_t GetUserInfo ();

  /**
   * Get the User Info
   *
   * \return userInfo User Info
   */
  uint8_t GetUserInfoType ();

  /**
   * Set user info
   *
   * \param userInfoType user info type
       * \param userInfo user info
   */
  void SetUserInfo (uint8_t userInfoType, uint64_t userInfo);

  /**
   * Get Signature
   *
   * \return signature
   */
  std::vector<uint8_t> GetSignature ();

  /**
   * Set signature
   *
   * \param signature Signature
   */
  void SetSignature (std::vector<uint8_t> signature);

  /**
   * Get Encrypted Payload
   *
   * \return  Encrypted Payload
   */
  std::vector<uint8_t> GetEncryptedPayload ();

  /**
   * Set Encrypted Payload
   *
   * \param encryptedPayload Encrypted Payload
   */
  void SetEncryptedPayload (std::vector<uint8_t> encryptedPayload);

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
   * ue security  capabilities
   */
  uint16_t m_ueSecurityCapabilities;

  /**
   * nonce2
   */
  std::vector<uint8_t> m_nonce2;

  /**
    * chosen algorithms integrity
    */
  uint8_t m_chosenAlgorithmsIntegrity;

  /**
   * chosen algorithms cyphering
   */
  uint8_t m_chosenAlgorithmsCyphering;

  /**
   * least significant bits of the KD-sess ID
   */
  uint8_t m_lsb;

  /**
   * optional msb
   */
  bool m_hasMsb;

  /**
   * most significant bits of the KD-sess ID
   */
  uint16_t m_msb;

  /**
  * optinal KD freshness
  */
  bool m_hasKdFreshness;

  /**
   * the KD freshness
   */
  std::vector<uint8_t> m_kdFreshness;

  /**
   * optional GPI
   */
  bool m_hasGpi;

  /**
   * GPI = GBA Push Information
   */
  std::vector<uint8_t> m_gpi;

  /**
   * optional user info
   */
  bool m_hasUserInfo;

  /**
   * user info
   */
  uint64_t m_userInfo;

  /**
   * user info type
   */
  uint8_t m_userInfoType;

  /**
   * optional signature
   */
  bool m_hasSignature;

  /**
   * signature
   */
  std::vector<uint8_t> m_signature;

  /**
   * optional encrypted payload
   */
  bool m_hasEncryptedPayload;

  /**
   * encrypted payload
   */
  std::vector<uint8_t> m_encryptedPayload;
};


/*
 * This class has fields corresponding to a DIRECT_SECURITY_MODE_COMPLETE message :
 * DIRECT_SECURITY_MODE_COMPLETE message identity
 * Sequence Number
 * LSB of Kd ID
 *
 */
class DirectSecurityModeComplete : public Header
{
public:
  /**
    * Default constructor
    */
  DirectSecurityModeComplete ();
  /**
   * Default destructor
   */
  ~DirectSecurityModeComplete ();

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
   * Get LSB
   *
   * \return LSB
   */
  uint16_t GetLsb ();

  /**
   * Set LSB
   *
   * \param lsb LSB
   */
  void SetLsb (uint16_t lsb);

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
   * optional lsb
   */
  bool m_hasLsb;

  /**
   * least significant bits of the Kd ID
   */
  uint16_t m_lsb;

};

/*
 * This class has fields corresponding to a DIRECT_SECURITY_MODE_REJECT message :
 * DIRECT_SECURITY_MODE_REJECT message identity
 * Sequence Number
 * PC5 Signalling Protocol Cause Value
 * RAND
 * AUTS
 */

class DirectSecurityModeReject : public Header
{
public:
  /**
    * Default constructor
    */
  DirectSecurityModeReject ();
  /**
   * Default destructor
   */
  ~DirectSecurityModeReject ();

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

  /**
   * Set RAND: provide the mobile station with a non-predictable challenge for the AKA procedure
   *
   * \param rand RAND
   */
  void SetRand (std::vector<uint8_t> rand);

  /**
   * Get RAND: provide the mobile station with a non-predictable challenge for the AKA procedure
   *
   * \return RAND
   */
  std::vector<uint8_t> GetRand ();

  /**
   * Set AUTS: provide the network with the necessary information to begin a re-synchronisation as part of the AKA procedure
   *
   * \param auts AUTS
   */
  void SetAuts (std::vector<uint8_t> auts);

  /**
   * Get AUTS: provide the network with the necessary information to begin a re-synchronisation as part of the AKA procedure
   *
   * \return AUTS
   */
  std::vector<uint8_t> GetAuts ();



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

  /**
   * optional RAND
   */
  bool m_hasRand;

  /**
   * RAND
   */
  std::vector<uint8_t> m_rand;

  /**
   * optional AUTS
   */
  bool m_hasAuts;

  /**
   * AUTS
   */
  std::vector<uint8_t> m_auts;
};

/*
 * This class has fields corresponding to a DIRECT_REKEYING_REQUEST message :
 * DIRECT_REKEYING_REQUEST message identity
 * Sequence Number
 * UE Security Capabilities
 * Nonce 1
 * MSB Kd-sess ID
 * Auth Flag: indicate that the KD is to be refreshed.
 * PRUK ID: provide the ProSe UE-to-network relay with the latest PRUK ID at the remote UE when the remote UE triggers rekeying.
 *
 */
class DirectRekeyingRequest : public Header
{
public:
  /**
    * Default constructor
    */
  DirectRekeyingRequest ();
  /**
   * Default destructor
   */
  ~DirectRekeyingRequest ();

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
   * \param ueSecurityCapabilities indicate which security algorithms are supported by the UE.
   * \param nonce1 indicate the nonce value generated by the UE which initiated the direct link setup procedure or direct link rekeying procedure.
   * \param msb carry the 8 most significant bits of the KD ID.
   * \param authFlag indicate that the KD is to be refreshed.
   * \param prukId provide the ProSe UE-to-network relay with the latest PRUK ID at the remote UE when the remote UE triggers rekeying.
   */
  void SetParameters (uint16_t ueSecurityCapabilities, std::vector<uint8_t> nonce1, uint8_t msb, uint8_t authFlag, std::vector<uint8_t> prukId);


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
    * Get Nonce 1
    *
    * \return nonce1
    */
  std::vector<uint8_t> GetNonce1 ();

  /**
   * Set Nonce 1
   *
   * \param nonce1 Nonce 1
   */
  void SetNonce1 ( std::vector<uint8_t> nonce1);

  /**
   * Set the 8 most significant bits of the KD-sess ID
   *
   * \param msb the 8 most significant bits of the KD-sess ID
   */
  void SetMsb (uint8_t msb);

  /**
   * Get the 8 most significant bits of the KD-sess ID
   *
   * \return the 8 most significant bits of the KD-sess ID
   */
  uint8_t GetMsb ();

  /**
   * Set Auth Flag
   *
   * \param authFlag Auth Flag
   */
  void SetAuthFlag (uint8_t authFlag);

  /**
   * Get Auth Flag
   *
   * \return Auth Flag
   */
  uint8_t GetAuthFlag ();

  /**
   * Set PRUK ID
   *
   * \param prukId PRUK ID
   */
  void SetPrukId (std::vector<uint8_t> prukId);

  /**
   * Get PRUK ID
   *
   * \return PRUK ID
   */
  std::vector<uint8_t> GetPrukId ();


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
   * ue security  capabilities
   */
  uint16_t m_ueSecurityCapabilities;

  /**
   * nonce1
   */
  std::vector<uint8_t> m_nonce1;

  /**
    * most significant bits of the KD-sess ID
    */
  uint8_t m_msb;

  /**
   * optinal Auth Flag
   */
  bool m_hasAuthFlag;

  /**
   * Auth Flag: indicate that the KD is to be refreshed.
   */
  uint8_t m_authFlag;

  /**
   * optinal PRUK ID
   */
  bool m_hasPrukId;

  /**
   * PRUK ID : provide the ProSe UE-to-network relay with the latest PRUK ID at the remote UE when the remote UE triggers rekeying.
   */
  std::vector<uint8_t> m_prukId;

};



/*
 * This class has fields corresponding to a DIRECT_REKEYING_RESPONSE message :
 * DIRECT_REKEYING_RESPONSE message identity
 * Sequence Number
 *
 */
class DirectRekeyingResponse : public Header
{
public:
  /**
    * Default constructor
    */
  DirectRekeyingResponse ();
  /**
   * Default destructor
   */
  ~DirectRekeyingResponse ();

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

private:
  /**
   * message identifier
   */
  uint8_t m_msgId;

  /**
   * sequence number
   */
  uint16_t m_seqNum;


};


/*
 * This class has fields corresponding to a DIRECT_REKEYING_TRIGGER message :
 * DIRECT_REKEYING_TRIGGER message identity
 * Sequence Number
 *
 */
class DirectRekeyingTrigger : public Header
{
public:
  /**
    * Default constructor
    */
  DirectRekeyingTrigger ();
  /**
   * Default destructor
   */
  ~DirectRekeyingTrigger ();

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

private:
  /**
   * message identifier
   */
  uint8_t m_msgId;

  /**
   * sequence number
   */
  uint16_t m_seqNum;

};

/*
 * This class has fields corresponding to a DIRECT_REMOTE_UE_INFO_REQUEST message :
 * DIRECT_REMOTE_UE_INFO_REQUEST message identity
 * Sequence Number
 * Remote UE Information Type
 */

class RemoteUeInfoRequest : public Header
{
public:
  /**
    * Default constructor
    */
  RemoteUeInfoRequest ();
  /**
   * Default destructor
   */
  ~RemoteUeInfoRequest ();

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
   * Get Remote UE Information Type
   *
   * \return Remote UE Information Type
   */
  uint8_t GetRemoteUeInfoType ();

  /**
   * Set Remote UE Information Type
   *
   * \param remoteUeInfoType Remote UE Information Type
   */
  void SetRemoteUeInfoType (uint8_t remoteUeInfoType);

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
   * Remote UE Information Type
   */
  uint8_t m_remoteUeInfoType;

};

/*
 * This class has fields corresponding to a DIRECT_REMOTE_UE_INFO_RESPONSE message :
 * DIRECT_REMOTE_UE_INFO_RESPONSE message identity
 * Sequence Number
 * IMEI
 */

class RemoteUeInfoResponse : public Header
{
public:
  /**
    * Default constructor
    */
  RemoteUeInfoResponse ();
  /**
   * Default destructor
   */
  ~RemoteUeInfoResponse ();

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
   * Get IMEI
   *
   * \return IMEI
   */
  uint64_t GetImei ();

  /**
   * Set IMEI
   *
   * \param imei IMEI
   */
  void SetImei (uint64_t imei);

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
   * optional IMEI or IMEISV Digits
   */
  bool m_hasImei;

  /**
   * IMEI
   */
  uint64_t m_imei;

};


}// namespace ns3

#endif //LTE_SL_PC5_SIGNALLING_HEADER_H

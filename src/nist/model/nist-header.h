/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 This software was developed at the National Institute of Standards and
 Technology by employees of the Federal Government in the course of
 their official duties. Pursuant to titleElement 17 Section 105 of the United
 States Code this software is not subject to copyright protection and
 is in the public domain.
 NIST assumes no responsibility whatsoever for its use by other parties,
 and makes no guarantees, expressed or implied, about its quality,
 reliability, or any other characteristic.

 We would appreciate acknowledgement if the software is used.

 NIST ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
 DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING
 FROM THE USE OF THIS SOFTWARE.
*/

#ifndef NIST_HEADER_H
#define NIST_HEADER_H

#include <stdint.h>
#include "ns3/string.h"
#include "ns3/header.h"
#include "ns3/nstime.h"

namespace ns3 {
/**
 * \ingroup nist
 * \brief Packet id for NIST applications
 *
 * This class provides a sequential packet number that can be used for packet IDs
 */
 
class NistSequenceNumber
{
public:
  /**
   * Variable with the last packet ID generated
   */
  static uint64_t s_sequenceNumber;
  /**
   * Return a new packet ID
   * \return A new packet ID value
   */
  static uint64_t GetSequenceNumber()
  {
    return ++s_sequenceNumber;	
  }
};

/**
 * \ingroup nist
 * \brief Brief packet header for NIST application
 *
 * This header only contains fields for the sequence number and the time the 
 * packet was sent (at the application level).
 */
class NistBriefHeader : public Header
{
public:
  /**
   * Default constructor
   */
  NistBriefHeader ();
  /**
   * Default destructor
   */
  ~NistBriefHeader ();

  /**
   * Get the TypeId for this class
   * 
   * \return The TypeId for this class
   */
  static TypeId GetTypeId (void);
  /**
   * Get the TypeId for this instance
   * 
   * \return The TypeId for this instance
   */
  TypeId GetInstanceTypeId (void) const;

  /**
   * Set the sequence number
   *
   * \param seqnum The sequence number to use in this header
   */
  void SetSeqNum (uint64_t seqnum);
  /**
   * Get the sequence number in this header
   *
   * \return The sequence number in this header
   */
  uint64_t GetSeqNum (void);

  /**
   * Set the time at which this packet is sent
   *
   * \param sentTime The time at which this packet is sent
   */
  void SetSentTime (Time sentTime);
  /**
   * Get the time at which this packet is sent
   *
   * \return The time at which this packet is sent.
   */
  Time GetSentTime (void);

  /**
   * Deserialize this header in the Buffer provided as a parameter
   *
   * \param start The Iterator to the Buffer where to read the header fields
   * \return The size of the deserialized header.
   */
  uint32_t Deserialize (Buffer::Iterator start);
  /**
   * Get the size of the header in serialized form
   *
   * \return The size of the header in serialized form.
   */
  uint32_t GetSerializedSize (void) const;
  /**
   * Print a string version of the header in the ostream parameter provided.
   *
   * \param os The ostream where to print this header
   */
  void Print (std::ostream &os) const;
  /**
   * Serialize this header in the buffer provided as a parameter
   *
   * \param start Iterator to the Buffer where to serialize this header
   */
  void Serialize (Buffer::Iterator start) const;

private:
  /**
   * Sequence number
   */
  uint64_t m_seqnum;
  /**
   * Time at which the packet is sent
   */
  Time m_sentTime;

};

/**
 * \ingroup nist
 * \brief Packet header for NIST application
 *
 * This class has fields corresponding to a NIST application header
 * (Sequence Number, Source ID, Destination ID, Source Port, Destination Port, 
 * Stream ID, Application ID),  as well as methods for serialization to and 
 * deserialization from a byte buffer.
 */
 
class NistHeader : public Header 
{
public:

  /**
   * Default constructor
   */
  NistHeader();
  /**
   * Default destructor
   */
  ~NistHeader ();
  
  /**
   * Function to set all the fields of the header
   *
   * \param sourceId The Source ID
   * \param destinationId The Destination ID
   * \param sourcePort The Source Port
   * \param destinationPort The Destination Port
   * \param streamId The Stream ID
   * \param applicationId The Application ID
   */
  void SetParameters (uint32_t sourceId, uint32_t destinationId, 
      uint16_t sourcePort, uint16_t destinationPort, 
      uint32_t streamId, uint16_t applicationId);

  /**
   * Set the Source ID
   *
   * \param id The source id for this NistHeader
   */
  void SetSourceId (uint32_t id);
  /**
   * Set the Destination ID
   *
   * \param id the destination id for this NistHeader
   */
  void SetDestinationId (uint32_t id);
  /**
   * Set the Source Port
   *
   * \param port The source port for this NistHeader
   */
  void SetSourcePort (uint16_t port);  
  /**
   * Set the Destination Port
   *
   * \param port the destination port for this NistHeader
   */
  void SetDestinationPort (uint16_t port);
  /**
   * Set the Stream ID
   *
   * \param id the stream id for this NistHeader
   */
  void SetStreamId (uint32_t id);
  /**
   * Set the Application ID
   *
   * \param id the application id for this NistHeader
   */
  void SetApplicationId (uint16_t id);
  
  /**
   * Get the Sequence Number
   *
   * \return The sequence number for this NistHeader
   */
  uint64_t GetSequenceNumber (void) const;
  /**
   * Get the Source ID
   *
   * \return The source id for this NistHeader
   */
  uint32_t GetSourceId (void) const;
  /**
   * Get the Destination ID
   *
   * \return the destination id for this NistHeader
   */
  uint32_t GetDestinationId (void) const;
  /**
   * Get the Source Port
   *
   * \return The source port for this NistHeader
   */
  uint16_t GetSourcePort (void) const;
  /**
   * Get the Destination Port
   *
   * \return the destination port for this NistHeader
   */
  uint16_t GetDestinationPort (void) const;
  /**
   * Get the Stream ID
   *
   * \return the stream id for this NistHeader
   */
  uint32_t GetStreamId (void) const;
  /**
   * Get the Application ID
   *
   * \return the application id for this NistHeader
   */
  uint16_t GetApplicationId (void) const;


  /**
   * Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * Get the TypeId for this instance
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
   * SequenceNumber
   */
  uint64_t m_sequenceNumber;
  /**
   * Source ID
   */
  uint32_t m_sourceId;
  /**
   * Destination ID
   */
  uint32_t m_destinationId;
  /**
   * Source Port
   */
  uint16_t m_sourcePort;
  /**
   * Destination Port
   */
  uint16_t m_destinationPort;
  /**
   * Stream ID
   */
  uint32_t m_streamId;
  /**
   * Application ID
   */
  uint16_t m_applicationId;

};

} // namespace ns3

#endif // NIST_HEADER_H

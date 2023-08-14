/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public service. You may use,
 * copy and distribute copies of the software in any medium, provided that you
 * keep intact this entire notice. You may improve,modify and create derivative
 * works of the software or any portion of the software, and you may copy and
 * distribute such modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and nature of
 * any such change. Please explicitly acknowledge the National Institute of
 * Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT
 * AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE
 * OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT
 * ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY
 * REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF,
 * INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY,
 * OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

#ifndef INTEL_HTTP_HEADER_H
#define INTEL_HTTP_HEADER_H

#include <cstdint>
#include <ns3/header.h>

namespace ns3 {
namespace psc {

/**
 * \ingroup psc
 * \brief Packet header for the Intel HTTP application
 *
 * This header is used by the Intel HTTP application, and provides a flag indicating whether a request is for the main HTML object
 * or any of the embedded objects (this is used in requests from the client), a count of the embedded objects in the main HTML object
 * (this is used in the response for the main HTML object), and the total size of the payload
 *
 * See the model documentation for details about this class
 */
class IntelHttpHeader : public Header
{
public:
  /**
   * Possible types of HTTP request/responses.
   */
  enum Type { Main, Embedded };

  /**
   * Default constructor
   */
  IntelHttpHeader ();
  /**
   * Default destructor
   */
  ~IntelHttpHeader () override;

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
  TypeId GetInstanceTypeId (void) const override;

  /**
   * Set the request type
   *
   * \param requestType The request type
   */
  void SetRequestType (Type requestType);
  /**
   * Get the request type
   *
   * \return The request type
   */
  Type GetRequestType (void) const;
  /**
   * Set the number of embedded objects linked in the HTML document
   *
   * \param numEmbeddedObjects Number of embedded objects linked in the HTML document
   */
  void SetNumberEmbeddedObjects (uint16_t numEmbeddedObjects);
  /**
   * Get the number of embedded objects linked in the HTML document
   *
   * \return The number of embedded objects linked in the HTML document
   */
  uint16_t GetNumberEmbeddedObjects (void) const;
  /**
   * Set the Payload Size
   *
   * \param payloadSize The new payload size in bytes
   */
  void SetPayloadSize (uint32_t payloadSize);
  /**
   * Get the total payload size in bytes
   *
   * \return uint32_t Total bytes in the payload
   */
  uint32_t GetPayloadSize (void) const;
  /**
   * Deserialize this header in the Buffer provided as a parameter
   *
   * \param start The Iterator to the Buffer where to read the header fields
   * \return The size of the deserialized header.
   */
  uint32_t Deserialize (Buffer::Iterator start) override;
  /**
   * Get the size of the header in serialized form
   *
   * \return The size of the header in serialized form.
   */
  uint32_t GetSerializedSize (void) const override;
  /**
   * Print a string version of the header in the ostream parameter provided.
   *
   * \param os The ostream where to print this header
   */
  void Print (std::ostream &os) const override;
  /**
   * Serialize this header in the buffer provided as a parameter
   *
   * \param start Iterator to the Buffer where to serialize this header
   */
  void Serialize (Buffer::Iterator start) const override;
  /**
   * Get the header size. This is a static method so we don't have to
   * hardcode the value in other classes that still don't have a full
   * object (e.g. to compute the size of a packet before building the packet)
   *
   * \return The size of the header
   */
  static uint32_t GetHeaderSize (void);

private:
  /**
   * Request type
   */
  Type m_requestType;
  /**
   * Number of embedded objects linked in the HTML document
   */
  uint16_t m_numEmbeddedObjects;
  /**
   * Total size of the payload in bytes
   */
  uint32_t m_payloadSize;
};

} // namespace psc
} // namespace ns3
#endif // INTEL_HTTP_HEADER_H

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

#include <ns3/simulator.h>
#include <ns3/log.h>

#include "intel-http-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("IntelHttpHeader");

namespace psc {

NS_OBJECT_ENSURE_REGISTERED (IntelHttpHeader);

IntelHttpHeader::IntelHttpHeader ()
{
  NS_LOG_FUNCTION (this);
  m_requestType = Type::Main;
  m_numEmbeddedObjects = 0;
}

IntelHttpHeader::~IntelHttpHeader ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
IntelHttpHeader::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::psc::IntelHttpHeader").SetParent<Header> ().AddConstructor<IntelHttpHeader> ();

  return tid;
}

TypeId
IntelHttpHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
IntelHttpHeader::SetRequestType (Type requestType)
{
  NS_LOG_FUNCTION (this << requestType);
  m_requestType = requestType;
}

IntelHttpHeader::Type
IntelHttpHeader::GetRequestType (void) const
{
  NS_LOG_FUNCTION (this);
  return m_requestType;
}

void
IntelHttpHeader::SetNumberEmbeddedObjects (uint16_t numEmbeddedObjects)
{
  NS_LOG_FUNCTION (this << numEmbeddedObjects);
  m_numEmbeddedObjects = numEmbeddedObjects;
}

uint16_t
IntelHttpHeader::GetNumberEmbeddedObjects (void) const
{
  NS_LOG_FUNCTION (this);
  return m_numEmbeddedObjects;
}

void
IntelHttpHeader::SetPayloadSize (uint32_t payloadSize)
{
  NS_LOG_FUNCTION (this << payloadSize);
  m_payloadSize = payloadSize;
}

uint32_t
IntelHttpHeader::GetPayloadSize (void) const
{
  NS_LOG_FUNCTION (this);
  return m_payloadSize;
}

uint32_t
IntelHttpHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION (this);

  Buffer::Iterator i = start;

  m_payloadSize = i.ReadNtohU32 ();

  uint16_t readVal = i.ReadNtohU16 ();
  if (readVal & 0x8000u)
    {
      m_requestType = Type::Main;
    }
  else
    {
      m_requestType = Type::Embedded;
    }
  m_numEmbeddedObjects = readVal & 0x0FFFu;

  return GetSerializedSize ();
}

uint32_t
IntelHttpHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION (this);

  return sizeof (uint16_t) + sizeof (uint32_t);
}

void
IntelHttpHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION (this);

  os << "Request Type: " << (m_requestType == Type::Main ? "HTML" : "EmbeddedObject")
     << " Payload Size: " << m_payloadSize
     << " Number of Embedded Objects: " << m_numEmbeddedObjects << std::endl;
}

void
IntelHttpHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION (this);

  Buffer::Iterator i = start;
  i.WriteHtonU32 (m_payloadSize);

  uint16_t typeAndObjects = 0u;
  if (m_requestType == Type::Main)
    {
      typeAndObjects = 0x8000u;
    }
  i.WriteHtonU16 (typeAndObjects + m_numEmbeddedObjects);
}

uint32_t
IntelHttpHeader::GetHeaderSize (void)
{
  return sizeof (uint16_t) + sizeof (uint32_t);
}

} // namespace psc
} // namespace ns3

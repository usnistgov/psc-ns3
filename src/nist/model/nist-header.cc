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

#include <ns3/simulator.h>
#include <ns3/log.h>

#include "nist-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistHeader");
NS_OBJECT_ENSURE_REGISTERED (NistBriefHeader);
NS_OBJECT_ENSURE_REGISTERED (NistHeader);

// Initialize static member 
uint64_t NistSequenceNumber::s_sequenceNumber = 0;

/// NistBriefHeader
NistBriefHeader::NistBriefHeader ()
{
  m_seqnum = 0;
  m_sentTime = Simulator::Now ();
}

NistBriefHeader::~NistBriefHeader ()
{
}

TypeId
NistBriefHeader::GetTypeId (void) 
{
  static TypeId tid = TypeId ("ns3::NistBriefHeader")
    .SetParent<Header> ()
    .AddConstructor<NistBriefHeader> ()
  ;
  return tid;
}

TypeId
NistBriefHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
NistBriefHeader::SetSeqNum (uint64_t seqnum)
{
  m_seqnum = seqnum;
}

uint64_t
NistBriefHeader::GetSeqNum (void)
{
  return m_seqnum;
}

void 
NistBriefHeader::SetSentTime (Time sentTime)
{
  m_sentTime = sentTime;
}

Time
NistBriefHeader::GetSentTime (void)
{
  return m_sentTime;
}

uint32_t 
NistBriefHeader::Deserialize (Buffer::Iterator start) 
{
  Buffer::Iterator i = start;
  m_seqnum = i.ReadNtohU64 ();
  m_sentTime = Time (i.ReadNtohU64 ());

  return GetSerializedSize ();
}

uint32_t
NistBriefHeader::GetSerializedSize (void) const
{
  return 
    sizeof (m_seqnum) + 
    sizeof (m_sentTime);
}

void 
NistBriefHeader::Print (std::ostream &os) const
{
  os << "SeqNum: " << m_seqnum << " TimeSent: " << m_sentTime.GetSeconds ();
}

void
NistBriefHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteHtonU32 (m_seqnum);
  i.WriteHtonU64 (m_sentTime.GetNanoSeconds () );
}


// Constructors
NistHeader::NistHeader ()
  : m_sequenceNumber (0),
    m_sourceId (0),
    m_destinationId (0),
    m_sourcePort (0),
    m_destinationPort (0),
    m_streamId (0),
    m_applicationId (0)
{
  m_sequenceNumber = NistSequenceNumber::GetSequenceNumber();
}
NistHeader::~NistHeader ()
{
}

// Setters
void 
NistHeader::SetParameters (
    uint32_t sourceId, 
    uint32_t destinationId, 
    uint16_t sourcePort, 
    uint16_t destinationPort, 
    uint32_t streamId, 
    uint16_t applicationId
    )
{
  SetSourceId(sourceId);
  SetDestinationId(destinationId);
  SetSourcePort(sourcePort);
  SetDestinationPort(destinationPort);
  SetStreamId(streamId);
  SetApplicationId(applicationId);
}

void
NistHeader:: SetSourceId (uint32_t id)
{
  m_sourceId = id;
}

void
NistHeader:: SetDestinationId (uint32_t id)
{
  m_destinationId = id;
}

void 
NistHeader::SetSourcePort (uint16_t port)
{
  m_sourcePort = port;
}

void 
NistHeader::SetDestinationPort (uint16_t port)
{
  m_destinationPort = port;
}

void
NistHeader::SetStreamId (uint32_t id)
{
  m_streamId = id;
}

void
NistHeader::SetApplicationId (uint16_t id)
{
  m_applicationId = id;
}

// Getters
uint64_t 
NistHeader::GetSequenceNumber (void) const
{
  return m_sequenceNumber;
}

uint32_t 
NistHeader::GetSourceId (void) const
{
  return m_sourceId;
}

uint32_t 
NistHeader::GetDestinationId (void) const
{
  return m_destinationId;
}

uint16_t 
NistHeader::GetSourcePort (void) const
{
  return m_sourcePort;
}

uint16_t 
NistHeader::GetDestinationPort (void) const
{
  return m_destinationPort;
}

uint32_t
NistHeader::GetStreamId (void) const
{
  return m_streamId;
}

uint16_t
NistHeader::GetApplicationId (void) const
{
  return m_applicationId;
}

// Other functions
TypeId 
NistHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NistHeader")
    .SetParent<Header> ()
    .AddConstructor<NistHeader> ()
  ;
  return tid;
}

TypeId 
NistHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void 
NistHeader::Print (std::ostream &os) const
{
  os << m_sequenceNumber << " " 
    << m_sourceId << " " 
    << m_destinationId << " "
    << m_streamId << " "
    << m_applicationId;
}

uint32_t 
NistHeader::GetSerializedSize (void) const
{
  return 
    sizeof (m_sequenceNumber) +
    sizeof (m_sourceId) +
    sizeof (m_destinationId) +
    sizeof (m_sourcePort) +
    sizeof (m_destinationPort) +
    sizeof (m_streamId) +
    sizeof (m_applicationId)
    ;
}

void
NistHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteHtonU64 (m_sequenceNumber);
  i.WriteHtonU32 (m_sourceId);
  i.WriteHtonU32 (m_destinationId);
  i.WriteHtonU16 (m_sourcePort);
  i.WriteHtonU16 (m_destinationPort);
  i.WriteHtonU32 (m_streamId);
  i.WriteHtonU16 (m_applicationId);
}

uint32_t
NistHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  
  m_sequenceNumber = i.ReadNtohU64 ();
  m_sourceId= i.ReadNtohU32 ();
  m_destinationId = i.ReadNtohU32 ();
  m_sourcePort = i.ReadNtohU16 ();
  m_destinationPort = i.ReadNtohU16 ();
  m_streamId = i.ReadNtohU32 ();
  m_applicationId = i.ReadNtohU16 ();

  return GetSerializedSize ();
}


} // namespace ns3


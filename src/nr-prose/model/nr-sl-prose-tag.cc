//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software
//

#include "nr-sl-prose-tag.h"

#include "nr-sl-pc5-signalling-header.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlProseTag");

NS_OBJECT_ENSURE_REGISTERED(NrSlProseAppTag);

TypeId
NrSlProseAppTag::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlProseAppTag").SetParent<Tag>().AddConstructor<NrSlProseAppTag>();
    return tid;
}

NrSlProseAppTag::NrSlProseAppTag()
{
}

TypeId
NrSlProseAppTag::GetInstanceTypeId() const
{
    return NrSlProseAppTag::GetTypeId();
}

void
NrSlProseAppTag::Deserialize(TagBuffer i)
{
}

uint32_t
NrSlProseAppTag::GetSerializedSize() const
{
    return 0;
}

void
NrSlProseAppTag::Print(std::ostream& os) const
{
    os << "ProseApp";
}

void
NrSlProseAppTag::Serialize(TagBuffer i) const
{
}

NS_OBJECT_ENSURE_REGISTERED(NrSlProseDiscoveryTag);

TypeId
NrSlProseDiscoveryTag::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlProseDiscoveryTag")
                            .SetParent<Tag>()
                            .AddConstructor<NrSlProseDiscoveryTag>();
    return tid;
}

NrSlProseDiscoveryTag::NrSlProseDiscoveryTag()
{
}

TypeId
NrSlProseDiscoveryTag::GetInstanceTypeId() const
{
    return NrSlProseDiscoveryTag::GetTypeId();
}

void
NrSlProseDiscoveryTag::Deserialize(TagBuffer i)
{
}

uint32_t
NrSlProseDiscoveryTag::GetSerializedSize() const
{
    return 0;
}

void
NrSlProseDiscoveryTag::Print(std::ostream& os) const
{
    os << "ProseDiscovery";
}

void
NrSlProseDiscoveryTag::Serialize(TagBuffer i) const
{
}

NS_OBJECT_ENSURE_REGISTERED(NrSlProsePc5SignallingTag);

TypeId
NrSlProsePc5SignallingTag::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlProsePc5SignallingTag")
                            .SetParent<Tag>()
                            .AddConstructor<NrSlProsePc5SignallingTag>();
    return tid;
}

NrSlProsePc5SignallingTag::NrSlProsePc5SignallingTag()
{
}

TypeId
NrSlProsePc5SignallingTag::GetInstanceTypeId() const
{
    return NrSlProsePc5SignallingTag::GetTypeId();
}

void
NrSlProsePc5SignallingTag::Deserialize(TagBuffer i)
{
    m_type = i.ReadU8();
}

uint32_t
NrSlProsePc5SignallingTag::GetSerializedSize() const
{
    return 1;
}

void
NrSlProsePc5SignallingTag::Print(std::ostream& os) const
{
    switch (m_type)
    {
    case NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentRequest:
        os << "ProseDirectLinkEstablishmentRequest";
        break;
    case NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentAccept:
        os << "ProseDirectLinkEstablishmentAccept";
        break;
    case NrSlPc5SignallingMessageType::ProseDirectLinkEstablishmentReject:
        os << "ProseDirectLinkEstablishmentReject";
        break;
    case NrSlPc5SignallingMessageType::ProseDirectLinkModificationRequest:
        os << "ProseDirectLinkModificationRequest";
        break;
    case NrSlPc5SignallingMessageType::ProseDirectLinkModificationAccept:
        os << "ProseDirectLinkModificationAccept";
        break;
    case NrSlPc5SignallingMessageType::ProseDirectLinkModificationReject:
        os << "ProseDirectLinkModificationReject";
        break;
    case NrSlPc5SignallingMessageType::ProseDirectLinkReleaseRequest:
        os << "ProseDirectLinkReleaseRequest";
        break;
    case NrSlPc5SignallingMessageType::ProseDirectLinkReleaseAccept:
        os << "ProseDirectLinkReleaseAccept";
        break;
    case NrSlPc5SignallingMessageType::ProseDirectLinkKeepaliveRequest:
        os << "ProseDirectLinkKeepaliveRequest";
        break;
    case NrSlPc5SignallingMessageType::ProseDirectLinkKeepaliveResponse:
        os << "ProseDirectLinkKeepaliveResponse";
        break;
    default:
        NS_ABORT_MSG("Uknown PC5 signalling message type");
    }
}

void
NrSlProsePc5SignallingTag::Serialize(TagBuffer i) const
{
    i.WriteU8(m_type);
}

uint8_t
NrSlProsePc5SignallingTag::GetType() const
{
    return m_type;
}

void
NrSlProsePc5SignallingTag::SetType(uint8_t type)
{
    m_type = type;
}

} // namespace ns3

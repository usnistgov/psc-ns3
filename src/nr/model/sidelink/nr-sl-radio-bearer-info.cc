// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

#include "nr-sl-radio-bearer-info.h"

#include "nr-sl-pdcp.h"

#include "ns3/nr-rlc.h"
#include <ns3/log.h>
#include <ns3/object.h>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrSlDataRadioBearerInfo);

TypeId
NrSlDataRadioBearerInfo::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSlDataRadioBearerInfo")
            .SetParent<NrRadioBearerInfo>()
            .AddConstructor<NrSlDataRadioBearerInfo>()
            .AddAttribute("DestinationL2Id",
                          "The destination identifier for the communication",
                          TypeId::ATTR_GET, // allow only getting it.
                          UintegerValue(0), // unused (attribute is read-only
                          MakeUintegerAccessor(&NrSlDataRadioBearerInfo::m_destinationL2Id),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("logicalChannelIdentity",
                          "The id of the Logical Channel corresponding to this Data Radio Bearer",
                          TypeId::ATTR_GET, // allow only getting it.
                          UintegerValue(0), // unused (attribute is read-only
                          MakeUintegerAccessor(&NrSlDataRadioBearerInfo::m_logicalChannelIdentity),
                          MakeUintegerChecker<uint8_t>())
            .AddAttribute("NrRlc",
                          "RLC instance of the radio bearer.",
                          PointerValue(),
                          MakePointerAccessor(&NrRadioBearerInfo::m_rlc),
                          MakePointerChecker<NrRlc>())
            .AddAttribute("NrPdcp",
                          "Sidelink PDCP instance of the radio bearer.",
                          PointerValue(),
                          MakePointerAccessor(&NrRadioBearerInfo::m_pdcp),
                          MakePointerChecker<NrSlPdcp>());
    return tid;
}

NS_OBJECT_ENSURE_REGISTERED(NrSlSignallingRadioBearerInfo);

TypeId
NrSlSignallingRadioBearerInfo::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::NrSlSignallingRadioBearerInfo")
            .SetParent<NrRadioBearerInfo>()
            .AddConstructor<NrSlSignallingRadioBearerInfo>()
            .AddAttribute("DestinationL2Id",
                          "The destination identifier for the communication",
                          TypeId::ATTR_GET, // allow only getting it.
                          UintegerValue(0), // unused (attribute is read-only)
                          MakeUintegerAccessor(&NrSlSignallingRadioBearerInfo::m_destinationL2Id),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute(
                "logicalChannelIdentity",
                "The id of the Logical Channel corresponding to this Data Radio Bearer",
                TypeId::ATTR_GET, // allow only getting it.
                UintegerValue(0), // unused (attribute is read-only)
                MakeUintegerAccessor(&NrSlSignallingRadioBearerInfo::m_logicalChannelIdentity),
                MakeUintegerChecker<uint8_t>())
            .AddAttribute("NrRlc",
                          "RLC instance of the radio bearer.",
                          PointerValue(),
                          MakePointerAccessor(&NrRadioBearerInfo::m_rlc),
                          MakePointerChecker<NrRlc>())
            .AddAttribute("NrPdcp",
                          "Sidelink PDCP instance of the radio bearer.",
                          PointerValue(),
                          MakePointerAccessor(&NrRadioBearerInfo::m_pdcp),
                          MakePointerChecker<NrSlPdcp>());
    return tid;
}

NS_OBJECT_ENSURE_REGISTERED(NrSlDiscoveryRadioBearerInfo);

TypeId
NrSlDiscoveryRadioBearerInfo::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::NrSlDiscoveryRadioBearerInfo")
            .SetParent<NrRadioBearerInfo>()
            .AddConstructor<NrSlDiscoveryRadioBearerInfo>()
            .AddAttribute("DestinationL2Id",
                          "The destination identifier for the discovery",
                          TypeId::ATTR_GET, // allow only getting it.
                          UintegerValue(0), // unused (attribute is read-only)
                          MakeUintegerAccessor(&NrSlDiscoveryRadioBearerInfo::m_destinationL2Id),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute(
                "logicalChannelIdentity",
                "The id of the Logical Channel corresponding to this discovery Radio Bearer",
                TypeId::ATTR_GET, // allow only getting it.
                UintegerValue(0), // unused (attribute is read-only)
                MakeUintegerAccessor(&NrSlDiscoveryRadioBearerInfo::m_logicalChannelIdentity),
                MakeUintegerChecker<uint8_t>())
            .AddAttribute("NrRlc",
                          "RLC instance of the radio bearer.",
                          PointerValue(),
                          MakePointerAccessor(&NrRadioBearerInfo::m_rlc),
                          MakePointerChecker<NrRlc>())
            .AddAttribute("NrPdcp",
                          "Sidelink PDCP instance of the radio bearer.",
                          PointerValue(),
                          MakePointerAccessor(&NrRadioBearerInfo::m_pdcp),
                          MakePointerChecker<NrSlPdcp>());
    return tid;
}

} // namespace ns3

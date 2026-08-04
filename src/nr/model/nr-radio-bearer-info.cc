// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.es>

#include "nr-radio-bearer-info.h"

#include "nr-pdcp.h"
#include "nr-rlc.h"

#include <ns3/log.h>

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED(NrRadioBearerInfo);

NrRadioBearerInfo::NrRadioBearerInfo()
{
}

NrRadioBearerInfo::~NrRadioBearerInfo()
{
}

TypeId
NrRadioBearerInfo::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrRadioBearerInfo").SetParent<Object>().AddConstructor<NrRadioBearerInfo>();
    return tid;
}

TypeId
NrDataRadioBearerInfo::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrDataRadioBearerInfo")
            .SetParent<NrRadioBearerInfo>()
            .AddConstructor<NrDataRadioBearerInfo>()
            .AddAttribute("DrbIdentity",
                          "The id of this Data Radio Bearer",
                          TypeId::ATTR_GET, // allow only getting it.
                          UintegerValue(0), // unused (attribute is read-only
                          MakeUintegerAccessor(&NrDataRadioBearerInfo::m_drbIdentity),
                          MakeUintegerChecker<uint8_t>())
            .AddAttribute("EpsBearerIdentity",
                          "The id of the EPS bearer corresponding to this Data Radio Bearer",
                          TypeId::ATTR_GET, // allow only getting it.
                          UintegerValue(0), // unused (attribute is read-only
                          MakeUintegerAccessor(&NrDataRadioBearerInfo::m_epsBearerIdentity),
                          MakeUintegerChecker<uint8_t>())
            .AddAttribute("logicalChannelIdentity",
                          "The id of the Logical Channel corresponding to this Data Radio Bearer",
                          TypeId::ATTR_GET, // allow only getting it.
                          UintegerValue(0), // unused (attribute is read-only
                          MakeUintegerAccessor(&NrDataRadioBearerInfo::m_logicalChannelIdentity),
                          MakeUintegerChecker<uint8_t>())
            .AddAttribute("NrRlc",
                          "RLC instance of the radio bearer.",
                          PointerValue(),
                          MakePointerAccessor(&NrRadioBearerInfo::m_rlc),
                          MakePointerChecker<NrRlc>())
            .AddAttribute("NrPdcp",
                          "PDCP instance of the radio bearer.",
                          PointerValue(),
                          MakePointerAccessor(&NrRadioBearerInfo::m_pdcp),
                          MakePointerChecker<NrPdcp>());
    return tid;
}

TypeId
NrSignalingRadioBearerInfo::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSignalingRadioBearerInfo")
            .SetParent<NrRadioBearerInfo>()
            .AddConstructor<NrSignalingRadioBearerInfo>()
            .AddAttribute("SrbIdentity",
                          "The id of this Signaling Radio Bearer",
                          TypeId::ATTR_GET, // allow only getting it.
                          UintegerValue(0), // unused (attribute is read-only
                          MakeUintegerAccessor(&NrSignalingRadioBearerInfo::m_srbIdentity),
                          MakeUintegerChecker<uint8_t>())
            .AddAttribute("NrRlc",
                          "RLC instance of the radio bearer.",
                          PointerValue(),
                          MakePointerAccessor(&NrRadioBearerInfo::m_rlc),
                          MakePointerChecker<NrRlc>())
            .AddAttribute("NrPdcp",
                          "PDCP instance of the radio bearer.",
                          PointerValue(),
                          MakePointerAccessor(&NrRadioBearerInfo::m_pdcp),
                          MakePointerChecker<NrPdcp>());
    return tid;
}

} // namespace ns3

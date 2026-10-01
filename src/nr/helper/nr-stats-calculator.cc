// Copyright (c) 2021 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-stats-calculator.h"

#include <ns3/config.h>
#include <ns3/log.h>
#include <ns3/nr-gnb-net-device.h>
#include <ns3/nr-gnb-rrc.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-ue-rrc.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrStatsCalculator");

NS_OBJECT_ENSURE_REGISTERED(NrStatsCalculator);

NrStatsCalculator::NrStatsCalculator()
    : m_dlOutputFilename(""),
      m_ulOutputFilename("")
{
    // Nothing to do here
}

NrStatsCalculator::~NrStatsCalculator()
{
    // Nothing to do here
}

TypeId
NrStatsCalculator::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrStatsCalculator")
                            .SetParent<Object>()
                            .SetGroupName("nr")
                            .AddConstructor<NrStatsCalculator>();
    return tid;
}

void
NrStatsCalculator::SetUlOutputFilename(std::string outputFilename)
{
    m_ulOutputFilename = outputFilename;
}

std::string
NrStatsCalculator::GetUlOutputFilename()
{
    return m_ulOutputFilename;
}

void
NrStatsCalculator::SetDlOutputFilename(std::string outputFilename)
{
    m_dlOutputFilename = outputFilename;
}

std::string
NrStatsCalculator::GetDlOutputFilename()
{
    return m_dlOutputFilename;
}

bool
NrStatsCalculator::ExistsImsiPath(std::string path)
{
    return m_pathImsiMap.find(path) != m_pathImsiMap.end();
}

void
NrStatsCalculator::SetImsiPath(std::string path, uint64_t imsi)
{
    NS_LOG_FUNCTION(this << path << imsi);
    m_pathImsiMap[path] = imsi;
}

uint64_t
NrStatsCalculator::GetImsiPath(std::string path)
{
    return m_pathImsiMap.find(path)->second;
}

bool
NrStatsCalculator::ExistsCellIdPath(std::string path)
{
    return m_pathCellIdMap.find(path) != m_pathCellIdMap.end();
}

void
NrStatsCalculator::SetCellIdPath(std::string path, uint16_t cellId)
{
    NS_LOG_FUNCTION(this << path << cellId);
    m_pathCellIdMap[path] = cellId;
}

uint16_t
NrStatsCalculator::GetCellIdPath(std::string path)
{
    return m_pathCellIdMap.find(path)->second;
}

uint64_t
NrStatsCalculator::FindImsiFromGnbRlcPath(std::string path)
{
    NS_LOG_FUNCTION(path);
    // Sample path input:
    // /NodeList/#NodeId/DeviceList/#DeviceId/NrGnbRrc/UeMap/#C-RNTI/DataRadioBearerMap/#LCID/NrRlc/RxPDU

    // We retrieve the NrUeManager associated to the C-RNTI and perform the IMSI lookup
    std::string ueMapPath = path.substr(0, path.find("/DataRadioBearerMap"));
    Config::MatchContainer match = Config::LookupMatches(ueMapPath);

    if (match.GetN() != 0)
    {
        Ptr<Object> ueInfo = match.Get(0);
        NS_LOG_LOGIC("FindImsiFromGnbRlcPath: " << path << ", "
                                                << ueInfo->GetObject<NrUeManager>()->GetImsi());
        return ueInfo->GetObject<NrUeManager>()->GetImsi();
    }
    else
    {
        NS_FATAL_ERROR("Lookup " << ueMapPath << " got no matches");
        return 0; // Silence compiler warning
    }
}

uint64_t
NrStatsCalculator::FindImsiFromNrUeNetDevice(std::string path)
{
    NS_LOG_FUNCTION(path);
    // Sample path input:
    // /NodeList/#NodeId/DeviceList/#DeviceId/

    // We retrieve the Imsi associated to the NrUeNetDevice
    Config::MatchContainer match = Config::LookupMatches(path);

    if (match.GetN() != 0)
    {
        Ptr<Object> ueNetDevice = match.Get(0);
        NS_LOG_LOGIC("FindImsiFromNrUeNetDevice: "
                     << path << ", " << ueNetDevice->GetObject<NrUeNetDevice>()->GetImsi());
        return ueNetDevice->GetObject<NrUeNetDevice>()->GetImsi();
    }
    else
    {
        NS_FATAL_ERROR("Lookup " << path << " got no matches");
        return 0; // Silence compiler warning
    }
}

uint16_t
NrStatsCalculator::FindCellIdFromGnbRlcPath(std::string path)
{
    NS_LOG_FUNCTION(path);
    // Sample path input:
    // /NodeList/#NodeId/DeviceList/#DeviceId/NrGnbRrc/UeMap/#C-RNTI/DataRadioBearerMap/#LCID/NrRlc/RxPDU

    // We retrieve the CellId associated to the gNB
    std::string gnbNetDevicePath = path.substr(0, path.find("/NrGnbRrc"));
    Config::MatchContainer match = Config::LookupMatches(gnbNetDevicePath);
    if (match.GetN() != 0)
    {
        Ptr<Object> gnbNetDevice = match.Get(0);
        NS_LOG_LOGIC("FindCellIdFromGnbRlcPath: "
                     << path << ", " << gnbNetDevice->GetObject<NrGnbNetDevice>()->GetCellId());
        return gnbNetDevice->GetObject<NrGnbNetDevice>()->GetCellId();
    }
    else
    {
        NS_FATAL_ERROR("Lookup " << gnbNetDevicePath << " got no matches");
        return 0; // Silence compiler warning
    }
}

uint64_t
NrStatsCalculator::FindImsiFromGnbMac(std::string path, uint16_t rnti)
{
    NS_LOG_FUNCTION(path << rnti);

    // /NodeList/#NodeId/DeviceList/#DeviceId/BandwidthPartMap/#BwpId/NrGnbMac/DlScheduling
    std::ostringstream oss;
    std::string p = path.substr(0, path.find("/BandwidthPartMap"));
    oss << rnti;
    p += "/NrGnbRrc/UeMap/" + oss.str();
    uint64_t imsi = FindImsiFromGnbRlcPath(p);
    NS_LOG_LOGIC("FindImsiFromGnbMac: " << path << ", " << rnti << ", " << imsi);
    return imsi;
}

uint16_t
NrStatsCalculator::FindCellIdFromGnbMac(std::string path, uint16_t rnti)
{
    NS_LOG_FUNCTION(path << rnti);
    // /NodeList/#NodeId/DeviceList/#DeviceId/BandwidthPartMap/#BwpId/NrGnbMac/DlScheduling
    std::ostringstream oss;
    std::string p = path.substr(0, path.find("/BandwidthPartMap"));
    oss << rnti;
    p += "/NrGnbRrc/UeMap/" + oss.str();
    uint16_t cellId = FindCellIdFromGnbRlcPath(p);
    NS_LOG_LOGIC("FindCellIdFromGnbMac: " << path << ", " << rnti << ", " << cellId);
    return cellId;
}

} // namespace ns3

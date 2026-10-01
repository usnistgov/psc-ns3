// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-sl-comm-resource-pool-factory.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlCommResourcePoolFactory");

NrSlCommResourcePoolFactory::NrSlCommResourcePoolFactory()
{
    NS_LOG_FUNCTION(this);
    m_setupReleasePscch = "SETUP";
    m_slTimeResourcePscch = 1;
    m_slFreqResourcePscch = 10;
    m_slSubchannelSize = 50;
    m_slSensingWindow = 100;
    m_slSelectionWindow = 5;
    m_slResourceReservePeriodList = {0};
    m_slTimeResource = {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1};
    m_slMaxNumPerReserve = 2;
    m_slPsfchPeriod = 0; // PSFCH disabled by default
    m_slMinTimeGapPsfch = 3;
}

NrSlCommResourcePoolFactory::~NrSlCommResourcePoolFactory()
{
    NS_LOG_FUNCTION(this);
}

const NrSlRrcSap::SlResourcePoolNr
NrSlCommResourcePoolFactory::CreatePool()
{
    if (m_setupReleasePscch == "SETUP")
    {
        m_pool.slPscchConfig.setupRelease = NrSlRrcSap::SlPscchConfig::SETUP;
    }
    else if (m_setupReleasePscch == "RELEASE")
    {
        NS_FATAL_ERROR("Releasing of PSCCH resources is not supported");
    }
    else
    {
        NS_FATAL_ERROR("Invalid setuprelease option : " << m_setupReleasePscch
                                                        << " for SlPscchConfig");
    }

    switch (m_slTimeResourcePscch)
    {
    case 1:
        m_pool.slPscchConfig.slTimeResourcePscch.resources = NrSlRrcSap::SlTimeResourcePscch::N1;
        break;
    case 2:
        m_pool.slPscchConfig.slTimeResourcePscch.resources = NrSlRrcSap::SlTimeResourcePscch::N2;
        break;
    case 3:
        m_pool.slPscchConfig.slTimeResourcePscch.resources = NrSlRrcSap::SlTimeResourcePscch::N3;
        break;
    default:
        NS_FATAL_ERROR("Invalid number of symbols : " << m_slTimeResourcePscch
                                                      << " chosen for SL PSCCH");
    }

    switch (m_slFreqResourcePscch)
    {
    case 10:
        m_pool.slPscchConfig.slFreqResourcePscch.resources = NrSlRrcSap::SlFreqResourcePscch::N10;
        break;
    case 12:
        m_pool.slPscchConfig.slFreqResourcePscch.resources = NrSlRrcSap::SlFreqResourcePscch::N12;
        break;
    case 15:
        m_pool.slPscchConfig.slFreqResourcePscch.resources = NrSlRrcSap::SlFreqResourcePscch::N15;
        break;
    case 20:
        m_pool.slPscchConfig.slFreqResourcePscch.resources = NrSlRrcSap::SlFreqResourcePscch::N20;
        break;
    case 25:
        m_pool.slPscchConfig.slFreqResourcePscch.resources = NrSlRrcSap::SlFreqResourcePscch::N25;
        break;
    default:
        NS_FATAL_ERROR("Invalid number of RBs : " << m_slFreqResourcePscch
                                                  << " chosen for SL PSCCH");
    }

    switch (m_slSubchannelSize)
    {
    case 10:
        m_pool.slSubchannelSize.numPrbs = NrSlRrcSap::SlSubchannelSize::N10;
        break;
    case 15:
        m_pool.slSubchannelSize.numPrbs = NrSlRrcSap::SlSubchannelSize::N15;
        break;
    case 20:
        m_pool.slSubchannelSize.numPrbs = NrSlRrcSap::SlSubchannelSize::N20;
        break;
    case 25:
        m_pool.slSubchannelSize.numPrbs = NrSlRrcSap::SlSubchannelSize::N25;
        break;
    case 50:
        m_pool.slSubchannelSize.numPrbs = NrSlRrcSap::SlSubchannelSize::N50;
        break;
    case 75:
        m_pool.slSubchannelSize.numPrbs = NrSlRrcSap::SlSubchannelSize::N75;
        break;
    case 100:
        m_pool.slSubchannelSize.numPrbs = NrSlRrcSap::SlSubchannelSize::N100;
        break;
    default:
        NS_FATAL_ERROR("Invalid subchannel size in RBs : " << m_slSubchannelSize);
    }

    switch (m_slSensingWindow)
    {
    case 100:
        m_pool.slUeSelectedConfigRp.slSensingWindow.windSize = NrSlRrcSap::SlSensingWindow::MS100;
        break;
    case 1100:
        m_pool.slUeSelectedConfigRp.slSensingWindow.windSize = NrSlRrcSap::SlSensingWindow::MS1100;
        break;
    default:
        NS_FATAL_ERROR("Invalid sensing window size : " << m_slSensingWindow);
    }

    switch (m_slSelectionWindow)
    {
    case 1:
        m_pool.slUeSelectedConfigRp.slSelectionWindow.windSize = NrSlRrcSap::SlSelectionWindow::N1;
        break;
    case 5:
        m_pool.slUeSelectedConfigRp.slSelectionWindow.windSize = NrSlRrcSap::SlSelectionWindow::N5;
        break;
    case 10:
        m_pool.slUeSelectedConfigRp.slSelectionWindow.windSize = NrSlRrcSap::SlSelectionWindow::N10;
        break;
    case 20:
        m_pool.slUeSelectedConfigRp.slSelectionWindow.windSize = NrSlRrcSap::SlSelectionWindow::N20;
        break;
    default:
        NS_FATAL_ERROR("Invalid selection window size : " << m_slSelectionWindow);
    }

    NrSlRrcSap::SlResourceReservePeriod slReserved;
    for (const auto i : m_slResourceReservePeriodList)
    {
        switch (i)
        {
        case 0:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS0;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 10:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS10;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 20:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS20;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 30:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS30;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 40:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS40;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 50:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS50;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 60:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS60;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 70:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS70;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 80:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS80;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 90:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS90;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 100:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS100;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 150:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS150;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 200:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS200;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 250:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS250;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 300:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS300;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 350:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS350;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 400:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS400;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 450:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS450;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 500:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS500;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 550:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS550;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 600:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS600;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 650:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS650;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 700:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS700;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 750:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS750;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 800:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS800;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 850:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS850;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 900:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS900;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 950:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS950;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        case 1000:
            slReserved.period = NrSlRrcSap::SlResourceReservePeriod::MS1000;
            m_pool.slUeSelectedConfigRp.slResourceReservePeriodList.push_back(slReserved);
            break;
        default:
            NS_FATAL_ERROR("Invalid sidelink reservation period : " << i << " used");
        }
    }

    switch (m_slMaxNumPerReserve)
    {
    case 1:
        m_pool.slUeSelectedConfigRp.slMaxNumPerReserve.maxNumPerRes =
            NrSlRrcSap::SlMaxNumPerReserve::N1;
        break;
    case 2:
        m_pool.slUeSelectedConfigRp.slMaxNumPerReserve.maxNumPerRes =
            NrSlRrcSap::SlMaxNumPerReserve::N2;
        break;
    case 3:
        m_pool.slUeSelectedConfigRp.slMaxNumPerReserve.maxNumPerRes =
            NrSlRrcSap::SlMaxNumPerReserve::N3;
        break;
    default:
        NS_FATAL_ERROR("Invalid sidelink value " << m_slMaxNumPerReserve
                                                 << " used for number SlMaxNumPerReserve");
    }

    m_pool.slTimeResource = m_slTimeResource;

    switch (m_slPsfchPeriod)
    {
    case 0:
        m_pool.slPsfchConfig.slPsfchPeriod.period = NrSlRrcSap::SlPsfchPeriod::SL0;
        break;
    case 1:
        m_pool.slPsfchConfig.slPsfchPeriod.period = NrSlRrcSap::SlPsfchPeriod::SL1;
        break;
    case 2:
        m_pool.slPsfchConfig.slPsfchPeriod.period = NrSlRrcSap::SlPsfchPeriod::SL2;
        break;
    case 4:
        m_pool.slPsfchConfig.slPsfchPeriod.period = NrSlRrcSap::SlPsfchPeriod::SL4;
        break;
    default:
        NS_FATAL_ERROR("Invalid PSFCH period value " << m_slPsfchPeriod);
    }

    switch (m_slMinTimeGapPsfch)
    {
    case 2:
        m_pool.slPsfchConfig.slMinTimeGapPsfch.gap = NrSlRrcSap::SlMinTimeGapPsfch::SL2;
        break;
    case 3:
        m_pool.slPsfchConfig.slMinTimeGapPsfch.gap = NrSlRrcSap::SlMinTimeGapPsfch::SL3;
        break;
    default:
        if (m_pool.slPsfchConfig.slPsfchPeriod.period != NrSlRrcSap::SlPsfchPeriod::SL0)
        {
            NS_FATAL_ERROR("Invalid MinTimeGapPSFCH value " << m_slMinTimeGapPsfch);
        }
    }

    return m_pool;
}

bool
NrSlCommResourcePoolFactory::IsSetupPscchResources() const
{
    bool isSetup = false;
    if (m_setupReleasePscch == "SETUP")
    {
        isSetup = true;
    }

    return isSetup;
}

void
NrSlCommResourcePoolFactory::SetupPscchResources()
{
    m_setupReleasePscch = std::to_string(NrSlRrcSap::SlPscchConfig::SETUP);
}

uint16_t
NrSlCommResourcePoolFactory::GetSlFreqResourcePscch() const
{
    return m_slFreqResourcePscch;
}

uint16_t
NrSlCommResourcePoolFactory::GetSlTimeResourcePscch() const
{
    return m_slTimeResourcePscch;
}

void
NrSlCommResourcePoolFactory::SetSlTimeResourcePscch(uint16_t slTimeResourcePscch)
{
    m_slTimeResourcePscch = slTimeResourcePscch;
}

void
NrSlCommResourcePoolFactory::SetSlFreqResourcePscch(uint16_t slFreqResourcePscch)
{
    m_slFreqResourcePscch = slFreqResourcePscch;
}

uint16_t
NrSlCommResourcePoolFactory::GetSlSubchannelSize() const
{
    return m_slSubchannelSize;
}

void
NrSlCommResourcePoolFactory::SetSlSubchannelSize(uint16_t slSubchannelSize)
{
    m_slSubchannelSize = slSubchannelSize;
}

uint16_t
NrSlCommResourcePoolFactory::GetSlSensingWindow() const
{
    return m_slSensingWindow;
}

void
NrSlCommResourcePoolFactory::SetSlSensingWindow(uint16_t slSensingWindow)
{
    m_slSensingWindow = slSensingWindow;
}

uint16_t
NrSlCommResourcePoolFactory::GetSlSelectionWindow() const
{
    return m_slSelectionWindow;
}

void
NrSlCommResourcePoolFactory::SetSlSelectionWindow(uint16_t slSelectionWindow)
{
    m_slSelectionWindow = slSelectionWindow;
}

const std::list<uint16_t>&
NrSlCommResourcePoolFactory::GetSlResourceReservePeriodList() const
{
    return m_slResourceReservePeriodList;
}

void
NrSlCommResourcePoolFactory::SetSlResourceReservePeriodList(
    std::list<uint16_t>& slResourceReservePeriodList)
{
    m_slResourceReservePeriodList = slResourceReservePeriodList;
}

const std::vector<std::bitset<1>>&
NrSlCommResourcePoolFactory::GetSlTimeResources() const
{
    return m_slTimeResource;
}

void
NrSlCommResourcePoolFactory::SetSlTimeResources(std::vector<std::bitset<1>>& slBitmap)
{
    m_slTimeResource = slBitmap;
}

uint16_t
NrSlCommResourcePoolFactory::GetSlMaxNumPerReserve() const
{
    return m_slMaxNumPerReserve;
}

void
NrSlCommResourcePoolFactory::SetSlMaxNumPerReserve(uint16_t maxNumPerReserve)
{
    m_slMaxNumPerReserve = maxNumPerReserve;
}

uint16_t
NrSlCommResourcePoolFactory::GetSlPsfchPeriod() const
{
    return m_slPsfchPeriod;
}

void
NrSlCommResourcePoolFactory::SetSlPsfchPeriod(uint16_t psfchPeriod)
{
    m_slPsfchPeriod = psfchPeriod;
}

uint16_t
NrSlCommResourcePoolFactory::GetSlMinTimeGapPsfch() const
{
    return m_slMinTimeGapPsfch;
}

void
NrSlCommResourcePoolFactory::SetSlMinTimeGapPsfch(uint16_t minTimeGap)
{
    m_slMinTimeGapPsfch = minTimeGap;
}

} // namespace ns3

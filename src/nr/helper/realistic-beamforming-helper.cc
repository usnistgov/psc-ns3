// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "realistic-beamforming-helper.h"

#include <ns3/log.h>
#include <ns3/nr-gnb-net-device.h>
#include <ns3/nr-gnb-phy.h>
#include <ns3/nr-spectrum-phy.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-ue-phy.h>
#include <ns3/nr-ue-rrc.h>
#include <ns3/vector.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("RealisticBeamformingHelper");
NS_OBJECT_ENSURE_REGISTERED(RealisticBeamformingHelper);

TypeId
RealisticBeamformingHelper::GetTypeId()
{
    static TypeId tid = TypeId("ns3::RealisticBeamformingHelper")
                            .SetParent<BeamformingHelperBase>()
                            .AddConstructor<RealisticBeamformingHelper>();
    return tid;
}

void
RealisticBeamformingHelper::DoDispose()
{
    for (auto& it : m_spectrumPhyPairToAlgorithm)
    {
        it.second->Dispose();
    }
    m_spectrumPhyPairToAlgorithm.clear();
}

void
RealisticBeamformingHelper::AddBeamformingTask(const Ptr<NrGnbNetDevice>& gNbDev,
                                               const Ptr<NrUeNetDevice>& ueDev)
{
    NS_LOG_FUNCTION(this);
    for (std::size_t ccId = 0; ccId < gNbDev->GetCcMapSize(); ccId++)
    {
        Ptr<NrSpectrumPhy> gnbSpectrumPhy = gNbDev->GetPhy(ccId)->GetSpectrumPhy();
        Ptr<NrSpectrumPhy> ueSpectrumPhy = ueDev->GetPhy(ccId)->GetSpectrumPhy();

        auto itAlgorithms =
            m_spectrumPhyPairToAlgorithm.find(std::make_pair(gnbSpectrumPhy, ueSpectrumPhy));
        NS_ABORT_MSG_IF(itAlgorithms != m_spectrumPhyPairToAlgorithm.end(),
                        "Realistic beamforming task already created for the provided devices");

        // for each pair of antenna arrays of transmitter and receiver create an instance of
        // beamforming algorithm
        Ptr<RealisticBeamformingAlgorithm> beamformingAlgorithm =
            m_algorithmFactory.Create<RealisticBeamformingAlgorithm>();

        beamformingAlgorithm->Install(gnbSpectrumPhy, ueSpectrumPhy, gNbDev->GetScheduler(ccId));

        m_spectrumPhyPairToAlgorithm[std::make_pair(gnbSpectrumPhy, ueSpectrumPhy)] =
            beamformingAlgorithm;
        // connect trace of the corresponding gNB PHY to the RealisticBeamformingAlgorithm
        // function
        gnbSpectrumPhy->AddSrsSinrReportCallback(
            MakeCallback(&RealisticBeamformingAlgorithm::NotifySrsSinrReport,
                         beamformingAlgorithm));
        gnbSpectrumPhy->AddSrsSnrReportCallback(
            MakeCallback(&RealisticBeamformingAlgorithm::NotifySrsSnrReport, beamformingAlgorithm));
        beamformingAlgorithm->SetTriggerCallback(
            MakeCallback(&RealisticBeamformingHelper::RunTask, this));
    }
}

BeamformingVectorPair
RealisticBeamformingHelper::GetBeamformingVectors(const Ptr<NrSpectrumPhy>& gnbSpectrumPhy,
                                                  const Ptr<NrSpectrumPhy>& ueSpectrumPhy) const
{
    auto itAlgo = m_spectrumPhyPairToAlgorithm.find(std::make_pair(gnbSpectrumPhy, ueSpectrumPhy));
    NS_ABORT_MSG_IF(itAlgo == m_spectrumPhyPairToAlgorithm.end(),
                    "There is no created task/algorithm for the specified pair of antenna arrays.");
    return itAlgo->second->GetBeamformingVectors();
}

void
RealisticBeamformingHelper::SetBeamformingMethod(const TypeId& beamformingMethod)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(beamformingMethod == RealisticBeamformingAlgorithm::GetTypeId() ||
              beamformingMethod.IsChildOf(RealisticBeamformingAlgorithm::GetTypeId()));

    m_algorithmFactory.SetTypeId(beamformingMethod);
}

} // namespace ns3

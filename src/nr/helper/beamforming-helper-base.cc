// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "beamforming-helper-base.h"

#include <ns3/log.h>
#include <ns3/node.h>
#include <ns3/nr-gnb-net-device.h>
#include <ns3/nr-gnb-phy.h>
#include <ns3/nr-spectrum-phy.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-ue-phy.h>
#include <ns3/vector.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("BeamformingHelperBase");
NS_OBJECT_ENSURE_REGISTERED(BeamformingHelperBase);

BeamformingHelperBase::BeamformingHelperBase()
{
    // TODO Auto-generated constructor stub
    NS_LOG_FUNCTION(this);
}

BeamformingHelperBase::~BeamformingHelperBase()
{
    // TODO Auto-generated destructor stub
    NS_LOG_FUNCTION(this);
}

TypeId
BeamformingHelperBase::GetTypeId()
{
    static TypeId tid = TypeId("ns3::BeamformingHelperBase").SetParent<Object>();
    return tid;
}

void
BeamformingHelperBase::RunTask(const Ptr<NrSpectrumPhy>& gnbSpectrumPhy,
                               const Ptr<NrSpectrumPhy>& ueSpectrumPhy) const
{
    NS_LOG_FUNCTION(this);
    NS_LOG_INFO(" Run beamforming task for gNB node Id:"
                << gnbSpectrumPhy->GetDevice()->GetNode()->GetId()
                << " and UE node Id:" << ueSpectrumPhy->GetDevice()->GetNode()->GetId());
    BeamformingVectorPair bfPair = GetBeamformingVectors(gnbSpectrumPhy, ueSpectrumPhy);

    NS_ASSERT(bfPair.first.first.GetSize() && bfPair.second.first.GetSize());
    gnbSpectrumPhy->GetBeamManager()->SaveBeamformingVector(bfPair.first,
                                                            ueSpectrumPhy->GetDevice());
    ueSpectrumPhy->GetBeamManager()->SaveBeamformingVector(bfPair.second,
                                                           gnbSpectrumPhy->GetDevice());
    ueSpectrumPhy->GetBeamManager()->ChangeBeamformingVector(gnbSpectrumPhy->GetDevice());
}

void
BeamformingHelperBase::SetBeamformingAlgorithmAttribute(const std::string& n,
                                                        const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_algorithmFactory.Set(n, v);
}

} // namespace ns3

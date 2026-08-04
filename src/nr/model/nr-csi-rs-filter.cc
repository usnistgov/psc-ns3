// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-csi-rs-filter.h"

#include "nr-spectrum-phy.h"
#include "nr-ue-net-device.h"

#include <ns3/boolean.h>
#include <ns3/spectrum-transmit-filter.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrCsiRsFilter");

NS_OBJECT_ENSURE_REGISTERED(NrCsiRsFilter);

NrCsiRsFilter::NrCsiRsFilter()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrCsiRsFilter::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrCsiRsFilter")
                            .SetParent<SpectrumTransmitFilter>()
                            .AddConstructor<NrCsiRsFilter>();
    return tid;
}

bool
NrCsiRsFilter::DoFilter(Ptr<const SpectrumSignalParameters> params,
                        Ptr<const SpectrumPhy> receiverPhy)
{
    NS_LOG_FUNCTION(this << params);

    auto csiRsSignal = DynamicCast<const NrSpectrumSignalParametersCsiRs>(params);
    if (!csiRsSignal)
    {
        // The signal is not CSI-RS, do not filter.
        return false;
    }

    auto nrReceiverPhy = DynamicCast<const NrSpectrumPhy>(receiverPhy);
    if (!nrReceiverPhy)
    {
        // The signal is CSI-RS, but the receiver is not NR device, filter the signal.
        return true;
    }

    if (nrReceiverPhy->IsGnb())
    {
        // The signal is CSI-RS, but the receiver is not NR UE, filter the signal.
        return true;
    }

    if (csiRsSignal->cellId != nrReceiverPhy->GetCellId())
    {
        // The signal is CSI-RS, the receiver is NR UE, but it is not from its own cell, filter the
        // signal.
        return true;
    }

    if (csiRsSignal->rnti != nrReceiverPhy->GetRnti())
    {
        // The signal is CSI-RS, the receiver is NR UE, but this CSI-RS is intended for other UE,
        // filter the signal.
        return true;
    }
    // The signal is CSI-RS, the receiver is NR UE, and the RNTI and cell ID correspond, do not
    // filter the signal.
    return false;
}

int64_t
NrCsiRsFilter::DoAssignStreams(int64_t stream)
{
    return 0;
}

} // namespace ns3

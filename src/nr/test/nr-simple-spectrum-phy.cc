/*
 * Copyright (c) 2014 Piotr Gawlowicz
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Piotr Gawlowicz <gawlowicz.p@gmail.com>
 *
 */

#include "nr-simple-spectrum-phy.h"

#include "ns3/nr-net-device.h"
#include "ns3/nr-phy-tag.h"
#include "ns3/nr-spectrum-signal-parameters.h"
#include <ns3/antenna-model.h>
#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/log.h>
#include <ns3/simulator.h>

#include <cmath>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSimpleSpectrumPhy");

NS_OBJECT_ENSURE_REGISTERED(NrSimpleSpectrumPhy);

NrSimpleSpectrumPhy::NrSimpleSpectrumPhy()
    : m_cellId(0)
{
}

NrSimpleSpectrumPhy::~NrSimpleSpectrumPhy()
{
    NS_LOG_FUNCTION(this);
}

void
NrSimpleSpectrumPhy::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_channel = nullptr;
    m_mobility = nullptr;
    m_device = nullptr;
    SpectrumPhy::DoDispose();
}

TypeId
NrSimpleSpectrumPhy::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrSimpleSpectrumPhy")
            .SetParent<SpectrumPhy>()
            .AddTraceSource("RxStart",
                            "Data reception start",
                            MakeTraceSourceAccessor(&NrSimpleSpectrumPhy::m_rxStart),
                            "ns3::SpectrumValue::TracedCallback");
    return tid;
}

Ptr<NetDevice>
NrSimpleSpectrumPhy::GetDevice() const
{
    NS_LOG_FUNCTION(this);
    return m_device;
}

Ptr<MobilityModel>
NrSimpleSpectrumPhy::GetMobility() const
{
    NS_LOG_FUNCTION(this);
    return m_mobility;
}

void
NrSimpleSpectrumPhy::SetDevice(Ptr<NetDevice> d)
{
    NS_LOG_FUNCTION(this << d);
    m_device = d;
}

void
NrSimpleSpectrumPhy::SetMobility(Ptr<MobilityModel> m)
{
    NS_LOG_FUNCTION(this << m);
    m_mobility = m;
}

void
NrSimpleSpectrumPhy::SetChannel(Ptr<SpectrumChannel> c)
{
    NS_LOG_FUNCTION(this << c);
    m_channel = c;
}

Ptr<const SpectrumModel>
NrSimpleSpectrumPhy::GetRxSpectrumModel() const
{
    return m_rxSpectrumModel;
}

Ptr<Object>
NrSimpleSpectrumPhy::GetAntenna() const
{
    return m_antenna;
}

void
NrSimpleSpectrumPhy::StartRx(Ptr<SpectrumSignalParameters> spectrumRxParams)
{
    NS_LOG_DEBUG("NrSimpleSpectrumPhy::StartRx");

    NS_LOG_FUNCTION(this << spectrumRxParams);
    Ptr<const SpectrumValue> rxPsd = spectrumRxParams->psd;
    Time duration = spectrumRxParams->duration;

    // the device might start RX only if the signal is of a type
    // understood by this device - in this case, an NR signal.
    Ptr<NrSpectrumSignalParametersDataFrame> nrDataRxParams =
        DynamicCast<NrSpectrumSignalParametersDataFrame>(spectrumRxParams);
    if (nrDataRxParams)
    {
        if (m_cellId > 0)
        {
            if (m_cellId == nrDataRxParams->cellId)
            {
                m_rxStart(rxPsd);
            }
        }
        else
        {
            m_rxStart(rxPsd);
        }
    }
}

void
NrSimpleSpectrumPhy::SetRxSpectrumModel(Ptr<const SpectrumModel> model)
{
    NS_LOG_FUNCTION(this);
    m_rxSpectrumModel = model;
}

void
NrSimpleSpectrumPhy::SetCellId(uint16_t cellId)
{
    NS_LOG_FUNCTION(this);
    m_cellId = cellId;
}

} // namespace ns3

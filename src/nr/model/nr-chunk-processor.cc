// Copyright (c) 2010 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Nicola Baldo <nbaldo@cttc.es>
// Modified by : Marco Miozzo <mmiozzo@cttc.es>
//        (move from CQI to Ctrl and Data SINR Chunk processors

#include "nr-chunk-processor.h"

#include <ns3/log.h>
#include <ns3/spectrum-value.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrChunkProcessor");

NrChunkProcessor::NrChunkProcessor()
{
    NS_LOG_FUNCTION(this);
}

NrChunkProcessor::~NrChunkProcessor()
{
    NS_LOG_FUNCTION(this);
}

void
NrChunkProcessor::AddCallback(NrChunkProcessorCallback c)
{
    NS_LOG_FUNCTION(this);
    m_nrChunkProcessorCallbacks.push_back(c);
}

void
NrChunkProcessor::Start()
{
    NS_LOG_FUNCTION(this);
    m_sumValues = nullptr;
    m_totDuration = MicroSeconds(0);
}

void
NrChunkProcessor::EvaluateChunk(const SpectrumValue& sinr, Time duration)
{
    NS_LOG_FUNCTION(this << sinr << duration);
    if (!m_sumValues)
    {
        m_sumValues = Create<SpectrumValue>(sinr.GetSpectrumModel());
    }
    (*m_sumValues) += sinr * duration.GetSeconds();
    m_totDuration += duration;
}

void
NrChunkProcessor::End()
{
    NS_LOG_FUNCTION(this);
    if (m_totDuration.GetSeconds() > 0)
    {
        for (auto it = m_nrChunkProcessorCallbacks.begin(); it != m_nrChunkProcessorCallbacks.end();
             it++)
        {
            (*it)((*m_sumValues) / m_totDuration.GetSeconds());
        }
    }
    else
    {
        NS_LOG_WARN("m_numSinr == 0");
    }
}

void
NrSpectrumValueCatcher::ReportValue(const SpectrumValue& value)
{
    m_value = value.Copy();
}

Ptr<SpectrumValue>
NrSpectrumValueCatcher::GetValue()
{
    return m_value;
}

} // namespace ns3

// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only AND NIST-Software

/*
 * Author: Nicola Baldo <nbaldo@cttc.es>
 * Modified by : Marco Miozzo <mmiozzo@cttc.es>
 *        (move from CQI to Ctrl and Data SINR Chunk processors)
 * Modified by : Piotr Gawlowicz <gawlowicz.p@gmail.com>
 *        (removed all Lte***ChunkProcessor implementations
 *        and created generic LteChunkProcessor)
 *
 * This NrSlChunkProcessor, authored by NIST
 * is derived from LteChunkProcessor originally authored by Nicola Baldo <nbaldo@cttc.es>.
 */

#include "nr-sl-chunk-processor.h"

#include <ns3/log.h>
#include <ns3/spectrum-value.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlChunkProcessor");

NrSlChunkProcessor::NrSlChunkProcessor()
{
    NS_LOG_FUNCTION(this);
}

NrSlChunkProcessor::~NrSlChunkProcessor()
{
    NS_LOG_FUNCTION(this);
}

void
NrSlChunkProcessor::AddCallback(NrSlChunkProcessorCallback c)
{
    NS_LOG_FUNCTION(this);
    m_nrSlChunkProcessorCallbacks.push_back(c);
}

void
NrSlChunkProcessor::Start(bool init)
{
    NS_LOG_FUNCTION(this);

    if (init)
    {
        m_chunkValues.clear();
    }

    // Creates a new storage
    NrSlChunkValue newValue;
    newValue.m_sumValues = nullptr;
    newValue.m_totDuration = MicroSeconds(0);

    m_chunkValues.push_back(newValue);
}

void
NrSlChunkProcessor::EvaluateChunk(uint32_t index, const SpectrumValue& sinr, Time duration)
{
    NS_LOG_FUNCTION(this << index << sinr << duration);
    if (!m_chunkValues[index].m_sumValues)
    {
        m_chunkValues[index].m_sumValues = Create<SpectrumValue>(sinr.GetSpectrumModel());
    }
    *(m_chunkValues[index].m_sumValues) += sinr * duration.GetSeconds();
    m_chunkValues[index].m_totDuration += duration;
}

void
NrSlChunkProcessor::End()
{
    NS_LOG_FUNCTION(this);

    if (m_chunkValues[0].m_totDuration.GetSeconds() > 0)
    {
        std::vector<SpectrumValue> values;
        std::vector<NrSlChunkValue>::iterator itValues;
        for (itValues = m_chunkValues.begin(); itValues != m_chunkValues.end(); itValues++)
        {
            values.push_back(*((*itValues).m_sumValues) / (*itValues).m_totDuration.GetSeconds());
        }

        std::vector<NrSlChunkProcessorCallback>::iterator it;
        for (it = m_nrSlChunkProcessorCallbacks.begin(); it != m_nrSlChunkProcessorCallbacks.end();
             ++it)
        {
            (*it)(values);
        }
    }
    else
    {
        NS_LOG_WARN("m_numSinr == 0");
    }
}

} // namespace ns3

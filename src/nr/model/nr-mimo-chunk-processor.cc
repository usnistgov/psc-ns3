// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mimo-chunk-processor.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMimoChunkProcessor");

void
NrMimoChunkProcessor::AddCallback(MimoSinrChunksCb cb)
{
    NS_LOG_FUNCTION(this);
    m_sinrChunksCbs.push_back(cb);
}

void
NrMimoChunkProcessor::AddCallback(MimoSignalChunksCb cb)
{
    NS_LOG_FUNCTION(this);
    m_signalChunksCbs.push_back(cb);
}

void
NrMimoChunkProcessor::Start()
{
    NS_LOG_FUNCTION(this);
    m_mimoSinrChunks.clear();
    m_mimoSignalChunks.clear();
}

void
NrMimoChunkProcessor::EvaluateChunk(const MimoSinrChunk& mimoSinr)
{
    NS_LOG_FUNCTION(this);
    m_mimoSinrChunks.push_back(mimoSinr);
}

void
NrMimoChunkProcessor::EvaluateChunk(const MimoSignalChunk& mimoSignal)
{
    NS_LOG_FUNCTION(this);
    m_mimoSignalChunks.push_back(mimoSignal);
}

void
NrMimoChunkProcessor::End()
{
    NS_LOG_FUNCTION(this);

    // Callbacks with a list of all SINR chunks seen in this slot
    for (const auto& cb : m_sinrChunksCbs)
    {
        (cb)(m_mimoSinrChunks);
    }
    // Callbacks with a list of all signal chunks seen in this slot
    for (const auto& cb : m_signalChunksCbs)
    {
        (cb)(m_mimoSignalChunks);
    }
}

} // namespace ns3

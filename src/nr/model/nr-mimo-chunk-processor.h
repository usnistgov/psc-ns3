// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_MIMO_CHUNK_PROCESSOR_H
#define NR_MIMO_CHUNK_PROCESSOR_H

#include "nr-mimo-matrices.h"

#include <ns3/nstime.h>
#include <ns3/object.h>
#include <ns3/ptr.h>
#include <ns3/spectrum-value.h>

namespace ns3
{

/// \brief MIMO SINR used to compute the TBLER of a data transmission
struct MimoSinrChunk
{
    NrSinrMatrix mimoSinr; ///< The MIMO SINR values, dimensions rank * nRBs
    uint16_t rnti{0};      ///< RNTI, required in OFDMA UL to filter received signals by UEs
    Time dur;              ///< Duration of the signal
};

/// \brief MIMO signal information used to compute CQI feedback including rank and precoding matrix
struct MimoSignalChunk
{
    ComplexMatrixArray chanSpct; ///< Frequency-domain channel matrix
    NrCovMat interfNoiseCov;     ///< Interference-and-noise-covariance matrix
    uint16_t rnti{0};            ///< RNTI, required in OFDMA UL to filter received signals by UEs
    Time dur;                    ///< Duration of the signal
};

using MimoSinrChunksCb = Callback<void, const std::vector<MimoSinrChunk>&>;
using MimoSignalChunksCb = Callback<void, const std::vector<MimoSignalChunk>&>;

class NrMimoChunkProcessor : public SimpleRefCount<NrMimoChunkProcessor>
{
  public:
    /// \brief Add a callback for processing received SINR values
    /// \param cb the callback function
    void AddCallback(MimoSinrChunksCb cb);

    /// \brief Add a callback for processing the MIMO signal parameters
    /// \param cb the callback function
    void AddCallback(MimoSignalChunksCb cb);

    /// \brief Start processing a transmission, clear internal variables
    void Start();

    /// \brief Store the current MIMO SINR and duration
    /// \param mimoSinr the MIMO SINR information
    void EvaluateChunk(const MimoSinrChunk& mimoSinr);

    /// \brief Store the current MIMO signal parameters
    /// \param mimoSignal the signal parameters including channel and interference covariance matrix
    void EvaluateChunk(const MimoSignalChunk& mimoSignal);

    /// \brief Finish calculation and inform interested objects about calculated values
    void End();

  private:
    std::vector<MimoSinrChunk> m_mimoSinrChunks;     ///< The MIMO SINR values seen in this TTI
    std::vector<MimoSignalChunk> m_mimoSignalChunks; ///< The MIMO signal values seen in this TTI

    std::vector<MimoSinrChunksCb> m_sinrChunksCbs;     ///< The callbacks for SINR values
    std::vector<MimoSignalChunksCb> m_signalChunksCbs; ///< The callbacks for signal values
};
} // namespace ns3

#endif // NR_MIMO_CHUNK_PROCESSOR_H

// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_RX_SIGNAL_H
#define NR_RX_SIGNAL_H

#include "nr-amc.h"
#include "nr-mimo-chunk-processor.h"

#include <ns3/uinteger.h>

namespace ns3
{

/// \brief Helper struct for processing and storing received signals for use in CSI feedback
struct NrMimoSignal : public SimpleRefCount<NrMimoSignal>
{
    NrMimoSignal() = default;
    /// \brief Constructor that consolidates the different signals in a vector of received chunks.
    /// \param mimoChunks the signal chunks with channel and interference covariance matrices
    NrMimoSignal(const std::vector<MimoSignalChunk>& mimoChunks);

    /// \brief Combine the multiple received PDSCH channel matrices into a single channel matrix.
    /// Each of the individual channel matrices can have pages with all-zero elements when the
    /// corresponding RB was not allocated to that specific UE. Combining all non-zero pages of all
    /// received matrices (all scheduled UEs in the cell) allows computing feedback over all RBs
    /// that were allocated in the current transmission.
    /// \param mimoChunks the vector of received signal chunks, one per UE and per time chunk.
    /// \return a single channel matrix that combines all non-zero pages of the different channel
    /// matrices in mimoChunks
    static ComplexMatrixArray ConsolidateChanSpctMimo(
        const std::vector<MimoSignalChunk>& mimoChunks);

    /// \brief Combine the multiple received PDSCH interference matrices into a single matrix.
    /// This function performs a simple linear average. When there multiple UEs, the interference
    /// matrix in each time chunk is counted multiple times, but this is averaged out.
    /// \param mimoChunks the vector of received signal chunks, one per UE and per time chunk.
    /// \return a single average interference and noise covariance matrix
    static NrCovMat ComputeAvgCovMatMimo(const std::vector<MimoSignalChunk>& mimoChunks);

    ComplexMatrixArray m_chanMat{}; ///< Channel Matrix; nRxPorts * nTxPorts * nRbs
    NrCovMat m_covMat{}; ///< Interference and noise covariance matrix; nRxPorts * nRxPorts * nRbs
};

} // namespace ns3

#endif // NR_RX_SIGNAL_H

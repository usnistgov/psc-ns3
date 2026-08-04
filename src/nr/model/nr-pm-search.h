// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_PM_SEARCH_H
#define NR_PM_SEARCH_H

#include "nr-amc.h"
#include "nr-mimo-chunk-processor.h"
#include "nr-mimo-signal.h"

#include "ns3/random-variable-stream.h"
#include <ns3/uinteger.h>

namespace ns3
{

/// \brief Base class for searching optimal precoding matrices and creating full CQI/PMI feedback
/// This is a mostly abstract base class that provides configuration for common parameters.
class NrPmSearch : public Object
{
  public:
    /// \brief Get TypeId
    /// \return the TypeId
    static TypeId GetTypeId();

    /// \brief Default constructor
    NrPmSearch();

    /// \brief Set the AMC object to be used for MCS and TB size calculation
    /// \param amc the NrAmc object
    void SetAmc(Ptr<const NrAmc> amc);

    /// \brief Set the antenna parameters of the gNB antenna.
    /// \param numTotalPorts Total number of ports in the gNB antenna array
    /// \param isDualPol True when gNB has a dual-polarized antenna array
    /// \param numHPorts Number of horizontal ports in the gNB antenna array
    /// \param numVPorts Number of vertical ports in the gNB antenna array
    void SetGnbParams(bool isDualPol, size_t numHPorts, size_t numVPorts);

    /// \brief Set the antenna parameters of the UE antenna.
    /// \param numTotalPorts Total number of ports in the UE antenna array
    void SetUeParams(size_t numTotalPorts);

    ///\brief Set the subband size (in number of RBs)
    /// \param subbandSize the subband size (in number of RBs)
    void SetSubbandSize(size_t subbandSize);

    /// \return The subband size in number of RBs
    size_t GetSubbandSize() const;

    /// \brief Create and initialize the codebook for each rank.
    virtual void InitCodebooks() = 0;

    /// \brief Parameters that define if PMI should be updated or if previous PMI values are used.
    struct PmiUpdate
    {
        PmiUpdate() = default;

        PmiUpdate(bool uWb, bool uSb)
        {
            updateWb = uWb;
            updateSb = uSb;
        }

        bool updateWb{false}; ///< Defines whether to update WB PMI
        bool updateSb{false}; ///< Defines whether to update SB PMI
    };

    /// \brief Create CQI feedback with optimal rank, optimal PMI, and corresponding CQI values.
    /// Optimal rank is considered as the rank that maximizes the achievable TB size when using the
    /// optimal PMI. The optimal WB/SB PMI values are updated based on pmiUpdate. If there is no
    /// update to the PMI values, the previously found PMI values can be ysed used.
    /// \param rxSignalRb the receive signal parameters (channel and interference matrices)
    /// \param pmiUpdate struct that defines if WB/SB PMIs need to be updated
    /// \return the CQI feedback message that contains the optimum RI, PMI, CQI, and full precoding
    /// matrix (dimensions: nGnbPorts * rank * nRbs)
    virtual PmCqiInfo CreateCqiFeedbackMimo(const NrMimoSignal& rxSignalRb,
                                            PmiUpdate pmiUpdate) = 0;

    /// \brief Downsample the input channel matrix into bins of at most m_subbandSize PRBs
    /// \param channelMatrix matrix to downsample
    /// \return downsampled matrix
    virtual NrIntfNormChanMat SubbandDownsampling(const NrIntfNormChanMat& channelMatrix);

    /// \brief Upsample the input per-subband precoding matrix into a per-PRB precoding matrix
    /// \param precMat matrix to upsample
    /// \return upsampled matrix
    virtual NrIntfNormChanMat SubbandUpsampling(const NrIntfNormChanMat& precMat,
                                                size_t numPrbs) const;

    enum DownsamplingTechnique
    {
        FirstPRB,   ///< Downsample m_subbandSize samples to bands based on the first PRB
        RandomPRB,  ///< Downsample m_subbandSize samples to bands based on a random PRB
        AveragePRB, ///< Downsample m_subbandSize samples to bands based on the average of PRBs
    };

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model.  Return the number of streams (possibly zero) that
     * have been assigned.
     *
     * \param stream first stream index to use
     * \return the number of stream indices assigned by this model
     */
    int64_t AssignStreams(int64_t stream);

  protected:
    struct PrecMatParams : public SimpleRefCount<PrecMatParams>
    {
        size_t wbPmi{};                 ///< Wideband PMI (i1, index of W1 matrix)
        std::vector<size_t> sbPmis{};   ///< Subband PMI values (i2, indices of W2 matrices)
        ComplexMatrixArray sbPrecMat{}; ///< Precoding matrix (nGnbPorts * rank * nSubbands)
        double perfMetric{}; ///< Performance metric for these precoding parameters (e.g., average
                             ///< capacity / SINR / CQI / TB size) used to find optimal precoding
    };

    size_t m_subbandSize{1};   ///< Size of each subband (in number of RBs)
    bool m_subbandCqiClamping; ///< Clamp sub-band CQI range to wideband CQI [-1,+2], according to
                               ///< 3GPP
    enum DownsamplingTechnique m_downsamplingTechnique; ///< Technique used to downsample PRBs
    Ptr<UniformRandomVariable>
        m_downsamplingUniRand; ///< Uniform variable stream used to downsample PRBs

    bool m_isGnbDualPol{false}; ///< True when gNB has a dual-polarized antenna array
    size_t m_nGnbHPorts{0};     ///< Number of horizontal ports in the gNB antenna array
    size_t m_nGnbVPorts{0};     ///< Number of vertical ports in the gNB antenna array
    size_t m_nGnbPorts{0};      ///< Total number of ports in the gNB antenna array
    size_t m_nRxPorts{0};       ///< Number of receive ports at this UE

    Ptr<const NrAmc> m_amc{nullptr}; ///< The NrAmc to be used for computing TB size and MCS

    uint8_t m_rankLimit{UINT8_MAX}; ///< Limit the UE's maximum supported rank
    std::vector<uint8_t> m_ranks{}; ///< The set of ranks for which to compute precoding matrices
  private:
    /**
     * Calculate the number of subbands should be allocated for a given channel matrix and subband
     * size.
     *
     * \param chanMat The input PRB-based channel matrix
     */
    size_t GetNumSubbands(const NrIntfNormChanMat& chanMat) const;

    /**
     * Calculate the SB value from the first PRB within a subband-size.
     * Called by SubbandDownsampling().
     *
     * \param chanMat The input PRB-based channel matrix
     * \param downsampledChanMat The output SB-based channel matrix
     */
    void GetSubbandDownsampleFirstPrb(const NrIntfNormChanMat& chanMat,
                                      ComplexMatrixArray& downsampledChanMat) const;
    /**
     * Calculate the SB value from a random PRB within a subband-size.
     * Called by SubbandDownsampling().
     *
     * \param chanMat The input PRB-based channel matrix
     * \param downsampledChanMat The output SB-based channel matrix
     * */
    void GetSubbandDownsampleRandomPrb(const NrIntfNormChanMat& chanMat,
                                       ComplexMatrixArray& downsampledChanMat) const;

    /**
     * Calculate the average SB value from PRBs within a subband-size.
     * Called by SubbandDownsampling().
     *
     * \param chanMat The input PRB-based channel matrix
     * \param downsampledChanMat The output SB-based channel matrix
     * */
    void GetSubbandDownsampleAveragePrb(const NrIntfNormChanMat& chanMat,
                                        ComplexMatrixArray& downsampledChanMat) const;
};

} // namespace ns3

#endif // NR_PM_SEARCH_H

// Copyright (c) 2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NRERRORMODEL_H
#define NRERRORMODEL_H

#include "nr-mimo-chunk-processor.h"

#include <ns3/object.h>
#include <ns3/spectrum-value.h>

#include <vector>

namespace ns3
{

/**
 * \ingroup error-models
 * \brief Store the output of an NRErrorModel
 *
 */
struct NrErrorModelOutput : public SimpleRefCount<NrErrorModelOutput>
{
    /**
     * \brief NrErrorModelOutput default constructor (deleted)
     */
    NrErrorModelOutput() = delete;

    /**
     * \brief Official NrErrorModelOutput constructor
     * \param tbler transport block error rate to store
     */
    NrErrorModelOutput(double tbler)
        : m_tbler(tbler)
    {
    }

    /**
     * \brief ~NrErrorModelOutput
     */
    virtual ~NrErrorModelOutput()
    {
    }

    double m_tbler{0.0}; //!< Transport Block Error Rate
};

/**
 * \ingroup error-models
 * \brief Interface for calculating the error probability for a transport block
 *
 * Any error model that wishes to work in Spectrum or in AMC should use
 * this class as a base class.
 *
 * \section nr_error_model_conf Configuration
 *
 * The type of the error model can be configured through the helper method
 * NrHelper::SetUlErrorModel() or NrHelper::SetDlErrorModel().
 *
 * The types of error model that can be used are the following:
 * NrEesmIrT2, NrEesmIrT1, NrEesmCcT1, NrEesmCcT2, NrLteMiErrorModel.
 *
 * \see GetTbDecodificationStats
 * \see NrEesmIrT2
 * \see NrEesmIrT1
 * \see NrEesmCcT1
 * \see NrEesmCcT2
 * \see NrLteMiErrorModel
 */
class NrErrorModel : public Object
{
  public:
    /**
     * \brief GetTypeId
     * \return the TypeId of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrErrorModel default constructor
     */
    NrErrorModel();

    /**
     * \brief deconstructor
     */
    ~NrErrorModel() override;

    /**
     * \brief Indicate the mode (UL or DL)
     *
     * In some methods, the error model has to know if the asked value
     * is for UL or DL.
     */
    enum Mode
    {
        DL, //!< DL
        UL  //!< UL
    };

    /**
     * \brief Vector of previous output
     *
     *
     * Used in case of HARQ: any result will be stored in this vector and used
     * to decode next retransmissions.
     */
    typedef std::vector<Ptr<NrErrorModelOutput>> NrErrorModelHistory;

    /**
     * \brief Get an output for the decodification error probability of a given
     * transport block.
     *
     * The subclasses can store more information by subclassing the NrErrorModelOutput
     * class, and returning a casted instance. The error model should take into
     * consideration the history, even if some time (e.g., when called by the AMC
     * or when called the first time by the spectrum model) the history will be
     * empty.
     *
     * This method should not return a nullptr, ever.
     *
     * \param sinr SINR vector
     * \param map RB map
     * \param size Transport block size
     * \param mcs MCS
     * \param history History of the retransmission
     * \return A pointer to an output, with the tbler and other customized values
     */
    virtual Ptr<NrErrorModelOutput> GetTbDecodificationStats(
        const SpectrumValue& sinr,
        const std::vector<int>& map,
        uint32_t size,
        uint8_t mcs,
        const NrErrorModelHistory& history) = 0;

    /**
     * \brief Get the SpectralEfficiency for a given CQI
     * \param cqi CQI to take into consideration
     * \return the spectral efficiency
     */
    virtual double GetSpectralEfficiencyForCqi(uint8_t cqi) = 0;

    /**
     * \brief Get the SpectralEfficiency for a given MCS
     * \param mcs MCS to take into consideration
     * \return the spectral efficiency
     */
    virtual double GetSpectralEfficiencyForMcs(uint8_t mcs) const = 0;

    /**
     * \brief Get the payload size (in bytes) for a given mcs and resource block number
     *
     * \param usefulSc Useful subcarriers
     * \param mcs MCS
     * \param rbNum Number of resource blocks (even in more than 1 symbol)
     * \param mode UL or DL mode
     * \return The payload size of the resource blocks, in bytes
     */
    virtual uint32_t GetPayloadSize(uint32_t usefulSc,
                                    uint8_t mcs,
                                    uint8_t rank,
                                    uint32_t rbNum,
                                    Mode mode) const = 0;

    /**
     * \brief Get the maximum codeblock size
     *
     * \param tbSize Transport block size for which calculate the CB size
     * \param mcs MCS of the transmission
     * \return the codeblock size
     */
    virtual uint32_t GetMaxCbSize(uint32_t tbSize, uint8_t mcs) const = 0;

    /**
     * \brief Get the maximum MCS
     *
     * \return the maximum MCS that is permitted with the error model
     */
    virtual uint8_t GetMaxMcs() const = 0;

    /// \brief Get an output for the decoding error probability of a given transport block.
    /// This method is not purely virtual. If derived ErrorModel does not override, the MIMO matrix
    /// is converted to a linear SpectrumValue, and the non-MIMO method is called.
    /// \param mimoChunks vector of SINR chunks containing MIMO SINR matrices
    /// \param map RB map (must be vector<int> as created by OSS code)
    /// \param size Transport block size
    /// \param mcs MCS
    /// \param history History of the retransmission
    /// \return A pointer to an output, with the tbler and other customized values
    virtual Ptr<NrErrorModelOutput> GetTbDecodificationStatsMimo(
        const std::vector<MimoSinrChunk>& mimoChunks,
        const std::vector<int>& map,
        uint32_t size,
        uint8_t mcs,
        uint8_t rank,
        const NrErrorModelHistory& history);

    /// \brief Compute an average SINR matrix
    /// \param mimoChunks vector of SINR chunks containing MIMO SINR matrices
    /// \return A 2D matrix of the average SINR for this TB reception, dimensions nMimoLayers x nRbs
    virtual NrSinrMatrix ComputeAvgSinrMimo(const std::vector<MimoSinrChunk>& sinrChunks);

    /// \brief Create an equivalent RB index map for vectorized SINR values
    /// Matches layer-to-codeword mapping in TR 38.211, Table 7.3.1.3-1
    /// If map contains index "j", the output vectorized map contains
    /// {j * rank, j * rank + 1, ..., j * rank + rank - 1}.
    /// Example: input RB map = {0, 1, 7, 11}, rank = 2
    /// vectorizedMap = {0, 1, 2, 3, 14, 15, 22, 23}
    /// \param map The indices of used RBs for this transmission (the columns of the SINR matrix)
    /// \param rank The number of MIMO layers
    /// \return the indices corresponding to "map" when the SINR matrix is vectorized
    /// Note: result will be used in OSS function which require vector<int> type
    std::vector<int> CreateVectorizedRbMap(std::vector<int> map, uint8_t rank);
};

} // namespace ns3
#endif // NRERRORMODEL_H

// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_PM_SEARCH_FULL_H
#define NR_PM_SEARCH_FULL_H

#include "nr-amc.h"
#include "nr-cb-two-port.h"
#include "nr-pm-search.h"

#include <ns3/object-factory.h>

namespace ns3
{

/// \brief An implementation of NrPmSearch that uses exhaustive search for 3GPP Type-I codebooks.
/// This class creates a CQI/PMI/RI feedback message by looping over all ranks and selecting the
/// rank that results in the largest achievable TB size.
/// When a PMI update is requested, the optimal precoding matrices (PMI) are updated using
/// exhaustive search over all possible precoding matrices specified in a codebook that is
/// compatible to 3GPP TS 38.214 Type-I.
class NrPmSearchFull : public NrPmSearch
{
  public:
    /// \brief Get TypeId
    /// \return the TypeId
    static TypeId GetTypeId();

    /// \brief Create and initialize the codebook for each rank.
    void InitCodebooks() override;

    /// \brief Create CQI feedback with optimal rank, optimal PMI, and corresponding CQI values.
    /// Optimal rank is considered as the rank that maximizes the achievable TB size when using the
    /// optimal PMI. The optimal WB/SB PMI values are updated based on pmiUpdate. If there is no
    /// update to the PMI values, the previously found PMI values are used.
    /// \param rxSignalRb the receive signal parameters (channel and interference matrices)
    /// \param pmiUpdate struct that defines if WB/SB PMIs need to be updated
    /// \return the CQI feedback message that contains the optimum CQI, RI, PMI, and full precoding
    /// matrix (dimensions: nGnbPorts * rank * nRbs)
    PmCqiInfo CreateCqiFeedbackMimo(const NrMimoSignal& rxSignalRb, PmiUpdate pmiUpdate) override;

    /// \brief Set the TypeId of the codebook (NrCbTypeOne) to be used.
    /// \param typeId the TypeId of the codebook
    void SetCodebookTypeId(const TypeId& typeId);

    /// \brief Set the ns-3 attribute of the codebook (NrCbTypeOne).
    /// \param attrName the name of the attribute
    /// \param attrVal the value of the attribute
    void SetCodebookAttribute(const std::string& attrName, const AttributeValue& attrVal);

  protected:
    struct RankParams
    {
        Ptr<PrecMatParams> precParams; ///< The precoding parameters (WB/SB PMIs)
        Ptr<NrCbTypeOne> cb;           ///< The codebook
    };

    /// \brief Update the WB and/or SB PMI, or neither.
    /// \param rbNormChanMat the interference-normed channel matrix per RB
    /// \param pmiUpdate the struct defining if updates to SB or WB PMI are necessary
    void ConditionallyUpdatePrecoding(const NrIntfNormChanMat& rbNormChanMat, PmiUpdate pmiUpdate);

    /// \brief For all ranks, update the optimum precoding matrices (wideband and subband).
    /// \param rbNormChanMat the interference-normed channel matrix per RB
    void UpdateAllPrecoding(const NrIntfNormChanMat& rbNormChanMat);

    /// \brief For all ranks, update the opt subband PMI assuming previous value of wideband PMI.
    /// \param rbNormChanMat the interference-normed channel matrix per RB
    void UpdateSubbandPrecoding(const NrIntfNormChanMat& rbNormChanMat);

    /// \brief Create CQI feedback message for a particular rank.
    /// \param rank the rank for which to create the CQI feedback
    /// \param rbNormChanMat the interference-normed channel matrix per RB
    /// \return the CQI message with PMI and CQI values, as well as expected TB size
    PmCqiInfo CreateCqiForRank(uint8_t rank, const NrIntfNormChanMat& rbNormChanMat) const;

    /// \brief Find the optimal subband precoding matrix for the given wideband precoding.
    /// \param sbNormChanMat the interference-normed channel matrix per subband
    /// \param i1 the index of the wideband precoding matrix W1
    /// \param rank the rank (number of MIMO layers)
    /// \return a struct containing wideband and subband PMIs, and full precoding matrix.
    Ptr<PrecMatParams> FindOptSubbandPrecoding(const NrIntfNormChanMat& sbNormChanMat,
                                               size_t i1,
                                               uint8_t rank) const;

    /// \brief Create the subband precoding matrices for the given wideband precoding.
    /// \param i1 the index of the wideband precoding matrix W1
    /// \param rank the rank (number of MIMO layers)
    /// \param nSubbands the number of subbands (desired number of pages in each precoding matrix)
    /// \return a vector of all possible precoding matrices for fixed i1 and rank
    std::vector<ComplexMatrixArray> CreateSubbandPrecoders(size_t i1,
                                                           uint8_t rank,
                                                           size_t nSubbands) const;

    static ComplexMatrixArray ExpandPrecodingMatrix(ComplexMatrixArray basePrecMat,
                                                    size_t nSubbands);

    /// \brief Compute the Shannon capacity for each possible precoding matrix in each subband.
    /// \param sbNormChanMat the interference-normed channel matrix per subband
    /// \param allPrecMats a vector of all possible subband precoding matrices for fixed i1 and rank
    /// \return a matrix with the capacity values (nSubbands x allPrecMats.size())
    DoubleMatrixArray ComputeCapacityForPrecoders(
        const NrIntfNormChanMat& sbNormChanMat,
        std::vector<ComplexMatrixArray> allPrecMats) const;

    std::vector<RankParams> m_rankParams; ///< The parameters (PMI values, codebook) for each rank
    ObjectFactory m_cbFactory;            ///< The factory used to create the codebooks
};

} // namespace ns3

#endif // NR_PM_SEARCH_FULL_H

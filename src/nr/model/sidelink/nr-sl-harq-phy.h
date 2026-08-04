// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_SL_HARQ_PHY_MODULE_H
#define NR_SL_HARQ_PHY_MODULE_H

#include "ns3/nr-error-model.h"
#include "ns3/nr-harq-phy.h"
#include <ns3/simple-ref-count.h>

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ns3
{

/**
 * \ingroup error-models
 *
 * \brief HARQ functionalities for the SL PHY layer
 *
 * (i.e., decoding buffers for incremental redundancy management)
 *
 */
class NrSlHarqPhy : public NrHarqPhy
{
  public:
    /**
     * \brief Destructor
     */
    ~NrSlHarqPhy();

    /**
     * \brief Return the info of the HARQ procId in case of retranmissions
     *        for NR SL DATA
     * \param rnti the RNTI
     * \param harqProcId the HARQ proc id
     * \return the vector of the info related to HARQ proc Id
     */
    const NrErrorModel::NrErrorModelHistory& GetHarqProcessInfoSl(uint16_t rnti,
                                                                  uint8_t harqProcId);
    /**
     * \brief Update the Info associated to the decodification of an HARQ process
     *        for NR SL DATA
     * \param rnti the RNTI
     * \param harqProcId the HARQ process id
     * \param output output of the error model
     */
    void UpdateSlDataHarqProcessStatus(uint16_t rnti,
                                       uint8_t harqProcId,
                                       const Ptr<NrErrorModelOutput>& output);
    /**
     * \brief Reset the info associated to the decodification of an HARQ process
     * for NR SL DATA
     * \param rnti the RNTI
     * \param id the HARQ process id
     */
    void ResetSlDataHarqProcessStatus(uint16_t rnti, uint8_t id);
    /**
     * \brief Store the info of the successfully decoded NR SL TB
     *
     * \param rnti The UE identifier
     * \param harqId The HARQ id
     */
    void IndicatePrevDecoded(uint16_t rnti, uint8_t harqId);
    /**
     * \brief Gets the flag that indicates whether or not the TB's with given
     *        \rnti and \pharqId has already been decoded.
     * \param rnti The UE identifier
     * \param harqId The HARQ id
     * \returns True, if the TB has been decoded; false, otherwise.
     */
    bool IsPrevDecoded(uint16_t rnti, uint8_t harqId);
    /**
     * \brief Remove the entry with given \rnti and \pharqId, which indicates
     *        that the TB's has already been decoded.
     * \param rnti The UE identifier
     * \param harqId The HARQ id
     */
    void RemovePrevDecoded(uint16_t rnti, uint8_t harqId);

  private:
    HistoryMap m_slHistory; //!< HARQ history map for NR SL PSSCH
    std::unordered_set<uint32_t>
        m_slDecodedTb; //!< Container to track NR Sidelink communication decoded TBs
};

} // namespace ns3

#endif /* NR_SL_HARQ_PHY_MODULE_H */

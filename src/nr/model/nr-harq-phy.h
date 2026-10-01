// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_HARQ_PHY_MODULE_H
#define NR_HARQ_PHY_MODULE_H

#include "nr-error-model.h"

#include <unordered_map>
#include <vector>

namespace ns3
{

/**
 * \ingroup error-models
 *
 * \brief HARQ functionalities for the PHY layer
 *
 * (i.e., decodification buffers for incremental redundancy management)
 *
 */
class NrHarqPhy
{
  public:
    /**
     * \brief Destructor
     */
    ~NrHarqPhy();

    /**
     * \brief Return the info of the HARQ procId in case of retransmissions
     * for DL/UL (asynchronous)
     * \param dl if downlink
     * \param rnti the RNTI
     * \param harqProcId the HARQ proc id
     * \return the vector of the info related to HARQ proc Id
     */
    const NrErrorModel::NrErrorModelHistory& GetHarqProcessInfoDlUl(bool dl,
                                                                    uint16_t rnti,
                                                                    uint8_t harqProcId)
    {
        return dl ? GetHarqProcessInfoDl(rnti, harqProcId) : GetHarqProcessInfoUl(rnti, harqProcId);
    };

    /**
     * \brief Return the info of the HARQ procId in case of retransmissions
     * for DL (asynchronous)
     * \param rnti the RNTI
     * \param harqProcId the HARQ proc id
     * \return the vector of the info related to HARQ proc Id
     */
    const NrErrorModel::NrErrorModelHistory& GetHarqProcessInfoDl(uint16_t rnti,
                                                                  uint8_t harqProcId);

    /**
     * \brief Return the info of the HARQ procId in case of retransmissions
     * for UL (asynchronous)
     * \param rnti the RNTI
     * \param harqProcId the HARQ process id
     * \return the vector of the info related to HARQ proc Id
     */
    const NrErrorModel::NrErrorModelHistory& GetHarqProcessInfoUl(uint16_t rnti,
                                                                  uint8_t harqProcId);

    /**
     * \brief Update the Info associated to the decodification of an HARQ process
     * for DL (asynchronous)
     * \param rnti the RNTI
     * \param harqProcId the HARQ process id
     * \param output output of the error model
     */
    void UpdateDlHarqProcessStatus(uint16_t rnti,
                                   uint8_t harqProcId,
                                   const Ptr<NrErrorModelOutput>& output);

    /**
     * \brief Reset the info associated to the decodification of an HARQ process
     * for DL (asynchronous)
     * \param rnti the RNTI
     * \param id the HARQ process id
     */
    void ResetDlHarqProcessStatus(uint16_t rnti, uint8_t id);

    /**
     * \brief Update the Info associated to the decodification of an HARQ process
     * for UL (asynchronous)
     * \param rnti the RNTI
     * \param harqProcId the HARQ process id
     * \param output output of the error model
     */
    void UpdateUlHarqProcessStatus(uint16_t rnti,
                                   uint8_t harqProcId,
                                   const Ptr<NrErrorModelOutput>& output);

    /**
     * \brief Reset the info associated to the decodification of an HARQ process
     * for UL (asynchronous)
     * \param rnti the RNTI
     * \param id the HARQ process id
     */
    void ResetUlHarqProcessStatus(uint16_t rnti, uint8_t id);

  protected:
    /**
     * \brief Map between a process id and its HARQ history (a vector of pointers)
     *
     * The HARQ history depends on the error model (LTE error model stores MI (MIESM-based), while
     * NR error model stores SINR (EESM-based)) as well as on the HARQ combining method.
     */
    typedef std::unordered_map<uint8_t, NrErrorModel::NrErrorModelHistory> ProcIdHistoryMap;
    /**
     * \brief Map between an RNTI and its ProcIdHistoryMap
     */
    typedef std::unordered_map<uint16_t, ProcIdHistoryMap> HistoryMap;
    /**
     * \brief Return the HARQ history map of the retransmissions of all process ids of a particular
     * RNTI \param rnti the RNTI \param map the Map between RNTIs and their history \return the
     * HistoryMap of such RNTI
     */
    HistoryMap::iterator GetHistoryMapOf(HistoryMap* map, uint16_t rnti) const;
    /**
     * \brief Return the HARQ history of a particular process id
     * \param procId the process id
     * \param map the Map between processes ids and their history
     * \return the ProcIdHistoryMap of such process id
     */
    ProcIdHistoryMap::iterator GetProcIdHistoryMapOf(ProcIdHistoryMap* map, uint16_t procId) const;

    /**
     * \brief Reset the HARQ history of a particular process id
     * \param rnti the RNTI
     * \param id the HARQ process id
     * \param map the Map between RNTIs and their history
     */
    void ResetHarqProcessStatus(HistoryMap* map, uint16_t rnti, uint8_t harqProcId) const;
    /**
     * \brief Update the HARQ history of a particular process id
     * \param rnti the RNTI
     * \param id the HARQ process id
     * \param map the Map between RNTIs and their history
     * \param output the new HARQ history to be included
     */
    void UpdateHarqProcessStatus(HistoryMap* map,
                                 uint16_t rnti,
                                 uint8_t harqProcId,
                                 const Ptr<NrErrorModelOutput>& output) const;
    /**
     * \brief Return the HARQ history of a particular process id
     * \param rnti the RNTI
     * \param id the HARQ process id
     * \param map the Map between RNTIs and their history
     * \return the HARQ history of such process id
     */
    const NrErrorModel::NrErrorModelHistory& GetHarqProcessInfo(HistoryMap* map,
                                                                uint16_t rnti,
                                                                uint8_t harqProcId) const;

  private:
    HistoryMap m_dlHistory; //!< HARQ history map for DL
    HistoryMap m_ulHistory; //!< HARQ history map for UL
};

} // namespace ns3

#endif /* NR_HARQ_PHY_MODULE_H */

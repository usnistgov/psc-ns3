// Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
// Copyright (c) 2013 Budiarto Herman
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Original work authors (from lte-enb-rrc.cc):
//   Nicola Baldo <nbaldo@cttc.es>
//   Marco Miozzo <mmiozzo@cttc.es>
//   Manuel Requena <manuel.requena@cttc.es>
//
// Converted to handover algorithm interface by:
//   Budiarto Herman <budiarto.herman@magister.fi>

#ifndef A2_A4_RSRQ_HANDOVER_ALGORITHM_H
#define A2_A4_RSRQ_HANDOVER_ALGORITHM_H

#include "nr-handover-algorithm.h"
#include "nr-handover-management-sap.h"
#include "nr-rrc-sap.h"

#include <ns3/ptr.h>
#include <ns3/simple-ref-count.h>

#include <map>

namespace ns3
{

/**
 * \brief Handover algorithm implementation based on RSRQ measurements, Event
 *        A2 and Event A4.
 *
 * Handover decision made by this algorithm is primarily based on Event A2
 * measurements (serving cell's RSRQ becomes worse than threshold). When the
 * event is triggered, the first condition of handover is fulfilled.
 *
 * Event A4 measurements (neighbour cell's RSRQ becomes better than threshold)
 * are used to detect neighbouring cells and their respective RSRQ. When a
 * neighbouring cell's RSRQ is higher than the serving cell's RSRQ by a certain
 * offset, then the second condition of handover is fulfilled.
 *
 * When the first and second conditions above are fulfilled, the algorithm
 * informs the eNodeB RRC to trigger a handover.
 *
 * The threshold for Event A2 can be configured in the `ServingCellThreshold`
 * attribute. The offset used in the second condition can also be configured by
 * setting the `NeighbourCellOffset` attribute.
 *
 * The following code snippet is an example of using and configuring the
 * handover algorithm in a simulation program:
 *
 *     Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
 *
 *     NodeContainer gnbNodes;
 *     // configure the nodes here...
 *
 *     nrHelper->SetHandoverAlgorithmType ("ns3::NrA2A4RsrqHandoverAlgorithm");
 *     nrHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold",
 *                                               UintegerValue (30));
 *     nrHelper->SetHandoverAlgorithmAttribute ("NeighbourCellOffset",
 *                                               UintegerValue (1));
 *     NetDeviceContainer nrGnbDevs = nrHelper->InstallGnbDevice (gnbNodes);
 *
 * \note Setting the handover algorithm type and attributes after the call to
 *       NrHelper::InstallGnbDevice does not have any effect to the devices
 *       that have already been installed.
 */
class NrA2A4RsrqHandoverAlgorithm : public NrHandoverAlgorithm
{
  public:
    /// Creates an A2-A4-RSRQ handover algorithm instance.
    NrA2A4RsrqHandoverAlgorithm();

    ~NrA2A4RsrqHandoverAlgorithm() override;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    // inherited from NrHandoverAlgorithm
    void SetNrHandoverManagementSapUser(NrHandoverManagementSapUser* s) override;
    NrHandoverManagementSapProvider* GetNrHandoverManagementSapProvider() override;

    /// let the forwarder class access the protected and private members
    friend class MemberNrHandoverManagementSapProvider<NrA2A4RsrqHandoverAlgorithm>;

  protected:
    // inherited from Object
    void DoInitialize() override;
    void DoDispose() override;

    // inherited from NrHandoverAlgorithm as a Handover Management SAP implementation
    void DoReportUeMeas(uint16_t rnti, NrRrcSap::MeasResults measResults) override;

  private:
    /**
     * Called when Event A2 is detected, then trigger a handover if needed.
     *
     * \param rnti The RNTI of the UE who reported the event.
     * \param servingCellRsrq The RSRQ of this cell as reported by the UE.
     */
    void EvaluateHandover(uint16_t rnti, uint8_t servingCellRsrq);

    /**
     * Determines if a neighbour cell is a valid destination for handover.
     * Currently always return true.
     *
     * \param cellId The cell ID of the neighbour cell.
     * \return True if the cell is a valid destination for handover.
     */
    bool IsValidNeighbour(uint16_t cellId);

    /**
     * Called when Event A4 is reported, then update the measurements table.
     * If the RNTI and/or cell ID is not found in the table, a corresponding
     * entry will be created. Only the latest measurements are stored in the
     * table.
     *
     * \param rnti The RNTI of the UE who reported the event.
     * \param cellId The cell ID of the measured cell.
     * \param rsrq The RSRQ of the cell as measured by the UE.
     */
    void UpdateNeighbourMeasurements(uint16_t rnti, uint16_t cellId, uint8_t rsrq);

    /// The expected measurement identities for A2 measurements.
    std::vector<uint8_t> m_a2MeasIds;
    /// The expected measurement identities for A4 measurements.
    std::vector<uint8_t> m_a4MeasIds;

    /**
     * Measurements reported by a UE for a cell ID. The values are quantized
     * according 3GPP TS 36.133 section 9.1.4 and 9.1.7.
     */
    class UeMeasure : public SimpleRefCount<UeMeasure>
    {
      public:
        uint16_t m_cellId; ///< Cell ID.
        uint8_t m_rsrp;    ///< RSRP in quantized format. \todo Can be removed?
        uint8_t m_rsrq;    ///< RSRQ in quantized format.
    };

    /**
     * Measurements reported by a UE for several cells. The structure is a map
     * indexed by the cell ID.
     */
    typedef std::map<uint16_t, Ptr<UeMeasure>> MeasurementRow_t;

    /**
     * Measurements reported by several UEs. The structure is a map indexed by
     * the RNTI of the UE.
     */
    typedef std::map<uint16_t, MeasurementRow_t> MeasurementTable_t;

    /// Table of measurement reports from all UEs.
    MeasurementTable_t m_neighbourCellMeasures;

    /**
     * The `ServingCellThreshold` attribute. If the RSRQ of the serving cell is
     * worse than this threshold, neighbour cells are consider for handover.
     * Expressed in quantized range of [0..34] as per Section 9.1.7 of
     * 3GPP TS 36.133.
     */
    uint8_t m_servingCellThreshold;

    /**
     * The `NeighbourCellOffset` attribute. Minimum offset between the serving
     * and the best neighbour cell to trigger the handover. Expressed in
     * quantized range of [0..34] as per Section 9.1.7 of 3GPP TS 36.133.
     */
    uint8_t m_neighbourCellOffset;

    /// Interface to the eNodeB RRC instance.
    NrHandoverManagementSapUser* m_handoverManagementSapUser;
    /// Receive API calls from the eNodeB RRC instance.
    NrHandoverManagementSapProvider* m_handoverManagementSapProvider;

}; // end of class NrA2A4RsrqHandoverAlgorithm

} // end of namespace ns3

#endif /* A2_A4_RSRQ_HANDOVER_ALGORITHM_H */

// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#pragma once

#include "nr-mac-scheduler-tdma.h"

#include <ns3/traced-value.h>

namespace ns3
{

/**
 * \ingroup scheduler
 * \brief The base for all the OFDMA schedulers
 *
 * An example of OFDMA-based scheduling is the following:
 * <pre>
 * (f)
 * ^
 * |=|======|=======|=|
 * |C| U  E | U  E  |C|
 * |T|  1   |  3    |T|
 * | |======|=======| |
 * |R| U  E | U  E  |R|
 * |L|  2   |   4   |L|
 * |----------------------------> (t)
 * </pre>
 *
 * The UEs are scheduled by prioritizing the assignment of frequencies: the entire
 * available spectrum is divided among UEs of the same beam, by a number of
 * symbols which is pre-computed and depends on the total byte to transmit
 * of each beam.
 *
 * The OFDMA scheduling is only done in downlink. In uplink, the division in
 * time is used, and therefore the class is based on top of NrMacSchedulerTdma.
 *
 * The implementation details to construct a slot like the one showed before
 * are in the functions AssignDLRBG() and AssignULRBG().
 * The choice of the UEs to be scheduled is, however, demanded to the subclasses.
 *
 * The DCI is created by CreateDlDci() or CreateUlDci(), which call CreateDci()
 * to perform the "hard" work.
 *
 * \see NrMacSchedulerOfdmaRR
 * \see NrMacSchedulerOfdmaPF
 * \see NrMacSchedulerOfdmaMR
 */
class NrMacSchedulerOfdma : public NrMacSchedulerTdma
{
  public:
    /**
     * \brief GetTypeId
     * \return The TypeId of the class
     */
    static TypeId GetTypeId();

    /**
     * \brief NrMacSchedulerOfdma constructor
     */
    NrMacSchedulerOfdma();

    /**
     * \brief Deconstructor
     */
    ~NrMacSchedulerOfdma() override
    {
    }

  protected:
    BeamSymbolMap AssignDLRBG(uint32_t symAvail, const ActiveUeMap& activeDl) const override;
    BeamSymbolMap AssignULRBG(uint32_t symAvail, const ActiveUeMap& activeUl) const override;

    std::shared_ptr<DciInfoElementTdma> CreateDlDci(
        PointInFTPlane* spoint,
        const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
        uint32_t maxSym) const override;
    std::shared_ptr<DciInfoElementTdma> CreateUlDci(
        PointInFTPlane* spoint,
        const std::shared_ptr<NrMacSchedulerUeInfo>& ueInfo,
        uint32_t maxSym) const override;

    /**
     * \brief Advance the starting point by the number of symbols specified,
     * resetting the RB count to 0
     * \param spoint Starting point
     * \param symOfBeam Number of symbols for the beam
     */
    void ChangeDlBeam(PointInFTPlane* spoint, uint32_t symOfBeam) const override;

    void ChangeUlBeam(PointInFTPlane* spoint, uint32_t symOfBeam) const override;

    NrMacSchedulerOfdma::BeamSymbolMap GetSymPerBeam(uint32_t symAvail,
                                                     const ActiveUeMap& activeDl) const;

    uint8_t GetTpc() const override;

  private:
    TracedValue<uint32_t> m_tracedValueSymPerBeam;
};
} // namespace ns3

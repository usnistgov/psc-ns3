// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_EESM_IR_T1_H
#define NR_EESM_IR_T1_H

#include "nr-eesm-ir.h"
#include "nr-eesm-t1.h"

namespace ns3
{

/**
 * \ingroup error-models
 * \brief The NrEesmIrT1 class
 *
 * Class that implements the IR-HARQ combining with Table 1. It can be used
 * directly in the code.
 */
class NrEesmIrT1 : public NrEesmIr
{
  public:
    /**
     * \brief Get the type id of the object
     * \return the type id of the object
     */
    static TypeId GetTypeId();

    /**
     * \brief NrEesmIrT1 constructor
     */
    NrEesmIrT1();
    /**
     * \brief ~NrEesmIrT1 deconstructor
     */
    ~NrEesmIrT1() override;

  protected:
    // inherited
    const std::vector<double>* GetBetaTable() const override;
    const std::vector<double>* GetMcsEcrTable() const override;
    const SimulatedBlerFromSINR* GetSimulatedBlerFromSINR() const override;
    const std::vector<uint8_t>* GetMcsMTable() const override;
    const std::vector<double>* GetSpectralEfficiencyForMcs() const override;
    const std::vector<double>* GetSpectralEfficiencyForCqi() const override;

  private:
    NrEesmT1 m_t1; //!< The reference table
};

} // namespace ns3

#endif // NR_EESM_IR_T1_H

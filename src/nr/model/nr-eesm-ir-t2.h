// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_EESM_IR_T2_H
#define NR_EESM_IR_T2_H

#include "nr-eesm-ir.h"
#include "nr-eesm-t2.h"

namespace ns3
{

/**
 * \brief The NrEesmIrT2 class
 * \ingroup error-models
 *
 * Class that implements the IR-HARQ combining with Table 2. It can be used
 * directly in the code.
 */
class NrEesmIrT2 : public NrEesmIr
{
  public:
    /**
     * \brief Get the type id of the object
     * \return the type id of the object
     */
    static TypeId GetTypeId();

    /**
     * \brief NrEesmIrT2 constructor
     */
    NrEesmIrT2();
    /**
     * \brief ~NrEesmIrT2 deconstructor
     */
    ~NrEesmIrT2() override;

  protected:
    const std::vector<double>* GetBetaTable() const override;
    const std::vector<double>* GetMcsEcrTable() const override;
    const SimulatedBlerFromSINR* GetSimulatedBlerFromSINR() const override;
    const std::vector<uint8_t>* GetMcsMTable() const override;
    const std::vector<double>* GetSpectralEfficiencyForMcs() const override;
    const std::vector<double>* GetSpectralEfficiencyForCqi() const override;

  private:
    NrEesmT2 m_t2; //!< The reference table
};

} // namespace ns3
#endif // NR_EESM_IR_T2_H

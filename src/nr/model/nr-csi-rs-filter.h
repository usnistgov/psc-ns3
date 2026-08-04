// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef NR_CSI_RS_FILTER_H
#define NR_CSI_RS_FILTER_H

#include <ns3/spectrum-transmit-filter.h>

namespace ns3
{

class NrCsiRsFilter : public SpectrumTransmitFilter
{
  public:
    NrCsiRsFilter();

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief Ignore the signal being received if the receiving SpectrumPhy is
     * not of type NrSpectrumPhy, and if the NrSpectrumPhy is not belonging to UE
     * device, and it the CSI-RS signal is not intended for this UE. Whether
     * CSI-RS signal is intended for the UE is being determined based on either
     * RNTI or beamId, which can be configured through an attribute of
     * NrCsiRsFilter.
     *
     * \param params the parameters of the signals being received
     * \param receiverPhy the SpectrumPhy of the receiver
     * \return whether the CSI-RS signal being received should be ignored
     */

    bool DoFilter(Ptr<const SpectrumSignalParameters> params,
                  Ptr<const SpectrumPhy> receiverPhy) override;

  protected:
    int64_t DoAssignStreams(int64_t stream) override;
};

} // namespace ns3

#endif

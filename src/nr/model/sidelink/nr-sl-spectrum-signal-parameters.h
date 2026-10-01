// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

#ifndef NR_SL_SPECTRUM_SIGNAL_PARAMETERS_H
#define NR_SL_SPECTRUM_SIGNAL_PARAMETERS_H

#include <ns3/nr-spectrum-signal-parameters.h>

#include <list>

namespace ns3
{

class PacketBurst;
class NrControlMessage;
class NrSlHarqFeedbackMessage;

/**
 * \ingroup gnb-phy
 * \ingroup ue-phy
 *
 * Signal parameters for NR SL Frame
 */
struct NrSpectrumSignalParametersSlFrame : public SpectrumSignalParameters
{
    // inherited from SpectrumSignalParameters
    Ptr<SpectrumSignalParameters> Copy() const override;

    /**
     * \brief NrSlSpectrumSignalParametersSlFrame default constructor
     */
    NrSpectrumSignalParametersSlFrame();

    /**
     * \brief NrSlSpectrumSignalParametersSlFrame copy constructor
     * \param p The NrSlSpectrumSignalParametersSlFrame
     */
    NrSpectrumSignalParametersSlFrame(const NrSpectrumSignalParametersSlFrame& p);

    Ptr<PacketBurst> packetBurst; //!< The packet burst being transmitted with this signal
    uint32_t nodeId{std::numeric_limits<uint32_t>::max()}; //!< Node id
                                                           // TODO
    // uint64_t slssId; //!< The Sidelink synchronization signal identifier of the transmitting UE
};

/**
 * \ingroup gnb-phy
 * \ingroup ue-phy
 *
 * Signal parameters for NR SL CTRL Frame (PSCCH)
 */
struct NrSpectrumSignalParametersSlCtrlFrame : public NrSpectrumSignalParametersSlFrame
{
    // inherited from SpectrumSignalParameters
    Ptr<SpectrumSignalParameters> Copy() const override;

    /**
     * NrSlSpectrumSignalParametersSlCtrlFrame default constructor
     */
    NrSpectrumSignalParametersSlCtrlFrame();

    /**
     * \brief NrSlSpectrumSignalParametersSlCtrlFrame copy constructor
     * \param p The NrSlSpectrumSignalParametersSlFrame
     */
    NrSpectrumSignalParametersSlCtrlFrame(const NrSpectrumSignalParametersSlCtrlFrame& p);
};

/**
 * \ingroup gnb-phy
 * \ingroup ue-phy
 *
 * Signal parameters for NR SL DATA Frame (PSSCH)
 */
struct NrSpectrumSignalParametersSlDataFrame : public NrSpectrumSignalParametersSlFrame
{
    // inherited from SpectrumSignalParameters
    Ptr<SpectrumSignalParameters> Copy() const override;

    /**
     * \brief NrSlSpectrumSignalParametersSlDataFrame default constructor
     */
    NrSpectrumSignalParametersSlDataFrame();

    /**
     * \brief NrSlSpectrumSignalParametersSlDataFrame copy constructor
     * \param p The NrSlSpectrumSignalParametersSlFrame
     */
    NrSpectrumSignalParametersSlDataFrame(const NrSpectrumSignalParametersSlDataFrame& p);
};

/**
 * \ingroup gnb-phy
 * \ingroup ue-phy
 *
 * Signal parameters for NR SL feedback (PSFCH)
 */
struct NrSpectrumSignalParametersSlFeedback : public NrSpectrumSignalParametersSlFrame
{
    // inherited from SpectrumSignalParameters
    Ptr<SpectrumSignalParameters> Copy() const override;

    /**
     * \brief NrSlSpectrumSignalParametersSlFeedback default constructor
     */
    NrSpectrumSignalParametersSlFeedback();

    /**
     * \brief NrSlSpectrumSignalParametersSlFeedback copy constructor
     * \param p The NrSlSpectrumSignalParametersSlFrame
     */
    NrSpectrumSignalParametersSlFeedback(const NrSpectrumSignalParametersSlFeedback& p);
    std::list<Ptr<NrSlHarqFeedbackMessage>> feedbackList; //!< Feedback message list
};

} // namespace ns3

#endif /* NR_SL_SPECTRUM_SIGNAL_PARAMETERS_H */

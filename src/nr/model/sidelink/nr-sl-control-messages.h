// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

#ifndef NR_SL_CONTROL_MESSAGES_H_
#define NR_SL_CONTROL_MESSAGES_H_

#include "nr-sl-phy-mac-common.h"

#include "ns3/nr-control-messages.h"

namespace ns3
{
/**
 * \ingroup utils
 * \brief SlHarqFeedback message
 *
 * The sidelink NrSlHarqFeedbackMessage defines the specific
 * messages for transmitting the SL HARQ feedback through PSFCH
 */
class NrSlHarqFeedbackMessage : public NrControlMessage
{
  public:
    /**
     * \brief NrSlHarqFeedbackMessage constructor
     */
    NrSlHarqFeedbackMessage();
    /**
     * \brief ~NrSlHarqFeedbackMessage
     */
    ~NrSlHarqFeedbackMessage() override = default;

    /**
     * \brief add a SL HARQ feedback record into the message.
     * \param m the SL HARQ feedback
     */
    void SetSlHarqFeedback(SlHarqInfo m);

    /**
     * \brief Get SL HARQ informations
     * \return SL HARQ message
     */
    SlHarqInfo GetSlHarqFeedback(void);

  private:
    SlHarqInfo m_slHarqInfo; //!< SL Harq Info
};

} // namespace ns3

#endif /* NR_SL_CONTROL_MESSAGES_H_ */

// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only and NIST-Software

#include "nr-sl-control-messages.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlControlMessages");

NrSlHarqFeedbackMessage::NrSlHarqFeedbackMessage(void)
{
    SetMessageType(NrControlMessage::SL_HARQ);
}

void
NrSlHarqFeedbackMessage::SetSlHarqFeedback(SlHarqInfo m)
{
    m_slHarqInfo = m;
}

SlHarqInfo
NrSlHarqFeedbackMessage::GetSlHarqFeedback(void)
{
    return m_slHarqInfo;
}

} // namespace ns3

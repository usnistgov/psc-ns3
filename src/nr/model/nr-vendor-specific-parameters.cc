// Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Marco Miozzo  <marco.miozzo@cttc.es>
///

#include "nr-vendor-specific-parameters.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrVendorSpecificParameters");

NrSrsCqiRntiVsp::NrSrsCqiRntiVsp(uint16_t rnti)
    : m_rnti(rnti)
{
}

NrSrsCqiRntiVsp::~NrSrsCqiRntiVsp()
{
}

uint16_t
NrSrsCqiRntiVsp::GetRnti() const
{
    return m_rnti;
}

} // namespace ns3

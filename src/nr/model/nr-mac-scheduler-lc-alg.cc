// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-mac-scheduler-lc-alg.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrMacSchedulerLcAlgorithm");
NS_OBJECT_ENSURE_REGISTERED(NrMacSchedulerLcAlgorithm);

NrMacSchedulerLcAlgorithm::NrMacSchedulerLcAlgorithm()
    : Object()
{
    NS_LOG_FUNCTION(this);
}

NrMacSchedulerLcAlgorithm::~NrMacSchedulerLcAlgorithm()
{
}

TypeId
NrMacSchedulerLcAlgorithm::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrMacSchedulerLcAlgorithm").SetParent<Object>();
    return tid;
}

} // namespace ns3

// Copyright (c) 2013 Budiarto Herman
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Budiarto Herman <budiarto.herman@magister.fi>

#include "nr-handover-algorithm.h"

#include <ns3/log.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrHandoverAlgorithm");

NS_OBJECT_ENSURE_REGISTERED(NrHandoverAlgorithm);

NrHandoverAlgorithm::NrHandoverAlgorithm()
{
}

NrHandoverAlgorithm::~NrHandoverAlgorithm()
{
}

TypeId
NrHandoverAlgorithm::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrHandoverAlgorithm").SetParent<Object>().SetGroupName("Nr");
    return tid;
}

void
NrHandoverAlgorithm::DoDispose()
{
}

} // end of namespace ns3

// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-cb-type-one.h"

#include <ns3/boolean.h>
#include <ns3/uinteger.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrCbTypeOne");
NS_OBJECT_ENSURE_REGISTERED(NrCbTypeOne);

TypeId
NrCbTypeOne::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrCbTypeOne")
                            .SetParent<Object>()
                            .AddAttribute("N1",
                                          "N1 (number of horizontal ports at the gNB)",
                                          UintegerValue(NR_CB_TYPE_ONE_INIT_N1),
                                          MakeUintegerAccessor(&NrCbTypeOne::m_n1),
                                          MakeUintegerChecker<uint8_t>())
                            .AddAttribute("N2",
                                          "N2 (number of vertical ports at the gNB)",
                                          UintegerValue(NR_CB_TYPE_ONE_INIT_N2),
                                          MakeUintegerAccessor(&NrCbTypeOne::m_n2),
                                          MakeUintegerChecker<uint8_t>())
                            .AddAttribute("IsDualPol",
                                          "True if the gNB antennas are dual-polarized",
                                          BooleanValue(NR_CB_TYPE_ONE_INIT_POL),
                                          MakeBooleanAccessor(&NrCbTypeOne::m_isDualPol),
                                          MakeBooleanChecker())
                            .AddAttribute("Rank",
                                          "Rank (number of MIMO layers)",
                                          UintegerValue(NR_CB_TYPE_ONE_INIT_RANK),
                                          MakeUintegerAccessor(&NrCbTypeOne::m_rank),
                                          MakeUintegerChecker<uint8_t>());
    return tid;
}

size_t
NrCbTypeOne::GetNumI1() const
{
    return m_numI1;
}

size_t
NrCbTypeOne::GetNumI2() const
{
    return m_numI2;
}

} // namespace ns3

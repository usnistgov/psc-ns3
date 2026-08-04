// Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>

#include "nr-rlc-sequence-number.h"

namespace ns3
{
namespace nr
{
/**
 * Ostream output function
 * \param os the output stream
 * \param val the sequence number
 * \returns the os
 */
std::ostream&
operator<<(std::ostream& os, const nr::SequenceNumber10& val)
{
    os << val.GetValue();
    return os;
}

} // namespace nr
} // namespace ns3

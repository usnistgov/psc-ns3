/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#ifndef NIST_LTE_RLC_SEQUENCE_NUMBER_H
#define NIST_LTE_RLC_SEQUENCE_NUMBER_H

#include <limits>
#include <iostream>
#include <stdint.h>

// #include "ns3/nist-lte-rlc.h"

namespace ns3 {


class NistSequenceNumber10
{
public:
  NistSequenceNumber10 ()
    : m_value (0),
      m_modulusBase (0)
  {}

  explicit NistSequenceNumber10 (uint16_t value)
    : m_value (value % 1024),
      m_modulusBase (0)
  {}

  NistSequenceNumber10 (NistSequenceNumber10 const &value)
    : m_value (value.m_value),
      m_modulusBase (value.m_modulusBase)
  {}

  NistSequenceNumber10& operator= (uint16_t value)
  {
    m_value = value % 1024;
    return *this;
  }


  /**
   * \brief Extracts the numeric value of the sequence number
   * \returns the sequence number value
   */
  uint16_t GetValue () const
  {
    return m_value;
  }

  void SetModulusBase (NistSequenceNumber10 modulusBase)
  {
    m_modulusBase = modulusBase.m_value;
  }

  void SetModulusBase (uint16_t modulusBase)
  {
    m_modulusBase = modulusBase;
  }

   // postfix ++
  NistSequenceNumber10 operator++ (int)
  {
    NistSequenceNumber10 retval (m_value);
    m_value = ((uint32_t)m_value + 1) % 1024;
    return retval;
  }

  NistSequenceNumber10 operator + (uint16_t delta) const
  {
    NistSequenceNumber10 ret ((m_value + delta) % 1024);
    ret.SetModulusBase (m_modulusBase);
    return ret;
  }

  NistSequenceNumber10 operator - (uint16_t delta) const
  {
    NistSequenceNumber10 ret ((m_value - delta) % 1024);
    ret.SetModulusBase (m_modulusBase);
    return ret;
  }

  uint16_t operator - (const NistSequenceNumber10 &other) const
  {
    uint16_t diff = m_value - other.m_value;
    return (diff);
  }

  bool operator > (const NistSequenceNumber10 &other) const
  {
    NistSequenceNumber10 v1 ((m_value - m_modulusBase) % 1024);
    NistSequenceNumber10 v2 ((other.m_value - other.m_modulusBase) % 1024);
    return ( v1.GetValue () > v2.GetValue () );
  }

  bool operator == (const NistSequenceNumber10 &other) const
  {
    return (m_value == other.m_value);
  }

  bool operator != (const NistSequenceNumber10 &other) const
  {
    return (m_value != other.m_value);
  }

  bool operator <= (const NistSequenceNumber10 &other) const
  {
    return (!this->operator> (other));
  }

  bool operator >= (const NistSequenceNumber10 &other) const
  {
    return (this->operator> (other) || this->operator== (other));
  }

  bool operator < (const NistSequenceNumber10 &other) const
  {
    return !this->operator> (other) && m_value != other.m_value;
  }


  friend std::ostream & operator<< (std::ostream& os, const NistSequenceNumber10 &val);

private:
  uint16_t m_value;
  uint16_t m_modulusBase;
};


} // namespace ns3

#endif // NIST_LTE_RLC_SEQUENCE_NUMBER_H

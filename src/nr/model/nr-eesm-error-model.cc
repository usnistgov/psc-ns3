/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
*   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
*   Copyright (c) 2019 Interdigital <kevin.wanuga@interdigital.com> tables
*   BlerForSinr1 and BlerForSinr2
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation;
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/

#include "nr-eesm-error-model.h"
#include "ns3/log.h"
#include <cmath>
#include <algorithm>
#include "ns3/enum.h"
#include "nr-phy-mac-common.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NrEesmErrorModel");
NS_OBJECT_ENSURE_REGISTERED (NrEesmErrorModel);

/**
 * \brief Table of Lifting Sizes for LDPC
 */
static const std::vector<uint16_t> LiftingSizeTableBG = {
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 20, 22, 24, 26, 28, 30,
  32, 36, 40, 44, 48, 52, 56, 60, 64, 72, 80, 88, 96, 104, 112, 120, 128, 144,
  160, 176, 192, 208, 224, 240, 256, 288, 320, 352, 384
};

std::vector<std::string>
NrEesmErrorModel::m_bgTypeName = { "BG1" , "BG2" };

NrEesmErrorModel::NrEesmErrorModel () : NrErrorModel ()
{
  NS_LOG_FUNCTION (this);
}

NrEesmErrorModel::~NrEesmErrorModel ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
NrEesmErrorModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NrEesmErrorModel")
    .SetParent<NrErrorModel> ()
  ;
  return tid;
}

TypeId
NrEesmErrorModel::GetInstanceTypeId() const
{
  return NrEesmErrorModel::GetTypeId ();
}

double
NrEesmErrorModel::SinrEff (const SpectrumValue& sinr, const std::vector<int>& map, uint8_t mcs, double a, double b) const
{
  // it follows: SINReff = - beta * ln [1/b * (sum (exp (-sinr/beta)) + a)]
  // for HARQ-IR: b = sum (map.size()), a = sum_j(sum_n (exp (-sinr/beta))) (for previous retx, till j=q-1)
  // for HARQ-CC: b = map.size(), a = 0.0 (SINRs are already combined in sinr input)

  double sinrExpSum = SinrExp (sinr, map, mcs);
  double beta = GetBetaTable ()->at (mcs);
  double SINR = -beta * log ((a + sinrExpSum)/b);

  NS_LOG_INFO (" Effective SINR = " << SINR);

  return SINR;
}

double
NrEesmErrorModel::SinrExp (const SpectrumValue& sinr, const std::vector<int>& map, uint8_t mcs) const
{
  // it returns sum_n (exp (-SINR/beta))
  NS_LOG_FUNCTION (sinr << &map << (uint8_t) mcs);
  NS_ABORT_MSG_IF (map.size () == 0,
                   " Error: number of allocated RBs cannot be 0 - EESM method - SinrEff function");

  double SINRexp = 0.0;
  double SINRsum = 0.0;
  double beta = GetBetaTable ()->at (mcs);
  SpectrumValue sinrCopy = sinr;
  for (uint32_t i = 0; i < map.size (); i++)
    {
      double sinrLin = sinrCopy [map.at (i)];
      SINRexp = exp (-sinrLin / beta);
      SINRsum += SINRexp;
    }
  return SINRsum;
}

const std::vector<double> &
NrEesmErrorModel::GetSinrDbVectorFromSimulatedValues (NrEesmErrorModel::GraphType graphType,
                                                      uint8_t mcs, uint32_t cbSizeIndex) const
{
  return std::get<0> (GetSimulatedBlerFromSINR ()->at (graphType).at (mcs).at (cbSizeIndex));
}

const std::vector<double> &
NrEesmErrorModel::GetBLERVectorFromSimulatedValues (NrEesmErrorModel::GraphType graphType,
                                                    uint8_t mcs, uint32_t cbSizeIndex) const
{
  return std::get<1> (GetSimulatedBlerFromSINR ()->at (graphType).at (mcs).at (cbSizeIndex));
}

double
NrEesmErrorModel::MappingSinrBler (double sinr, uint8_t mcs, uint32_t cbSizeBit)
{
  NS_LOG_FUNCTION (sinr << (uint8_t) mcs << (uint32_t) cbSizeBit);
  NS_ABORT_MSG_IF (mcs > GetMaxMcs (), "MCS out of range [0..27/28]: " << static_cast<uint8_t> (mcs));

  // use cbSize to obtain the index of CBSIZE in the map, jointly with mcs and sinr. take the
  // lowest CBSIZE simulated including this CB for removing CB size quatization
  // errors. sinr is also lower-bounded.
  double bler = 0.0;
  double sinr_db = 10 * log10 (sinr);
  GraphType bg_type = GetBaseGraphType (cbSizeBit, mcs);

  // Get the index of CBSIZE in the map
  NS_LOG_INFO ("For sinr " << sinr << " and mcs " << +mcs <<
                " CbSizebit " << cbSizeBit << " we got bg type " << m_bgTypeName[bg_type]);
  auto cbMap = GetSimulatedBlerFromSINR ()->at (bg_type).at (mcs);
  auto cbIt = cbMap.upper_bound (cbSizeBit);

  if (cbIt != cbMap.begin ())
    {
      cbIt--;
    }

  if (sinr_db < GetSinrDbVectorFromSimulatedValues (bg_type, mcs, cbIt->first).front ())
    {
      bler = 1.0;
    }
  else if (sinr_db > GetSinrDbVectorFromSimulatedValues (bg_type, mcs, cbIt->first).back ())
    {
      bler = 0.0;
    }
  else
    {
      // Get the index of SINR in the vector
      auto sinrIt = std::upper_bound (GetSinrDbVectorFromSimulatedValues (bg_type, mcs, cbIt->first).begin (),
                                      GetSinrDbVectorFromSimulatedValues (bg_type, mcs, cbIt->first).end (),
                                      sinr_db);

      if (sinrIt != GetSinrDbVectorFromSimulatedValues (bg_type, mcs, cbIt->first).begin ())
        {
          sinrIt--;
        }

      auto sinr_index = std::distance (GetSinrDbVectorFromSimulatedValues (bg_type, mcs, cbIt->first).begin (),
                                       sinrIt);
      bler = GetBLERVectorFromSimulatedValues (bg_type, mcs, cbIt->first).at (sinr_index);
    }

  NS_LOG_LOGIC ("SINR effective: " << sinr << " BLER:" << bler);
  return bler;
}

NrEesmErrorModel::GraphType
NrEesmErrorModel::GetBaseGraphType (uint32_t tbSizeBit, uint8_t mcs) const
{
  double ecr = GetMcsEcrTable ()->at (mcs);

  GraphType bg_type = FIRST;
  if (tbSizeBit <= 292 || ecr <= 0.25 || (tbSizeBit <= 3824 && ecr <= 0.67))
    {
      bg_type = SECOND;
    }
  return bg_type;
}

std::pair<uint32_t, uint32_t>
NrEesmErrorModel::CodeBlockSegmentation (uint32_t B, GraphType bg_type) const
{
  uint16_t Kcb;
  uint8_t Kb;
  if (bg_type == FIRST)
    {
      Kcb = 8448;  // max size of a codeblock (including CRC) for BG1
      Kb = 22;
    }
  else
    {
      NS_ASSERT (bg_type == SECOND);
      Kcb = 3840;  // max size of a codeblock (including CRC) for BG2
      if (B >= 640)
        {
          Kb = 10;
        }
      else if (B >= 560)
        {
          Kb = 9;
        }
      else if (B >= 192)
        {
          Kb = 8;
        }
      else
        {
          Kb = 6;
        }
    }

  uint32_t L = 0;
  uint32_t C = 0; // no. of codeblocks
  uint32_t B1 = 0;

  if (B <= Kcb)
    {
      // only one codeblock
      L = 0;
      C = 1;
      B1 = B;
    }
  else
    {
      L = 24;
      C = ceil ((double)B / ((double)(Kcb - L)));
      B1 = B + C * L;
    }

  // Zc = minimum Z in all sets of lifting sizes table such that Kb * Z >= K1
  uint32_t K1 = B1 / C;

  // returns an iterator pointing to the first element in the range [first,last)
  // which compares greater than the third parameter.
  auto ZcIt = std::upper_bound (LiftingSizeTableBG.begin (),
                                --LiftingSizeTableBG.end (),
                                (static_cast<double> (K1) / Kb) + 0.001);
  uint16_t Zc = *ZcIt;
  uint32_t K;

  if (bg_type == FIRST)
    {
      K = Zc * 22;  // no. of bits in each code block
    }
  else  // bg_type==2
    {
      NS_ASSERT (bg_type == SECOND);
      K = Zc * 10;  // no. of bits in each code block
    }

  NS_LOG_INFO ("EESMErrorModel: TBS of " << B << " needs of " << B1 <<
               " bits distributed in " << C << " CBs of " << K << " bits");
  return std::make_pair(K,C);
}

Ptr<NrErrorModelOutput>
NrEesmErrorModel::GetTbDecodificationStats (const SpectrumValue& sinr, const std::vector<int>& map,
                                            uint32_t size, uint8_t mcs,
                                            const NrErrorModelHistory &sinrHistory)
{
  return GetTbBitDecodificationStats (sinr, map, size * 8, mcs, sinrHistory);
}

std::string
NrEesmErrorModel::PrintMap (const std::vector<int> &map) const
{
  std::stringstream ss;

  for (const auto &v : map)
    {
      ss << v << ", ";
    }

  return ss.str ();
}

Ptr<NrErrorModelOutput>
NrEesmErrorModel::GetTbBitDecodificationStats (const SpectrumValue& sinr,
                                               const std::vector<int>& map,
                                               uint32_t sizeBit, uint8_t mcs,
                                               const NrErrorModelHistory &sinrHistory)
{
  NS_LOG_FUNCTION (this);
  NS_ABORT_IF (mcs > GetMaxMcs ());

  double tbSinr = SinrEff (sinr, map, mcs, 0, map.size());  // effective SINR for this tx
  double SINR = tbSinr;
  double sinrExpSum = SinrExp (sinr, map, mcs);  // exponential sum of SINRs for this tx

  NS_LOG_DEBUG (" mcs " << +mcs << " TBSize in bit " << sizeBit <<
                " history elements: " << sinrHistory.size () << " SINR of the tx: " <<
                tbSinr << std::endl << "MAP: " << PrintMap (map) << std::endl <<
                "SINR: " << sinr);

  if (sinrHistory.size () > 0)
    {
      SINR = ComputeSINR (sinr, map, mcs, sizeBit, sinrHistory);
    }

  NS_LOG_DEBUG (" SINR after processing all retx (if any): " << SINR << " SINR last tx" << tbSinr);

  // LDPC base graph type selection (1 or 2), as per TS 38.212, using the payload (A)
  GraphType bg_type = GetBaseGraphType (sizeBit, mcs);
  NS_LOG_INFO ("BG type selection: " << bg_type);

  // code block segmentation, as per TS 38.212, using payload + TB CRC attachment (B)
  uint32_t B = sizeBit + 24; // input to code block segmentation, in bits
  std::pair<uint32_t, uint32_t> cbSeg = CodeBlockSegmentation(B, bg_type);
  uint32_t K = cbSeg.first;
  uint32_t C = cbSeg.second;
  NS_LOG_INFO ("EESMErrorModel: TBS of " << B << " bits distributed in " << C <<
               " CBs of " << K << " bits");

  uint8_t mcs_eq = mcs;
  if ((sinrHistory.size () > 0) && (mcs > 0))
    {
      mcs_eq = GetMcsEq (mcs);
    }

  NS_LOG_INFO (" MCS of tx " << +mcs <<
               " Equivalent MCS for PHY abstraction (just for HARQ-IR) " << +mcs_eq);

  double errorRate = 1.0;
  if (C != 1)
    {
      double cbler = MappingSinrBler (SINR, mcs_eq, K);
      errorRate = 1.0 - pow (1.0 - cbler, C);
    }
  else
    {
      errorRate = MappingSinrBler (SINR, mcs_eq, K);
    }

  NS_LOG_DEBUG ("Calculated Error rate " << errorRate);
  NS_ASSERT (GetMcsEcrTable () != nullptr);

  Ptr<NrEesmErrorModelOutput> ret = Create<NrEesmErrorModelOutput> (errorRate);
  ret->m_sinrEff = SINR;
  ret->m_sinr = sinr;
  ret->m_map = map;
  if (sinrHistory.size () == 0)
    {
      ret->m_sinrExp =  sinrExpSum;  // it is first tx!
    }
  else
    {
      double m_sinrExpPrevious = DynamicCast<NrEesmErrorModelOutput> (sinrHistory.back ())->m_sinrExp;
      ret->m_sinrExp = m_sinrExpPrevious + sinrExpSum;  // it sums over previous tx (recursively)
    }
  ret->m_infoBits = sizeBit;
  ret->m_codeBits = sizeBit / GetMcsEcrTable ()->at (mcs);

  return ret;
}

double
NrEesmErrorModel::GetSpectralEfficiencyForCqi (uint8_t cqi)
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (GetSpectralEfficiencyForCqi () != nullptr);
  NS_ABORT_MSG_UNLESS (cqi >= 0 && cqi <= 15, "CQI must be in [0..15] = " << cqi);

  return GetSpectralEfficiencyForCqi ()->at (cqi);
}

double
NrEesmErrorModel::GetSpectralEfficiencyForMcs (uint8_t mcs) const
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (GetSpectralEfficiencyForMcs () != nullptr);
  NS_ABORT_IF (mcs > GetMaxMcs ());

  return GetSpectralEfficiencyForMcs() ->at (mcs);
}

uint32_t
NrEesmErrorModel::GetPayloadSize (uint32_t usefulSc, uint8_t mcs, uint32_t rbNum, [[maybe_unused]] Mode mode) const
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (GetMcsEcrTable () != nullptr);
  const uint32_t rscElement = usefulSc * rbNum;
  double Rcode = GetMcsEcrTable ()->at (mcs);
  uint8_t Qm = GetMcsMTable ()->at (mcs);

  const double spectralEfficiency = rscElement * Qm * Rcode;

  return static_cast<uint32_t> (std::floor (spectralEfficiency / 8));
}

uint32_t
NrEesmErrorModel::GetMaxCbSize (uint32_t tbSize, uint8_t mcs) const
{
  GraphType bgType = GetBaseGraphType (tbSize * 8, mcs);
  if (bgType == FIRST)
    {
      return LiftingSizeTableBG.back () * 22 / 8;  // return CBsize in bytes
    }
  NS_ASSERT (bgType == SECOND);
  return LiftingSizeTableBG.back () * 10 / 8;     // return CBsize in bytes
}

uint8_t
NrEesmErrorModel::GetMaxMcs () const
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT (GetMcsEcrTable () != nullptr);
  NS_LOG_INFO (" Max MCS: " << +(GetMcsEcrTable ()->size () - 1));
  return static_cast<uint8_t> (GetMcsEcrTable ()->size () - 1);
}

} // namespace ns3


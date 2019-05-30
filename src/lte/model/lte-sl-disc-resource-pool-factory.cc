/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in 
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#include "lte-sl-disc-resource-pool-factory.h"
#include "ns3/log.h"


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlDiscResourcePoolFactory");

LteSlDiscResourcePoolFactory::LteSlDiscResourcePoolFactory ()
  : m_discCpLen ("NORMAL"),
    m_period ("rf32"),
    m_numRetx (0),
    m_numRep (1),
    m_discPrbNum (10),
    m_discPrbStart (10),
    m_discPrbEnd (40),
    m_discOffset (0),
    m_discBitmapValue (0x11111),
    m_txParam (true),
    m_txAlpha ("al09"),
    m_discP0 (-40),
    m_poolSelection ("RSRPBASED"),
    m_rsrpBased (true),
    m_rsrpThreshLow (0),
    m_rsrpThreshHigh (7),
    m_discTxProb ("p100"),
    m_rxParam(false)
{
  NS_LOG_FUNCTION (this);
}

LteRrcSap::SlDiscResourcePool
LteSlDiscResourcePoolFactory::CreatePool ()
{
  if (m_discCpLen == "NORMAL")
    {
      m_pool.cpLen.cplen = LteRrcSap::SlCpLen::NORMAL;
    }
  else if (m_discCpLen == "EXTENDED")
    {
      m_pool.cpLen.cplen = LteRrcSap::SlCpLen::EXTENDED;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED CONTROL CP LENGTH");
    }
  if (m_period == "rf32")
    {
      m_pool.discPeriod.period = LteRrcSap::SlPeriodDisc::rf32;
    }
  else if (m_period == "rf64")
    {
      m_pool.discPeriod.period = LteRrcSap::SlPeriodDisc::rf64;
    }
  else if (m_period == "rf128")
    {
      m_pool.discPeriod.period = LteRrcSap::SlPeriodDisc::rf128;
    }
  else if (m_period == "rf256")
    {
      m_pool.discPeriod.period = LteRrcSap::SlPeriodDisc::rf256;
    }
  else if (m_period == "rf512")
    {
      m_pool.discPeriod.period = LteRrcSap::SlPeriodDisc::rf512;
    }
  else if (m_period == "rf1024")
    {
      m_pool.discPeriod.period = LteRrcSap::SlPeriodDisc::rf1024;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED DISCOVERY PERIOD");
    }
  m_pool.numRetx = m_numRetx;
  m_pool.numRepetition = m_numRep;
  m_pool.tfResourceConfig.prbNum = m_discPrbNum;
  m_pool.tfResourceConfig.prbStart = m_discPrbStart;
  m_pool.tfResourceConfig.prbEnd = m_discPrbEnd;
  m_pool.tfResourceConfig.offsetIndicator.offset = m_discOffset;
  m_pool.tfResourceConfig.subframeBitmap.bitmap = std::bitset<40> (m_discBitmapValue);
  m_pool.haveTxParameters = m_txParam;
  if (m_txAlpha == "al0")
    {
      m_pool.txParameters.txParametersGeneral.alpha = LteRrcSap::SlTxParameters::al0;
    }
  else if (m_txAlpha == "al04")
    {
      m_pool.txParameters.txParametersGeneral.alpha = LteRrcSap::SlTxParameters::al04;
    }
  else if (m_txAlpha == "al05")
    {
      m_pool.txParameters.txParametersGeneral.alpha = LteRrcSap::SlTxParameters::al05;
    }
  else if (m_txAlpha == "al06")
    {
      m_pool.txParameters.txParametersGeneral.alpha = LteRrcSap::SlTxParameters::al06;
    }
  else if (m_txAlpha == "al07")
    {
      m_pool.txParameters.txParametersGeneral.alpha = LteRrcSap::SlTxParameters::al07;
    }
  else if (m_txAlpha == "al08")
    {
      m_pool.txParameters.txParametersGeneral.alpha = LteRrcSap::SlTxParameters::al08;
    }
  else if (m_txAlpha == "al09")
    {
      m_pool.txParameters.txParametersGeneral.alpha = LteRrcSap::SlTxParameters::al09;
    }
  else if (m_txAlpha == "al1")
    {
      m_pool.txParameters.txParametersGeneral.alpha = LteRrcSap::SlTxParameters::al1;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED CONTROL TX ALPHA");
    }
  m_pool.txParameters.txParametersGeneral.p0 = m_discP0;
  if (m_poolSelection == "RSRPBASED")
    {
      m_pool.txParameters.ueSelectedResourceConfig.poolSelection.selection = LteRrcSap::PoolSelection::RSRPBASED;
    }
  else if (m_poolSelection == "RANDOM")
    {
      m_pool.txParameters.ueSelectedResourceConfig.poolSelection.selection = LteRrcSap::PoolSelection::RANDOM;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED POOL SELECTION");
    }
  m_pool.txParameters.ueSelectedResourceConfig.havePoolSelectionRsrpBased = m_rsrpBased;
  if (m_rsrpBased && m_rsrpThreshLow <= 7 && m_rsrpThreshHigh <= 7 && m_rsrpThreshLow <= m_rsrpThreshHigh)
    {
      m_pool.txParameters.ueSelectedResourceConfig.poolSelectionRsrpBased.threshLow = m_rsrpThreshLow;
      m_pool.txParameters.ueSelectedResourceConfig.poolSelectionRsrpBased.threshHigh = m_rsrpThreshHigh;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED RSRP THRESHOLD");
    }
  if (m_discTxProb == "p25")
    {
      m_pool.txParameters.ueSelectedResourceConfig.txProbability.probability = LteRrcSap::TxProbability::p25;
    }
  else if (m_discTxProb == "p50")
    {
      m_pool.txParameters.ueSelectedResourceConfig.txProbability.probability = LteRrcSap::TxProbability::p50;
    }
  else if (m_discTxProb == "p75")
    {
      m_pool.txParameters.ueSelectedResourceConfig.txProbability.probability = LteRrcSap::TxProbability::p75;
    }
  else if (m_discTxProb == "p100")
    {
      m_pool.txParameters.ueSelectedResourceConfig.txProbability.probability = LteRrcSap::TxProbability::p100;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED DISCOVERY PROBABILITY");
    }

  m_pool.haveRxParameters = m_rxParam;
  //todo: add other Rx parameters
  
  return m_pool;
}

void
LteSlDiscResourcePoolFactory::SetDiscCpLen (std::string cpLen)
{
  NS_LOG_FUNCTION (this << cpLen);
  m_discCpLen = cpLen;
}

void
LteSlDiscResourcePoolFactory::SetDiscPeriod (std::string period)
{
  NS_LOG_FUNCTION (this << period);
  m_period = period;
}

void
LteSlDiscResourcePoolFactory::SetNumRetx (int8_t numRetx)
{
  NS_LOG_FUNCTION (this << numRetx);
  m_numRetx = numRetx;
}

void
LteSlDiscResourcePoolFactory::SetNumRepetition (int32_t numRep)
{
  NS_LOG_FUNCTION (this << numRep);
  m_numRep = numRep;
}

void
LteSlDiscResourcePoolFactory::SetDiscPrbNum (int32_t prbNum)
{
  NS_LOG_FUNCTION (this << prbNum);
  m_discPrbNum = prbNum;
}

void
LteSlDiscResourcePoolFactory::SetDiscPrbStart (int32_t prbStart)
{
  NS_LOG_FUNCTION (this << prbStart);
  m_discPrbStart = prbStart;
}

void
LteSlDiscResourcePoolFactory::SetDiscPrbEnd (int32_t prbEnd)
{
  NS_LOG_FUNCTION (this << prbEnd);
  m_discPrbEnd = prbEnd;
}

void
LteSlDiscResourcePoolFactory::SetDiscOffset (int32_t offset)
{
  NS_LOG_FUNCTION (this << offset);
  m_discOffset = offset;
}

void
LteSlDiscResourcePoolFactory::SetDiscBitmap (int64_t value)
{
  NS_LOG_FUNCTION (this << value);
  m_discBitmapValue = value;
}

// Tx Parameters
void 
LteSlDiscResourcePoolFactory::SetHaveTxParameters (bool txParam)
{
  NS_LOG_FUNCTION (this << txParam);
  m_txParam = txParam;
}

void 
LteSlDiscResourcePoolFactory::SetDiscTxAlpha (std::string alpha)
{
  NS_LOG_FUNCTION (this << alpha);
  m_txAlpha = alpha;
}

void 
LteSlDiscResourcePoolFactory::SetDiscTxP0 (int32_t p0)
{
  NS_LOG_FUNCTION (this << p0);
  m_discP0 = p0;
}

void 
LteSlDiscResourcePoolFactory::SetDiscPoolSelection (std::string poolSelection)
{
  NS_LOG_FUNCTION (this << poolSelection);
  m_poolSelection = poolSelection;
}

void
LteSlDiscResourcePoolFactory::SetHaveRsrpBasedPoolSelection (bool rsrpBased)
{
  NS_LOG_FUNCTION (this << rsrpBased);
  m_rsrpBased = rsrpBased;
}

void
LteSlDiscResourcePoolFactory::SetRsrpThreshLow (int32_t rsrpThreshLow)
{
  NS_LOG_FUNCTION (this << rsrpThreshLow);
  m_rsrpThreshLow = rsrpThreshLow;
}

void
LteSlDiscResourcePoolFactory::SetRsrpThreshHigh (int32_t rsrpThreshHigh)
{
  NS_LOG_FUNCTION (this << rsrpThreshHigh);
  m_rsrpThreshHigh = rsrpThreshHigh;
}

void
LteSlDiscResourcePoolFactory::SetDiscTxProbability (std::string txProb)
{
  NS_LOG_FUNCTION (this << txProb);
  m_discTxProb = txProb;
}

std::string
LteSlDiscResourcePoolFactory::GetDiscCpLen (void)
{
  NS_LOG_FUNCTION (this << m_discCpLen);
  return m_discCpLen;
}

std::string
LteSlDiscResourcePoolFactory::GetDiscPeriod (void)
{
  NS_LOG_FUNCTION (this << m_period);
  return m_period;
}

int8_t
LteSlDiscResourcePoolFactory::GetNumRetx (void)
{
  NS_LOG_FUNCTION (this << m_numRetx);
  return m_numRetx;
}

int32_t
LteSlDiscResourcePoolFactory::GetNumRepetition (void)
{
  NS_LOG_FUNCTION (this << m_numRep);
  return m_numRep;
}

int32_t
LteSlDiscResourcePoolFactory::GetDiscPrbNum (void)
{
  NS_LOG_FUNCTION (this << m_discPrbNum);
  return m_discPrbNum;
}

int32_t
LteSlDiscResourcePoolFactory::GetDiscPrbStart (void)
{
  NS_LOG_FUNCTION (this << m_discPrbStart);
  return m_discPrbStart;
}

int32_t
LteSlDiscResourcePoolFactory::GetDiscPrbEnd (void)
{
  NS_LOG_FUNCTION (this << m_discPrbEnd);
  return m_discPrbEnd;
}

int32_t
LteSlDiscResourcePoolFactory::GetDiscOffset (void)
{
  NS_LOG_FUNCTION (this << m_discOffset);
  return m_discOffset;
}

int64_t
LteSlDiscResourcePoolFactory::GetDiscBitmap (void)
{
  NS_LOG_FUNCTION (this << m_discBitmapValue);
  return m_discBitmapValue;
}

// Tx Parameters
bool
LteSlDiscResourcePoolFactory::GetHaveTxParameters (void)
{
  NS_LOG_FUNCTION (this << m_txParam);
  return m_txParam;
}

std::string
LteSlDiscResourcePoolFactory::GetDiscTxAlpha (void)
{
  NS_LOG_FUNCTION (this << m_txAlpha);
  return m_txAlpha;
}

int32_t
LteSlDiscResourcePoolFactory::GetDiscTxP0 (void)
{
  NS_LOG_FUNCTION (this << m_discP0);
  return m_discP0;
}

std::string
LteSlDiscResourcePoolFactory::GetDiscPoolSelection (void)
{
  NS_LOG_FUNCTION (this << m_poolSelection);
  return m_poolSelection;
}

bool
LteSlDiscResourcePoolFactory::GetHaveRsrpBasedPoolSelection (void)
{
  NS_LOG_FUNCTION (this << m_rsrpBased);
  return m_rsrpBased;
}

int32_t
LteSlDiscResourcePoolFactory::GetRsrpThreshLow (void)
{
  NS_LOG_FUNCTION (this << m_rsrpThreshLow);
  return m_rsrpThreshLow;
}

int32_t
LteSlDiscResourcePoolFactory::GetRsrpThreshHigh (void)
{
  NS_LOG_FUNCTION (this << m_rsrpThreshHigh);
  return m_rsrpThreshHigh;
}

std::string
LteSlDiscResourcePoolFactory::GetDiscTxProbability (void)
{
  NS_LOG_FUNCTION (this << m_discTxProb);
  return m_discTxProb;
}

} // namespace ns3

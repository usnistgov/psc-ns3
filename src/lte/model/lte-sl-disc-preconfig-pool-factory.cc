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

#include "lte-sl-disc-preconfig-pool-factory.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlDiscPreconfigPoolFactory");

LteSlDiscPreconfigPoolFactory::LteSlDiscPreconfigPoolFactory ()
{
  NS_LOG_FUNCTION (this);
  m_discCpLen = "NORMAL";
  m_period = "rf32";
  m_numRetx = 0;
  m_numRep = 1,
  m_discPrbNum = 10;
  m_discPrbStart = 10;
  m_discPrbEnd = 40;
  m_discOffset = 0;
  m_discBitmapValue = 0x11111;
  m_txAlpha = "al09";
  m_discP0 = -40;
  m_discTxProb = "p100";

}

LteRrcSap::SlPreconfigDiscPool
LteSlDiscPreconfigPoolFactory::CreatePool ()
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
  if (m_discTxProb == "p25")
    {
      m_pool.txParameters.txProbability.probability = LteRrcSap::TxProbability::p25;
    }
  else if (m_discTxProb == "p50")
    {
      m_pool.txParameters.txProbability.probability = LteRrcSap::TxProbability::p50;
    }
  else if (m_discTxProb == "p75")
    {
      m_pool.txParameters.txProbability.probability = LteRrcSap::TxProbability::p75;
    }
  else if (m_discTxProb == "p100")
    {
      m_pool.txParameters.txProbability.probability = LteRrcSap::TxProbability::p100;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED DISCOVERY PROBABILITY");
    }
  return m_pool;
}

} // namespace ns3

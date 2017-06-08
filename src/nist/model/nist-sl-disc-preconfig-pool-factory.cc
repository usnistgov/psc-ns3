/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This software was developed at the National Institute of Standards and
 * Technology by employees of the Federal Government in the course of
 * their official duties. Pursuant to titleElement 17 Section 105 of the United
 * States Code this software is not subject to copyright protection and
 * is in the public domain.
 * NIST assumes no responsibility whatsoever for its use by other parties,
 * and makes no guarantees, expressed or implied, about its quality,
 * reliability, or any other characteristic.

 * We would appreciate acknowledgement if the software is used.

 * NIST ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS" CONDITION AND
 * DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING
 * FROM THE USE OF THIS SOFTWARE.
 *
 * Modified by: NIST
 */

#include "nist-sl-disc-preconfig-pool-factory.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistSlDiscPreconfigPoolFactory");

NistSlDiscPreconfigPoolFactory::NistSlDiscPreconfigPoolFactory () :
  m_discCpLen("NORMAL"),
  m_period("rf32"),
  m_numRetx(0),
  m_numRep(1),
  m_discPrbNum(10),
  m_discPrbStart(10),
  m_discPrbEnd(40),
  m_discOffset(0),
  m_discBitmapValue(0x11111),
  m_txParam(true),
  m_discAlpha("al09"),
  m_discP0(-40),
  m_discTxProb("p100")
{
  NS_LOG_FUNCTION (this);
}

NistLteRrcSap::SlDiscResourcePool
NistSlDiscResourcePoolFactory::CreatePool ()
{
  if (m_discCpLen == "NORMAL") {m_pool.scCpLen.cplen = NistLteRrcSap::SlCpLen::NORMAL;}
  else if (m_discCpLen == "EXTENDED") {m_pool.scCpLen.cplen = NistLteRrcSap::SlCpLen::EXTENDED;}
  else {NS_FATAL_ERROR ("UNSUPPORTED CONTROL CP LENGTH");}
  if (m_period = "rf32") {m_pool.discPeriod.period = NistLteRrcSap::SlPeriodDisc::rf32;}
  else if (m_period = "rf64") {m_pool.discPeriod.period = NistLteRrcSap::SlPeriodDisc::rf64;}
  else if (m_period = "rf128") {m_pool.discPeriod.period = NistLteRrcSap::SlPeriodDisc::rf128;}
  else if (m_period = "rf256") {m_pool.discPeriod.period = NistLteRrcSap::SlPeriodDisc::rf256;}
  else if (m_period = "rf512") {m_pool.discPeriod.period = NistLteRrcSap::SlPeriodDisc::rf512;}
  else if (m_period = "rf1024") {m_pool.discPeriod.period = NistLteRrcSap::SlPeriodDisc::rf1024;}
  else {NS_FATAL_ERROR ("UNSUPPORTED DISCOVERY PERIOD");}
  m_pool.numRetx = m_numRetx;
  m_pool.numRepitition = m_numRep;
  m_pool.tfResourceConfig.prbNum = m_discPrbNum;
  m_pool.tfResourceConfig.prbStart = m_discPrbStart;
  m_pool.tfResourceConfig.prbEnd = m_discPrbEnd;
  m_pool.tfResourceConfig.offsetIndicator.offset = m_discOffset;
  m_pool.tfResourceConfig.subframeBitmap.bitmap = std::bitmap<40> (m_discBitmapValue);
  if (m_txAlpha == "al0") {m_pool.txParameters.txParametersGeneral.alpha = NistLteRrcSap::SlTxParameters::al0;}
  else if (m_txAlpha == "al04") {m_pool.txParameters.txParametersGeneral.alpha = NistLteRrcSap::SlTxParameters::al04;}
  else if (m_txAlpha == "al05") {m_pool.txParameters.txParametersGeneral.alpha = NistLteRrcSap::SlTxParameters::al05;}
  else if (m_txAlpha == "al06") {m_pool.txParameters.txParametersGeneral.alpha = NistLteRrcSap::SlTxParameters::al06;}
  else if (m_txAlpha == "al07") {m_pool.txParameters.txParametersGeneral.alpha = NistLteRrcSap::SlTxParameters::al07;}
  else if (m_txAlpha == "al08") {m_pool.txParameters.txParametersGeneral.alpha = NistLteRrcSap::SlTxParameters::al08;}
  else if (m_txAlpha == "al09") {m_pool.txParameters.txParametersGeneral.alpha = NistLteRrcSap::SlTxParameters::al09;}
  else if (m_txAlpha == "al1") {m_pool.txParameters.txParametersGeneral.alpha = NistLteRrcSap::SlTxParameters::al1;}
  else {NS_FATAL_ERROR ("UNSUPPORTED CONTROL TX ALPHA");}
  m_pool.txParameters.txParametersGeneral.p0 = m_discP0;
  if (m_discTxProb == "p25") {m_pool.txParameters.ueSelectedResourceConfig.txProbability.probability = NistLteRrcSap::TxProbability::p25;}
  else if (m_discTxProb == "p50") {m_pool.txParameters.ueSelectedResourceConfig.txProbability.probability = NistLteRrcSap::TxProbability::p50;}
  else if (m_discTxProb == "p75") {m_pool.txParameters.ueSelectedResourceConfig.txProbability.probability = NistLteRrcSap::TxProbability::p75;}
  else if (m_discTxProb == "p100") {m_pool.txParameters.ueSelectedResourceConfig.txProbability.probability = NistLteRrcSap::TxProbability::p100;}
  else {NS_FATAL_ERROR ("UNSUPPORTED DISCOVERY PROBABILITY");}
  return m_pool;
}

} // namespace ns3

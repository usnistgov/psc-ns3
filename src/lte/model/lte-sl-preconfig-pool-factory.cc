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

#include "lte-sl-preconfig-pool-factory.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlPreconfigPoolFactory");

LteSlPreconfigPoolFactory::LteSlPreconfigPoolFactory ()
{
  m_scCpLen = "NORMAL";
  m_period = "sf40";
  m_scPrbNum = 22;
  m_scPrbStart = 0;
  m_scPrbEnd = 49;
  m_scOffset = 0;
  m_scBitmapValue = 0x00000000FF;
  m_dataCpLen = "NORMAL";
  m_hoppingParameter = 0;
  m_subbands = "ns4";
  m_rbOffset = 0;
  m_hoppingInfo = 0xFF;
  m_ueSelected = true;
  m_haveTrptSubset = true;
  m_trptSubsetValue = 0x7;
  m_dataPrbNum = 25;
  m_dataPrbStart = 0;
  m_dataPrbEnd = 49;
  m_dataOffset = 8;
  m_dataBitmapValue = 0xFFFFFFFFFF;
  m_txParam = true;
  m_txAlpha = "al09";
  m_txP0 = -40;
  m_dataAlpha = "al09";
  m_dataP0 = -4;
  NS_LOG_FUNCTION (this);
}

LteRrcSap::SlPreconfigCommPool
LteSlPreconfigPoolFactory::CreatePool ()
{
  // Control
  //pool.scCpLen.cplen = LteRrcSap::SlCpLen::NORMAL;
  if (m_scCpLen == "NORMAL")
    {
      m_pool.scCpLen.cplen = LteRrcSap::SlCpLen::NORMAL;
    }
  else if (m_scCpLen == "EXTENDED")
    {
      m_pool.scCpLen.cplen = LteRrcSap::SlCpLen::EXTENDED;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED CONTROL CP LENGTH");
    }
  //pool.scPeriod.period = LteRrcSap::SlPeriodComm::sf40;
  if (m_period == "sf40")
    {
      m_pool.scPeriod.period = LteRrcSap::SlPeriodComm::sf40;
    }
  else if (m_period == "sf60")
    {
      m_pool.scPeriod.period = LteRrcSap::SlPeriodComm::sf60;
    }
  else if (m_period == "sf70")
    {
      m_pool.scPeriod.period = LteRrcSap::SlPeriodComm::sf70;
    }
  else if (m_period == "sf80")
    {
      m_pool.scPeriod.period = LteRrcSap::SlPeriodComm::sf80;
    }
  else if (m_period == "sf120")
    {
      m_pool.scPeriod.period = LteRrcSap::SlPeriodComm::sf120;
    }
  else if (m_period == "sf140")
    {
      m_pool.scPeriod.period = LteRrcSap::SlPeriodComm::sf140;
    }
  else if (m_period == "sf160")
    {
      m_pool.scPeriod.period = LteRrcSap::SlPeriodComm::sf160;
    }
  else if (m_period == "sf240")
    {
      m_pool.scPeriod.period = LteRrcSap::SlPeriodComm::sf240;
    }
  else if (m_period == "sf280")
    {
      m_pool.scPeriod.period = LteRrcSap::SlPeriodComm::sf280;
    }
  else if (m_period == "sf320")
    {
      m_pool.scPeriod.period = LteRrcSap::SlPeriodComm::sf320;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED CONTROL PERIOD LENGTH");
    }
  m_pool.scTfResourceConfig.prbNum = m_scPrbNum;
  m_pool.scTfResourceConfig.prbStart = m_scPrbStart;
  m_pool.scTfResourceConfig.prbEnd = m_scPrbEnd;
  m_pool.scTfResourceConfig.offsetIndicator.offset = m_scOffset;
  m_pool.scTfResourceConfig.subframeBitmap.bitmap = std::bitset<40> (m_scBitmapValue); //(0xFF00000000);
  //data
  //pool.dataCpLen.cplen = LteRrcSap::SlCpLen::NORMAL;
  if (m_dataCpLen == "NORMAL")
    {
      m_pool.dataCpLen.cplen = LteRrcSap::SlCpLen::NORMAL;
    }
  else if (m_dataCpLen == "EXTENDED")
    {
      m_pool.dataCpLen.cplen = LteRrcSap::SlCpLen::EXTENDED;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED CONTROL CP LENGTH");
    }
  if (m_hoppingParameter >= 504)
    {
      m_pool.dataHoppingConfig.hoppingParameter = 510;
    }
  else
    {
      m_pool.dataHoppingConfig.hoppingParameter = m_hoppingParameter;
    }
  if (m_subbands == "ns1")
    {
      m_pool.dataHoppingConfig.numSubbands = LteRrcSap::SlHoppingConfigComm::ns1;
    }
  else if (m_subbands == "ns2")
    {
      m_pool.dataHoppingConfig.numSubbands = LteRrcSap::SlHoppingConfigComm::ns2;
    }
  else if (m_subbands == "ns4")
    {
      m_pool.dataHoppingConfig.numSubbands = LteRrcSap::SlHoppingConfigComm::ns4;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED NUMBER OF SUBBANDS");
    }
  if (m_rbOffset >= 0 && m_rbOffset <= 110)
    {
      m_pool.dataHoppingConfig.rbOffset = m_rbOffset;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED RB OFFSET");
    }
  m_pool.dataHoppingConfig.hoppingInfo = m_hoppingInfo;
  //UE selected parameters
  m_pool.trptSubset.subset = std::bitset<3> (m_trptSubsetValue);
  m_pool.dataTfResourceConfig.prbNum = m_dataPrbNum;
  m_pool.dataTfResourceConfig.prbStart = m_dataPrbStart;
  m_pool.dataTfResourceConfig.prbEnd = m_dataPrbEnd;
  m_pool.dataTfResourceConfig.offsetIndicator.offset = m_dataOffset;
  m_pool.dataTfResourceConfig.subframeBitmap.bitmap = std::bitset<40> (m_dataBitmapValue);

  if (m_txAlpha == "al0")
    {
      m_pool.scTxParameters.alpha = LteRrcSap::SlTxParameters::al0;
    }
  else if (m_txAlpha == "al04")
    {
      m_pool.scTxParameters.alpha = LteRrcSap::SlTxParameters::al04;
    }
  else if (m_txAlpha == "al05")
    {
      m_pool.scTxParameters.alpha = LteRrcSap::SlTxParameters::al05;
    }
  else if (m_txAlpha == "al06")
    {
      m_pool.scTxParameters.alpha = LteRrcSap::SlTxParameters::al06;
    }
  else if (m_txAlpha == "al07")
    {
      m_pool.scTxParameters.alpha = LteRrcSap::SlTxParameters::al07;
    }
  else if (m_txAlpha == "al08")
    {
      m_pool.scTxParameters.alpha = LteRrcSap::SlTxParameters::al08;
    }
  else if (m_txAlpha == "al09")
    {
      m_pool.scTxParameters.alpha = LteRrcSap::SlTxParameters::al09;
    }
  else if (m_txAlpha == "al1")
    {
      m_pool.scTxParameters.alpha = LteRrcSap::SlTxParameters::al1;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED CONTROL TX ALPHA");
    }
  m_pool.scTxParameters.p0 = m_txP0;
  //pool.txParameters.dataTxParameters.alpha = m_dataAlpha;
  if (m_dataAlpha == "al0")
    {
      m_pool.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al0;
    }
  else if (m_dataAlpha == "al04")
    {
      m_pool.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al04;
    }
  else if (m_dataAlpha == "al05")
    {
      m_pool.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al05;
    }
  else if (m_dataAlpha == "al06")
    {
      m_pool.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al06;
    }
  else if (m_dataAlpha == "al07")
    {
      m_pool.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al07;
    }
  else if (m_dataAlpha == "al08")
    {
      m_pool.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al08;
    }
  else if (m_dataAlpha == "al09")
    {
      m_pool.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al09;
    }
  else if (m_dataAlpha == "al1")
    {
      m_pool.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al1;
    }
  else
    {
      NS_FATAL_ERROR ("UNSUPPORTED DATA TX ALPHA");
    }
  m_pool.dataTxParameters.p0 = m_dataP0;
  return m_pool;
}

} // namespace ns3

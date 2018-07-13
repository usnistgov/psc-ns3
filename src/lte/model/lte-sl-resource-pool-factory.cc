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

#include "lte-sl-resource-pool-factory.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LteSlResourcePoolFactory");

LteSlResourcePoolFactory::LteSlResourcePoolFactory ()
  : m_scCpLen ("NORMAL"),
    m_period ("sf40"),
    m_scPrbNum (22),
    m_scPrbStart (0),
    m_scPrbEnd (49),
    m_scOffset (0),
    m_scBitmapValue (0x00000000FF),
    m_dataCpLen ("NORMAL"),
    m_hoppingParameter (0),
    m_subbands ("ns4"),
    m_rbOffset (0),
    m_hoppingInfo (0xFF),
    m_ueSelected (true),
    m_haveTrptSubset (false),
    m_trptSubsetValue (0x7),
    m_dataPrbNum (25),
    m_dataPrbStart (0),
    m_dataPrbEnd (49),
    m_dataOffset (8),
    m_dataBitmapValue (0xFFFFFFFFFF),
    m_txParam (true),
    m_txAlpha ("al09"),
    m_txP0 (-40),
    m_dataAlpha ("al09"),
    m_dataP0 (-40)
{
  NS_LOG_FUNCTION (this);
}

LteRrcSap::SlCommResourcePool
LteSlResourcePoolFactory::CreatePool ()
{
  // Control
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
  m_pool.scTfResourceConfig.subframeBitmap.bitmap = std::bitset<40> (m_scBitmapValue);

  // Data
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

  // Ue Selected Parameters
  m_pool.haveUeSelectedResourceConfig = m_ueSelected;
  if (m_ueSelected)
    {
      m_pool.ueSelectedResourceConfig.haveTrptSubset = m_haveTrptSubset;
      if (m_haveTrptSubset)
        {
          m_pool.ueSelectedResourceConfig.trptSubset.subset = std::bitset<3> (m_trptSubsetValue);
        }
      m_pool.ueSelectedResourceConfig.dataTfResourceConfig.prbNum = m_dataPrbNum;
      m_pool.ueSelectedResourceConfig.dataTfResourceConfig.prbStart = m_dataPrbStart;
      m_pool.ueSelectedResourceConfig.dataTfResourceConfig.prbEnd = m_dataPrbEnd;
      m_pool.ueSelectedResourceConfig.dataTfResourceConfig.offsetIndicator.offset = m_dataOffset;
      m_pool.ueSelectedResourceConfig.dataTfResourceConfig.subframeBitmap.bitmap = std::bitset<40> (m_dataBitmapValue);
    }

  // Tx Parameters
  m_pool.haveTxParameters = m_txParam;
  if (m_txParam)
    {
      if (m_txAlpha == "al0")
        {
          m_pool.txParameters.scTxParameters.alpha = LteRrcSap::SlTxParameters::al0;
        }
      else if (m_txAlpha == "al04")
        {
          m_pool.txParameters.scTxParameters.alpha = LteRrcSap::SlTxParameters::al04;
        }
      else if (m_txAlpha == "al05")
        {
          m_pool.txParameters.scTxParameters.alpha = LteRrcSap::SlTxParameters::al05;
        }
      else if (m_txAlpha == "al06")
        {
          m_pool.txParameters.scTxParameters.alpha = LteRrcSap::SlTxParameters::al06;
        }
      else if (m_txAlpha == "al07")
        {
          m_pool.txParameters.scTxParameters.alpha = LteRrcSap::SlTxParameters::al07;
        }
      else if (m_txAlpha == "al08")
        {
          m_pool.txParameters.scTxParameters.alpha = LteRrcSap::SlTxParameters::al08;
        }
      else if (m_txAlpha == "al09")
        {
          m_pool.txParameters.scTxParameters.alpha = LteRrcSap::SlTxParameters::al09;
        }
      else if (m_txAlpha == "al1")
        {
          m_pool.txParameters.scTxParameters.alpha = LteRrcSap::SlTxParameters::al1;
        }
      else
        {
          NS_FATAL_ERROR ("UNSUPPORTED CONTROL TX ALPHA");
        }
      m_pool.txParameters.scTxParameters.p0 = m_txP0;
      if (m_dataAlpha == "al0")
        {
          m_pool.txParameters.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al0;
        }
      else if (m_dataAlpha == "al04")
        {
          m_pool.txParameters.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al04;
        }
      else if (m_dataAlpha == "al05")
        {
          m_pool.txParameters.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al05;
        }
      else if (m_dataAlpha == "al06")
        {
          m_pool.txParameters.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al06;
        }
      else if (m_dataAlpha == "al07")
        {
          m_pool.txParameters.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al07;
        }
      else if (m_dataAlpha == "al08")
        {
          m_pool.txParameters.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al08;
        }
      else if (m_dataAlpha == "al09")
        {
          m_pool.txParameters.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al09;
        }
      else if (m_dataAlpha == "al1")
        {
          m_pool.txParameters.dataTxParameters.alpha = LteRrcSap::SlTxParameters::al1;
        }
      else
        {
          NS_FATAL_ERROR ("UNSUPPORTED DATA TX ALPHA");
        }
      m_pool.txParameters.dataTxParameters.p0 = m_dataP0;
    }

  return m_pool;
}

// Control
void
LteSlResourcePoolFactory::SetControlCpLen (std::string cpLen)
{
  NS_LOG_FUNCTION (this << cpLen);
  m_scCpLen = cpLen;
}

void
LteSlResourcePoolFactory::SetControlPeriod (std::string period)
{
  NS_LOG_FUNCTION (this << period);
  m_period = period;
}

void
LteSlResourcePoolFactory::SetControlPrbNum (int8_t prbNum)
{
  NS_LOG_FUNCTION (this << prbNum);
  m_scPrbNum = prbNum;
}

void
LteSlResourcePoolFactory::SetControlPrbStart (int8_t prbStart)
{
  NS_LOG_FUNCTION (this << prbStart);
  m_scPrbStart = prbStart;
}

void
LteSlResourcePoolFactory::SetControlPrbEnd (int8_t prbEnd)
{
  NS_LOG_FUNCTION (this << prbEnd);
  m_scPrbEnd = prbEnd;
}

void
LteSlResourcePoolFactory::SetControlOffset (int16_t offset)
{
  NS_LOG_FUNCTION (this << offset);
  m_scOffset = offset;
}

void
LteSlResourcePoolFactory::SetControlBitmap (int64_t value)
{
  NS_LOG_FUNCTION (this << value);
  m_scBitmapValue = value;
}

// Data
void
LteSlResourcePoolFactory::SetDataCpLen (std::string cpLen)
{
  NS_LOG_FUNCTION (this << cpLen);
  m_dataCpLen = cpLen;
}

void
LteSlResourcePoolFactory::SetDataHoppingParameter (uint16_t hoppingParameter)
{
  NS_LOG_FUNCTION (this << hoppingParameter);
  m_hoppingParameter = hoppingParameter;
}

void
LteSlResourcePoolFactory::SetDataHoppingSubbands (std::string subbands)
{
  NS_LOG_FUNCTION (this << subbands);
  m_subbands = subbands;
}

void
LteSlResourcePoolFactory::SetDataHoppingOffset (int8_t rbOffset)
{
  NS_LOG_FUNCTION (this << rbOffset);
  m_rbOffset = rbOffset;
}

void
LteSlResourcePoolFactory::SetDataHoppingInfo (int8_t hoppingInfo)
{
  NS_LOG_FUNCTION (this << hoppingInfo);
  m_hoppingInfo = hoppingInfo;
}

// UE Selected Parameters
void
LteSlResourcePoolFactory::SetHaveUeSelectedResourceConfig (bool ueSelected)
{
  NS_LOG_FUNCTION (this << ueSelected);
  m_ueSelected = ueSelected;
}

void
LteSlResourcePoolFactory::SetHaveTrptSubset (bool haveTrptSubset)
{
  NS_LOG_FUNCTION (this << haveTrptSubset);
  m_haveTrptSubset = haveTrptSubset;
}

void
LteSlResourcePoolFactory::SetTrptSubset (int64_t value)
{
  NS_LOG_FUNCTION (this << value);
  m_trptSubsetValue = value;
}

void
LteSlResourcePoolFactory::SetDataPrbNum (int8_t prbNum)
{
  NS_LOG_FUNCTION (this << prbNum);
  m_dataPrbNum = prbNum;
}

void
LteSlResourcePoolFactory::SetDataPrbStart (int8_t prbStart)
{
  NS_LOG_FUNCTION (this << prbStart);
  m_dataPrbStart = prbStart;
}

void
LteSlResourcePoolFactory::SetDataPrbEnd (int8_t prbEnd)
{
  NS_LOG_FUNCTION (this << prbEnd);
  m_dataPrbEnd = prbEnd;
}

void
LteSlResourcePoolFactory::SetDataOffset (int16_t offset)
{
  NS_LOG_FUNCTION (this << offset);
  m_dataOffset = offset;
}

void
LteSlResourcePoolFactory::SetDataBitmap (int64_t value)
{
  NS_LOG_FUNCTION (this << value);
  m_dataBitmapValue = value;
}

// Tx Parameters
void
LteSlResourcePoolFactory::SetHaveTxParameters (bool txParam)
{
  NS_LOG_FUNCTION (this << txParam);
  m_txParam = txParam;
}

void
LteSlResourcePoolFactory::SetControlTxAlpha (std::string alpha)
{
  NS_LOG_FUNCTION (this << alpha);
  m_txAlpha = alpha;
}

void
LteSlResourcePoolFactory::SetControlTxP0 (int16_t p0)
{
  NS_LOG_FUNCTION (this << p0);
  m_txP0 = p0;
}

void
LteSlResourcePoolFactory::SetDataTxAlpha (std::string alpha)
{
  NS_LOG_FUNCTION (this << alpha);
  m_dataAlpha = alpha;
}

void
LteSlResourcePoolFactory::SetDataTxP0 (int16_t p0)
{
  NS_LOG_FUNCTION (this << p0);
  m_dataP0 = p0;
}

std::string
LteSlResourcePoolFactory::GetControlCpLen (void)
{
  NS_LOG_FUNCTION (this);
  return m_scCpLen;
}

std::string
LteSlResourcePoolFactory::GetControlPeriod (void)
{
  NS_LOG_FUNCTION (this);
  return m_period;
}

int8_t
LteSlResourcePoolFactory::GetControlPrbNum (void)
{
  NS_LOG_FUNCTION (this);
  return m_scPrbNum;
}

int8_t
LteSlResourcePoolFactory::GetControlPrbStart (void)
{
  NS_LOG_FUNCTION (this);
  return m_scPrbStart;
}

int8_t
LteSlResourcePoolFactory::GetControlPrbEnd (void)
{
  NS_LOG_FUNCTION (this);
  return m_scPrbEnd;
}

int16_t
LteSlResourcePoolFactory::GetControlOffset (void)
{
  NS_LOG_FUNCTION (this);
  return m_scOffset;
}

int64_t
LteSlResourcePoolFactory::GetControlBitmap (void)
{
  NS_LOG_FUNCTION (this);
  return m_scBitmapValue;
}

// Data
std::string
LteSlResourcePoolFactory::GetDataCpLen (void)
{
  NS_LOG_FUNCTION (this);
  return m_dataCpLen;
}

uint16_t
LteSlResourcePoolFactory::GetDataHoppingParameter (void)
{
  NS_LOG_FUNCTION (this);
  return m_hoppingParameter;
}

std::string
LteSlResourcePoolFactory::GetDataHoppingSubbands (void)
{
  NS_LOG_FUNCTION (this);
  return m_subbands;
}

int8_t
LteSlResourcePoolFactory::GetDataHoppingOffset (void)
{
  NS_LOG_FUNCTION (this);
  return m_rbOffset;
}

int8_t
LteSlResourcePoolFactory::GetDataHoppingInfo (void)
{
  NS_LOG_FUNCTION (this);
  return m_hoppingInfo;
}

// UE Selected Parameters
bool
LteSlResourcePoolFactory::GetHaveUeSelectedResourceConfig (void)
{
  NS_LOG_FUNCTION (this);
  return m_ueSelected;
}

bool
LteSlResourcePoolFactory::GetHaveTrptSubset (void)
{
  NS_LOG_FUNCTION (this);
  return m_haveTrptSubset;
}

int64_t
LteSlResourcePoolFactory::GetTrptSubset (void)
{
  NS_LOG_FUNCTION (this);
  return m_trptSubsetValue;
}

int8_t
LteSlResourcePoolFactory::GetDataPrbNum (void)
{
  NS_LOG_FUNCTION (this);
  return m_dataPrbNum;
}

int8_t
LteSlResourcePoolFactory::GetDataPrbStart (void)
{
  NS_LOG_FUNCTION (this);
  return m_dataPrbStart;
}

int8_t
LteSlResourcePoolFactory::GetDataPrbEnd (void)
{
  NS_LOG_FUNCTION (this);
  return m_dataPrbEnd;
}

int16_t
LteSlResourcePoolFactory::GetDataOffset (void)
{
  NS_LOG_FUNCTION (this);
  return m_dataOffset;
}

int64_t
LteSlResourcePoolFactory::GetDataBitmap (void)
{
  NS_LOG_FUNCTION (this);
  return m_dataBitmapValue;
}

// Tx Parameters
bool
LteSlResourcePoolFactory::GetHaveTxParameters (void)
{
  NS_LOG_FUNCTION (this);
  return m_txParam;
}

std::string
LteSlResourcePoolFactory::GetControlTxAlpha (void)
{
  NS_LOG_FUNCTION (this);
  return m_txAlpha;
}

int16_t
LteSlResourcePoolFactory::GetControlTxP0 (void)
{
  NS_LOG_FUNCTION (this);
  return m_txP0;
}

std::string
LteSlResourcePoolFactory::GetDataTxAlpha (void)
{
  NS_LOG_FUNCTION (this);
  return m_dataAlpha;
}

int16_t
LteSlResourcePoolFactory::GetDataTxP0 ()
{
  NS_LOG_FUNCTION (this);
  return m_dataP0;
}

} // namespace ns3

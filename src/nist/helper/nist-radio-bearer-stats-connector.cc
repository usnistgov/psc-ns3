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
 * Author: Nicola Baldo <nbaldo@cttc.es>
 */


#include <ns3/log.h>


#include "nist-radio-bearer-stats-connector.h"
#include "nist-radio-bearer-stats-calculator.h"
#include <ns3/nist-lte-enb-rrc.h>
#include <ns3/nist-lte-enb-net-device.h>
#include <ns3/nist-lte-ue-rrc.h>
#include <ns3/nist-lte-ue-net-device.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NistRadioBearerStatsConnector");

/**
  * Less than operator for NistCellIdRnti, because it is used as key in map
  */
bool
operator < (const NistRadioBearerStatsConnector::NistCellIdRnti& a, const NistRadioBearerStatsConnector::NistCellIdRnti& b)
{
  return ( (a.cellId < b.cellId) || ( (a.cellId == b.cellId) && (a.rnti < b.rnti) ) );
}

/**
 * This structure is used as interface between trace
 * sources and NistRadioBearerStatsCalculator. It stores
 * and provides calculators with cellId and IMSI,
 * because most trace sources do not provide it.
 */
struct NistBoundCallbackArgument : public SimpleRefCount<NistBoundCallbackArgument>
{
public:
  Ptr<NistRadioBearerStatsCalculator> stats;  //!< statistics calculator
  uint64_t imsi; //!< imsi
  uint16_t cellId; //!< cellId
};

/**
 * Callback function for DL TX statistics for both RLC and PDCP
 * /param arg
 * /param path
 * /param rnti
 * /param lcid
 * /param packetSize
 */
void
DlTxPduCallback (Ptr<NistBoundCallbackArgument> arg, std::string path,
                 uint16_t rnti, uint8_t lcid, uint32_t packetSize)
{
  NS_LOG_LOGIC (path << rnti << (uint16_t)lcid << packetSize);
  arg->stats->DlTxPdu (arg->cellId, arg->imsi, rnti, lcid, packetSize);
}

/**
 * Callback function for DL RX statistics for both RLC and PDCP
 * /param arg
 * /param path
 * /param rnti
 * /param lcid
 * /param packetSize
 * /param delay
 */
void
DlRxPduCallback (Ptr<NistBoundCallbackArgument> arg, std::string path,
                 uint16_t rnti, uint8_t lcid, uint32_t packetSize, uint64_t delay)
{
  NS_LOG_LOGIC (path << rnti << (uint16_t)lcid << packetSize << delay);
  arg->stats->DlRxPdu (arg->cellId, arg->imsi, rnti, lcid, packetSize, delay);
}

/**
 * Callback function for UL TX statistics for both RLC and PDCP
 * /param arg
 * /param path
 * /param rnti
 * /param lcid
 * /param packetSize
 */
void
UlTxPduCallback (Ptr<NistBoundCallbackArgument> arg, std::string path,
                 uint16_t rnti, uint8_t lcid, uint32_t packetSize)
{
  NS_LOG_LOGIC (path << rnti << (uint16_t)lcid << packetSize);
 
  arg->stats->UlTxPdu (arg->cellId, arg->imsi, rnti, lcid, packetSize);
}

/**
 * Callback function for UL RX statistics for both RLC and PDCP
 * /param arg
 * /param path
 * /param rnti
 * /param lcid
 * /param packetSize
 * /param delay
 */
void
UlRxPduCallback (Ptr<NistBoundCallbackArgument> arg, std::string path,
                 uint16_t rnti, uint8_t lcid, uint32_t packetSize, uint64_t delay)
{
  NS_LOG_LOGIC (path << rnti << (uint16_t)lcid << packetSize << delay);
 
  arg->stats->UlRxPdu (arg->cellId, arg->imsi, rnti, lcid, packetSize, delay);
}



NistRadioBearerStatsConnector::NistRadioBearerStatsConnector ()
  : m_connected (false)
{
}

void 
NistRadioBearerStatsConnector::EnableRlcStats (Ptr<NistRadioBearerStatsCalculator> rlcStats)
{
  m_rlcStats = rlcStats;
  EnsureConnected ();
}

void 
NistRadioBearerStatsConnector::EnablePdcpStats (Ptr<NistRadioBearerStatsCalculator> pdcpStats)
{
  m_pdcpStats = pdcpStats;
  EnsureConnected ();
}

void 
NistRadioBearerStatsConnector::EnsureConnected ()
{
  NS_LOG_FUNCTION (this);
  if (!m_connected)
    {
      Config::Connect ("/NodeList/*/DeviceList/*/NistLteEnbRrc/NewUeContext",
		       MakeBoundCallback (&NistRadioBearerStatsConnector::NotifyNewUeContextEnb, this));
      Config::Connect ("/NodeList/*/DeviceList/*/NistLteUeRrc/RandomAccessSuccessful",
		       MakeBoundCallback (&NistRadioBearerStatsConnector::NotifyRandomAccessSuccessfulUe, this));
      Config::Connect ("/NodeList/*/DeviceList/*/NistLteEnbRrc/ConnectionReconfiguration",
		       MakeBoundCallback (&NistRadioBearerStatsConnector::NotifyConnectionReconfigurationEnb, this));
      Config::Connect ("/NodeList/*/DeviceList/*/NistLteUeRrc/ConnectionReconfiguration",
		       MakeBoundCallback (&NistRadioBearerStatsConnector::NotifyConnectionReconfigurationUe, this));
      Config::Connect ("/NodeList/*/DeviceList/*/NistLteEnbRrc/HandoverStart",
		       MakeBoundCallback (&NistRadioBearerStatsConnector::NotifyHandoverStartEnb, this));
      Config::Connect ("/NodeList/*/DeviceList/*/NistLteUeRrc/HandoverStart",
		       MakeBoundCallback (&NistRadioBearerStatsConnector::NotifyHandoverStartUe, this));
      Config::Connect ("/NodeList/*/DeviceList/*/NistLteEnbRrc/HandoverEndOk",
		       MakeBoundCallback (&NistRadioBearerStatsConnector::NotifyHandoverEndOkEnb, this));
      Config::Connect ("/NodeList/*/DeviceList/*/NistLteUeRrc/HandoverEndOk",
		       MakeBoundCallback (&NistRadioBearerStatsConnector::NotifyHandoverEndOkUe, this));
      m_connected = true;
    }
}

void 
NistRadioBearerStatsConnector::NotifyRandomAccessSuccessfulUe (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->ConnectSrb0Traces (context, imsi, cellId, rnti);
}

void 
NistRadioBearerStatsConnector::NotifyConnectionSetupUe (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->ConnectSrb1TracesUe (context, imsi, cellId, rnti);
}

void 
NistRadioBearerStatsConnector::NotifyConnectionReconfigurationUe (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->ConnectTracesUeIfFirstTime (context, imsi, cellId, rnti);
}

void 
NistRadioBearerStatsConnector::NotifyHandoverStartUe (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti, uint16_t targetCellId)
{
  c->DisconnectTracesUe (context, imsi, cellId, rnti);
}

void 
NistRadioBearerStatsConnector::NotifyHandoverEndOkUe (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->ConnectTracesUe (context, imsi, cellId, rnti);
}

void 
NistRadioBearerStatsConnector::NotifyNewUeContextEnb (NistRadioBearerStatsConnector* c, std::string context, uint16_t cellId, uint16_t rnti)
{
  c->StoreNistUeManagerPath (context, cellId, rnti);
}

void 
NistRadioBearerStatsConnector::NotifyConnectionReconfigurationEnb (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->ConnectTracesEnbIfFirstTime (context, imsi, cellId, rnti);
}

void 
NistRadioBearerStatsConnector::NotifyHandoverStartEnb (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti, uint16_t targetCellId)
{
  c->DisconnectTracesEnb (context, imsi, cellId, rnti);
}

void 
NistRadioBearerStatsConnector::NotifyHandoverEndOkEnb (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  c->ConnectTracesEnb (context, imsi, cellId, rnti);
}

void 
NistRadioBearerStatsConnector::StoreNistUeManagerPath (std::string context, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context << cellId << rnti);
  std::ostringstream ueManagerPath;
  ueManagerPath <<  context.substr (0, context.rfind ("/")) << "/UeMap/" << (uint32_t) rnti;
  NistCellIdRnti key;
  key.cellId = cellId;
  key.rnti = rnti;
  m_ueManagerPathByNistCellIdRnti[key] = ueManagerPath.str ();
}

void 
NistRadioBearerStatsConnector::ConnectSrb0Traces (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << imsi << cellId << rnti);
  std::string ueRrcPath =  context.substr (0, context.rfind ("/"));
  NistCellIdRnti key;
  key.cellId = cellId;
  key.rnti = rnti;
  std::map<NistCellIdRnti, std::string>::iterator it = m_ueManagerPathByNistCellIdRnti.find (key);
  NS_ASSERT (it != m_ueManagerPathByNistCellIdRnti.end ());
  std::string ueManagerPath = it->second;  
  NS_LOG_LOGIC (this << " ueManagerPath: " << ueManagerPath);
  m_ueManagerPathByNistCellIdRnti.erase (it);

  if (m_rlcStats)
    {
      Ptr<NistBoundCallbackArgument> arg = Create<NistBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId; 
      arg->stats = m_rlcStats;

      // diconnect eventually previously connected SRB0 both at UE and eNB
      Config::Disconnect (ueRrcPath + "/Srb0/NistLteRlc/TxPDU",
                          MakeBoundCallback (&UlTxPduCallback, arg));
      Config::Disconnect (ueRrcPath + "/Srb0/NistLteRlc/RxPDU",
                          MakeBoundCallback (&DlRxPduCallback, arg));
      Config::Disconnect (ueManagerPath + "/Srb0/NistLteRlc/TxPDU",
                          MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Disconnect (ueManagerPath + "/Srb0/NistLteRlc/RxPDU",
                          MakeBoundCallback (&UlRxPduCallback, arg));

      // connect SRB0 both at UE and eNB
      Config::Connect (ueRrcPath + "/Srb0/NistLteRlc/TxPDU",
                       MakeBoundCallback (&UlTxPduCallback, arg));
      Config::Connect (ueRrcPath + "/Srb0/NistLteRlc/RxPDU",
                       MakeBoundCallback (&DlRxPduCallback, arg));
      Config::Connect (ueManagerPath + "/Srb0/NistLteRlc/TxPDU",
                       MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Connect (ueManagerPath + "/Srb0/NistLteRlc/RxPDU",
                       MakeBoundCallback (&UlRxPduCallback, arg));

      // connect SRB1 at eNB only (at UE SRB1 will be setup later)
      Config::Connect (ueManagerPath + "/Srb1/NistLteRlc/TxPDU",
                       MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Connect (ueManagerPath + "/Srb1/NistLteRlc/RxPDU",
                       MakeBoundCallback (&UlRxPduCallback, arg));
    }
  if (m_pdcpStats)
    {
      Ptr<NistBoundCallbackArgument> arg = Create<NistBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId; 
      arg->stats = m_pdcpStats;

      // connect SRB1 at eNB only (at UE SRB1 will be setup later)
      Config::Connect (ueManagerPath + "/Srb1/NistLtePdcp/RxPDU",
		       MakeBoundCallback (&UlRxPduCallback, arg));
      Config::Connect (ueManagerPath + "/Srb1/NistLtePdcp/TxPDU",
		       MakeBoundCallback (&DlTxPduCallback, arg));
    }
}

void 
NistRadioBearerStatsConnector::ConnectSrb1TracesUe (std::string ueRrcPath, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << imsi << cellId << rnti);
   if (m_rlcStats)
    {
      Ptr<NistBoundCallbackArgument> arg = Create<NistBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId; 
      arg->stats = m_rlcStats;
      Config::Connect (ueRrcPath + "/Srb1/NistLteRlc/TxPDU",
                       MakeBoundCallback (&UlTxPduCallback, arg));
      Config::Connect (ueRrcPath + "/Srb1/NistLteRlc/RxPDU",
                       MakeBoundCallback (&DlRxPduCallback, arg));
    }
  if (m_pdcpStats)
    {
      Ptr<NistBoundCallbackArgument> arg = Create<NistBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId; 
      arg->stats = m_pdcpStats;
      Config::Connect (ueRrcPath + "/Srb1/NistLtePdcp/RxPDU",
		       MakeBoundCallback (&DlRxPduCallback, arg));
      Config::Connect (ueRrcPath + "/Srb1/NistLtePdcp/TxPDU",
		       MakeBoundCallback (&UlTxPduCallback, arg));
    }
}
  
void 
NistRadioBearerStatsConnector::ConnectTracesUeIfFirstTime (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context);
  if (m_imsiSeenUe.find (imsi) == m_imsiSeenUe.end ())
    {
      m_imsiSeenUe.insert (imsi);
      ConnectTracesUe (context, imsi, cellId, rnti);
    }
}
 
void 
NistRadioBearerStatsConnector::ConnectTracesEnbIfFirstTime (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context);
   if (m_imsiSeenEnb.find (imsi) == m_imsiSeenEnb.end ())
    {
      m_imsiSeenEnb.insert (imsi);
      ConnectTracesEnb (context, imsi, cellId, rnti);
    }
}

void 
NistRadioBearerStatsConnector::ConnectTracesUe (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context);
  NS_LOG_LOGIC (this << "expected context should match /NodeList/*/DeviceList/*/NistLteUeRrc/");
  std::string basePath = context.substr (0, context.rfind ("/"));
  if (m_rlcStats)
    {
      Ptr<NistBoundCallbackArgument> arg = Create<NistBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId; 
      arg->stats = m_rlcStats;
      Config::Connect (basePath + "/DataRadioBearerMap/*/NistLteRlc/TxPDU",
		       MakeBoundCallback (&UlTxPduCallback, arg));
      Config::Connect (basePath + "/DataRadioBearerMap/*/NistLteRlc/RxPDU",
		       MakeBoundCallback (&DlRxPduCallback, arg));
      Config::Connect (basePath + "/Srb1/NistLteRlc/TxPDU",
		       MakeBoundCallback (&UlTxPduCallback, arg));
      Config::Connect (basePath + "/Srb1/NistLteRlc/RxPDU",
		       MakeBoundCallback (&DlRxPduCallback, arg));

    }
  if (m_pdcpStats)
    {
      Ptr<NistBoundCallbackArgument> arg = Create<NistBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId; 
      arg->stats = m_pdcpStats;
      Config::Connect (basePath + "/DataRadioBearerMap/*/NistLtePdcp/RxPDU",
		       MakeBoundCallback (&DlRxPduCallback, arg));
      Config::Connect (basePath + "/DataRadioBearerMap/*/NistLtePdcp/TxPDU",
		       MakeBoundCallback (&UlTxPduCallback, arg));
      Config::Connect (basePath + "/Srb1/NistLtePdcp/RxPDU",
		       MakeBoundCallback (&DlRxPduCallback, arg));
      Config::Connect (basePath + "/Srb1/NistLtePdcp/TxPDU",
		       MakeBoundCallback (&UlTxPduCallback, arg));
    }
}

void 
NistRadioBearerStatsConnector::ConnectTracesEnb (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this << context);
  NS_LOG_LOGIC (this << "expected context  should match /NodeList/*/DeviceList/*/NistLteEnbRrc/");
  std::ostringstream basePath;
  basePath <<  context.substr (0, context.rfind ("/")) << "/UeMap/" << (uint32_t) rnti;
  if (m_rlcStats)
    {
      Ptr<NistBoundCallbackArgument> arg = Create<NistBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId; 
      arg->stats = m_rlcStats;
      Config::Connect (basePath.str () + "/DataRadioBearerMap/*/NistLteRlc/RxPDU",
		       MakeBoundCallback (&UlRxPduCallback, arg));
      Config::Connect (basePath.str () + "/DataRadioBearerMap/*/NistLteRlc/TxPDU",
		       MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Connect (basePath.str () + "/Srb0/NistLteRlc/RxPDU",
		       MakeBoundCallback (&UlRxPduCallback, arg));
      Config::Connect (basePath.str () + "/Srb0/NistLteRlc/TxPDU",
		       MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Connect (basePath.str () + "/Srb1/NistLteRlc/RxPDU",
		       MakeBoundCallback (&UlRxPduCallback, arg));
      Config::Connect (basePath.str () + "/Srb1/NistLteRlc/TxPDU",
		       MakeBoundCallback (&DlTxPduCallback, arg));
    }
  if (m_pdcpStats)
    {
      Ptr<NistBoundCallbackArgument> arg = Create<NistBoundCallbackArgument> ();
      arg->imsi = imsi;
      arg->cellId = cellId; 
      arg->stats = m_pdcpStats;
      Config::Connect (basePath.str () + "/DataRadioBearerMap/*/NistLtePdcp/TxPDU",
		       MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Connect (basePath.str () + "/DataRadioBearerMap/*/NistLtePdcp/RxPDU",
		       MakeBoundCallback (&UlRxPduCallback, arg));
      Config::Connect (basePath.str () + "/Srb1/NistLtePdcp/TxPDU",
		       MakeBoundCallback (&DlTxPduCallback, arg));
      Config::Connect (basePath.str () + "/Srb1/NistLtePdcp/RxPDU",
		       MakeBoundCallback (&UlRxPduCallback, arg));
    }
}

void 
NistRadioBearerStatsConnector::DisconnectTracesUe (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this);
}


void 
NistRadioBearerStatsConnector::DisconnectTracesEnb (std::string context, uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
  NS_LOG_FUNCTION (this);
}



} // namespace ns3

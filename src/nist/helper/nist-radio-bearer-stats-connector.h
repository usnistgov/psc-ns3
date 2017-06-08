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



#ifndef NIST_RADIO_BEARER_STATS_CONNECTOR_H
#define NIST_RADIO_BEARER_STATS_CONNECTOR_H


#include <ns3/traced-callback.h>
#include <ns3/config.h>
#include <ns3/simple-ref-count.h>
#include <ns3/ptr.h>

#include <set>
#include <map>

namespace ns3 {

class NistRadioBearerStatsCalculator;

/**
 * \ingroup lte
 *
 * This class is very useful when user needs to collect
 * statistics from PDCD and RLC. It automatically connects
 * NistRadioBearerStatsCalculator to appropriate trace sinks.
 * Usually user do not use this class. All he/she needs to
 * to do is to call: NistLteHelper::EnablePdcpTraces() and/or
 * NistLteHelper::EnableRlcTraces().
 */

class NistRadioBearerStatsConnector
{
public:
  /// Constructor
  NistRadioBearerStatsConnector ();

  /**
   * Enables trace sinks for RLC layer. Usually, this function
   * is called by NistLteHelper::EnableRlcTraces().
   * \param rlcStats statistics calculator for RLC layer
   */
  void EnableRlcStats (Ptr<NistRadioBearerStatsCalculator> rlcStats);

  /**
   * Enables trace sinks for PDCP layer. Usually, this function
   * is called by NistLteHelper::EnablePdcpTraces().
   * \param pdcpStats statistics calculator for PDCP layer
   */
  void EnablePdcpStats (Ptr<NistRadioBearerStatsCalculator> pdcpStats);

  /**
   * Connects trace sinks to appropriate trace sources
   */
  void EnsureConnected ();

  // trace sinks, to be used with MakeBoundCallback

  /**
   * Function hooked to RandomAccessSuccessful trace source at UE RRC,
   * which is fired upon successful completion of the random access procedure
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyRandomAccessSuccessfulUe (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Sink connected source of UE Connection Setup trace. Not used.
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyConnectionSetupUe (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to ConnectionReconfiguration trace source at UE RRC,
   * which is fired upon RRC connection reconfiguration
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyConnectionReconfigurationUe (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to HandoverStart trace source at UE RRC,
   * which is fired upon start of a handover procedure
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   * \param targetCellId
   */
  static void NotifyHandoverStartUe (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId);

  /**
   * Function hooked to HandoverStart trace source at UE RRC,
   * which is fired upon successful termination of a handover procedure
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyHandoverEndOkUe (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to NewUeContext trace source at eNB RRC,
   * which is fired upon creation of a new UE context
   * \param c
   * \param context
   * \param cellid
   * \param rnti
   */
  static void NotifyNewUeContextEnb (NistRadioBearerStatsConnector* c, std::string context, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to ConnectionReconfiguration trace source at eNB RRC,
   * which is fired upon RRC connection reconfiguration
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyConnectionReconfigurationEnb (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to HandoverStart trace source at eNB RRC,
   * which is fired upon start of a handover procedure
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   * \param targetCellId
   */
  static void NotifyHandoverStartEnb (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId);

  /**
   * Function hooked to HandoverEndOk trace source at eNB RRC,
   * which is fired upon successful termination of a handover procedure
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyHandoverEndOkEnb (NistRadioBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

private:
  /**
   * Creates UE Manager path and stores it in m_ueManagerPathByNistCellIdRnti
   * \param ueManagerPath
   * \param cellId
   * \param rnti
   */
  void StoreNistUeManagerPath (std::string ueManagerPath, uint16_t cellId, uint16_t rnti);

  /**
   * Connects Srb0 trace sources at UE and eNB to RLC and PDCP calculators,
   * and Srb1 trace sources at eNB to RLC and PDCP calculators,
   * \param ueRrcPath
   * \param imsi
   * \param cellId
   * \param rnti
   */
  void ConnectSrb0Traces (std::string ueRrcPath, uint64_t imsi, uint16_t cellId, uint16_t rnti);

  /**
   * Connects Srb1 trace sources at UE to RLC and PDCP calculators
   * \param ueRrcPath
   * \param imsi
   * \param cellId
   * \param rnti
   */
  void ConnectSrb1TracesUe (std::string ueRrcPath, uint64_t imsi, uint16_t cellId, uint16_t rnti);

  /**
   * Connects all trace sources at UE to RLC and PDCP calculators.
   * This function can connect traces only once for UE.
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void ConnectTracesUeIfFirstTime (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Connects all trace sources at eNB to RLC and PDCP calculators.
   * This function can connect traces only once for eNB.
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void ConnectTracesEnbIfFirstTime (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Connects all trace sources at UE to RLC and PDCP calculators.
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void ConnectTracesUe (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Disconnects all trace sources at UE to RLC and PDCP calculators.
   * Function is not implemented.
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void DisconnectTracesUe (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Connects all trace sources at eNB to RLC and PDCP calculators
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void ConnectTracesEnb (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Disconnects all trace sources at eNB to RLC and PDCP calculators.
   * Function is not implemented.
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  void DisconnectTracesEnb (std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);


  Ptr<NistRadioBearerStatsCalculator> m_rlcStats; //!< Calculator for RLC Statistics
  Ptr<NistRadioBearerStatsCalculator> m_pdcpStats; //!< Calculator for PDCP Statistics

  bool m_connected; //!< true if traces are connected to sinks, initially set to false
  std::set<uint64_t> m_imsiSeenUe; //!< stores all UEs for which RLC and PDCP traces were connected
  std::set<uint64_t> m_imsiSeenEnb; //!< stores all eNBs for which RLC and PDCP traces were connected
  
  /**
   * Struct used as key in m_ueManagerPathByNistCellIdRnti map
   */
  struct NistCellIdRnti
  {
    uint16_t cellId; //!< cellId
    uint16_t rnti; //!< rnti
  };

  /**
   * Less than operator for NistCellIdRnti, because it is used as key in map
   */
  friend bool operator < (const NistCellIdRnti &a, const NistCellIdRnti &b);

  /**
   * List UE Manager Paths by NistCellIdRnti
   */
  std::map<NistCellIdRnti, std::string> m_ueManagerPathByNistCellIdRnti;

};



} // namespace ns3


#endif // NIST_RADIO_BEARER_STATS_CONNECTOR_H

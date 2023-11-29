/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
*   Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
*   Copyright (c) 2015, NYU WIRELESS, Tandon School of Engineering, New York University
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
*   Author: Marco Miozzo <marco.miozzo@cttc.es>
*           Nicola Baldo  <nbaldo@cttc.es>
*
*   Modified by: Marco Mezzavilla < mezzavilla@nyu.edu>
*                         Sourjya Dutta <sdutta@nyu.edu>
*                         Russell Ford <russell.ford@nyu.edu>
*                         Menglei Zhang <menglei@nyu.edu>
*/


#ifndef NR_BEARER_STATS_CONNECTOR_H
#define NR_BEARER_STATS_CONNECTOR_H


#include <ns3/traced-callback.h>
#include <ns3/config.h>
#include <ns3/simple-ref-count.h>
#include <ns3/ptr.h>

#include <set>
#include <map>

namespace ns3 {

class NrBearerStatsBase;

/**
 * \ingroup utils
 *
 * This class is very useful when user needs to collect
 * statistics from PDCD and RLC. It automatically connects
 * NrBearerStatsCalculator to appropriate trace sinks.
 * Usually user do not use this class. All he/she needs to
 * to do is to call: LteHelper::EnablePdcpTraces() and/or
 * LteHelper::EnableRlcTraces().
 */

class NrBearerStatsConnector
{
public:
  /// Constructor
  NrBearerStatsConnector ();

  /**
   * Enables trace sinks for RLC layer.
   * \param rlcStats statistics calculator for RLC layer
   */
  void EnableRlcStats (Ptr<NrBearerStatsBase> rlcStats);

  /**
   * Enables trace sinks for PDCP layer.
   * \param pdcpStats statistics calculator for PDCP layer
   */
  void EnablePdcpStats (Ptr<NrBearerStatsBase> pdcpStats);

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
  static void NotifyRandomAccessSuccessfulUe (NrBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Sink connected source of UE Connection Setup trace. Not used.
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyConnectionSetupUe (NrBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to ConnectionReconfiguration trace source at UE RRC,
   * which is fired upon RRC connection reconfiguration
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyConnectionReconfigurationUe (NrBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

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
  static void NotifyHandoverStartUe (NrBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId);

  /**
   * Function hooked to HandoverStart trace source at UE RRC,
   * which is fired upon successful termination of a handover procedure
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyHandoverEndOkUe (NrBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to NewUeContext trace source at eNB RRC,
   * which is fired upon creation of a new UE context
   * \param c
   * \param context
   * \param cellid
   * \param rnti
   */
  static void NotifyNewUeContextEnb (NrBearerStatsConnector* c, std::string context, uint16_t cellid, uint16_t rnti);

  /**
   * Function hooked to ConnectionReconfiguration trace source at eNB RRC,
   * which is fired upon RRC connection reconfiguration
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyConnectionReconfigurationEnb (NrBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

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
  static void NotifyHandoverStartEnb (NrBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId);

  /**
   * Function hooked to HandoverEndOk trace source at eNB RRC,
   * which is fired upon successful termination of a handover procedure
   * \param c
   * \param context
   * \param imsi
   * \param cellid
   * \param rnti
   */
  static void NotifyHandoverEndOkEnb (NrBearerStatsConnector* c, std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);

  /**
   * \return RLC stats
   */
  Ptr<NrBearerStatsBase> GetRlcStats ();
  /**
   * \return PDCP stats
   */
  Ptr<NrBearerStatsBase> GetPdcpStats ();

private:
  /**
   * Creates UE Manager path and stores it in m_ueManagerPathByCellIdRnti
   * \param ueManagerPath
   * \param cellId
   * \param rnti
   */
  void StoreUeManagerPath (std::string ueManagerPath, uint16_t cellId, uint16_t rnti);

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

  Ptr<NrBearerStatsBase> m_rlcStats; //!< Calculator for RLC Statistics
  Ptr<NrBearerStatsBase> m_pdcpStats; //!< Calculator for PDCP Statistics

  bool m_connected; //!< true if traces are connected to sinks, initially set to false
  std::set<uint64_t> m_imsiSeenUe; //!< stores all UEs for which RLC and PDCP traces were connected
  std::set<uint64_t> m_imsiSeenEnb; //!< stores all eNBs for which RLC and PDCP traces were connected

  /**
   * Struct used as key in m_ueManagerPathByCellIdRnti map
   */
  struct CellIdRnti
  {
    uint16_t cellId; //!< cellId
    uint16_t rnti; //!< rnti
  };

  /**
   * Less than operator for CellIdRnti, because it is used as key in map
   */
  friend bool operator < (const CellIdRnti &a, const CellIdRnti &b);

  /**
   * List UE Manager Paths by CellIdRnti
   */
  std::map<CellIdRnti, std::string> m_ueManagerPathByCellIdRnti;

};



} // namespace ns3


#endif // NR_BEARER_STATS_CONNECTOR_H

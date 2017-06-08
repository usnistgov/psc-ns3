/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Piotr Gawlowicz
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
 * Author: Piotr Gawlowicz <gawlowicz.p@gmail.com>
 * Modified by: NIST
 */

#ifndef NIST_LTE_UE_POWER_CONTROL_H
#define NIST_LTE_UE_POWER_CONTROL_H

#include <ns3/ptr.h>
#include <ns3/traced-callback.h>
#include <ns3/object.h>
#include <vector>


namespace ns3 {

/**
 * \brief This class realizes Uplink Power Control functionality
 *
 * When NistLteUePhy is about sending PUSCH/PUCCH/SRS it should ask
 * NistLteUePowerControl for current channel TX power level and then
 * use it while creating SpectrumValue for Uplink Transmission
 *
 * NistLteUePowerControl computes TX power level for PUSCH and SRS.
 * PUCCH is realized in ideal way and PUSCH do not use any resources,
 * so there is no need to compute power for that channel
 *
 * NistLteUePowerControlcomputes TX power based on some preconfigured
 * parameters and current Path-loss. Path-loss is computed as difference
 * between current RSRP and referenceSignalPower level. Current RSRP
 * is passed to NistLteUePowerControl by NistLteUePhy. referenceSignalPower is
 * configurable by attribute system
 *
 * Moreover, NistLteUePhy pass all received TPC values to NistLteUePowerControl,
 * what is a part of Closed Loop Power Control functionality
 */

class NistLteUePowerControl : public Object
{
public:

  NistLteUePowerControl ();
  virtual ~NistLteUePowerControl ();

  // inherited from Object
  static TypeId GetTypeId (void);
  virtual void DoInitialize (void);
  virtual void DoDispose (void);

  /**
   * Set the PC max transmission power in dBm
   * \param value PC max transmission power in dBm
   */
  void SetPcmax (double value);

  /**
   * Get the PC max transmission power in dBm
   * \return the PC max transmission power in dBm
   */
  double GetPcmax ();

  /**
   * Set transmit power 
   * \param value the transmit power value   
   */
  void SetTxPower (double value);

  /**
   * Set the reference signal power 
   * \param referenceSignalPower the reference signal power 
   */
  void ConfigureReferenceSignalPower (int8_t referenceSignalPower);

  /**
   * Set the cell ID 
   * \param cellId	the cell ID
   */
  void SetCellId (uint16_t cellId);

  /**
   * Set the RNTI
   * \param rnti the RNTI to set
   */
  void SetRnti (uint16_t rnti);

  /**
   * Set the Po Nominal PUSCH
   * \param value the value to set
   */
  void SetPoNominalPusch (int16_t value);

  /**
   * Set PO UE PUSCH
   * \param value the value to set
   */
  void SetPoUePusch (int16_t value);

  /**
   * Set alpha function
   * \param value	the alpha value to set
   */
  void SetAlpha (double value);

  /**
   * Set RSRP 
   * \param value the RSRP value to set
   */
  void SetRsrp (double value);

  /**
   * Configure reference signal power 
   * \param tpc	the TPC to report
   */
  void ReportTpc (uint8_t tpc);

  /**
   * Calculate PUSCH transmit power 
   */
  void CalculatePuschTxPower ();
  
  /**
   * Calculate PUCCH transmit power 
   */
  void CalculatePucchTxPower ();

  /**
   * Calculate SRS transmit power 
   */
  void CalculateSrsTxPower ();

  /**
   * Get PUSCH Tx power for LTE
   * \param rb the RB list
   * \return the transmission power to be used 
   */
  double GetPuschTxPower (std::vector <int> rb);
    
  /**
   * Get PUCCH Tx power for LTE
   * \param rb the RB list
   * \return the transmission power to be used 
   */
  double GetPucchTxPower (std::vector <int> rb);
  
  /**
   * Get SRS Tx power for LTE
   * \param rb the RB list
   * \return the transmission power to be used 
   */
  double GetSrsTxPower (std::vector <int> rb);
  
  /**
   * Get PSSCH Tx power for sidelink
   * \param rb the RB list
   * \return the transmission power to be used 
   */
  double GetPsschTxPower (std::vector <int> rb);

  /**
   * Get PSCCH Tx power for sidelink
   * \param rb the RB list
   * \return the transmission power to be used 
   */
  double GetPscchTxPower (std::vector <int> rb);
    
  /**
   * Get PSDCH Tx power for sidelink
   * \param rb the RB list
   * \return the transmission power to be used 
   */
  double GetPsdchTxPower (std::vector <int> rb);
  //


  /**
   * TracedCallback signature for uplink transmit power.
   *
   * \param [in] cellId Cell identifier.
   * \param [in] rnti The C-RNTI identifying the UE.
   * \param [in] power The current TX power.
   */
  typedef void (* TxPowerTracedCallback)
    (const uint16_t cellId, const uint16_t rnti, const double power);

private:
  void SetSubChannelMask (std::vector <int> mask);

  double m_txPower;
  double m_Pcmax;
  double m_Pcmin;

  double m_curPuschTxPower;
  double m_curPucchTxPower;
  double m_curSrsTxPower;

  // NIST
  // Define PSSCH, PSCCH and PSDCH Tx power for sidelink
  // to be adjusted them in the scenario
  double m_psschTxPower;
  double m_pscchTxPower;
  double m_psdchTxPower;
  //

  double m_referenceSignalPower;
  bool m_rsrpSet;
  double m_rsrp;

  std::vector<int16_t> m_PoNominalPusch;
  std::vector<int16_t> m_PoUePusch;

  int16_t m_PsrsOffset;

  uint16_t m_M_Pusch;
  std::vector<double> m_alpha;
  double m_pathLoss;
  double m_deltaTF;

  std::vector<int8_t> m_deltaPusch;
  double m_fc;

  uint16_t m_srsBandwidth;

  bool m_closedLoop;
  bool m_accumulationEnabled;

  uint16_t m_cellId;
  uint16_t m_rnti;
  /**
   * Trace information regarding Uplink TxPower
   * uint16_t cellId, uint16_t rnti, double txPower
   */
  TracedCallback<uint16_t, uint16_t, double> m_reportPuschTxPower;
  TracedCallback<uint16_t, uint16_t, double> m_reportPucchTxPower;
  TracedCallback<uint16_t, uint16_t, double> m_reportSrsTxPower;

  // NIST
  // Traces to collect TxPowerSl for PSSCH, PSCCH and PSDCH channels for sidelink
  TracedCallback<uint16_t, uint16_t, double> m_reportPsschTxPower;
  TracedCallback<uint16_t, uint16_t, double> m_reportPscchTxPower; 
  TracedCallback<uint16_t, uint16_t, double> m_reportPsdchTxPower;
  //

};


}

#endif /* NIST_LTE_UE_POWER_CONTROL_H */

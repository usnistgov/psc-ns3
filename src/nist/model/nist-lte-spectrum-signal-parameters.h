/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 CTTC
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
 * Modified by Marco Miozzo <mmiozzo@cttc.es> (add data and ctrl diversity)
 * Modified by: NIST
 */

#ifndef NIST_LTE_SPECTRUM_SIGNAL_PARAMETERS_H
#define NIST_LTE_SPECTRUM_SIGNAL_PARAMETERS_H


#include <ns3/spectrum-signal-parameters.h>

namespace ns3 {

class PacketBurst;
class NistLteControlMessage;


/**
 * \ingroup lte
 *
 * Signal parameters for Lte
 */
struct NistLteSpectrumSignalParameters : public SpectrumSignalParameters
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();

  /**
   * default constructor
   */
  NistLteSpectrumSignalParameters ();

  /**
   * copy constructor
   */
  NistLteSpectrumSignalParameters (const NistLteSpectrumSignalParameters& p);

  /**
   * The packet burst being transmitted with this signal
   */
  Ptr<PacketBurst> packetBurst;
};


/**
* \ingroup lte
*
* Signal parameters for Lte Data Frame (PDSCH), and eventually after some 
* control messages through other control channel embedded in PDSCH
* (i.e. PBCH)
*/
struct NistLteSpectrumSignalParametersDataFrame : public SpectrumSignalParameters
{
  
  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();
  
  /**
  * default constructor
  */
  NistLteSpectrumSignalParametersDataFrame ();
  
  /**
  * copy constructor
  */
  NistLteSpectrumSignalParametersDataFrame (const NistLteSpectrumSignalParametersDataFrame& p);
  
  /**
  * The packet burst being transmitted with this signal
  */
  Ptr<PacketBurst> packetBurst;
  
  std::list<Ptr<NistLteControlMessage> > ctrlMsgList;
  
  uint16_t cellId;
};


/**
* \ingroup lte
*
* Signal parameters for Lte DL Ctrl Frame (RS, PCFICH and PDCCH)
*/
struct NistLteSpectrumSignalParametersDlCtrlFrame : public SpectrumSignalParameters
{
  
  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();
  
  /**
  * default constructor
  */
  NistLteSpectrumSignalParametersDlCtrlFrame ();
  
  /**
  * copy constructor
  */
  NistLteSpectrumSignalParametersDlCtrlFrame (const NistLteSpectrumSignalParametersDlCtrlFrame& p);


  std::list<Ptr<NistLteControlMessage> > ctrlMsgList;
  
  uint16_t cellId;
  bool pss; // primary synchronization signal
};



/**
* \ingroup lte
*
* Signal parameters for Lte SRS Frame
*/
struct NistLteSpectrumSignalParametersUlSrsFrame : public SpectrumSignalParameters
{
  
  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();
  
  /**
  * default constructor
  */
  NistLteSpectrumSignalParametersUlSrsFrame ();
  
  /**
  * copy constructor
  */
  NistLteSpectrumSignalParametersUlSrsFrame (const NistLteSpectrumSignalParametersUlSrsFrame& p);
  
  uint16_t cellId;
};

/**
* \ingroup lte
*
* Signal parameters for Lte SL Frame (PSCCH and PSSCH)
*/
struct NistLteSpectrumSignalParametersSlFrame : public SpectrumSignalParameters
{
  
  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();
  
  /**
  * default constructor
  */
  NistLteSpectrumSignalParametersSlFrame ();
  
  /**
  * copy constructor
  */
  NistLteSpectrumSignalParametersSlFrame (const NistLteSpectrumSignalParametersSlFrame& p);


  /**
  * The packet burst being transmitted with this signal
  */
  Ptr<PacketBurst> packetBurst;

  /**
   * The control messages being sent (for sidelink, there should only be 1)
   */
  std::list<Ptr<NistLteControlMessage> > ctrlMsgList;
  
  uint32_t nodeId;
  uint8_t groupId;

  /**
   * The Sidelink synchronization signal identifier of the transmitting UE
   */
  uint64_t slssId;
  
};
  
}  // namespace ns3


#endif /* NIST_LTE_SPECTRUM_SIGNAL_PARAMETERS_H */

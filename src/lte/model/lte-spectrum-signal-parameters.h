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
 * Modified by: NIST // Contributions may not be subject to US copyright.
 */

#ifndef LTE_SPECTRUM_SIGNAL_PARAMETERS_H
#define LTE_SPECTRUM_SIGNAL_PARAMETERS_H


#include <ns3/spectrum-signal-parameters.h>

namespace ns3 {

class PacketBurst;
class LteControlMessage;


/**
 * \ingroup lte
 *
 * Signal parameters for Lte
 */
struct LteSpectrumSignalParameters : public SpectrumSignalParameters
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();

  /**
   * default constructor
   */
  LteSpectrumSignalParameters ();

  /**
   * copy constructor
   * \param p the LteSpectrumSignalParameters to copy
   */
  LteSpectrumSignalParameters (const LteSpectrumSignalParameters& p);

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
struct LteSpectrumSignalParametersDataFrame : public SpectrumSignalParameters
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();

  /**
  * default constructor
  */
  LteSpectrumSignalParametersDataFrame ();

  /**
  * copy constructor
  * \param p the LteSpectrumSignalParametersDataFrame to copy
  */
  LteSpectrumSignalParametersDataFrame (const LteSpectrumSignalParametersDataFrame& p);

  /**
  * The packet burst being transmitted with this signal
  */
  Ptr<PacketBurst> packetBurst;

  std::list<Ptr<LteControlMessage> > ctrlMsgList; ///< the control message list

  uint16_t cellId; ///< cell ID
};


/**
* \ingroup lte
*
* Signal parameters for Lte DL Ctrl Frame (RS, PCFICH and PDCCH)
*/
struct LteSpectrumSignalParametersDlCtrlFrame : public SpectrumSignalParameters
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();

  /**
  * default constructor
  */
  LteSpectrumSignalParametersDlCtrlFrame ();

  /**
  * copy constructor
  * \param p the LteSpectrumSignalParametersDlCtrlFrame to copy
  */
  LteSpectrumSignalParametersDlCtrlFrame (const LteSpectrumSignalParametersDlCtrlFrame& p);


  std::list<Ptr<LteControlMessage> > ctrlMsgList; ///< control message list

  uint16_t cellId; ///< cell ID
  bool pss; ///< primary synchronization signal
};



/**
* \ingroup lte
*
* Signal parameters for Lte SRS Frame
*/
struct LteSpectrumSignalParametersUlSrsFrame : public SpectrumSignalParameters
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();

  /**
  * default constructor
  */
  LteSpectrumSignalParametersUlSrsFrame ();

  /**
  * copy constructor
  * \param p the LteSpectrumSignalParametersUlSrsFrame to copy
  */
  LteSpectrumSignalParametersUlSrsFrame (const LteSpectrumSignalParametersUlSrsFrame& p);

  uint16_t cellId; ///< cell ID
};

/**
* \ingroup lte
*
* Signal parameters for Lte SL Frame (PSCCH and PSSCH)
*/
struct LteSpectrumSignalParametersSlFrame : public SpectrumSignalParameters
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();

  /**
  * default constructor
  */
  LteSpectrumSignalParametersSlFrame ();

  /**
  * copy constructor
  * \param p The LteSpectrumSignalParametersSlFrame
  */
  LteSpectrumSignalParametersSlFrame (const LteSpectrumSignalParametersSlFrame& p);


  /**
  * The packet burst being transmitted with this signal
  */
  Ptr<PacketBurst> packetBurst;

  /**
   * The control messages being sent (for sidelink, there should only be 1)
   */
  std::list<Ptr<LteControlMessage> > ctrlMsgList;

  uint32_t nodeId; ///< Node id

  /**
   * The Sidelink synchronization signal identifier of the transmitting UE
   */
  uint64_t slssId;

};

/**
* \ingroup lte
*
* Signal parameters for Lte SL Frame (PSCCH and PSSCH)
*/
struct LteSpectrumSignalParametersSlCtrlFrame : public LteSpectrumSignalParametersSlFrame
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();

  /**
  * default constructor
  */
  LteSpectrumSignalParametersSlCtrlFrame ();

  /**
  * copy constructor
  * \param p The LteSpectrumSignalParametersSlFrame
  */
  LteSpectrumSignalParametersSlCtrlFrame (const LteSpectrumSignalParametersSlCtrlFrame& p);

  uint8_t groupId; ///< Sidelink group id

};

/**
* \ingroup lte
*
* Signal parameters for Lte SL Frame (PSCCH and PSSCH)
*/
struct LteSpectrumSignalParametersSlDataFrame : public LteSpectrumSignalParametersSlFrame
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();

  /**
  * default constructor
  */
  LteSpectrumSignalParametersSlDataFrame ();

  /**
  * copy constructor
  * \param p The LteSpectrumSignalParametersSlFrame
  */
  LteSpectrumSignalParametersSlDataFrame (const LteSpectrumSignalParametersSlDataFrame& p);

  uint8_t groupId; ///< Sidelink group id

};

/**
* \ingroup lte
*
* Signal parameters for Lte SL Frame (PSCCH and PSSCH)
*/
struct LteSpectrumSignalParametersSlDiscFrame : public LteSpectrumSignalParametersSlFrame
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();

  /**
  * default constructor
  */
  LteSpectrumSignalParametersSlDiscFrame ();

  /**
  * copy constructor
  * \param p The LteSpectrumSignalParametersSlFrame
  */
  LteSpectrumSignalParametersSlDiscFrame (const LteSpectrumSignalParametersSlDiscFrame& p);

  uint32_t resNo; ///< Resource number
  uint8_t rv; ///< Redundancy version

};

/**
* \ingroup lte
*
* Signal parameters for Lte SL Frame (PSCCH and PSSCH)
*/
struct LteSpectrumSignalParametersSlMibFrame : public LteSpectrumSignalParametersSlFrame
{

  // inherited from SpectrumSignalParameters
  virtual Ptr<SpectrumSignalParameters> Copy ();

  /**
  * default constructor
  */
  LteSpectrumSignalParametersSlMibFrame ();

  /**
  * copy constructor
  * \param p The LteSpectrumSignalParametersSlFrame
  */
  LteSpectrumSignalParametersSlMibFrame (const LteSpectrumSignalParametersSlMibFrame& p);

};

}  // namespace ns3


#endif /* LTE_SPECTRUM_SIGNAL_PARAMETERS_H */

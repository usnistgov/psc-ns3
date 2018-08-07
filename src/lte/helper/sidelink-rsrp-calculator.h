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

#ifndef SIDELINK_RSRP_CALCULATOR_H
#define SIDELINK_RSRP_CALCULATOR_H

#include <ns3/propagation-loss-model.h>
#include <ns3/lte-spectrum-phy.h>
#include <ns3/lte-spectrum-value-helper.h>
#include <ns3/double.h>
#include <ns3/pointer.h>
#include <ns3/random-variable-stream.h>


namespace ns3 {

/**
 * This class allows to compute Sidelink RSRP used to associate Sidelink UEs. This class implements
 * both the methods defined in 3GPP TR 36.843 and TS 36.214.
 */

class SidelinkRsrpCalculator : public Object
{
public:
  /**
   * Constructor
   */
  SidelinkRsrpCalculator ();

  /**
   * Destructor
   */
  virtual ~SidelinkRsrpCalculator ();

  // inherited from Object
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  /**
   * Computes the S-RSRP between 2 UEs as defined in TS 36.214. Information about the uplink frequency and bandwidth is necessary to
   * be able to call the function before the simulation starts.
   * \param lossModel The loss model to use in the calculation
   * \param txPower Transmit power for the reference signal
   * \param ulEarfcn Uplink frequency
   * \param ulBandwidth Uplink bandwidth
   * \param txPhy The transmitter
   * \param rxPhy The receiver
   *
   * \return RSRP value in dBm
   */
  static double CalcSlRsrpPsbch (Ptr<PropagationLossModel> lossModel, double txPower, double ulEarfcn, double ulBandwidth, Ptr<SpectrumPhy> txPhy, Ptr<SpectrumPhy> rxPhy);
  /**
   * Computes the S-RSRP between a transmitter UE and a receiver UE as defined in TR 36.843.
   * \param lossModel The loss model to use in the calculation
   * \param txPower Transmit power for the reference signal
   * \param txPhy The transmitter
   * \param rxPhy The receiver
   *
   * \return RSRP value in dBm
   */
  static double CalcSlRsrpTxPw (Ptr<PropagationLossModel> lossModel, double txPower, Ptr<SpectrumPhy> txPhy, Ptr<SpectrumPhy> rxPhy);

private:
  /**
   * Compute the RSRP between the given nodes for the given propagation loss model (used by CalcSidelinkRsrp function)
   * This code is derived from the multi-model-spectrum-channel class. It can be used for both uplink and downlink
   * \param propagationLoss The loss model
   * \param psd The power spectral density of the transmitter
   * \param txPhy The transmitter
   * \param rxPhy The receiver
   *
   * \return The RSRP in dBm
   */
  static double DoCalcRsrp (Ptr<PropagationLossModel> propagationLoss, Ptr<SpectrumValue> psd, Ptr<SpectrumPhy> txPhy, Ptr<SpectrumPhy> rxPhy);

  /**
   * Compute the RSRP between the given nodes for the given propagation loss model (used by CalcSidelinkRsrpEval function)
   * This code is derived from the multi-model-spectrum-channel class. It can be used for both uplink and downlink
   * \param propagationLoss The loss model
   * \param txPower The transmit power
   * \param txPhy The transmitter
   * \param rxPhy The receiver
   *
   * \return The RSRP in dBm
   */
  static double DoCalcRsrp (Ptr<PropagationLossModel> propagationLoss, double txPower, Ptr<SpectrumPhy> txPhy, Ptr<SpectrumPhy> rxPhy);

};


} // namespace ns3


#endif // SIDELINK_RSRP_CALCULATOR_H

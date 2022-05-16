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

#include <ns3/sidelink-rsrp-calculator.h>

#include <ns3/mobility-model.h>
#include <ns3/antenna-model.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/angles.h>
#include <cfloat>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SidelinkRsrpCalculator");


NS_OBJECT_ENSURE_REGISTERED (SidelinkRsrpCalculator);

/// BwConfs structure
static const struct BwConfs
{
  double m_ulBandwidth; ///< Uplink bandwidth in RBs
  int m_indexLowerRb; ///< Index of lower RB
  int m_indexUpperRb; ///< Index of upper RB
} g_bwConfs[] = {
  { 6, 0, 5},
  { 15, 4, 10},
  { 25, 9, 15},
  { 50, 22, 27},
  { 75, 34, 40},
  { 100, 47, 52}
}; ///< six central RB configuration

/** \returns number of bandwidth configurations */
const uint16_t NUM_BW_CONFS (sizeof (g_bwConfs) / sizeof (BwConfs));

SidelinkRsrpCalculator::SidelinkRsrpCalculator ()
{
  NS_LOG_FUNCTION (this);
}

SidelinkRsrpCalculator::~SidelinkRsrpCalculator ()
{
  NS_LOG_FUNCTION (this);
}

TypeId SidelinkRsrpCalculator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SidelinkRsrpCalculator")
    .SetParent<Object> ()
    .AddConstructor<SidelinkRsrpCalculator> ()

  ;
  return tid;
}

void
SidelinkRsrpCalculator::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  Object::DoDispose ();
}


double
SidelinkRsrpCalculator::CalcSlRsrpPsbch (Ptr<PropagationLossModel> lossModel, double txPower, double ulEarfcn, double ulBandwidth, Ptr<SpectrumPhy> txPhy, Ptr<SpectrumPhy> rxPhy)
{

  NS_ASSERT_MSG (lossModel != 0, "No PropagationLossModel provided");

  /*
    36.214: Sidelink Reference Signal Received Power (S-RSRP) is defined as the linear average over the
    power contributions (in [W]) of the resource elements that carry demodulation reference signals
    associated with PSBCH, within the central 6 PRBs of the applicable subframes.
  */
  //This method returned very low values of RSRP
  std::vector <int> rbMask;
  int indexLowerRb = 0;
  int indexUpperRb = 0;

  for (uint16_t i = 0; i < NUM_BW_CONFS; ++i)
    {
      if ((g_bwConfs[i].m_ulBandwidth == ulBandwidth))
        {
          indexLowerRb = g_bwConfs[i].m_indexLowerRb;
          indexUpperRb = g_bwConfs[i].m_indexUpperRb;
          break;
        }
    }
  for (int i = indexLowerRb; i <= indexUpperRb; i++)
    {
      rbMask.push_back (i);
    }
  LteSpectrumValueHelper psdHelper;
  Ptr<SpectrumValue> psd = psdHelper.CreateUlTxPowerSpectralDensity (ulEarfcn, ulBandwidth, txPower, rbMask);

  double rsrp = DoCalcRsrp (lossModel, psd, txPhy, rxPhy);

  NS_LOG_INFO ("S-RSRP=" << rsrp);

  return rsrp;
}

double
SidelinkRsrpCalculator::CalcSlRsrpTxPw (Ptr<PropagationLossModel> lossModel, double txPower, Ptr<SpectrumPhy> txPhy, Ptr<SpectrumPhy> rxPhy)
{

  NS_ASSERT_MSG (lossModel != 0, "No PropagationLossModel provided");

  /*
    36.843: RSRP is calculated for transmit power of 23dBm by the transmitter UE and is the received power at the receiver UE calculated
    after accounting for large scale path loss and shadowing. Additionally note that wrap around is used for path loss calculations except
    for the case of partial -coverage.
  */
  double rsrp = DoCalcRsrp (lossModel, txPower, txPhy, rxPhy);

  NS_LOG_INFO ("RSRP=" << rsrp);

  return rsrp;
}

//called by CalcSlRsrpPsbch function
double
SidelinkRsrpCalculator::DoCalcRsrp (Ptr<PropagationLossModel> propagationLoss, Ptr<SpectrumValue> psd, Ptr<SpectrumPhy> txPhy, Ptr<SpectrumPhy> rxPhy)
{
  Ptr<MobilityModel> txMobility = txPhy->GetMobility ();
  Ptr<MobilityModel> rxMobility = rxPhy->GetMobility ();

  double pathLossDb = 0;
  Ptr<AntennaModel> txAntenna = DynamicCast<AntennaModel> (txPhy->GetAntenna ());
  if (txAntenna)
    {
      Angles txAngles (rxMobility->GetPosition (), txMobility->GetPosition ());
      double txAntennaGain = txAntenna->GetGainDb (txAngles);
      NS_LOG_DEBUG ("txAntennaGain = " << txAntennaGain << " dB");
      pathLossDb -= txAntennaGain;
    }
  Ptr<AntennaModel> rxAntenna = DynamicCast<AntennaModel> (rxPhy->GetAntenna ());
  if (rxAntenna)
    {
      Angles rxAngles (txMobility->GetPosition (), rxMobility->GetPosition ());
      double rxAntennaGain = rxAntenna->GetGainDb (rxAngles);
      NS_LOG_DEBUG ("rxAntennaGain = " << rxAntennaGain << " dB");
      pathLossDb -= rxAntennaGain;
    }
  if (propagationLoss)
    {
      double propagationGainDb = propagationLoss->CalcRxPower (0, txMobility, rxMobility);
      NS_LOG_DEBUG ("propagationGainDb = " << propagationGainDb << " dB");
      pathLossDb -= propagationGainDb;
    }
  NS_LOG_DEBUG ("total pathLoss = " << pathLossDb << " dB");

  double pathGainLinear = std::pow (10.0, (-pathLossDb) / 10.0);
  Ptr<SpectrumValue> rxPsd = Copy<SpectrumValue> (psd);
  *rxPsd *= pathGainLinear;

  // RSRP evaluated as averaged received power among RBs
  double sum = 0.0;
  uint8_t rbNum = 0;
  Values::const_iterator it;
  for (it = (*rxPsd).ValuesBegin (); it != (*rxPsd).ValuesEnd (); it++)
    {
      //The non active RB will be set to -inf
      //We count only the active
      if ((*it))
        {
          // convert PSD [W/Hz] to linear power [W] for the single RE
          // we consider only one RE for the RS since the channel is
          // flat within the same RB
          double powerTxW = ((*it) * 180000.0) / 12.0;
          sum += powerTxW;
          rbNum++;
        }
    }
  double rsrp = (rbNum > 0) ? (sum / rbNum) : DBL_MAX;

  NS_LOG_INFO ("RSRP linear=" << rsrp << " (" << 10 * std::log10 (rsrp) + 30 << "dBm)");

  return 10 * std::log10 (rsrp) + 30;
}

//used by CalcSlRsrpTxPw function
double
SidelinkRsrpCalculator::DoCalcRsrp (Ptr<PropagationLossModel> propagationLoss, double txPower, Ptr<SpectrumPhy> txPhy, Ptr<SpectrumPhy> rxPhy)
{
  Ptr<MobilityModel> txMobility = txPhy->GetMobility ();
  Ptr<MobilityModel> rxMobility = rxPhy->GetMobility ();

  double pathLossDb = 0;
  Ptr<AntennaModel> txAntenna = DynamicCast<AntennaModel> (txPhy->GetAntenna ());
  if (txAntenna)
    {
      Angles txAngles (rxMobility->GetPosition (), txMobility->GetPosition ());
      double txAntennaGain = txAntenna->GetGainDb (txAngles);
      NS_LOG_DEBUG ("txAntennaGain = " << txAntennaGain << " dB");
      pathLossDb -= txAntennaGain;
    }
  Ptr<AntennaModel> rxAntenna = DynamicCast<AntennaModel> (rxPhy->GetAntenna ());
  if (rxAntenna)
    {
      Angles rxAngles (txMobility->GetPosition (), rxMobility->GetPosition ());
      double rxAntennaGain = rxAntenna->GetGainDb (rxAngles);
      NS_LOG_DEBUG ("rxAntennaGain = " << rxAntennaGain << " dB");
      pathLossDb -= rxAntennaGain;
    }
  if (propagationLoss)
    {
      double propagationGainDb = propagationLoss->CalcRxPower (0, txMobility, rxMobility);
      NS_LOG_DEBUG ("propagationGainDb = " << propagationGainDb << " dB");
      pathLossDb -= propagationGainDb;
    }
  NS_LOG_DEBUG ("total pathLoss = " << pathLossDb << " dB");

  double rsrp = txPower - pathLossDb;

  NS_LOG_INFO ("RSRP=" << rsrp << " dBm");

  return rsrp;
}

} // namespace ns3


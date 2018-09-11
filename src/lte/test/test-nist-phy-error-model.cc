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

#include <ns3/log.h>
#include <ns3/test.h>
#include <ns3/double.h>
#include <ns3/lte-nist-error-model.h>
#include <ns3/lte-harq-phy.h>
#include <ns3/lte-sl-harq-phy.h>
#include <ns3/simulator.h>
#include <ns3/math.h>
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>


NS_LOG_COMPONENT_DEFINE ("TestNistPhyErrorModel");

using namespace ns3;

enum NistPhyErrorModelTestCondition
{
  EQUAL = 0
};

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Lte Nist physical error model test case.
 */
class LteNistPhyErrorModelTestCase : public TestCase
{
public:
  /**
   * Build name string function
   *
   * \param channel The type of physical channel
   * \param fadingChannel The fading channel
   * \param txMode The transmission mode
   * \param mcs The MCS
   * \param newSinr The SINR
   * \param harqHistory The HARQ history
   * \returns the name string
   */
  static std::string BuildNameString (LteNistErrorModel::LtePhyChannel channel,
                                      LteNistErrorModel::LteFadingModel fadingChannel,
                                      LteNistErrorModel::LteTxMode txMode,
                                      uint16_t mcs,
                                      double newSinr,
                                      HarqProcessInfoList_t harqHistory);
  /**
   * Constructor
   *
   * \param channel The type of physical channel
   * \param fadingChannel The fading channel
   * \param txMode The transmission mode
   * \param mcs The MCS
   * \param newSinr The SINR
   * \param harqHistory The HARQ history
   * \param expectedBler The expected BLER value
   * \param cond The test condition
   */
  LteNistPhyErrorModelTestCase (LteNistErrorModel::LtePhyChannel channel,
                                LteNistErrorModel::LteFadingModel fadingChannel,
                                LteNistErrorModel::LteTxMode txMode,
                                uint16_t mcs,
                                double newSinr,
                                HarqProcessInfoList_t harqHistory,
                                double expectedBler,
                                NistPhyErrorModelTestCondition cond);


private:
  virtual void DoRun (void);

  LteNistErrorModel::LtePhyChannel m_channel; ///< The type of physical channel
  LteNistErrorModel::LteFadingModel m_fadingChannel; ///< The fading channel
  LteNistErrorModel::LteTxMode m_txmode; ///< The transmission mode
  uint16_t m_mcs; ///< The MCS
  double m_newSinr; ///< The SINR
  HarqProcessInfoList_t m_harqHistory; ///< The HARQ history
  double m_expectedBler; ///< The expected BLER value
  NistPhyErrorModelTestCondition m_cond; ///< The test condition
};

std::string LteNistPhyErrorModelTestCase::BuildNameString (LteNistErrorModel::LtePhyChannel channel, LteNistErrorModel::LteFadingModel fadingChannel, LteNistErrorModel::LteTxMode txmode, uint16_t mcs, double newSinr, HarqProcessInfoList_t harqHistory)
{
  std::ostringstream oss;

  switch (channel)
    {
    case LteNistErrorModel::PSCCH:
      oss << "PSCCH";
      break;
    case LteNistErrorModel::PSSCH:
      oss << "PSSCH";
      break;
    case LteNistErrorModel::PSDCH:
      oss << "PSDCH";
      break;
    case LteNistErrorModel::PUSCH:
      oss << "PUSCH";
      break;
    default:
      oss << "OTHER channel";
      break;
    }
  oss << ", ";

  switch (fadingChannel)
    {
    case LteNistErrorModel::AWGN:
      oss << "AWGN";
      break;
    default:
      oss << "Other";
      break;
    }
  oss << ", tx mode=" << txmode
      << ", mcs=" << mcs
      << ", newSinr=" << newSinr << " W (" << 10 * std::log10 (newSinr) << " dB)"
      << ", harqHistory size=" << harqHistory.size ();
  if (harqHistory.size () > 0)
    {
      oss << ", last HarqSinr" <<  harqHistory[harqHistory.size () - 1].m_sinr << " W (" << 10 * std::log10 (harqHistory[harqHistory.size () - 1].m_sinr) << " dB)";
    }

  return oss.str ();
}


LteNistPhyErrorModelTestCase::LteNistPhyErrorModelTestCase (LteNistErrorModel::LtePhyChannel channel, LteNistErrorModel::LteFadingModel fadingChannel, LteNistErrorModel::LteTxMode txmode, uint16_t mcs, double newSinr, HarqProcessInfoList_t harqHistory, double expectedBler, NistPhyErrorModelTestCondition cond)
  : TestCase (BuildNameString (channel, fadingChannel, txmode, mcs, newSinr, harqHistory)),
    m_channel (channel),
    m_fadingChannel (fadingChannel),
    m_txmode (txmode),
    m_mcs (mcs),
    m_newSinr (newSinr),
    m_harqHistory (harqHistory),
    m_expectedBler (expectedBler),
    m_cond (cond)
{
}

void
LteNistPhyErrorModelTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_channel, m_fadingChannel, m_txmode, m_mcs, m_newSinr, m_harqHistory));

  TbErrorStats_t tbStat;

  switch (m_channel)
    {
    case LteNistErrorModel::PSCCH:
      tbStat = LteNistErrorModel::GetPscchBler (m_fadingChannel, m_txmode, m_newSinr);
      break;
    case LteNistErrorModel::PSSCH:
      tbStat = LteNistErrorModel::GetPsschBler (m_fadingChannel, m_txmode, m_mcs, m_newSinr, m_harqHistory);
      break;
    case LteNistErrorModel::PSDCH:
      tbStat = LteNistErrorModel::GetPsdchBler (m_fadingChannel, m_txmode, m_newSinr, m_harqHistory);
      break;
    case LteNistErrorModel::PUSCH:
      tbStat = LteNistErrorModel::GetPuschBler (m_fadingChannel, m_txmode, m_mcs, m_newSinr, m_harqHistory);
      break;
    default:
      tbStat.tbler = -1; //put invalid value to make sure the test fails
      break;
    }
  switch (m_cond)
    {
    case EQUAL:
      NS_TEST_EXPECT_MSG_EQ_TOL (tbStat.tbler, m_expectedBler, 0.001, "wrong value of the bler");
      break;
    default:
      break;
    }
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Lte Nist physical error model test suite.
 */
class LteNistPhyErrorModelTestSuite : public TestSuite
{
public:
  LteNistPhyErrorModelTestSuite ();
};

LteNistPhyErrorModelTestSuite::LteNistPhyErrorModelTestSuite ()
  : TestSuite ("nist-phy-error-model", UNIT)
{

  //LogComponentEnable("LteNistErrorModel", LOG_LEVEL_ALL);
  //LogComponentEnable("TestNistPhyErrorModel", LOG_LEVEL_ALL);

  HarqProcessInfoList_t harqInfoList;

  //Tests with no HARQ history
  //tests not requiring interpolation
  AddTestCase (new LteNistPhyErrorModelTestCase (LteNistErrorModel::PSSCH, LteNistErrorModel::AWGN, LteNistErrorModel::SISO, 10, std::pow (10, (-1 / 10.0)),  harqInfoList, 1,    EQUAL), TestCase::QUICK);
  AddTestCase (new LteNistPhyErrorModelTestCase (LteNistErrorModel::PSSCH, LteNistErrorModel::AWGN, LteNistErrorModel::SISO, 10, std::pow (10, (2 / 10.0)),  harqInfoList, 0.968,    EQUAL), TestCase::QUICK);
  AddTestCase (new LteNistPhyErrorModelTestCase (LteNistErrorModel::PSSCH, LteNistErrorModel::AWGN, LteNistErrorModel::SISO, 10, std::pow (10, (3.2 / 10.0)),  harqInfoList, 0.048,    EQUAL), TestCase::QUICK);

  //tests with interpolation
  AddTestCase (new LteNistPhyErrorModelTestCase (LteNistErrorModel::PSSCH, LteNistErrorModel::AWGN, LteNistErrorModel::SISO, 10, std::pow (10, (2.5 / 10.0)),  harqInfoList, 0.657,    EQUAL), TestCase::QUICK);

  //Tests with 1 HARQ history
  HarqProcessInfoElement_t el;
  el.m_sinr = std::pow (10, (-2 / 10.0));
  harqInfoList.push_back (el);
  //if SINR does not change, there is no interpolation
  AddTestCase (new LteNistPhyErrorModelTestCase (LteNistErrorModel::PSSCH, LteNistErrorModel::AWGN, LteNistErrorModel::SISO, 10, std::pow (10, (-2 / 10.0)),  harqInfoList, 0.96,    EQUAL), TestCase::QUICK);
  //if SINR does not change, there is no interpolation
  AddTestCase (new LteNistPhyErrorModelTestCase (LteNistErrorModel::PSSCH, LteNistErrorModel::AWGN, LteNistErrorModel::SISO, 10, std::pow (10, (-1 / 10.0)),  harqInfoList, 0.463,    EQUAL), TestCase::QUICK);

  harqInfoList.erase (harqInfoList.begin ());
  el.m_sinr = std::pow (10, (-1 / 10.0));
  harqInfoList.push_back (el);
  AddTestCase (new LteNistPhyErrorModelTestCase (LteNistErrorModel::PSSCH, LteNistErrorModel::AWGN, LteNistErrorModel::SISO, 10, std::pow (10, (-2 / 10.0)),  harqInfoList, 0.565,    EQUAL), TestCase::QUICK);

  harqInfoList.erase (harqInfoList.begin ());
  el.m_sinr = std::pow (10, (-4 / 10.0));
  harqInfoList.push_back (el);
  AddTestCase (new LteNistPhyErrorModelTestCase (LteNistErrorModel::PSSCH, LteNistErrorModel::AWGN, LteNistErrorModel::SISO, 10, std::pow (10, (-3 / 10.0)),  harqInfoList, 1,    EQUAL), TestCase::QUICK);


  //Testing PSCCH
  harqInfoList.erase (harqInfoList.begin ());
  AddTestCase (new LteNistPhyErrorModelTestCase (LteNistErrorModel::PSCCH, LteNistErrorModel::AWGN, LteNistErrorModel::SISO, 0, std::pow (10, (-7 / 10.0)),  harqInfoList, 1,    EQUAL), TestCase::QUICK);

  AddTestCase (new LteNistPhyErrorModelTestCase (LteNistErrorModel::PSCCH, LteNistErrorModel::AWGN, LteNistErrorModel::SISO, 0, std::pow (10, (2 / 10.0)),  harqInfoList, 0,    EQUAL), TestCase::QUICK);

}

static LteNistPhyErrorModelTestSuite staticLteNistPhyErrorModelTestSuiteInstance;

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


#include "ns3/lte-rrc-sap.h"
#include "ns3/lte-sl-pool.h"
#include "ns3/lte-sl-disc-resource-pool-factory.h"
#include <string>
#include <ns3/log.h>
#include <ns3/test.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TestSidelinkDiscPool");

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidlelink discovery pool next frame and subframe test case.
 */
class SidlelinkDiscPoolNextFrameSubframeTestCase : public TestCase
{
public:
  /**
   * Build name string function
   *
   * \param pfactory LteSlDiscResourcePoolFactory
   * \param frameNo Frame number
   * \param subframeNo Subframe number
   * \returns the name string
   */
  static std::string BuildNameString (LteSlDiscResourcePoolFactory pfactory,uint32_t frameNo,uint32_t subframeNo);

  /**
   * Constructor
   *
   * \param pfactory LteSlDiscResourcePoolFactory
   * \param frameNo Frame number
   * \param subframeNo Subframe number
   * \param expectedNextFrameNo Expected next frame number
   * \param expectedNextSubframeNo Expected subframe number
   */
  SidlelinkDiscPoolNextFrameSubframeTestCase (LteSlDiscResourcePoolFactory pfactory,uint32_t frameNo,
                                              uint32_t subframeNo, uint32_t expectedNextFrameNo, uint32_t expectedNextSubframeNo);

private:
  virtual void DoRun (void);
  LteSlDiscResourcePoolFactory m_pfactory; ///< LteSlDiscResourcePoolFactory
  uint32_t m_frameNo; ///< Frame number
  uint32_t m_subframeNo; ///< Subframe number
  uint32_t m_expectedNextFrameNo; ///< Expected next frame number
  uint32_t m_expectedNextSubframeNo; ///< Expected next subframe number
};

std::string SidlelinkDiscPoolNextFrameSubframeTestCase::BuildNameString (LteSlDiscResourcePoolFactory pfactory,uint32_t frameNo, uint32_t subframeNo)
{
  std::ostringstream oss;

  oss << "TestCase:SidlelinkDiscPoolNextFrameSubframeTestCase, ";
  oss << "DiscCpLen:" << pfactory.GetDiscCpLen () << ", ";
  oss << "DiscPeriod:" << pfactory.GetDiscPeriod () << ", ";
  oss << "NumRetx:" << static_cast<int> (pfactory.GetNumRetx ()) << ", ";
  oss << "NumRepetition:" << pfactory.GetNumRepetition () << ", ";
  oss << "DiscPrbNum:" << pfactory.GetDiscPrbNum () << ", ";
  oss << "DiscPrbStart:" << pfactory.GetDiscPrbStart () << ", ";
  oss << "DiscPrbEnd:" << pfactory.GetDiscPrbEnd () << ", ";
  oss << "DiscOffset:" << pfactory.GetDiscOffset () << ", ";
  oss << "DiscBitmap:" << std::hex << pfactory.GetDiscBitmap () << std::dec << ", ";
  oss << "HaveTxParameters:" << pfactory.GetHaveTxParameters () << ", ";
  oss << "DiscTxAlpha:" << pfactory.GetDiscTxAlpha () << ", ";
  oss << "DiscTxP0:" << pfactory.GetDiscTxP0 () << ", ";
  oss << "DiscPoolSelection:" << pfactory.GetDiscPoolSelection () << ", ";
  oss << "HaveRsrpBasedPoolSelection:" << pfactory.GetHaveRsrpBasedPoolSelection () << ", ";
  oss << "RsrpThreshLow:" << pfactory.GetRsrpThreshLow () << ", ";
  oss << "RsrpThreshHigh:" << pfactory.GetRsrpThreshHigh () << ", ";
  oss << "DiscTxProbability:" << pfactory.GetDiscTxProbability () << ", ";
  oss << "FrameNo:" << frameNo << ", ";
  oss << "SubframeNo:" << subframeNo;

  return oss.str ();
}

SidlelinkDiscPoolNextFrameSubframeTestCase::SidlelinkDiscPoolNextFrameSubframeTestCase (LteSlDiscResourcePoolFactory pfactory,
                                                                                        uint32_t frameNo,uint32_t subframeNo,uint32_t expectedNextFrameNo,
                                                                                        uint32_t expectedNextSubframeNo)
  : TestCase (BuildNameString (pfactory,frameNo,subframeNo)),
    m_pfactory (pfactory),
    m_frameNo (frameNo),
    m_subframeNo (subframeNo),
    m_expectedNextFrameNo (expectedNextFrameNo),
    m_expectedNextSubframeNo (expectedNextSubframeNo)
{
}

void SidlelinkDiscPoolNextFrameSubframeTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_pfactory, m_frameNo, m_subframeNo));

  LteRrcSap::SlDiscResourcePool pool = m_pfactory.CreatePool ();

  Ptr<SidelinkTxDiscResourcePool> txpool = CreateObject<SidelinkTxDiscResourcePool> ();
  txpool->SetPool (pool);

  SidelinkDiscResourcePool::SubframeInfo actualNextDiscPeriod = txpool->GetNextDiscPeriod (m_frameNo, m_subframeNo);

  NS_LOG_INFO ("Actual NextDiscPeriod = " << actualNextDiscPeriod.frameNo << "/" << actualNextDiscPeriod.subframeNo);

  NS_TEST_EXPECT_MSG_EQ (((m_expectedNextFrameNo == actualNextDiscPeriod.frameNo)&&(m_expectedNextSubframeNo == actualNextDiscPeriod.subframeNo)), true, "Expected and Actual Next Frame & Subframe numbers are not equal");
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidlelink discovery pool resource opportunity test case.
 */
class SidelinkDiscPoolResourceOpportunityTestCase : public TestCase
{
public:
  /**
   * Build name string function
   *
   * \param pfactory LteSlDiscResourcePoolFactory
   * \param psdchResourceNo PSDCH resource number
   * \returns the name string
   */
  static std::string BuildNameString (LteSlDiscResourcePoolFactory pfactory, uint32_t psdchResourceNo);

  /**
   * Constructor
   *
   * \param pfactory LteSlDiscResourcePoolFactory
   * \param psdchResourceNo PSDCH resource number
   * \param expectedFrameNo Expected frame number
   * \param expectedSubframeNo Expected subframe number
   * \param expectedRbStart Expected RB index
   * \param expectedNbCount Expected total number of RBs
   */
  SidelinkDiscPoolResourceOpportunityTestCase (LteSlDiscResourcePoolFactory pfactory, uint32_t psdchResourceNo,
                                               uint32_t expectedFrameNo, uint32_t expectedSubframeNo,
                                               uint32_t expectedRbStart, uint32_t expectedNbCount);

private:
  virtual void DoRun (void);
  LteSlDiscResourcePoolFactory m_pfactory; ///< LteSlDiscResourcePoolFactory
  uint32_t m_psdchResourceNo; ///< PSDCH resource number
  uint32_t m_expectedFrameNo; ///< Expected frame number
  uint32_t m_expectedSubframeNo; ///< Expected subframe number
  uint32_t m_expectedRbStart; ///< Expected RB start
  uint32_t m_expectedNbCount; ///< Expected total number of RBs
};

std::string SidelinkDiscPoolResourceOpportunityTestCase::BuildNameString (LteSlDiscResourcePoolFactory pfactory,uint32_t psdchResourceNo)
{
  std::ostringstream oss;

  oss << "TestCase:SidelinkDiscPoolResourceOpportunityTestCase, ";
  oss << "DiscCpLen:" << pfactory.GetDiscCpLen () << ", ";
  oss << "DiscPeriod:" << pfactory.GetDiscPeriod () << ", ";
  oss << "NumRetx:" << static_cast<int> (pfactory.GetNumRetx ()) << ", ";
  oss << "NumRepetition:" << pfactory.GetNumRepetition () << ", ";
  oss << "DiscPrbNum:" << pfactory.GetDiscPrbNum () << ", ";
  oss << "DiscPrbStart:" << pfactory.GetDiscPrbStart () << ", ";
  oss << "DiscPrbEnd:" << pfactory.GetDiscPrbEnd () << ", ";
  oss << "DiscOffset:" << pfactory.GetDiscOffset () << ", ";
  oss << "DiscBitmap:" << std::hex << pfactory.GetDiscBitmap () << std::dec << ", ";
  oss << "HaveTxParameters:" << pfactory.GetHaveTxParameters () << ", ";
  oss << "DiscTxAlpha:" << pfactory.GetDiscTxAlpha () << ", ";
  oss << "DiscTxP0:" << pfactory.GetDiscTxP0 () << ", ";
  oss << "DiscPoolSelection:" << pfactory.GetDiscPoolSelection () << ", ";
  oss << "HaveRsrpBasedPoolSelection:" << pfactory.GetHaveRsrpBasedPoolSelection () << ", ";
  oss << "RsrpThreshLow:" << pfactory.GetRsrpThreshLow () << ", ";
  oss << "RsrpThreshHigh:" << pfactory.GetRsrpThreshHigh () << ", ";
  oss << "DiscTxProbability:" << pfactory.GetDiscTxProbability () << ", ";
  oss << "PsdchResourceNo:" << psdchResourceNo;

  return oss.str ();
}

SidelinkDiscPoolResourceOpportunityTestCase::SidelinkDiscPoolResourceOpportunityTestCase (LteSlDiscResourcePoolFactory pfactory,
                                                                                          uint32_t psdchResourceNo,uint32_t expectedFrameNo,
                                                                                          uint32_t expectedSubframeNo,uint32_t expectedRbStart,
                                                                                          uint32_t expectedNbCount)
  : TestCase (BuildNameString (pfactory,psdchResourceNo)),
    m_pfactory (pfactory),
    m_psdchResourceNo (psdchResourceNo),
    m_expectedFrameNo (expectedFrameNo),
    m_expectedSubframeNo (expectedSubframeNo),
    m_expectedRbStart (expectedRbStart),
    m_expectedNbCount (expectedNbCount)
{
}

void SidelinkDiscPoolResourceOpportunityTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_pfactory, m_psdchResourceNo));

  LteRrcSap::SlDiscResourcePool pool = m_pfactory.CreatePool ();

  Ptr<SidelinkTxDiscResourcePool> txpool = CreateObject<SidelinkTxDiscResourcePool> ();
  txpool->SetPool (pool);

  NS_LOG_INFO ("Number of PSDCH resources = " << txpool->GetNPsdch ());

  std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo> txInfo = txpool->GetPsdchTransmissions (m_psdchResourceNo);

  uint32_t actualFrameNo = 0;
  uint32_t actualSubframeNo = 0;
  uint32_t actualRbStart = 0;
  uint32_t actualNbCount = 0;

  for (std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo>::iterator it = txInfo.begin (); it != txInfo.end (); it++)
    {
      actualFrameNo = it->subframe.frameNo;
      actualSubframeNo = it->subframe.subframeNo;
      actualRbStart = static_cast<uint32_t> (it->rbStart);
      actualNbCount = static_cast<uint32_t> (it->nbRb);
    }

  NS_LOG_INFO ("actualFrameNo = " << actualFrameNo);
  NS_LOG_INFO ("actualSubframeNo = " << actualSubframeNo);
  NS_LOG_INFO ("actualRbStart = " << actualRbStart);
  NS_LOG_INFO ("actualNbCount = " << actualNbCount);

  NS_TEST_EXPECT_MSG_EQ (((m_expectedFrameNo == actualFrameNo)&&(m_expectedSubframeNo == actualSubframeNo)&&(m_expectedRbStart == actualRbStart)&&(m_expectedNbCount == actualNbCount)), true, "Expected and Actual Frame no, Subframe no, RbStart and NbCount are not equal");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidlelink discovery pool resource block per subframe test case.
 */
class SidlelinkDiscPoolRbsPerSubframeTestCase : public TestCase
{
public:
  /**
   * Build name string function
   *
   * \param pfactory LteSlDiscResourcePoolFactory
   * \param frameNo Frame number
   * \param subframeNo Subframe number
   * \returns the name string
   */
  static std::string BuildNameString (LteSlDiscResourcePoolFactory pfactory,uint32_t frameNo,uint32_t subframeNo);

  /**
   * Constructor
   *
   * \param pfactory LteSlDiscResourcePoolFactory
   * \param frameNo Frame number
   * \param subframeNo Subframe number
   * \param expectedTotalRbs Expected total RBs
   * \param expectedRbIndex Indices of the expected RBs
   */
  SidlelinkDiscPoolRbsPerSubframeTestCase (LteSlDiscResourcePoolFactory pfactory,uint32_t frameNo,
                                           uint32_t subframeNo, uint32_t expectedTotalRbs,
                                           std::vector <uint32_t> expectedRbIndex);

private:
  virtual void DoRun (void);
  LteSlDiscResourcePoolFactory m_pfactory; ///< LteSlDiscResourcePoolFactory
  uint32_t m_frameNo; ///< Frame number
  uint32_t m_subframeNo; ///< Subframe number
  uint32_t m_expectedTotalRbs; ///< Expected Total RBs
  std::vector <uint32_t> m_expectedRbIndex; ///< Expected RB index
};

std::string SidlelinkDiscPoolRbsPerSubframeTestCase::BuildNameString (LteSlDiscResourcePoolFactory pfactory,
                                                                      uint32_t frameNo, uint32_t subframeNo)
{
  std::ostringstream oss;

  oss << "TestCase:SidlelinkDiscPoolRbsPerSubframeTestCase, ";
  oss << "DiscCpLen:" << pfactory.GetDiscCpLen () << ", ";
  oss << "DiscPeriod:" << pfactory.GetDiscPeriod () << ", ";
  oss << "NumRetx:" << static_cast<int> (pfactory.GetNumRetx ()) << ", ";
  oss << "NumRepetition:" << pfactory.GetNumRepetition () << ", ";
  oss << "DiscPrbNum:" << pfactory.GetDiscPrbNum () << ", ";
  oss << "DiscPrbStart:" << pfactory.GetDiscPrbStart () << ", ";
  oss << "DiscPrbEnd:" << pfactory.GetDiscPrbEnd () << ", ";
  oss << "DiscOffset:" << pfactory.GetDiscOffset () << ", ";
  oss << "DiscBitmap:" << std::hex << pfactory.GetDiscBitmap () << std::dec << ", ";
  oss << "HaveTxParameters:" << pfactory.GetHaveTxParameters () << ", ";
  oss << "DiscTxAlpha:" << pfactory.GetDiscTxAlpha () << ", ";
  oss << "DiscTxP0:" << pfactory.GetDiscTxP0 () << ", ";
  oss << "DiscPoolSelection:" << pfactory.GetDiscPoolSelection () << ", ";
  oss << "HaveRsrpBasedPoolSelection:" << pfactory.GetHaveRsrpBasedPoolSelection () << ", ";
  oss << "RsrpThreshLow:" << pfactory.GetRsrpThreshLow () << ", ";
  oss << "RsrpThreshHigh:" << pfactory.GetRsrpThreshHigh () << ", ";
  oss << "DiscTxProbability:" << pfactory.GetDiscTxProbability () << ", ";
  oss << "FrameNo:" << frameNo << ", ";
  oss << "SubframeNo:" << subframeNo;

  return oss.str ();
}

SidlelinkDiscPoolRbsPerSubframeTestCase::SidlelinkDiscPoolRbsPerSubframeTestCase (LteSlDiscResourcePoolFactory pfactory,
                                                                                  uint32_t frameNo,uint32_t subframeNo,
                                                                                  uint32_t expectedTotalRbs,
                                                                                  std::vector <uint32_t> expectedRbIndex)
  : TestCase (BuildNameString (pfactory,frameNo,subframeNo)),
    m_pfactory (pfactory),
    m_frameNo (frameNo),
    m_subframeNo (subframeNo),
    m_expectedTotalRbs (expectedTotalRbs),
    m_expectedRbIndex (expectedRbIndex)
{
}

void SidlelinkDiscPoolRbsPerSubframeTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_pfactory, m_frameNo, m_subframeNo));

  LteRrcSap::SlDiscResourcePool pool = m_pfactory.CreatePool ();

  Ptr<SidelinkTxDiscResourcePool> txpool = CreateObject<SidelinkTxDiscResourcePool> ();
  txpool->SetPool (pool);

  std::vector<uint32_t> totalRbs = txpool->GetPsdchOpportunities (m_frameNo, m_subframeNo);

  NS_LOG_INFO ("Actual total number of RBs = " << totalRbs.size () <<
               " Expected total number of Rbs = " << m_expectedTotalRbs);

  NS_TEST_EXPECT_MSG_EQ (totalRbs.size (), m_expectedTotalRbs, "Total number of RBs are not as expected");

  bool flag = true;

  if (totalRbs.size () > 0)
    {
      if (totalRbs != m_expectedRbIndex)
        {
          flag = false;
        }
    }

  NS_TEST_ASSERT_MSG_EQ (flag, true, " The indicies of resource blocks are not equal to the expected indicies");
}


/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidlelink discovery pool test suite.
 */
class SidelinkDiscPoolTestSuite : public TestSuite
{
public:
  SidelinkDiscPoolTestSuite ();
};

SidelinkDiscPoolTestSuite::SidelinkDiscPoolTestSuite ()
  : TestSuite ("sidelink-disc-pool", UNIT)
{
  // LogComponentEnable ("TestSidelinkDiscPool", LOG_LEVEL_ALL);

  LteSlDiscResourcePoolFactory pfactory;

  pfactory.SetDiscCpLen ("NORMAL");
  pfactory.SetDiscPeriod ("rf32");
  pfactory.SetNumRetx (0);
  pfactory.SetNumRepetition (1);
  pfactory.SetDiscPrbNum (1);
  pfactory.SetDiscPrbStart (10);
  pfactory.SetDiscPrbEnd (11);
  pfactory.SetDiscOffset (80);
  pfactory.SetDiscBitmap (0x0000000003);
  pfactory.SetHaveTxParameters (true);
  pfactory.SetDiscTxAlpha ("al09");
  pfactory.SetDiscTxP0 (-40);
  pfactory.SetDiscPoolSelection ("RSRPBASED");
  pfactory.SetHaveRsrpBasedPoolSelection (true);
  pfactory.SetRsrpThreshLow (0);
  pfactory.SetRsrpThreshHigh (7);
  pfactory.SetDiscTxProbability ("p100");


  //SidlelinkDiscPoolNextFrameSubframeTestCase Input Format:
  //pfactory,frameNo,subframeNo,expectedNextFrameNo,expectedNextSubframeNo

  //The current subframe is less than the offset value. The next period will be the first one and will start at 80 ms.
  AddTestCase (new SidlelinkDiscPoolNextFrameSubframeTestCase (pfactory, 0, 5, 8, 0),TestCase::QUICK);
  AddTestCase (new SidlelinkDiscPoolNextFrameSubframeTestCase (pfactory, 8, 0, 40, 0),TestCase::QUICK);
  AddTestCase (new SidlelinkDiscPoolNextFrameSubframeTestCase (pfactory, 40, 0, 72, 0),TestCase::QUICK);

  //Move to the end of the frameNo range to test roll over
  AddTestCase (new SidlelinkDiscPoolNextFrameSubframeTestCase (pfactory, 1023, 0, 8, 0),TestCase::QUICK);
  //Frame and subframe number from the on going discovery period
  AddTestCase (new SidlelinkDiscPoolNextFrameSubframeTestCase (pfactory, 800, 0, 808, 0),TestCase::QUICK);


  //SidelinkDiscPoolResourceOpportunityTestCase Input Format:
  //pfactory,psdchResourceNo,expectedFrameNo,expectedSubframeNo,expectedRbStart,expectedNbCount

  //psdchResourceNo: Input 0 for 1st resource, 1 for 2nd resource
  //Discovery transmissions one subframe for each transmission
  //Since we have only 2 subframes, we have only two transmissions

  //psdchResourceNo:0
  AddTestCase (new SidelinkDiscPoolResourceOpportunityTestCase (pfactory, 0, 0, 0, 10, 2),TestCase::QUICK);
  //psdchResourceNo:1
  AddTestCase (new SidelinkDiscPoolResourceOpportunityTestCase (pfactory, 1, 0, 1, 10, 2),TestCase::QUICK);

  std::vector<uint32_t> rbIndex;
  //If we go to 80 ms (frame 8, subframe 0), we should be on the first subframe of the discovery channel
  // and RB 10 and 11 should be assigned for transmission
  rbIndex.push_back (10);
  rbIndex.push_back (11);
  AddTestCase (new SidlelinkDiscPoolRbsPerSubframeTestCase (pfactory, 8, 0, 2, rbIndex),TestCase::QUICK);
  //If we go to 81 ms (frame 8, subframe 1), we should be on the second subframe of the discovery channel
  //and RB 10 and 11 should be assigned for transmission
  AddTestCase (new SidlelinkDiscPoolRbsPerSubframeTestCase (pfactory, 8, 1, 2, rbIndex),TestCase::QUICK);

  //If we go to 82 ms (frame 8, subframe 2), we should be outside the discovery channel and there
  //should be no resource block assignment
  rbIndex.clear ();
  AddTestCase (new SidlelinkDiscPoolRbsPerSubframeTestCase (pfactory, 8, 2, 0, rbIndex),TestCase::QUICK);
}

static SidelinkDiscPoolTestSuite staticSidelinkDiscPoolTestSuite;

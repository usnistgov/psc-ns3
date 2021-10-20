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
#include "ns3/lte-sl-resource-pool-factory.h"
#include <ns3/log.h>
#include <ns3/test.h>
#include <string>
#include <iostream>
#include <sstream>

NS_LOG_COMPONENT_DEFINE ("TestSidelinkCommPool");

using namespace ns3;

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink communication pool next frame subframe test case.
 */
class SidelinkCommPoolNextFrameSubframeTestCase : public TestCase
{
public:
  /**
   * Build name string function
   *
   * \param pfactory LteSlResourcePoolFactory
   * \param frameNo Frame number
   * \param subframeNo Subframe number
   * \returns the name string
   */
  static std::string BuildNameString (LteSlResourcePoolFactory pfactory, uint32_t frameNo, uint32_t subframeNo);

  /**
   * Constructor
   *
   * \param pfactory LteSlResourcePoolFactory
   * \param frameNo Frame number
   * \param subframeNo Subframe number
   * \param expectedNextFrameNo Expected frame number
   * \param expectedNextSubframeNo Expected subframe number
   */
  SidelinkCommPoolNextFrameSubframeTestCase (LteSlResourcePoolFactory pfactory, uint32_t frameNo,
                                             uint32_t subframeNo, uint32_t expectedNextFrameNo, uint32_t expectedNextSubframeNo);
private:
  virtual void DoRun (void);
  LteSlResourcePoolFactory m_pfactory; ///< LteSlResourcePoolFactory
  uint32_t m_frameNo; ///< Frame number
  uint32_t m_subframeNo; ///< Subframe number
  uint32_t m_expectedNextFrameNo; ///< Expected next frame number
  uint32_t m_expectedNextSubframeNo; ///< Expected next subframe number
};

std::string SidelinkCommPoolNextFrameSubframeTestCase::BuildNameString (LteSlResourcePoolFactory pfactory, uint32_t frameNo, uint32_t subframeNo)
{
  std::ostringstream oss;

  oss << "TestCase:SidelinkCommPoolNextFrameSubframeTestCase, ";
  oss << "ControlPeriod:" << pfactory.GetControlPeriod () << ", ";
  oss << "ControlBitmap:" << std::hex << pfactory.GetControlBitmap () << std::dec << ", ";
  oss << "ControlPrbNum:" << (uint16_t) pfactory.GetControlPrbNum () << ", ";
  oss << "ControlPrbStart:" << (uint16_t) pfactory.GetControlPrbStart () << ", ";
  oss << "ControlPrbEnd:" << (uint16_t) pfactory.GetControlPrbEnd () << ", ";
  oss << "ControlOffset:" << pfactory.GetControlOffset () << ", ";
  oss << "DataBitmap:" << std::hex << pfactory.GetDataBitmap () << std::dec << ", ";
  oss << "DataPrbNum:" << (uint16_t) pfactory.GetDataPrbNum () << ", ";
  oss << "DataPrbStart:" << (uint16_t) pfactory.GetDataPrbStart () << ", ";
  oss << "DataPrbEnd:" << (uint16_t) pfactory.GetDataPrbEnd () << ", ";
  oss << "DataOffset:" << pfactory.GetDataOffset () << ", ";
  oss << "FrameNo:" << frameNo << ", ";
  oss << "SubframeNo:" << subframeNo;

  return oss.str ();
}

SidelinkCommPoolNextFrameSubframeTestCase::SidelinkCommPoolNextFrameSubframeTestCase (LteSlResourcePoolFactory pfactory,
                                                                                      uint32_t frameNo, uint32_t subframeNo,
                                                                                      uint32_t expectedNextFrameNo, uint32_t expectedNextSubframeNo)
  : TestCase (BuildNameString (pfactory, frameNo, subframeNo)),
    m_pfactory (pfactory),
    m_frameNo (frameNo),
    m_subframeNo (subframeNo),
    m_expectedNextFrameNo (expectedNextFrameNo),
    m_expectedNextSubframeNo (expectedNextSubframeNo)
{
}

void SidelinkCommPoolNextFrameSubframeTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_pfactory, m_frameNo, m_subframeNo));

  LteRrcSap::SlCommResourcePool pool = m_pfactory.CreatePool ();
  Ptr<SidelinkTxCommResourcePool> txpool = CreateObject<SidelinkTxCommResourcePool> ();
  txpool->SetPool (pool);

  SidelinkCommResourcePool::SubframeInfo actualNextScPeriod = txpool->GetNextScPeriod (m_frameNo, m_subframeNo);

  NS_LOG_INFO ("Actual NextScPeriod = " << actualNextScPeriod.frameNo << "/" << actualNextScPeriod.subframeNo);

  NS_TEST_EXPECT_MSG_EQ (((m_expectedNextFrameNo == actualNextScPeriod.frameNo) && (m_expectedNextSubframeNo == actualNextScPeriod.subframeNo)), true, "Expected and Actual Next Frame & Subframe numbers are not equal");
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink pool resource opportunity test case.
 */
class SidelinkCommPoolResourceOpportunityTestCase : public TestCase
{
public:
  /**
   * Build name string function
   *
   * \param pfactory LteSlResourcePoolFactory
   * \param pscchResourceNo PSCCH resource number
   * \param pscchTransmissionNo PSCCH transmission number
   * \returns the name string
   */
  static std::string BuildNameString (LteSlResourcePoolFactory pfactory, uint32_t pscchResourceNo,uint32_t pscchTransmissionNo);

  /**
   * Constructor
   *
   * \param pfactory LteSlResourcePoolFactory
   * \param pscchResourceNo PSCCH resource number
   * \param pscchTransmissionNo PSCCH transmission number
   * \param expectedFrameNo Expected frame number
   * \param expectedSubframeNo Expected subframe number
   * \param expectedRbStart Expected index of the starting RB
   * \param expectedNbCount Expected total number of RBs
   */
  SidelinkCommPoolResourceOpportunityTestCase (LteSlResourcePoolFactory pfactory, uint32_t pscchResourceNo, uint32_t pscchTransmissionNo,
                                               uint32_t expectedFrameNo, uint32_t expectedSubframeNo, uint32_t expectedRbStart, uint32_t expectedNbCount);

private:
  virtual void DoRun (void);
  LteSlResourcePoolFactory m_pfactory; ///< LteSlResourcePoolFactory
  uint32_t m_pscchResourceNo; ///< PSCCH resource number
  uint32_t m_pscchTransmissionNo; ///< PSCCH transmission number
  uint32_t m_expectedFrameNo; ///< Expected frame number
  uint32_t m_expectedSubframeNo; ///< Expected subframe number
  uint32_t m_expectedRbStart; ///< Expected RB index
  uint32_t m_expectedNbCount; ///< Expected total number of RBs
};

std::string SidelinkCommPoolResourceOpportunityTestCase::BuildNameString (LteSlResourcePoolFactory pfactory, uint32_t pscchResourceNo, uint32_t pscchTransmissionNo)
{
  std::ostringstream oss;

  oss << "TestCase:SidelinkCommPoolResourceOpportunityTestCase, ";
  oss << "ControlPeriod:" << pfactory.GetControlPeriod () << ", ";
  oss << "ControlBitmap:" << std::hex << pfactory.GetControlBitmap () << std::dec << ", ";
  oss << "ControlPrbNum:" << (uint16_t) pfactory.GetControlPrbNum () << ", ";
  oss << "ControlPrbStart:" << (uint16_t) pfactory.GetControlPrbStart () << ", ";
  oss << "ControlPrbEnd:" << (uint16_t) pfactory.GetControlPrbEnd () << ", ";
  oss << "ControlOffset:" << pfactory.GetControlOffset () << ", ";
  oss << "DataBitmap:" << std::hex << pfactory.GetDataBitmap () << std::dec << ", ";
  oss << "DataPrbNum:" << (uint16_t) pfactory.GetDataPrbNum () << ", ";
  oss << "DataPrbStart:" << (uint16_t) pfactory.GetDataPrbStart () << ", ";
  oss << "DataPrbEnd:" << (uint16_t) pfactory.GetDataPrbEnd () << ", ";
  oss << "DataOffset:" << pfactory.GetDataOffset () << ", ";
  oss << "PscchResourceNo:" << pscchResourceNo << ", ";
  oss << "PscchTransmissionNo:" << pscchTransmissionNo;

  return oss.str ();
}

SidelinkCommPoolResourceOpportunityTestCase::SidelinkCommPoolResourceOpportunityTestCase (LteSlResourcePoolFactory pfactory, uint32_t pscchResourceNo,
                                                                                          uint32_t pscchTransmissionNo, uint32_t expectedFrameNo, uint32_t expectedSubframeNo,
                                                                                          uint32_t expectedRbStart,uint32_t expectedNbCount)

  : TestCase (BuildNameString (pfactory, pscchResourceNo, pscchTransmissionNo)),
    m_pfactory (pfactory),
    m_pscchResourceNo (pscchResourceNo),
    m_pscchTransmissionNo (pscchTransmissionNo),
    m_expectedFrameNo (expectedFrameNo),
    m_expectedSubframeNo (expectedSubframeNo),
    m_expectedRbStart (expectedRbStart),
    m_expectedNbCount (expectedNbCount)
{
}

void SidelinkCommPoolResourceOpportunityTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_pfactory, m_pscchResourceNo, m_pscchTransmissionNo));

  LteRrcSap::SlCommResourcePool pool = m_pfactory.CreatePool ();
  Ptr<SidelinkTxCommResourcePool> txpool = CreateObject<SidelinkTxCommResourcePool> ();
  txpool->SetPool (pool);

  NS_LOG_INFO ("Number of PSCCH resources = " << txpool->GetNPscch ());


  std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> txInfo = txpool->GetPscchTransmissions (m_pscchResourceNo);

  uint32_t ctr = 0;
  uint32_t actualFrameNo = 0;
  uint32_t actualSubframeNo = 0;
  uint32_t actualRbStart = 0;
  uint32_t actualRbCount = 0;

  for (std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator it = txInfo.begin (); it != txInfo.end (); it++)
    {
      if (ctr == m_pscchTransmissionNo)
        {
          actualFrameNo = it->subframe.frameNo;
          actualSubframeNo = it->subframe.subframeNo;
          actualRbStart = static_cast<uint32_t> (it->rbStart);
          actualRbCount = static_cast<uint32_t> (it->nbRb);
          break;
        }
      ctr++;
    }

  NS_LOG_INFO ("actualFrameNo = " << actualFrameNo);
  NS_LOG_INFO ("actualSubframeNo = " << actualSubframeNo);
  NS_LOG_INFO ("actualRbStart = " << actualRbStart);
  NS_LOG_INFO ("actualRbCount = " << actualRbCount);

  NS_TEST_EXPECT_MSG_EQ (((m_expectedFrameNo == actualFrameNo) && (m_expectedSubframeNo == actualSubframeNo) && (m_expectedRbStart == actualRbStart) && (m_expectedNbCount == actualRbCount)), true, "Expected and Actual Frame no, Subframe no, rbStart and NbCount are not equal");
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink communication pool subframe opportunity test case.
 */
class SidelinkCommPoolSubframeOpportunityTestCase : public TestCase
{
public:

  /**
   * Build name string function
   *
   * \param pfactory LteSlResourcePoolFactory
   * \param frameNo Frame number
   * \param subframeNo Subframe number
   * \returns the name string
   */
  static std::string BuildNameString (LteSlResourcePoolFactory pfactory, uint32_t frameNo, uint32_t subframeNo);

  /**
   * Constructor
   *
   * \param pfactory LteSlResourcePoolFactory
   * \param frameNo Frame number
   * \param subframeNo Subframe number
   * \param expectedPscchSubframeOp1 Expected PSCCH subframe opportunity 1
   * \param expectedPscchSubframeOp2 Expected PSCCH subframe opportunity 2
   */
  SidelinkCommPoolSubframeOpportunityTestCase (LteSlResourcePoolFactory pfactory, uint32_t frameNo, uint32_t subframeNo,
                                               int32_t expectedPscchSubframeOp1, int32_t expectedPscchSubframeOp2);

private:
  virtual void DoRun (void);
  LteSlResourcePoolFactory m_pfactory; ///< LteSlResourcePoolFactory
  uint32_t m_frameNo; ///< Frame number
  uint32_t m_subframeNo; ///< Subframe number
  int32_t m_expectedPscchSubframeOp1; ///< Expected PSCCH subframe opportunity 1
  int32_t m_expectedPscchSubframeOp2; ///< Expected PSCCH subframe opportunity 2
};

std::string SidelinkCommPoolSubframeOpportunityTestCase::BuildNameString (LteSlResourcePoolFactory pfactory, uint32_t frameNo,uint32_t subframeNo)
{
  std::ostringstream oss;
  oss << "TestCase:SidelinkCommPoolSubframeOpportunityTestCase, ";
  oss << "ControlPeriod:" << pfactory.GetControlPeriod () << ", ";
  oss << "ControlBitmap:" << std::hex << pfactory.GetControlBitmap () << std::dec << ", ";
  oss << "ControlPrbNum:" << (uint16_t) pfactory.GetControlPrbNum () << ", ";
  oss << "ControlPrbStart:" << (uint16_t) pfactory.GetControlPrbStart () << ", ";
  oss << "ControlPrbEnd:" << (uint16_t) pfactory.GetControlPrbEnd () << ", ";
  oss << "ControlOffset:" << pfactory.GetControlOffset () << ", ";
  oss << "DataBitmap:" << std::hex << pfactory.GetDataBitmap () << std::dec << ", ";
  oss << "DataPrbNum:" << (uint16_t) pfactory.GetDataPrbNum () << ", ";
  oss << "DataPrbStart:" << (uint16_t) pfactory.GetDataPrbStart () << ", ";
  oss << "DataPrbEnd:" << (uint16_t) pfactory.GetDataPrbEnd () << ", ";
  oss << "DataOffset:" << pfactory.GetDataOffset () << ", ";
  oss << "FrameNo:" << frameNo << ", ";
  oss << "SubframeNo:" << subframeNo;

  return oss.str ();
}

SidelinkCommPoolSubframeOpportunityTestCase::SidelinkCommPoolSubframeOpportunityTestCase (LteSlResourcePoolFactory pfactory, uint32_t frameNo,
                                                                                          uint32_t subframeNo, int32_t expectedPscchSubframeOp1,
                                                                                          int32_t expectedPscchSubframeOp2)

  : TestCase (BuildNameString (pfactory, frameNo, subframeNo)),
    m_pfactory (pfactory),
    m_frameNo (frameNo),
    m_subframeNo (subframeNo),
    m_expectedPscchSubframeOp1 (expectedPscchSubframeOp1),
    m_expectedPscchSubframeOp2 (expectedPscchSubframeOp2)
{
}

void SidelinkCommPoolSubframeOpportunityTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_pfactory, m_frameNo, m_subframeNo));

  LteRrcSap::SlCommResourcePool pool = m_pfactory.CreatePool ();
  Ptr<SidelinkTxCommResourcePool> txpool = CreateObject<SidelinkTxCommResourcePool> ();
  txpool->SetPool (pool);

  std::list<uint8_t> rbs;
  rbs = txpool->GetPscchOpportunities (m_frameNo, m_subframeNo);

  bool flag = true;
  int ctr = 0;
  int32_t actualPscchSubframeOp1 = -1;
  int32_t actualPscchSubframeOp2 = -1;

  if (rbs.size () > 0)
    {
      for (std::list<uint8_t>::iterator it = rbs.begin (); it != rbs.end (); it++)
        {
          if (ctr == 0)
            {
              actualPscchSubframeOp1 = static_cast<uint32_t> (*it);
              if (actualPscchSubframeOp1 != m_expectedPscchSubframeOp1)
                {
                  flag = false;
                }
            }
          if (ctr == 1)
            {
              actualPscchSubframeOp2 = static_cast<uint32_t> (*it);
              if (actualPscchSubframeOp2 != m_expectedPscchSubframeOp2)
                {
                  flag = false;
                }
            }

          ctr = ctr + 1;
        }
      NS_LOG_INFO ("actualPscchSubframeOp1 = " << actualPscchSubframeOp1);
      NS_LOG_INFO ("actualPscchSubframeOp2 = " << actualPscchSubframeOp2);
    }
  else
    {
      NS_LOG_INFO ("No opportunities");
      actualPscchSubframeOp1 = -1;
      actualPscchSubframeOp2 = -1;
      if ((actualPscchSubframeOp1 != m_expectedPscchSubframeOp1) || (actualPscchSubframeOp2 != m_expectedPscchSubframeOp2))
        {
          flag = false;
        }
    }

  NS_TEST_EXPECT_MSG_EQ (flag, true, "Expected and Actual PSCCH opportunities for subframe are not equal");
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink communication pool PSSCH test case.
 */
class SidelinkCommPoolPsschTestCase : public TestCase
{
public:
  /**
   * Build name string function
   *
   * \param pfactory LteSlResourcePoolFactory
   * \param frameNo Frame number
   * \param subframeNo Subframe number
   * \param iTrp Time resource pattern index
   * \param rbStart Starting RB index
   * \param rbLen Total number of RBs
   * \param psschTransmissionNo PSSCH transmission number
   * \returns the name string
   */
  static std::string BuildNameString (LteSlResourcePoolFactory pfactory, uint32_t frameNo, uint32_t subframeNo,
                                      uint32_t iTrp, uint32_t rbStart, uint32_t rbLen, uint32_t psschTransmissionNo);
  /**
   * Build name string function
   *
   * \param pfactory LteSlResourcePoolFactory
   * \param frameNo Frame number
   * \param subframeNo Subframe number
   * \param iTrp Time resource pattern index
   * \param rbStart Starting RB index
   * \param rbLen Total number of RBs
   * \param psschTransmissionNo PSSCH transmission number
   * \param expectedFrameNo Expected frame number
   * \param expectedSubframeNo Expected subframe number
   * \param expectedRbStart Expected RB index
   * \param expectedNbCount Expected total number of RBs
   */
  SidelinkCommPoolPsschTestCase (LteSlResourcePoolFactory pfactory,uint32_t frameNo, uint32_t subframeNo, uint32_t iTrp,
                                 uint32_t rbStart, uint32_t rbLen, uint32_t psschTransmissionNo, uint32_t expectedFrameNo,
                                 uint32_t expectedSubframeNo, uint32_t expectedRbStart, uint32_t expectedNbCount);

private:
  virtual void DoRun (void);
  LteSlResourcePoolFactory m_pfactory; ///< LteSlResourcePoolFactory
  uint32_t m_frameNo; ///< Frame number
  uint32_t m_subframeNo; ///< Subframe number
  uint32_t m_iTrp; ///< Time resource pattern index
  uint32_t m_rbStart; ///< Starting RB index
  uint32_t m_rbLen; ///< Total number of RBs
  uint32_t m_psschTransmissionNo; ///< PSCCH transmission number
  uint32_t m_expectedFrameNo; ///< Expected Frame number
  uint32_t m_expectedSubframeNo; ///< Expected subframe number
  uint32_t m_expectedRbStart; ///< Expected RB index
  uint32_t m_expectedNbCount; ///< Expected total number of RBs
};

std::string SidelinkCommPoolPsschTestCase::BuildNameString (LteSlResourcePoolFactory pfactory, uint32_t frameNo,
                                                            uint32_t subframeNo, uint32_t iTrp, uint32_t rbStart,
                                                            uint32_t rbLen, uint32_t psschTransmissionNo)
{
  std::ostringstream oss;

  oss << "TestCase:SidelinkCommPoolPsschTestCase, ";
  oss << "ControlPeriod:" << pfactory.GetControlPeriod () << ", ";
  oss << "ControlBitmap:" << std::hex << pfactory.GetControlBitmap () << std::dec << ", ";
  oss << "ControlPrbNum:" << (uint16_t) pfactory.GetControlPrbNum () << ", ";
  oss << "ControlPrbStart:" << (uint16_t) pfactory.GetControlPrbStart () << ", ";
  oss << "ControlPrbEnd:" << (uint16_t) pfactory.GetControlPrbEnd () << ", ";
  oss << "ControlOffset:" << pfactory.GetControlOffset () << ", ";
  oss << "DataBitmap:" << std::hex << pfactory.GetDataBitmap () << std::dec << ", ";
  oss << "DataPrbNum:" << (uint16_t) pfactory.GetDataPrbNum () << ", ";
  oss << "DataPrbStart:" << (uint16_t) pfactory.GetDataPrbStart () << ", ";
  oss << "DataPrbEnd:" << (uint16_t) pfactory.GetDataPrbEnd () << ", ";
  oss << "DataOffset:" << pfactory.GetDataOffset () << ", ";
  oss << "FrameNo:" << frameNo << ", ";
  oss << "SubFrameNo:" << subframeNo << ", ";
  oss << "ITrp:" << iTrp << ", ";
  oss << "RbStart:" << rbStart << ", ";
  oss << "RbLen:" << rbLen << ", ";
  oss << "HoppingInfo:" << (uint16_t) pfactory.GetDataHoppingInfo () << ", ";
  oss << "PsschTransmissionNo:" << psschTransmissionNo;

  return oss.str ();
}

SidelinkCommPoolPsschTestCase::SidelinkCommPoolPsschTestCase (LteSlResourcePoolFactory pfactory, uint32_t frameNo,
                                                              uint32_t subframeNo, uint32_t iTrp,uint32_t rbStart,
                                                              uint32_t rbLen,uint32_t psschTransmissionNo,
                                                              uint32_t expectedFrameNo, uint32_t expectedSubframeNo,
                                                              uint32_t expectedRbStart, uint32_t expectedNbCount)
  : TestCase (BuildNameString (pfactory, frameNo, subframeNo, iTrp, rbStart, rbLen, psschTransmissionNo)),
    m_pfactory (pfactory),
    m_frameNo (frameNo),
    m_subframeNo (subframeNo),
    m_iTrp (iTrp),
    m_rbStart (rbStart),
    m_rbLen (rbLen),
    m_psschTransmissionNo (psschTransmissionNo),
    m_expectedFrameNo (expectedFrameNo),
    m_expectedSubframeNo (expectedSubframeNo),
    m_expectedRbStart (expectedRbStart),
    m_expectedNbCount (expectedNbCount)
{
}

void SidelinkCommPoolPsschTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_pfactory, m_frameNo, m_subframeNo, m_iTrp, m_rbStart, m_rbLen, m_psschTransmissionNo));

  LteRrcSap::SlCommResourcePool pool = m_pfactory.CreatePool ();
  Ptr<SidelinkTxCommResourcePool> txpool = CreateObject<SidelinkTxCommResourcePool> ();
  txpool->SetPool (pool);

  SidelinkCommResourcePool::SubframeInfo nextScPeriod = txpool->GetNextScPeriod (m_frameNo, m_subframeNo);
  std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> txInfo = txpool->GetPsschTransmissions (nextScPeriod, m_iTrp, m_rbStart, m_rbLen);

  NS_LOG_INFO ("PSSCH transmissions = " << txInfo.size ());

  uint32_t ctr = 0;
  uint32_t actualFrameNo = 0;
  uint32_t actualSubframeNo = 0;
  uint32_t actualRbStart = 0;
  uint32_t actualRbCount = 0;

  for (std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator it = txInfo.begin (); it != txInfo.end (); it++)
    {
      if (ctr == m_psschTransmissionNo)
        {
          actualFrameNo = it->subframe.frameNo;
          actualSubframeNo = it->subframe.subframeNo;
          actualRbStart = static_cast<uint32_t> (it->rbStart);
          actualRbCount = static_cast<uint32_t> (it->nbRb);
          break;
        }
      ctr++;
    }

  NS_LOG_INFO ("actualFrameNo = " << actualFrameNo);
  NS_LOG_INFO ("actualSubframeNo = " << actualSubframeNo);
  NS_LOG_INFO ("actualRbStart = " << actualRbStart);

  NS_TEST_EXPECT_MSG_EQ (((m_expectedFrameNo == actualFrameNo) && (m_expectedSubframeNo == actualSubframeNo) && (m_expectedRbStart == actualRbStart) && (m_expectedNbCount == actualRbCount)), true, "Expected and Actual Frame no, Subframe no, and rbStart for PSSCH are not equal");
}

/**
 * \ingroup lte-test
 * \ingroup tests
 *
 * \brief Sidelink Communication pool test suite.
 */
class SidelinkCommPoolTestSuite : public TestSuite
{
public:
  SidelinkCommPoolTestSuite ();
};


SidelinkCommPoolTestSuite::SidelinkCommPoolTestSuite ()
  : TestSuite ("sidelink-comm-pool", UNIT)
{
  // LogComponentEnable ("TestSidelinkCommPool", LOG_LEVEL_ALL);

  LteSlResourcePoolFactory pfactory;
  pfactory.SetControlPeriod ("sf120");
  pfactory.SetControlBitmap (0x0000000003);
  pfactory.SetControlPrbNum (1);
  pfactory.SetControlPrbStart (10);
  pfactory.SetControlPrbEnd (40);
  pfactory.SetControlOffset (80);
  pfactory.SetDataBitmap (0xCCCCCCCCCC);
  pfactory.SetDataPrbNum (10);
  pfactory.SetDataPrbStart (0);
  pfactory.SetDataPrbEnd (49);
  pfactory.SetDataOffset (8);

  //SidelinkCommPoolNextFrameSubframeTestCase Input Format:
  //pfactory,frameNo,subframeNo,expectedNextFrameNo,expectedNextSubframeNo

  //The current subframe is less than the offset value. The next period will be the first one and will start at 80 ms.
  AddTestCase (new SidelinkCommPoolNextFrameSubframeTestCase (pfactory,0,5,8,0),TestCase::QUICK);

  //Move to the end of the frameNo range to test roll over
  AddTestCase (new SidelinkCommPoolNextFrameSubframeTestCase (pfactory,1023,0,8,0),TestCase::QUICK);

  //Other example: expected next period: 1004/0
  AddTestCase (new SidelinkCommPoolNextFrameSubframeTestCase (pfactory,1002,0,1004,0),TestCase::QUICK);



  //SidelinkCommPoolResourceOpportunityTestCase Input Format:
  //pfactory,pscchResourceNo,pscchTransmissionNo,expectedFrameNo,expectedSubframeNo,expectedRbStart,expectedNbCount

  //pscchResourceNo: Input 0 for 1st resource, 1 for 2nd resource
  //pscchTransmissionNo: Input 0 for 1st transmission, 1 for 2nd transmission

  //PscchResourceNo:0,PscchTransmissionNo:0
  AddTestCase (new SidelinkCommPoolResourceOpportunityTestCase (pfactory,0,0,0,0,10,1),TestCase::QUICK);
  //PscchResourceNo:0,PscchTransmissionNo:1
  AddTestCase (new SidelinkCommPoolResourceOpportunityTestCase (pfactory,0,1,0,1,40,1),TestCase::QUICK);
  //PscchResourceNo:1,PscchTransmissionNo:0
  AddTestCase (new SidelinkCommPoolResourceOpportunityTestCase (pfactory,1,0,0,0,40,1),TestCase::QUICK);
  //PscchResourceNo:1,PscchTransmissionNo:1
  AddTestCase (new SidelinkCommPoolResourceOpportunityTestCase (pfactory,1,1,0,1,10,1),TestCase::QUICK);

  //SidelinkCommPoolSubframeOpportunityTestCase Input Format:
  //pfactory,frameNo,subframeNo,expectedPscchSubframeOp1,expectedPscchSubframeOp2

  //For no opportunities, use expectedPscchSubframeOp1 = -1 and expectedPscchSubframeOp2 = -1

  //If we go to 80 ms (frame 8, subframe 0), we should be on the first subframe of the control channel
  AddTestCase (new SidelinkCommPoolSubframeOpportunityTestCase (pfactory, 8,0,10,40),TestCase::QUICK);
  //If we go to 81 ms (frame 8, subframe 1), we should be on the second subframe of the control channel
  AddTestCase (new SidelinkCommPoolSubframeOpportunityTestCase (pfactory, 8,1,10,40),TestCase::QUICK);
  //If we go to 82 ms (frame 8, subframe 2), we should be outside the control channel
  AddTestCase (new SidelinkCommPoolSubframeOpportunityTestCase (pfactory, 8,2,-1,-1),TestCase::QUICK);

  //SidelinkCommPoolPsschTestCase Input Format:
  //pfactory,frameNo,subframeNo,iTrp,rbStart,rbLen,psschTransmissionNo,expectedFrameNo,expectedSubframeNo,expectedRbStart,expectedNbCount

  //With the following configuration, only 1 bit is set in the TRP bitmap (the 6th D2D subframe)
  //Because the bitmap for data is 0011001100110011001100110011001100110011
  //                                          |
  //                                       6th D2D subframe
  //The first transmission should occur on: periodStart (80) + dataOffset (8) + TRP subframe (11) = 99
  //The second one (and followings) will occur every 16 ms

  //psschTransmissionNo: Input 0 for 1st transmission, 1 for 2nd transmission, 2 for 3rd transmission, 3 for 4th transmission

  //psschTransmissionNo:0
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,6,0,9,9,2,6),TestCase::QUICK);
  //psschTransmissionNo:1
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,6,1,11,5,2,6),TestCase::QUICK);
  //psschTransmissionNo:2
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,6,2,13,1,2,6),TestCase::QUICK);
  //psschTransmissionNo:3
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,6,3,14,7,2,6),TestCase::QUICK);

  //Frequency hopping tests
  //Type 1: constant hopping
  pfactory.SetDataHoppingInfo(0); //Hopping info: 0
  //psschTransmissionNo:0, 1st
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,0,9,9,2,3),TestCase::QUICK);
  //psschTransmissionNo:1, 2nd
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,1,11,5,7,3),TestCase::QUICK);
  //psschTransmissionNo:2, 3rd
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,2,13,1,2,3),TestCase::QUICK);
  //psschTransmissionNo:3, 4th
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,3,14,7,7,3),TestCase::QUICK);
  
  pfactory.SetDataHoppingInfo(1); //Hopping info: 1
  //psschTransmissionNo:0, 1st
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,0,9,9,2,3),TestCase::QUICK);
  //psschTransmissionNo:1, 2nd
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,1,11,5,17,3),TestCase::QUICK);
  //psschTransmissionNo:2, 3rd
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,2,13,1,2,3),TestCase::QUICK);
  //psschTransmissionNo:3, 4th
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,3,14,7,17,3),TestCase::QUICK);

  pfactory.SetDataHoppingInfo(2); //Hopping info: 2
  //psschTransmissionNo:0, 1st
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,0,9,9,2,3),TestCase::QUICK);
  //psschTransmissionNo:1, 2nd
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,1,11,5,12,3),TestCase::QUICK);
  //psschTransmissionNo:2, 3rd
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,2,13,1,2,3),TestCase::QUICK);
  //psschTransmissionNo:3, 4th
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,3,14,7,12,3),TestCase::QUICK);

  //Type 2: pseudo-random hopping
  pfactory.SetDataHoppingInfo(3); //Hopping info: 3
  pfactory.SetDataHoppingParameter(0); //Hopping seed
  pfactory.SetDataHoppingSubbands ("ns2"); //2 sub-bands
  //psschTransmissionNo:0, 1st
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,0,9,9,42,3),TestCase::QUICK);
  //psschTransmissionNo:1, 2nd
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,1,11,5,45,3),TestCase::QUICK);
  //psschTransmissionNo:2, 3rd
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,2,13,1,5,3),TestCase::QUICK);
  //psschTransmissionNo:3, 4th
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,3,14,7,5,3),TestCase::QUICK);
  
  pfactory.SetDataHoppingSubbands ("ns4"); //4 sub-bands
  //psschTransmissionNo:0, 1st
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,0,9,9,47,3),TestCase::QUICK);
  //psschTransmissionNo:1, 2nd
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,1,11,5,45,3),TestCase::QUICK);
  //psschTransmissionNo:2, 3rd
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,2,13,1,5,3),TestCase::QUICK);
  //psschTransmissionNo:3, 4th
  AddTestCase (new SidelinkCommPoolPsschTestCase (pfactory,0,5,5,2,3,3,14,7,0,3),TestCase::QUICK);}

static SidelinkCommPoolTestSuite staticSidelinkCommPoolTestSuite;

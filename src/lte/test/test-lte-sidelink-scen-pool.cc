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

NS_LOG_COMPONENT_DEFINE ("LteSidelinkScenPool");

using namespace ns3;

class LteSidelinkScenPoolNextFrameSubframeTestCase : public TestCase
{
public:
  static std::string BuildNameString (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset,uint32_t FrameNo,uint32_t SubframeNo);
  LteSidelinkScenPoolNextFrameSubframeTestCase (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset,uint32_t FrameNo,uint32_t SubframeNo,uint32_t ExpectedNextFrameNo,uint32_t ExpectedNextSubframeNo);
  
private:
  virtual void DoRun (void);
  std::string m_ControlPeriod;
  uint64_t m_ControlBitmap;
  int m_ControlPrbNum;
  int m_ControlPrbStart;
  int m_ControlPrbEnd; 
  int m_ControlOffset;
  uint64_t m_DataBitmap;
  int m_DataPrbNum;
  int m_DataPrbStart;
  int m_DataPrbEnd;
  int m_DataOffset;
  uint32_t m_FrameNo;
  uint32_t m_SubframeNo;
  uint32_t m_ExpectedNextFrameNo;
  uint32_t m_ExpectedNextSubframeNo;
};

std::string LteSidelinkScenPoolNextFrameSubframeTestCase::BuildNameString (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset,uint32_t FrameNo,uint32_t SubframeNo)
{
  std::ostringstream oss;
  
  oss << "TestCase:LteSidelinkScenPoolNextFrameSubframeTestCase, ";
  oss << "ControlPeriod:" << ControlPeriod << ", " ;
  oss << "ControlBitmap:" << std::hex << ControlBitmap << std::dec << ", " ; 
  oss << "ControlPrbNum:" << ControlPrbNum << ", " ; 
  oss << "ControlPrbStart:" << ControlPrbStart << ", " ; 
  oss << "ControlPrbEnd:" << ControlPrbEnd << ", " ; 
  oss << "ControlOffset:" << ControlOffset << ", " ;
  oss << "DataBitmap:" << std::hex << DataBitmap << std::dec << ", " ; 
  oss << "DataPrbNum:" << DataPrbNum << ", " ; 
  oss << "DataPrbStart:" << DataPrbStart << ", " ;  
  oss << "DataPrbEnd:" << DataPrbEnd << ", " ; 
  oss << "DataOffset:" << DataOffset << ", " ; 
  oss << "FrameNo:" << FrameNo << ", " ; 
  oss << "SubframeNo:" << SubframeNo ; 

  return oss.str ();
}

LteSidelinkScenPoolNextFrameSubframeTestCase::LteSidelinkScenPoolNextFrameSubframeTestCase (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset,uint32_t FrameNo,uint32_t SubframeNo,uint32_t ExpectedNextFrameNo,uint32_t ExpectedNextSubframeNo)
  : TestCase (BuildNameString(ControlPeriod,ControlBitmap,ControlPrbNum,ControlPrbStart,ControlPrbEnd,ControlOffset,DataBitmap,DataPrbNum,DataPrbStart,DataPrbEnd,DataOffset,FrameNo,SubframeNo)),
    m_ControlPeriod (ControlPeriod),
    m_ControlBitmap (ControlBitmap),
    m_ControlPrbNum (ControlPrbNum),
    m_ControlPrbStart (ControlPrbStart),
    m_ControlPrbEnd (ControlPrbEnd),
    m_ControlOffset (ControlOffset),
    m_DataBitmap (DataBitmap),
    m_DataPrbNum (DataPrbNum),
    m_DataPrbStart (DataPrbStart),
    m_DataPrbEnd (DataPrbEnd),
    m_DataOffset (DataOffset),
    m_FrameNo (FrameNo),
    m_SubframeNo (SubframeNo),
    m_ExpectedNextFrameNo (ExpectedNextFrameNo),
    m_ExpectedNextSubframeNo (ExpectedNextSubframeNo)
{
}

void LteSidelinkScenPoolNextFrameSubframeTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ControlPeriod, m_ControlBitmap, m_ControlPrbNum, m_ControlPrbStart, m_ControlPrbEnd, m_ControlOffset, m_DataBitmap, m_DataPrbNum, m_DataPrbStart, m_DataPrbEnd, m_DataOffset, m_FrameNo, m_SubframeNo));
  
  LteSlResourcePoolFactory pfactory;
  pfactory.SetControlPeriod (m_ControlPeriod);
  pfactory.SetControlBitmap (m_ControlBitmap); 
  pfactory.SetControlPrbNum (m_ControlPrbNum);
  pfactory.SetControlPrbStart (m_ControlPrbStart);
  pfactory.SetControlPrbEnd (m_ControlPrbEnd);
  pfactory.SetControlOffset (m_ControlOffset);
  pfactory.SetDataBitmap (m_DataBitmap);
  pfactory.SetDataPrbNum (m_DataPrbNum);
  pfactory.SetDataPrbStart (m_DataPrbStart);
  pfactory.SetDataPrbEnd (m_DataPrbEnd);
  pfactory.SetDataOffset (m_DataOffset);
  
  LteRrcSap::SlCommResourcePool pool = pfactory.CreatePool ();
  Ptr<SidelinkTxCommResourcePool> txpool = CreateObject<SidelinkTxCommResourcePool>();
  txpool->SetPool(pool);
  
  SidelinkCommResourcePool::SubframeInfo actualNextScPeriod = txpool->GetNextScPeriod (m_FrameNo, m_SubframeNo);
  
  NS_LOG_INFO ("Actual NextScPeriod= " << actualNextScPeriod.frameNo << "/" << actualNextScPeriod.subframeNo);

  NS_TEST_EXPECT_MSG_EQ(((m_ExpectedNextFrameNo==actualNextScPeriod.frameNo)&&(m_ExpectedNextSubframeNo==actualNextScPeriod.subframeNo)), true, "Expected and Actual Next Frame & Subframe numbers are not equal");
}








class LteSidelinkScenPoolResourceOpportunityTestCase : public TestCase
{
public:
  static std::string BuildNameString (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset);
  LteSidelinkScenPoolResourceOpportunityTestCase (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset,uint32_t PscchResourceNo,uint32_t PscchTransmissionNo,uint32_t ExpectedFrameNo,uint32_t ExpectedSubframeNo, uint32_t ExpectedRbStart,uint32_t ExpectedNbCount);
  
private:
  virtual void DoRun (void);
  std::string m_ControlPeriod;
  uint64_t m_ControlBitmap;
  int m_ControlPrbNum;
  int m_ControlPrbStart;
  int m_ControlPrbEnd; 
  int m_ControlOffset;
  uint64_t m_DataBitmap;
  int m_DataPrbNum;
  int m_DataPrbStart;
  int m_DataPrbEnd;
  int m_DataOffset;
  uint32_t m_PscchResourceNo;
  uint32_t m_PscchTransmissionNo;
  uint32_t m_ExpectedFrameNo;
  uint32_t m_ExpectedSubframeNo;
  uint32_t m_ExpectedRbStart;
  uint32_t m_ExpectedNbCount;
};

std::string LteSidelinkScenPoolResourceOpportunityTestCase::BuildNameString (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset)
{
  std::ostringstream oss;
  
  oss << "TestCase:LteSidelinkScenPoolResourceOpportunityTestCase, ";
  oss << "ControlPeriod:" << ControlPeriod << ", " ;
  oss << "ControlBitmap:" << std::hex << ControlBitmap << std::dec << ", " ; 
  oss << "ControlPrbNum:" << ControlPrbNum << ", " ; 
  oss << "ControlPrbStart:" << ControlPrbStart << ", " ; 
  oss << "ControlPrbEnd:" << ControlPrbEnd << ", " ; 
  oss << "ControlOffset:" << ControlOffset << ", " ;
  oss << "DataBitmap:" << std::hex << DataBitmap << std::dec << ", " ; 
  oss << "DataPrbNum:" << DataPrbNum << ", " ; 
  oss << "DataPrbStart:" << DataPrbStart << ", " ;  
  oss << "DataPrbEnd:" << DataPrbEnd << ", " ; 
  oss << "DataOffset:" << DataOffset;

  return oss.str ();
}

LteSidelinkScenPoolResourceOpportunityTestCase::LteSidelinkScenPoolResourceOpportunityTestCase (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset,uint32_t PscchResourceNo,uint32_t PscchTransmissionNo,uint32_t ExpectedFrameNo,uint32_t ExpectedSubframeNo,uint32_t ExpectedRbStart,uint32_t ExpectedNbCount)
  : TestCase (BuildNameString(ControlPeriod,ControlBitmap,ControlPrbNum,ControlPrbStart,ControlPrbEnd,ControlOffset,DataBitmap,DataPrbNum,DataPrbStart,DataPrbEnd,DataOffset)),
    m_ControlPeriod (ControlPeriod),
    m_ControlBitmap (ControlBitmap),
    m_ControlPrbNum (ControlPrbNum),
    m_ControlPrbStart (ControlPrbStart),
    m_ControlPrbEnd (ControlPrbEnd),
    m_ControlOffset (ControlOffset),
    m_DataBitmap (DataBitmap),
    m_DataPrbNum (DataPrbNum),
    m_DataPrbStart (DataPrbStart),
    m_DataPrbEnd (DataPrbEnd),
    m_DataOffset (DataOffset),
    m_PscchResourceNo (PscchResourceNo),
    m_PscchTransmissionNo (PscchTransmissionNo),
    m_ExpectedFrameNo (ExpectedFrameNo),
    m_ExpectedSubframeNo (ExpectedSubframeNo),
    m_ExpectedRbStart (ExpectedRbStart),
    m_ExpectedNbCount (ExpectedNbCount)
{
}

void LteSidelinkScenPoolResourceOpportunityTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ControlPeriod, m_ControlBitmap, m_ControlPrbNum, m_ControlPrbStart, m_ControlPrbEnd, m_ControlOffset, m_DataBitmap, m_DataPrbNum, m_DataPrbStart, m_DataPrbEnd, m_DataOffset));
  
  LteSlResourcePoolFactory pfactory;
  pfactory.SetControlPeriod (m_ControlPeriod);
  pfactory.SetControlBitmap (m_ControlBitmap); 
  pfactory.SetControlPrbNum (m_ControlPrbNum);
  pfactory.SetControlPrbStart (m_ControlPrbStart);
  pfactory.SetControlPrbEnd (m_ControlPrbEnd);
  pfactory.SetControlOffset (m_ControlOffset);
  pfactory.SetDataBitmap (m_DataBitmap);
  pfactory.SetDataPrbNum (m_DataPrbNum);
  pfactory.SetDataPrbStart (m_DataPrbStart);
  pfactory.SetDataPrbEnd (m_DataPrbEnd);
  pfactory.SetDataOffset (m_DataOffset);
  
  LteRrcSap::SlCommResourcePool pool = pfactory.CreatePool ();
  Ptr<SidelinkTxCommResourcePool> txpool = CreateObject<SidelinkTxCommResourcePool>();
  txpool->SetPool(pool);
  
  NS_LOG_INFO ("Number of PSCCH resources= " << txpool->GetNPscch());
  
  
  std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> txInfo = txpool->GetPscchTransmissions (m_PscchResourceNo);
  
  uint32_t ctr = 0;
  uint32_t ActualFrameNo = 0;
  uint32_t ActualSubframeNo = 0;
  uint32_t ActualRbStart = 0;
  uint32_t ActualNbCount = 0;
  
  for (std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator it=txInfo.begin() ; it!=txInfo.end() ; it++)
  {
	  if (ctr == m_PscchTransmissionNo)
	  {
		  ActualFrameNo = it->subframe.frameNo;
		  ActualSubframeNo = it->subframe.subframeNo;
		  ActualRbStart = (uint32_t) it->rbStart;
		  ActualNbCount = (uint32_t) it->nbRb;
		  break;
	  }
	  ctr++;
  }
  
  NS_LOG_INFO ("ActualFrameNo= " << ActualFrameNo);
  NS_LOG_INFO ("ActualSubframeNo= " << ActualSubframeNo);
  NS_LOG_INFO ("ActualRbStart= " << ActualRbStart);
  NS_LOG_INFO ("ActualNbCount= " << ActualNbCount);
  
  NS_TEST_EXPECT_MSG_EQ(((m_ExpectedFrameNo==ActualFrameNo)&&(m_ExpectedSubframeNo==ActualSubframeNo)&&(m_ExpectedRbStart==ActualRbStart)&&(m_ExpectedNbCount==ActualNbCount)), true, "Expected and Actual Frame no, Subframe no, RbStart and NbCount are not equal");
}






class LteSidelinkScenPoolSubframeOpportunityTestCase : public TestCase
{
public:
  static std::string BuildNameString (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset,uint32_t FrameNo,uint32_t SubframeNo);
  LteSidelinkScenPoolSubframeOpportunityTestCase (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset,uint32_t FrameNo,uint32_t SubframeNo,int32_t ExpectedPscchSubframeOp1,int32_t ExpectedPscchSubframeOp2);
  
private:
  virtual void DoRun (void);
  std::string m_ControlPeriod;
  uint64_t m_ControlBitmap;
  int m_ControlPrbNum;
  int m_ControlPrbStart;
  int m_ControlPrbEnd; 
  int m_ControlOffset;
  uint64_t m_DataBitmap;
  int m_DataPrbNum;
  int m_DataPrbStart;
  int m_DataPrbEnd;
  int m_DataOffset;
  uint32_t m_FrameNo;
  uint32_t m_SubframeNo;
  int32_t m_ExpectedPscchSubframeOp1;
  int32_t m_ExpectedPscchSubframeOp2;
};

std::string LteSidelinkScenPoolSubframeOpportunityTestCase::BuildNameString (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset,uint32_t FrameNo,uint32_t SubframeNo)
{
  std::ostringstream oss;
  
  oss << "TestCase:LteSidelinkScenPoolSubframeOpportunityTestCase, ";
  oss << "ControlPeriod:" << ControlPeriod << ", " ;
  oss << "ControlBitmap:" << std::hex << ControlBitmap << std::dec << ", " ; 
  oss << "ControlPrbNum:" << ControlPrbNum << ", " ; 
  oss << "ControlPrbStart:" << ControlPrbStart << ", " ; 
  oss << "ControlPrbEnd:" << ControlPrbEnd << ", " ; 
  oss << "ControlOffset:" << ControlOffset << ", " ;
  oss << "DataBitmap:" << std::hex << DataBitmap << std::dec << ", " ; 
  oss << "DataPrbNum:" << DataPrbNum << ", " ; 
  oss << "DataPrbStart:" << DataPrbStart << ", " ;  
  oss << "DataPrbEnd:" << DataPrbEnd << ", " ; 
  oss << "DataOffset:" << DataOffset << ", " ; 
  oss << "FrameNo:" << FrameNo << ", " ; 
  oss << "SubframeNo:" << SubframeNo ; 

  return oss.str ();
}

LteSidelinkScenPoolSubframeOpportunityTestCase::LteSidelinkScenPoolSubframeOpportunityTestCase (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset,uint32_t FrameNo,uint32_t SubframeNo,int32_t ExpectedPscchSubframeOp1,int32_t ExpectedPscchSubframeOp2)
  : TestCase (BuildNameString(ControlPeriod,ControlBitmap,ControlPrbNum,ControlPrbStart,ControlPrbEnd,ControlOffset,DataBitmap,DataPrbNum,DataPrbStart,DataPrbEnd,DataOffset,FrameNo,SubframeNo)),
    m_ControlPeriod (ControlPeriod),
    m_ControlBitmap (ControlBitmap),
    m_ControlPrbNum (ControlPrbNum),
    m_ControlPrbStart (ControlPrbStart),
    m_ControlPrbEnd (ControlPrbEnd),
    m_ControlOffset (ControlOffset),
    m_DataBitmap (DataBitmap),
    m_DataPrbNum (DataPrbNum),
    m_DataPrbStart (DataPrbStart),
    m_DataPrbEnd (DataPrbEnd),
    m_DataOffset (DataOffset),
    m_FrameNo (FrameNo),
    m_SubframeNo (SubframeNo),
    m_ExpectedPscchSubframeOp1 (ExpectedPscchSubframeOp1),
    m_ExpectedPscchSubframeOp2 (ExpectedPscchSubframeOp2)
{
}

void LteSidelinkScenPoolSubframeOpportunityTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ControlPeriod, m_ControlBitmap, m_ControlPrbNum, m_ControlPrbStart, m_ControlPrbEnd, m_ControlOffset, m_DataBitmap, m_DataPrbNum, m_DataPrbStart, m_DataPrbEnd, m_DataOffset, m_FrameNo, m_SubframeNo));
  
  LteSlResourcePoolFactory pfactory;
  pfactory.SetControlPeriod (m_ControlPeriod);
  pfactory.SetControlBitmap (m_ControlBitmap); 
  pfactory.SetControlPrbNum (m_ControlPrbNum);
  pfactory.SetControlPrbStart (m_ControlPrbStart);
  pfactory.SetControlPrbEnd (m_ControlPrbEnd);
  pfactory.SetControlOffset (m_ControlOffset);
  pfactory.SetDataBitmap (m_DataBitmap);
  pfactory.SetDataPrbNum (m_DataPrbNum);
  pfactory.SetDataPrbStart (m_DataPrbStart);
  pfactory.SetDataPrbEnd (m_DataPrbEnd);
  pfactory.SetDataOffset (m_DataOffset);
  
  LteRrcSap::SlCommResourcePool pool = pfactory.CreatePool ();
  Ptr<SidelinkTxCommResourcePool> txpool = CreateObject<SidelinkTxCommResourcePool>();
  txpool->SetPool(pool);
 
  std::list<uint8_t> rbs; 
  rbs = txpool->GetPscchOpportunities (m_FrameNo, m_SubframeNo);
  
  bool flag = true;
  int ctr = 0;
  int32_t ActualPscchSubframeOp1;
  int32_t ActualPscchSubframeOp2;
  
  if (rbs.size() > 0)
  {
	  for (std::list<uint8_t>::iterator it = rbs.begin() ; it != rbs.end() ; it++) 
	  {
		  if (ctr == 0)
		  {
			ActualPscchSubframeOp1 = (uint32_t) (*it);  
			if (ActualPscchSubframeOp1 != m_ExpectedPscchSubframeOp1) 
			{
				flag = false;
			}			
		  }
		  if (ctr == 1)
		  {
			ActualPscchSubframeOp2 = (uint32_t) (*it);
			if (ActualPscchSubframeOp2 != m_ExpectedPscchSubframeOp2) 
			{
				flag = false;
			}			
		  } 
		  
		  ctr = ctr + 1;
	  }
	  NS_LOG_INFO ("ActualPscchSubframeOp1= " << ActualPscchSubframeOp1);
	  NS_LOG_INFO ("ActualPscchSubframeOp2= " << ActualPscchSubframeOp2);
  }
  else
  {
	  NS_LOG_INFO ("No opportunities");
	  ActualPscchSubframeOp1 = -1;
	  ActualPscchSubframeOp2 = -1;
	  if ((ActualPscchSubframeOp1 != m_ExpectedPscchSubframeOp1) || (ActualPscchSubframeOp2 != m_ExpectedPscchSubframeOp2))
	  {
		  flag = false;
	  }
  }
  
  NS_TEST_EXPECT_MSG_EQ(flag, true, "Expected and Actual PSCCH opportunities for subframe are not equal");
}


//ends here



class LteSidelinkScenPoolPsschTestCase : public TestCase
{
public:
  static std::string BuildNameString (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset);
  LteSidelinkScenPoolPsschTestCase (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset,uint32_t FrameNo,uint32_t SubframeNo,uint32_t Itrp,uint32_t RbStart,uint32_t RbLen,uint32_t PsschTransmissionNo,uint32_t ExpectedFrameNo,uint32_t ExpectedSubframeNo,uint32_t ExpectedRbStart,uint32_t ExpectedNbCount);
  
private:
  virtual void DoRun (void);
  std::string m_ControlPeriod;
  uint64_t m_ControlBitmap;
  int m_ControlPrbNum;
  int m_ControlPrbStart;
  int m_ControlPrbEnd; 
  int m_ControlOffset;
  uint64_t m_DataBitmap;
  int m_DataPrbNum;
  int m_DataPrbStart;
  int m_DataPrbEnd;
  int m_DataOffset;
  uint32_t m_FrameNo;
  uint32_t m_SubframeNo;
  uint32_t m_Itrp;
  uint32_t m_RbStart;
  uint32_t m_RbLen;
  uint32_t m_PsschTransmissionNo;
  uint32_t m_ExpectedFrameNo;
  uint32_t m_ExpectedSubframeNo;
  uint32_t m_ExpectedRbStart;
  uint32_t m_ExpectedNbCount;
};

std::string LteSidelinkScenPoolPsschTestCase::BuildNameString (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset)
{
  std::ostringstream oss;
  
  oss << "TestCase:LteSidelinkScenPoolPsschTestCase, ";
  oss << "ControlPeriod:" << ControlPeriod << ", " ;
  oss << "ControlBitmap:" << std::hex << ControlBitmap << std::dec << ", " ; 
  oss << "ControlPrbNum:" << ControlPrbNum << ", " ; 
  oss << "ControlPrbStart:" << ControlPrbStart << ", " ; 
  oss << "ControlPrbEnd:" << ControlPrbEnd << ", " ; 
  oss << "ControlOffset:" << ControlOffset << ", " ;
  oss << "DataBitmap:" << std::hex << DataBitmap << std::dec << ", " ; 
  oss << "DataPrbNum:" << DataPrbNum << ", " ; 
  oss << "DataPrbStart:" << DataPrbStart << ", " ;  
  oss << "DataPrbEnd:" << DataPrbEnd << ", " ; 
  oss << "DataOffset:" << DataOffset;

  return oss.str ();
}

LteSidelinkScenPoolPsschTestCase::LteSidelinkScenPoolPsschTestCase (std::string ControlPeriod,uint64_t ControlBitmap,int ControlPrbNum,int ControlPrbStart,int ControlPrbEnd,int ControlOffset,uint64_t DataBitmap,int DataPrbNum,int DataPrbStart,int DataPrbEnd,int DataOffset,uint32_t FrameNo,uint32_t SubframeNo,uint32_t Itrp,uint32_t RbStart,uint32_t RbLen,uint32_t PsschTransmissionNo,uint32_t ExpectedFrameNo,uint32_t ExpectedSubframeNo,uint32_t ExpectedRbStart,uint32_t ExpectedNbCount)
  : TestCase (BuildNameString(ControlPeriod,ControlBitmap,ControlPrbNum,ControlPrbStart,ControlPrbEnd,ControlOffset,DataBitmap,DataPrbNum,DataPrbStart,DataPrbEnd,DataOffset)),
    m_ControlPeriod (ControlPeriod),
    m_ControlBitmap (ControlBitmap),
    m_ControlPrbNum (ControlPrbNum),
    m_ControlPrbStart (ControlPrbStart),
    m_ControlPrbEnd (ControlPrbEnd),
    m_ControlOffset (ControlOffset),
    m_DataBitmap (DataBitmap),
    m_DataPrbNum (DataPrbNum),
    m_DataPrbStart (DataPrbStart),
    m_DataPrbEnd (DataPrbEnd),
    m_DataOffset (DataOffset),
    m_FrameNo (FrameNo),
	m_SubframeNo (SubframeNo),
	m_Itrp (Itrp),
	m_RbStart (RbStart),
	m_RbLen (RbLen),
	m_PsschTransmissionNo (PsschTransmissionNo),
	m_ExpectedFrameNo (ExpectedFrameNo),
	m_ExpectedSubframeNo (ExpectedSubframeNo),
	m_ExpectedRbStart (ExpectedRbStart),
	m_ExpectedNbCount (ExpectedNbCount)
{
}

void LteSidelinkScenPoolPsschTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this << BuildNameString (m_ControlPeriod, m_ControlBitmap, m_ControlPrbNum, m_ControlPrbStart, m_ControlPrbEnd, m_ControlOffset, m_DataBitmap, m_DataPrbNum, m_DataPrbStart, m_DataPrbEnd, m_DataOffset));
  
  LteSlResourcePoolFactory pfactory;
  pfactory.SetControlPeriod (m_ControlPeriod);
  pfactory.SetControlBitmap (m_ControlBitmap); 
  pfactory.SetControlPrbNum (m_ControlPrbNum);
  pfactory.SetControlPrbStart (m_ControlPrbStart);
  pfactory.SetControlPrbEnd (m_ControlPrbEnd);
  pfactory.SetControlOffset (m_ControlOffset);
  pfactory.SetDataBitmap (m_DataBitmap);
  pfactory.SetDataPrbNum (m_DataPrbNum);
  pfactory.SetDataPrbStart (m_DataPrbStart);
  pfactory.SetDataPrbEnd (m_DataPrbEnd);
  pfactory.SetDataOffset (m_DataOffset);
  
  LteRrcSap::SlCommResourcePool pool = pfactory.CreatePool ();
  Ptr<SidelinkTxCommResourcePool> txpool = CreateObject<SidelinkTxCommResourcePool>();
  txpool->SetPool(pool);
  
  SidelinkCommResourcePool::SubframeInfo nextScPeriod = txpool->GetNextScPeriod (m_FrameNo, m_SubframeNo);
  std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> txInfo = txpool->GetPsschTransmissions (nextScPeriod, m_Itrp, m_RbStart, m_RbLen);
    
  NS_LOG_INFO ("PSSCH transmissions= " << txInfo.size());
  
  uint32_t ctr = 0;
  uint32_t ActualFrameNo = 0;
  uint32_t ActualSubframeNo = 0;
  uint32_t ActualRbStart = 0;
  uint32_t ActualNbCount = 0;
  
  for (std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator it=txInfo.begin() ; it!=txInfo.end() ; it++)
  {
	  if (ctr == m_PsschTransmissionNo)
	  {
		  ActualFrameNo = it->subframe.frameNo;
		  ActualSubframeNo = it->subframe.subframeNo;
		  ActualRbStart = (uint32_t) it->rbStart;
		  ActualNbCount = (uint32_t) it->nbRb;
		  break;
	  }
	  ctr++;
  }
  
  NS_LOG_INFO ("ActualFrameNo= " << ActualFrameNo);
  NS_LOG_INFO ("ActualSubframeNo= " << ActualSubframeNo);
  NS_LOG_INFO ("ActualRbStart= " << ActualRbStart);
  NS_LOG_INFO ("ActualNbCount= " << ActualNbCount);
  
  NS_TEST_EXPECT_MSG_EQ(((m_ExpectedFrameNo==ActualFrameNo)&&(m_ExpectedSubframeNo==ActualSubframeNo)&&(m_ExpectedRbStart==ActualRbStart)&&(m_ExpectedNbCount==ActualNbCount)), true, "Expected and Actual Frame no, Subframe no, RbStart and NbCount for PSSCH are not equal");
}








class LteSidelinkScenPoolTestSuite : public TestSuite
{
public:
  LteSidelinkScenPoolTestSuite ();
};


LteSidelinkScenPoolTestSuite::LteSidelinkScenPoolTestSuite ()
  : TestSuite ("lte-sidelink-scen-pool", UNIT)
{
  LogComponentEnable("LteSidelinkScenPool", LOG_LEVEL_ALL);
  
  //LteSidelinkScenPoolNextFrameSubframeTestCase Input Format: 
  //ControlPeriod,ControlBitmap,ControlPrbNum,ControlPrbStart,ControlPrbEnd,ControlOffset,DataBitmap,DataPrbNum,DataPrbStart,DataPrbEnd,DataOffset,FrameNo,SubframeNo,ExpectedNextFrameNo,ExpectedNextSubframeNo
  
  //The current subframe is less than the offset value. The next period will be the first one and will start at 80 ms.
  AddTestCase(new LteSidelinkScenPoolNextFrameSubframeTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,0,5,8,0),TestCase::QUICK);
  
  //Move to the end of the frameNo range to test rollover
  AddTestCase(new LteSidelinkScenPoolNextFrameSubframeTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,1023,0,4,0),TestCase::QUICK);
  
  //Other example: expected next period: 1004/0
  AddTestCase(new LteSidelinkScenPoolNextFrameSubframeTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,1002,0,1004,0),TestCase::QUICK);
  
  
  
  //LteSidelinkScenPoolResourceOpportunityTestCase Input Format: 
  //ControlPeriod,ControlBitmap,ControlPrbNum,ControlPrbStart,ControlPrbEnd,ControlOffset,DataBitmap,DataPrbNum,DataPrbStart,DataPrbEnd,DataOffset,PscchResourceNo,PscchTransmissionNo,ExpectedFrameNo,ExpectedSubframeNo,ExpectedRbStart,ExpectedNbCount
  
  //PscchResourceNo: Input 0 for 1st resource, 1 for 2nd resource
  //PscchTransmissionNo: Input 0 for 1st transmission, 1 for 2nd transmission
  
  //PscchResourceNo:0,PscchTransmissionNo:0
  AddTestCase(new LteSidelinkScenPoolResourceOpportunityTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,0,0,0,0,10,1),TestCase::QUICK);
  //PscchResourceNo:0,PscchTransmissionNo:1
  AddTestCase(new LteSidelinkScenPoolResourceOpportunityTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,0,1,0,1,40,1),TestCase::QUICK);
  //PscchResourceNo:1,PscchTransmissionNo:0
  AddTestCase(new LteSidelinkScenPoolResourceOpportunityTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,1,0,0,0,40,1),TestCase::QUICK);
  //PscchResourceNo:1,PscchTransmissionNo:1
  AddTestCase(new LteSidelinkScenPoolResourceOpportunityTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,1,1,0,1,10,1),TestCase::QUICK);
  
  
  
  //LteSidelinkScenPoolSubframeOpportunityTestCase Input Format:
  //ControlPeriod,ControlBitmap,ControlPrbNum,ControlPrbStart,ControlPrbEnd,ControlOffset,DataBitmap,DataPrbNum,DataPrbStart,DataPrbEnd,DataOffset,FrameNo,SubframeNo,ExpectedPscchSubframeOp1,ExpectedPscchSubframeOp2
  
  //For no opportunities, use ExpectedPscchSubframeOp1 = -1 and ExpectedPscchSubframeOp2 = -1
  
  //If we go to 80 ms (frame 8, subframe 0), we should be on the first subframe of the control channel
  AddTestCase(new LteSidelinkScenPoolSubframeOpportunityTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,8,0,10,40),TestCase::QUICK);
  //If we go to 81 ms (frame 8, subframe 1), we should be on the second subframe of the control channel
  AddTestCase(new LteSidelinkScenPoolSubframeOpportunityTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,8,1,10,40),TestCase::QUICK);
  //If we go to 82 ms (frame 8, subframe 2), we should be outside the control channel
  AddTestCase(new LteSidelinkScenPoolSubframeOpportunityTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,8,2,-1,-1),TestCase::QUICK);
  
  
  
  //LteSidelinkScenPoolPsschTestCase Input Format: 
  //ControlPeriod,ControlBitmap,ControlPrbNum,ControlPrbStart,ControlPrbEnd,ControlOffset,DataBitmap,DataPrbNum,DataPrbStart,DataPrbEnd,DataOffset,FrameNo,SubframeNo,Itrp,RbStart,RbLen,PsschTransmissionNo,ExpectedFrameNo,ExpectedSubframeNo,ExpectedRbStart,ExpectedNbCount

  //With the following configuration, only 1 bit is set in the TRP bitmap (the 6th D2D subframe)
  //Because the bitmap for data is 0011001100110011001100110011001100110011
  //                                          |
  //                                       6th D2D subframe
  //The first transmission should occur on: periodStart (80) + dataOffset (8) + TRP subframe (11) = 99
  //The second one (and followings) will occur every 16 ms
  
  
  //PsschTransmissionNo: Input 0 for 1st transmission, 1 for 2nd transmission, 2 for 3rd transmission, 3 for 4th transmission
    
  //PsschTransmissionNo:0
  AddTestCase(new LteSidelinkScenPoolPsschTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,0,5,5,2,6,0,9,9,2,6),TestCase::QUICK);
  //PsschTransmissionNo:1
  AddTestCase(new LteSidelinkScenPoolPsschTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,0,5,5,2,6,1,11,5,2,6),TestCase::QUICK);
  //PsschTransmissionNo:2
  AddTestCase(new LteSidelinkScenPoolPsschTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,0,5,5,2,6,2,13,1,2,6),TestCase::QUICK);
  //PsschTransmissionNo:3
  AddTestCase(new LteSidelinkScenPoolPsschTestCase("sf120",0xC000000000,1,10,40,80,0x3333333333,10,0,49,8,0,5,5,2,6,3,14,7,2,6),TestCase::QUICK);
  
  
};



static LteSidelinkScenPoolTestSuite staticLteSidelinkScenPoolTestSuite;

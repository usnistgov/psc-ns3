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

#include "lte-sl-pool.h"
#include <ns3/log.h>

namespace ns3 {
/**
 * Mapping betwen the index of the transmission repetition pattern and the number
 * of subframes available for D2D transmission (out of 8) as defined in TS 36.213 14.1.1.1.1
 */
static const uint8_t ItrpToKtrpMap[107] =
{
  1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 4, 4, 4, 8
};

/**
 * Mapping between the ITRP and the actual bitmap identifying which
 * subframes can be used for D2D transmissions as defined in TS 36.213 14.1.1.1.1
 */
static const std::bitset<8> ItrpToBitmap[107] =
{
  std::bitset<8> (std::string ("10000000")),
  std::bitset<8> (std::string ("01000000")),
  std::bitset<8> (std::string ("00100000")),
  std::bitset<8> (std::string ("00010000")),
  std::bitset<8> (std::string ("00001000")),
  std::bitset<8> (std::string ("00000100")),
  std::bitset<8> (std::string ("00000010")),
  std::bitset<8> (std::string ("00000001")),
  std::bitset<8> (std::string ("11000000")),    //k=2
  std::bitset<8> (std::string ("10100000")),
  std::bitset<8> (std::string ("01100000")),    //10
  std::bitset<8> (std::string ("10010000")),
  std::bitset<8> (std::string ("01010000")),
  std::bitset<8> (std::string ("00110000")),
  std::bitset<8> (std::string ("10001000")),
  std::bitset<8> (std::string ("01001000")),
  std::bitset<8> (std::string ("00101000")),
  std::bitset<8> (std::string ("00011000")),
  std::bitset<8> (std::string ("10000100")),
  std::bitset<8> (std::string ("01000100")),
  std::bitset<8> (std::string ("00100100")),    //20
  std::bitset<8> (std::string ("00010100")),
  std::bitset<8> (std::string ("00001100")),
  std::bitset<8> (std::string ("10000010")),
  std::bitset<8> (std::string ("01000010")),
  std::bitset<8> (std::string ("00100010")),
  std::bitset<8> (std::string ("00010010")),
  std::bitset<8> (std::string ("00001010")),
  std::bitset<8> (std::string ("00000110")),
  std::bitset<8> (std::string ("10000001")),
  std::bitset<8> (std::string ("01000001")),    //30
  std::bitset<8> (std::string ("00100001")),
  std::bitset<8> (std::string ("00010001")),
  std::bitset<8> (std::string ("00001001")),
  std::bitset<8> (std::string ("00000101")),
  std::bitset<8> (std::string ("00000011")),
  std::bitset<8> (std::string ("11110000")),
  std::bitset<8> (std::string ("11101000")),
  std::bitset<8> (std::string ("11011000")),
  std::bitset<8> (std::string ("10111000")),
  std::bitset<8> (std::string ("01111000")),    //40
  std::bitset<8> (std::string ("11100100")),
  std::bitset<8> (std::string ("11010100")),
  std::bitset<8> (std::string ("10110100")),
  std::bitset<8> (std::string ("01110100")),
  std::bitset<8> (std::string ("11001100")),
  std::bitset<8> (std::string ("10101100")),
  std::bitset<8> (std::string ("01101100")),
  std::bitset<8> (std::string ("10011100")),
  std::bitset<8> (std::string ("01011100")),
  std::bitset<8> (std::string ("00111100")),    //50
  std::bitset<8> (std::string ("11100010")),
  std::bitset<8> (std::string ("11010010")),
  std::bitset<8> (std::string ("10110010")),
  std::bitset<8> (std::string ("01110010")),
  std::bitset<8> (std::string ("11001010")),
  std::bitset<8> (std::string ("10101010")),
  std::bitset<8> (std::string ("01101010")),
  std::bitset<8> (std::string ("10011010")),
  std::bitset<8> (std::string ("01011010")),
  std::bitset<8> (std::string ("00111010")),    //60
  std::bitset<8> (std::string ("11000110")),
  std::bitset<8> (std::string ("11000110")),
  std::bitset<8> (std::string ("01100110")),
  std::bitset<8> (std::string ("10010110")),
  std::bitset<8> (std::string ("01010110")),
  std::bitset<8> (std::string ("00110110")),
  std::bitset<8> (std::string ("10001110")),
  std::bitset<8> (std::string ("01001110")),
  std::bitset<8> (std::string ("00101110")),
  std::bitset<8> (std::string ("00011110")),    //70
  std::bitset<8> (std::string ("11100001")),
  std::bitset<8> (std::string ("11010001")),
  std::bitset<8> (std::string ("10110001")),
  std::bitset<8> (std::string ("01110001")),
  std::bitset<8> (std::string ("11001001")),
  std::bitset<8> (std::string ("10101001")),
  std::bitset<8> (std::string ("01101001")),
  std::bitset<8> (std::string ("10011001")),
  std::bitset<8> (std::string ("01011001")),
  std::bitset<8> (std::string ("00111001")),    //80
  std::bitset<8> (std::string ("11000101")),
  std::bitset<8> (std::string ("10100101")),
  std::bitset<8> (std::string ("01100101")),
  std::bitset<8> (std::string ("10010101")),
  std::bitset<8> (std::string ("01010101")),
  std::bitset<8> (std::string ("00110101")),
  std::bitset<8> (std::string ("10001101")),
  std::bitset<8> (std::string ("01001101")),
  std::bitset<8> (std::string ("00101101")),
  std::bitset<8> (std::string ("00011101")),    //90
  std::bitset<8> (std::string ("11000011")),
  std::bitset<8> (std::string ("10100011")),
  std::bitset<8> (std::string ("01100011")),
  std::bitset<8> (std::string ("10010011")),
  std::bitset<8> (std::string ("01010011")),
  std::bitset<8> (std::string ("00110011")),
  std::bitset<8> (std::string ("10001011")),
  std::bitset<8> (std::string ("01001011")),
  std::bitset<8> (std::string ("00101011")),
  std::bitset<8> (std::string ("00011011")),    //100
  std::bitset<8> (std::string ("10000111")),
  std::bitset<8> (std::string ("01000111")),
  std::bitset<8> (std::string ("00100111")),
  std::bitset<8> (std::string ("00010111")),
  std::bitset<8> (std::string ("00001111")),
  std::bitset<8> (std::string ("11111111"))
};

NS_LOG_COMPONENT_DEFINE ("LteSlPool");
NS_OBJECT_ENSURE_REGISTERED (SidelinkCommResourcePool);

///// SidelinkCommResourcePool //////
SidelinkCommResourcePool::SidelinkCommResourcePool (void) : m_type (SidelinkCommResourcePool::UNKNOWN)
{
  NS_LOG_FUNCTION (this);
  m_preconfigured = false;
}

SidelinkCommResourcePool::~SidelinkCommResourcePool (void)
{
  NS_LOG_FUNCTION (this);
}


TypeId
SidelinkCommResourcePool::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SidelinkCommResourcePool")
    .SetParent<Object> ()
    .AddConstructor<SidelinkCommResourcePool> ()
    .AddTraceSource ("ReportNextScPeriod",
                     "Fired upon when the next Sidelink Control (SC) period is computed.",
                     MakeTraceSourceAccessor (&SidelinkCommResourcePool:: m_nextScPeriod),
                     "ns3::SidelinkCommResourcePool::ScPeriodTracedCallback")
  ;
  return tid;
}


bool SidelinkCommResourcePool::operator== (const SidelinkCommResourcePool& other)
{
  bool equal = m_type == other.m_type
    && m_scCpLen.cplen == other.m_scCpLen.cplen
    && m_scPeriod.period == other.m_scPeriod.period
    && m_scTfResourceConfig == other.m_scTfResourceConfig
    && m_dataCpLen.cplen == other.m_dataCpLen.cplen
    && m_dataHoppingConfig == other.m_dataHoppingConfig;
  if (equal && m_type == SidelinkCommResourcePool::UE_SELECTED)
    {
      equal = equal && m_dataTfResourceConfig == other.m_dataTfResourceConfig
        && m_trptSubset.subset == other.m_trptSubset.subset;
    }
  return equal;
}

void
SidelinkCommResourcePool::SetPool (LteRrcSap::SlCommResourcePool pool)
{
  NS_LOG_FUNCTION (this);
  //parse information
  m_scCpLen = pool.scCpLen;
  m_scPeriod = pool.scPeriod;
  m_scTfResourceConfig = pool.scTfResourceConfig;
  m_dataCpLen = pool.dataCpLen;
  m_dataHoppingConfig = pool.dataHoppingConfig;

  if (pool.haveUeSelectedResourceConfig)
    {
      //this is a UE selected pool
      m_type = SidelinkCommResourcePool::UE_SELECTED;
      m_dataTfResourceConfig = pool.ueSelectedResourceConfig.dataTfResourceConfig;
      if (pool.ueSelectedResourceConfig.haveTrptSubset)
        {
          m_trptSubset = pool.ueSelectedResourceConfig.trptSubset;
        }
      else
        {
          //assumes all T-RPT set available
          m_trptSubset.subset.set ();
        }

    }
  else
    {
      //this is a scheduled pool
      m_type = SidelinkCommResourcePool::SCHEDULED;
    }

  Initialize ();
}

void
SidelinkCommResourcePool::SetPool (LteRrcSap::SlPreconfigCommPool pool)
{
  NS_LOG_FUNCTION (this);
  //preconfigured pools are always UE selected
  m_type = SidelinkCommResourcePool::UE_SELECTED;
  m_preconfigured = false;
  //parse information
  m_scCpLen = pool.scCpLen;
  m_scPeriod = pool.scPeriod;
  m_scTfResourceConfig = pool.scTfResourceConfig;
  m_dataCpLen = pool.dataCpLen;
  m_dataHoppingConfig = pool.dataHoppingConfig;
  m_dataTfResourceConfig = pool.dataTfResourceConfig;
  m_trptSubset = pool.trptSubset;

  Initialize ();
}

void
SidelinkCommResourcePool::Initialize ()
{
  NS_LOG_FUNCTION (this);
  ComputeNumberOfPscchResources ();
  ComputeNumberOfPsschResources ();
}

SidelinkCommResourcePool::SlPoolType
SidelinkCommResourcePool::GetSchedulingType ()
{
  NS_LOG_FUNCTION (this);
  return m_type;
}


SidelinkCommResourcePool::SubframeInfo
SidelinkCommResourcePool::GetCurrentScPeriod (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this << frameNo << subframeNo);
  SubframeInfo currentScPeriod;
  int32_t subframe = 10 * (frameNo % 1024) + subframeNo % 10;
  int32_t period = LteRrcSap::PeriodAsInt (m_scPeriod);
  NS_LOG_DEBUG ("subframe = " << subframe << ", period (ms) = " << period);
  int32_t currentPeriod = std::floor ((subframe - m_scTfResourceConfig.offsetIndicator.offset) / (double) period);
  uint32_t currentStart = m_scTfResourceConfig.offsetIndicator.offset + currentPeriod * period;
  currentScPeriod.frameNo = (currentStart / 10) % 1024;
  currentScPeriod.subframeNo = currentStart % 10;

  NS_LOG_DEBUG ("Current frame = " << currentScPeriod.frameNo << "Current subframe = " << currentScPeriod.subframeNo <<
                ", period (ms) = " << period << ", currentPeriod = " << currentPeriod);

  return currentScPeriod;
}

SidelinkCommResourcePool::SubframeInfo
SidelinkCommResourcePool::GetNextScPeriod (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this << frameNo << subframeNo);
  SubframeInfo nextScPeriod;
  int32_t subframe = 10 * (frameNo % 1024) + subframeNo % 10;
  int32_t period = LteRrcSap::PeriodAsInt (m_scPeriod);
  int32_t currentPeriod = std::floor ((subframe - m_scTfResourceConfig.offsetIndicator.offset) / (double) period);

  //if the frame is the last period, we need to start over
  if (currentPeriod >= ((10240 - m_scTfResourceConfig.offsetIndicator.offset) / period) - 1)
    {
      currentPeriod = -1;
    }

  uint32_t nextStart = (m_scTfResourceConfig.offsetIndicator.offset + (currentPeriod + 1) * period);
  nextScPeriod.frameNo = (nextStart / 10) % 1024;
  nextScPeriod.subframeNo = nextStart % 10;

  NS_LOG_DEBUG ("subframe = " << subframe << ", period (ms) = " << period << ", currentPeriod = " << currentPeriod << ", nextStart = " << nextStart);
  NS_LOG_DEBUG ("Next Frame No = " << nextScPeriod.frameNo << ", Next subframe No = " << nextScPeriod.subframeNo);
  m_nextScPeriod (nextScPeriod.frameNo, nextScPeriod.subframeNo, nextStart);  //ReportNextScPeriod trace
  return nextScPeriod;
}

std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>
SidelinkCommResourcePool::GetPscchTransmissions (uint32_t n)
{
  NS_LOG_FUNCTION (this << n);
  NS_ASSERT_MSG (n < m_nPscchResources, "Requesting resource " << n << " but max is " << m_nPscchResources);

  std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> trans;
  //36.213 rel 12.5 - 14.2.1.1
  SidelinkCommResourcePool::SidelinkTransmissionInfo first;
  uint32_t subframe = n % m_lpscch;
  first.subframe.frameNo = subframe / 10;
  first.subframe.subframeNo = subframe % 10;
  first.rbStart = std::floor (n / m_lpscch);
  first.nbRb = 1;

  SidelinkCommResourcePool::SidelinkTransmissionInfo second;
  subframe = (n + 1 + (uint32_t) (std::floor (n / m_lpscch)) % (m_lpscch - 1)) % m_lpscch;
  second.subframe.frameNo = subframe / 10;
  second.subframe.subframeNo = subframe % 10;
  second.rbStart = std::floor (n / m_lpscch) + std::floor (m_rbpscch / 2);
  second.nbRb = 1;

  if (first.subframe < second.subframe)
    {
      trans.push_back (TranslatePscch (first));
      trans.push_back (TranslatePscch (second));
    }
  else
    {
      trans.push_back (TranslatePscch (second));
      trans.push_back (TranslatePscch (first));
    }

  return trans;
}

std::list<uint8_t>
SidelinkCommResourcePool::GetPscchOpportunities (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this << frameNo << subframeNo);
  std::list<uint8_t> opportunities;
  //first check if the subframe is part of the SC period
  int32_t subframe = 10 * (frameNo % 1024) + subframeNo % 10;
  int32_t period = LteRrcSap::PeriodAsInt (m_scPeriod);
  int32_t currentPeriod = std::floor ((subframe - m_scTfResourceConfig.offsetIndicator.offset) / (double) period);
  NS_LOG_DEBUG ("Subframe " << subframe << ", offset" << m_scTfResourceConfig.offsetIndicator.offset << ", current period" << currentPeriod);
  uint32_t currentStart = m_scTfResourceConfig.offsetIndicator.offset + currentPeriod * period;

  //check if we are still within the frames that could contain PSCCH opportunities
  if (subframe - currentStart < m_scTfResourceConfig.subframeBitmap.bitmap.size ())
    {
      //check if the bitmap is set for this subframe
      if (m_scTfResourceConfig.subframeBitmap.bitmap.test (subframe - currentStart))
        {
          //subframe is set, report the RBs that could be used
          for (std::vector<uint32_t>::iterator it = m_rbpscchVector.begin (); it != m_rbpscchVector.end (); it++)
            {
              opportunities.push_back ((*it));
            }
        }
    }


  return opportunities;
}

std::vector<int>
SidelinkCommResourcePool::GetPscchRbs (uint32_t frameNo, uint32_t subframeNo, uint32_t n)
{
  NS_LOG_FUNCTION (this << frameNo << subframeNo << n);
  std::vector<int> rbMask;

  std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> pscchN = GetPscchTransmissions (n);

  SubframeInfo current;
  current.frameNo = frameNo;
  current.subframeNo = subframeNo;

  SubframeInfo currentStart = GetCurrentScPeriod (frameNo, subframeNo);

  std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>::iterator txIt;
  for (txIt = pscchN.begin (); txIt != pscchN.end (); txIt++)
    {
      if (currentStart + txIt->subframe == current)
        {
          //provide list of RBs to use
          for (int i = txIt->rbStart; i < txIt->rbStart + txIt->nbRb; i++)
            {
              rbMask.push_back (i);
            }
        }
    }

  return rbMask;
}

uint32_t
SidelinkCommResourcePool::GetNPscch ()
{
  NS_LOG_FUNCTION (this);
  return m_nPscchResources;
}

std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo>
SidelinkCommResourcePool::GetPsschTransmissions (SidelinkCommResourcePool::SubframeInfo periodStart, uint8_t itrp, uint8_t rbStart, uint8_t rbLen)
{
  NS_LOG_FUNCTION (this << Simulator::Now ().GetMilliSeconds () << "Frame no. " << periodStart.frameNo << "subframe no. " << periodStart.subframeNo << "itrp  " << (uint16_t) itrp << " rbStart " << (uint16_t) rbStart << " rbLen " << (uint16_t) rbLen);

  if (m_type == SidelinkCommResourcePool::UE_SELECTED)
    {
      //check that the itrp is allowed. If preconfigured, all values are allowed
      uint8_t ktrp = ItrpToKtrpMap[itrp];
      NS_ASSERT ((ktrp == 1 && m_trptSubset.subset[0]) || (ktrp == 2 && m_trptSubset.subset[1]) || (ktrp == 4 && m_trptSubset.subset[2]) || ktrp == 8 /*m_preconfigured*/);

      //All RBs must be within the allowable RBs defined in the pool
      for (uint8_t i = rbStart; i < rbStart + rbLen; i++)
        {
          NS_ASSERT_MSG (i < m_dataTfResourceConfig.prbStart + m_dataTfResourceConfig.prbNum
                             || i > m_dataTfResourceConfig.prbEnd - m_dataTfResourceConfig.prbNum,
                         "Rb = " << (uint16_t) i << " not within range: prbStart = " << (uint16_t) m_dataTfResourceConfig.prbStart << ", prbStop = "
                                 << (uint16_t) m_dataTfResourceConfig.prbEnd << ", prbNum = " << (uint16_t) m_dataTfResourceConfig.prbNum);
        }
    }

  int32_t periodSubframe = 10 * (periodStart.frameNo % 1024) + periodStart.subframeNo % 10;

  //N_TRP and the bitmap b' as defined in TS 36.213 14.1.1.1.1
  uint32_t ntrp = 8;
  std::bitset<8> bitmap = ItrpToBitmap[itrp];

  std::vector<uint32_t> psschsubframes;
  for (uint32_t i = 0; i < m_lpssch; i++)
    {
      //compute b_i = b'_(i mod N_TRP)
      if (bitmap[7 - (i % ntrp)] && periodSubframe + m_lpsschVector[i] < 10240) //reverse order because b0 is the msb in the bitmap
        {
          psschsubframes.push_back (periodSubframe + m_lpsschVector[i]);
        }
    }

  //make sure the number of subframes is a multiple of 4
  int tmp = psschsubframes.size () % 4;
  for (int i = 0; i < tmp; i++)
    {
      psschsubframes.pop_back ();
    }

  uint8_t  nSl0 = 0;        //Type 1 frequency hopping, n slot 0 prb calculated for odd transmissions
  uint8_t  nSl1 = 0;        //Type 1 frequency hopping, n slot 1 prb calculated for even transmissions
  std::vector <std::vector <uint32_t> > nPrbVector;   //Type 2 frequency hopping PRBs starting indexes per transmission

  if (m_dataHoppingConfig.hoppingInfo <= 3)     //If frequency hopping enabled
    {
      uint8_t rbOffset = m_dataHoppingConfig.rbOffset; //resource block offset used for frequency hopping

      if (rbOffset % 2 != 0)    //check if rbOffset is odd, if so make it even
        {
          rbOffset++;
        }

      if (m_dataHoppingConfig.hoppingInfo <= 2) //Type 1 frequency hopping enabled
        {
          uint32_t nPrb = GetNprbType1 (rbStart);
          nSl0 = rbStart + (rbOffset / 2); //n slot 0 prb calculated for odd transmissions
          nSl1 = nPrb + (rbOffset / 2);  //n slot 1 prb calculated for even transmissions
        }
      else //Type 2 frequency hopping
        {
          nPrbVector = GetNprbType2 (rbStart, rbLen, psschsubframes);
          NS_ASSERT_MSG ( nPrbVector.size () == psschsubframes.size (), "SOMETHING WENT WRONG WHILE COMPUTING PRB INDEXES FOR FREQUENCY HOPPING TYPE 2, nPrbVector SIZE MISMATCH!");
        }
    }

  std::list<SidelinkCommResourcePool::SidelinkTransmissionInfo> txInfo;
  uint32_t tx_counter = 1;   //Transmission counter, used to keep track of parity when frequency hopping.
  for (std::vector<uint32_t>::iterator it = psschsubframes.begin (); it != psschsubframes.end (); it++)
    {
      SidelinkCommResourcePool::SidelinkTransmissionInfo info;
      info.subframe.frameNo = (*it) / 10;
      info.subframe.subframeNo = (*it) % 10;
      info.rbStart = rbStart; //default value used without frequency hopping.
      info.nbRb = rbLen;

      //PERFORM FREQUENCY HOPPING IF ENABLED
      //if hoppingInfo < 4 then frequency hopping is enabled
      //NOTE: have not accounted for System_BW options between 6-49,
      //      since we are using System_BW of 50.
      if (m_dataHoppingConfig.hoppingInfo <= 2) //type 1 frequency hopping enabled
        {
          //check if even or odd transmission number
          if (tx_counter % 2 == 0)
            {
              info.rbStart = nSl1;  //n slot 1 prb calculated for even subframes
            }
          else
            {
              info.rbStart = nSl0;      //n slot 0 prb calculated for odd subframes
            }
        }
      else if (m_dataHoppingConfig.hoppingInfo == 3)    //type 2 frequency hopping enabled
        {
          NS_LOG_INFO ("SF: " << *it << ", info.rbStart = nPRB = nPrbVector[" << tx_counter - 1 << "][0]: " << nPrbVector[tx_counter - 1][0] );
          info.rbStart = (uint8_t) nPrbVector[tx_counter - 1][0];  //set the slot or subframe to the according nPRB value
        }
      txInfo.push_back (info);
      tx_counter++;
    }
  return txInfo;
}

std::vector<uint32_t>
SidelinkCommResourcePool::GetAllPsschSubframes ()
{
  NS_LOG_FUNCTION (this);
  return m_lpsschVector;
}

LteRrcSap::SlHoppingConfigComm
SidelinkCommResourcePool::GetDataHoppingConfig ()
{
  NS_LOG_FUNCTION (this);
  return m_dataHoppingConfig;
}

uint32_t
SidelinkCommResourcePool::GetPsschBandwidth ()
{
  NS_LOG_FUNCTION (this);
  return m_rbpssch;
}

std::vector<uint32_t>
SidelinkCommResourcePool::GetTbPerSlPeriod ()
{
  NS_LOG_FUNCTION (this);
  std::vector<uint32_t> tbPerKtrpVector (4, 0);
  
  for (uint8_t i = 0; i < m_trptSubset.subset.size(); i++)
  {
      if(m_trptSubset.subset[i])
      {
          tbPerKtrpVector[i] = (uint32_t) std::pow(2,double(i)) * m_lpssch / 32;// Ntrp=8;HARQ=4;Ntrp*HARQ = 32
      }
  }
  if(m_trptSubset.subset.size() == 3 && m_trptSubset.subset.to_ulong() == 0)
  {
      tbPerKtrpVector[3] = (uint32_t) std::pow(2,double(3)) * m_lpssch / 32;//  Ntrp=8;HARQ=4;Ntrp*HARQ = 32
  }
  return tbPerKtrpVector;
}

std::vector<uint8_t>
SidelinkCommResourcePool::GetValidRBstart (uint8_t rbLen)
{
  NS_LOG_FUNCTION (this << "rbLen " << (uint16_t) rbLen);
  std::vector<uint8_t> rbStartIndexes;      //stores valid rbStart indexes
  if (m_dataHoppingConfig.hoppingInfo <= 3)   //Frequency Hopping Enabled
    {
      uint8_t rbOffset = m_dataHoppingConfig.rbOffset;  //resource block offset

      if (rbOffset % 2 != 0)  //check if rbOffset is odd, if so make it even
        {
          rbOffset++;
        }

      //used to check that the rbLen is valid (if given rbLen from GetValidAllocations() it will always be a valid value
      uint8_t nSl_hop  = 0;  //number of bits reserved for hopping information
      uint8_t y        = 0;  //size of the resource allocation field (in size of bits)
      uint8_t maxLcrbs = 0;  //max number of contiguous RBs for frequency hopping

      if ((m_rbpssch >= 6) && (m_rbpssch <= 49))
        {
          nSl_hop = 1;  //1 bit required to hold frequency hopping info
        }
      else if ((m_rbpssch > 49) && (m_rbpssch <= 110))
        {
          nSl_hop = 2; //2 bits required to hold frequency hopping info
        }

      y = uint8_t (std::ceil ((std::log (m_rbpssch * (m_rbpssch + 1) / 2)) / std::log (2)) - nSl_hop);


      if (m_dataHoppingConfig.hoppingInfo <= 2) //Type 1 Frequency Hopping Enabled
        {

          maxLcrbs = uint8_t (std::floor (std::pow (2,double(y)) / m_rbpssch));
          NS_LOG_LOGIC ("Type 1, hoppingInfo: " << (uint16_t) m_dataHoppingConfig.hoppingInfo << ", m_rbpssch: " << m_rbpssch << ", maxLcrbs: " << (uint16_t) maxLcrbs);

          //check if the rbLen is invalid
          if (rbLen > maxLcrbs)
            {
              NS_FATAL_ERROR (this << "INVALID rbLen " << uint16_t (rbLen) << " must be <= " << uint16_t (maxLcrbs));
            }

          uint8_t nPrb = 0; //stores the computed nPrb value for
          uint8_t nSL0 = 0; //stores the rbStart for odd subframes
          uint8_t nSL1 = 0; //stores the rbStart for even subframes
          NS_LOG_LOGIC ("rbStartIndexes:");

          for (uint8_t i = m_dataTfResourceConfig.prbStart; i <= m_dataTfResourceConfig.prbEnd; ++i)
            {
              //check if the rbStart and contiguous resource blocks are within the valid resource pool
              if (IsInPool (i, 1))
                {
                  nSL0 = i + (rbOffset / 2);
                  nPrb = GetNprbType1 (i);
                  nSL1 = nPrb + (rbOffset / 2);

                  //check if for both equations nSL0 and nSL1 the rbStart index is valid
                  if (IsInPool (nSL0,rbLen) && IsInPool (nSL1,rbLen))
                    {
                      //if the rbStart index is valid then add it the "list"
                      rbStartIndexes.push_back (i);
                      NS_LOG_LOGIC ("rbStartIndexes = " << (uint16_t) i);
                    }
                }
            }
        } //End of Type 1 Frequency Hopping Enabled
      else if (m_dataHoppingConfig.hoppingInfo == 3) //Type 2 Frequency Hopping Enabled
        {
          uint8_t nRbpssch = 0; //to store the number of Pssch RBs available for hopping

          //compute the number of Pssch RBs available
          if (m_dataHoppingConfig.numSubbands == 1)
            {
              nRbpssch = m_rbpssch;
            }
          else if (m_dataHoppingConfig.numSubbands > 1)
            {
              nRbpssch = m_rbpssch - rbOffset;
            }
          else
            {
              NS_FATAL_ERROR (this << " INVALID NUMBER OF SUBBANDS FOR FREQUENCY HOPPING TYPE 2.");
            }

          uint8_t sbSize = std::floor (nRbpssch / m_dataHoppingConfig.numSubbands);
          maxLcrbs = uint8_t (std::min (std::floor (std::pow (2, double(y)) / m_rbpssch), std::floor (nRbpssch / m_dataHoppingConfig.numSubbands)));
          NS_LOG_LOGIC ("Type 2, hoppingInfo: " << (uint16_t) m_dataHoppingConfig.hoppingInfo << ", nRbpssch: " << (uint16_t) nRbpssch << ", numSubbands: " << m_dataHoppingConfig.numSubbands << ", sbSize: " << (uint16_t) sbSize << ", maxLcrbs: " << (uint16_t) maxLcrbs);

          //check if the rbLen is invalid
          if (rbLen > maxLcrbs)
            {
              NS_FATAL_ERROR (this << "INVALID rbLen " << uint16_t (rbLen) << " must be <= " << uint16_t (maxLcrbs));
            }
          uint8_t totalAllocationsPerSb = sbSize - rbLen + 1; //Number of possible allocations in a sub-band
          uint8_t jAllocation = 0; //Sub-band allocation counter
          uint8_t iVrb = 0; //Virtual index of available RB considering the effectiveOffset
          uint8_t effectiveOffset = m_rbpssch - nRbpssch; //avoid comparing numSubbands to figure out if an offset was used
          NS_LOG_LOGIC ("rbStartIndexes:");
          for (uint8_t i = m_dataTfResourceConfig.prbStart; i <= m_dataTfResourceConfig.prbEnd; ++i)
            { //check if the resource block is within the valid resource pool
              if (IsInPool (i, 1, effectiveOffset))
                { //New sub-band
                  if (!(iVrb % sbSize))
                    {
                      jAllocation = 0;
                    }
                  //Avoid allocations to go over a sub-band's boundary.
                  if (jAllocation < totalAllocationsPerSb)
                    {
                      //check if the contiguous resource blocks are within the valid resource pool
                      if (IsInPool (i, rbLen, effectiveOffset))
                        {
                          //if the i index is valid, then add it rbStartIndexes
                          rbStartIndexes.push_back (i);
                          NS_LOG_LOGIC ((uint16_t) i);
                          jAllocation++;
                        }
                    }
                  iVrb++;
                }
            }
        } // end of Type 2 Frequency Hopping Enabled
    } //end of Frequency Hopping Enabled
  else
    {
      for (uint8_t i = m_dataTfResourceConfig.prbStart; i <= m_dataTfResourceConfig.prbEnd; ++i)
        {
          //check if the rbStart and contiguous resource blocks are within the valid resource pool
          if (IsInPool (i, rbLen))
            {
              rbStartIndexes.push_back (i);
            }
        }
    }

  return rbStartIndexes;
}

std::vector< std::vector<uint8_t> >
SidelinkCommResourcePool::GetValidAllocations ()
{
  NS_LOG_FUNCTION (this);
  std::vector< std::vector<uint8_t> > allValidRBstartIndexes;
  NS_LOG_DEBUG ("HoppingInfo = " << uint16_t (m_dataHoppingConfig.hoppingInfo));
  if (m_dataHoppingConfig.hoppingInfo <= 3)   //Frequency Hopping Enabled
    {
      //used to check that the number of contiguous resource blocks available are valid length
      uint8_t nSl_hop  = 0;  //number of bits reserved for hopping information
      uint8_t y        = 0;  //size of the resource allocation field (in size of bits)
      uint8_t maxLcrbs = 0;  //max number of contiguous RBs for frequency hopping

      if ((m_rbpssch >= 6) && (m_rbpssch <= 49))
        {
          nSl_hop = 1;  //1 bit required to hold frequency hopping info
        }
      else if ((m_rbpssch > 49) && (m_rbpssch <= 110))
        {
          nSl_hop = 2; //2 bits required to hold frequency hopping info
        }
      y = uint8_t (std::ceil ((std::log (m_rbpssch * (m_rbpssch + 1) / 2)) / std::log (2)) - nSl_hop);

      if (m_dataHoppingConfig.hoppingInfo <= 2)  //Type 1 Frequency Hopping Enabled
        {
          maxLcrbs = uint8_t (std::floor (std::pow (2,double(y)) / m_rbpssch));
          NS_LOG_DEBUG ("maxLcrbs = " << uint16_t (maxLcrbs));
          //for all validRBstarts add them to the "list"
          for (uint8_t mrb = 1; mrb <= maxLcrbs; ++mrb)
            {
              std::vector<uint8_t> validRBstart = GetValidRBstart (mrb);
              allValidRBstartIndexes.push_back (validRBstart);
            }
        }
      else if (m_dataHoppingConfig.hoppingInfo == 3) //Type 2 Frequency Hopping Enabled
        {
          NS_FATAL_ERROR (this << " GetValidAllocations() TYPE 2 FREQUENCY HOPPING IS NOT IMPLEMENTED.");

          uint8_t nRbpssch = 0; //to store the number of Pssch RBs available for hopping
          uint8_t rbOffset = m_dataHoppingConfig.rbOffset; //resource block offset

          if (rbOffset % 2 != 0) //check if rbOffset is odd, if so make it even
            {
              rbOffset++;
            }

          if (m_dataHoppingConfig.numSubbands == 1)
            {
              nRbpssch = m_rbpssch;
            }
          else if (m_dataHoppingConfig.numSubbands > 1)
            {
              nRbpssch = m_rbpssch - rbOffset;
            }
          else
            {
              NS_FATAL_ERROR (this << " INVALID NUMBER OF SUBBANDS FOR FREQUENCY HOPPING TYPE 2.");
            }

          maxLcrbs = uint8_t (std::min (std::floor (std::pow (2, double(y)) / m_rbpssch), std::floor (nRbpssch / m_dataHoppingConfig.numSubbands)));
        }
      else
        {
          //if frequency hopping is disabled we could generate random rbStart values here?
          NS_FATAL_ERROR (this << " GetValidAllocations() CANNOT BE CALLED WITH FREUQNECY HOPPING DISABLED.");
        }
    }
  return allValidRBstartIndexes;
}

SidelinkCommResourcePool::SidelinkTransmissionInfo
SidelinkCommResourcePool::TranslatePscch (SidelinkCommResourcePool::SidelinkTransmissionInfo info)
{
  NS_LOG_FUNCTION (this);
  SidelinkCommResourcePool::SidelinkTransmissionInfo translatedInfo;

  //For FDD the bitmap is 40 bits so assuming that all the subframe are set
  //then the maximum is 39
  uint32_t frameNo = 10 * info.subframe.frameNo + info.subframe.subframeNo;
  NS_ASSERT (frameNo < m_lpscchVector.size ());

  uint32_t mappedFrame = m_lpscchVector.at (frameNo);
  translatedInfo.subframe.frameNo = mappedFrame / 10;
  translatedInfo.subframe.subframeNo = mappedFrame % 10;
  NS_LOG_DEBUG ("Mapped frame/subframe number " << info.subframe.frameNo << "/" << info.subframe.subframeNo << " to frame/subframe number "
                                                << translatedInfo.subframe.frameNo << "/" << translatedInfo.subframe.subframeNo);

  //translate the RB
  NS_ASSERT (info.rbStart != m_rbpscchVector.size ());
  translatedInfo.rbStart = m_rbpscchVector.at (info.rbStart);
  translatedInfo.nbRb = info.nbRb;

  return translatedInfo;
}

bool
SidelinkCommResourcePool::IsInPool (uint8_t rbStart, uint8_t rbLen, uint8_t rbOffset)
{
  NS_LOG_FUNCTION (this);
  for (uint8_t i = rbStart; i < rbStart + rbLen; ++i)
    {
      //check if the resource are outside the pool, if so return false
      if (((i >= m_dataTfResourceConfig.prbStart + m_dataTfResourceConfig.prbNum
            && i <= m_dataTfResourceConfig.prbEnd - m_dataTfResourceConfig.prbNum)
           || i < m_dataTfResourceConfig.prbStart + (rbOffset / 2) || i > m_dataTfResourceConfig.prbEnd - (rbOffset / 2) ))
        {
          return false;
        }
    }
  return true;
}

uint32_t
SidelinkCommResourcePool::GetNprbType1 (uint8_t rbStart)
{
  NS_LOG_FUNCTION (this << (uint16_t) rbStart);
  uint32_t nPrb = 0;     //holds the computed nPrb value

  if (m_dataHoppingConfig.hoppingInfo <= 2)   //Type 1 Frequency Hopping Enabled
    {
      int32_t nRbpssch = 0;  //needed to store the number of Pssch RBs available for hopping
      uint8_t  rbOffset = m_dataHoppingConfig.rbOffset; //resource block offset used for frequency hopping

      if (rbOffset % 2 != 0)  //check if rbOffset is odd, if so make it even
        {
          rbOffset++;
        }

      nRbpssch = m_rbpssch - rbOffset - (m_rbpssch % 2);
      // 3GPP TS 36.213 8.4, 8.4.1
      switch (m_dataHoppingConfig.hoppingInfo)
        {
        case 0:
          nPrb = uint32_t (nRbpssch / 4 + rbStart) % nRbpssch;
          break;
        case 1:
          {
            //Modulo operation does not compute the desired result for negative dividends, hence we have to implement it.
            int32_t divd = -1 * nRbpssch / 4 + rbStart; //dividend
            nPrb = uint32_t (std::abs (divd - nRbpssch * std::floor (divd / double (nRbpssch))));
          }
          break;
        case 2:
          nPrb = uint32_t (nRbpssch / 2 + rbStart) % nRbpssch;
          break;
        default:
          NS_FATAL_ERROR (this << " INVALID HOPPINGINFO VALUE FOR TYPE 1 FREQUENCY HOPPING.");  //should never occur
        }
    }
  else
    {
      NS_FATAL_ERROR (this << " GetNprbType1, ONLY TYPE 1 FREQUENCY HOPPING IS ALLOWED.");
    }

  NS_LOG_DEBUG ("m_dataHoppingConfig.hoppingInfo : " << (uint16_t)m_dataHoppingConfig.hoppingInfo << " nPrb : " << nPrb);

  return nPrb;
}

std::vector <std::vector <uint32_t> >
SidelinkCommResourcePool::GetNprbType2 (uint8_t rbStart, uint8_t rbLen, std::vector <uint32_t> psschSFIndexes)
{
  NS_LOG_FUNCTION (this << "rbStart " << (uint16_t) rbStart << "rbLen " << (uint16_t) rbLen);
  // Indexes names including "Tilde" are without offset.
  uint32_t nTildeVrbStart = 0;      //calculate the virtual resource block number
  uint32_t sbSize = 0;      //calculate the size of the subbands
  uint32_t nPrb  = 0;      //temporary variable for calculating the the PRB index of each allocated PRB
  std::vector <std::vector <uint32_t> > nPrbVector (psschSFIndexes.size (), std::vector <uint32_t> (rbLen, 0));

  if (m_dataHoppingConfig.numSubbands == 1)
    {
      // offset is not considered when numSubbands = 1
      // NOTE: The VRB is computed from the pool resource index, not the absolute PRB.
      nTildeVrbStart = m_rbpsschPoolPrbToVrbIndexMap[rbStart];
      sbSize = m_rbpssch;
    }
  else if (m_dataHoppingConfig.numSubbands > 1)
    {
      nTildeVrbStart = m_rbpsschPoolPrbToVrbIndexMap[rbStart] - std::ceil (m_dataHoppingConfig.rbOffset / 2);
      sbSize = std::floor ((m_rbpssch - m_dataHoppingConfig.rbOffset - (m_dataHoppingConfig.rbOffset % 2))
                           / m_dataHoppingConfig.numSubbands);
    }
  NS_LOG_LOGIC ("numSubbands " << m_dataHoppingConfig.numSubbands << ", sbSize " << sbSize);
  //Compute all nPrb for every transmission opportunity in the SL period.
  uint32_t sf_iter = 0;
  uint32_t hop_distance = 0;
  uint8_t hopIndex = 0;
  uint32_t nTildeVrb = 0;
  uint8_t mirroring = 0;
  for (std::vector<uint32_t>::iterator it = psschSFIndexes.begin (); it != psschSFIndexes.end (); it++)
    {
      hopIndex = m_hopSequence[*it];   //FHopFunction (*it);
      hop_distance = hopIndex * sbSize;
      if (m_dataHoppingConfig.numSubbands == 1)
        {
          mirroring = FMirroringFunction (sf_iter + 1); //inter-subframe hopping
        }
      else if (m_dataHoppingConfig.numSubbands > 1)
        {
          mirroring = FMirroringFunction (*it);
        }
      NS_LOG_LOGIC ("SFN: " << *it << ", hopIndex " << (uint16_t) hopIndex << ", hop_distance: " << hop_distance);
      for (uint32_t iVrbGrant = 0; iVrbGrant < rbLen; iVrbGrant++)
        {
          nTildeVrb = nTildeVrbStart + iVrbGrant;

          nPrb =
            (uint32_t) ((nTildeVrb + hop_distance + ((sbSize - 1) - 2 * (uint32_t) (nTildeVrb % sbSize)) * mirroring) % (sbSize * m_dataHoppingConfig.numSubbands));

          if (m_dataHoppingConfig.numSubbands > 1)
            {
              nPrb += std::ceil (m_dataHoppingConfig.rbOffset / 2);
            }

          //Convert virtual pool indexes to absolute PRB indexes
          if (mirroring)
            {
              //Re-arrange PRB index, since there is no support yet for mirroring usage of RBs at the spectrum phy,
              //however, a mirrored grant can still benefit from the RB displacement.
              nPrbVector[sf_iter][rbLen - 1 - iVrbGrant] = m_rbpsschVector[nPrb];
              NS_LOG_LOGIC ("nTildeVrb " << nTildeVrb << ", nPrbVector[" << sf_iter << "][" << rbLen - 1 - iVrbGrant << "]: " << nPrbVector[sf_iter][rbLen - 1 - iVrbGrant] );

            }
          else
            {
              nPrbVector[sf_iter][iVrbGrant] = m_rbpsschVector[nPrb];
              NS_LOG_LOGIC ("nTildeVrb " << nTildeVrb << ", nPrbVector[" << sf_iter << "][" << iVrbGrant << "]: " << nPrbVector[sf_iter][iVrbGrant] );
            }
        }
      sf_iter++;
    }

  return nPrbVector;
}
uint8_t
SidelinkCommResourcePool::FHopFunction (uint32_t i)
{
  NS_LOG_FUNCTION (this << i);
  if (m_dataHoppingConfig.numSubbands == 1)
    {
      return 0;
    }

  uint8_t fhop_prev = 0;
  if (i > 0)
    {
      fhop_prev = FHopFunction (i - 1);
    }

  uint32_t sum = 0;
  for (uint32_t k = i * 10 + 1; k <= i * 10 + 9; k++)
    {
      NS_ASSERT (k < m_goldSequence.size ());
      sum += m_goldSequence[k] * (uint32_t) std::pow (2, k - (i * 10 + 1));
    }

  if (m_dataHoppingConfig.numSubbands == 2)
    {
      return uint8_t ((fhop_prev + sum) % m_dataHoppingConfig.numSubbands);
    }
  else if (m_dataHoppingConfig.numSubbands > 2)
    {
      sum = sum % (m_dataHoppingConfig.numSubbands - 1);
      return uint8_t ((fhop_prev + sum + 1) % m_dataHoppingConfig.numSubbands);
    }
  else
    {
      NS_FATAL_ERROR (this << " INVALID NUMBER OF SUBBANDS FOR FREQUENCY HOPPING. ONLY VALUES 1, 2, OR 4 ARE VALID.");
    }
}

void
SidelinkCommResourcePool::GenerateHopSequence ()
{
  NS_LOG_FUNCTION (this);
  //Ensure the vector is empty in case this function gets called again.
  if (m_hopSequence.size () != 0)
    {
      m_hopSequence.clear ();
    }

  uint8_t fhop_prev = 0;
  for (uint32_t i = 0; i < 10240; i++)
    {
      if (m_dataHoppingConfig.numSubbands == 1)
        {
          m_hopSequence.push_back (0);
        }
      else
        {
          uint32_t sum = 0;
          for (uint32_t k = i * 10 + 1; k <= i * 10 + 9; k++)
            {
              NS_ASSERT (k < m_goldSequence.size ());
              sum += m_goldSequence[k] * (uint32_t) std::pow (2, k - (i * 10 + 1));
            }

          if (m_dataHoppingConfig.numSubbands == 2)
            {
              m_hopSequence.push_back (uint8_t ((fhop_prev + sum) % m_dataHoppingConfig.numSubbands));
            }
          else if (m_dataHoppingConfig.numSubbands > 2)
            {
              sum = sum % (m_dataHoppingConfig.numSubbands - 1);
              m_hopSequence.push_back (uint8_t ((fhop_prev + sum + 1) % m_dataHoppingConfig.numSubbands));
            }
          else
            {
              NS_FATAL_ERROR (this << " INVALID NUMBER OF SUBBANDS FOR FREQUENCY HOPPING. ONLY VALUES 1, 2, OR 4 ARE VALID.");
            }
        }
      fhop_prev =  m_hopSequence.back ();
    }
}

void
SidelinkCommResourcePool::GenerateGoldSequence ()
{
  NS_LOG_FUNCTION (this);
  NS_LOG_INFO ("hoppingParameter: " << m_dataHoppingConfig.hoppingParameter);
  // This function computes the entire pseudo-random sequence as defined in TS36.211, Section 7.2.
  uint32_t maxMpn = 102400; //10239 * 10 + 10;     //max sequence length, derived from TS36.211, 5.3.4, where fhop(i) is defined, and i = SF number.
  uint32_t Nc = 1600; //derived from TS36.211 7.2
  uint8_t *x1 = new uint8_t [Nc + maxMpn + 31]; //derived from TS36.211 7.2
  uint8_t *x2 = new uint8_t [Nc + maxMpn + 31]; //derived from TS36.211 7.2

  //initialize x1
  x1[0] = 1;
  for (uint32_t i = 1; i < 31; ++i)
    {
      x1[i] = 0;
    }

  //initialize x2
  for (uint32_t i = 0; i < 31; ++i)
    {
      //here we are doing a shift right because we have to divide cinit by 2^i.
      // Additionally, cinit = NcellId because we are using frame structure type 1
      // for FDD.
      x2[i] = (m_dataHoppingConfig.hoppingParameter >> i) & 0x01;
    }

  //once we have generated the initial values we must solve for all additional values
  for (uint32_t i = 0; i < Nc + maxMpn; ++i)
    {
      x1[i + 31] = (x1[i + 3] + x1[i]) % 2;
      x2[i + 31] = (x2[i + 3] + x2[i + 2] + x2[i + 1] + x2[i]) % 2;
    }

  //Ensure the vector is empty in case this function gets called again.
  if (m_goldSequence.size () != 0)
    {
      m_goldSequence.clear ();
    }

  uint32_t sum = 0;
  uint32_t total_sum_test = 0;
  for (uint32_t i = 0; i < maxMpn; ++i)
    {
      sum = (x1[i + Nc] + x2[i + Nc]);
      m_goldSequence.push_back (sum % 2);
      total_sum_test += sum % 2;
    }
  NS_LOG_INFO ("GoldSeq size " << m_goldSequence.size () << ", Gold sum " << total_sum_test);

  delete [] x1;
  delete [] x2;
}

uint8_t
SidelinkCommResourcePool::FMirroringFunction (uint32_t i)
{
  NS_LOG_FUNCTION (this << i);
  //return 0; //Mirroring is not supported due to ns3 limitations

  if (m_dataHoppingConfig.numSubbands == 1)
    {
      return i % 2;
    }
  else if (m_dataHoppingConfig.numSubbands > 1)
    {
      return m_goldSequence[i * 10];
    }
  else
    {
      NS_FATAL_ERROR (this << " INVALID NUMBER OF SUBBANDS FOR FREQUENCY HOPPING. ONLY VALUES 1, 2, OR 4 ARE VALID.");
    }

}

void
SidelinkCommResourcePool::ComputeNumberOfPscchResources ()
{
  NS_LOG_FUNCTION (this);
  //3GPP TS 36.213 14.2.1.1
  //number of usable subframes
  for (uint32_t i = 0; i < m_scTfResourceConfig.subframeBitmap.bitmap.size (); i++)
    {
      if (m_scTfResourceConfig.subframeBitmap.bitmap.test (i))
        {
          //found a subframe that is part of the PSCCH pool
          m_lpscchVector.push_back (i);
        }
    }
  m_lpscch = m_lpscchVector.size ();
  //number of usable RBs
  for (int i = m_scTfResourceConfig.prbStart; i <= m_scTfResourceConfig.prbEnd; i++)
    {
      if (i < m_scTfResourceConfig.prbStart + m_scTfResourceConfig.prbNum
          || i > m_scTfResourceConfig.prbEnd - m_scTfResourceConfig.prbNum)
        {
          //found an RB that is part of the PSCCH pool
          m_rbpscchVector.push_back (uint32_t (i));
        }
    }
  m_rbpscch = m_rbpscchVector.size ();
  m_nPscchResources = std::floor (m_lpscch * m_rbpscch / 2);
  NS_LOG_DEBUG ("L_pscch = " << m_lpscch << ", m_rb = " << m_rbpscch << ", m_nPscchResources = " << m_nPscchResources);
}

void
SidelinkCommResourcePool::ComputeNumberOfPsschResources ()
{
  NS_LOG_FUNCTION (this);
  //number of usable subframes
  //m_lpssch = 0;
  if (m_type == SidelinkCommResourcePool::SCHEDULED)
    {
      //3GPP TS 36.213 14.1.4 UE procedure for determining subframe pool for Sidelink transmission mode 1
      //all subframes with index greater than lpscch + 1
      for (uint32_t i = m_lpscchVector.at (m_lpscch - 1) + 1; i < LteRrcSap::PeriodAsInt (m_scPeriod); i++)
        {
          m_lpsschVector.push_back (i);
        }
      m_lpssch = m_lpsschVector.size ();
      m_rbpssch = 0;
    }
  else if (m_type == SidelinkCommResourcePool::UE_SELECTED)
    {
      //section 14.1.3
      uint32_t b_j;
      for (uint32_t i = m_dataTfResourceConfig.offsetIndicator.offset; i < LteRrcSap::PeriodAsInt (m_scPeriod); i++)
        {
          b_j = (i - m_dataTfResourceConfig.offsetIndicator.offset) % m_dataTfResourceConfig.subframeBitmap.bitmap.size ();
          if (m_dataTfResourceConfig.subframeBitmap.bitmap.test (b_j))
            {
              m_lpsschVector.push_back (i);
            }
        }
      m_lpssch = m_lpsschVector.size ();

      uint32_t pool_index = 0;
      for (uint32_t i = m_dataTfResourceConfig.prbStart; i <= m_dataTfResourceConfig.prbEnd; i++)
        {
          if (IsInPool (i, 1))
            {
              m_rbpsschVector.push_back (i);
              //Associate pool index to the corresponding RB index
              m_rbpsschPoolPrbToVrbIndexMap[i] = pool_index;
              pool_index++;
            }
        }
      m_rbpssch = m_rbpsschVector.size ();  //number of usable RBs
      if (m_dataHoppingConfig.hoppingInfo == 3)   //Type 2 frequency hopping
        {
          GenerateGoldSequence ();
          GenerateHopSequence ();
        }

    }
  NS_LOG_DEBUG ("PSSCH subframes = " << m_lpssch << ", RBs = " << m_rbpssch);
}

///// SidelinkRxCommResourcePool //////
TypeId
SidelinkRxCommResourcePool::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::SidelinkRxCommResourcePool")
    .SetParent<SidelinkCommResourcePool> ()
    .AddConstructor<SidelinkRxCommResourcePool> ()
  ;
  return tid;
}

///// SidelinkTxCommResourcePool //////
TypeId
SidelinkTxCommResourcePool::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::SidelinkTxCommResourcePool")
    .SetParent<SidelinkCommResourcePool> ()
    .AddConstructor<SidelinkTxCommResourcePool> ()
  ;
  return tid;
}

void
SidelinkTxCommResourcePool::SetPool (LteRrcSap::SlCommResourcePool pool)
{
  NS_LOG_FUNCTION (this);
  SidelinkCommResourcePool::SetPool (pool);
  m_index = 0;
  m_mcs = 0;
  //check Tx parameters
  if (m_type == SidelinkCommResourcePool::UE_SELECTED)
    {
      NS_ASSERT (pool.haveTxParameters);
      m_scTxParameters = pool.txParameters.scTxParameters;
      m_dataTxParameters = pool.txParameters.dataTxParameters;
    }
}

void
SidelinkTxCommResourcePool::SetPool (LteRrcSap::SlPreconfigCommPool pool)
{
  NS_LOG_FUNCTION (this);
  SidelinkCommResourcePool::SetPool (pool);
  m_index = 0;
  m_mcs = 0;
  m_scTxParameters = pool.scTxParameters;
  m_dataTxParameters = pool.dataTxParameters;
}

void
SidelinkTxCommResourcePool::SetScheduledTxParameters (uint16_t slrnti, LteRrcSap::SlMacMainConfigSl macMainConfig, LteRrcSap::SlCommResourcePool commTxConfig, uint8_t index)
{
  NS_LOG_FUNCTION (this);
  m_slrnti = slrnti;
  m_macMainConfig = macMainConfig;
  m_commTxConfig = commTxConfig;
  m_index = index;
}

void
SidelinkTxCommResourcePool::SetScheduledTxParameters (uint16_t slrnti, LteRrcSap::SlMacMainConfigSl macMainConfig, LteRrcSap::SlCommResourcePool commTxConfig, uint8_t index, uint8_t mcs)
{
  NS_LOG_FUNCTION (this);
  SetScheduledTxParameters (slrnti, macMainConfig, commTxConfig, index);
  m_mcs = mcs;
}

void
SidelinkTxCommResourcePool::SetUeSelectedTxParameters (uint8_t identity)
{
  NS_LOG_FUNCTION (this << identity);
  m_poolIdentity = identity;
}

uint8_t
SidelinkTxCommResourcePool::GetIndex ()
{
  NS_LOG_FUNCTION (this);
  return m_index;
}

uint8_t
SidelinkTxCommResourcePool::GetMcs ()
{
  NS_LOG_FUNCTION (this);
  return m_mcs;
}

uint8_t
SidelinkTxCommResourcePool::GetPoolIdentity ()
{
  NS_LOG_FUNCTION (this);
  return m_poolIdentity;
}

bool 
SidelinkTxCommResourcePool::operator == (const SidelinkTxCommResourcePool& rhs)
{
  bool equal = m_scTxParameters == rhs.m_scTxParameters
  && m_dataTxParameters == rhs.m_dataTxParameters
  && m_poolIdentity == rhs.m_poolIdentity
  && m_slrnti == rhs.m_slrnti
  && m_macMainConfig == rhs.m_macMainConfig
  && m_commTxConfig == rhs.m_commTxConfig
  && m_index == rhs.m_index
  && m_haveMcs == rhs.m_haveMcs;
  if (equal && m_haveMcs)
    {
      equal = equal && m_mcs == rhs.m_mcs;
    }
  
  return equal;
}

///// SidelinkDiscResourcePool //////
SidelinkDiscResourcePool::SidelinkDiscResourcePool (void) : m_type (SidelinkDiscResourcePool::UNKNOWN)
{
  NS_LOG_FUNCTION (this);
  m_preconfigured = false;
}

SidelinkDiscResourcePool::~SidelinkDiscResourcePool (void)
{
  NS_LOG_FUNCTION (this);
}

TypeId
SidelinkDiscResourcePool::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::SidelinkDiscResourcePool")
    .SetParent<Object> ()
    .AddConstructor<SidelinkDiscResourcePool> ()
  ;
  return tid;
}

void
SidelinkDiscResourcePool::SetPool (LteRrcSap::SlDiscResourcePool pool)
{
  NS_LOG_FUNCTION (this);
  //parse information
  m_discCpLen = pool.cpLen;
  m_discPeriod = pool.discPeriod;
  NS_ABORT_MSG_IF (pool.numRetx > 3, "For discovery maximum number of retransmissions "
                   "should be less than " << (uint16_t) pool.numRetx);
  m_numRetx = pool.numRetx;
  m_numRepetition = pool.numRepetition;
  m_discTfResourceConfig = pool.tfResourceConfig;
  m_haveTxParameters = pool.haveTxParameters;
  m_txParametersGeneral = pool.txParameters.txParametersGeneral;
  if (pool.haveTxParameters)
    {
      m_type = SidelinkDiscResourcePool::UE_SELECTED;
    }
  else
    {
      m_type = SidelinkDiscResourcePool::SCHEDULED;
    }
  Initialize ();
}

void
SidelinkDiscResourcePool::SetPool (LteRrcSap::SlPreconfigDiscPool pool)
{
  NS_LOG_FUNCTION (this);
  //preconfigured pools are always UE selected
  m_type = SidelinkDiscResourcePool::UE_SELECTED;
  m_preconfigured = true;
  m_discCpLen = pool.cpLen;
  m_discPeriod = pool.discPeriod;
  NS_ABORT_MSG_IF (pool.numRetx > 3, "For discovery maximum number of retransmissions "
                   "should be less than " << (uint16_t)pool.numRetx);
  m_numRetx = pool.numRetx;
  m_numRepetition = pool.numRepetition;
  m_discTfResourceConfig = pool.tfResourceConfig;
  Initialize ();
}


void
SidelinkDiscResourcePool::Initialize ()
{
  NS_LOG_FUNCTION (this);
  ComputeNumberOfPsdchResources ();
  BuildListOfConflictingResources ();
}

void
SidelinkDiscResourcePool::BuildListOfConflictingResources ()
{
  //build the list of resources that uses the same subframes
  std::vector < std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo> > txInfoVector;
  for (uint32_t i = 0; i < m_nPsdchResources; i++)
    {
      NS_LOG_DEBUG ("  Checking resource " << i);
      std::set <uint32_t> emptySet;
      m_resourceConflictsVector.push_back (emptySet);
      //get transmission information for resource i
      std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo> txInfo = GetPsdchTransmissions (i);
      //find the list of resources that collide with each of the transmission of resource i by looking
      //at the list of previous resource information
      std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo>::iterator itNewTx;
      for (itNewTx = txInfo.begin (); itNewTx != txInfo.end (); itNewTx++)
        {
          //check for overlapping with any of the previous transmission
          for (uint32_t j = 0; j < i; j++)
            {
              std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo>::iterator itPrevTx;
              for (itPrevTx = txInfoVector.at (j).begin (); itPrevTx != txInfoVector.at (j).end (); itPrevTx++)
                {
                  if ((*itNewTx).subframe == (*itPrevTx).subframe)
                    {
                      NS_LOG_DEBUG ("    Resource " << j << " collides on subframe " << (*itNewTx).subframe.frameNo << "/" << (*itNewTx).subframe.subframeNo);
                      //Overlapping subframe
                      if (m_resourceConflictsVector.at (i).find (j) == m_resourceConflictsVector.at (i).end ())
                        {
                          NS_LOG_DEBUG ("      Adding to conflict list");
                          m_resourceConflictsVector.at (i).insert (j);
                          m_resourceConflictsVector.at (j).insert (i);
                        }
                    }
                }
            }
          //add himself to list of conflicts
          m_resourceConflictsVector.at (i).insert (i);
        }

      txInfoVector.push_back (txInfo);
    }
}

SidelinkDiscResourcePool::SlPoolType
SidelinkDiscResourcePool::GetSchedulingType ()
{
  NS_LOG_FUNCTION (this);
  return m_type;
}

SidelinkDiscResourcePool::SubframeInfo
SidelinkDiscResourcePool::GetCurrentDiscPeriod (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this << frameNo << subframeNo);
  SubframeInfo currentDiscPeriod;
  int32_t subframe = 10 * (frameNo % 1024) + subframeNo % 10;
  int32_t period = LteRrcSap::DiscPeriodAsInt (m_discPeriod);
  int32_t currentPeriod = std::floor ((subframe - m_discTfResourceConfig.offsetIndicator.offset) / (double) period);

  uint32_t currentStart = m_discTfResourceConfig.offsetIndicator.offset + currentPeriod * period;
  currentDiscPeriod.frameNo = (currentStart / 10) % 1024;
  currentDiscPeriod.subframeNo = currentStart % 10;
  return currentDiscPeriod;
}

SidelinkDiscResourcePool::SubframeInfo
SidelinkDiscResourcePool::GetNextDiscPeriod (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this << frameNo << subframeNo);
  SubframeInfo nextDiscPeriod;
  int32_t subframe = 10 * (frameNo % 1024) + subframeNo % 10;
  int32_t period = LteRrcSap::DiscPeriodAsInt (m_discPeriod);
  int32_t currentPeriod = std::floor ((subframe - m_discTfResourceConfig.offsetIndicator.offset) / (double) period);

  //if the frame is the last period, we need to start over
  if (currentPeriod >= ((10240 - m_discTfResourceConfig.offsetIndicator.offset) / period) - 1)
    {
      currentPeriod = -1;
    }
  uint32_t nextStart = (m_discTfResourceConfig.offsetIndicator.offset + (currentPeriod + 1) * period);
  nextDiscPeriod.frameNo = (nextStart / 10) % 1024;
  nextDiscPeriod.subframeNo = nextStart % 10;
  return nextDiscPeriod;
}

uint32_t
SidelinkDiscResourcePool::GetNPsdch ()
{
  NS_LOG_FUNCTION (this);
  return m_nPsdchResources;
}

uint32_t
SidelinkDiscResourcePool::GetNSubframes ()
{
  NS_LOG_FUNCTION (this);
  return m_lpsdch;
}

uint32_t
SidelinkDiscResourcePool::GetNRbPairs ()
{
  NS_LOG_FUNCTION (this);
  return m_rbpsdch / 2;
}

uint8_t
SidelinkDiscResourcePool::GetNumRetx ()
{
  NS_LOG_FUNCTION (this);
  return m_numRetx;
}

int32_t
SidelinkDiscResourcePool::GetDiscPeriod ()
{
  NS_LOG_FUNCTION (this);
  return LteRrcSap::DiscPeriodAsInt (m_discPeriod);
}


std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo>
SidelinkDiscResourcePool::GetPsdchTransmissions (uint32_t npsdch)
{
  NS_LOG_FUNCTION (this << npsdch);
  std::list<SidelinkDiscResourcePool::SidelinkTransmissionInfo> txInfo;

  // 36.213 14.3.1
  uint32_t n = m_numRetx + 1;
  uint32_t nf = std::floor (m_rbpsdch / 2);
  uint32_t nt = std::floor (m_lpsdch / n);
  uint32_t aj;
  uint32_t b1;
  NS_ABORT_MSG_IF (m_lpsdch < n, "No subframe available for retransmission. Check the discovery bitmap");

  if (m_type == SidelinkDiscResourcePool::UE_SELECTED)
    {
      for (uint32_t j = 1; j <= n; j++)
        {
          uint32_t inter = ((j - 1) * std::floor (nf / n)) + std::floor (npsdch / nt);
          aj = inter % nf;
          b1 = npsdch % nt;
          SidelinkDiscResourcePool::SidelinkTransmissionInfo info;
          uint32_t subframe = (n * b1) + j - 1;
          info.subframe.frameNo = subframe / 10;
          info.subframe.subframeNo = subframe % 10;
          info.rbStart = std::floor (2 * aj);
          info.nbRb = 2;
          txInfo.push_back (TranslatePsdch (info));
        }
    }
  else if (m_type == SidelinkDiscResourcePool::SCHEDULED)
    {
      // not implemented yet //
      // TODO
    }


  return txInfo;
}

void
SidelinkDiscResourcePool::ComputeNumberOfPsdchResources ()
{
  NS_LOG_FUNCTION (this);
  //36.213 14.3.3
  if (m_type == SidelinkDiscResourcePool::UE_SELECTED)
    {
      //subframes pool
      uint32_t bj;
      for (uint32_t j = 0; j < (m_numRepetition * m_discTfResourceConfig.subframeBitmap.bitmap.size ()); j++)
        {
          bj = j % m_discTfResourceConfig.subframeBitmap.bitmap.size ();
          if (m_discTfResourceConfig.subframeBitmap.bitmap.test (bj))
            {
              m_lpsdchVector.push_back (j);
            }
        }

      m_lpsdch = m_lpsdchVector.size ();

      //resource blocks pool
      for (uint8_t i = m_discTfResourceConfig.prbStart; i <= m_discTfResourceConfig.prbEnd; i++)
        {
          if ((i < m_discTfResourceConfig.prbStart + m_discTfResourceConfig.prbNum) || (i > m_discTfResourceConfig.prbEnd - m_discTfResourceConfig.prbNum))
            {
              m_rbpsdchVector.push_back ((uint32_t)i);
            }
        }
      m_rbpsdch = m_rbpsdchVector.size ();

      //validate RBs to make sure that each opportunity is composed of two contiguous RBs
      for (uint32_t i = 0; i < std::floor (m_rbpsdch / 2); i++)
        {
          //check that m_rbpsdchVector[i] and m_rbpsdchVector[i + 1] are contiguous RBs
          NS_ABORT_MSG_IF (m_rbpsdchVector[(2 * i) + 1] != m_rbpsdchVector[(2 * i)] + 1,
                           "Non contiguous Rbs for Sidelink Discovery pool ("
                           << m_rbpsdchVector[2 * i] << " and " << m_rbpsdchVector[2 * (i + 1)] << ")");
        }
    }

  else if (m_type == SidelinkDiscResourcePool::SCHEDULED)
    {
      // Not implemented yet //
      //TODO
      /*
      for (uint32_t i = 0; i < m_discTfIndexList.nbPair; i++)
      {
        m_lpsdchVector.push_back (m_discTfIndexList.pair [i].discSfIndex);
        m_rbpsdchVector.push_back (m_discTfIndexList.pair [i].discPrbIndex);
      }
      m_lpsdch = m_lpsdchVector.size();
      m_rbpsdch = m_rbpsdchVector.size();
      */
    }

  m_nPsdchResources = std::floor (m_lpsdch / (m_numRetx + 1)) * std::floor (m_rbpsdch / 2);
  NS_LOG_DEBUG ("l_psdch = " << m_lpsdch << ", m_rb = " << m_rbpsdch << ", m_nPsdchResources = " << m_nPsdchResources);
}

SidelinkDiscResourcePool::SidelinkTransmissionInfo
SidelinkDiscResourcePool::TranslatePsdch (SidelinkDiscResourcePool::SidelinkTransmissionInfo info)
{
  NS_LOG_FUNCTION (this);
  SidelinkDiscResourcePool::SidelinkTransmissionInfo translatedInfo;

  //For FDD the bitmap is 40 bits so assuming that all the subframe are set
  //then the maximum is 39
  uint32_t frameNo = 10 * info.subframe.frameNo + info.subframe.subframeNo;
  NS_ASSERT (frameNo < m_lpsdchVector.size ());

  uint32_t mappedFrame = m_lpsdchVector.at (frameNo);
  translatedInfo.subframe.frameNo = mappedFrame / 10;
  translatedInfo.subframe.subframeNo = mappedFrame % 10;
  NS_LOG_DEBUG ("Mapped frame/subframe number " << info.subframe.frameNo << "/" << info.subframe.subframeNo << " to frame/subframe number "
                                                << translatedInfo.subframe.frameNo << "/" << translatedInfo.subframe.subframeNo);

  //translate the RB
  NS_ASSERT (info.rbStart != m_rbpsdchVector.size ());
  translatedInfo.rbStart = m_rbpsdchVector.at (info.rbStart);
  translatedInfo.nbRb = info.nbRb;

  return translatedInfo;
}

std::vector<uint32_t>
SidelinkDiscResourcePool::GetPsdchOpportunities (uint32_t frameNo, uint32_t subframeNo)
{
  NS_LOG_FUNCTION (this << frameNo << subframeNo);
  std::vector<uint32_t> opportunities;
  //first check if the subframe is part of the discovery period
  int32_t subframe = 10 * (frameNo % 1024) + subframeNo % 10;
  int32_t period = LteRrcSap::DiscPeriodAsInt (m_discPeriod);
  int32_t currentPeriod = std::floor ((subframe - m_discTfResourceConfig.offsetIndicator.offset) / (double) period);
  uint32_t currentStart = m_discTfResourceConfig.offsetIndicator.offset + currentPeriod * period;

  //check if we are still within the frames that could contain PSDCH opportunities
  if (subframe - currentStart < m_discTfResourceConfig.subframeBitmap.bitmap.size ())
    {
      //check if the bitmap is set for this subframe
      if (m_discTfResourceConfig.subframeBitmap.bitmap.test (subframe - currentStart))
        {
          //subframe is set, report the RBs that could be used
          for (std::vector<uint32_t>::iterator it = m_rbpsdchVector.begin (); it != m_rbpsdchVector.end (); it++)
            {
              opportunities.push_back ((*it));
            }
        }
    }
  return opportunities;
}

uint32_t
SidelinkDiscResourcePool::TxProbabilityAsInt (LteRrcSap::TxProbability prob)
{
  uint32_t p = 0;
  switch (prob.probability)
    {
    case LteRrcSap::TxProbability::p25:
      p = 25;
      break;
    case LteRrcSap::TxProbability::p50:
      p = 50;
      break;
    case LteRrcSap::TxProbability::p75:
      p = 75;
      break;
    case LteRrcSap::TxProbability::p100:
      p = 100;
      break;
    default:
      NS_FATAL_ERROR ("Unknown value of TxProbability");
    }
  return p;
}

LteRrcSap::TxProbability
SidelinkDiscResourcePool::TxProbabilityFromInt (uint32_t p)
{
  LteRrcSap::TxProbability prob;
  switch (p)
    {
    case 25:
      prob.probability = LteRrcSap::TxProbability::p25;
      break;
    case 50:
      prob.probability = LteRrcSap::TxProbability::p50;
      break;
    case 75:
      prob.probability = LteRrcSap::TxProbability::p75;
      break;
    case 100:
      prob.probability = LteRrcSap::TxProbability::p100;
      break;
    default:
      NS_FATAL_ERROR ("Unknown value of TxProbability");
    }
  return prob;
}

std::set<uint32_t>
SidelinkDiscResourcePool::GetConflictingResources (uint32_t res)
{
  NS_LOG_FUNCTION (this << res);
  NS_ASSERT (res < m_nPsdchResources);
  return m_resourceConflictsVector.at (res);
}

///// SidelinkRxDiscResourcePool //////
TypeId
SidelinkRxDiscResourcePool::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::SidelinkRxDiscResourcePool")
    .SetParent<SidelinkDiscResourcePool> ()
    .AddConstructor<SidelinkRxDiscResourcePool> ()
  ;
  return tid;
}

///// SidelinkTxDiscResourcePool //////
TypeId
SidelinkTxDiscResourcePool::GetTypeId (void)
{
  static TypeId
    tid =
    TypeId ("ns3::SidelinkTxDiscResourcePool")
    .SetParent<SidelinkDiscResourcePool> ()
    .AddConstructor<SidelinkTxDiscResourcePool> ()
  ;
  return tid;
}

void
SidelinkTxDiscResourcePool::SetPool (LteRrcSap::SlDiscResourcePool pool)
{
  NS_LOG_FUNCTION (this);
  SidelinkDiscResourcePool::SetPool (pool);

  //check Tx parameters
  if (m_type == SidelinkDiscResourcePool::UE_SELECTED)
    {
      NS_ASSERT (pool.haveTxParameters);
      m_haveTxParameters = pool.haveTxParameters;
      m_txProbability = pool.txParameters.ueSelectedResourceConfig.txProbability;
      m_poolSelection = pool.txParameters.ueSelectedResourceConfig.poolSelection;
      m_havePoolSelectionRsrpBased = pool.txParameters.ueSelectedResourceConfig.havePoolSelectionRsrpBased;
      if (pool.txParameters.ueSelectedResourceConfig.havePoolSelectionRsrpBased)
        {
          m_poolSelectionRsrpBased.threshLow = pool.txParameters.ueSelectedResourceConfig.poolSelectionRsrpBased.threshLow;
          m_poolSelectionRsrpBased.threshHigh = pool.txParameters.ueSelectedResourceConfig.poolSelectionRsrpBased.threshHigh;
        }
      else
        {
          /*random */
        }
      m_txProbChanged = false;
    }
}

void
SidelinkTxDiscResourcePool::SetPool (LteRrcSap::SlPreconfigDiscPool pool)
{
  NS_LOG_FUNCTION (this);
  SidelinkDiscResourcePool::SetPool (pool);
  m_txParametersGeneral = pool.txParameters.txParametersGeneral;
  m_txProbability = pool.txParameters.txProbability;
  m_txProbChanged = false;
}


void
SidelinkTxDiscResourcePool::SetScheduledTxParameters (LteRrcSap::SlDiscResourcePool discPool, LteRrcSap::SlTfIndexPairList discResources, LteRrcSap::SlHoppingConfigDisc discHopping)
{
  NS_LOG_FUNCTION (this);
  m_discTxConfig = discPool;
  m_discTfIndexList = discResources;
  m_discHoppingConfigDisc = discHopping;
}

void
SidelinkTxDiscResourcePool::SetUeSelectedTxParameters (uint8_t identity)
{
  NS_LOG_FUNCTION (this << identity);
  m_poolIdentity = identity;
}

uint32_t
SidelinkTxDiscResourcePool::GetTxProbability ()
{
  NS_LOG_FUNCTION (this);
  return SidelinkDiscResourcePool::TxProbabilityAsInt (m_txProbability);
}

void SidelinkTxDiscResourcePool::SetTxProbability (uint32_t theta)
{
  NS_LOG_FUNCTION (this);
  LteRrcSap::TxProbability txProb;
  switch (theta)
    {
    case 25:
      txProb.probability = LteRrcSap::TxProbability::p25;
      break;
    case 50:
      txProb.probability = LteRrcSap::TxProbability::p50;
      break;
    case 75:
      txProb.probability = LteRrcSap::TxProbability::p75;
      break;
    case 100:
      txProb.probability = LteRrcSap::TxProbability::p100;
      break;
    default:
      NS_FATAL_ERROR ("Unknown value of TxProbability");
    }
  m_txProbability = txProb;
}

uint8_t
SidelinkTxDiscResourcePool::GetPoolIdentity ()
{
  NS_LOG_FUNCTION (this);
  return m_poolIdentity;
}


}

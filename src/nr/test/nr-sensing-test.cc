/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2022 University of Washington
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <string>
#include <array>
#include <bitset>
#include <unordered_map>
#include <ns3/test.h>
#include <ns3/log.h>
#include <ns3/boolean.h>
#include <ns3/uinteger.h>
#include <ns3/integer.h>
#include <ns3/double.h>
#include <ns3/nr-ue-mac.h>
#include <ns3/nr-ue-phy.h>
#include <ns3/nr-sl-comm-resource-pool.h>
#include <ns3/nr-sl-comm-resource-pool-factory.h>
#include <ns3/lte-rrc-sap.h>
#include <ns3/nr-sl-ue-rrc.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NrSensingTest");

// Helper function for configuring a resource pool 
// similarly to what is found in cttc-nr-v2x-demo-simple.cc
Ptr<NrSlCommResourcePool>
CreateNrSlCommResourcePool(uint16_t numSubchannels)
{
  Ptr<NrSlCommResourcePool> slPool = CreateObject<NrSlCommResourcePool> ();
  Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory> ();
  std::vector <std::bitset<1> > slBitmap = {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1};
  ptrFactory->SetSlTimeResources (slBitmap);
  ptrFactory->SetSlSensingWindow (100); // T0 in ms
  ptrFactory->SetSlSelectionWindow (5);
  ptrFactory->SetSlFreqResourcePscch (10); // PSCCH RBs
  ptrFactory->SetSlSubchannelSize (50); // RBs
  ptrFactory->SetSlMaxNumPerReserve (3);
  std::list<uint16_t> resourceReservePeriodList = {0, 100}; // in ms
  ptrFactory->SetSlResourceReservePeriodList (resourceReservePeriodList);
  //Once parameters are configured, we can create the pool
  LteRrcSap::SlResourcePoolNr pool = ptrFactory->CreatePool ();
  LteRrcSap::SlResourcePoolNr slResourcePoolNr = pool;

  std::array <LteRrcSap::SlFreqConfigCommonNr, 1> slPreconfigFreqInfoList;
  LteRrcSap::SlFreqConfigCommonNr slFreConfigCommonNr;
  LteRrcSap::SlBwpConfigCommonNr slBwpConfigCommonNr;
  LteRrcSap::SlBwpGeneric slBwpGeneric;
  LteRrcSap::SlResourcePoolConfigNr slresoPoolConfigNr;
  slresoPoolConfigNr.haveSlResourcePoolConfigNr = true;
  //Pool id, ranges from 0 to 15
  uint16_t poolId = 0;
  LteRrcSap::SlResourcePoolIdNr slResourcePoolIdNr;
  slResourcePoolIdNr.id = poolId;
  slresoPoolConfigNr.slResourcePoolId = slResourcePoolIdNr;
  slresoPoolConfigNr.slResourcePool = slResourcePoolNr;

  //Configure the SlBwpPoolConfigCommonNr IE, which hold an array of pools
  LteRrcSap::SlBwpPoolConfigCommonNr slBwpPoolConfigCommonNr;
  //Array for pools, we insert the pool in the array as per its poolId
  slBwpPoolConfigCommonNr.slTxPoolSelectedNormal [slResourcePoolIdNr.id] = slresoPoolConfigNr;

  //Configure the BWP IE
  LteRrcSap::Bwp bwp;
  bwp.numerology = 2;
  bwp.symbolsPerSlots = 14;
  bwp.rbPerRbg = 1;
  bwp.bandwidth = 400 * numSubchannels; // 40 MHz = 400 (100 KHz blocks)
  // one RB at numerology 2 is 720 Khz, so 40 MHz is roughly 55 RBs

  slBwpGeneric.bwp = bwp;
  slBwpGeneric.slLengthSymbols = LteRrcSap::GetSlLengthSymbolsEnum (14);
  slBwpGeneric.slStartSymbol = LteRrcSap::GetSlStartSymbolEnum (0);

  slBwpConfigCommonNr.haveSlBwpGeneric = true;
  slBwpConfigCommonNr.slBwpGeneric = slBwpGeneric;
  slBwpConfigCommonNr.haveSlBwpPoolConfigCommonNr = true;
  slBwpConfigCommonNr.slBwpPoolConfigCommonNr = slBwpPoolConfigCommonNr;

  std::set<uint8_t> bwpIdContainer;
  bwpIdContainer.insert (0);
  for (const auto &it:bwpIdContainer)
    {
      slFreConfigCommonNr.slBwpList [it] = slBwpConfigCommonNr;
    }

  slPreconfigFreqInfoList [0] = slFreConfigCommonNr;
  slPool->SetNrSlPreConfigFreqInfoList (slPreconfigFreqInfoList);

  // From LteUeRrc::PopulateNrSlPools ()
  std::array <LteRrcSap::SlBwpConfigCommonNr, 4> slBwpList;
  slBwpList = slPreconfigFreqInfoList [0].slBwpList;

  std::unordered_map<uint8_t, std::unordered_map <uint16_t, std::vector <std::bitset<1>>> > mapPerBwp;
  std::unordered_map <uint16_t, std::vector <std::bitset<1>>> mapPerPool;
  std::set <uint8_t> bwpIds;
  bwpIds.insert (0);

  std::vector<NrSlUeRrc::LteNrTddSlotType> tddPattern = { NrSlUeRrc::DL, NrSlUeRrc::DL, NrSlUeRrc::DL,
    NrSlUeRrc::F, NrSlUeRrc::UL, NrSlUeRrc::UL, NrSlUeRrc::UL, NrSlUeRrc::UL,
    NrSlUeRrc::UL, NrSlUeRrc::UL};

  for (uint8_t index = 0; index < slBwpList.size (); ++index)
    {
      //index of slBwpList is used as BWP id
      //send SL pool to only that BWP for which SlBwpGeneric and SlBwpPoolConfigCommonNr are configured.
      if (slBwpList [index].haveSlBwpGeneric && slBwpList [index].haveSlBwpPoolConfigCommonNr)
        {
          auto it = bwpIds.find (index);
          NS_ASSERT_MSG (it != bwpIds.end (), "UE is not prepared to use BWP id " << +index << " for SL");

          std::array <LteRrcSap::SlResourcePoolConfigNr, 8> txPoolList;
          txPoolList = slBwpList [index].slBwpPoolConfigCommonNr.slTxPoolSelectedNormal;
          for (const auto& it:txPoolList) // fill the map per pool
            {
              if (it.haveSlResourcePoolConfigNr) // if this true, it means pools are set
                {
                  //check if subchannel size in RBs is less or equal to the total
                  //available BW is RBs
                  uint16_t sbChSizeInRbs = LteRrcSap::GetNrSlSubChSizeValue (it.slResourcePool.slSubchannelSize);
                  uint16_t numPscchRbs = LteRrcSap::GetSlFResoPscchValue (it.slResourcePool.slPscchConfig.slFreqResourcePscch);
                  NS_ASSERT_MSG (numPscchRbs <= sbChSizeInRbs, "Incorrect number of PSCCH RBs : "
                                 << numPscchRbs << " . Must be less or equal to the SL subchannel size of " << sbChSizeInRbs << " RBs");
                  std::vector <std::bitset<1>> physicalPool = NrSlUeRrc::GetPhysicalSlPool (it.slResourcePool.slTimeResource, tddPattern);
                  mapPerPool.emplace (std::make_pair (it.slResourcePoolId.id, physicalPool));
                }
            }

          NS_ASSERT_MSG (mapPerPool.size () > 0, "No SL pool set for BWP " << +index);
          mapPerBwp.emplace (std::make_pair (index, mapPerPool));
        }

      if (mapPerBwp.size () > 0) // we found SL BWP
        {
          slPool = CreateObject <NrSlCommResourcePool> ();
          //slPool->SetNrSlPreConfigFreqInfoList (preConfig.slPreconfigFreqInfoList);
          slPool->SetNrSlPreConfigFreqInfoList (slPreconfigFreqInfoList);
          slPool->SetNrSlPhysicalPoolMap (mapPerBwp);
          slPool->SetNrSlSchedulingType (NrSlCommResourcePool::UE_SELECTED);
          slPool->SetTddPattern (tddPattern);
          mapPerPool.clear ();
          mapPerBwp.clear ();
        }
    }
  return slPool;
}

/*
 * \brief Multi-channel sensing testcase
 *
 * This testcase demonstrates a limitation of the previous sensing
 * implementation, but now fixed in the current code.
 *
 * Sensing is defined in Sec. 8.1.4 of 3GPP TS 38.214.
 * The implementation in ns-3 did not follow the seven-step algorithm
 * exactly as defined in 38.214.  In particular, there are two differences.
 * The first is that candidate resources are not fully considered in both
 * time and frequency when the exclusions are performed in step 6c.
 * Subchannels are marked as occupied or not-occupied, and this information
 * is passed to the scheduler, but the calculation of whether enough
 * resources are available (before incrementing the SNR threshold) is
 * performed on the basis of occupied slots, not occupied resources.
 * Second, the sensing code in the NrUeMac class does not know the size of
 * the transport block (in subchannels) it is trying to obtain candidate
 * resources for.  As a result, the list of candidates that is passed
 * to the scheduler may list a number of partially occupied slots that
 * prevents scheduling, while the algorithm would have iterated and raised
 * RSRP thresholds until finding the least occupied resources with sufficient
 * contiguous subchannels.
 *
 * This testcase creates instances of a resource pool (with two subchannels),
 * an NrUeMac and an NrUePhy, and configures enough of these objects to
 * support the scheduling process (NrUeMac::GetNrSlCandidateResources).
 * Configuration is based heavily on the example program
 * cttc-nr-v2x-demo-simple.cc, except that two subchannels are used.
 * The sensing window is 100 ms (400 slots), and the selection window is
 * 32 slots, some of which (15) are sidelink slots and some of which are not.
 * The resource reservation interval is 100 ms.
 *
 * The test artificially creates sensing data as if an SCI 1-A were received
 * in the first subchannel of each slot in the sensing window.  The SCI 1-A
 * power (RSRP) is weak (-126 dBm)-- only 2 dB above the threshold of
 * -128 dBm, although the test behaves the same even if the configured
 * RSRP is higher.  This artificial sensing data is passed to the NrUeMac
 * object in lieu of actual sensing.
 *
 * At time 2.11 sec (corresponding to the first sensing time found in
 * cttc-nr-v2x-demo-simple), the test calls GetNrSlCandidateResources().
 * This code therefore sets the Sfn to a time corresponding to 2.11 sec
 * and runs the algorithm.  The ns-3 code follows most of the algorithm in
 * TS 38.214 and projects the (past) sensed transmissions into the
 * respective (future) slots in the sensing window.
 * As a result, all sidelink slots in the sensing window are considered
 * to be partially occupied with an RSRP of -126 dBm after the first pass of
 * the algorithm, but only in the first subchannel.
 *
 * If the resources needed for the transport block are two subchannels wide,
 * at the end of the first pass of the algorithm, the algorithm should
 * evaluate and decide that insufficient resources are available,
 * and should increment the threshold by 3 dB and go back to step 4.  
 * Instead, the current code evaluates that all 15 slots are available
 * (because they each have one subchannel out of the two available)
 * and passes this list to the scheduler, with each slot indicating that
 * subchannel 0 is occupied.  The scheduler, at this point, should fail to
 * schedule the grant, because it should find no resources that are
 * two subchannels wide.
 *
 * What should have instead happened in this case is that the algorithm
 * should have iterated one more time and declared all of the resources
 * available above the new RSRP threshold of -125 dBm.  However, in practice,
 * not all interference will be equal power, and in general, the raising
 * of thresholds according to the algorithm will result in the weaker
 * interference resources being selected first.  This distinction between
 * weaker and stronger interferers can be lost in the current code if the
 * algorithm does not iterate as specified.
 */
class NrSensingTestCase : public TestCase
{
public:
  /**
   * \brief Create NrSensingTestCase
   * \param name Name of the test
   */
  NrSensingTestCase (const std::string &name)
    : TestCase (name) {}

private:
  virtual void DoRun (void) override;
  void TraceSensingAlgorithm (const struct NrUeMac::SensingTraceReport& report,
    const std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo>& candidates,
    const std::list<SensingData>& sensingData, const std::list<SfnSf>& transmitHistory) const;
};

void
NrSensingTestCase::TraceSensingAlgorithm (const struct NrUeMac::SensingTraceReport& report,
    const std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo>& candidates,
    const std::list<SensingData>& sensingData, const std::list<SfnSf>& transmitHistory) const
{
  NS_LOG_DEBUG ("Sfn " << report.m_sfn.Normalize () << " candidates " << candidates.size () << " sensing " << sensingData.size () << " history " << transmitHistory.size ());
  NS_LOG_DEBUG ("subch " << report.m_subchannels << " rsrp " << report.m_finalRsrpThreshold
    << " initial slots " << report.m_initialCandidateSlotsSize << " initial resources " 
    << report.m_initialCandidateResourcesSize);
}

void
NrSensingTestCase::DoRun ()
{
  NS_LOG_FUNCTION (this);

  Ptr<NrUeMac> nrUeMac = CreateObject<NrUeMac> ();
  // Set attributes used by the sensing code
  nrUeMac->SetAttribute ("EnableSensing", BooleanValue (true));
  nrUeMac->SetAttribute ("T1", UintegerValue (2));
  nrUeMac->SetAttribute ("T2", UintegerValue (33));
  nrUeMac->SetAttribute ("ResourcePercentage", UintegerValue (20));
  nrUeMac->SetAttribute ("SlThresPsschRsrp", IntegerValue (-128));
  nrUeMac->TraceConnectWithoutContext("SensingAlgorithm", MakeCallback(&NrSensingTestCase::TraceSensingAlgorithm, this));

  // Time 2.11 seconds similar to cttc-nr-v2x-demo-simple first transmission
  SfnSf currentSfn (211, 0, 0, 2);
  // T_0 is 400 slots behind = 100 ms behind
  SfnSf t0Sfn (201, 0, 0, 2);

  // The below code will populate the sensing database with false SCI-1
  // transmissions of one subchannel found in each of the sensed slots
  uint16_t rsvp = 100;  // Resource reservation period
  uint8_t sbChLength = 1; // Sensed energy over 1 subchannel at index 0
  uint8_t sbChStart = 0;
  uint8_t priority = 7; // Priority is unused
  double rsrpDbm = -126; // 2 dB greater than the initial -128 dBm threshold
  std::list<SensingData> sensingDataList;
  for (uint32_t i = 0; i < 400; i++)
    {
      SfnSf slot = t0Sfn;
      slot.Add (i);
      SensingData sensingData (slot, rsvp, sbChLength, sbChStart, priority,
        rsrpDbm, 255, 255, 255, 255);
      sensingDataList.push_back (sensingData);
    }
  std::list<SfnSf> transmitHistory; // empty
  Time slotPeriod = MicroSeconds (250); // numerology 2
  uint64_t imsi = 0; // parameter only used for logging
  uint8_t bwpId = 0;
  uint16_t poolId = 0;
  uint8_t totalSubCh = 2;  // Two subchannels in the BWP

  // Call the sensing algorithm and inspect the list of slots that result.
  // This list would normally be passed to a scheduler as a next step.
  // lSubch is the parameter we are testing.  If set to 1, it should return
  // candidates of width one subchannel.  If set to 2, it should return
  // candidates of width two subchannels (after the logic increases the
  // sensing threshold from -128 dBm to -125 dBm)
  uint16_t lSubch = 1; // Number of subchannels to use
  // The following parameters don't matter here, but are needed for the constructor
  Time packetDelayBudget = MilliSeconds (20);
  Time pRsvpTx = MilliSeconds (100);
  uint16_t cResel = 5;
  NrSlTransmissionParams params {priority, packetDelayBudget, lSubch, pRsvpTx, cResel};
  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> availableReso;
  availableReso = nrUeMac->GetNrSlCandidateResourcesPrivate (currentSfn, params, CreateNrSlCommResourcePool (totalSubCh), 
    slotPeriod, imsi, bwpId, poolId, totalSubCh, sensingDataList, transmitHistory);

  for (const NrSlUeMacSchedSapProvider::NrSlSlotInfo& slot : availableReso)
    {
      NS_TEST_ASSERT_MSG_EQ (availableReso.size (), 15, "Expecting 15 resources");
      NS_TEST_ASSERT_MSG_NE (slot.slSubchannelStart, 0, "Subchannel 0 should not be marked as available");
    }
  // Resetting the subchannel width to 2 should cause 15 resources of two subchannel width
  lSubch = 2;
  NrSlTransmissionParams params2 {priority, packetDelayBudget, lSubch, pRsvpTx, cResel};
  availableReso = nrUeMac->GetNrSlCandidateResourcesPrivate (currentSfn, params2, CreateNrSlCommResourcePool (totalSubCh), 
    slotPeriod, imsi, bwpId, poolId, totalSubCh, sensingDataList, transmitHistory);
  for (const NrSlUeMacSchedSapProvider::NrSlSlotInfo& slot : availableReso)
    {
      NS_TEST_ASSERT_MSG_EQ (availableReso.size (), 15, "Expecting 15 resources");
      NS_TEST_ASSERT_MSG_EQ (slot.slSubchannelStart, 0, "Subchannel 0 should be marked as available");
      NS_TEST_ASSERT_MSG_EQ (slot.slSubchannelLength, 2, "Full subchannel width should be available");
    }


  // Check that sensing data entered in slots 8045 and 8046 cause exclusions
  // 100ms later at slots 8445 and 8446
  std::list<SensingData> sensingDataList2;
  for (uint32_t i = 0; i < 400; i++)
    {
      SfnSf slot = t0Sfn;
      slot.Add (i);
      if (slot.Normalize () == 8045 || slot.Normalize () == 8046)
        {
          SensingData sensingData (slot, rsvp, sbChLength, sbChStart, priority,
            rsrpDbm, 255, 255, 255, 255);
          sensingDataList2.push_back (sensingData);
        }
    }
  availableReso = nrUeMac->GetNrSlCandidateResourcesPrivate (currentSfn, params2, CreateNrSlCommResourcePool (totalSubCh), 
    slotPeriod, imsi, bwpId, poolId, totalSubCh, sensingDataList2, transmitHistory);
  // Two resources (at 8445 and 8446) should have been excluded from the original 15
  NS_TEST_ASSERT_MSG_EQ (availableReso.size (), 13, "Expecting 13 resources");
  for (auto it : availableReso)
    {
      NS_TEST_ASSERT_MSG_NE (it.sfn.Normalize (), 8445, "Slot 8445 should be excluded");
      NS_TEST_ASSERT_MSG_NE (it.sfn.Normalize (), 8446, "Slot 8446 should be excluded");
    }
}

/**
 * \brief Check that a past transmission in the sensing window is appropriately
 *        excluded.
 */
class NrSensingTransmitHistoryTest : public TestCase
{
public:
  /**
   * \brief NrSensingTransmitHistoryTest
   * \param name Name of the test
   */
  NrSensingTransmitHistoryTest (const std::string &name)
    : TestCase (name) {}

private:
  virtual void DoRun (void) override;
  void TraceSensingAlgorithm (const struct NrUeMac::SensingTraceReport& report,
    const std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo>& candidates,
    const std::list<SensingData>& sensingData, const std::list<SfnSf>& transmitHistory) const;
};

void
NrSensingTransmitHistoryTest::TraceSensingAlgorithm (const struct NrUeMac::SensingTraceReport& report,
    const std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo>& candidates,
    const std::list<SensingData>& sensingData, const std::list<SfnSf>& transmitHistory) const
{
  NS_LOG_DEBUG ("Sfn " << report.m_sfn.Normalize () << " candidates " << candidates.size () << " sensing " << sensingData.size () << " history " << transmitHistory.size ());
  NS_LOG_DEBUG ("subch " << report.m_subchannels << " rsrp " << report.m_finalRsrpThreshold
    << " initial slots " << report.m_initialCandidateSlotsSize << " initial resources " 
    << report.m_initialCandidateResourcesSize);
}

void
NrSensingTransmitHistoryTest::DoRun ()
{
  NS_LOG_FUNCTION (this);

  Ptr<NrUeMac> nrUeMac = CreateObject<NrUeMac> ();
  // Set attributes used by the sensing code
  nrUeMac->SetAttribute ("EnableSensing", BooleanValue (true));
  nrUeMac->SetAttribute ("T1", UintegerValue (2));
  nrUeMac->SetAttribute ("T2", UintegerValue (33));
  nrUeMac->SetAttribute ("ResourcePercentage", UintegerValue (20));
  nrUeMac->SetAttribute ("SlThresPsschRsrp", IntegerValue (-128));
  nrUeMac->TraceConnectWithoutContext("SensingAlgorithm", MakeCallback(&NrSensingTransmitHistoryTest::TraceSensingAlgorithm, this));

  // Time 2.11 seconds similar to cttc-nr-v2x-demo-simple first transmission
  SfnSf currentSfn (211, 0, 0, 2);
  // T_0 is 400 slots behind = 100 ms behind
  SfnSf t0Sfn (201, 0, 0, 2);

  std::list<SensingData> sensingDataList; // empty
  std::list<SfnSf> transmitHistory;
  // CreateNrSlCommResourcePool() above sets resourceReservePeriodList to 100ms 
  // There is a sidelink slot in the selection window at normalized slot
  // 8445.  The below code creates a fake past transmission for 100 ms prior
  // (400 slots prior), at 8045.  The step 5 in the algorithm should remove
  // the candidates at normalized slot 8445 as a result.
  SfnSf pastTransmission (201, 1, 1, 2); // Normalized slot 8045
  transmitHistory.push_back (pastTransmission);
  Time slotPeriod = MicroSeconds (250); // numerology 2
  uint64_t imsi = 0; // parameter only used for logging
  uint8_t bwpId = 0;
  uint16_t poolId = 0;
  uint8_t totalSubCh = 2;  // Two subchannels in the BWP

  // Call the sensing algorithm and inspect the list of slots that result.
  uint16_t lSubch = 1; // Number of subchannels to use
  // The following parameters don't matter here, but are needed for the constructor
  Time packetDelayBudget = MilliSeconds (20);
  Time pRsvpTx = MilliSeconds (100);
  uint16_t cResel = 5;
  uint8_t priority = 0;
  NrSlTransmissionParams params {priority, packetDelayBudget, lSubch, pRsvpTx, cResel};
  std::list <NrSlUeMacSchedSapProvider::NrSlSlotInfo> availableReso;
  availableReso = nrUeMac->GetNrSlCandidateResourcesPrivate (currentSfn, params, CreateNrSlCommResourcePool (totalSubCh), 
    slotPeriod, imsi, bwpId, poolId, totalSubCh, sensingDataList, transmitHistory);

  for (auto it : availableReso)
    {
      NS_TEST_ASSERT_MSG_NE (it.sfn.Normalize (), 8445, "Slot 8445 should be excluded");
    }
}

/**
 * \brief RemoveOldSensingData testcase
 */
class NrSlRemoveOldSensingDataTest : public TestCase
{
public:
  /**
   * \brief Create NrSlRemoveOldSensingDataTest
   * \param name Name of the test
   */
  NrSlRemoveOldSensingDataTest (const std::string &name)
    : TestCase (name) {}

private:
  virtual void DoRun (void) override;
};

void
NrSlRemoveOldSensingDataTest::DoRun ()
{
  NS_LOG_FUNCTION (this);
  auto nrUeMac = CreateObject<NrUeMac> ();
  uint16_t sensingWindow = 100;
  uint64_t imsi = 1;

  // Assume that simulation time is at the time corresponding to frame 203,
  // subframe 0, and slot 0 with numerology 2.
  SfnSf sfnNow {203, 0, 0, 2};
  // If sensed data corresponds to the previous slot, it should be retained.
  SfnSf sfnNowMinusOne {202, 9, 3, 2};
  // Assume a sensing window of 100 slots; T0 should be 100 slots behind
  // the current time.  With numerology 2, each frame is 40 slots, so 100
  // slots in the past is 2.5 frames, or SfnSf (200, 5, 0);
  SfnSf sfnT0 {200, 5, 0, 2};
  // If sensed data corresponds to one slot earlier than T0, it should be
  // trimmed by RemoveOldSensingData ().
  SfnSf sfnT0MinusOne {200, 4, 3, 2};

  // SensingData constructor takes a lot of parameters but only the SfnSf
  // is used in this test.
  std::list<SensingData> sensingData;
  sensingData.emplace_back(SensingData (sfnT0MinusOne, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  NS_TEST_ASSERT_MSG_EQ (sensingData.size (), 1, "sensing data should have one element");
  nrUeMac->RemoveOldSensingData (sfnNow, sensingWindow, sensingData, imsi);
  NS_TEST_ASSERT_MSG_EQ (sensingData.size (), 0, "sensing data should now be empty");
  sensingData.emplace_back(SensingData (sfnT0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  sensingData.emplace_back(SensingData (sfnNowMinusOne, 0, 0, 0, 0, 0, 0, 0, 0, 0));
  NS_TEST_ASSERT_MSG_EQ (sensingData.size (), 2, "sensing data should have two elements");
  nrUeMac->RemoveOldSensingData (sfnNow, sensingWindow, sensingData, imsi);
  NS_TEST_ASSERT_MSG_EQ (sensingData.size (), 2, "sensing data should still have two elements");
}

class NrSensingTestSuite : public TestSuite
{
public:
  NrSensingTestSuite () : TestSuite ("nr-sensing", SYSTEM)
  {
    AddTestCase (new NrSensingTestCase ("Check algorithm iterating steps 4-7 when one subchannel occupied"), QUICK);
    AddTestCase (new NrSensingTransmitHistoryTest ("Check exclusions due to past transmit history"), QUICK);
    AddTestCase (new NrSlRemoveOldSensingDataTest ("Check RemoveOldSensingData"), QUICK);
  }
};

static NrSensingTestSuite nrSensingTestSuite; //!< Nr sensing test suite


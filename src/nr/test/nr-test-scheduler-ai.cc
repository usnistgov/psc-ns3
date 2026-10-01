// Copyright (c) 2024 Seoul National University (SNU)
// Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include <ns3/beam-id.h>
#include <ns3/callback.h>
#include <ns3/node.h>
#include <ns3/nr-control-messages.h>
#include <ns3/nr-eps-bearer.h>
#include <ns3/nr-gnb-mac.h>
#include <ns3/nr-mac-sched-sap.h>
#include <ns3/nr-mac-scheduler-ns3.h>
#include <ns3/nr-mac-scheduler-ofdma-ai.h>
#include <ns3/nr-mac-scheduler-tdma-ai.h>
#include <ns3/nr-mac-scheduler-ue-info-ai.h>
#include <ns3/nr-phy-sap.h>
#include <ns3/object-factory.h>
#include <ns3/test.h>

#include <algorithm>
#include <unordered_set>

/**
 * \file nr-test-scheduler-ai.cc
 * \ingroup test
 *
 * \brief Unit-testing for the scheduler AI. The test checks that the scheduler
 * is created correctly.
 *
 * This unit test is designed to verify the functionality of the callback used for invoking
 * the ns3-gym module during the resource assigning process of an AI scheduler.
 * The test defines a custom callback and checks whether the information passed as
 * arguments matches the information of the User Equipment (UE) and the associated flow installed in
 * each UE. Specifically, the test involves three UEs, each containing flow information
 * corresponding to 5QI values of 1, 3, and 9, respectively. The test ensures that the callback
 * receives the correct flow and UE details, confirming the proper interaction between the AI
 * scheduler and the gym environment.
 */
namespace ns3
{

class TestSchedulerAiPhySapProvider : public NrPhySapProvider
{
  public:
    TestSchedulerAiPhySapProvider();
    ~TestSchedulerAiPhySapProvider() override;
    uint32_t GetSymbolsPerSlot() const override;
    Ptr<const SpectrumModel> GetSpectrumModel() override;
    uint16_t GetBwpId() const override;
    uint16_t GetCellId() const override;
    Time GetSlotPeriod() const override;
    void SendMacPdu(const Ptr<Packet>& p,
                    const SfnSf& sfn,
                    uint8_t symStart,
                    uint16_t rnti) override;
    void SendControlMessage(Ptr<NrControlMessage> msg) override;
    void SendRachPreamble(uint8_t PreambleId, uint8_t Rnti) override;
    void SetSlotAllocInfo(const SlotAllocInfo& slotAllocInfo) override;
    void NotifyConnectionSuccessful() override;
    uint32_t GetRbNum() const override;
    BeamId GetBeamId(uint8_t rnti) const override;
    void SetParams(uint32_t numOfUesPerBeam, uint32_t numOfBeams);

  private:
    uint32_t m_sapNumOfUesPerBeam = 0;
    uint32_t m_sapNumOfBeams = 0;
};

TestSchedulerAiPhySapProvider::TestSchedulerAiPhySapProvider()
{
}

TestSchedulerAiPhySapProvider::~TestSchedulerAiPhySapProvider()
{
}

void
TestSchedulerAiPhySapProvider::SetParams(uint32_t numOfUesPerBeam, uint32_t numOfBeams)
{
    m_sapNumOfUesPerBeam = numOfUesPerBeam;
    m_sapNumOfBeams = numOfBeams;
}

uint32_t
TestSchedulerAiPhySapProvider::GetSymbolsPerSlot() const
{
    // Fixed 14 symbols per slot.
    return 14;
}

Ptr<const SpectrumModel>
TestSchedulerAiPhySapProvider::GetSpectrumModel()
{
    return nullptr;
}

uint16_t
TestSchedulerAiPhySapProvider::GetBwpId() const
{
    return 0;
}

uint16_t
TestSchedulerAiPhySapProvider::GetCellId() const
{
    return 0;
}

Time
TestSchedulerAiPhySapProvider::GetSlotPeriod() const
{
    return MilliSeconds(1);
}

void
TestSchedulerAiPhySapProvider::SendMacPdu(const Ptr<Packet>& p,
                                          const SfnSf& sfn,
                                          uint8_t symStart,
                                          uint16_t rnti)
{
}

void
TestSchedulerAiPhySapProvider::SendControlMessage(Ptr<NrControlMessage> msg)
{
}

void
TestSchedulerAiPhySapProvider::SendRachPreamble(uint8_t PreambleId, uint8_t Rnti)
{
}

void
TestSchedulerAiPhySapProvider::SetSlotAllocInfo(const SlotAllocInfo& slotAllocInfo)
{
}

void
TestSchedulerAiPhySapProvider::NotifyConnectionSuccessful()
{
}

uint32_t
TestSchedulerAiPhySapProvider::GetRbNum() const
{
    // If in the future the scheduler calls this method, remove this assert"
    NS_FATAL_ERROR("GetRbNum should not be called");
    return 53;
}

BeamId
TestSchedulerAiPhySapProvider::GetBeamId(uint8_t rnti) const
{
    BeamId beamId = BeamId(0, 0.0);
    uint8_t rntiCnt = 1;
    for (uint32_t beam = 0; beam < m_sapNumOfBeams; beam++)
    {
        for (uint32_t u = 0; u < m_sapNumOfUesPerBeam; u++)
        {
            if (rnti == 0 || (rntiCnt == rnti && beam == 0))
            {
                beamId = BeamId(0, 0.0);
            }
            else if (rntiCnt == rnti && beam == 1)
            {
                beamId = BeamId(1, 120.0);
            }
            rntiCnt++;
        }
    }
    return beamId;
}

class NrTestSchedulerAiCase : public TestCase
{
  public:
    using FlowProfileList =
        std::list<std::pair<uint8_t, nr::LogicalChannelConfigListElement_s::QosBearerType_e>>;

    NrTestSchedulerAiCase(const std::string& schedulerType)
        : TestCase("NrTestSchedulerAiCase"),
          m_schedulerType(schedulerType)
    {
    }

    ~NrTestSchedulerAiCase() override
    {
    }

    void Notify(const std::vector<NrMacSchedulerUeInfoAi::LcObservation>& observation,
                bool isGameOver,
                float reward,
                const std::string& extraInfo,
                const NrMacSchedulerUeInfoAi::UpdateAllUeWeightsFn& updateWeightsFn);

  private:
    void DoRun() override;
    Ptr<NrMacSchedulerNs3> CreateScheduler(const std::string& schedulerType) const;
    Ptr<NrGnbMac> CreateMac(Ptr<NrMacSchedulerNs3>& scheduler,
                            NrMacCschedSapProvider::CschedCellConfigReqParameters& params) const;
    bool m_verbose = false;
    std::string m_schedulerType;
    TestSchedulerAiPhySapProvider* m_phySapProvider;
    const std::unordered_map<uint8_t, NrEpsBearer> m_epsBearerMap = {
        {1, static_cast<NrEpsBearer::Qci>(1)},
        {2, static_cast<NrEpsBearer::Qci>(3)},
        {3, static_cast<NrEpsBearer::Qci>(9)}};
};

void
NrTestSchedulerAiCase::Notify(const std::vector<NrMacSchedulerUeInfoAi::LcObservation>& observation,
                              bool isGameOver,
                              float reward,
                              const std::string& extraInfo,
                              const NrMacSchedulerUeInfoAi::UpdateAllUeWeightsFn& updateWeightsFn)
{
    NS_TEST_ASSERT_MSG_EQ(observation.size(),
                          m_epsBearerMap.size(),
                          "Observation size should be equal to the flow profile size");
    if (m_verbose)
    {
        std::cout << "Notify called" << std::endl;
        std::cout << "isGameOver: " << isGameOver << std::endl;
        std::cout << "reward: " << reward << std::endl;
        std::cout << "extraInfo: " << extraInfo << std::endl;
        std::cout << "observation size: " << observation.size() << std::endl;
    }
    NrMacSchedulerUeInfoAi::UeWeightsMap ueWeightsMap;
    for (auto& obs : observation)
    {
        if (m_verbose)
        {
            std::cout << "rnti: " << obs.rnti << " qci: " << obs.qci << " lcId: " << obs.lcId
                      << " priority: " << obs.priority << " holDelay: " << obs.holDelay
                      << std::endl;
        }

        const auto& it = m_epsBearerMap.find(obs.rnti);
        if (it == m_epsBearerMap.end())
        {
            NS_FATAL_ERROR("RNTI not found");
        }
        NS_TEST_ASSERT_MSG_EQ(obs.lcId, 1, "LC ID should be 1");
        NS_TEST_ASSERT_MSG_EQ(it->first, obs.rnti, "RNTI should be equal");
        NS_TEST_ASSERT_MSG_EQ(it->second.qci, obs.qci, "QCI should be equal");
        NS_TEST_ASSERT_MSG_EQ(it->second.GetPriority(), obs.priority, "Priority should be equal");
        NS_TEST_ASSERT_MSG_EQ(it->second.GetPacketDelayBudgetMs(),
                              obs.holDelay,
                              "Packet delay budget should be equal");
        ueWeightsMap[obs.rnti] = NrMacSchedulerUeInfoAi::Weights{{obs.lcId, 1.0}};
    }
    NS_TEST_ASSERT_MSG_EQ(isGameOver, false, "Game should not be over");
    NS_TEST_ASSERT_MSG_EQ(reward, 0.0, "Reward should be 0.0");
    updateWeightsFn(ueWeightsMap);
}

Ptr<NrMacSchedulerNs3>
NrTestSchedulerAiCase::CreateScheduler(const std::string& schedulerType) const
{
    ObjectFactory schedFactory;
    schedFactory.SetTypeId(schedulerType);

    Ptr<NrMacSchedulerNs3> sched = DynamicCast<NrMacSchedulerNs3>(schedFactory.Create());
    NS_ABORT_MSG_IF(sched == nullptr,
                    "Can't create a NrMacSchedulerNs3 from type " + schedulerType);

    return sched;
}

Ptr<NrGnbMac>
NrTestSchedulerAiCase::CreateMac(
    Ptr<NrMacSchedulerNs3>& scheduler,
    NrMacCschedSapProvider::CschedCellConfigReqParameters& params) const
{
    Ptr<NrGnbMac> mac = CreateObject<NrGnbMac>();

    mac->SetNrMacSchedSapProvider(scheduler->GetMacSchedSapProvider());
    mac->SetNrMacCschedSapProvider(scheduler->GetMacCschedSapProvider());
    scheduler->SetMacSchedSapUser(mac->GetNrMacSchedSapUser());
    scheduler->SetMacCschedSapUser(mac->GetNrMacCschedSapUser());
    // Config sched
    scheduler->DoCschedCellConfigReq(params);

    return mac;
}

void
NrTestSchedulerAiCase::DoRun()
{
    NrMacCschedSapProvider::CschedCellConfigReqParameters params;
    // 53 RBs for 10 MHz bandwidth
    params.m_ulBandwidth = 53;
    params.m_dlBandwidth = 53;

    auto sched = CreateScheduler(m_schedulerType);
    auto mac = CreateMac(sched, params);

    m_phySapProvider = new TestSchedulerAiPhySapProvider();
    m_phySapProvider->SetParams(m_epsBearerMap.size(), 1);

    mac->SetPhySapProvider(m_phySapProvider);

    Ptr<NrAmc> amc = CreateObject<NrAmc>();
    sched->InstallDlAmc(amc);

    uint8_t rnti;
    NrEpsBearer bearer;
    for (auto& pair : m_epsBearerMap)
    {
        rnti = pair.first;
        bearer = pair.second;

        NrMacCschedSapProvider::CschedUeConfigReqParameters paramsUe;
        paramsUe.m_rnti = rnti;
        paramsUe.m_beamId = m_phySapProvider->GetBeamId(rnti);

        if (m_verbose)
        {
            std::cout << " rnti: " << paramsUe.m_rnti << " beam Id: " << paramsUe.m_beamId
                      << " scheduler: " << m_schedulerType << std::endl;
        }
        // Add Users
        sched->DoCschedUeConfigReq(paramsUe); // Repeat for the number of UEs

        // Create LC
        NrMacCschedSapProvider::CschedLcConfigReqParameters paramsLc;
        paramsLc.m_rnti = rnti;
        paramsLc.m_reconfigureFlag = false;

        nr::LogicalChannelConfigListElement_s lc;
        lc.m_logicalChannelIdentity = 1;
        lc.m_logicalChannelGroup = 2;
        lc.m_direction = nr::LogicalChannelConfigListElement_s::DIR_DL;
        lc.m_qosBearerType = static_cast<nr::LogicalChannelConfigListElement_s::QosBearerType_e>(
            bearer.GetResourceType());
        lc.m_qci = bearer.qci;
        paramsLc.m_logicalChannelConfigList.emplace_back(lc);

        sched->DoCschedLcConfigReq(paramsLc);

        // Update queue
        NrMacSchedSapProvider::SchedDlRlcBufferReqParameters paramsDlRlc;
        paramsDlRlc.m_rnti = rnti;
        paramsDlRlc.m_logicalChannelIdentity = 1;
        paramsDlRlc.m_rlcRetransmissionHolDelay = 0;
        paramsDlRlc.m_rlcRetransmissionQueueSize = 0;
        paramsDlRlc.m_rlcStatusPduSize = 0;
        paramsDlRlc.m_rlcTransmissionQueueHolDelay = bearer.GetPacketDelayBudgetMs();
        paramsDlRlc.m_rlcTransmissionQueueSize = 1284;

        sched->DoSchedDlRlcBufferReq(paramsDlRlc);
    }

    if (m_schedulerType.find("Ofdma") != std::string::npos)
    {
        Ptr<NrMacSchedulerOfdmaAi> schedAi = DynamicCast<NrMacSchedulerOfdmaAi>(sched);
        schedAi->SetNotifyCbDl(
            MakeCallback(&NrTestSchedulerAiCase::Notify, this)); // Set the notify function
    }

    if (m_schedulerType.find("Tdma") != std::string::npos)
    {
        Ptr<NrMacSchedulerTdmaAi> schedAi = DynamicCast<NrMacSchedulerTdmaAi>(sched);
        schedAi->SetNotifyCbDl(
            MakeCallback(&NrTestSchedulerAiCase::Notify, this)); // Set the notify function
    }

    // Get Active UEs
    Ptr<NrMacSchedulerTdma> schedTdma = DynamicCast<NrMacSchedulerTdma>(sched);
    NrMacSchedulerNs3::ActiveUeMap activeUe;
    schedTdma->ComputeActiveUe(&activeUe,
                               &NrMacSchedulerUeInfo::GetDlLCG,
                               &NrMacSchedulerUeInfo::GetDlHarqVector,
                               "DL");
    std::vector<NrMacSchedulerNs3::UePtrAndBufferReq> ueVector =
        schedTdma->GetUeVectorFromActiveUeMap(activeUe);
    // Call Notify
    schedTdma->CallNotifyDlFn(ueVector);

    delete m_phySapProvider;
}

class NrTestSchedulerAiSuite : public TestSuite
{
  public:
    NrTestSchedulerAiSuite()
        : TestSuite("nr-test-scheduler-ai", Type::UNIT)
    {
        std::list<std::string> subdivision = {"Tdma", "Ofdma"};
        std::string type = {"Ai"};
        for (const auto& subType : subdivision)
        {
            std::stringstream schedName;
            schedName << "ns3::NrMacScheduler" << subType << type;

            AddTestCase(new NrTestSchedulerAiCase(schedName.str()), Duration::QUICK);
        }
    }
};

static NrTestSchedulerAiSuite nrTestSchedulerAiSuite; ///< RL-based scheduler test suite

} // namespace ns3

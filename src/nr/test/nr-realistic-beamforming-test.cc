// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include <ns3/antenna-module.h>
#include <ns3/core-module.h>
#include <ns3/mobility-module.h>
#include <ns3/nr-module.h>
#include <ns3/spectrum-model.h>
#include <ns3/three-gpp-channel-model.h>
#include <ns3/three-gpp-propagation-loss-model.h>
#include <ns3/three-gpp-spectrum-propagation-loss-model.h>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrRealisticBeamformingTest");

/**
 * \ingroup test
 * \file nr-realistic-beamforming-test.cc
 *
 * \brief This test tests how different levels of received SINR SRS
 * affect the realistic beamforming algorithm performance. What is expected
 * is that when SINR is high that realistic beamforming algorithm will
 * select the same beamforming vector pair as it would ideal beamforming
 * algorithm that has the perfect knowledge of the channel.
 * On the other hand, when SINR is low it is expected that the error in
 * estimation of the channel is high, thus the selected beamforming pair
 * is expected to be different from those that are selected by the ideal
 * beamforming algorithm.
 * Note that as the ideal and realistic beamforming algorithms are not exactly
 * the same, i.e., ideal beamforming algorithm assumes perfect knowledge
 * of the full channel (including long-term component of the fading,
 * the Doppler, and frequency-selectivity) while realistic beamforming
 * algorithm only estimates the long-term component of the fading.
 * Hence, then slight variations on the best beam selection may appear.
 */

class NrRealisticBeamformingTestSuite : public TestSuite
{
  public:
    NrRealisticBeamformingTestSuite();
};

class NrRealisticBeamformingTestCase : public TestCase
{
  public:
    NrRealisticBeamformingTestCase(std::string name, Duration duration);
    ~NrRealisticBeamformingTestCase() override;

  private:
    void DoRun() override;

    Duration m_testDuration; //!< the test execution mode type
};

/**
 * TestSuite
 */
NrRealisticBeamformingTestSuite::NrRealisticBeamformingTestSuite()
    : TestSuite("nr-realistic-beamforming-test", Type::SYSTEM)
{
    NS_LOG_INFO("Creating NrRealisticBeamformingTestSuite");

    auto durationQuick = Duration::QUICK;
    auto durationExtensive = Duration::EXTENSIVE;

    AddTestCase(
        new NrRealisticBeamformingTestCase("RealisticBeamforming basic test case", durationQuick),
        durationQuick);
    AddTestCase(new NrRealisticBeamformingTestCase("RealisticBeamforming basic test case",
                                                   durationExtensive),
                durationExtensive);
}

/**
 * TestCase
 */

NrRealisticBeamformingTestCase::NrRealisticBeamformingTestCase(std::string name, Duration duration)
    : TestCase(name)
{
    m_testDuration = duration;
}

NrRealisticBeamformingTestCase::~NrRealisticBeamformingTestCase()
{
}

void
NrRealisticBeamformingTestCase::DoRun()
{
    RngSeedManager::SetSeed(1);

    std::list<Vector> uePositionsExtensive = {Vector(10, -10, 1.5),
                                              Vector(0, 10, 1.5),
                                              Vector(0, -10, 1.5)};

    uint16_t totalCounter = 0;
    uint16_t highSinrCounter = 0;
    uint16_t lowSinrCounter = 0;

    std::list<uint16_t> rngList = (m_testDuration == Duration::EXTENSIVE)
                                      ? std::list<uint16_t>({2, 3})
                                      : std::list<uint16_t>({1});

    std::list<Vector> uePositions =
        (m_testDuration == Duration::EXTENSIVE)
            ? uePositionsExtensive
            : std::list<Vector>({Vector(10, 10, 1.5), Vector(-10, 10, 1.5)});

    std::list<uint16_t> antennaConfList = (m_testDuration == Duration::EXTENSIVE)
                                              ? std::list<uint16_t>({3, 4})
                                              : std::list<uint16_t>({2});

    for (auto rng : rngList)
    {
        RngSeedManager::SetRun(rng);

        for (const auto& pos : uePositions)
        {
            for (const auto& antennaConf : antennaConfList)
            {
                for (auto iso : {false, true})
                {
                    totalCounter++;

                    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
                    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();
                    channelHelper->ConfigureFactories("UMa", "LOS");
                    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));
                    // Create Nodes: eNodeB and UE
                    NodeContainer gnbNodes;
                    NodeContainer ueNodes;
                    gnbNodes.Create(1);
                    ueNodes.Create(1);
                    NodeContainer allNodes = NodeContainer(gnbNodes, ueNodes);

                    // Install Mobility Model
                    Ptr<ListPositionAllocator> positionAlloc =
                        CreateObject<ListPositionAllocator>();
                    positionAlloc->Add(Vector(0, 0.0, 10)); // gNB
                    positionAlloc->Add(pos);                // UE

                    MobilityHelper mobility;
                    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
                    mobility.SetPositionAllocator(positionAlloc);
                    mobility.Install(allNodes);

                    // Create Devices and install them in the Nodes (gNB and UE)
                    NetDeviceContainer gnbDevs;
                    NetDeviceContainer ueDevs;

                    CcBwpCreator::SimpleOperationBandConf bandConf(29e9, 100e6, 1);
                    CcBwpCreator ccBwpCreator;
                    OperationBandInfo band = ccBwpCreator.CreateOperationBandContiguousCc(bandConf);
                    // Initialize channel and pathloss, plus other things inside band.
                    channelHelper->AssignChannelsToBands({band});

                    BandwidthPartInfoPtrVector allBwps = CcBwpCreator::GetAllBwps({band});

                    // Antennas for the gNbs
                    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(antennaConf));
                    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(antennaConf));

                    // Antennas for the UEs
                    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(antennaConf));
                    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(antennaConf));

                    // Antenna element type for both gNB and UE
                    if (iso)
                    {
                        nrHelper->SetGnbAntennaAttribute(
                            "AntennaElement",
                            PointerValue(CreateObject<IsotropicAntennaModel>()));
                        nrHelper->SetUeAntennaAttribute(
                            "AntennaElement",
                            PointerValue(CreateObject<IsotropicAntennaModel>()));
                    }
                    else
                    {
                        nrHelper->SetGnbAntennaAttribute(
                            "AntennaElement",
                            PointerValue(CreateObject<ThreeGppAntennaModel>()));
                        nrHelper->SetUeAntennaAttribute(
                            "AntennaElement",
                            PointerValue(CreateObject<ThreeGppAntennaModel>()));
                    }

                    nrHelper->SetGnbBeamManagerTypeId(RealisticBfManager::GetTypeId());

                    gnbDevs = nrHelper->InstallGnbDevice(gnbNodes, allBwps);
                    ueDevs = nrHelper->InstallUeDevice(ueNodes, allBwps);

                    // This test requires that the NrGnbNetDevice::ConfigureCell()
                    // is called before GetBeamformingVectors() is called below.
                    // Usually NrHelper::AttachToGnb() or NrGnbNetDevice::DoInitialize()
                    // takes care of this, but in this test we need to manually
                    // call it here.
                    for (auto it = gnbDevs.Begin(); it != gnbDevs.End(); ++it)
                    {
                        DynamicCast<NrGnbNetDevice>(*it)->ConfigureCell();
                    }

                    Ptr<NrUePhy> uePhy = nrHelper->GetUePhy(ueDevs.Get(0), 0);

                    Ptr<NrSpectrumPhy> txSpectrumPhy =
                        nrHelper->GetGnbPhy(gnbDevs.Get(0), 0)->GetSpectrumPhy();
                    Ptr<SpectrumChannel> txSpectrumChannel = txSpectrumPhy->GetSpectrumChannel();
                    Ptr<ThreeGppPropagationLossModel> propagationLossModel =
                        DynamicCast<ThreeGppPropagationLossModel>(
                            txSpectrumChannel->GetPropagationLossModel());
                    NS_ASSERT(propagationLossModel != nullptr);
                    propagationLossModel->AssignStreams(1);
                    Ptr<ChannelConditionModel> channelConditionModel =
                        propagationLossModel->GetChannelConditionModel();
                    channelConditionModel->AssignStreams(1);
                    Ptr<ThreeGppSpectrumPropagationLossModel> spectrumLossModel =
                        DynamicCast<ThreeGppSpectrumPropagationLossModel>(
                            txSpectrumChannel->GetPhasedArraySpectrumPropagationLossModel());
                    NS_ASSERT(spectrumLossModel != nullptr);
                    Ptr<ThreeGppChannelModel> channel =
                        DynamicCast<ThreeGppChannelModel>(spectrumLossModel->GetChannelModel());
                    channel->AssignStreams(1);

                    double sinrSrsHighLineal = std::pow(10.0, 0.1 * 40);
                    double sinrSrsLowLineal = std::pow(10.0, 0.1 * (-10));

                    Ptr<CellScanBeamforming> cellScanBeamforming =
                        CreateObject<CellScanBeamforming>();

                    BeamformingVectorPair bfPairIdeal =
                        cellScanBeamforming->GetBeamformingVectors(txSpectrumPhy,
                                                                   uePhy->GetSpectrumPhy());

                    Ptr<RealisticBeamformingAlgorithm> realisticBeamforming =
                        CreateObject<RealisticBeamformingAlgorithm>();
                    realisticBeamforming->Install(
                        txSpectrumPhy,
                        uePhy->GetSpectrumPhy(),
                        DynamicCast<NrGnbNetDevice>(gnbDevs.Get(0))->GetScheduler(0));

                    // directly update max SINR SRS to a high value, skipping other set functions of
                    // the algorithm
                    realisticBeamforming->m_maxSrsSinrPerSlot = sinrSrsHighLineal;

                    BeamformingVectorPair bfPairReal1 =
                        realisticBeamforming->GetBeamformingVectors();

                    // directly update max SINR SRS to a new lower value, skipping other set
                    // functions of the algorithm,
                    realisticBeamforming->m_maxSrsSinrPerSlot = sinrSrsLowLineal;

                    BeamformingVectorPair bfPairReal2 =
                        realisticBeamforming->GetBeamformingVectors();

                    if ((bfPairIdeal.first.second == bfPairReal1.first.second) &&
                        (bfPairIdeal.second.second == bfPairReal1.second.second))
                    {
                        highSinrCounter++;
                    }

                    if (!((bfPairIdeal.first.second == bfPairReal2.first.second) &&
                          (bfPairIdeal.second.second == bfPairReal2.second.second)))
                    {
                        lowSinrCounter++;
                    }
                }
            }
        }
    }

    double tolerance = 0.2;
    if (m_testDuration == Duration::EXTENSIVE)
    {
        tolerance = 0.2;
    }
    else
    {
        tolerance =
            0.3; // relax tolerance for QUICK mode since there are only 4 test configurations, e.g.,
                 // if 3 results of 4 are as expected that is already enough, but that gives 0.75
                 // thus it needs larger tolerance than 0.2 which is fine for EXTENSIVE mode
    }

    NS_TEST_ASSERT_MSG_EQ_TOL(highSinrCounter / (double)totalCounter,
                              1,
                              tolerance,
                              "The pair of beamforming vectors should be equal in most of the "
                              "cases when SINR is high, and they are not");
    NS_TEST_ASSERT_MSG_EQ_TOL(lowSinrCounter / (double)totalCounter,
                              1,
                              tolerance,
                              "The pair of beamforming vectors should not be equal in most of the "
                              "cases when SINR is low, and they are");

    NS_LOG_INFO("The result is as expected when high SINR in " << highSinrCounter << " out of "
                                                               << totalCounter << " total cases.");
    NS_LOG_INFO("The result is as expected when low SINR in " << lowSinrCounter << " out of "
                                                              << totalCounter << " total cases.");

    Simulator::Destroy();
}

// Do not forget to allocate an instance of this TestSuite
static NrRealisticBeamformingTestSuite nrTestSuite;

} // namespace ns3

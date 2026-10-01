//
// SPDX-License-Identifier: NIST-Software
//

/**
 * \file nr-sl-ideal-mcs-controller-test.cc
 * \ingroup test
 *
 * This test setup is similar to that of nr-sl-ideal-mcs-controller-example.cc (two nodes,
 * sending two packets at a particular distance, checking the resulting MCS for a given
 * fading error model).
 *
 * The test combinations can be cross-checked against the nr-sl-ideal-mcs-controller-example.cc
 * For example:
 *       testCase = new NrSlIdealMcsControllerTest("static-1000-0-0.1 yields MCS 5", "static", 1000,
 * 0, 0.1, 5); can be verified against running:
 *       ./ns3 run nr-sl-ideal-mcs-controller-example -- --errorModel='static' --distance=1000
 * --ns3::NrSlIdealMcsController::Margin=0 --ns3::NrSlIdealMcsController::TargetBler=0.1
 */

#include <ns3/abort.h>
#include <ns3/address.h>
#include <ns3/application-container.h>
#include <ns3/application-helper.h>
#include <ns3/assert.h>
#include <ns3/boolean.h>
#include <ns3/config.h>
#include <ns3/constant-position-mobility-model.h>
#include <ns3/data-rate.h>
#include <ns3/double.h>
#include <ns3/inet-socket-address.h>
#include <ns3/internet-stack-helper.h>
#include <ns3/ipv4-address-helper.h>
#include <ns3/ipv4-address.h>
#include <ns3/log.h>
#include <ns3/net-device.h>
#include <ns3/node-container.h>
#include <ns3/node.h>
#include <ns3/nr-sl-epa-error-model.h>
#include <ns3/nr-sl-helper.h>
#include <ns3/nr-sl-ideal-mcs-controller.h>
#include <ns3/nr-sl-phy-mac-common.h>
#include <ns3/nr-sl-static-error-model.h>
#include <ns3/nr-sl-tft.h>
#include <ns3/nr-sl-trace-helper.h>
#include <ns3/nr-sl-ue-mac.h>
#include <ns3/nstime.h>
#include <ns3/on-off-helper.h>
#include <ns3/packet-sink-helper.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/random-variable-stream.h>
#include <ns3/simulator.h>
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/test.h>
#include <ns3/uinteger.h>
#include <ns3/vector.h>

#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace ns3
{

namespace test
{

NS_LOG_COMPONENT_DEFINE("NrSlIdealMcsControllerTest");

/**
 * @ingroup tests
 * Test case for NrSlIdealMcsController testing
 */
class NrSlIdealMcsControllerTest : public TestCase
{
  public:
    /**
     * Constructor
     *
     * @param name Name of the test
     * @param errorModelType error model type
     * @param distance distance in meters
     * @param margin margin in dB
     * @param targetBler target BLER
     * @param expectedMcs the expected MCS value
     */
    NrSlIdealMcsControllerTest(const std::string& name,
                               const std::string& errorModelType,
                               double distance,
                               double margin,
                               double targetBler,
                               uint8_t expectedMcs)
        : TestCase(name),
          m_errorModelType(errorModelType),
          m_distance(distance),
          m_margin(margin),
          m_targetBler(targetBler),
          m_expectedMcs(expectedMcs)
    {
    }

  protected:
    void DoRun(void) override;

  private:
    /**
     * @param params structure of values used in the PSCCH
     * Trace sink for SlPscchScheduling trace source
     */
    void SlPscchScheduling(SlPscchUeMacStatParameters params);

    /**
     * @param clientDevice sending device
     * @param serverDevice receiving device
     * @param destination IPv4 address of destination
     * @param port UDP port value
     * @param slInfo the sidelink info for the bearer
     * @param maxPackets maximum number of packets
     * @param packetSize packet size in bytes
     * @param interval interval between packets
     * @param nrSlHelper
     * @param startTime start time for application
     * @param finalSimTime final simulation time (also stop time for applications
     */
    std::tuple<ApplicationContainer, ApplicationContainer> CreateTraffic(
        Ptr<NetDevice> clientDevice,
        Ptr<NetDevice> serverDevice,
        Ipv4Address destination,
        uint16_t port,
        SidelinkInfo slInfo,
        uint32_t maxPackets,
        uint32_t packetSize,
        Time interval,
        Ptr<NrSlHelper> nrSlHelper,
        Time startTime,
        Time finalSimTime);

    std::vector<std::pair<Time, uint8_t>> m_pscchTransmissions; //!< record of PSCCH
    std::string m_errorModelType;                               //!< error model type to use
    double m_distance;                                          //!< distance in m
    double m_margin;                                            //!< margin in dB
    double m_targetBler;                                        //!< target BLE
    uint8_t m_expectedMcs;                                      //!< expected MCS
};

void
NrSlIdealMcsControllerTest::SlPscchScheduling(SlPscchUeMacStatParameters params)
{
    NS_LOG_INFO("MCS announced in SCI-1: " << +params.mcs);
    m_pscchTransmissions.push_back(std::make_pair(Now(), params.mcs));
}

std::tuple<ApplicationContainer, ApplicationContainer>
NrSlIdealMcsControllerTest::CreateTraffic(Ptr<NetDevice> clientDevice,
                                          Ptr<NetDevice> serverDevice,
                                          Ipv4Address destination,
                                          uint16_t port,
                                          SidelinkInfo slInfo,
                                          uint32_t maxPackets,
                                          uint32_t packetSize,
                                          Time interval,
                                          Ptr<NrSlHelper> nrSlHelper,
                                          Time startTime,
                                          Time finalSimTime)
{
    Ptr<NrSlTft> txTft;
    Ptr<NrSlTft> rxTft;
    Address remoteAddress = InetSocketAddress(destination, port);
    txTft = Create<NrSlTft>(NrSlTft::BearerType::TRANSMIT, destination, port, slInfo);
    rxTft = Create<NrSlTft>(NrSlTft::BearerType::RECEIVE, destination, port, slInfo);
    nrSlHelper->ActivateNrSlBearer(startTime, clientDevice, txTft);
    nrSlHelper->ActivateNrSlBearer(startTime, serverDevice, rxTft);
    OnOffHelper sidelinkClient("ns3::UdpSocketFactory", remoteAddress);
    sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(false));
    sidelinkClient.SetAttribute("MaxBytes", UintegerValue(maxPackets * packetSize));
    // In the future (ns-3.44 or greater) the below two lines can be replaced
    // by OnOffHelper::SetConstantInterval()
    DataRate dataRate{static_cast<uint64_t>(packetSize * 8 / interval.GetSeconds())};
    sidelinkClient.SetConstantRate(dataRate, packetSize);
    ApplicationContainer clientApps = sidelinkClient.Install(clientDevice->GetNode());
    clientApps.Start(startTime);
    clientApps.Stop(finalSimTime);
    ApplicationContainer serverApps;
    Address localAddress = InetSocketAddress(Ipv4Address::GetAny(), port);
    PacketSinkHelper sidelinkSink("ns3::UdpSocketFactory", localAddress);
    sidelinkSink.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(false));
    serverApps = sidelinkSink.Install(serverDevice->GetNode());
    serverApps.Start(startTime);
    return std::make_tuple(clientApps, serverApps);
}

void
NrSlIdealMcsControllerTest::DoRun()
{
    Config::SetDefault("ns3::NrSlIdealMcsController::Margin", DoubleValue(m_margin));
    Config::SetDefault("ns3::NrSlIdealMcsController::TargetBler", DoubleValue(m_targetBler));
    // Traffic parameters
    uint32_t packetSize = 50; // bytes. Keep below 57 bytes to avoid RLC segmentation;
                              // account for 20 (IPv4) + 8 (UDP) + 2 (RLC) + 5 (SCI-2A)
                              // = 35 bytes overhead.
    uint16_t maxPackets = 2;

    // The application should start at time 10 seconds or later to allow the routing to converge
    Time trafficStartTime(Seconds(2)); // Time to start the traffic in the application layer
                                       // Actual start time will be at 3 sec

    // NR parameters
    uint16_t numerologyBwpSl(0);          // The numerology to be used in sidelink bandwidth part
    double centralFrequencyBandSl(793e6); // band n14 (793 MHz)
    double bandwidthBandSl(10e6);         // Units of Hz
    double txPower(23);                   // Units of dBm

    int64_t randomStreamIndex(1000);
    int64_t randomStreamIncrement(1000);
    [[maybe_unused]] int64_t streamsUsed(0);

    // Check if the frequency is in the allowed range.
    NS_ABORT_IF(centralFrequencyBandSl > 6e9);

    // Simulation timeline parameters
    Time simTime(trafficStartTime + Seconds(maxPackets + 1));

    // Create channel before nodes
    auto channel = CreateObject<SingleModelSpectrumChannel>();
    auto lossModel = CreateObject<LogDistancePropagationLossModel>();
    // Urban Microcell (UMi) LOS model from WINNER+ B1 model, also in NIST LLS
    // The NIST LLS code is:
    // 40 * log10(d_m) + 7.56 - 17.3 * log10(hBS_e) - 17.3 * log10(hMS_e) + 2.7 * log10(f_GHz);
    // We can represent this by the ns-3 LogDistance model by setting the exponent to 4
    // and by setting the ReferenceLoss attribute to the other terms.  The heights are
    // 1.5m.
    //
    // Using this loss model and the static error model:
    // - the distance of 1200m can be used to send at MCS 0
    // - the distance of 800m can be used to send at MCS 14
    // - the distance of 400m can be used to send at MCS 28
    //
    double referenceLoss =
        7.56 - 2 * 17.3 * std::log10(1.5) + 2.7 * std::log10(centralFrequencyBandSl / 1e9);
    lossModel->GetObject<LogDistancePropagationLossModel>()->SetAttribute("Exponent",
                                                                          DoubleValue(4));
    lossModel->GetObject<LogDistancePropagationLossModel>()->SetAttribute(
        "ReferenceLoss",
        DoubleValue(referenceLoss));
    channel->AddPropagationLossModel(lossModel);

    // Node and mobility configuration
    NodeContainer nodes;
    auto node = CreateObject<Node>();
    auto mm = CreateObject<ConstantPositionMobilityModel>();
    mm->SetPosition(Vector(0, 0, 0));
    node->AggregateObject(mm);
    nodes.Add(node);
    node = CreateObject<Node>();
    mm = CreateObject<ConstantPositionMobilityModel>();
    mm->SetPosition(Vector(m_distance, 0, 0));
    node->AggregateObject(mm);
    nodes.Add(node);

    // Configure NR SL
    auto nrHelper = CreateObject<NrSlHelper>();
    if (m_errorModelType == "static")
    {
        nrHelper->SetSlErrorModelTypeId(NrSlStaticErrorModel::GetTypeId());
    }
    else if (m_errorModelType == "epa")
    {
        nrHelper->SetSlErrorModelTypeId(NrSlEpaErrorModel::GetTypeId());
    }

    // Configure any attributes for the UE SL configuration and then install to the nodes
    nrHelper->SetUePhyAttribute("TxPower", DoubleValue(txPower));
    nrHelper->SetUeMacAttribute("EnableSensing", BooleanValue(true));
    nrHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(0));
    nrHelper->SetMcsControllerTypeId(NrSlIdealMcsController::GetTypeId());

    // Override helper method defaults
    nrHelper->SetAttribute("CentralFrequency", DoubleValue(centralFrequencyBandSl));
    nrHelper->SetAttribute("Bandwidth", DoubleValue(bandwidthBandSl));
    nrHelper->SetAttribute("Numerology", UintegerValue(numerologyBwpSl));
    // Add NR SL devices to the nodes in the MANET container
    auto netDevices = nrHelper->ConfigureSlNetwork(nodes, channel);

    // Configure internet
    InternetStackHelper internet;
    internet.Install(nodes);
    streamsUsed = internet.AssignStreams(nodes, randomStreamIndex);
    randomStreamIndex += randomStreamIncrement;

    // Assign IP address for the UEs
    Ipv4AddressHelper addrHelper;
    addrHelper.SetBase("7.0.0.0", "255.0.0.0");
    auto ipIfaces = addrHelper.Assign(netDevices);

    SidelinkInfo slInfo;
    slInfo.m_harqEnabled = false;
    slInfo.m_pdb = MilliSeconds(20);
    slInfo.m_dynamic = true;
    slInfo.m_dstL2Id = 2;
    slInfo.m_castType = SidelinkInfo::CastType::Unicast;
    slInfo.m_priority = 1;           // this is unused
    slInfo.m_rri = MilliSeconds(20); // this is unused
    uint16_t port = 8000;
    Time trafficInterval = Seconds(1);
    auto [clientApps, serverApps] = CreateTraffic(netDevices.Get(0),
                                                  netDevices.Get(1),
                                                  ipIfaces.GetAddress(1),
                                                  port,
                                                  slInfo,
                                                  maxPackets,
                                                  packetSize,
                                                  trafficInterval,
                                                  nrHelper,
                                                  trafficStartTime,
                                                  simTime);

    // Trace the use of the MCS change by the transmitting NrSlUeMac
    NrSlTraceHelper traceHelper;
    auto mac = traceHelper.GetNrSlUeMac(nodes.Get(0));
    mac->TraceConnectWithoutContext(
        "SlPscchScheduling",
        MakeCallback(&NrSlIdealMcsControllerTest::SlPscchScheduling, this));

    /******************* Set random variable stream numbers **************************/
    randomStreamIndex += randomStreamIncrement;
    streamsUsed = nrHelper->AssignStreams(netDevices, randomStreamIndex);
    randomStreamIndex += randomStreamIncrement;
    streamsUsed += ApplicationHelper::AssignStreamsToAllApps(nodes, randomStreamIndex);
    /******************* End set random variable stream numbers **********************/

    Simulator::Stop(simTime + TimeStep(1));
    Simulator::Run();

    NS_TEST_ASSERT_MSG_EQ(m_pscchTransmissions.empty(), false, "Expected at least one SCI-1");
    const auto& [scheduledTime, mcs] = m_pscchTransmissions.back();
    NS_TEST_ASSERT_MSG_EQ(+mcs,
                          +m_expectedMcs,
                          "Expected MCS " << +m_expectedMcs << " but got " << +mcs
                                          << " for distance " << m_distance << " error model "
                                          << m_errorModelType << " margin " << m_margin
                                          << " target BLER " << m_targetBler);

    Simulator::Destroy();
}

/**
 * @ingroup test
 * @ingroup tests
 * TestSuite for NrSlIdealMcsController test cases
 */
class NrSlIdealMcsControllerTestSuite : public TestSuite
{
  public:
    /**
     * Constructor
     */
    NrSlIdealMcsControllerTestSuite()
        : TestSuite("nr-sl-ideal-mcs-controller", Type::UNIT)
    {
        NrSlIdealMcsControllerTest* testCase;

        // Parameters:  "test name", errorModelType, distance, margin, target BLER, expected MCS
        testCase = new NrSlIdealMcsControllerTest("static-1000-0-0.1 yields MCS 5",
                                                  "static",
                                                  1000,
                                                  0,
                                                  0.1,
                                                  5);
        AddTestCase(testCase);
        testCase = new NrSlIdealMcsControllerTest("static-800-0-0.1 yields MCS 10",
                                                  "static",
                                                  800,
                                                  0,
                                                  0.1,
                                                  10);
        AddTestCase(testCase);
        testCase = new NrSlIdealMcsControllerTest("static-800-1.5-0.1 yields MCS 8",
                                                  "static",
                                                  800,
                                                  1.5,
                                                  0.1,
                                                  8);
        AddTestCase(testCase);
        testCase = new NrSlIdealMcsControllerTest("static-800-(-1)-0.1 yields MCS 11",
                                                  "static",
                                                  800,
                                                  -1,
                                                  0.1,
                                                  11);
        AddTestCase(testCase);
        testCase = new NrSlIdealMcsControllerTest("static-800-0-0.01 yields MCS 9",
                                                  "static",
                                                  800,
                                                  0,
                                                  0.01,
                                                  9);
        AddTestCase(testCase);
        testCase = new NrSlIdealMcsControllerTest("static-800-0-0.0001 yields MCS 8",
                                                  "static",
                                                  800,
                                                  0,
                                                  0.0001,
                                                  8);
        AddTestCase(testCase);
        testCase = new NrSlIdealMcsControllerTest("static-100-0-0.1 yields MCS 28",
                                                  "static",
                                                  100,
                                                  0,
                                                  0.1,
                                                  28);
        AddTestCase(testCase);
        testCase = new NrSlIdealMcsControllerTest("static-10000-0-0.1 yields MCS 0",
                                                  "static",
                                                  10000,
                                                  0,
                                                  0.1,
                                                  0);
        AddTestCase(testCase);
        testCase =
            new NrSlIdealMcsControllerTest("epa-10000-0-0.1 yields MCS 0", "epa", 10000, 0, 0.1, 0);
        AddTestCase(testCase);
        testCase =
            new NrSlIdealMcsControllerTest("epa-100-0-0.1 yields MCS 28", "epa", 100, 0, 0.1, 28);
        AddTestCase(testCase);
        testCase =
            new NrSlIdealMcsControllerTest("epa-200-0-0.1 yields MCS 27", "epa", 200, 0, 0.1, 27);
        AddTestCase(testCase);
        testCase =
            new NrSlIdealMcsControllerTest("epa-200-2-0.1 yields MCS 25", "epa", 200, 2, 0.1, 25);
        AddTestCase(testCase);
        testCase =
            new NrSlIdealMcsControllerTest("epa-200-0-0.01 yields MCS 18", "epa", 200, 0, 0.01, 18);
        AddTestCase(testCase);
    }
};

static NrSlIdealMcsControllerTestSuite
    g_testNrSlIdealMcsControllerSuite; //!< ideal MCS controller test suite

} // namespace test
} // namespace ns3

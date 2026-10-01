//
// SPDX-License-Identifier: NIST-Software
//

/*
 * \ingroup examples
 * \file nr-prose-discovery-l3-relay-selection.cc
 * \brief An example of integrating NR ProSe relay discovery and NR ProSe
 * relay unicast communication
 *
 * Channel configuration:
 * This example sets up a NR sidelink out-of-coverage simulation using the
 * default propagation and channel models configured by the NrHelper (which
 * default to the 38.901 UMa pathloss model and 37.885 channel condition model)
 *
 * System configuration:
 * Sidelink will use one operational band, containing one component carrier,
 * and two bandwidth parts. One bandwidth part is used for in-network
 * communication, i.e., UL and DL between in-network relay UEs and gNBs,
 * and the other bandwidth part is used for SL communication between UEs using SL.
 *
 * Topology:
 * This scenario is composed of one gNB and a number of UEs (ueNum).
 * The first UEs (relayNum) act as in-network L3 UE-to-Network relay UEs
 * (which are attached to the gNB ). The rest of the UEs (ueNum-relayNum)
 * act as out-of-network remote UEs.
 * All UEs are randomly deployed and will start performing NR discovery
 * (randomly between discStartMin and discStartMax) using either Model A or B
 * (specified in discModel).
 * Once a relay is discovered, the relay selection algorithm (relaySelectAlgorithm:
 * FirstAvailableRelay|RandomRelay|MaxRsrpRelay) is initiated and the unicast link
 * between the remote UEs and their chosen relay is establishing.
 * If, previously, a different relay has been selected, that connection is released
 * before estblishing the direct link with the newly selected relay.
 *
 * Traffic:
 * There are two CBR traffic flows (UL and DL) with the same configuration
 * for each out-of-network UEs (acting as remote UEs) to be served
 * when they connect to the available U2N relay UE.
 * Traffic starts at trafficStart.
 *
 * Outputs:
 * 1/ NrSlPc5SignallingPacketTrace.txt: log of the transmitted and received PC5
 * signaling messages used for the establishment of each ProSe unicast direct
 * link.
 * 2/ NrSlRelayNasRxPacketTrace.txt: log of the packets received and routed by
 * the NAS of the UE acting as L3 UE-to-Network UE.
 * 3/ NrSlRelayDiscoveryTrace.txt: to keep track of discovered relays
 * 4/ NrSlRelaySelectionTrace.txt: to keep track of relay selection attempts
 *
 */

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/nr-prose-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/stats-module.h"

#include <iomanip>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrProseDiscoveryL3RelaySelection");

/*
 * \brief Trace sink function for logging transmission and reception of PC5
 *        signaling (PC5-S) messages
 *
 * \param stream the output stream wrapper where the trace will be written
 * \param node the pointer to the UE node
 * \param srcL2Id the L2 ID of the UE sending the PC5-S packet
 * \param dstL2Id the L2 ID of the UE receiving the PC5-S packet
 * \param isTx flag that indicates if the UE is transmitting the PC5-S packet
 * \param p the PC5-S packet
 */
void
TraceSinkPC5SignallingPacketTrace(Ptr<OutputStreamWrapper> stream,
                                  uint32_t srcL2Id,
                                  uint32_t dstL2Id,
                                  bool isTx,
                                  Ptr<Packet> p)
{
    NrSlPc5SignallingMessageType pc5smt;
    p->PeekHeader(pc5smt);
    *stream->GetStream() << Simulator::Now().GetSeconds();
    if (isTx)
    {
        *stream->GetStream() << "\t"
                             << "TX";
    }
    else
    {
        *stream->GetStream() << "\t"
                             << "RX";
    }
    *stream->GetStream() << "\t" << srcL2Id << "\t" << dstL2Id << "\t" << pc5smt.GetMessageName();
    *stream->GetStream() << std::endl;
}

std::map<std::string, uint32_t> g_relayNasPacketCounter;

/*
 * \brief Trace sink function for logging reception of data packets in the NAS
 *        layer by UE(s) acting as relay UE
 *
 * \param stream the output stream wrapper where the trace will be written
 * \param nodeIp the IP of the relay UE
 * \param srcIp the IP of the node sending the packet
 * \param dstIp the IP of the node that would be receiving the packet
 * \param srcLink the link from which the relay UE received the packet (UL, DL, or SL)
 * \param dstLink the link towards which the relay routed the packet (UL, DL, or SL)
 * \param p the packet
 */
void
TraceSinkRelayNasRxPacketTrace(Ptr<OutputStreamWrapper> stream,
                               Ipv4Address nodeIp,
                               Ipv4Address srcIp,
                               Ipv4Address dstIp,
                               std::string srcLink,
                               std::string dstLink,
                               Ptr<Packet> p)
{
    *stream->GetStream() << Simulator::Now().GetSeconds() << "\t" << nodeIp << "\t" << srcIp << "\t"
                         << dstIp << "\t" << srcLink << "\t" << dstLink << std::endl;
    std::ostringstream oss;
    oss << nodeIp << "      " << srcIp << "->" << dstIp << "      " << srcLink << "->" << dstLink;
    std::string mapKey = oss.str();
    auto it = g_relayNasPacketCounter.find(mapKey);
    if (it == g_relayNasPacketCounter.end())
    {
        g_relayNasPacketCounter.insert(std::pair<std::string, uint32_t>(mapKey, 1));
    }
    else
    {
        it->second += 1;
    }
}

int
main(int argc, char* argv[])
{
    // Common configuration
    uint8_t numBands = 1;
    double centralFrequencyHz = 5.89e9; // band n47 (From SL examples)
    double bandwidth = 40e6;            // 40 MHz
    double centralFrequencyCc0 = 5.89e9;
    double bandwidthCc0 = bandwidth;
    std::string pattern = "DL|DL|DL|F|UL|UL|UL|UL|UL|UL|"; // From SL examples
    double bandwidthCc0Bpw0 = bandwidthCc0 / 2;
    double bandwidthCc0Bpw1 = bandwidthCc0 / 2;

    // In-network devices configuration
    uint16_t gNbNum = 1;
    double gNbHeight = 10;
    double ueHeight = 1.5;
    uint16_t numerologyCc0Bwp0 = 1;    //                  BWP0 will be used for the in-network
    double gNBtotalTxPower = 46.0;     // dBm
    bool cellScan = false;             // Beamforming method.
    double beamSearchAngleStep = 10.0; // Beamforming parameter.

    // Sidelink configuration
    uint16_t numerologyCc0Bwp1 = 1; //(From SL examples)  BWP1 will be used for SL

    // Topology parameters
    uint16_t ueNum = 3;      // Number of SL UEs in the simulation
    uint16_t relayNum = 2;   // Number of Relay UEs
    double ueTxPower = 23.0; // Tx Power for UEs

    // Simulation timeline parameters
    Time simTime = Seconds(15.0); // Total simulation time

    // NR Discovery
    Time discInterval = Seconds(2);   // interval between two discovery announcements
    double discStartMin = 2;          // minimum of discovery start in seconds
    double discStartMax = 4;          // maximum of discovery start in seconds
    std::string discModel = "ModelB"; // discovery model
    std::string relaySelectAlgorithm(
        "MaxRsrpRelay"); // relay selection algorithm: FirstAvailableRelay/RandomRelay/MaxRsrpRelay
    Time t5087 = Seconds(5); // duration of Timer T5087 (Prose Direct Link Release Request
                             // Retransmission): 5s is the dafault value

    // Applications configuration
    uint32_t packetSizeDlUl = 500; // bytes
    uint32_t lambdaDlUl = 60;      // packets per second
    uint32_t trafficStart = 4;     // traffis start time in seconds

    bool writeTraces = true; // flag to control writing traces

    CommandLine cmd;
    cmd.AddValue("ueNum", "Number of UEs in the simulation", ueNum);
    cmd.AddValue("relayNum", "Number of relay UEs in the simulation", relayNum);
    cmd.AddValue("simTime", "Simulation time in seconds", simTime);
    cmd.AddValue("discStartMin", "Lower bound of discovery start time in seconds", discStartMin);
    cmd.AddValue("discStartMax", "Upper bound of discovery start time in seconds", discStartMax);
    cmd.AddValue("discInterval",
                 "Interval between two Prose discovery announcements",
                 discInterval);
    cmd.AddValue("discModel",
                 "Discovery model (ModelA for announcements and ModelB for requests/responses)",
                 discModel);
    cmd.AddValue("relaySelectAlgorithm",
                 "The Relay UE (re)selection algorithm the Remote UEs will use "
                 "(FirstAvailableRelay|RandomRelay|MaxRsrpRelay)",
                 relaySelectAlgorithm);
    cmd.AddValue("t5087",
                 "The duration of Timer T5087 (Prose Direct Link Release Request Retransmission)",
                 t5087);
    cmd.AddValue("trafficStart", "the start time of remote traffic in seconds", trafficStart);
    cmd.AddValue("writeTraces", "Flag to control the writing of output traces", writeTraces);

    // Parse the command line
    cmd.Parse(argc, argv);

    NS_ABORT_IF(numBands < 1);

    // Number of relay and remote UEs
    uint16_t remoteNum = ueNum - relayNum;
    std::cout << "UEs configuration: " << std::endl;
    std::cout << " Number of Relay UEs = " << relayNum << std::endl
              << " Number of Remote UEs = " << remoteNum << std::endl;

    // Check if the frequency is in the allowed range.
    NS_ABORT_IF(centralFrequencyHz > 6e9);

    // Setup large enough buffer size to avoid overflow
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    // Set the TxPower for UEs
    Config::SetDefault("ns3::NrUePhy::TxPower", DoubleValue(ueTxPower));

    // Discovery Interval
    Config::SetDefault("ns3::NrSlUeProse::DiscoveryInterval", TimeValue(discInterval));
    // T5087 timer for retransmission of failed Prose Direct Link Release Request
    Config::SetDefault("ns3::NrSlUeProseDirectLink::T5087", TimeValue(t5087));

    // Create gNBs and in-network UEs, configure positions
    NodeContainer gNbNodes;
    NodeContainer inNetUeNodes;
    MobilityHelper mobilityG;

    gNbNodes.Create(gNbNum);

    Ptr<ListPositionAllocator> gNbPositionAlloc = CreateObject<ListPositionAllocator>();
    int32_t yValue = 0.0;

    for (uint32_t i = 1; i <= gNbNodes.GetN(); ++i)
    {
        if (i % 2 != 0)
        {
            yValue = static_cast<int>(i) * 30;
        }
        else
        {
            yValue = -yValue;
        }

        gNbPositionAlloc->Add(Vector(0.0, yValue, gNbHeight));
    }

    mobilityG.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityG.SetPositionAllocator(gNbPositionAlloc);
    mobilityG.Install(gNbNodes);

    // Create UE nodes and define their mobility
    NodeContainer relayUeNodes;
    relayUeNodes.Create(relayNum);
    NodeContainer remoteUeNodes;
    remoteUeNodes.Create(remoteNum);

    /*
     * Fix the random streams
     */
    int64_t stream = 1;
    const uint64_t streamIncrement = 1000;
    // Use the first few stream numbers for node position
    Ptr<UniformRandomVariable> m_uniformRandomVariablePositionX =
        CreateObject<UniformRandomVariable>();
    m_uniformRandomVariablePositionX->SetStream(stream++);
    Ptr<UniformRandomVariable> m_uniformRandomVariablePositionY =
        CreateObject<UniformRandomVariable>();
    m_uniformRandomVariablePositionY->SetStream(stream++);

    MobilityHelper mobilityRemotes;
    mobilityRemotes.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                     "Bounds",
                                     RectangleValue(Rectangle(3000, 3200, 0, 100)));
    Ptr<ListPositionAllocator> positionAllocRemotes = CreateObject<ListPositionAllocator>();

    for (uint16_t i = 0; i < remoteNum; i++)
    {
        double x = m_uniformRandomVariablePositionX->GetValue(3050, 3150);
        double y = m_uniformRandomVariablePositionY->GetValue(25, 75);
        positionAllocRemotes->Add(Vector(x, y, ueHeight));
    }
    mobilityRemotes.SetPositionAllocator(positionAllocRemotes);
    mobilityRemotes.Install(remoteUeNodes);
    stream += streamIncrement;
    for (uint32_t i = 0; i < remoteUeNodes.GetN(); i++)
    {
        remoteUeNodes.Get(i)->GetObject<RandomWalk2dMobilityModel>()->AssignStreams(stream);
    }

    MobilityHelper mobilityRelays;
    mobilityRelays.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                    "Bounds",
                                    RectangleValue(Rectangle(2800, 3000, 0, 100)));
    Ptr<ListPositionAllocator> positionAllocRelays = CreateObject<ListPositionAllocator>();
    for (uint16_t i = 0; i < relayNum; i++)
    {
        double x = m_uniformRandomVariablePositionX->GetValue(2850, 2950);
        double y = m_uniformRandomVariablePositionY->GetValue(25, 75);
        positionAllocRelays->Add(Vector(x, y, ueHeight));
    }
    mobilityRelays.SetPositionAllocator(positionAllocRelays);
    mobilityRelays.Install(relayUeNodes);
    stream += streamIncrement;
    for (uint32_t i = 0; i < relayUeNodes.GetN(); i++)
    {
        relayUeNodes.Get(i)->GetObject<RandomWalk2dMobilityModel>()->AssignStreams(stream);
    }

    for (uint32_t i = 0; i < gNbNodes.GetN(); ++i)
    {
        auto gNbMobility = gNbNodes.Get(i)->GetObject<MobilityModel>();
        for (uint32_t j = 0; j < relayUeNodes.GetN(); j++)
        {
            std::cout << "Relay " << j << " initial distance from gNB " << i << " is "
                      << relayUeNodes.Get(j)->GetObject<MobilityModel>()->GetDistanceFrom(
                             gNbMobility)
                      << std::endl;
        }
    }

    for (uint32_t i = 0; i < relayUeNodes.GetN(); ++i)
    {
        auto relayUeMobility = relayUeNodes.Get(i)->GetObject<MobilityModel>();
        for (uint32_t j = 0; j < remoteUeNodes.GetN(); j++)
        {
            std::cout << "Remote UE " << j << " initial distance from relay " << i << " is "
                      << remoteUeNodes.Get(j)->GetObject<MobilityModel>()->GetDistanceFrom(
                             relayUeMobility)
                      << std::endl;
        }
    }

    // Store UEs positions
    std::ofstream myfile;
    myfile.open("NrSlDiscoveryTopology.txt");

    myfile << "  X Y Z\n";
    uint32_t gnb = 1;
    for (NodeContainer::Iterator j = gNbNodes.Begin(); j != gNbNodes.End(); ++j)
    {
        Ptr<Node> object = *j;
        Ptr<MobilityModel> position = object->GetObject<MobilityModel>();
        NS_ASSERT_MSG(position, "Mobility model not found");
        Vector pos = position->GetPosition();
        myfile << "gNB " << gnb << " " << pos.x << " " << pos.y << " " << pos.z << "\n";
        gnb++;
    }
    uint32_t ue = 1;
    for (NodeContainer::Iterator j = relayUeNodes.Begin(); j != relayUeNodes.End(); ++j)
    {
        Ptr<Node> object = *j;
        Ptr<MobilityModel> position = object->GetObject<MobilityModel>();
        NS_ASSERT_MSG(position, "Mobility model not found");
        Vector pos = position->GetPosition();
        myfile << "UE " << ue << " " << pos.x << " " << pos.y << " " << pos.z << "\n";
        ue++;
    }
    for (NodeContainer::Iterator j = remoteUeNodes.Begin(); j != remoteUeNodes.End(); ++j)
    {
        Ptr<Node> object = *j;
        Ptr<MobilityModel> position = object->GetObject<MobilityModel>();
        NS_ASSERT_MSG(position, "Mobility model not found");
        Vector pos = position->GetPosition();
        myfile << "UE " << ue << " " << pos.x << " " << pos.y << " " << pos.z << "\n";
        ue++;
    }

    myfile.close();

    /*
     * Setup the NR module. We create the various helpers needed for the
     * NR simulation:
     * - EpcHelper, which will setup the core network entities
     * - NrHelper, which takes care of creating and connecting the various
     * part of the NR stack
     */
    Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    nrHelper->SetEpcHelper(epcHelper);
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();
    channelHelper->ConfigureFactories("V2V-Highway", "Default", "ThreeGpp");

    /*************************Spectrum division ****************************/

    BandwidthPartInfoPtrVector allBwps;

    OperationBandInfo band;

    /*
     * The configured spectrum division is:
     * |-------------- Band ------------|
     * |---------------CC0--------------|
     * |------BWP0------|------BWP1-----|
     */

    std::unique_ptr<ComponentCarrierInfo> cc0(new ComponentCarrierInfo());
    std::unique_ptr<BandwidthPartInfo> bwp0(new BandwidthPartInfo());
    std::unique_ptr<BandwidthPartInfo> bwp1(new BandwidthPartInfo());

    band.m_centralFrequency = centralFrequencyHz;
    band.m_channelBandwidth = bandwidth;
    band.m_lowerFrequency = band.m_centralFrequency - band.m_channelBandwidth / 2;
    band.m_higherFrequency = band.m_centralFrequency + band.m_channelBandwidth / 2;

    // Component Carrier 0
    cc0->m_ccId = 0;
    cc0->m_centralFrequency = centralFrequencyCc0;
    cc0->m_channelBandwidth = bandwidthCc0;
    cc0->m_lowerFrequency = cc0->m_centralFrequency - cc0->m_channelBandwidth / 2;
    cc0->m_higherFrequency = cc0->m_centralFrequency + cc0->m_channelBandwidth / 2;

    // BWP 0
    bwp0->m_bwpId = 0;
    bwp0->m_centralFrequency = cc0->m_lowerFrequency + cc0->m_channelBandwidth / 4;
    bwp0->m_channelBandwidth = bandwidthCc0Bpw0;
    bwp0->m_lowerFrequency = bwp0->m_centralFrequency - bwp0->m_channelBandwidth / 2;
    bwp0->m_higherFrequency = bwp0->m_centralFrequency + bwp0->m_channelBandwidth / 2;

    cc0->AddBwp(std::move(bwp0));

    // BWP 1
    bwp1->m_bwpId = 1;
    bwp1->m_centralFrequency = cc0->m_higherFrequency - cc0->m_channelBandwidth / 4;
    bwp1->m_channelBandwidth = bandwidthCc0Bpw1;
    bwp1->m_lowerFrequency = bwp1->m_centralFrequency - bwp1->m_channelBandwidth / 2;
    bwp1->m_higherFrequency = bwp1->m_centralFrequency + bwp1->m_channelBandwidth / 2;

    cc0->AddBwp(std::move(bwp1));

    // Add CC to the corresponding operation band.
    band.AddCc(std::move(cc0));

    /********************* END Spectrum division ****************************/

    channelHelper->AssignChannelsToBands({band});

    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));
    epcHelper->SetAttribute("S1uLinkDelay", TimeValue(MilliSeconds(0)));

    // Set gNB scheduler
    nrHelper->SetSchedulerTypeId(TypeId::LookupByName("ns3::NrMacSchedulerTdmaRR"));

    // gNB Beamforming method
    if (cellScan)
    {
        idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                             TypeIdValue(CellScanBeamforming::GetTypeId()));
        idealBeamformingHelper->SetBeamformingAlgorithmAttribute("BeamSearchAngleStep",
                                                                 DoubleValue(beamSearchAngleStep));
    }
    else
    {
        idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                             TypeIdValue(DirectPathBeamforming::GetTypeId()));
    }

    allBwps = CcBwpCreator::GetAllBwps({band});

    // Antennas for all the UEs
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(1));    // From SL examples
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(1)); // From SL examples
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));

    // Antennas for all the gNbs
    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(4));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(8));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     PointerValue(CreateObject<IsotropicAntennaModel>()));

    // gNB bandwidth part manager setup.
    // The current algorithm multiplexes BWPs depending on the associated bearer QCI
    nrHelper->SetGnbBwpManagerAlgorithmAttribute(
        "GBR_CONV_VOICE",
        UintegerValue(0)); // The BWP index is 0 because only one BWP will be installed in the eNB

    // Install only in the BWP that will be used for in-network
    uint8_t bwpIdInNet = 0;
    BandwidthPartInfoPtrVector inNetBwp;
    inNetBwp.insert(inNetBwp.end(), band.GetBwpAt(/*CC*/ 0, bwpIdInNet));
    NetDeviceContainer inNetUeNetDev = nrHelper->InstallUeDevice(inNetUeNodes, inNetBwp);
    NetDeviceContainer gnbNetDev = nrHelper->InstallGnbDevice(gNbNodes, inNetBwp);

    // SL BWP manager configuration
    uint8_t bwpIdSl = 1;
    nrHelper->SetBwpManagerTypeId(TypeId::LookupByName("ns3::NrSlBwpManagerUe"));
    nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_MC_PUSH_TO_TALK", UintegerValue(bwpIdSl));

    nrHelper->SetUeRrcTypeId(NrSlUeRrc::GetTypeId());
    nrHelper->SetEpcUeNasTypeId(NrSlEpcUeNas::GetTypeId());
    // For relays, we need a special configuration with one Bwp configured
    // with a Mac of type NrUeMac, and one Bwp configured with a Mac of type
    // NrSlUeMac.  Similarly, we need one Phy of NrUePhy and one of NrSlUePhy.
    // Use a variation of InstallUeDevice to configure that, and
    // pass in a vector of object factories to account for the different Macs
    std::vector<ObjectFactory> nrUeMacFactories;
    std::vector<ObjectFactory> nrUePhyFactories;
    ObjectFactory nrUeMacFactory;
    ObjectFactory nrUePhyFactory;
    nrUeMacFactory.SetTypeId(NrUeMac::GetTypeId());
    nrUeMacFactories.emplace_back(nrUeMacFactory);
    nrUePhyFactory.SetTypeId(NrUePhy::GetTypeId());
    nrUePhyFactories.emplace_back(nrUePhyFactory);
    ObjectFactory nrSlUeMacFactory;
    ObjectFactory nrSlUePhyFactory;
    nrSlUeMacFactory.SetTypeId(NrSlUeMac::GetTypeId());
    nrSlUeMacFactory.Set("EnableSensing", BooleanValue(false));
    nrSlUeMacFactory.Set("T1", UintegerValue(2));
    nrSlUeMacFactory.Set("ActivePoolId", UintegerValue(0));
    nrSlUeMacFactory.Set("NumHarqProcess", UintegerValue(255));
    nrSlUeMacFactory.Set("SlThresPsschRsrp", IntegerValue(-128));
    nrUeMacFactories.emplace_back(nrSlUeMacFactory);
    nrSlUePhyFactory.SetTypeId(NrSlUePhy::GetTypeId());
    nrUePhyFactories.emplace_back(nrSlUePhyFactory);

    // Install both BWPs on U2N relays
    NetDeviceContainer relayUeNetDev =
        nrHelper->InstallUeDevice(relayUeNodes, allBwps, nrUeMacFactories, nrUePhyFactories);

    std::set<uint8_t> slBwpIdContainer;
    slBwpIdContainer.insert(bwpIdInNet);
    slBwpIdContainer.insert(bwpIdSl);

    // Setup BWPs numerology, Tx Power and pattern
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)
        ->SetAttribute("Numerology", UintegerValue(numerologyCc0Bwp0));
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)->SetAttribute("Pattern", StringValue(pattern));
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)->SetAttribute("TxPower", DoubleValue(gNBtotalTxPower));

    /*Create NrSlHelper which will configure the UEs protocol stack to be ready to
     *perform Sidelink related procedures
     */
    Ptr<NrSlHelper> nrSlHelper = CreateObject<NrSlHelper>();
    // This is needed to add the UE's IMSI to the EpcPgwApp
    nrSlHelper->SetEpcHelper(epcHelper);
    nrSlHelper->SetUeBwpManagerAlgorithmAttribute("GBR_MC_PUSH_TO_TALK", UintegerValue(bwpIdSl));

    // SL UE MAC configuration (for non relay UEs)
    nrSlHelper->SetUeMacAttribute("EnableSensing", BooleanValue(false));
    nrSlHelper->SetUeMacAttribute("T1", UintegerValue(2));
    nrSlHelper->SetUeMacAttribute("ActivePoolId", UintegerValue(0));
    nrSlHelper->SetUeMacAttribute("NumHarqProcess", UintegerValue(255));
    nrSlHelper->SetUeMacAttribute("SlThresPsschRsrp", IntegerValue(-128));

    // Install both BWPs on remote UEs
    // This was needed to avoid errors with bwpId and vector indexes during device installation

    NetDeviceContainer remoteUeNetDev = nrSlHelper->InstallUeDevice(remoteUeNodes, allBwps);

    // The sidelink fast fading error model is installed in the NrSlSpectrumPhy
    // and needs to be configured separately from the above loss model.
    // The default is a static channel, but the NrSlEpaErrorModel may also be
    // configured for a fading channel
    nrSlHelper->SetSlErrorModelTypeId(NrSlStaticErrorModel::GetTypeId());

    /*
     * Set the SL scheduler attributes
     * In this example we use NrSlUeMacSchedulerDefault scheduler, which uses
     * fixed MCS value and schedules logical channels
     * by priority order first and then SPS followed by dynamic grants.
     */
    nrSlHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrSlHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(14));

    // Configure U2N relay UEs for SL
    std::set<uint8_t> slBwpIdContainerRelay;
    slBwpIdContainerRelay.insert(bwpIdSl); // Only in the SL BWP for the relay UEs
    nrSlHelper->PrepareUeForSidelink(relayUeNetDev, slBwpIdContainerRelay);

    // Configure SL-only UEs for SL
    nrSlHelper->PrepareUeForSidelink(remoteUeNetDev, slBwpIdContainer);

    /*
     * Start preparing for all the sub Structs/RRC Information Element (IEs)
     * of NrSlRrcSap::SidelinkPreconfigNr. This is the main structure, which would
     * hold all the pre-configuration related to Sidelink.
     */

    // SlResourcePoolNr IE
    NrSlRrcSap::SlResourcePoolNr slResourcePoolNr;
    // get it from pool factory
    Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory>();
    // Configure specific parameters of interest:
    std::vector<std::bitset<1>> slBitmap = {1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1};
    ptrFactory->SetSlTimeResources(slBitmap);
    ptrFactory->SetSlSensingWindow(100); // T0 in ms
    ptrFactory->SetSlSelectionWindow(5);
    ptrFactory->SetSlFreqResourcePscch(10); // PSCCH RBs
    ptrFactory->SetSlSubchannelSize(10);
    ptrFactory->SetSlMaxNumPerReserve(3);
    // Once parameters are configured, we can create the pool
    NrSlRrcSap::SlResourcePoolNr pool = ptrFactory->CreatePool();
    slResourcePoolNr = pool;

    // Configure the SlResourcePoolConfigNr IE, which hold a pool and its id
    NrSlRrcSap::SlResourcePoolConfigNr slresoPoolConfigNr;
    slresoPoolConfigNr.haveSlResourcePoolConfigNr = true;
    // Pool id, ranges from 0 to 15
    uint16_t poolId = 0;
    NrSlRrcSap::SlResourcePoolIdNr slResourcePoolIdNr;
    slResourcePoolIdNr.id = poolId;
    slresoPoolConfigNr.slResourcePoolId = slResourcePoolIdNr;
    slresoPoolConfigNr.slResourcePool = slResourcePoolNr;

    // Configure the SlBwpPoolConfigCommonNr IE, which hold an array of pools
    NrSlRrcSap::SlBwpPoolConfigCommonNr slBwpPoolConfigCommonNr;
    // Array for pools, we insert the pool in the array as per its poolId
    slBwpPoolConfigCommonNr.slTxPoolSelectedNormal[slResourcePoolIdNr.id] = slresoPoolConfigNr;

    // Configure the BWP IE
    NrSlRrcSap::Bwp bwp;
    bwp.numerology = numerologyCc0Bwp1;
    bwp.symbolsPerSlots = 14;
    bwp.rbPerRbg = 1;
    bwp.bandwidth =
        bandwidthCc0Bpw1 / 1000 / 100; // SL configuration requires BW in Multiple of 100 KHz

    // Configure the SlBwpGeneric IE
    NrSlRrcSap::SlBwpGeneric slBwpGeneric;
    slBwpGeneric.bwp = bwp;
    slBwpGeneric.slLengthSymbols = NrSlRrcSap::GetSlLengthSymbolsEnum(14);
    slBwpGeneric.slStartSymbol = NrSlRrcSap::GetSlStartSymbolEnum(0);

    // Configure the SlBwpConfigCommonNr IE
    NrSlRrcSap::SlBwpConfigCommonNr slBwpConfigCommonNr;
    slBwpConfigCommonNr.haveSlBwpGeneric = true;
    slBwpConfigCommonNr.slBwpGeneric = slBwpGeneric;
    slBwpConfigCommonNr.haveSlBwpPoolConfigCommonNr = true;
    slBwpConfigCommonNr.slBwpPoolConfigCommonNr = slBwpPoolConfigCommonNr;

    // Configure the SlFreqConfigCommonNr IE, which holds the array to store
    // the configuration of all Sidelink BWP (s).
    NrSlRrcSap::SlFreqConfigCommonNr slFreConfigCommonNr;
    // Array for BWPs. Here we will iterate over the BWPs, which
    // we want to use for SL.
    for (const auto& it : slBwpIdContainer)
    {
        // it is the BWP id
        slFreConfigCommonNr.slBwpList[it] = slBwpConfigCommonNr;
    }

    // Configure the TddUlDlConfigCommon IE
    NrSlRrcSap::TddUlDlConfigCommon tddUlDlConfigCommon;
    tddUlDlConfigCommon.tddPattern = pattern;

    // Configure the SlPreconfigGeneralNr IE
    NrSlRrcSap::SlPreconfigGeneralNr slPreconfigGeneralNr;
    slPreconfigGeneralNr.slTddConfig = tddUlDlConfigCommon;

    // Configure the SlUeSelectedConfig IE
    NrSlRrcSap::SlUeSelectedConfig slUeSelectedPreConfig;
    slUeSelectedPreConfig.slProbResourceKeep = 0;
    // Configure the SlPsschTxParameters IE
    NrSlRrcSap::SlPsschTxParameters psschParams;
    psschParams.slMaxTxTransNumPssch = 1;
    // Configure the SlPsschTxConfigList IE
    NrSlRrcSap::SlPsschTxConfigList pscchTxConfigList;
    pscchTxConfigList.slPsschTxParameters[0] = psschParams;
    slUeSelectedPreConfig.slPsschTxConfigList = pscchTxConfigList;

    /*
     * Finally, configure the SidelinkPreconfigNr This is the main structure
     * that needs to be communicated to NrSlUeRrc class
     */
    NrSlRrcSap::SidelinkPreconfigNr slPreConfigNr;
    slPreConfigNr.slPreconfigGeneral = slPreconfigGeneralNr;
    slPreConfigNr.slUeSelectedPreConfig = slUeSelectedPreConfig;
    slPreConfigNr.slPreconfigFreqInfoList[0] = slFreConfigCommonNr;

    // Communicate the above pre-configuration to the NrSlHelper
    // For SL-only UEs
    nrSlHelper->InstallNrSlPreConfiguration(remoteUeNetDev, slPreConfigNr);

    // For U2N relay UEs
    // We need to modify some parameters to configure *only* BWP1 on the relay for
    //  SL and avoid MAC problems
    NrSlRrcSap::SlFreqConfigCommonNr slFreConfigCommonNrRelay;
    slFreConfigCommonNrRelay.slBwpList[bwpIdSl] = slBwpConfigCommonNr;

    NrSlRrcSap::SidelinkPreconfigNr slPreConfigNrRelay;
    slPreConfigNrRelay.slPreconfigGeneral = slPreconfigGeneralNr;
    slPreConfigNrRelay.slUeSelectedPreConfig = slUeSelectedPreConfig;
    slPreConfigNrRelay.slPreconfigFreqInfoList[0] = slFreConfigCommonNrRelay;

    nrSlHelper->InstallNrSlPreConfiguration(relayUeNetDev, slPreConfigNrRelay);

    // For L3 U2N relay (re)selection criteria
    // NrSlRrcSap::SlRelayUeConfig slRelayConfig;
    NrSlRrcSap::SlRemoteUeConfig slRemoteConfig;
    slRemoteConfig.slReselectionConfig.slRsrpThres = -110;
    slRemoteConfig.slReselectionConfig.slFilterCoefficientRsrp = 0.5;
    slRemoteConfig.slReselectionConfig.slHystMin = 10;

    NrSlRrcSap::SlDiscConfigCommon slDiscConfigCommon;
    // slDiscConfigCommon.slRelayUeConfigCommon = slRelayConfig;
    slDiscConfigCommon.slRemoteUeConfigCommon = slRemoteConfig;

    /****************************** End SL Configuration ***********************/

    stream += streamIncrement;
    nrHelper->AssignStreams(gnbNetDev, stream);
    stream += streamIncrement;
    nrHelper->AssignStreams(relayUeNetDev, stream);
    stream += streamIncrement;
    nrSlHelper->AssignStreams(relayUeNetDev, stream);
    stream += streamIncrement;
    nrHelper->AssignStreams(remoteUeNetDev, stream);
    stream += streamIncrement;
    nrSlHelper->AssignStreams(remoteUeNetDev, stream);

    // create the internet and install the IP stack on the UEs
    // get SGW/PGW and create a single RemoteHost
    Ptr<Node> pgw = epcHelper->GetPgwNode();
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);
    InternetStackHelper internet;
    internet.Install(remoteHostContainer);

    // connect a remoteHost to pgw. Setup routing too
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(DataRate("100Gb/s")));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(2500));
    p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.000)));
    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);
    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress(1);

    std::cout << "IP configuration: " << std::endl;
    std::cout << " Remote Host: " << remoteHostAddr << std::endl;

    // Configure in-network U2N relay UEs
    internet.Install(relayUeNodes);
    Ipv4InterfaceContainer ueIpIfaceRelay;
    ueIpIfaceRelay = epcHelper->AssignUeIpv4Address(NetDeviceContainer(relayUeNetDev));
    std::vector<Ipv4Address> relayIpv4AddressVector(relayNum);

    for (uint32_t u = 0; u < relayUeNodes.GetN(); ++u)
    {
        // Set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(relayUeNodes.Get(u)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

        // Obtain local IPv4 addresses that will be used to route the unicast traffic upon setup of
        // the direct link
        relayIpv4AddressVector[u] =
            relayUeNodes.Get(u)->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal();
        std::cout << " In-network U2N relay UE: " << relayIpv4AddressVector[u] << std::endl;
    }

    // Attach relay UEs to the closest gNB
    nrHelper->AttachToClosestGnb(relayUeNetDev, gnbNetDev);

    // Configure out-of-network remote UEs
    internet.Install(remoteUeNodes);
    Ipv4InterfaceContainer ueIpIfaceRemote;
    ueIpIfaceRemote = epcHelper->AssignUeIpv4Address(NetDeviceContainer(remoteUeNetDev));
    std::vector<Ipv4Address> remoteIpv4AddressVector(remoteNum);

    for (uint32_t u = 0; u < remoteUeNodes.GetN(); ++u)
    {
        // Set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(remoteUeNodes.Get(u)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

        // Obtain local IPv4 addresses that will be used to route the unicast traffic upon setup of
        // the direct link
        remoteIpv4AddressVector[u] =
            remoteUeNodes.Get(u)->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal();
        std::cout << " Out-of-network remote UE: " << remoteIpv4AddressVector[u] << std::endl;
    }

    // Create ProSe helper
    Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject<NrSlProseHelper>();
    nrSlProseHelper->SetEpcHelper(epcHelper);

    // Install ProSe layer and corresponding SAPs in the UES
    nrSlProseHelper->PrepareUesForProse(relayUeNetDev);
    nrSlProseHelper->PrepareUesForProse(remoteUeNetDev);

    // Configure ProSe Unicast parameters. At the moment it only instruct the MAC
    // layer (and PHY therefore) to monitor packets directed the UE's own Layer 2 ID
    nrSlProseHelper->PrepareUesForUnicast(relayUeNetDev);
    nrSlProseHelper->PrepareUesForUnicast(remoteUeNetDev);

    nrSlProseHelper->InstallNrSlDiscoveryConfiguration(relayUeNetDev,
                                                       remoteUeNetDev,
                                                       slDiscConfigCommon);

    // Configure the value of timer Timer T5080 (Prose Direct Link Establishment Request
    // Retransmission) to a lower value than the standard (8.0 s) to speed connection in shorter
    // simulation time
    Config::SetDefault("ns3::NrSlUeProseDirectLink::T5080", TimeValue(Seconds(2.0)));

    /*
     * Setup discovery applications
     */
    NS_LOG_INFO("Configuring discovery relay");

    // Relay Discovery Model
    NrSlUeProse::DiscoveryModel model;
    if (discModel == "ModelA")
    {
        model = NrSlUeProse::ModelA;
    }
    else if (discModel == "ModelB")
    {
        model = NrSlUeProse::ModelB;
    }
    else
    {
        NS_FATAL_ERROR("Wrong discovery model! It should be either ModelA or ModelB");
    }

    // Relay Selection Algorithm
    Ptr<NrSlUeProseRelaySelectionAlgorithm> algorithm;
    if (relaySelectAlgorithm == "FirstAvailableRelay")
    {
        algorithm = CreateObject<NrSlUeProseRelaySelectionAlgorithmFirstAvailable>();
    }
    else if (relaySelectAlgorithm == "RandomRelay")
    {
        algorithm = CreateObject<NrSlUeProseRelaySelectionAlgorithmRandom>();
    }
    else if (relaySelectAlgorithm == "MaxRsrpRelay")
    {
        algorithm = CreateObject<NrSlUeProseRelaySelectionAlgorithmMaxRsrp>();
    }
    else
    {
        NS_FATAL_ERROR("Unrecognized relay selection algorithm!");
    }

    // Configure discovery for Relay UEs
    std::vector<uint32_t> relayCodes;
    std::vector<uint32_t> relayDestL2Ids;

    std::vector<Time> startTimeRemote;
    std::vector<Time> startTimeRelay;

    Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();
    rand->SetStream(stream++);
    double discStart;
    std::cout << "Discovery configuration: " << std::endl;

    for (uint32_t i = 1; i <= relayNum; ++i)
    {
        relayCodes.push_back(i + 100);
        relayDestL2Ids.push_back(i + 500);

        discStart = rand->GetValue(discStartMin, discStartMax);
        startTimeRelay.push_back(Seconds(discStart));
        // relayUeNetDev.Get(i-1)->GetObject<NrSlUeProse>()->SetDiscoveryInterval(Seconds(i));
        std::cout << " UE " << i << ": discovery start = " << discStart
                  << " s and discovery interval = " << discInterval.GetSeconds() << " s"
                  << std::endl;
    }
    for (uint32_t j = 1; j <= remoteNum; ++j)
    {
        discStart = rand->GetValue(discStartMin, discStartMax);
        startTimeRemote.push_back(Seconds(discStart));
        // remoteUeNetDev.Get(j-1)->GetObject<NrSlUeProse>()->SetDiscoveryInterval(Seconds(j));
        std::cout << " UE " << j + relayNum << ": discovery start = " << discStart
                  << " s and discovery interval = " << discInterval.GetSeconds() << " s"
                  << std::endl;
    }

    // Configure the UL data radio bearer that the relay UE will use for U2N relaying traffic
    Ptr<NrEpcTft> tftRelay = Create<NrEpcTft>();
    NrEpcTft::PacketFilter pfRelay;
    tftRelay->Add(pfRelay);
    enum NrEpsBearer::Qci qciRelay;
    qciRelay = NrEpsBearer::GBR_CONV_VOICE;
    NrEpsBearer bearerRelay(qciRelay);

    // Start discovery and relay selection
    nrSlProseHelper->StartRemoteRelayConnection(remoteUeNetDev,
                                                startTimeRemote,
                                                relayUeNetDev,
                                                startTimeRelay,
                                                relayCodes,
                                                relayDestL2Ids,
                                                model,
                                                algorithm,
                                                tftRelay,
                                                bearerRelay);

    /*********************** End ProSe configuration ***************************/

    /********* Applications configuration ******/
    ///*
    // install UDP applications
    uint16_t dlPort = 1234;
    uint16_t ulPort = dlPort + gNbNum + 1;
    ApplicationContainer clientApps, serverApps;

    std::cout << "Remote traffic configuration: " << std::endl;

    // REMOTE UEs TRAFFIC
    for (uint32_t u = 0; u < remoteUeNodes.GetN(); ++u)
    {
        // DL traffic
        PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
                                            InetSocketAddress(Ipv4Address::GetAny(), dlPort));
        serverApps.Add(dlPacketSinkHelper.Install(remoteUeNodes.Get(u)));

        UdpClientHelper dlClient(ueIpIfaceRemote.GetAddress(u), dlPort);
        dlClient.SetAttribute("PacketSize", UintegerValue(packetSizeDlUl));
        dlClient.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaDlUl)));
        dlClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        clientApps.Add(dlClient.Install(remoteHost));

        std::cout << " DL: " << remoteHostAddr << " -> " << ueIpIfaceRemote.GetAddress(u) << ":"
                  << dlPort << " start time: " << trafficStart
                  << " s, end time: " << simTime.GetSeconds() << " s" << std::endl;

        Ptr<NrEpcTft> tftDl = Create<NrEpcTft>();
        NrEpcTft::PacketFilter pfDl;
        pfDl.localPortStart = dlPort;
        pfDl.localPortEnd = dlPort;
        ++dlPort;
        tftDl->Add(pfDl);

        enum NrEpsBearer::Qci qDl;
        qDl = NrEpsBearer::GBR_CONV_VOICE;

        NrEpsBearer bearerDl(qDl);
        nrHelper->ActivateDedicatedEpsBearer(remoteUeNetDev.Get(u), bearerDl, tftDl);

        // UL traffic
        PacketSinkHelper ulPacketSinkHelper("ns3::UdpSocketFactory",
                                            InetSocketAddress(Ipv4Address::GetAny(), ulPort));
        serverApps.Add(ulPacketSinkHelper.Install(remoteHost));

        UdpClientHelper ulClient(remoteHostAddr, ulPort);
        ulClient.SetAttribute("PacketSize", UintegerValue(packetSizeDlUl));
        ulClient.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaDlUl)));
        ulClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        clientApps.Add(ulClient.Install(remoteUeNodes.Get(u)));

        std::cout << " UL: " << ueIpIfaceRemote.GetAddress(u) << " -> " << remoteHostAddr << ":"
                  << ulPort << " start time: " << trafficStart
                  << " s, end time: " << simTime.GetSeconds() << " s" << std::endl;

        Ptr<NrEpcTft> tftUl = Create<NrEpcTft>();
        NrEpcTft::PacketFilter pfUl;
        pfUl.remoteAddress = remoteHostAddr; // IMPORTANT!!!
        pfUl.remotePortStart = ulPort;
        pfUl.remotePortEnd = ulPort;
        ++ulPort;
        tftUl->Add(pfUl);

        enum NrEpsBearer::Qci qUl;

        qUl = NrEpsBearer::GBR_CONV_VOICE;
        NrEpsBearer bearerUl(qUl);
        nrHelper->ActivateDedicatedEpsBearer(remoteUeNetDev.Get(u), bearerUl, tftUl);
    }
    std::cout << std::endl;

    serverApps.Start(Seconds(trafficStart));
    clientApps.Start(Seconds(trafficStart));
    serverApps.Stop(simTime);
    clientApps.Stop(simTime);
    //*/
    /********* END traffic applications configuration ******/

    if (writeTraces)
    {
        AsciiTraceHelper ascii;
        // PC5-S messages tracing
        Ptr<OutputStreamWrapper> Pc5SignallingPacketTraceStream =
            ascii.CreateFileStream("NrSlPc5SignallingPacketTrace.txt");
        *Pc5SignallingPacketTraceStream->GetStream()
            << "Time (s)\tTX/RX\tsrcL2Id\tdstL2Id\tmsgType" << std::endl;
        for (uint32_t i = 0; i < remoteUeNetDev.GetN(); ++i)
        {
            Ptr<NrSlUeProse> prose = remoteUeNetDev.Get(i)->GetObject<NrSlUeProse>();
            prose->TraceConnectWithoutContext("PC5SignallingPacketTrace",
                                              MakeBoundCallback(&TraceSinkPC5SignallingPacketTrace,
                                                                Pc5SignallingPacketTraceStream));
        }
        for (uint32_t i = 0; i < relayUeNetDev.GetN(); ++i)
        {
            Ptr<NrSlUeProse> prose = relayUeNetDev.Get(i)->GetObject<NrSlUeProse>();
            prose->TraceConnectWithoutContext("PC5SignallingPacketTrace",
                                              MakeBoundCallback(&TraceSinkPC5SignallingPacketTrace,
                                                                Pc5SignallingPacketTraceStream));
        }

        // Remote messages tracing
        Ptr<OutputStreamWrapper> RelayNasRxPacketTraceStream =
            ascii.CreateFileStream("NrSlRelayNasRxPacketTrace.txt");
        *RelayNasRxPacketTraceStream->GetStream()
            << "Time (s)\tnodeIp\tsrcIp\tdstIp\tsrcLink\tdstLink" << std::endl;
        for (uint32_t i = 0; i < relayUeNetDev.GetN(); ++i)
        {
            Ptr<NrEpcUeNas> epcUeNas = relayUeNetDev.Get(i)->GetObject<NrUeNetDevice>()->GetNas();

            epcUeNas->TraceConnectWithoutContext(
                "NrSlRelayRxPacketTrace",
                MakeBoundCallback(&TraceSinkRelayNasRxPacketTrace, RelayNasRxPacketTraceStream));
        }

        // Enable discovery traces
        nrSlProseHelper->EnableDiscoveryTraces();

        // Enable relay traces
        nrSlProseHelper->EnableRelayTraces();
    }

    // Run the simulation
    Simulator::Stop(simTime);
    Simulator::Run();

    // Write traces
    std::cout << "/*********** Simulation done! ***********/\n" << std::endl;
    std::cout << "Number of packets relayed by the L3 UE-to-Network relays:" << std::endl;
    std::cout << " relayIp      srcIp->dstIp      srcLink->dstLink\t\tnPackets" << std::endl;
    for (auto it = g_relayNasPacketCounter.begin(); it != g_relayNasPacketCounter.end(); ++it)
    {
        std::cout << " " << it->first << "\t\t" << it->second << std::endl;
    }

    Simulator::Destroy();
    return 0;
}

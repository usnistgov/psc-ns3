//
// SPDX-License-Identifier: NIST-Software
//

/**
 * \file nr-prose-unicast-l3-relay.cc
 * \ingroup examples
 *
 * \brief Basic scenario with some UEs doing in-network communication, some UEs
 *        doing ProSe unicast communication over SL and also in-network
 *        communication through a L3 UE-to-Network (U2N) relay UE.
 *
 * Channel configuration:
 * This example setups a simulation using the 3GPP channel model from TR 37.885
 * and it uses the default configuration of its implementation.
 *
 * System configuration:
 * The scenario uses one operational band, containing one component carrier,
 * and two bandwidth parts. One bandwidth part is used for in-network
 * communication, i.e., UL and DL between in-network UEs and gNBs, and the
 * other bandwidth part is used for SL communication between UEs using SL.
 *
 * Topology:
 * The scenario is composed of one gNB and four UEs. The node ID of the
 * gNB is 0, and IDs (and IMSIs) of the UEs are 1-4.  Two of the UEs (UE1 and
 * UE2) are attached to the gNB and the other two UEs (UE3 and UE4) are
 * out-of-network. UE2 is configured to serve as a L3 UE-to-Network (U2N)
 * relay.
 *
 *        -  gNB              (0.0, 30.0, 10.0)
 *        |
 *   20 m |
 *        -  UE1 UE2          (0.0, 10.0, 1.5) (1.0, 10.0, 1.5)
 *   10 m |       |
 *        -  UE3  |  UE4      (0.0, 0.0, 1.5) (2.0, 0.0, 1.5)
 *            |---|---|
 *             1 m 1 m
 *            |-------|
 *               2 m
 *
 * ProSe Unicast:
 * UE3 and UE4 will start the establishment of the ProSe unicast direct links
 * before the start of the unicast traffic. The configuration is the following:
 *     Link     | Initiating UE  |   Target UE
 * ---------------------------------------------
 * UE3 <-> UE4  |      UE3       |     UE4
 *
 * L3 UE-to-Network relay:
 * UE3 and UE4 will start the establishment of the L3 U2N relay connection
 * before the start of the in-network traffic. This will internally start the
 * establishment of the corresponding ProSe unicast direct links. The
 * configuration is the following:
 *              |   Remote UE    |  Relay UE
 *     Link     |(Initiating UE) |(Target UE)
 * ---------------------------------------------
 * UE3 <-> UE2  |       UE3      |    UE2
 * UE4 <-> UE2  |       UE4      |    UE2
 *
 *
 * Traffic:
 * There are two CBR traffic flows concerning the in-network UEs (UE1 and UE2),
 * one from a Remote Host in the internet towards each in-network UE (DL) and
 * one from the in-network UEs towards the Remote Host (UL). Additionally, two
 * CBR traffic flows with the same configuration are configured for each
 * out-of-network UE (UE3 and UE4) to be served when they connect to the U2N
 * relay UE (UE2).
 * Regarding SL unicast only traffic, two CBR traffic flows involve the
 * out-of-network UEs (UE3 and UE4), one on each direction of each established
 * unicast direct link.
 *
 * Output:
 * The example will print on-screen the traffic flows configuration and the
 * end-to-end statistics of each of them after the simulation finishes together
 * with the number of packets relayed by the L3 UE-to-Network relay.
 * The example also produces three output files:
 * 1. default-nr-prose-unicast-l3-relay-flowMonitorOutput.txt" Contains the
 * end-to-end statistics of each traffic flow.
 * 2. default-nr-prose-unicast-l3-relay-SlTraces.db: contains MAC and PHY layer
 * traces in a sqlite3 database created using ns-3 stats module.
 * 3. NrSlPc5SignallingPacketTrace.txt: log of the transmitted and received PC5
 * signaling messages used for the establishment of each ProSe unicast direct
 * link.
 * 4. NrSlRelayNasRxPacketTrace.txt: log of the packets received and routed by
 * the NAS of the UE acting as L3 UE-to-Network UE.
 *
 * \code{.unparsed}
$ ./ns3 run "nr-prose-unicast-l3-relay --Help"
    \endcode
 */

#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-module.h"
#include "ns3/nr-prose-module.h"
#include "ns3/point-to-point-module.h"

#include <sqlite3.h>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrProseUnicastL3Relay");

/*************************** Methods for tracing SL **************************/
/*Please refer to nr-prose-unicast-multi-link.cc for function documentation */
void
NotifySlPscchScheduling(UeMacPscchTxOutputStats* pscchStats,
                        const SlPscchUeMacStatParameters pscchStatsParams)
{
    pscchStats->Save(pscchStatsParams);
}

void
NotifySlPsschScheduling(UeMacPsschTxOutputStats* psschStats,
                        const SlPsschUeMacStatParameters psschStatsParams)
{
    psschStats->Save(psschStatsParams);
}

void
NotifySlPscchRx(UePhyPscchRxOutputStats* pscchStats,
                const SlRxCtrlPacketTraceParams pscchStatsParams)
{
    pscchStats->Save(pscchStatsParams);
}

void
NotifySlPsschRx(UePhyPsschRxOutputStats* psschStats,
                const SlRxDataPacketTraceParams psschStatsParams)
{
    psschStats->Save(psschStatsParams);
}

void
UePacketTraceDb(UeToUePktTxRxOutputStats* stats,
                Ptr<Node> node,
                const Address& localAddrs,
                std::string txRx,
                Ptr<const Packet> p,
                const Address& srcAddrs,
                const Address& dstAddrs,
                const SeqTsSizeHeader& seqTsSizeHeader)
{
    uint32_t nodeId = node->GetId();
    uint64_t imsi = node->GetDevice(0)->GetObject<NrUeNetDevice>()->GetImsi();
    uint32_t seq = seqTsSizeHeader.GetSeq();
    uint32_t pktSize = p->GetSize() + seqTsSizeHeader.GetSerializedSize();

    stats->Save(txRx, localAddrs, nodeId, imsi, pktSize, srcAddrs, dstAddrs, seq);
}

/************************END Methods for tracing SL **************************/

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
    double centralFrequencyBand = 5.89e9; // band n47 (From SL examples)
    double bandwidthBand = 40e6;          // 40 MHz
    double centralFrequencyCc0 = 5.89e9;
    double bandwidthCc0 = bandwidthBand;
    std::string pattern = "DL|DL|DL|F|UL|UL|UL|UL|UL|UL|"; // From SL examples
    double bandwidthCc0Bpw0 = bandwidthCc0 / 2;
    double bandwidthCc0Bpw1 = bandwidthCc0 / 2;

    // In-network devices configuration
    uint16_t gNbNum = 1;
    uint16_t inNetUeNumPerGnb = 1;
    double gNbHeight = 10;
    double ueHeight = 1.5;
    uint16_t numerologyCc0Bwp0 = 3;    //                  BWP0 will be used for the in-network
    double gNBtotalTxPower = 8;        // dBm
    bool cellScan = false;             // Beamforming method.
    double beamSearchAngleStep = 10.0; // Beamforming parameter.

    // In-network applications configuration
    uint32_t packetSizeDlUl = 500;      // bytes
    uint32_t lambdaDl = 60;             // packets per second
    uint32_t lambdaUl = 50;             // packets per second
    double inNetTrafficStartTime = 5.0; // seconds

    // Sidelink configuration
    uint16_t numerologyCc0Bwp1 = 2; //(From SL examples)  BWP1 will be used for SL
    uint16_t slUeNum = 2;
    uint16_t slInterUeDistance = 2; // m
    Time startDirLinkTime =
        Seconds(2.0); // Time to start the Prose Unicast Direct Link establishment procedure
    Time startRelayConnTime = Seconds(2.0); // Time to start the U2N relay connection establishment

    // Sidelink applications configuration
    uint32_t packetSizeSl = 200; // bytes
    double dataRateSl = 16;      // 16 kilobits per second
    bool bidirectionalSlTraffic = true;
    Time slTrafficStartTime = Seconds(5.0); // Time to start the traffic in the application layer

    // U2N Relay configuration
    uint16_t relayUeNum = 1;

    // Simulation configuration
    std::string simTag = "default";
    std::string outputDir = "./";
    double simTime = 10; // seconds

    CommandLine cmd;

    cmd.AddValue("simTime", "Simulation time", simTime);

    cmd.Parse(argc, argv);

    NS_ABORT_IF(numBands < 1);

    // Setup large enough buffer size to avoid overflow
    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    // create gNBs and in-network UEs, configure positions
    NodeContainer gNbNodes;
    NodeContainer inNetUeNodes;
    MobilityHelper mobility;

    gNbNodes.Create(gNbNum);
    inNetUeNodes.Create(inNetUeNumPerGnb * gNbNum);

    Ptr<ListPositionAllocator> gNbPositionAlloc = CreateObject<ListPositionAllocator>();
    Ptr<ListPositionAllocator> uePositionAlloc = CreateObject<ListPositionAllocator>();
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

        double xValue = 0.0;
        for (uint32_t j = 1; j <= inNetUeNumPerGnb; ++j)
        {
            if (j % 2 != 0)
            {
                xValue = j;
            }
            else
            {
                xValue = -xValue;
            }

            if (yValue > 0)
            {
                uePositionAlloc->Add(Vector(xValue, 10, ueHeight));
            }
            else
            {
                uePositionAlloc->Add(Vector(xValue, -10, ueHeight));
            }
        }
    }

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(gNbPositionAlloc);
    mobility.Install(gNbNodes);

    mobility.SetPositionAllocator(uePositionAlloc);
    mobility.Install(inNetUeNodes);

    // Create U2N relay nodes
    NodeContainer relayUeNodes;
    relayUeNodes.Create(relayUeNum);
    // UE nodes mobility setup
    MobilityHelper mobilityRelays;
    mobilityRelays.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> relayUesPositionAlloc = CreateObject<ListPositionAllocator>();
    for (uint16_t i = 0; i < relayUeNum; i++)
    {
        relayUesPositionAlloc->Add(Vector(slInterUeDistance * i, 10.0, 1.5));
    }
    mobilityRelays.SetPositionAllocator(relayUesPositionAlloc);
    mobilityRelays.Install(relayUeNodes);

    // Create SL only UEs, configure positions
    NodeContainer slUeNodes;
    slUeNodes.Create(slUeNum);
    // UE nodes mobility setup
    MobilityHelper mobilitySl;
    mobilitySl.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> slUesPositionAlloc = CreateObject<ListPositionAllocator>();
    for (uint16_t i = 0; i < slUeNum; i++)
    {
        slUesPositionAlloc->Add(Vector(slInterUeDistance * i, 0.0, 1.5));
    }
    mobilitySl.SetPositionAllocator(slUesPositionAlloc);
    mobilitySl.Install(slUeNodes);

    // Setup Helpers
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    nrHelper->SetEpcHelper(epcHelper);

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

    band.m_centralFrequency = centralFrequencyBand;
    band.m_channelBandwidth = bandwidthBand;
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

    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();
    channelHelper->ConfigureFactories("RMa", "Default", "ThreeGpp");
    /**
     * Use channelHelper API to define the attributes for the channel model (condition, pathloss and
     * spectrum)
     */
    channelHelper->SetChannelConditionModelAttribute("UpdatePeriod", TimeValue(MilliSeconds(0)));
    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));
    channelHelper->AssignChannelsToBands({band});

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

    /* Create NrSlHelper which will configure the UEs protocol stack to be ready to
     * perform Sidelink related procedures
     */
    Ptr<NrSlHelper> nrSlHelper = CreateObject<NrSlHelper>();
    nrSlHelper->SetEpcHelper(epcHelper);
    nrSlHelper->SetUeBwpManagerAlgorithmAttribute("GBR_MC_PUSH_TO_TALK", UintegerValue(bwpIdSl));

    // SL UE MAC configuration (for non relay UEs)
    nrSlHelper->SetUeMacAttribute("EnableSensing", BooleanValue(false));
    nrSlHelper->SetUeMacAttribute("T1", UintegerValue(2));
    nrSlHelper->SetUeMacAttribute("ActivePoolId", UintegerValue(0));
    nrSlHelper->SetUeMacAttribute("NumHarqProcess", UintegerValue(255));
    nrSlHelper->SetUeMacAttribute("SlThresPsschRsrp", IntegerValue(-128));

    // Install both BWPs on SL-only UEs
    // This was needed to avoid errors with bwpId and vector indexes during device installation
    NetDeviceContainer slUeNetDev = nrSlHelper->InstallUeDevice(slUeNodes, allBwps);
    // Set the SL error model and AMC
    nrSlHelper->SetSlErrorModelTypeId(NrSlStaticErrorModel::GetTypeId());

    // Set the SL scheduler attributes
    nrSlHelper->SetNrSlSchedulerTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    nrSlHelper->SetUeSlSchedulerAttribute("DefaultMcs", UintegerValue(14));

    // Configure U2N relay UEs for SL
    std::set<uint8_t> slBwpIdContainerRelay;
    slBwpIdContainerRelay.insert(bwpIdSl); // Only in the SL BWP for the relay UEs
    nrSlHelper->PrepareUeForSidelink(relayUeNetDev, slBwpIdContainerRelay);

    // Configure SL-only UEs for SL
    nrSlHelper->PrepareUeForSidelink(slUeNetDev, slBwpIdContainer);

    /***SL IEs configuration **/

    // SlResourcePoolNr IE
    NrSlRrcSap::SlResourcePoolNr slResourcePoolNr;
    // get it from pool factory
    Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory>();
    // Configure specific parameters of interest:
    std::vector<std::bitset<1>> slBitmap = {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1};
    ptrFactory->SetSlTimeResources(slBitmap);
    ptrFactory->SetSlSensingWindow(100); // T0 in ms
    ptrFactory->SetSlSelectionWindow(5);
    ptrFactory->SetSlFreqResourcePscch(10); // PSCCH RBs
    ptrFactory->SetSlSubchannelSize(10);
    ptrFactory->SetSlMaxNumPerReserve(3);
    std::list<uint16_t> resourceReservePeriodList = {0, 100}; // in ms
    ptrFactory->SetSlResourceReservePeriodList(resourceReservePeriodList);
    // Once parameters are configured, we can create the pool
    NrSlRrcSap::SlResourcePoolNr pool = ptrFactory->CreatePool();
    slResourcePoolNr = pool;

    // Configure the SlResourcePoolConfigNr IE, which holds a pool and its id
    NrSlRrcSap::SlResourcePoolConfigNr slresoPoolConfigNr;
    slresoPoolConfigNr.haveSlResourcePoolConfigNr = true;
    // Pool id, ranges from 0 to 15
    uint16_t poolId = 0;
    NrSlRrcSap::SlResourcePoolIdNr slResourcePoolIdNr;
    slResourcePoolIdNr.id = poolId;
    slresoPoolConfigNr.slResourcePoolId = slResourcePoolIdNr;
    slresoPoolConfigNr.slResourcePool = slResourcePoolNr;

    // Configure the SlBwpPoolConfigCommonNr IE, which holds an array of pools
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
    psschParams.slMaxTxTransNumPssch = 5;
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
    nrSlHelper->InstallNrSlPreConfiguration(slUeNetDev, slPreConfigNr);

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

    /***END SL IEs configuration **/

    // Set random streams
    int64_t randomStream = 1;
    const uint64_t streamIncrement = 1000;
    nrHelper->AssignStreams(gnbNetDev, randomStream);
    randomStream += streamIncrement;
    nrHelper->AssignStreams(inNetUeNetDev, randomStream);
    randomStream += streamIncrement;
    nrHelper->AssignStreams(relayUeNetDev, randomStream);
    randomStream += streamIncrement;
    nrSlHelper->AssignStreams(relayUeNetDev, randomStream);
    randomStream += streamIncrement;
    nrHelper->AssignStreams(slUeNetDev, randomStream);
    randomStream += streamIncrement;
    nrSlHelper->AssignStreams(slUeNetDev, randomStream);

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

    // Configure in-network only UEs
    internet.Install(inNetUeNodes);
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(inNetUeNetDev));
    // Set the default gateway for the in-network UEs
    for (uint32_t j = 0; j < inNetUeNodes.GetN(); ++j)
    {
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(inNetUeNodes.Get(j)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);
        std::cout << " In-network only UE: "
                  << inNetUeNodes.Get(j)->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal()
                  << std::endl;
    }

    // Attach in-network UEs to the closest gNB
    nrHelper->AttachToClosestGnb(inNetUeNetDev, gnbNetDev);

    // Configure U2N relay UEs
    internet.Install(relayUeNodes);
    Ipv4InterfaceContainer ueIpIfaceRelays;
    ueIpIfaceRelays = epcHelper->AssignUeIpv4Address(NetDeviceContainer(relayUeNetDev));
    std::vector<Ipv4Address> relaysIpv4AddressVector(relayUeNum);

    for (uint32_t u = 0; u < relayUeNodes.GetN(); ++u)
    {
        // Set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(relayUeNodes.Get(u)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

        // Obtain local IPv4 addresses that will be used to route the unicast traffic upon setup of
        // the direct link
        relaysIpv4AddressVector[u] =
            relayUeNodes.Get(u)->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal();
        std::cout << " Relay UE: " << relaysIpv4AddressVector[u] << std::endl;
    }

    // Attach U2N relay UEs to the closest gNB
    nrHelper->AttachToClosestGnb(relayUeNetDev, gnbNetDev);

    // Configure out-of-network UEs
    internet.Install(slUeNodes);
    Ipv4InterfaceContainer ueIpIfaceSl;
    ueIpIfaceSl = epcHelper->AssignUeIpv4Address(NetDeviceContainer(slUeNetDev));
    std::vector<Ipv4Address> slIpv4AddressVector(slUeNum);

    for (uint32_t u = 0; u < slUeNodes.GetN(); ++u)
    {
        // Set the default gateway for the UE
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            ipv4RoutingHelper.GetStaticRouting(slUeNodes.Get(u)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

        // Obtain local IPv4 addresses that will be used to route the unicast traffic upon setup of
        // the direct link
        slIpv4AddressVector[u] =
            slUeNodes.Get(u)->GetObject<Ipv4L3Protocol>()->GetAddress(1, 0).GetLocal();
        std::cout << " Out-of-network UE: " << slIpv4AddressVector[u] << std::endl;
    }

    /******** Configure ProSe layer in the UEs that will do SL  **********/
    // Create ProSe helper
    Ptr<NrSlProseHelper> nrSlProseHelper = CreateObject<NrSlProseHelper>();
    nrSlProseHelper->SetEpcHelper(epcHelper);

    // Install ProSe layer and corresponding SAPs in the UEs
    nrSlProseHelper->PrepareUesForProse(relayUeNetDev);
    nrSlProseHelper->PrepareUesForProse(slUeNetDev);

    // Configure ProSe Unicast parameters. At the moment it only instruct the MAC
    // layer (and PHY therefore) to monitor packets directed the UE's own Layer 2 ID
    nrSlProseHelper->PrepareUesForUnicast(relayUeNetDev);
    nrSlProseHelper->PrepareUesForUnicast(slUeNetDev);

    // Configure the value of timer Timer T5080 (Prose Direct Link Establishment Request
    // Retransmission) to a lower value than the standard (8.0 s) to speed connection in shorter
    // simulation time
    Config::SetDefault("ns3::NrSlUeProseDirectLink::T5080", TimeValue(Seconds(2.0)));

    /*
     * Setup the start of the ProSe direct link establishment procedure
     * (with the 'Real' protocol, PC5-S messages are exchanged over the SL)
     * First UE on the function call will be the initiating UE (UE i),
     * which starts the procedure, and it is interested in establish a direct
     * link with the following j UEs, which will be the target UEs
     */
    SidelinkInfo initSlInfo;
    initSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
    initSlInfo.m_dynamic = true;
    initSlInfo.m_harqEnabled = false;
    initSlInfo.m_priority = 0;
    initSlInfo.m_rri = Seconds(0);
    initSlInfo.m_pdb = MilliSeconds(20);

    SidelinkInfo trgtSlInfo;
    trgtSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
    trgtSlInfo.m_dynamic = true;
    trgtSlInfo.m_harqEnabled = false;
    trgtSlInfo.m_priority = 0;
    trgtSlInfo.m_rri = Seconds(0);
    trgtSlInfo.m_pdb = MilliSeconds(20);
    NS_LOG_INFO("Configuring SL-only unicast direct links...");

    for (uint32_t i = 0; i < slUeNodes.GetN() - 1; ++i)
    {
        for (uint32_t j = i + 1; j < slUeNodes.GetN(); ++j)
        {
            nrSlProseHelper->EstablishRealDirectLink(startDirLinkTime,
                                                     slUeNetDev.Get(i),
                                                     slIpv4AddressVector[i],
                                                     initSlInfo, // Initiating UE
                                                     slUeNetDev.Get(j),
                                                     slIpv4AddressVector[j],
                                                     trgtSlInfo); // Target UE
            NS_LOG_INFO("Initiating UE nodeId " << i << " target UE nodeId " << j);
        }
    }
    /******** END Configure ProSe layer in the UEs that will do SL  **********/

    /******************** L3 U2N Relay configuration ***************************/
    // Provision L3 U2N configuration on the relay UEs

    //-Configure relay service codes
    // Only one relay service per relay UE can currently be provided
    uint32_t relayServiceCode = 5;
    std::set<uint32_t> providedRelaySCs;
    providedRelaySCs.insert(relayServiceCode);

    //-Configure the UL data radio bearer that the relay UE will use for U2N relaying traffic
    Ptr<NrEpcTft> tftRelay = Create<NrEpcTft>();
    NrEpcTft::PacketFilter pfRelay;
    tftRelay->Add(pfRelay);
    enum NrEpsBearer::Qci qciRelay;
    qciRelay = NrEpsBearer::GBR_CONV_VOICE;
    NrEpsBearer bearerRelay(qciRelay);

    // Apply the configuration on the devices acting as relay UEs
    nrSlProseHelper->ConfigureL3UeToNetworkRelay(relayUeNetDev,
                                                 providedRelaySCs,
                                                 bearerRelay,
                                                 tftRelay);

    // Configure direct link connection between remote UEs and relay UEs
    NS_LOG_INFO("Configuring remote UE - relay UE connection...");
    SidelinkInfo remoteUeSlInfo;
    remoteUeSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
    remoteUeSlInfo.m_dynamic = true;
    remoteUeSlInfo.m_harqEnabled = false;
    remoteUeSlInfo.m_priority = 0;
    remoteUeSlInfo.m_rri = Seconds(0);
    remoteUeSlInfo.m_pdb = MilliSeconds(20);

    SidelinkInfo relayUeSlInfo;
    relayUeSlInfo.m_castType = SidelinkInfo::CastType::Unicast;
    relayUeSlInfo.m_dynamic = true;
    relayUeSlInfo.m_harqEnabled = false;
    relayUeSlInfo.m_priority = 0;
    relayUeSlInfo.m_rri = Seconds(0);
    relayUeSlInfo.m_pdb = MilliSeconds(20);
    for (uint32_t i = 0; i < slUeNodes.GetN(); ++i)
    {
        for (uint32_t j = 0; j < relayUeNetDev.GetN(); ++j)
        {
            nrSlProseHelper->EstablishL3UeToNetworkRelayConnection(startRelayConnTime,
                                                                   slUeNetDev.Get(i),
                                                                   slIpv4AddressVector[i],
                                                                   remoteUeSlInfo, // Remote UE
                                                                   relayUeNetDev.Get(j),
                                                                   relaysIpv4AddressVector[j],
                                                                   relayUeSlInfo, // Relay UE
                                                                   relayServiceCode);

            NS_LOG_INFO("Remote UE nodeId " << slUeNetDev.Get(i)->GetNode()->GetId()
                                            << " Relay UE nodeId "
                                            << relayUeNetDev.Get(j)->GetNode()->GetId());
        }
    }
    /******************** END L3 U2N Relay configuration ***********************/

    /********* In-network only applications configuration ******/
    // install UDP applications
    uint16_t dlPort = 1234;
    uint16_t ulPort = dlPort + gNbNum * inNetUeNumPerGnb + 1;
    ApplicationContainer clientApps, serverApps;

    // IN-NETWORK ONLY UEs TRAFFIC
    std::cout << "In-network traffic flows: " << std::endl;
    for (uint32_t u = 0; u < inNetUeNodes.GetN(); ++u)
    {
        // DL traffic
        PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
                                            InetSocketAddress(Ipv4Address::GetAny(), dlPort));
        serverApps.Add(dlPacketSinkHelper.Install(inNetUeNodes.Get(u)));

        UdpClientHelper dlClient(ueIpIface.GetAddress(u), dlPort);
        dlClient.SetAttribute("PacketSize", UintegerValue(packetSizeDlUl));
        dlClient.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaDl)));
        dlClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        clientApps.Add(dlClient.Install(remoteHost));

        std::cout << " DL: " << remoteHostAddr << " -> " << ueIpIface.GetAddress(u) << ":" << dlPort
                  << " start time: " << inNetTrafficStartTime << " s, end time: " << simTime << " s"
                  << std::endl;

        Ptr<NrEpcTft> tftDl = Create<NrEpcTft>();
        NrEpcTft::PacketFilter pfDl;
        pfDl.localPortStart = dlPort;
        pfDl.localPortEnd = dlPort;
        ++dlPort;
        tftDl->Add(pfDl);

        enum NrEpsBearer::Qci qDl;
        qDl = NrEpsBearer::GBR_CONV_VOICE;

        NrEpsBearer bearerDl(qDl);
        nrHelper->ActivateDedicatedEpsBearer(inNetUeNetDev.Get(u), bearerDl, tftDl);

        // UL traffic
        PacketSinkHelper ulPacketSinkHelper("ns3::UdpSocketFactory",
                                            InetSocketAddress(Ipv4Address::GetAny(), ulPort));
        serverApps.Add(ulPacketSinkHelper.Install(remoteHost));

        UdpClientHelper ulClient(remoteHostAddr, ulPort);
        ulClient.SetAttribute("PacketSize", UintegerValue(packetSizeDlUl));
        ulClient.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaUl)));
        ulClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        clientApps.Add(ulClient.Install(inNetUeNodes.Get(u)));

        std::cout << " UL: " << ueIpIface.GetAddress(u) << " -> " << remoteHostAddr << ":" << ulPort
                  << " start time: " << inNetTrafficStartTime << " s, end time: " << simTime << " s"
                  << std::endl;

        Ptr<NrEpcTft> tftUl = Create<NrEpcTft>();
        NrEpcTft::PacketFilter pfUl;
        pfUl.remotePortStart = ulPort;
        pfUl.remotePortEnd = ulPort;
        ++ulPort;
        tftUl->Add(pfUl);

        enum NrEpsBearer::Qci qUl;

        qUl = NrEpsBearer::GBR_CONV_VOICE;
        NrEpsBearer bearerUl(qUl);
        nrHelper->ActivateDedicatedEpsBearer(inNetUeNetDev.Get(u), bearerUl, tftUl);
    }

    // RELAY UE's OWN IN-NETWORK TRAFFIC
    for (uint32_t u = 0; u < relayUeNodes.GetN(); ++u)
    {
        // DL traffic
        PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
                                            InetSocketAddress(Ipv4Address::GetAny(), dlPort));
        serverApps.Add(dlPacketSinkHelper.Install(relayUeNodes.Get(u)));

        UdpClientHelper dlClient(ueIpIfaceRelays.GetAddress(u), dlPort);
        dlClient.SetAttribute("PacketSize", UintegerValue(packetSizeDlUl));
        dlClient.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaDl)));
        dlClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        clientApps.Add(dlClient.Install(remoteHost));

        std::cout << " DL: " << remoteHostAddr << " -> " << ueIpIfaceRelays.GetAddress(u) << ":"
                  << dlPort << " start time: " << inNetTrafficStartTime
                  << " s, end time: " << simTime << " s" << std::endl;

        Ptr<NrEpcTft> tftDl = Create<NrEpcTft>();
        NrEpcTft::PacketFilter pfDl;
        pfDl.localPortStart = dlPort;
        pfDl.localPortEnd = dlPort;
        ++dlPort;
        tftDl->Add(pfDl);

        enum NrEpsBearer::Qci qDl;
        qDl = NrEpsBearer::GBR_CONV_VOICE;

        NrEpsBearer bearerDl(qDl);
        nrHelper->ActivateDedicatedEpsBearer(relayUeNetDev.Get(u), bearerDl, tftDl);

        // UL traffic
        PacketSinkHelper ulPacketSinkHelper("ns3::UdpSocketFactory",
                                            InetSocketAddress(Ipv4Address::GetAny(), ulPort));
        serverApps.Add(ulPacketSinkHelper.Install(remoteHost));

        UdpClientHelper ulClient(remoteHostAddr, ulPort);
        ulClient.SetAttribute("PacketSize", UintegerValue(packetSizeDlUl));
        ulClient.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaUl)));
        ulClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        clientApps.Add(ulClient.Install(relayUeNodes.Get(u)));

        std::cout << " UL: " << ueIpIfaceRelays.GetAddress(u) << " -> " << remoteHostAddr << ":"
                  << ulPort << " start time: " << inNetTrafficStartTime
                  << " s, end time: " << simTime << " s" << std::endl;

        Ptr<NrEpcTft> tftUl = Create<NrEpcTft>();
        NrEpcTft::PacketFilter pfUl;
        pfUl.remoteAddress = remoteHostAddr;
        pfUl.remotePortStart = ulPort;
        pfUl.remotePortEnd = ulPort;
        ++ulPort;
        tftUl->Add(pfUl);

        enum NrEpsBearer::Qci qUl;

        qUl = NrEpsBearer::GBR_CONV_VOICE;
        NrEpsBearer bearerUl(qUl);
        nrHelper->ActivateDedicatedEpsBearer(relayUeNetDev.Get(u), bearerUl, tftUl);
    }

    // REMOTE UEs IN-NETWORK TRAFFIC
    for (uint32_t u = 0; u < slUeNodes.GetN(); ++u)
    {
        // DL traffic
        PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
                                            InetSocketAddress(Ipv4Address::GetAny(), dlPort));
        serverApps.Add(dlPacketSinkHelper.Install(slUeNodes.Get(u)));

        UdpClientHelper dlClient(ueIpIfaceSl.GetAddress(u), dlPort);
        dlClient.SetAttribute("PacketSize", UintegerValue(packetSizeDlUl));
        dlClient.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaDl)));
        dlClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        clientApps.Add(dlClient.Install(remoteHost));

        std::cout << " DL: " << remoteHostAddr << " -> " << ueIpIfaceSl.GetAddress(u) << ":"
                  << dlPort << " start time: " << inNetTrafficStartTime
                  << " s, end time: " << simTime << " s" << std::endl;

        Ptr<NrEpcTft> tftDl = Create<NrEpcTft>();
        NrEpcTft::PacketFilter pfDl;
        pfDl.localPortStart = dlPort;
        pfDl.localPortEnd = dlPort;
        ++dlPort;
        tftDl->Add(pfDl);

        enum NrEpsBearer::Qci qDl;
        qDl = NrEpsBearer::GBR_CONV_VOICE;

        NrEpsBearer bearerDl(qDl);
        nrHelper->ActivateDedicatedEpsBearer(slUeNetDev.Get(u), bearerDl, tftDl);

        // UL traffic
        PacketSinkHelper ulPacketSinkHelper("ns3::UdpSocketFactory",
                                            InetSocketAddress(Ipv4Address::GetAny(), ulPort));
        serverApps.Add(ulPacketSinkHelper.Install(remoteHost));

        UdpClientHelper ulClient(remoteHostAddr, ulPort);
        ulClient.SetAttribute("PacketSize", UintegerValue(packetSizeDlUl));
        ulClient.SetAttribute("Interval", TimeValue(Seconds(1.0 / lambdaUl)));
        ulClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        clientApps.Add(ulClient.Install(slUeNodes.Get(u)));

        std::cout << " UL: " << ueIpIfaceSl.GetAddress(u) << " -> " << remoteHostAddr << ":"
                  << ulPort << " start time: " << inNetTrafficStartTime
                  << " s, end time: " << simTime << " s" << std::endl;

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
        nrHelper->ActivateDedicatedEpsBearer(slUeNetDev.Get(u), bearerUl, tftUl);
    }

    serverApps.Start(Seconds(inNetTrafficStartTime));
    clientApps.Start(Seconds(inNetTrafficStartTime));
    serverApps.Stop(Seconds(simTime));
    clientApps.Stop(Seconds(simTime));
    /********* END In-network only applications configuration ******/

    /******************* SL application configuration *****************************/
    /* - Client app: OnOff application configured to generate CBR traffic.
     *               Installed in the initiating UE of each link by default, and
     *               in target UE if 'bidirectional' flag is True.
     * - Server app: PacketSink application to consume the received traffic.
     *               Installed in all UEs
     */
    // Random variable to randomize a bit start times of the client applications
    // to avoid simulation artifacts of all the TX UEs transmitting at the same time.
    Ptr<UniformRandomVariable> startTimeRnd = CreateObject<UniformRandomVariable>();
    randomStream += streamIncrement;
    startTimeRnd->SetStream(randomStream);
    startTimeRnd->SetAttribute("Min", DoubleValue(0));
    startTimeRnd->SetAttribute("Max", DoubleValue(0.10));
    uint16_t slPort = 8000;

    std::string dataRateString = std::to_string(dataRateSl) + "kb/s";
    ApplicationContainer slClientApps;
    std::cout << "Sidelink traffic flows: " << std::endl;
    for (uint32_t i = 0; i < slUeNodes.GetN() - 1; ++i)
    {
        for (uint32_t j = i + 1; j < slUeNodes.GetN(); ++j)
        {
            OnOffHelper sidelinkClient(
                "ns3::UdpSocketFactory",
                InetSocketAddress(slIpv4AddressVector[j], slPort)); // Towards UE j
            sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
            sidelinkClient.SetConstantRate(DataRate(dataRateString), packetSizeSl);
            ApplicationContainer app =
                sidelinkClient.Install(slUeNodes.Get(i)); // Installed in UE i
            Time appStart = slTrafficStartTime + Seconds(startTimeRnd->GetValue());
            app.Start(appStart);
            slClientApps.Add(app);
            NS_LOG_INFO("OnOff application installed in UE nodeId "
                        << slUeNetDev.Get(i)->GetNode()->GetId() << " srcIp "
                        << slIpv4AddressVector[i] << " towards UE nodeId "
                        << slUeNetDev.Get(j)->GetNode()->GetId() << " dstIp "
                        << slIpv4AddressVector[j]);
            std::cout << " SL: " << slIpv4AddressVector[i] << " -> " << slIpv4AddressVector[j]
                      << " start time: " << appStart.GetSeconds() << " s, end time: " << simTime
                      << " s" << std::endl;

            if (bidirectionalSlTraffic)
            {
                OnOffHelper sidelinkClient(
                    "ns3::UdpSocketFactory",
                    InetSocketAddress(slIpv4AddressVector[i], slPort)); // Towards UE i
                sidelinkClient.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
                sidelinkClient.SetConstantRate(DataRate(dataRateString), packetSizeSl);
                ApplicationContainer app =
                    sidelinkClient.Install(slUeNodes.Get(j)); // Installed in UE j
                Time appStart = slTrafficStartTime + Seconds(startTimeRnd->GetValue());
                app.Start(appStart);
                slClientApps.Add(app);
                NS_LOG_INFO("OnOff application installed in UE nodeId "
                            << slUeNetDev.Get(j)->GetNode()->GetId() << " srcIp "
                            << slIpv4AddressVector[j] << " towards UE nodeId "
                            << slUeNetDev.Get(i)->GetNode()->GetId() << " dstIp "
                            << slIpv4AddressVector[i]);
                std::cout << " SL: " << slIpv4AddressVector[j] << " -> " << slIpv4AddressVector[i]
                          << " start time: " << appStart.GetSeconds() << " s, end time: " << simTime
                          << " s" << std::endl;
            }
        }
    }

    slClientApps.Stop(Seconds(simTime));

    ApplicationContainer slServerApps;
    PacketSinkHelper sidelinkSink("ns3::UdpSocketFactory",
                                  InetSocketAddress(Ipv4Address::GetAny(), slPort));
    sidelinkSink.SetAttribute("EnableSeqTsSizeHeader", BooleanValue(true));
    slServerApps =
        sidelinkSink.Install(NodeContainer(slUeNodes, relayUeNodes)); // Installed in all UEs
    slServerApps.Start(Seconds(1.0));
    slServerApps.Stop(Seconds(simTime));
    /******************** End SL application configuration ************************/

    randomStream += streamIncrement;
    ApplicationHelper::AssignStreamsToAllApps(gNbNodes, randomStream);
    randomStream += streamIncrement;
    ApplicationHelper::AssignStreamsToAllApps(inNetUeNodes, randomStream);
    randomStream += streamIncrement;
    ApplicationHelper::AssignStreamsToAllApps(relayUeNodes, randomStream);
    randomStream += streamIncrement;
    ApplicationHelper::AssignStreamsToAllApps(slUeNodes, randomStream);
    randomStream += streamIncrement;
    ApplicationHelper::AssignStreamsToAllApps(remoteHostContainer, randomStream);

    /******************* PC5-S messages tracing ********************************/
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> Pc5SignallingPacketTraceStream =
        ascii.CreateFileStream("NrSlPc5SignallingPacketTrace.txt");
    *Pc5SignallingPacketTraceStream->GetStream()
        << "time(s)\tTX/RX\tsrcL2Id\tdstL2Id\tmsgType" << std::endl;
    for (uint32_t i = 0; i < slUeNetDev.GetN(); ++i)
    {
        Ptr<NrSlUeProse> prose = slUeNetDev.Get(i)->GetObject<NrSlUeProse>();
        prose->TraceConnectWithoutContext(
            "PC5SignallingPacketTrace",
            MakeBoundCallback(&TraceSinkPC5SignallingPacketTrace, Pc5SignallingPacketTraceStream));
    }
    for (uint32_t i = 0; i < relayUeNetDev.GetN(); ++i)
    {
        Ptr<NrSlUeProse> prose = relayUeNetDev.Get(i)->GetObject<NrSlUeProse>();
        prose->TraceConnectWithoutContext(
            "PC5SignallingPacketTrace",
            MakeBoundCallback(&TraceSinkPC5SignallingPacketTrace, Pc5SignallingPacketTraceStream));
    }
    /******************* END PC5-S messages tracing *****************************/

    Ptr<OutputStreamWrapper> RelayNasRxPacketTraceStream =
        ascii.CreateFileStream("NrSlRelayNasRxPacketTrace.txt");
    *RelayNasRxPacketTraceStream->GetStream()
        << "time(s)\tnodeIp\tsrcIp\tdstIp\tsrcLink\tdstLink" << std::endl;
    for (uint32_t i = 0; i < relayUeNetDev.GetN(); ++i)
    {
        Ptr<NrEpcUeNas> epcUeNas = relayUeNetDev.Get(i)->GetObject<NrUeNetDevice>()->GetNas();

        epcUeNas->TraceConnectWithoutContext(
            "NrSlRelayRxPacketTrace",
            MakeBoundCallback(&TraceSinkRelayNasRxPacketTrace, RelayNasRxPacketTraceStream));
    }

    /************ SL traces database setup *************************************/
    std::string exampleName = simTag + "-" + "nr-prose-unicast-l3-relay";
    SQLiteOutput db(outputDir + exampleName + "-SlTraces.db");

    UeMacPscchTxOutputStats pscchStats;
    pscchStats.SetDb(&db, "pscchTxUeMac");
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
                                  "ComponentCarrierMapUe/*/NrUeMac/SlPscchScheduling",
                                  MakeBoundCallback(&NotifySlPscchScheduling, &pscchStats));

    UeMacPsschTxOutputStats psschStats;
    psschStats.SetDb(&db, "psschTxUeMac");
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/"
                                  "ComponentCarrierMapUe/*/NrUeMac/SlPsschScheduling",
                                  MakeBoundCallback(&NotifySlPsschScheduling, &psschStats));

    UePhyPscchRxOutputStats pscchPhyStats;
    pscchPhyStats.SetDb(&db, "pscchRxUePhy");
    Config::ConnectWithoutContext(
        "/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/NrUePhy/"
        "SpectrumPhy/RxPscchTraceUe",
        MakeBoundCallback(&NotifySlPscchRx, &pscchPhyStats));

    UePhyPsschRxOutputStats psschPhyStats;
    psschPhyStats.SetDb(&db, "psschRxUePhy");
    Config::ConnectWithoutContext(
        "/NodeList/*/DeviceList/*/$ns3::NrUeNetDevice/ComponentCarrierMapUe/*/NrUePhy/"
        "SpectrumPhy/RxPsschTraceUe",
        MakeBoundCallback(&NotifySlPsschRx, &psschPhyStats));

    UeToUePktTxRxOutputStats pktStats;
    pktStats.SetDb(&db, "pktTxRx");

    for (uint16_t ac = 0; ac < slClientApps.GetN(); ac++)
    {
        Ipv4Address localAddrs = slClientApps.Get(ac)
                                     ->GetNode()
                                     ->GetObject<Ipv4L3Protocol>()
                                     ->GetAddress(1, 0)
                                     .GetLocal();
        slClientApps.Get(ac)->TraceConnect("TxWithSeqTsSize",
                                           "tx",
                                           MakeBoundCallback(&UePacketTraceDb,
                                                             &pktStats,
                                                             slClientApps.Get(ac)->GetNode(),
                                                             localAddrs));
    }

    for (uint16_t ac = 0; ac < slServerApps.GetN(); ac++)
    {
        Ipv4Address localAddrs = slServerApps.Get(ac)
                                     ->GetNode()
                                     ->GetObject<Ipv4L3Protocol>()
                                     ->GetAddress(1, 0)
                                     .GetLocal();
        slServerApps.Get(ac)->TraceConnect("RxWithSeqTsSize",
                                           "rx",
                                           MakeBoundCallback(&UePacketTraceDb,
                                                             &pktStats,
                                                             slServerApps.Get(ac)->GetNode(),
                                                             localAddrs));
    }
    /************ END SL traces database setup *************************************/

    // Configure FlowMonitor to get traffic flow statistics
    FlowMonitorHelper flowmonHelper;
    NodeContainer endpointNodes;
    endpointNodes.Add(remoteHost);
    endpointNodes.Add(inNetUeNodes);
    endpointNodes.Add(slUeNodes);
    endpointNodes.Add(relayUeNodes);

    Ptr<ns3::FlowMonitor> monitor = flowmonHelper.Install(endpointNodes);
    monitor->SetAttribute("DelayBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("JitterBinWidth", DoubleValue(0.001));
    monitor->SetAttribute("PacketSizeBinWidth", DoubleValue(20));

    // Run simulation
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    // SL database dump
    pktStats.EmptyCache();
    pscchStats.EmptyCache();
    psschStats.EmptyCache();
    pscchPhyStats.EmptyCache();
    psschPhyStats.EmptyCache();

    // Print per-flow statistics
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowmonHelper.GetClassifier());
    FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();

    std::ofstream outFile;
    std::string filename = outputDir + "/" + exampleName + "-flowMonitorOutput.txt";
    outFile.open(filename.c_str(), std::ofstream::out | std::ofstream::trunc);
    if (!outFile.is_open())
    {
        std::cerr << "Can't open file " << filename << std::endl;
        return 1;
    }

    outFile.setf(std::ios_base::fixed);

    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin();
         i != stats.end();
         ++i)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        std::stringstream protoStream;
        protoStream << (uint16_t)t.protocol;
        if (t.protocol == 6)
        {
            protoStream.str("TCP");
        }
        if (t.protocol == 17)
        {
            protoStream.str("UDP");
        }

        double appDuration = 0;
        if (t.destinationPort == 8000) // SL
        {
            appDuration =
                simTime - slTrafficStartTime.GetSeconds(); // Some inaccuracy is expected due to
                                                           // randomization of start time.
        }
        else
        {
            appDuration = simTime - inNetTrafficStartTime;
        }

        outFile << "  Flow " << i->first << " (" << t.sourceAddress << " -> "
                << t.destinationAddress << ") " << protoStream.str() << "\n";
        outFile << "    Tx Packets: " << i->second.txPackets << "\n";
        outFile << "    Tx Bytes:   " << i->second.txBytes << "\n";
        outFile << "    TxOffered:  " << i->second.txBytes * 8.0 / appDuration / 1000 / 1000
                << " Mbps\n";
        outFile << "    Rx Packets: " << i->second.rxPackets << "\n";
        outFile << "    Rx Bytes:   " << i->second.rxBytes << "\n";
        if (i->second.rxPackets > 0)
        {
            outFile << "    Throughput: " << i->second.rxBytes * 8.0 / appDuration / 1000 / 1000
                    << " Mbps\n";
            outFile << "    Mean delay:  "
                    << 1000 * i->second.delaySum.GetSeconds() / i->second.rxPackets << " ms\n";
            outFile << "    Mean jitter:  "
                    << 1000 * i->second.jitterSum.GetSeconds() / i->second.rxPackets << " ms\n";
        }
        else
        {
            outFile << "    Throughput:  0 Mbps\n";
            outFile << "    Mean delay:  0 ms\n";
            outFile << "    Mean jitter: 0 ms\n";
        }
    }
    outFile.close();

    std::cout << "Simulation done!" << std::endl << "Traffic flows statistics: " << std::endl;
    std::ifstream f(filename.c_str());
    if (f.is_open())
    {
        std::cout << f.rdbuf();
    }
    std::cout << "Number of packets relayed by the L3 UE-to-Network relays:" << std::endl;
    std::cout << "relayIp      srcIp->dstIp      srcLink->dstLink\t\tnPackets" << std::endl;
    for (auto it = g_relayNasPacketCounter.begin(); it != g_relayNasPacketCounter.end(); ++it)
    {
        std::cout << it->first << "\t\t" << it->second << std::endl;
    }

    Simulator::Destroy();
    return 0;
}

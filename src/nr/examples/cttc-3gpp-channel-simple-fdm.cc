// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

/**
 *
 * \file cttc-3gpp-channel-simple-fdm.cc
 * \ingroup examples
 * \brief Simple frequency division multiplexing example.
 *
 * This example describes how to setup a simple simulation with the frequency
 * division multiplexing. Simulation example allows configuration of the two
 * bandwidth parts where each is dedicated to different traffic type.
 * The topology is a simple topology that consists of 1 UE and 1 gNB. There
 * is one data bearer active and it will be multiplexed over a one of
 * the two bandwidth parts depending on whether the traffic is configured to
 * be low latency or not. By default the traffic is low latency. So,
 * the example can be run from the command line in the following way:
 *
 * ./ns3 run cttc-3gpp-channel-simple-fdm
 *
 * or to configure flow as not ultra low latency:
 *
 * ./ns3 run 'cttc-3gpp-channel-simple-fdm --isUll=0'
 *
 * Variables that are accessible through the command line (e.g. numerology of
 * BWP 1 can be configured by using --numerologyBwp1=4, so if the user would
 * like to specify this parameter the program can be run in the following way:
 *
 * ./ns3 run "cttc-3gpp-channel-simple-fdm --numerologyBwp1=4"
 *
 *
 *
 * The configured spectrum division is as follows:
 *
 * -----------------------------Band 1---------------------------------
 * -----------------------------CC1------------------------------------
 * ------------BWP1---------------|--------------BWP2------------------
 *
 */

#include "ns3/antenna-module.h"
#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-eps-bearer-tag.h"
#include "ns3/nr-helper.h"
#include "ns3/nr-module.h"
#include "ns3/nr-point-to-point-epc-helper.h"
#include "ns3/three-gpp-spectrum-propagation-loss-model.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Cttc3gppChannelSimpleFdm");

static int g_rlcTraceCallbackCalled =
    false; //!< Global variable used to check if the callback function for RLC is called and thus to
           //!< determine if the example is run correctly or not
static int g_pdcpTraceCallbackCalled =
    false; //!< Global variable used to check if the callback function for PDCP is called and thus
           //!< to determine if the example is run correctly or not

/**
 * Function creates a single packet and directly calls the function send
 * of a device to send the packet to the destination address.
 * @param device Device that will send the packet to the destination address.
 * @param addr Destination address for a packet.
 * @param packetSize The packet size.
 */
static void
SendPacket(Ptr<NetDevice> device, Address& addr, uint32_t packetSize)
{
    Ptr<Packet> pkt = Create<Packet>(packetSize);
    // Adding empty IPV4 header after adding the IPV6 support for NR module.
    // NrNetDevice::Receive need to peek the header to know the IP protocol.
    // Since, there are no apps install in this test, this packet will be
    // dropped in Ipv4L3Protocol::Receive method upon not finding the route.
    Ipv4Header ipHeader;
    pkt->AddHeader(ipHeader);

    // the dedicated bearer that we activate in the simulation
    // will have bearerId = 2
    NrEpsBearerTag tag(1, 2);
    pkt->AddPacketTag(tag);
    device->Send(pkt, addr, Ipv4L3Protocol::PROT_NUMBER);
}

/**
 * Function that prints out PDCP delay. This function is designed as a callback
 * for PDCP trace source.
 * @param path The path that matches the trace source
 * @param rnti RNTI of UE
 * @param lcid logical channel id
 * @param bytes PDCP PDU size in bytes
 * @param pdcpDelay PDCP delay
 */
void
RxPdcpPDU(std::string path, uint16_t rnti, uint8_t lcid, uint32_t bytes, uint64_t pdcpDelay)
{
    std::cout << "\n Packet PDCP delay:" << pdcpDelay << "\n";
    g_pdcpTraceCallbackCalled = true;
}

/**
 * Function that prints out RLC statistics, such as RNTI, lcId, RLC PDU size,
 * delay. This function is designed as a callback
 * for RLC trace source.
 * @param path The path that matches the trace source
 * @param rnti RNTI of UE
 * @param lcid logical channel id
 * @param bytes RLC PDU size in bytes
 * @param rlcDelay RLC PDU delay
 */
void
RxRlcPDU(std::string path, uint16_t rnti, uint8_t lcid, uint32_t bytes, uint64_t rlcDelay)
{
    std::cout << "\n\n Data received by UE RLC at:" << Simulator::Now() << std::endl;
    std::cout << "\n rnti:" << rnti << std::endl;
    std::cout << "\n lcid:" << (unsigned)lcid << std::endl;
    std::cout << "\n bytes :" << bytes << std::endl;
    std::cout << "\n delay :" << rlcDelay << std::endl;
    g_rlcTraceCallbackCalled = true;
}

/**
 * Function that connects PDCP and RLC traces to the corresponding trace sources.
 */
void
ConnectPdcpRlcTraces()
{
    // after recent changes in the EPC UE node ID has changed to 3
    // dedicated bearer that we have activated has bearer id 2
    Config::Connect("/NodeList/*/DeviceList/*/NrUeRrc/DataRadioBearerMap/*/NrPdcp/RxPDU",
                    MakeCallback(&RxPdcpPDU));
    // after recent changes in the EPC UE node ID has changed to 3
    // dedicated bearer that we have activated has bearer id 2
    Config::Connect("/NodeList/*/DeviceList/*/NrUeRrc/DataRadioBearerMap/*/NrRlc/RxPDU",
                    MakeCallback(&RxRlcPDU));
}

int
main(int argc, char* argv[])
{
    uint16_t gNbNum = 1;
    uint16_t ueNumPergNb = 1;
    uint16_t numerologyBwp1 = 4;
    uint16_t numerologyBwp2 = 2;
    double centralFrequencyBand = 28.1e9;
    double bandwidthBand = 200e6;
    double txPowerPerBwp = 4;
    uint32_t packetSize = 1000;
    bool isUll = true; // Whether the flow is a low latency type of traffic.

    Time sendPacketTime = Seconds(0.4);

    CommandLine cmd(__FILE__);
    cmd.AddValue("gNbNum", "The number of gNbs in multiple-ue topology", gNbNum);
    cmd.AddValue("ueNumPergNb", "The number of UE per gNb in multiple-ue topology", ueNumPergNb);
    cmd.AddValue("numerologyBwp1", "The numerology to be used in bandwidth part 1", numerologyBwp1);
    cmd.AddValue("numerologyBwp2", "The numerology to be used in bandwidth part 2", numerologyBwp2);
    cmd.AddValue("frequency", "The system frequency", centralFrequencyBand);
    cmd.AddValue("bandwidthBand", "The system bandwidth", bandwidthBand);
    cmd.AddValue("packetSize", "packet size in bytes", packetSize);
    cmd.AddValue("isUll", "Enable Uplink", isUll);
    cmd.Parse(argc, argv);

    int64_t randomStream = 1;
    // Create the scenario
    GridScenarioHelper gridScenario;
    gridScenario.SetRows(1);
    gridScenario.SetColumns(gNbNum);
    gridScenario.SetHorizontalBsDistance(5.0);
    gridScenario.SetBsHeight(10.0);
    gridScenario.SetUtHeight(1.5);
    // must be set before BS number
    gridScenario.SetSectorization(GridScenarioHelper::SINGLE);
    gridScenario.SetBsNumber(gNbNum);
    gridScenario.SetUtNumber(ueNumPergNb * gNbNum);
    gridScenario.SetScenarioHeight(3); // Create a 3x3 scenario where the UE will
    gridScenario.SetScenarioLength(3); // be distributed.
    randomStream += gridScenario.AssignStreams(randomStream);
    gridScenario.CreateScenario();

    Config::SetDefault("ns3::NrEpsBearer::Release", UintegerValue(15));

    Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();

    nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    nrHelper->SetEpcHelper(nrEpcHelper);
    // Set the spectrum channel using UMi scenario, default channel condition and 3GPP channel model
    channelHelper->ConfigureFactories("UMi", "Default", "ThreeGpp");
    // Disable shadowing
    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));
    // Create one operational band containing one CC with 2 bandwidth parts
    BandwidthPartInfoPtrVector allBwps;
    CcBwpCreator ccBwpCreator;
    const uint8_t numCcPerBand = 1; // one CC per Band

    // Create the configuration for the CcBwpHelper
    CcBwpCreator::SimpleOperationBandConf bandConf(centralFrequencyBand,
                                                   bandwidthBand,
                                                   numCcPerBand);
    bandConf.m_numBwp = 2; // two BWPs per CC

    // By using the configuration created, it is time to make the operation band
    OperationBandInfo band = ccBwpCreator.CreateOperationBandContiguousCc(bandConf);
    // Set and create channel for this band
    channelHelper->AssignChannelsToBands({band});
    allBwps = CcBwpCreator::GetAllBwps({band});

    // Beamforming method
    idealBeamformingHelper->SetAttribute("BeamformingMethod",
                                         TypeIdValue(DirectPathBeamforming::GetTypeId()));

    // Antennas for all the UEs
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(2));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(4));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));

    // Antennas for all the gNbs
    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(4));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(8));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     PointerValue(CreateObject<IsotropicAntennaModel>()));

    uint32_t bwpIdForLowLat = 0;
    uint32_t bwpIdForVoice = 1;

    // gNb routing between Bearer and bandwidh part
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB",
                                                 UintegerValue(bwpIdForLowLat));
    nrHelper->SetGnbBwpManagerAlgorithmAttribute("GBR_CONV_VOICE", UintegerValue(bwpIdForVoice));

    // Ue routing between Bearer and bandwidth part
    nrHelper->SetUeBwpManagerAlgorithmAttribute("NGBR_LOW_LAT_EMBB", UintegerValue(bwpIdForLowLat));
    nrHelper->SetUeBwpManagerAlgorithmAttribute("GBR_CONV_VOICE", UintegerValue(bwpIdForVoice));

    // Install and get the pointers to the NetDevices
    NetDeviceContainer gnbNetDev =
        nrHelper->InstallGnbDevice(gridScenario.GetBaseStations(), allBwps);
    NetDeviceContainer ueNetDev =
        nrHelper->InstallUeDevice(gridScenario.GetUserTerminals(), allBwps);

    randomStream += nrHelper->AssignStreams(gnbNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueNetDev, randomStream);

    // Set the attribute of the netdevice (gnbNetDev.Get (0)) and bandwidth part (0)/(1)
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)
        ->SetAttribute("Numerology", UintegerValue(numerologyBwp1));
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 1)
        ->SetAttribute("Numerology", UintegerValue(numerologyBwp2));
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)->SetTxPower(txPowerPerBwp);
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 1)->SetTxPower(txPowerPerBwp);

    InternetStackHelper internet;
    internet.Install(gridScenario.GetUserTerminals());
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueNetDev));

    Simulator::Schedule(sendPacketTime,
                        &SendPacket,
                        gnbNetDev.Get(0),
                        ueNetDev.Get(0)->GetAddress(),
                        packetSize);

    // attach UEs to the closest gNB
    nrHelper->AttachToClosestGnb(ueNetDev, gnbNetDev);

    Ptr<NrEpcTft> tft = Create<NrEpcTft>();
    NrEpcTft::PacketFilter dlpf;
    dlpf.localPortStart = 1234;
    dlpf.localPortEnd = 1235;
    tft->Add(dlpf);
    enum NrEpsBearer::Qci q;

    if (isUll)
    {
        q = NrEpsBearer::NGBR_LOW_LAT_EMBB;
    }
    else
    {
        q = NrEpsBearer::GBR_CONV_VOICE;
    }

    NrEpsBearer bearer(q);
    nrHelper->ActivateDedicatedEpsBearer(ueNetDev, bearer, tft);

    Simulator::Schedule(Seconds(0.2), &ConnectPdcpRlcTraces);

    nrHelper->EnableTraces();

    Simulator::Stop(Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    if (g_rlcTraceCallbackCalled && g_pdcpTraceCallbackCalled)
    {
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

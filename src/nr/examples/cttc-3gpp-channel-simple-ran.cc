// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

/**
 * \ingroup examples
 * \file cttc-3gpp-channel-simple-ran.cc
 * \brief Simple RAN
 *
 * This example describes how to setup a simulation using the 3GPP channel model
 * from TR 38.901. This example consists of a simple topology of 1 UE and 1 gNb,
 * and only NR RAN part is simulated. One Bandwidth part and one CC are defined.
 * A packet is created and directly sent to gNb device by SendPacket function.
 * Then several functions are connected to PDCP and RLC traces and the delay is
 * printed.
 */

#include "ns3/antenna-module.h"
#include "ns3/config-store.h"
#include "ns3/core-module.h"
#include "ns3/grid-scenario-helper.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/log.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/nr-eps-bearer-tag.h"
#include "ns3/nr-helper.h"
#include "ns3/nr-module.h"
#include "ns3/nr-point-to-point-epc-helper.h"

using namespace ns3;

/*
 * Enable the logs of the file by enabling the component "Cttc3gppChannelSimpleRan",
 * in this way:
 * $ export NS_LOG="Cttc3gppChannelSimpleRan=level_info|prefix_func|prefix_time"
 */
NS_LOG_COMPONENT_DEFINE("Cttc3gppChannelSimpleRan");

static bool g_rxPdcpCallbackCalled = false;
static bool g_rxRxRlcPDUCallbackCalled = false;

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
    Ipv4Header ipv4Header;
    ipv4Header.SetProtocol(UdpL4Protocol::PROT_NUMBER);
    pkt->AddHeader(ipv4Header);
    NrEpsBearerTag tag(1, 1);
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
    g_rxPdcpCallbackCalled = true;
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
    std::cout << "\n\n Data received at RLC layer at:" << Simulator::Now() << std::endl;
    std::cout << "\n rnti:" << rnti << std::endl;
    std::cout << "\n lcid:" << (unsigned)lcid << std::endl;
    std::cout << "\n bytes :" << bytes << std::endl;
    std::cout << "\n delay :" << rlcDelay << std::endl;
    g_rxRxRlcPDUCallbackCalled = true;
}

/**
 * Function that connects PDCP and RLC traces to the corresponding trace sources.
 */
void
ConnectPdcpRlcTraces()
{
    Config::Connect("/NodeList/*/DeviceList/*/NrUeRrc/DataRadioBearerMap/1/NrPdcp/RxPDU",
                    MakeCallback(&RxPdcpPDU));

    Config::Connect("/NodeList/*/DeviceList/*/NrUeRrc/DataRadioBearerMap/1/NrRlc/RxPDU",
                    MakeCallback(&RxRlcPDU));
}

/**
 * Function that connects UL PDCP and RLC traces to the corresponding trace sources.
 */
void
ConnectUlPdcpRlcTraces()
{
    Config::Connect("/NodeList/*/DeviceList/*/NrGnbRrc/UeMap/*/DataRadioBearerMap/*/NrPdcp/RxPDU",
                    MakeCallback(&RxPdcpPDU));

    Config::Connect("/NodeList/*/DeviceList/*/NrGnbRrc/UeMap/*/DataRadioBearerMap/*/NrRlc/RxPDU",
                    MakeCallback(&RxRlcPDU));
}

int
main(int argc, char* argv[])
{
    uint16_t numerologyBwp1 = 0;
    uint32_t udpPacketSize = 1000;
    double centralFrequencyBand1 = 28e9;
    double bandwidthBand1 = 400e6;
    uint16_t gNbNum = 1;
    uint16_t ueNumPergNb = 1;
    bool enableUl = false;

    Time sendPacketTime = Seconds(0.4);

    CommandLine cmd(__FILE__);
    cmd.AddValue("numerologyBwp1", "The numerology to be used in bandwidth part 1", numerologyBwp1);
    cmd.AddValue("centralFrequencyBand1",
                 "The system frequency to be used in band 1",
                 centralFrequencyBand1);
    cmd.AddValue("bandwidthBand1", "The system bandwidth to be used in band 1", bandwidthBand1);
    cmd.AddValue("packetSize", "packet size in bytes", udpPacketSize);
    cmd.AddValue("enableUl", "Enable Uplink", enableUl);
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

    Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<IdealBeamformingHelper> idealBeamformingHelper = CreateObject<IdealBeamformingHelper>();
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    Ptr<NrChannelHelper> channelHelper = CreateObject<NrChannelHelper>();

    nrHelper->SetBeamformingHelper(idealBeamformingHelper);
    nrHelper->SetEpcHelper(nrEpcHelper);
    // Configure the spectrum channel
    channelHelper->ConfigureFactories("UMi", "Default", "ThreeGpp");
    Config::SetDefault("ns3::ThreeGppChannelModel::UpdatePeriod", TimeValue(MilliSeconds(0)));
    channelHelper->SetChannelConditionModelAttribute("UpdatePeriod", TimeValue(MilliSeconds(0)));
    channelHelper->SetPathlossAttribute("ShadowingEnabled", BooleanValue(false));

    // Create one operational band containing one CC with one bandwidth part
    BandwidthPartInfoPtrVector allBwps;
    CcBwpCreator ccBwpCreator;
    const uint8_t numCcPerBand = 1;

    // Create the configuration for the CcBwpHelper
    CcBwpCreator::SimpleOperationBandConf bandConf1(centralFrequencyBand1,
                                                    bandwidthBand1,
                                                    numCcPerBand);

    // By using the configuration created, it is time to make the operation band
    OperationBandInfo band1 = ccBwpCreator.CreateOperationBandContiguousCc(bandConf1);
    // Set and create the channel
    channelHelper->AssignChannelsToBands({band1});
    allBwps = CcBwpCreator::GetAllBwps({band1});

    nrHelper->SetSchedulerAttribute("FixedMcsDl", BooleanValue(true));
    nrHelper->SetSchedulerAttribute("StartingMcsDl", UintegerValue(28));

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

    // Install and get the pointers to the NetDevices
    NetDeviceContainer gnbNetDev =
        nrHelper->InstallGnbDevice(gridScenario.GetBaseStations(), allBwps);
    NetDeviceContainer ueNetDev =
        nrHelper->InstallUeDevice(gridScenario.GetUserTerminals(), allBwps);

    randomStream += nrHelper->AssignStreams(gnbNetDev, randomStream);
    randomStream += nrHelper->AssignStreams(ueNetDev, randomStream);

    // Set the attribute of the netdevice (gnbNetDev.Get (0)) and bandwidth part (0)
    nrHelper->GetGnbPhy(gnbNetDev.Get(0), 0)
        ->SetAttribute("Numerology", UintegerValue(numerologyBwp1));

    InternetStackHelper internet;
    internet.Install(gridScenario.GetUserTerminals());
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = nrEpcHelper->AssignUeIpv4Address(NetDeviceContainer(ueNetDev));

    if (enableUl)
    {
        Simulator::Schedule(sendPacketTime,
                            &SendPacket,
                            ueNetDev.Get(0),
                            gnbNetDev.Get(0)->GetAddress(),
                            udpPacketSize);
    }
    else
    {
        Simulator::Schedule(sendPacketTime,
                            &SendPacket,
                            gnbNetDev.Get(0),
                            ueNetDev.Get(0)->GetAddress(),
                            udpPacketSize);
    }

    // attach UEs to the closest gNB
    nrHelper->AttachToClosestGnb(ueNetDev, gnbNetDev);

    if (enableUl)
    {
        std::cout << "\n Sending data in uplink." << std::endl;
        Simulator::Schedule(Seconds(0.2), &ConnectUlPdcpRlcTraces);
    }
    else
    {
        std::cout << "\n Sending data in downlink." << std::endl;
        Simulator::Schedule(Seconds(0.2), &ConnectPdcpRlcTraces);
    }

    nrHelper->EnableTraces();

    Simulator::Stop(Seconds(1));
    Simulator::Run();
    Simulator::Destroy();

    if (g_rxPdcpCallbackCalled && g_rxRxRlcPDUCallbackCalled)
    {
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

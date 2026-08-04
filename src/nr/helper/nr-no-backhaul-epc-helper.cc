// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only
//
// Author: Manuel Requena <manuel.requena@cttc.es>
//         (based on the original point-to-point-epc-helper.cc)

#include "nr-no-backhaul-epc-helper.h"

#include "ns3/boolean.h"
#include "ns3/icmpv6-l4-protocol.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/log.h"
#include "ns3/nr-epc-gnb-application.h"
#include "ns3/nr-epc-mme-application.h"
#include "ns3/nr-epc-pgw-application.h"
#include "ns3/nr-epc-sgw-application.h"
#include "ns3/nr-epc-ue-nas.h"
#include "ns3/nr-epc-x2.h"
#include "ns3/nr-gnb-net-device.h"
#include "ns3/nr-gnb-rrc.h"
#include "ns3/nr-ue-net-device.h"
#include "ns3/packet-socket-address.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/string.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrNoBackhaulEpcHelper");

NS_OBJECT_ENSURE_REGISTERED(NrNoBackhaulEpcHelper);

NrNoBackhaulEpcHelper::NrNoBackhaulEpcHelper()
    : m_gtpuUdpPort(2152), // fixed by the standard
      m_s11LinkDataRate(DataRate("10Gb/s")),
      m_s11LinkDelay(Seconds(0)),
      m_s11LinkMtu(3000),
      m_gtpcUdpPort(2123), // fixed by the standard
      m_s5LinkDataRate(DataRate("10Gb/s")),
      m_s5LinkDelay(Seconds(0)),
      m_s5LinkMtu(3000)
{
    NS_LOG_FUNCTION(this);
    // To access the attribute value within the constructor
    ObjectBase::ConstructSelf(AttributeConstructionList());

    int retval;

    // since we use point-to-point links for links between the core network nodes,
    // we use a /30 subnet which can hold exactly two addresses
    // (remember that net broadcast and null address are not valid)
    m_x2Ipv4AddressHelper.SetBase("12.0.0.0", "255.255.255.252");
    m_s11Ipv4AddressHelper.SetBase("13.0.0.0", "255.255.255.252");
    m_s5Ipv4AddressHelper.SetBase("14.0.0.0", "255.255.255.252");

    // we use a /8 net for all UEs
    m_uePgwAddressHelper.SetBase("7.0.0.0", "255.0.0.0");

    // we use a /64 IPv6 net all UEs
    m_uePgwAddressHelper6.SetBase("7777:f00d::", Ipv6Prefix(64));

    // Create PGW, SGW and MME nodes
    m_pgw = CreateObject<Node>();
    m_sgw = CreateObject<Node>();
    m_mme = CreateObject<Node>();
    InternetStackHelper internet;
    internet.Install(m_pgw);
    internet.Install(m_sgw);
    internet.Install(m_mme);

    // The Tun device resides in different 64 bit subnet.
    // We must create an unique route to tun device for all the packets destined
    // to all 64 bit IPv6 prefixes of UEs, based by the unique 48 bit network prefix of this EPC
    // network
    Ipv6StaticRoutingHelper ipv6RoutingHelper;
    Ptr<Ipv6StaticRouting> pgwStaticRouting =
        ipv6RoutingHelper.GetStaticRouting(m_pgw->GetObject<Ipv6>());
    pgwStaticRouting->AddNetworkRouteTo("7777:f00d::", Ipv6Prefix(64), Ipv6Address("::"), 1, 0);

    // create TUN device implementing tunneling of user data over GTP-U/UDP/IP in the PGW
    m_tunDevice = CreateObject<VirtualNetDevice>();

    // allow jumbo packets
    m_tunDevice->SetAttribute("Mtu", UintegerValue(30000));

    // yes we need this
    m_tunDevice->SetAddress(Mac48Address::Allocate());

    m_pgw->AddDevice(m_tunDevice);
    NetDeviceContainer tunDeviceContainer;
    tunDeviceContainer.Add(m_tunDevice);
    // the TUN device is on the same subnet as the UEs, so when a packet
    // addressed to an UE arrives at the internet to the WAN interface of
    // the PGW it will be forwarded to the TUN device.
    Ipv4InterfaceContainer tunDeviceIpv4IfContainer = AssignUeIpv4Address(tunDeviceContainer);

    // the TUN device for IPv6 address is on the different subnet as the
    // UEs, it will forward the UE packets as we have inserted the route
    // for all UEs at the time of assigning UE addresses
    Ipv6InterfaceContainer tunDeviceIpv6IfContainer = AssignUeIpv6Address(tunDeviceContainer);

    // Set Forwarding of the IPv6 interface
    tunDeviceIpv6IfContainer.SetForwarding(0, true);
    tunDeviceIpv6IfContainer.SetDefaultRouteInAllNodes(0);

    // Create S5 link between PGW and SGW
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(m_s5LinkDataRate));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(m_s5LinkMtu));
    p2ph.SetChannelAttribute("Delay", TimeValue(m_s5LinkDelay));
    NetDeviceContainer pgwSgwDevices = p2ph.Install(m_pgw, m_sgw);
    NS_LOG_LOGIC("IPv4 ifaces of the PGW after installing p2p dev: "
                 << m_pgw->GetObject<Ipv4>()->GetNInterfaces());
    NS_LOG_LOGIC("IPv4 ifaces of the SGW after installing p2p dev: "
                 << m_sgw->GetObject<Ipv4>()->GetNInterfaces());
    Ptr<NetDevice> pgwDev = pgwSgwDevices.Get(0);
    Ptr<NetDevice> sgwDev = pgwSgwDevices.Get(1);
    m_s5Ipv4AddressHelper.NewNetwork();
    Ipv4InterfaceContainer pgwSgwIpIfaces = m_s5Ipv4AddressHelper.Assign(pgwSgwDevices);
    NS_LOG_LOGIC("IPv4 ifaces of the PGW after assigning Ipv4 addr to S5 dev: "
                 << m_pgw->GetObject<Ipv4>()->GetNInterfaces());
    NS_LOG_LOGIC("IPv4 ifaces of the SGW after assigning Ipv4 addr to S5 dev: "
                 << m_sgw->GetObject<Ipv4>()->GetNInterfaces());

    Ipv4Address pgwS5Address = pgwSgwIpIfaces.GetAddress(0);
    Ipv4Address sgwS5Address = pgwSgwIpIfaces.GetAddress(1);

    // Create S5-U socket in the PGW
    Ptr<Socket> pgwS5uSocket =
        Socket::CreateSocket(m_pgw, TypeId::LookupByName("ns3::UdpSocketFactory"));
    retval = pgwS5uSocket->Bind(InetSocketAddress(pgwS5Address, m_gtpuUdpPort));
    NS_ASSERT(retval == 0);

    // Create S5-C socket in the PGW
    Ptr<Socket> pgwS5cSocket =
        Socket::CreateSocket(m_pgw, TypeId::LookupByName("ns3::UdpSocketFactory"));
    retval = pgwS5cSocket->Bind(InetSocketAddress(pgwS5Address, m_gtpcUdpPort));
    NS_ASSERT(retval == 0);

    // Create NrEpcPgwApplication
    m_pgwApp =
        CreateObject<NrEpcPgwApplication>(m_tunDevice, pgwS5Address, pgwS5uSocket, pgwS5cSocket);
    m_pgw->AddApplication(m_pgwApp);

    // Connect NrEpcPgwApplication and virtual net device for tunneling
    m_tunDevice->SetSendCallback(MakeCallback(&NrEpcPgwApplication::RecvFromTunDevice, m_pgwApp));

    // Create S5-U socket in the SGW
    Ptr<Socket> sgwS5uSocket =
        Socket::CreateSocket(m_sgw, TypeId::LookupByName("ns3::UdpSocketFactory"));
    retval = sgwS5uSocket->Bind(InetSocketAddress(sgwS5Address, m_gtpuUdpPort));
    NS_ASSERT(retval == 0);

    // Create S5-C socket in the SGW
    Ptr<Socket> sgwS5cSocket =
        Socket::CreateSocket(m_sgw, TypeId::LookupByName("ns3::UdpSocketFactory"));
    retval = sgwS5cSocket->Bind(InetSocketAddress(sgwS5Address, m_gtpcUdpPort));
    NS_ASSERT(retval == 0);

    // Create S1-U socket in the SGW
    Ptr<Socket> sgwS1uSocket =
        Socket::CreateSocket(m_sgw, TypeId::LookupByName("ns3::UdpSocketFactory"));
    retval = sgwS1uSocket->Bind(InetSocketAddress(Ipv4Address::GetAny(), m_gtpuUdpPort));
    NS_ASSERT(retval == 0);

    // Create NrEpcSgwApplication
    m_sgwApp =
        CreateObject<NrEpcSgwApplication>(sgwS1uSocket, sgwS5Address, sgwS5uSocket, sgwS5cSocket);
    m_sgw->AddApplication(m_sgwApp);
    m_sgwApp->AddPgw(pgwS5Address);
    m_pgwApp->AddSgw(sgwS5Address);

    // Create S11 link between MME and SGW
    PointToPointHelper s11P2ph;
    s11P2ph.SetDeviceAttribute("DataRate", DataRateValue(m_s11LinkDataRate));
    s11P2ph.SetDeviceAttribute("Mtu", UintegerValue(m_s11LinkMtu));
    s11P2ph.SetChannelAttribute("Delay", TimeValue(m_s11LinkDelay));
    NetDeviceContainer mmeSgwDevices = s11P2ph.Install(m_mme, m_sgw);
    NS_LOG_LOGIC("MME's IPv4 ifaces after installing p2p dev: "
                 << m_mme->GetObject<Ipv4>()->GetNInterfaces());
    NS_LOG_LOGIC("SGW's IPv4 ifaces after installing p2p dev: "
                 << m_sgw->GetObject<Ipv4>()->GetNInterfaces());
    Ptr<NetDevice> mmeDev = mmeSgwDevices.Get(0);
    Ptr<NetDevice> sgwS11Dev = mmeSgwDevices.Get(1);
    m_s11Ipv4AddressHelper.NewNetwork();
    Ipv4InterfaceContainer mmeSgwIpIfaces = m_s11Ipv4AddressHelper.Assign(mmeSgwDevices);
    NS_LOG_LOGIC("MME's IPv4 ifaces after assigning Ipv4 addr to S11 dev: "
                 << m_mme->GetObject<Ipv4>()->GetNInterfaces());
    NS_LOG_LOGIC("SGW's IPv4 ifaces after assigning Ipv4 addr to S11 dev: "
                 << m_sgw->GetObject<Ipv4>()->GetNInterfaces());

    Ipv4Address mmeS11Address = mmeSgwIpIfaces.GetAddress(0);
    Ipv4Address sgwS11Address = mmeSgwIpIfaces.GetAddress(1);

    // Create S11 socket in the MME
    Ptr<Socket> mmeS11Socket =
        Socket::CreateSocket(m_mme, TypeId::LookupByName("ns3::UdpSocketFactory"));
    retval = mmeS11Socket->Bind(InetSocketAddress(mmeS11Address, m_gtpcUdpPort));
    NS_ASSERT(retval == 0);

    // Create S11 socket in the SGW
    Ptr<Socket> sgwS11Socket =
        Socket::CreateSocket(m_sgw, TypeId::LookupByName("ns3::UdpSocketFactory"));
    retval = sgwS11Socket->Bind(InetSocketAddress(sgwS11Address, m_gtpcUdpPort));
    NS_ASSERT(retval == 0);

    // Create MME Application and connect with SGW via S11 interface
    m_mmeApp = CreateObject<NrEpcMmeApplication>();
    m_mme->AddApplication(m_mmeApp);
    m_mmeApp->AddSgw(sgwS11Address, mmeS11Address, mmeS11Socket);
    m_sgwApp->AddMme(mmeS11Address, sgwS11Socket);
}

NrNoBackhaulEpcHelper::~NrNoBackhaulEpcHelper()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrNoBackhaulEpcHelper::GetTypeId()
{
    NS_LOG_FUNCTION_NOARGS();
    static TypeId tid =
        TypeId("ns3::NrNoBackhaulEpcHelper")
            .SetParent<NrEpcHelper>()
            .SetGroupName("Nr")
            .AddConstructor<NrNoBackhaulEpcHelper>()
            .AddAttribute("S5LinkDataRate",
                          "The data rate to be used for the next S5 link to be created",
                          DataRateValue(DataRate("10Gb/s")),
                          MakeDataRateAccessor(&NrNoBackhaulEpcHelper::m_s5LinkDataRate),
                          MakeDataRateChecker())
            .AddAttribute("S5LinkDelay",
                          "The delay to be used for the next S5 link to be created",
                          TimeValue(Seconds(0)),
                          MakeTimeAccessor(&NrNoBackhaulEpcHelper::m_s5LinkDelay),
                          MakeTimeChecker())
            .AddAttribute("S5LinkMtu",
                          "The MTU of the next S5 link to be created",
                          UintegerValue(2000),
                          MakeUintegerAccessor(&NrNoBackhaulEpcHelper::m_s5LinkMtu),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("S11LinkDataRate",
                          "The data rate to be used for the next S11 link to be created",
                          DataRateValue(DataRate("10Gb/s")),
                          MakeDataRateAccessor(&NrNoBackhaulEpcHelper::m_s11LinkDataRate),
                          MakeDataRateChecker())
            .AddAttribute("S11LinkDelay",
                          "The delay to be used for the next S11 link to be created",
                          TimeValue(Seconds(0)),
                          MakeTimeAccessor(&NrNoBackhaulEpcHelper::m_s11LinkDelay),
                          MakeTimeChecker())
            .AddAttribute("S11LinkMtu",
                          "The MTU of the next S11 link to be created.",
                          UintegerValue(2000),
                          MakeUintegerAccessor(&NrNoBackhaulEpcHelper::m_s11LinkMtu),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("X2LinkDataRate",
                          "The data rate to be used for the next X2 link to be created",
                          DataRateValue(DataRate("10Gb/s")),
                          MakeDataRateAccessor(&NrNoBackhaulEpcHelper::m_x2LinkDataRate),
                          MakeDataRateChecker())
            .AddAttribute("X2LinkDelay",
                          "The delay to be used for the next X2 link to be created",
                          TimeValue(Seconds(0)),
                          MakeTimeAccessor(&NrNoBackhaulEpcHelper::m_x2LinkDelay),
                          MakeTimeChecker())
            .AddAttribute("X2LinkMtu",
                          "The MTU of the next X2 link to be created. Note that, because of some "
                          "big X2 messages, you need a big MTU.",
                          UintegerValue(3000),
                          MakeUintegerAccessor(&NrNoBackhaulEpcHelper::m_x2LinkMtu),
                          MakeUintegerChecker<uint16_t>())
            .AddAttribute("X2LinkPcapPrefix",
                          "Prefix for Pcap generated by X2 link",
                          StringValue("x2"),
                          MakeStringAccessor(&NrNoBackhaulEpcHelper::m_x2LinkPcapPrefix),
                          MakeStringChecker())
            .AddAttribute("X2LinkEnablePcap",
                          "Enable Pcap for X2 link",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrNoBackhaulEpcHelper::m_x2LinkEnablePcap),
                          MakeBooleanChecker());
    return tid;
}

void
NrNoBackhaulEpcHelper::DoDispose()
{
    NS_LOG_FUNCTION(this);
    m_tunDevice->SetSendCallback(
        MakeNullCallback<bool, Ptr<Packet>, const Address&, const Address&, uint16_t>());
    m_tunDevice = nullptr;
    m_sgwApp = nullptr;
    m_sgw->Dispose();
    m_pgwApp = nullptr;
    m_pgw->Dispose();
    m_mmeApp = nullptr;
    m_mme->Dispose();
}

void
NrNoBackhaulEpcHelper::AddGnb(Ptr<Node> gnb,
                              Ptr<NetDevice> nrGnbNetDevice,
                              std::vector<uint16_t> cellIds)
{
    NS_LOG_FUNCTION(this << gnb << nrGnbNetDevice << cellIds.size());
    NS_ASSERT(gnb == nrGnbNetDevice->GetNode());

    int retval;

    // add an IPv4 stack to the previously created gNB
    InternetStackHelper internet;
    internet.Install(gnb);
    NS_LOG_LOGIC("number of Ipv4 ifaces of the gNB after node creation: "
                 << gnb->GetObject<Ipv4>()->GetNInterfaces());

    // create NR socket for the gNB
    Ptr<Socket> nrGnbSocket =
        Socket::CreateSocket(gnb, TypeId::LookupByName("ns3::PacketSocketFactory"));
    PacketSocketAddress nrGnbSocketBindAddress;
    nrGnbSocketBindAddress.SetSingleDevice(nrGnbNetDevice->GetIfIndex());
    nrGnbSocketBindAddress.SetProtocol(Ipv4L3Protocol::PROT_NUMBER);
    retval = nrGnbSocket->Bind(nrGnbSocketBindAddress);
    NS_ASSERT(retval == 0);
    PacketSocketAddress nrGnbSocketConnectAddress;
    nrGnbSocketConnectAddress.SetPhysicalAddress(Mac48Address::GetBroadcast());
    nrGnbSocketConnectAddress.SetSingleDevice(nrGnbNetDevice->GetIfIndex());
    nrGnbSocketConnectAddress.SetProtocol(Ipv4L3Protocol::PROT_NUMBER);
    retval = nrGnbSocket->Connect(nrGnbSocketConnectAddress);
    NS_ASSERT(retval == 0);

    // create NR socket for the gNB
    Ptr<Socket> nrGnbSocket6 =
        Socket::CreateSocket(gnb, TypeId::LookupByName("ns3::PacketSocketFactory"));
    PacketSocketAddress nrGnbSocketBindAddress6;
    nrGnbSocketBindAddress6.SetSingleDevice(nrGnbNetDevice->GetIfIndex());
    nrGnbSocketBindAddress6.SetProtocol(Ipv6L3Protocol::PROT_NUMBER);
    retval = nrGnbSocket6->Bind(nrGnbSocketBindAddress6);
    NS_ASSERT(retval == 0);
    PacketSocketAddress nrGnbSocketConnectAddress6;
    nrGnbSocketConnectAddress6.SetPhysicalAddress(Mac48Address::GetBroadcast());
    nrGnbSocketConnectAddress6.SetSingleDevice(nrGnbNetDevice->GetIfIndex());
    nrGnbSocketConnectAddress6.SetProtocol(Ipv6L3Protocol::PROT_NUMBER);
    retval = nrGnbSocket6->Connect(nrGnbSocketConnectAddress6);
    NS_ASSERT(retval == 0);

    NS_LOG_INFO("Create NrEpcGnbApplication for cell ID " << cellIds.at(0));
    Ptr<NrEpcGnbApplication> gnbApp =
        CreateObject<NrEpcGnbApplication>(nrGnbSocket, nrGnbSocket6, cellIds.at(0));
    gnb->AddApplication(gnbApp);
    NS_ASSERT(gnb->GetNApplications() == 1);
    NS_ASSERT_MSG(gnb->GetApplication(0)->GetObject<NrEpcGnbApplication>(),
                  "cannot retrieve NrEpcGnbApplication");
    NS_LOG_LOGIC("gnb: " << gnb << ", gnb->GetApplication (0): " << gnb->GetApplication(0));

    NS_LOG_INFO("Create NrEpcX2 entity");
    Ptr<NrEpcX2> x2 = CreateObject<NrEpcX2>();
    gnb->AggregateObject(x2);
}

void
NrNoBackhaulEpcHelper::AddX2Interface(Ptr<Node> gnb1, Ptr<Node> gnb2)
{
    NS_LOG_FUNCTION(this << gnb1 << gnb2);

    // Create a point to point link between the two eNBs with
    // the corresponding new NetDevices on each side
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", DataRateValue(m_x2LinkDataRate));
    p2ph.SetDeviceAttribute("Mtu", UintegerValue(m_x2LinkMtu));
    p2ph.SetChannelAttribute("Delay", TimeValue(m_x2LinkDelay));
    NetDeviceContainer gnbDevices = p2ph.Install(gnb1, gnb2);
    NS_LOG_LOGIC("number of Ipv4 ifaces of the gNB #1 after installing p2p dev: "
                 << gnb1->GetObject<Ipv4>()->GetNInterfaces());
    NS_LOG_LOGIC("number of Ipv4 ifaces of the gNB #2 after installing p2p dev: "
                 << gnb2->GetObject<Ipv4>()->GetNInterfaces());

    if (m_x2LinkEnablePcap)
    {
        p2ph.EnablePcapAll(m_x2LinkPcapPrefix);
    }

    m_x2Ipv4AddressHelper.NewNetwork();
    Ipv4InterfaceContainer gnbIpIfaces = m_x2Ipv4AddressHelper.Assign(gnbDevices);
    NS_LOG_LOGIC("number of Ipv4 ifaces of the gNB #1 after assigning Ipv4 addr to X2 dev: "
                 << gnb1->GetObject<Ipv4>()->GetNInterfaces());
    NS_LOG_LOGIC("number of Ipv4 ifaces of the gNB #2 after assigning Ipv4 addr to X2 dev: "
                 << gnb2->GetObject<Ipv4>()->GetNInterfaces());

    Ipv4Address gnb1X2Address = gnbIpIfaces.GetAddress(0);
    Ipv4Address gnb2X2Address = gnbIpIfaces.GetAddress(1);

    // Add X2 interface to both eNBs' X2 entities
    Ptr<NrEpcX2> gnb1X2 = gnb1->GetObject<NrEpcX2>();
    Ptr<NrEpcX2> gnb2X2 = gnb2->GetObject<NrEpcX2>();

    Ptr<NetDevice> gnb1NrDev = gnb1->GetDevice(0);
    Ptr<NetDevice> gnb2NrDev = gnb2->GetDevice(0);

    DoAddX2Interface(gnb1X2, gnb1NrDev, gnb1X2Address, gnb2X2, gnb2NrDev, gnb2X2Address);
}

void
NrNoBackhaulEpcHelper::DoAddX2Interface(const Ptr<NrEpcX2>& gnb1X2,
                                        const Ptr<NetDevice>& gnb1NrDev,
                                        const Ipv4Address& gnb1X2Address,
                                        const Ptr<NrEpcX2>& gnb2X2,
                                        const Ptr<NetDevice>& gnb2NrDev,
                                        const Ipv4Address& gnb2X2Address) const
{
    NS_LOG_FUNCTION(this);

    Ptr<NrGnbNetDevice> gnb1NrDevice = gnb1NrDev->GetObject<NrGnbNetDevice>();
    Ptr<NrGnbNetDevice> gnb2NrDevice = gnb2NrDev->GetObject<NrGnbNetDevice>();

    NS_ABORT_MSG_IF(!gnb1NrDevice, "Unable to find NrGnbNetDevice for the first gNB");
    NS_ABORT_MSG_IF(!gnb2NrDevice, "Unable to find NrGnbNetDevice for the second gNB");

    std::vector<uint16_t> gnb1CellIds = gnb1NrDevice->GetCellIds();
    std::vector<uint16_t> gnb2CellIds = gnb2NrDevice->GetCellIds();

    uint16_t gnb1CellId = gnb1CellIds.at(0);
    uint16_t gnb2CellId = gnb2CellIds.at(0);

    NS_LOG_LOGIC("NrGnbNetDevice #1 = " << gnb1NrDev << " - CellId = " << gnb1CellId);
    NS_LOG_LOGIC("NrGnbNetDevice #2 = " << gnb2NrDev << " - CellId = " << gnb2CellId);

    gnb1X2->AddX2Interface(gnb1CellId, gnb1X2Address, gnb2CellIds, gnb2X2Address);
    gnb2X2->AddX2Interface(gnb2CellId, gnb2X2Address, gnb1CellIds, gnb1X2Address);

    gnb1NrDevice->GetRrc()->AddX2Neighbour(gnb2CellId);
    gnb2NrDevice->GetRrc()->AddX2Neighbour(gnb1CellId);
}

void
NrNoBackhaulEpcHelper::AddUe(Ptr<NetDevice> ueDevice, uint64_t imsi)
{
    NS_LOG_FUNCTION(this << imsi << ueDevice);

    m_mmeApp->AddUe(imsi);
    m_pgwApp->AddUe(imsi);
}

uint8_t
NrNoBackhaulEpcHelper::ActivateEpsBearer(Ptr<NetDevice> ueDevice,
                                         uint64_t imsi,
                                         Ptr<NrEpcTft> tft,
                                         NrEpsBearer bearer)
{
    NS_LOG_FUNCTION(this << ueDevice << imsi);

    // we now retrieve the IPv4/IPv6 address of the UE and notify it to the SGW;
    // we couldn't do it before since address assignment is triggered by
    // the user simulation program, rather than done by the EPC
    Ptr<Node> ueNode = ueDevice->GetNode();
    Ptr<Ipv4> ueIpv4 = ueNode->GetObject<Ipv4>();
    Ptr<Ipv6> ueIpv6 = ueNode->GetObject<Ipv6>();
    NS_ASSERT_MSG(ueIpv4 || ueIpv6,
                  "UEs need to have IPv4/IPv6 installed before EPS bearers can be activated");

    if (ueIpv4)
    {
        int32_t interface = ueIpv4->GetInterfaceForDevice(ueDevice);
        if (interface >= 0 && ueIpv4->GetNAddresses(interface) == 1)
        {
            Ipv4Address ueAddr = ueIpv4->GetAddress(interface, 0).GetLocal();
            NS_LOG_LOGIC(" UE IPv4 address: " << ueAddr);
            m_pgwApp->SetUeAddress(imsi, ueAddr);
        }
    }
    if (ueIpv6)
    {
        int32_t interface6 = ueIpv6->GetInterfaceForDevice(ueDevice);
        if (interface6 >= 0 && ueIpv6->GetNAddresses(interface6) == 2)
        {
            Ipv6Address ueAddr6 = ueIpv6->GetAddress(interface6, 1).GetAddress();
            NS_LOG_LOGIC(" UE IPv6 address: " << ueAddr6);
            m_pgwApp->SetUeAddress6(imsi, ueAddr6);
        }
    }
    uint8_t bearerId = m_mmeApp->AddBearer(imsi, tft, bearer);
    DoActivateEpsBearerForUe(ueDevice, tft, bearer);

    return bearerId;
}

void
NrNoBackhaulEpcHelper::DoActivateEpsBearerForUe(const Ptr<NetDevice>& ueDevice,
                                                const Ptr<NrEpcTft>& tft,
                                                const NrEpsBearer& bearer) const
{
    NS_LOG_FUNCTION(this);
    Ptr<NrUeNetDevice> ueNrDevice = DynamicCast<NrUeNetDevice>(ueDevice);
    if (!ueNrDevice)
    {
        // You may wonder why this is not an assert. Well, take a look in epc-test-s1u-downlink
        // and -uplink: we are using CSMA to simulate UEs.
        NS_LOG_WARN("Unable to find NrUeNetDevice while activating the EPS bearer");
    }
    else
    {
        // Schedule with context so that logging statements have Node ID
        Simulator::ScheduleWithContext(ueNrDevice->GetNode()->GetId(),
                                       Time(),
                                       &NrEpcUeNas::ActivateEpsBearer,
                                       ueNrDevice->GetNas(),
                                       bearer,
                                       tft);
    }
}

Ptr<Node>
NrNoBackhaulEpcHelper::GetPgwNode() const
{
    return m_pgw;
}

Ipv4InterfaceContainer
NrNoBackhaulEpcHelper::AssignUeIpv4Address(NetDeviceContainer ueDevices)
{
    return m_uePgwAddressHelper.Assign(ueDevices);
}

Ipv6InterfaceContainer
NrNoBackhaulEpcHelper::AssignUeIpv6Address(NetDeviceContainer ueDevices)
{
    for (auto iter = ueDevices.Begin(); iter != ueDevices.End(); iter++)
    {
        Ptr<Icmpv6L4Protocol> icmpv6 = (*iter)->GetNode()->GetObject<Icmpv6L4Protocol>();
        icmpv6->SetAttribute("DAD", BooleanValue(false));
    }
    return m_uePgwAddressHelper6.Assign(ueDevices);
}

Ipv4Address
NrNoBackhaulEpcHelper::GetUeDefaultGatewayAddress()
{
    // return the address of the tun device
    return m_pgw->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
}

Ipv6Address
NrNoBackhaulEpcHelper::GetUeDefaultGatewayAddress6()
{
    // return the address of the tun device
    return m_pgw->GetObject<Ipv6>()->GetAddress(1, 1).GetAddress();
}

Ptr<Node>
NrNoBackhaulEpcHelper::GetSgwNode() const
{
    return m_sgw;
}

void
NrNoBackhaulEpcHelper::AddS1Interface(Ptr<Node> gnb,
                                      Ipv4Address gnbAddress,
                                      Ipv4Address sgwAddress,
                                      std::vector<uint16_t> cellIds)
{
    NS_LOG_FUNCTION(this << gnb << gnbAddress << sgwAddress << cellIds.size());

    // create S1-U socket for the gNB
    Ptr<Socket> gnbS1uSocket =
        Socket::CreateSocket(gnb, TypeId::LookupByName("ns3::UdpSocketFactory"));
    int retval = gnbS1uSocket->Bind(InetSocketAddress(gnbAddress, m_gtpuUdpPort));
    NS_ASSERT(retval == 0);

    Ptr<NrEpcGnbApplication> gnbApp = gnb->GetApplication(0)->GetObject<NrEpcGnbApplication>();
    NS_ASSERT_MSG(gnbApp, "NrEpcGnbApplication not available");
    gnbApp->AddS1Interface(gnbS1uSocket, gnbAddress, sgwAddress);

    NS_LOG_INFO("Connect S1-AP interface");
    for (uint16_t cellId : cellIds)
    {
        NS_LOG_DEBUG("Adding MME and SGW for cell ID " << cellId);
        m_mmeApp->AddGnb(cellId, gnbAddress, gnbApp->GetS1apSapGnb());
        m_sgwApp->AddGnb(cellId, gnbAddress, sgwAddress);
    }
    gnbApp->SetS1apSapMme(m_mmeApp->GetS1apSapMme());
}

int64_t
NrNoBackhaulEpcHelper::AssignStreams(int64_t stream)
{
    int64_t currentStream = stream;
    NS_ABORT_MSG_UNLESS(m_pgw && m_sgw && m_mme, "Running AssignStreams on empty node pointers");
    InternetStackHelper internet;
    NodeContainer nc;
    nc.Add(m_pgw);
    nc.Add(m_sgw);
    nc.Add(m_mme);
    currentStream += internet.AssignStreams(nc, currentStream);
    return (currentStream - stream);
}

void
NrNoBackhaulEpcHelper::AddRemoteUe(uint64_t relayImsi, Ipv4Address ueAddr)
{
    NS_LOG_FUNCTION(this << relayImsi << ueAddr);
    m_pgwApp->AddRemoteUe(relayImsi, ueAddr);
}

void
NrNoBackhaulEpcHelper::RemoveRemoteUe(uint64_t relayImsi, Ipv4Address ueAddr)
{
    NS_LOG_FUNCTION(this << relayImsi << ueAddr);
    m_pgwApp->RemoveRemoteUe(relayImsi, ueAddr);
}

} // namespace ns3

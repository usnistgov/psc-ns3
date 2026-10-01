/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "nr-test-rlc-um-e2e.h"

#include "nr-simple-helper.h"
#include "nr-test-entities.h"

#include "ns3/config.h"
#include "ns3/error-model.h"
#include "ns3/log.h"
#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/nr-bearer-stats-calculator.h"
#include "ns3/nr-rlc-header.h"
#include "ns3/nr-rlc-um.h"
#include "ns3/packet.h"
#include "ns3/pointer.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/simulator.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrRlcUmE2eTest");

/**
 * Test x.x.x RLC UM: End-to-end flow
 */

/**
 * TestSuite
 */

NrRlcUmE2eTestSuite::NrRlcUmE2eTestSuite()
    : TestSuite("nr-rlc-um-e2e", Type::SYSTEM)
{
    // NS_LOG_INFO ("Creating NrRlcUmE2eTestSuite");

    double losses[] = {0.0, 0.10, 0.25, 0.50, 0.75, 0.90, 1.00};
    uint32_t seeds[] = {1111, 2222, 3333, 4444, 5555, 6666, 7777, 8888, 9999, 10101};

    for (uint32_t l = 0; l < (sizeof(losses) / sizeof(double)); l++)
    {
        for (uint32_t s = 0; s < (sizeof(seeds) / sizeof(uint32_t)); s++)
        {
            std::ostringstream name;
            name << " Losses = " << losses[l] << "%. Seed = " << seeds[s];
            TestCase::Duration testDuration;
            if (l == 1 && s == 0)
            {
                testDuration = TestCase::Duration::QUICK;
            }
            else
            {
                testDuration = TestCase::Duration::EXTENSIVE;
            }
            AddTestCase(new NrRlcUmE2eTestCase(name.str(), seeds[s], losses[l]), testDuration);
        }
    }
}

/**
 * \ingroup nr-test
 * Static variable for test initialization
 */
static NrRlcUmE2eTestSuite nrRlcUmE2eTestSuite;

/**
 * TestCase
 */

NrRlcUmE2eTestCase::NrRlcUmE2eTestCase(std::string name, uint32_t seed, double losses)
    : TestCase(name)
{
    // NS_LOG_UNCOND ("Creating NrRlcUmTestingTestCase: " + name);

    m_seed = seed;
    m_losses = losses;

    m_dlDrops = 0;
    m_ulDrops = 0;
}

NrRlcUmE2eTestCase::~NrRlcUmE2eTestCase()
{
}

void
NrRlcUmE2eTestCase::DlDropEvent(Ptr<const Packet> p)
{
    // NS_LOG_FUNCTION (this);
    m_dlDrops++;
}

void
NrRlcUmE2eTestCase::UlDropEvent(Ptr<const Packet> p)
{
    // NS_LOG_FUNCTION (this);
    m_ulDrops++;
}

void
NrRlcUmE2eTestCase::DoRun()
{
    uint16_t numberOfNodes = 1;

    // LogLevel level = (LogLevel) (LOG_LEVEL_ALL | LOG_PREFIX_TIME | LOG_PREFIX_NODE |
    // LOG_PREFIX_FUNC); LogComponentEnable ("NrRlcUmE2eTest", level); LogComponentEnable
    // ("ErrorModel", level); LogComponentEnable ("NrSimpleHelper", level); LogComponentEnable
    // ("NrSimpleNetDevice", level); LogComponentEnable ("SimpleNetDevice", level);
    // LogComponentEnable ("SimpleChannel", level);
    // LogComponentEnable ("NrTestEntities", level);
    // LogComponentEnable ("NrPdcp", level);
    // LogComponentEnable ("NrRlc", level);
    // LogComponentEnable ("NrRlcUm", level);
    // LogComponentEnable ("NrRlcAm", level);

    RngSeedManager::SetSeed(m_seed);

    Ptr<NrSimpleHelper> nrSimpleHelper = CreateObject<NrSimpleHelper>();
    // nrSimpleHelper->EnableLogComponents ();
    // nrSimpleHelper->EnableTraces ();

    nrSimpleHelper->SetAttribute("RlcEntity", StringValue("RlcUm"));

    // gNB and UE nodes
    NodeContainer ueNodes;
    NodeContainer gnbNodes;
    gnbNodes.Create(numberOfNodes);
    ueNodes.Create(numberOfNodes);

    // Install NR Devices to the nodes
    NetDeviceContainer nrGnbDevs = nrSimpleHelper->InstallGnbDevice(gnbNodes);
    NetDeviceContainer ueNrDevs = nrSimpleHelper->InstallUeDevice(ueNodes);

    // Note: Just one gNB and UE are supported. Everything is done in InstallGnbDevice and
    // InstallUeDevice

    // Attach one UE per eNodeB
    // for (uint16_t i = 0; i < numberOfNodes; i++)
    //   {
    //     nrSimpleHelper->Attach (ueNrDevs.Get(i), nrGnbDevs.Get(i));
    //   }

    // nrSimpleHelper->ActivateEpsBearer (ueNrDevs, NrEpsBearer
    // (NrEpsBearer::NGBR_VIDEO_TCP_DEFAULT), NrEpcTft::Default ());

    // Error models: downlink and uplink
    Ptr<RateErrorModel> dlEm = CreateObject<RateErrorModel>();
    dlEm->SetAttribute("ErrorRate", DoubleValue(m_losses));
    dlEm->SetAttribute("ErrorUnit", StringValue("ERROR_UNIT_PACKET"));

    Ptr<RateErrorModel> ulEm = CreateObject<RateErrorModel>();
    ulEm->SetAttribute("ErrorRate", DoubleValue(m_losses));
    ulEm->SetAttribute("ErrorUnit", StringValue("ERROR_UNIT_PACKET"));

    // The below hooks will cause drops to be counted at simple phy layer
    ueNrDevs.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(dlEm));
    ueNrDevs.Get(0)->TraceConnectWithoutContext(
        "PhyRxDrop",
        MakeCallback(&NrRlcUmE2eTestCase::DlDropEvent, this));
    nrGnbDevs.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(ulEm));
    nrGnbDevs.Get(0)->TraceConnectWithoutContext(
        "PhyRxDrop",
        MakeCallback(&NrRlcUmE2eTestCase::UlDropEvent, this));

    // Sending packets from gNB RRC layer (eNB -> UE)
    nrSimpleHelper->m_gnbRrc->SetArrivalTime(Seconds(0.010));
    nrSimpleHelper->m_gnbRrc->SetPduSize(100);

    // MAC sends transmission opportunities (TxOpp)
    nrSimpleHelper->m_gnbMac->SetTxOppSize(150);
    nrSimpleHelper->m_gnbMac->SetTxOppTime(Seconds(0.005));
    nrSimpleHelper->m_gnbMac->SetTxOpportunityMode(NrTestMac::RANDOM_MODE);

    // Sending packets from UE RRC layer (UE -> eNB)
    nrSimpleHelper->m_ueRrc->SetArrivalTime(Seconds(0.010));
    nrSimpleHelper->m_ueRrc->SetPduSize(100);

    // MAC sends transmission opportunities (TxOpp)
    nrSimpleHelper->m_ueMac->SetTxOppSize(150);
    nrSimpleHelper->m_ueMac->SetTxOppTime(Seconds(0.005));
    nrSimpleHelper->m_ueMac->SetTxOpportunityMode(NrTestMac::RANDOM_MODE);

    // Start/Stop pseudo-application at gNB RRC
    Simulator::Schedule(Seconds(0.100), &NrTestRrc::Start, nrSimpleHelper->m_gnbRrc);
    Simulator::Schedule(Seconds(10.100), &NrTestRrc::Stop, nrSimpleHelper->m_gnbRrc);

    // Start/Stop pseudo-application at UE RRC
    Simulator::Schedule(Seconds(20.100), &NrTestRrc::Start, nrSimpleHelper->m_ueRrc);
    Simulator::Schedule(Seconds(30.100), &NrTestRrc::Stop, nrSimpleHelper->m_ueRrc);

    Simulator::Stop(Seconds(31.000));
    Simulator::Run();

    uint32_t txGnbRrcPdus = nrSimpleHelper->m_gnbRrc->GetTxPdus();
    uint32_t rxUeRrcPdus = nrSimpleHelper->m_ueRrc->GetRxPdus();

    uint32_t txUeRrcPdus = nrSimpleHelper->m_ueRrc->GetTxPdus();
    uint32_t rxGnbRrcPdus = nrSimpleHelper->m_gnbRrc->GetRxPdus();

    // NS_LOG_INFO ("Seed = " << m_seed);
    // NS_LOG_INFO ("Losses (%) = " << uint32_t (m_losses * 100));

    // NS_LOG_INFO ("dl dev drops = " << m_dlDrops);
    // NS_LOG_INFO ("ul dev drops = " << m_ulDrops);

    // NS_LOG_INFO ("eNB tx RRC count = " << txGnbRrcPdus);
    // NS_LOG_INFO ("eNB rx RRC count = " << rxGnbRrcPdus);
    // NS_LOG_INFO ("UE tx RRC count = " << txUeRrcPdus);
    // NS_LOG_INFO ("UE rx RRC count = " << rxUeRrcPdus);

    NS_LOG_INFO(m_seed << "\t" << m_losses << "\t" << txGnbRrcPdus << "\t" << rxUeRrcPdus << "\t"
                       << m_dlDrops);
    NS_LOG_INFO(m_seed << "\t" << m_losses << "\t" << txUeRrcPdus << "\t" << rxGnbRrcPdus << "\t"
                       << m_ulDrops);

    NS_TEST_ASSERT_MSG_EQ(txGnbRrcPdus,
                          rxUeRrcPdus + m_dlDrops,
                          "Downlink: TX PDUs (" << txGnbRrcPdus << ") != RX PDUs (" << rxUeRrcPdus
                                                << ") + DROPS (" << m_dlDrops << ")");
    NS_TEST_ASSERT_MSG_EQ(txUeRrcPdus,
                          rxGnbRrcPdus + m_ulDrops,
                          "Uplink: TX PDUs (" << txUeRrcPdus << ") != RX PDUs (" << rxGnbRrcPdus
                                              << ") + DROPS (" << m_ulDrops << ")");

    Simulator::Destroy();
}

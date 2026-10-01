/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "nr-test-rlc-um-transmitter.h"

#include "nr-test-entities.h"

#include "ns3/log.h"
#include "ns3/nr-rlc-header.h"
#include "ns3/nr-rlc-um.h"
#include "ns3/simulator.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrRlcUmTransmitterTest");

/**
 * TestSuite 4.1.1 RLC UM: Only transmitter
 */

NrRlcUmTransmitterTestSuite::NrRlcUmTransmitterTestSuite()
    : TestSuite("nr-rlc-um-transmitter", Type::SYSTEM)
{
    // LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_ALL);
    // LogComponentEnable ("NrRlcUmTransmitterTest", logLevel);

    // NS_LOG_INFO ("Creating NrRlcUmTransmitterTestSuite");

    AddTestCase(new NrRlcUmTransmitterOneSduTestCase("One SDU, one PDU"),
                TestCase::Duration::QUICK);
    AddTestCase(new NrRlcUmTransmitterSegmentationTestCase("Segmentation"),
                TestCase::Duration::QUICK);
    AddTestCase(new NrRlcUmTransmitterConcatenationTestCase("Concatenation"),
                TestCase::Duration::QUICK);
    AddTestCase(new NrRlcUmTransmitterReportBufferStatusTestCase("ReportBufferStatus primitive"),
                TestCase::Duration::QUICK);
}

/**
 * \ingroup nr-test
 * Static variable for test initialization
 */
static NrRlcUmTransmitterTestSuite nrRlcUmTransmitterTestSuite;

NrRlcUmTransmitterTestCase::NrRlcUmTransmitterTestCase(std::string name)
    : TestCase(name)
{
    // NS_LOG_UNCOND ("Creating NrRlcUmTransmitterTestCase: " + name);
}

NrRlcUmTransmitterTestCase::~NrRlcUmTransmitterTestCase()
{
}

void
NrRlcUmTransmitterTestCase::DoRun()
{
    // LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_ALL);
    // LogComponentEnable ("NrRlcUmTransmitterTest", logLevel);
    // LogComponentEnable ("NrTestEntities", logLevel);
    // LogComponentEnable ("NrRlc", logLevel);
    // LogComponentEnable ("NrRlcUm", logLevel);
    // LogComponentEnable ("NrRlcHeader", logLevel);

    uint16_t rnti = 1111;
    uint8_t lcid = 222;

    Packet::EnablePrinting();

    // Create topology

    // Create transmission PDCP test entity
    txPdcp = CreateObject<NrTestPdcp>();

    // Create transmission RLC entity
    txRlc = CreateObject<NrRlcUm>();
    txRlc->SetRnti(rnti);
    txRlc->SetLcId(lcid);

    // Create transmission MAC test entity
    txMac = CreateObject<NrTestMac>();
    txMac->SetRlcHeaderType(NrTestMac::UM_RLC_HEADER);

    // Connect SAPs: PDCP (TX) <-> RLC (Tx) <-> MAC (Tx)
    txPdcp->SetNrRlcSapProvider(txRlc->GetNrRlcSapProvider());
    txRlc->SetNrRlcSapUser(txPdcp->GetNrRlcSapUser());

    txRlc->SetNrMacSapProvider(txMac->GetNrMacSapProvider());
    txMac->SetNrMacSapUser(txRlc->GetNrMacSapUser());
}

void
NrRlcUmTransmitterTestCase::CheckDataReceived(Time time,
                                              std::string shouldReceived,
                                              std::string assertMsg)
{
    Simulator::Schedule(time,
                        &NrRlcUmTransmitterTestCase::DoCheckDataReceived,
                        this,
                        shouldReceived,
                        assertMsg);
}

void
NrRlcUmTransmitterTestCase::DoCheckDataReceived(std::string shouldReceived, std::string assertMsg)
{
    NS_TEST_ASSERT_MSG_EQ(shouldReceived, txMac->GetDataReceived(), assertMsg);
}

/**
 * Test 4.1.1.1 One SDU, One PDU
 */
NrRlcUmTransmitterOneSduTestCase::NrRlcUmTransmitterOneSduTestCase(std::string name)
    : NrRlcUmTransmitterTestCase(name)
{
}

NrRlcUmTransmitterOneSduTestCase::~NrRlcUmTransmitterOneSduTestCase()
{
}

void
NrRlcUmTransmitterOneSduTestCase::DoRun()
{
    // Create topology
    NrRlcUmTransmitterTestCase::DoRun();

    //
    // a) One SDU generates one PDU
    //

    // PDCP entity sends data
    txPdcp->SendData(Seconds(0.100), "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    // MAC entity sends TxOpp to RLC entity
    txMac->SendTxOpportunity(Seconds(0.150), 28);
    CheckDataReceived(Seconds(0.200), "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "SDU is not OK");

    Simulator::Run();
    Simulator::Destroy();
}

/**
 * Test 4.1.1.2 Segmentation (One SDU => n PDUs)
 */
NrRlcUmTransmitterSegmentationTestCase::NrRlcUmTransmitterSegmentationTestCase(std::string name)
    : NrRlcUmTransmitterTestCase(name)
{
}

NrRlcUmTransmitterSegmentationTestCase::~NrRlcUmTransmitterSegmentationTestCase()
{
}

void
NrRlcUmTransmitterSegmentationTestCase::DoRun()
{
    // Create topology
    NrRlcUmTransmitterTestCase::DoRun();

    //
    // b) Segmentation: one SDU generates n PDUs
    //

    // PDCP entity sends data
    txPdcp->SendData(Seconds(0.100), "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    // MAC entity sends small TxOpp to RLC entity generating four segments
    txMac->SendTxOpportunity(Seconds(0.150), 10);
    CheckDataReceived(Seconds(0.200), "ABCDEFGH", "Segment #1 is not OK");

    txMac->SendTxOpportunity(Seconds(0.200), 10);
    CheckDataReceived(Seconds(0.250), "IJKLMNOP", "Segment #2 is not OK");

    txMac->SendTxOpportunity(Seconds(0.300), 10);
    CheckDataReceived(Seconds(0.350), "QRSTUVWX", "Segment #3 is not OK");

    txMac->SendTxOpportunity(Seconds(0.400), 4);
    CheckDataReceived(Seconds(0.450), "YZ", "Segment #4 is not OK");

    Simulator::Run();
    Simulator::Destroy();
}

/**
 * Test 4.1.1.3 Concatenation (n SDUs => One PDU)
 */
NrRlcUmTransmitterConcatenationTestCase::NrRlcUmTransmitterConcatenationTestCase(std::string name)
    : NrRlcUmTransmitterTestCase(name)
{
}

NrRlcUmTransmitterConcatenationTestCase::~NrRlcUmTransmitterConcatenationTestCase()
{
}

void
NrRlcUmTransmitterConcatenationTestCase::DoRun()
{
    // Create topology
    NrRlcUmTransmitterTestCase::DoRun();

    //
    // c) Concatenation: n SDUs generate one PDU
    //

    // PDCP entity sends three data packets
    txPdcp->SendData(Seconds(0.100), "ABCDEFGH");
    txPdcp->SendData(Seconds(0.150), "IJKLMNOPQR");
    txPdcp->SendData(Seconds(0.200), "STUVWXYZ");

    // MAC entity sends TxOpp to RLC entity generating only one concatenated PDU
    txMac->SendTxOpportunity(Seconds(0.250), 31);
    CheckDataReceived(Seconds(0.300), "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Concatenation is not OK");

    Simulator::Run();
    Simulator::Destroy();
}

/**
 * Test 4.1.1.4 Report Buffer Status (test primitive parameters)
 */
NrRlcUmTransmitterReportBufferStatusTestCase::NrRlcUmTransmitterReportBufferStatusTestCase(
    std::string name)
    : NrRlcUmTransmitterTestCase(name)
{
}

NrRlcUmTransmitterReportBufferStatusTestCase::~NrRlcUmTransmitterReportBufferStatusTestCase()
{
}

void
NrRlcUmTransmitterReportBufferStatusTestCase::DoRun()
{
    // Create topology
    NrRlcUmTransmitterTestCase::DoRun();

    //
    // d) Test the parameters of the ReportBufferStatus primitive
    //

    // PDCP entity sends data
    txPdcp->SendData(Seconds(0.100), "ABCDEFGHIJ"); // 10
    txPdcp->SendData(Seconds(0.150), "KLMNOPQRS");  // 9
    txPdcp->SendData(Seconds(0.200), "TUVWXYZ");    // 7

    txMac->SendTxOpportunity(Seconds(0.250), (2 + 2) + (10 + 6));
    CheckDataReceived(Seconds(0.300), "ABCDEFGHIJKLMNOP", "SDU is not OK");

    txPdcp->SendData(Seconds(0.350), "ABCDEFGH");     // 8
    txPdcp->SendData(Seconds(0.400), "IJKLMNOPQRST"); // 12
    txPdcp->SendData(Seconds(0.450), "UVWXYZ");       // 6

    txMac->SendTxOpportunity(Seconds(0.500), 2 + 3);
    CheckDataReceived(Seconds(0.550), "QRS", "SDU is not OK");

    txPdcp->SendData(Seconds(0.600), "ABCDEFGH");     // 8
    txPdcp->SendData(Seconds(0.650), "IJKLMNOPQRST"); // 12
    txPdcp->SendData(Seconds(0.700), "UVWXYZ");       // 6

    txPdcp->SendData(Seconds(0.750), "ABCDEFGHIJ"); // 10
    txPdcp->SendData(Seconds(0.800), "KLMNOPQRST"); // 10
    txPdcp->SendData(Seconds(0.850), "UVWXYZ");     // 6

    txMac->SendTxOpportunity(Seconds(0.900), 2 + 7);
    CheckDataReceived(Seconds(0.950), "TUVWXYZ", "SDU is not OK");

    txMac->SendTxOpportunity(Seconds(1.000), (2 + 2) + (8 + 2));
    CheckDataReceived(Seconds(1.050), "ABCDEFGHIJ", "SDU is not OK");

    txPdcp->SendData(Seconds(1.100), "ABCDEFGHIJ"); // 10
    txPdcp->SendData(Seconds(1.150), "KLMNOPQRST"); // 10
    txPdcp->SendData(Seconds(1.200), "UVWXYZ");     // 6

    txMac->SendTxOpportunity(Seconds(1.250), 2 + 2);
    CheckDataReceived(Seconds(1.300), "KL", "SDU is not OK");

    txMac->SendTxOpportunity(Seconds(1.350), 2 + 3);
    CheckDataReceived(Seconds(1.400), "MNO", "SDU is not OK");

    txMac->SendTxOpportunity(Seconds(1.450), 2 + 5);
    CheckDataReceived(Seconds(1.500), "PQRST", "SDU is not OK");

    txMac->SendTxOpportunity(Seconds(1.550),
                             (2 + 2 + 1 + 2 + 1 + 2 + 1) + (6 + 8 + 12 + 6 + 10 + 10 + 3));
    CheckDataReceived(Seconds(1.600),
                      "UVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVW",
                      "SDU is not OK");

    txMac->SendTxOpportunity(Seconds(1.650), (2 + 2 + 1 + 2) + (3 + 10 + 10 + 6));
    CheckDataReceived(Seconds(1.700), "XYZABCDEFGHIJKLMNOPQRSTUVWXYZ", "SDU is not OK");

    Simulator::Run();
    Simulator::Destroy();
}

/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#include "nr-test-rlc-am-transmitter.h"

#include "nr-test-entities.h"

#include "ns3/log.h"
#include "ns3/nr-rlc-am.h"
#include "ns3/nr-rlc-header.h"
#include "ns3/simulator.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrRlcAmTransmitterTest");

/**
 * TestSuite 4.1.1 RLC AM: Only transmitter
 */

NrRlcAmTransmitterTestSuite::NrRlcAmTransmitterTestSuite()
    : TestSuite("nr-rlc-am-transmitter", Type::SYSTEM)
{
    // LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_ALL);
    // LogComponentEnable ("NrRlcAmTransmitterTest", logLevel);

    AddTestCase(new NrRlcAmTransmitterOneSduTestCase("One SDU, one PDU"),
                TestCase::Duration::QUICK);
    AddTestCase(new NrRlcAmTransmitterSegmentationTestCase("Segmentation"),
                TestCase::Duration::QUICK);
    AddTestCase(new NrRlcAmTransmitterConcatenationTestCase("Concatenation"),
                TestCase::Duration::QUICK);
    AddTestCase(new NrRlcAmTransmitterReportBufferStatusTestCase("ReportBufferStatus primitive"),
                TestCase::Duration::QUICK);
}

/**
 * \ingroup nr-test
 * Static variable for test initialization
 */
static NrRlcAmTransmitterTestSuite nrRlcAmTransmitterTestSuite;

NrRlcAmTransmitterTestCase::NrRlcAmTransmitterTestCase(std::string name)
    : TestCase(name)
{
}

NrRlcAmTransmitterTestCase::~NrRlcAmTransmitterTestCase()
{
}

void
NrRlcAmTransmitterTestCase::DoRun()
{
    // LogLevel logLevel = (LogLevel)(LOG_PREFIX_FUNC | LOG_PREFIX_TIME | LOG_LEVEL_ALL);
    // LogComponentEnable ("NrRlcAmTransmitterTest", logLevel);
    // LogComponentEnable ("NrTestEntities", logLevel);
    // LogComponentEnable ("NrRlc", logLevel);
    // LogComponentEnable ("NrRlcAm", logLevel);
    // LogComponentEnable ("NrRlcHeader", logLevel);

    uint16_t rnti = 1111;
    uint8_t lcid = 222;

    Packet::EnablePrinting();

    // Create topology

    // Create transmission PDCP test entity
    txPdcp = CreateObject<NrTestPdcp>();

    // Create transmission RLC entity
    txRlc = CreateObject<NrRlcAm>();
    txRlc->SetRnti(rnti);
    txRlc->SetLcId(lcid);

    // Create transmission MAC test entity
    txMac = CreateObject<NrTestMac>();
    txMac->SetRlcHeaderType(NrTestMac::AM_RLC_HEADER);

    // Connect SAPs: PDCP (TX) <-> RLC (Tx) <-> MAC (Tx)
    txPdcp->SetNrRlcSapProvider(txRlc->GetNrRlcSapProvider());
    txRlc->SetNrRlcSapUser(txPdcp->GetNrRlcSapUser());

    txRlc->SetNrMacSapProvider(txMac->GetNrMacSapProvider());
    txMac->SetNrMacSapUser(txRlc->GetNrMacSapUser());
}

void
NrRlcAmTransmitterTestCase::CheckDataReceived(Time time,
                                              std::string shouldReceived,
                                              std::string assertMsg)
{
    Simulator::Schedule(time,
                        &NrRlcAmTransmitterTestCase::DoCheckDataReceived,
                        this,
                        shouldReceived,
                        assertMsg);
}

void
NrRlcAmTransmitterTestCase::DoCheckDataReceived(std::string shouldReceived, std::string assertMsg)
{
    NS_TEST_ASSERT_MSG_EQ(shouldReceived, txMac->GetDataReceived(), assertMsg);
}

/**
 * Test 4.1.1.1 One SDU, One PDU
 */
NrRlcAmTransmitterOneSduTestCase::NrRlcAmTransmitterOneSduTestCase(std::string name)
    : NrRlcAmTransmitterTestCase(name)
{
}

NrRlcAmTransmitterOneSduTestCase::~NrRlcAmTransmitterOneSduTestCase()
{
}

void
NrRlcAmTransmitterOneSduTestCase::DoRun()
{
    // Create topology
    NrRlcAmTransmitterTestCase::DoRun();

    //
    // a) One SDU generates one PDU
    //

    // PDCP entity sends data
    txPdcp->SendData(Seconds(0.100), "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    txMac->SendTxOpportunity(Seconds(0.150), 30);
    CheckDataReceived(Seconds(0.200), "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "SDU is not OK");

    Simulator::Stop(Seconds(0.3));
    Simulator::Run();
    Simulator::Destroy();
}

/**
 * Test 4.1.1.2 Segmentation (One SDU => n PDUs)
 */
NrRlcAmTransmitterSegmentationTestCase::NrRlcAmTransmitterSegmentationTestCase(std::string name)
    : NrRlcAmTransmitterTestCase(name)
{
}

NrRlcAmTransmitterSegmentationTestCase::~NrRlcAmTransmitterSegmentationTestCase()
{
}

void
NrRlcAmTransmitterSegmentationTestCase::DoRun()
{
    // Create topology
    NrRlcAmTransmitterTestCase::DoRun();

    //
    // b) Segmentation: one SDU generates n PDUs
    //

    // PDCP entity sends data
    txPdcp->SendData(Seconds(0.100), "ABCDEFGHIJKLMNOPQRSTUVWXYZZ");

    // MAC entity sends small TxOpp to RLC entity generating four segments
    txMac->SendTxOpportunity(Seconds(0.150), 12);
    CheckDataReceived(Seconds(0.200), "ABCDEFGH", "Segment #1 is not OK");

    txMac->SendTxOpportunity(Seconds(0.250), 12);
    CheckDataReceived(Seconds(0.300), "IJKLMNOP", "Segment #2 is not OK");

    txMac->SendTxOpportunity(Seconds(0.350), 12);
    CheckDataReceived(Seconds(0.400), "QRSTUVWX", "Segment #3 is not OK");

    txMac->SendTxOpportunity(Seconds(0.450), 7);
    CheckDataReceived(Seconds(0.500), "YZZ", "Segment #4 is not OK");

    Simulator::Stop(Seconds(0.6));
    Simulator::Run();
    Simulator::Destroy();
}

/**
 * Test 4.1.1.3 Concatenation (n SDUs => One PDU)
 */
NrRlcAmTransmitterConcatenationTestCase::NrRlcAmTransmitterConcatenationTestCase(std::string name)
    : NrRlcAmTransmitterTestCase(name)
{
}

NrRlcAmTransmitterConcatenationTestCase::~NrRlcAmTransmitterConcatenationTestCase()
{
}

void
NrRlcAmTransmitterConcatenationTestCase::DoRun()
{
    // Create topology
    NrRlcAmTransmitterTestCase::DoRun();

    //
    // c) Concatenation: n SDUs generate one PDU
    //

    // PDCP entity sends three data packets
    txPdcp->SendData(Seconds(0.100), "ABCDEFGH");
    txPdcp->SendData(Seconds(0.150), "IJKLMNOPQR");
    txPdcp->SendData(Seconds(0.200), "STUVWXYZ");

    // MAC entity sends TxOpp to RLC entity generating only one concatenated PDU

    txMac->SendTxOpportunity(Seconds(0.250), 33);
    CheckDataReceived(Seconds(0.300), "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "Concatenation is not OK");

    Simulator::Stop(Seconds(0.4));
    Simulator::Run();
    Simulator::Destroy();
}

/**
 * Test 4.1.1.4 Report Buffer Status (test primitive parameters)
 */
NrRlcAmTransmitterReportBufferStatusTestCase::NrRlcAmTransmitterReportBufferStatusTestCase(
    std::string name)
    : NrRlcAmTransmitterTestCase(name)
{
}

NrRlcAmTransmitterReportBufferStatusTestCase::~NrRlcAmTransmitterReportBufferStatusTestCase()
{
}

void
NrRlcAmTransmitterReportBufferStatusTestCase::DoRun()
{
    // Create topology
    NrRlcAmTransmitterTestCase::DoRun();

    //
    // d) Test the parameters of the ReportBufferStatus primitive
    //

    //   txMac->SendTxOpportunity (Seconds (0.1), (2+2) + (10+6));

    // PDCP entity sends data
    txPdcp->SendData(Seconds(0.100), "ABCDEFGHIJ"); // 10
    txPdcp->SendData(Seconds(0.150), "KLMNOPQRS");  // 9
    txPdcp->SendData(Seconds(0.200), "TUVWXYZ");    // 7

    txMac->SendTxOpportunity(Seconds(0.250), (4 + 2) + (10 + 6));
    CheckDataReceived(Seconds(0.300), "ABCDEFGHIJKLMNOP", "SDU #1 is not OK");

    txPdcp->SendData(Seconds(0.350), "ABCDEFGH");     // 8
    txPdcp->SendData(Seconds(0.400), "IJKLMNOPQRST"); // 12
    txPdcp->SendData(Seconds(0.450), "UVWXYZ");       // 6

    txMac->SendTxOpportunity(Seconds(0.500), 4 + 3);
    CheckDataReceived(Seconds(0.550), "QRS", "SDU #2 is not OK");

    txPdcp->SendData(Seconds(0.600), "ABCDEFGH");     // 8
    txPdcp->SendData(Seconds(0.650), "IJKLMNOPQRST"); // 12
    txPdcp->SendData(Seconds(0.700), "UVWXYZ");       // 6

    txPdcp->SendData(Seconds(0.750), "ABCDEFGHIJ"); // 10
    txPdcp->SendData(Seconds(0.800), "KLMNOPQRST"); // 10
    txPdcp->SendData(Seconds(0.850), "UVWXYZ");     // 6

    txMac->SendTxOpportunity(Seconds(0.900), 4 + 7);
    CheckDataReceived(Seconds(0.950), "TUVWXYZ", "SDU #3 is not OK");

    txMac->SendTxOpportunity(Seconds(1.000), (4 + 2) + (8 + 2));
    CheckDataReceived(Seconds(1.050), "ABCDEFGHIJ", "SDU #4 is not OK");

    txPdcp->SendData(Seconds(1.100), "ABCDEFGHIJ");  // 10
    txPdcp->SendData(Seconds(1.150), "KLMNOPQRSTU"); // 11
    txPdcp->SendData(Seconds(1.200), "VWXYZ");       // 5

    txMac->SendTxOpportunity(Seconds(1.250), 4 + 3);
    CheckDataReceived(Seconds(1.300), "KLM", "SDU #5 is not OK");

    txMac->SendTxOpportunity(Seconds(1.350), 4 + 3);
    CheckDataReceived(Seconds(1.400), "NOP", "SDU #6 is not OK");

    txMac->SendTxOpportunity(Seconds(1.450), 4 + 4);
    CheckDataReceived(Seconds(1.500), "QRST", "SDU #7 is not OK");

    txMac->SendTxOpportunity(Seconds(1.550),
                             (4 + 2 + 1 + 2 + 1 + 2 + 1) + (6 + 8 + 12 + 6 + 10 + 10 + 3));
    CheckDataReceived(Seconds(1.600),
                      "UVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVW",
                      "SDU #8 is not OK");

    txMac->SendTxOpportunity(Seconds(1.650), (4 + 2 + 1 + 2) + (3 + 10 + 10 + 7));
    CheckDataReceived(Seconds(1.700), "XYZABCDEFGHIJKLMNOPQRSTUVWXYZ", "SDU #9 is not OK");

    Simulator::Stop(Seconds(2));
    Simulator::Run();
    Simulator::Destroy();
}

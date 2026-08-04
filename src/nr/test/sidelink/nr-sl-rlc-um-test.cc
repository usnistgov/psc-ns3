//
// SPDX-License-Identifier: NIST-Software
//

#include "nr-sl-rlc-um-test.h"

#include "nr-sl-test-mock-objects.h"

#include "ns3/log.h"
#include "ns3/nr-rlc-header.h"
#include "ns3/nr-sl-rlc-um.h"
#include "ns3/simulator.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrSlRlcUmTest");

NrSlRlcUmTestSuite::NrSlRlcUmTestSuite()
    : TestSuite("nr-sl-rlc-um", Type::UNIT)
{
    AddTestCase(new NrSlRlcUmOneSduTestCase("One SDU, one PDU"), TestCase::Duration::QUICK);
    AddTestCase(new NrSlRlcUmPdbTestCase("Discard Timer and PDB Enforcement"),
                TestCase::Duration::QUICK);
}

static NrSlRlcUmTestSuite nrRlcUmTestSuite;

NrSlRlcUmTestCase::NrSlRlcUmTestCase(std::string name)
    : TestCase(name)
{
}

void
NrSlRlcUmTestCase::DoSetup()
{
    // Create transmission PDCP test entity
    m_txPdcp = CreateObject<NrSlTestPdcp>();

    // Create transmission RLC entity
    m_txRlc = CreateObject<NrSlRlcUm>();
    m_txRlc->SetRnti(10);
    m_txRlc->SetLcId(5);

    // Create transmission MAC test entity
    m_txMac = CreateObject<NrSlTestMac>();

    // Connect SAPs: PDCP (TX) <-> RLC (Tx) <-> MAC (Tx)
    m_txPdcp->SetNrSlRlcSapProvider(m_txRlc->GetNrSlRlcSapProvider());
    m_txRlc->SetNrSlRlcSapUser(m_txPdcp->GetNrSlRlcSapUser());

    m_txRlc->SetNrSlMacSapProvider(m_txMac->GetNrSlMacSapProvider());
    m_txMac->SetNrSlMacSapUser(m_txRlc->GetNrSlMacSapUser());
}

void
NrSlRlcUmTestCase::CheckDataReceived(Time time, std::string shouldReceived, std::string assertMsg)
{
    Simulator::Schedule(time,
                        &NrSlRlcUmTestCase::DoCheckDataReceived,
                        this,
                        shouldReceived,
                        assertMsg);
}

void
NrSlRlcUmTestCase::DoCheckDataReceived(std::string shouldReceived, std::string assertMsg)
{
    NS_TEST_ASSERT_MSG_EQ(shouldReceived, m_txMac->GetDataReceived(), assertMsg);
}

/**
 * One SDU, One PDU
 */
NrSlRlcUmOneSduTestCase::NrSlRlcUmOneSduTestCase(std::string name)
    : NrSlRlcUmTestCase(name)
{
}

void
NrSlRlcUmOneSduTestCase::DoRun()
{
    // PDCP entity sends data
    m_txPdcp->SendData(Seconds(0.100), "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    // MAC entity sends TxOpp to RLC entity
    m_txMac->SendTxOpportunity(Seconds(0.150), 28);
    CheckDataReceived(Seconds(0.200), "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "SDU is not OK");

    Simulator::Run();
    Simulator::Destroy();
}

/**
 * Discard Timer and PDB Enforcement
 */
NrSlRlcUmPdbTestCase::NrSlRlcUmPdbTestCase(std::string name)
    : NrSlRlcUmTestCase(name)
{
}

void
NrSlRlcUmPdbTestCase::DoRun()
{
    m_txRlc->SetPacketDelayBudgetMs(40);
    m_txRlc->SetAttribute("EnablePdcpDiscarding", BooleanValue(true));
    m_txRlc->TraceConnectWithoutContext("TxDrop",
                                        MakeCallback(&NrSlRlcUmPdbTestCase::TxDropCallback, this));

    // PDCP entity sends data
    m_txPdcp->SendData(Seconds(0.100), "ABCDEFGHIJKLM");
    m_txPdcp->SendData(Seconds(0.150), "NOPQRSTUVWXYZ");

    // MAC entity sends TxOpp to RLC entity
    m_txMac->SendTxOpportunity(Seconds(0.175), 30);
    CheckDataReceived(Seconds(0.200), "NOPQRSTUVWXYZ", "SDU is not OK");

    Simulator::Run();
    Simulator::Destroy();

    NS_TEST_ASSERT_MSG_EQ(m_pktDrop, true, "No packet was dropped");
}

void
NrSlRlcUmPdbTestCase::TxDropCallback(Ptr<const Packet> p)
{
    NS_TEST_ASSERT_MSG_EQ(m_pktDrop, false, "More than one packet has been dropped");

    m_pktDrop = true;
}

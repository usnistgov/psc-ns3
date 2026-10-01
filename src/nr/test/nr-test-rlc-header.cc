/*
 * Copyright (c) 2011, 2012, 2013 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Lluis Parcerisa <lparcerisa@cttc.cat> (TestUtils from test-asn1-encoding.cc)
 *         Nicola Baldo <nbaldo@cttc.es> (actual test)
 */

#include "ns3/log.h"
#include "ns3/nr-rlc-am-header.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/test.h"

#include <bitset>
#include <iomanip>
#include <list>
#include <vector>

NS_LOG_COMPONENT_DEFINE("TestNrRlcHeader");

namespace ns3
{

/**
 * \ingroup nr-test
 *
 * \brief Test Utils
 */
class TestUtils
{
  public:
    /**
     * Function to convert packet contents in hex format
     * \param pkt the packet
     * \returns a text string
     */
    static std::string sprintPacketContentsHex(Ptr<Packet> pkt)
    {
        std::vector<uint8_t> buffer(pkt->GetSize());
        std::ostringstream oss(std::ostringstream::out);
        pkt->CopyData(buffer.data(), buffer.size());
        for (auto b : buffer)
        {
            oss << std::setfill('0') << std::setw(2) << std::hex << (uint32_t)b;
        }
        return oss.str();
    }

    /**
     * Function to convert packet contents in binary format
     * \param pkt the packet
     * \returns a text string
     */
    static std::string sprintPacketContentsBin(Ptr<Packet> pkt)
    {
        std::vector<uint8_t> buffer(pkt->GetSize());
        std::ostringstream oss(std::ostringstream::out);
        pkt->CopyData(buffer.data(), buffer.size());
        for (auto b : buffer)
        {
            oss << (std::bitset<8>(b));
        }
        return std::string(oss.str() + "\n");
    }

    /**
     * Function to log packet contents
     * \param pkt the packet
     */
    static void LogPacketContents(Ptr<Packet> pkt)
    {
        NS_LOG_DEBUG("---- SERIALIZED PACKET CONTENTS (HEX): -------");
        NS_LOG_DEBUG("Hex: " << TestUtils::sprintPacketContentsHex(pkt));
        NS_LOG_DEBUG("Bin: " << TestUtils::sprintPacketContentsBin(pkt));
    }

    /**
     * Log packet info function
     * \param source T
     * \param s text string to log
     */
    template <class T>
    static void LogPacketInfo(T source, std::string s)
    {
        NS_LOG_DEBUG("--------- " << s.data() << " INFO: -------");
        std::ostringstream oss(std::ostringstream::out);
        source.Print(oss);
        NS_LOG_DEBUG(oss.str());
    }
};

/**
 * \ingroup nr-test
 *
 * \brief Rlc Am Status Pdu Test Case
 */
class NrRlcAmStatusPduTestCase : public TestCase
{
  public:
    /**
     * Constructor
     *
     * \param ackSn the sequence number
     * \param nackSnList list of nack sequence numbers
     * \param hex string
     */
    NrRlcAmStatusPduTestCase(nr::SequenceNumber10 ackSn,
                             std::list<nr::SequenceNumber10> nackSnList,
                             std::string hex);

  protected:
    void DoRun() override;

    nr::SequenceNumber10 m_ackSn;                 ///< ack sequence number
    std::list<nr::SequenceNumber10> m_nackSnList; ///< list of nack sequence numbers
    std::string m_hex;                            ///< hex string
};

NrRlcAmStatusPduTestCase::NrRlcAmStatusPduTestCase(nr::SequenceNumber10 ackSn,
                                                   std::list<nr::SequenceNumber10> nackSnList,
                                                   std::string hex)
    : TestCase(hex),
      m_ackSn(ackSn),
      m_nackSnList(nackSnList),
      m_hex(hex)
{
    NS_LOG_FUNCTION(this << hex);
}

void

NrRlcAmStatusPduTestCase::DoRun()
{
    NS_LOG_FUNCTION(this);

    Ptr<Packet> p = Create<Packet>();
    NrRlcAmHeader h;
    h.SetControlPdu(NrRlcAmHeader::STATUS_PDU);
    h.SetAckSn(m_ackSn);
    for (auto it = m_nackSnList.begin(); it != m_nackSnList.end(); ++it)
    {
        h.PushNack(it->GetValue());
    }
    p->AddHeader(h);

    TestUtils::LogPacketContents(p);
    std::string hex = TestUtils::sprintPacketContentsHex(p);
    NS_TEST_ASSERT_MSG_EQ(m_hex,
                          hex,
                          "serialized packet content " << hex << " differs from test vector "
                                                       << m_hex);

    NrRlcAmHeader h2;
    p->RemoveHeader(h2);
    nr::SequenceNumber10 ackSn = h2.GetAckSn();
    NS_TEST_ASSERT_MSG_EQ(ackSn, m_ackSn, "deserialized ACK SN differs from test vector");

    for (auto it = m_nackSnList.begin(); it != m_nackSnList.end(); ++it)
    {
        int nackSn = h2.PopNack();
        NS_TEST_ASSERT_MSG_GT(nackSn, -1, "not enough elements in deserialized NACK list");
        NS_TEST_ASSERT_MSG_EQ(nackSn,
                              it->GetValue(),
                              "deserialized NACK SN  differs from test vector");
    }
    int retVal = h2.PopNack();
    NS_TEST_ASSERT_MSG_LT(retVal, 0, "too many elements in deserialized NACK list");
}

/**
 * \ingroup nr-test
 *
 * \brief Nr Rlc Header Test Suite
 */
class NrRlcHeaderTestSuite : public TestSuite
{
  public:
    NrRlcHeaderTestSuite();
} staticNrRlcHeaderTestSuiteInstance; ///< the test suite

NrRlcHeaderTestSuite::NrRlcHeaderTestSuite()
    : TestSuite("nr-rlc-header", Type::UNIT)
{
    NS_LOG_FUNCTION(this);

    {
        nr::SequenceNumber10 ackSn(8);
        std::list<nr::SequenceNumber10> nackSnList;
        std::string hex("0020");
        AddTestCase(new NrRlcAmStatusPduTestCase(ackSn, nackSnList, hex),
                    TestCase::Duration::QUICK);
    }

    {
        nr::SequenceNumber10 ackSn(873);
        std::list<nr::SequenceNumber10> nackSnList;
        std::string hex("0da4");
        AddTestCase(new NrRlcAmStatusPduTestCase(ackSn, nackSnList, hex),
                    TestCase::Duration::QUICK);
    }

    {
        nr::SequenceNumber10 ackSn(2);
        const std::list<nr::SequenceNumber10> nackSnList{
            nr::SequenceNumber10(873),
        };
        std::string hex("000bb480");
        AddTestCase(new NrRlcAmStatusPduTestCase(ackSn, nackSnList, hex),
                    TestCase::Duration::QUICK);
    }

    {
        nr::SequenceNumber10 ackSn(2);
        const std::list<nr::SequenceNumber10> nackSnList{
            nr::SequenceNumber10(1021),
            nr::SequenceNumber10(754),
        };
        std::string hex("000bfed790");
        AddTestCase(new NrRlcAmStatusPduTestCase(ackSn, nackSnList, hex),
                    TestCase::Duration::QUICK);
    }

    {
        nr::SequenceNumber10 ackSn(2);
        const std::list<nr::SequenceNumber10> nackSnList{
            nr::SequenceNumber10(1021),
            nr::SequenceNumber10(754),
            nr::SequenceNumber10(947),
        };
        std::string hex("000bfed795d980");
        AddTestCase(new NrRlcAmStatusPduTestCase(ackSn, nackSnList, hex),
                    TestCase::Duration::QUICK);
    }

    {
        nr::SequenceNumber10 ackSn(2);
        const std::list<nr::SequenceNumber10> nackSnList{
            nr::SequenceNumber10(1021),
            nr::SequenceNumber10(754),
            nr::SequenceNumber10(947),
            nr::SequenceNumber10(347),
        };
        std::string hex("000bfed795d9cad8");
        AddTestCase(new NrRlcAmStatusPduTestCase(ackSn, nackSnList, hex),
                    TestCase::Duration::QUICK);
    }
}

} // namespace ns3

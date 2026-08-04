// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include <ns3/nr-mac-short-bsr-ce.h>
#include <ns3/test.h>

/**
 * \file test-nr-mac-vs-header.cc
 * \ingroup test
 * \brief Unit-testing for the variable-size MAC header, DL and UL
 *
 */
namespace ns3
{

/**
 * \brief
 */
class NrMacShortBsrCeTest : public TestCase
{
  public:
    /**
     * \brief Constructor
     * \param name Name of the test
     */
    NrMacShortBsrCeTest(const std::string& name)
        : TestCase(name)
    {
    }

  private:
    void DoRun() override;
};

void
NrMacShortBsrCeTest::DoRun()
{
    Ptr<Packet> applicationData = Create<Packet>();

    Packet::EnablePrinting();
    Packet::EnableChecking();

    Ptr<Packet> pdu = Create<Packet>();

    {
        NrMacShortBsrCe bsr;
        bsr.m_bufferSizeLevel_0 = NrMacShortBsrCe::FromBytesToLevel(12);
        bsr.m_bufferSizeLevel_1 = NrMacShortBsrCe::FromBytesToLevel(400);
        bsr.m_bufferSizeLevel_2 = NrMacShortBsrCe::FromBytesToLevel(5400);
        bsr.m_bufferSizeLevel_3 = NrMacShortBsrCe::FromBytesToLevel(500000);

        pdu->AddHeader(bsr);
    }

    std::cout << " the pdu is: ";
    pdu->Print(std::cout);
    std::cout << std::endl;

    // Inside our PDU, there is one subPDU composed by our header: { [HEADER] }
    // Let's try to deserialize it, checking if it's the same

    {
        NrMacShortBsrCe bsr;

        pdu->RemoveHeader(bsr);

        std::cout << "the SDU is: ";
        bsr.Print(std::cout);
        std::cout << std::endl;

        NS_TEST_ASSERT_MSG_EQ(bsr.m_bufferSizeLevel_0,
                              NrMacShortBsrCe::FromBytesToLevel(12),
                              "Deserialize failed for BufferLevel");
        NS_TEST_ASSERT_MSG_EQ(bsr.m_bufferSizeLevel_1,
                              NrMacShortBsrCe::FromBytesToLevel(400),
                              "Deserialize failed for BufferLevel");
        NS_TEST_ASSERT_MSG_EQ(bsr.m_bufferSizeLevel_2,
                              NrMacShortBsrCe::FromBytesToLevel(5400),
                              "Deserialize failed for BufferLevel");
        NS_TEST_ASSERT_MSG_EQ(bsr.m_bufferSizeLevel_3,
                              NrMacShortBsrCe::FromBytesToLevel(500000),
                              "Deserialize failed for BufferLevel");
    }
}

class NrMacShortBsrCeTestSuite : public TestSuite
{
  public:
    NrMacShortBsrCeTestSuite()
        : TestSuite("nr-mac-short-bsr-ce-test", Type::UNIT)
    {
        AddTestCase(new NrMacShortBsrCeTest("Short BSR CE test"), Duration::QUICK);
    }
};

static NrMacShortBsrCeTestSuite nrMacShortBsrCeTestSuite;

} // namespace ns3

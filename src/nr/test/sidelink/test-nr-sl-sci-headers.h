// Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef TEST_NR_SL_SCI_HEADERS
#define TEST_NR_SL_SCI_HEADERS

#include <ns3/nr-sl-sci-f1a-header.h>
#include <ns3/nr-sl-sci-f2a-header.h>
#include <ns3/test.h>

using namespace ns3;

/**
 * \brief Test suite for
 *
 * \sa ns3::NrSlSciF1aTestCase
 * \sa ns3::NrSlSciF2aTestCase
 */
class NrSlSciHeadersTestSuite : public TestSuite
{
  public:
    NrSlSciHeadersTestSuite();
};

/**
 * \ingroup nr
 *
 * \brief Testing NR Sidelink SCI format 1A header for it correct serialization
 *        and deserialization
 */
class NrSlSciF1aTestCase : public TestCase
{
  public:
    /**
     * \brief Creates an instance of the NR Sidelink SCI Format 1A test case.

     * \param sciF1a SCI format 1A header
     * \param expectedHeaderSize The expected size of the header
     */
    NrSlSciF1aTestCase(NrSlSciF1aHeader sciF1a, uint16_t expectedHeaderSize);

    ~NrSlSciF1aTestCase() override;

  private:
    /**
     * \brief Builds the test name string based on provided parameter values
     * \param sciF1a the SCI format 1A header
     * \param expectedHeaderSize The expected header size
     * \returns the name string
     */
    std::string BuildNameString(const NrSlSciF1aHeader& sciF1a, uint16_t expectedHeaderSize);
    /**
     * \brief Setup the simulation according to the configuration set by the
     *        class constructor, run it, and verify the result.
     */
    void DoRun() override;

    NrSlSciF1aHeader m_sciF1a;     //!< SCI format 1A header
    uint16_t m_expectedHeaderSize; //!< The expected header size

}; // end of class NrSlSciF1aTestCase

/**
 * \ingroup nr
 *
 * \brief Testing NR Sidelink SCI format 2A header for it correct serialization
 *        and deserialization
 */
class NrSlSciF2aTestCase : public TestCase
{
  public:
    /**
     * \brief Creates an instance of the NR Sidelink SCI Format 02 test case.

     * \param sciF2a SCI format 2A header
     * \param expectedHeaderSize The expected size of the header
     */
    NrSlSciF2aTestCase(NrSlSciF2aHeader sciF2a, uint16_t expectedHeaderSize);

    ~NrSlSciF2aTestCase() override;

  private:
    /**
     * \brief Builds the test name string based on provided parameter values
     * \param sciF2a the SCI format 02 header
     * \param expectedHeaderSize The expected header size
     * \returns the name string
     */
    std::string BuildNameString(const NrSlSciF2aHeader& sciF2a, uint16_t expectedHeaderSize);
    /**
     * \brief Setup the simulation according to the configuration set by the
     *        class constructor, run it, and verify the result.
     */
    void DoRun() override;

    NrSlSciF2aHeader m_sciF2a;     //!< SCI format 02 header
    uint16_t m_expectedHeaderSize; //!< The expected header size

}; // end of class NrSlSciF2aTestCase

#endif /* TEST_NR_SL_SCI_HEADERS */

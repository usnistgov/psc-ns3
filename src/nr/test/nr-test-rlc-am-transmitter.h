/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#ifndef NR_TEST_RLC_AM_TRANSMITTER_H
#define NR_TEST_RLC_AM_TRANSMITTER_H

#include "ns3/nstime.h"
#include "ns3/ptr.h"
#include "ns3/test.h"

namespace ns3
{

class NrTestRrc;
class NrTestMac;
class NrTestPdcp;
class NrRlc;

} // namespace ns3

using namespace ns3;

/**
 * \ingroup nr-test
 *
 * \brief TestSuite 4.1.1 RLC AM: Only transmitter functionality.
 */
class NrRlcAmTransmitterTestSuite : public TestSuite
{
  public:
    NrRlcAmTransmitterTestSuite();
};

/**
 * \ingroup nr-test
 *
 * \brief Test case used by NrRlcAmTransmitterOneSduTestCase to create topology
 * and to implement functionalities and check if data received corresponds to
 * data sent.
 */
class NrRlcAmTransmitterTestCase : public TestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the reference name
     */
    NrRlcAmTransmitterTestCase(std::string name);
    NrRlcAmTransmitterTestCase();
    ~NrRlcAmTransmitterTestCase() override;

    /**
     * Check data received function
     * \param time the time to check
     * \param shouldReceived should have received indicator
     * \param assertMsg the assert message
     */
    void CheckDataReceived(Time time, std::string shouldReceived, std::string assertMsg);

  protected:
    void DoRun() override;

    Ptr<NrTestPdcp> txPdcp; ///< the transmit PDCP
    Ptr<NrRlc> txRlc;       ///< the RLC
    Ptr<NrTestMac> txMac;   ///< the MAC

  private:
    /**
     * Check data received function
     * \param shouldReceived should have received indicator
     * \param assertMsg the assert message
     */
    void DoCheckDataReceived(std::string shouldReceived, std::string assertMsg);
};

/**
 * \ingroup nr-test
 *
 * \brief Test 4.1.1.1 Test that SDU transmitted at PDCP corresponds to PDU
 * received by MAC.
 */
class NrRlcAmTransmitterOneSduTestCase : public NrRlcAmTransmitterTestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the reference name
     */
    NrRlcAmTransmitterOneSduTestCase(std::string name);
    NrRlcAmTransmitterOneSduTestCase();
    ~NrRlcAmTransmitterOneSduTestCase() override;

  private:
    void DoRun() override;
};

/**
 * \ingroup nr-test
 *
 * \brief Test 4.1.1.2 Test the correct functionality of the Segmentation.
 * Test check that single SDU is properly segmented to n PDUs.
 */
class NrRlcAmTransmitterSegmentationTestCase : public NrRlcAmTransmitterTestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the reference name
     */
    NrRlcAmTransmitterSegmentationTestCase(std::string name);
    NrRlcAmTransmitterSegmentationTestCase();
    ~NrRlcAmTransmitterSegmentationTestCase() override;

  private:
    void DoRun() override;
};

/**
 * \ingroup nr-test
 *
 * \brief Test 4.1.1.3 Test that concatenation functionality works properly.
 * Test check if n SDUs are correctly contactenate to single PDU.
 */
class NrRlcAmTransmitterConcatenationTestCase : public NrRlcAmTransmitterTestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the reference name
     */
    NrRlcAmTransmitterConcatenationTestCase(std::string name);
    NrRlcAmTransmitterConcatenationTestCase();
    ~NrRlcAmTransmitterConcatenationTestCase() override;

  private:
    void DoRun() override;
};

/**
 * \ingroup nr-test
 *
 * \brief Test 4.1.1.4 Test checks functionality of Report Buffer Status by
 * testing primitive parameters.
 */
class NrRlcAmTransmitterReportBufferStatusTestCase : public NrRlcAmTransmitterTestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the reference name
     */
    NrRlcAmTransmitterReportBufferStatusTestCase(std::string name);
    NrRlcAmTransmitterReportBufferStatusTestCase();
    ~NrRlcAmTransmitterReportBufferStatusTestCase() override;

  private:
    void DoRun() override;
};

#endif // NR_TEST_RLC_AM_TRANSMITTER_H

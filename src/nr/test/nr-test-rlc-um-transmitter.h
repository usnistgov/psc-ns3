/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Manuel Requena <manuel.requena@cttc.es>
 */

#ifndef NR_TEST_RLC_UM_TRANSMITTER_H
#define NR_TEST_RLC_UM_TRANSMITTER_H

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
 * \brief TestSuite 4.1.1 for RLC UM: Only transmitter part.
 */
class NrRlcUmTransmitterTestSuite : public TestSuite
{
  public:
    NrRlcUmTransmitterTestSuite();
};

/**
 * \ingroup nr-test
 *
 * \brief Test case used by NrRlcUmTransmitterOneSduTestCase to create topology
 * and to implement functionalities and check if data received corresponds to
 * data sent.
 */
class NrRlcUmTransmitterTestCase : public TestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the test name
     */
    NrRlcUmTransmitterTestCase(std::string name);
    NrRlcUmTransmitterTestCase();
    ~NrRlcUmTransmitterTestCase() override;

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
 * \brief Test 4.1.1.1 One SDU, One PDU
 */
class NrRlcUmTransmitterOneSduTestCase : public NrRlcUmTransmitterTestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the test name
     */
    NrRlcUmTransmitterOneSduTestCase(std::string name);
    NrRlcUmTransmitterOneSduTestCase();
    ~NrRlcUmTransmitterOneSduTestCase() override;

  private:
    void DoRun() override;
};

/**
 * \ingroup nr-test
 *
 * \brief Test 4.1.1.2 Segmentation (One SDU => n PDUs)
 */
class NrRlcUmTransmitterSegmentationTestCase : public NrRlcUmTransmitterTestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the reference name
     */
    NrRlcUmTransmitterSegmentationTestCase(std::string name);
    NrRlcUmTransmitterSegmentationTestCase();
    ~NrRlcUmTransmitterSegmentationTestCase() override;

  private:
    void DoRun() override;
};

/**
 * \ingroup nr-test
 *
 * \brief Test 4.1.1.3 Concatenation (n SDUs => One PDU)
 */
class NrRlcUmTransmitterConcatenationTestCase : public NrRlcUmTransmitterTestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the reference name
     */
    NrRlcUmTransmitterConcatenationTestCase(std::string name);
    NrRlcUmTransmitterConcatenationTestCase();
    ~NrRlcUmTransmitterConcatenationTestCase() override;

  private:
    void DoRun() override;
};

/**
 * \ingroup nr-test
 *
 * \brief Test 4.1.1.4 Report Buffer Status (test primitive parameters)
 */
class NrRlcUmTransmitterReportBufferStatusTestCase : public NrRlcUmTransmitterTestCase
{
  public:
    /**
     * Constructor
     *
     * \param name the reference name
     */
    NrRlcUmTransmitterReportBufferStatusTestCase(std::string name);
    NrRlcUmTransmitterReportBufferStatusTestCase();
    ~NrRlcUmTransmitterReportBufferStatusTestCase() override;

  private:
    void DoRun() override;
};

#endif /* NR_TEST_RLC_UM_TRANSMITTER_H */

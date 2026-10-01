/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Jaume Nin <jaume.nin@cttc.cat>
 */

#ifndef NR_EPC_TEST_GTPU_H
#define NR_EPC_TEST_GTPU_H

#include "ns3/nr-epc-gtpu-header.h"
#include "ns3/test.h"

using namespace ns3;

/**
 * \ingroup nr
 * \ingroup tests
 * \defgroup nr-test nr module tests
 */

/**
 * \ingroup nr-test
 *
 * \brief Test suite for testing GPRS tunnelling protocol header coding and decoding.
 */
class NrEpsGtpuTestSuite : public TestSuite
{
  public:
    NrEpsGtpuTestSuite();
};

/**
 * Test 1.Check header coding and decoding
 */
class NrEpsGtpuHeaderTestCase : public TestCase
{
  public:
    NrEpsGtpuHeaderTestCase();
    ~NrEpsGtpuHeaderTestCase() override;

  private:
    void DoRun() override;
};

#endif /* NR_EPC_TEST_GTPU_H */

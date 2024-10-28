/*
 * Copyright (c) 2009 IITP RAS
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Pavel Boyko <boyko@iitp.ru>
 */

#include "olsrv2-bug780-test.h"
#include "olsrv2-hello-regression-test.h"
#include "olsrv2-tc-regression-test.h"

using namespace ns3;
using namespace olsrv2;

/**
 * \ingroup olsr-test
 * \ingroup tests
 *
 * Various olsr regression tests
 */
class RegressionTestSuite : public TestSuite
{
  public:
    RegressionTestSuite()
        : TestSuite("routing-olsrv2-regression", Type::SYSTEM)
    {
        SetDataDir(NS_TEST_SOURCEDIR);
        AddTestCase(new HelloRegressionTest, TestCase::Duration::QUICK);
        AddTestCase(new TcRegressionTest, TestCase::Duration::QUICK);
        AddTestCase(new Bug780Test, TestCase::Duration::QUICK);
    }
};

static RegressionTestSuite g_olsrv2RegressionTestSuite; //!< Static variable for test initialization

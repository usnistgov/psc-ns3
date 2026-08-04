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

namespace ns3
{
namespace olsrv2
{

/**
 * @ingroup olsrv2-test
 * @ingroup tests
 *
 * Various olsrv2 regression tests
 */
class RegressionTestSuite : public TestSuite
{
  public:
    RegressionTestSuite()
        : TestSuite("routing-olsrv2-regression", Type::SYSTEM)
    {
        SetDataDir(NS_TEST_SOURCEDIR);
        AddTestCase(new Olsrv2HelloRegressionTest, TestCase::Duration::QUICK);
        AddTestCase(new Olsrv2TcRegressionTest, TestCase::Duration::QUICK);
        AddTestCase(new Olsrv2Bug780Test, TestCase::Duration::QUICK);
    }
};
} // namespace olsrv2
} // namespace ns3

static ns3::olsrv2::RegressionTestSuite
    g_olsrv2RegressionTestSuite; //!< Static variable for test initialization

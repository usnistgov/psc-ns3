// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include <ns3/sfnsf.h>
#include <ns3/test.h>

/**
 * \file test-sfnsf.cc
 * \ingroup test
 *
 * \brief Unit-testing for the frame/subframe/slot numbering, along with the
 * numerology. The test checks that the normalized slot number equals a
 * monotonically-increased integer, for every numerology.
 */
namespace ns3
{

class TestSfnSfTestCase : public TestCase
{
  public:
    TestSfnSfTestCase(uint16_t num, const std::string& name)
        : TestCase(name),
          m_numerology(num)
    {
    }

  private:
    void DoRun() override;
    uint16_t m_numerology{0};
};

void
TestSfnSfTestCase::DoRun()
{
    SfnSf sfn(0, 0, 0, m_numerology);

    for (uint32_t i = 0; i < 9999; ++i)
    {
        NS_TEST_ASSERT_MSG_EQ(sfn.Normalize(), i, "Mm");
        sfn.Add(1);
    }
}

class TestSfnSf : public TestSuite
{
  public:
    TestSfnSf()
        : TestSuite("nr-test-sfnsf", Type::UNIT)
    {
        AddTestCase(new TestSfnSfTestCase(0, "SfnSf TestAdd with num 2"), Duration::QUICK);
        AddTestCase(new TestSfnSfTestCase(1, "SfnSf TestAdd with num 2"), Duration::QUICK);
        AddTestCase(new TestSfnSfTestCase(2, "SfnSf TestAdd with num 2"), Duration::QUICK);
        AddTestCase(new TestSfnSfTestCase(3, "SfnSf TestAdd with num 2"), Duration::QUICK);
        AddTestCase(new TestSfnSfTestCase(4, "SfnSf TestAdd with num 2"), Duration::QUICK);
    }
};

static TestSfnSf testSfnSf; //!< SfnSf test

} // namespace ns3

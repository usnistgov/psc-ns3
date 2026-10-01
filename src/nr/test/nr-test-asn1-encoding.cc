/*
 * Copyright (c) 2011, 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Lluis Parcerisa <lparcerisa@cttc.cat>
 */

#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/enum.h"
#include "ns3/log.h"
#include "ns3/nr-rrc-header.h"
#include "ns3/nr-rrc-sap.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/string.h"
#include "ns3/test.h"

#include <iomanip>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrAsn1EncodingTest");

/**
 * \ingroup nr-test
 *
 * \brief Contains ASN encoding test utility functions.
 */
class TestUtils
{
  public:
    /**
     * Function to convert packet contents in hex format
     * \param pkt the packet
     * \returns the text string
     */
    static std::string sprintPacketContentsHex(Ptr<Packet> pkt)
    {
        std::vector<uint8_t> buffer(pkt->GetSize());
        std::ostringstream oss(std::ostringstream::out);
        pkt->CopyData(buffer.data(), buffer.size());
        for (auto b : buffer)
        {
            oss << std::setfill('0') << std::setw(2) << std::hex << +b << " ";
        }
        return std::string(oss.str() + "\n");
    }

    /**
     * Function to convert packet contents in binary format
     * \param pkt the packet
     * \returns the text string
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
     * Function to log packet info
     * \param source T
     * \param s the string
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

// --------------------------- CLASS RrcHeaderTestCase -----------------------------
/**
 * \ingroup nr-test
 *
 * \brief This class provides common functions to be inherited
 * by the children TestCases
 */
class RrcHeaderTestCase : public TestCase
{
  public:
    /**
     * Constructor
     * \param s the reference name
     */
    RrcHeaderTestCase(std::string s);
    void DoRun() override = 0;
    /**
     * \brief Create radio resource config dedicated
     * \returns NrRrcSap::RadioResourceConfigDedicated
     */
    NrRrcSap::RadioResourceConfigDedicated CreateRadioResourceConfigDedicated();
    /**
     * \brief Assert equal radio resource config dedicated
     * \param rrcd1 NrRrcSap::RadioResourceConfigDedicated # 1
     * \param rrcd2 NrRrcSap::RadioResourceConfigDedicated # 2
     */
    void AssertEqualRadioResourceConfigDedicated(NrRrcSap::RadioResourceConfigDedicated rrcd1,
                                                 NrRrcSap::RadioResourceConfigDedicated rrcd2);

  protected:
    Ptr<Packet> packet; ///< the packet
};

RrcHeaderTestCase::RrcHeaderTestCase(std::string s)
    : TestCase(s)
{
}

NrRrcSap::RadioResourceConfigDedicated
RrcHeaderTestCase::CreateRadioResourceConfigDedicated()
{
    NrRrcSap::RadioResourceConfigDedicated rrd;

    rrd.drbToReleaseList = std::list<uint8_t>(4, 2);

    NrRrcSap::SrbToAddMod srbToAddMod;
    srbToAddMod.srbIdentity = 2;

    NrRrcSap::LogicalChannelConfig logicalChannelConfig;
    logicalChannelConfig.priority = 9;
    logicalChannelConfig.prioritizedBitRateKbps = 128;
    logicalChannelConfig.bucketSizeDurationMs = 100;
    logicalChannelConfig.logicalChannelGroup = 3;
    srbToAddMod.logicalChannelConfig = logicalChannelConfig;

    rrd.srbToAddModList.insert(rrd.srbToAddModList.begin(), srbToAddMod);

    NrRrcSap::DrbToAddMod drbToAddMod;
    drbToAddMod.epsBearerIdentity = 1;
    drbToAddMod.drbIdentity = 1;
    drbToAddMod.logicalChannelIdentity = 5;
    NrRrcSap::RlcConfig rlcConfig;
    rlcConfig.choice = NrRrcSap::RlcConfig::UM_BI_DIRECTIONAL;
    drbToAddMod.rlcConfig = rlcConfig;

    NrRrcSap::LogicalChannelConfig logicalChannelConfig2;
    logicalChannelConfig2.priority = 7;
    logicalChannelConfig2.prioritizedBitRateKbps = 256;
    logicalChannelConfig2.bucketSizeDurationMs = 50;
    logicalChannelConfig2.logicalChannelGroup = 2;
    drbToAddMod.logicalChannelConfig = logicalChannelConfig2;

    rrd.drbToAddModList.insert(rrd.drbToAddModList.begin(), drbToAddMod);

    rrd.havePhysicalConfigDedicated = true;
    NrRrcSap::PhysicalConfigDedicated physicalConfigDedicated;
    physicalConfigDedicated.haveSoundingRsUlConfigDedicated = true;
    physicalConfigDedicated.soundingRsUlConfigDedicated.type =
        NrRrcSap::SoundingRsUlConfigDedicated::SETUP;
    physicalConfigDedicated.soundingRsUlConfigDedicated.srsBandwidth = 2;
    physicalConfigDedicated.soundingRsUlConfigDedicated.srsConfigIndex = 12;

    physicalConfigDedicated.haveAntennaInfoDedicated = true;
    physicalConfigDedicated.antennaInfo.transmissionMode = 2;

    physicalConfigDedicated.havePdschConfigDedicated = true;
    physicalConfigDedicated.pdschConfigDedicated.pa = NrRrcSap::PdschConfigDedicated::dB0;

    rrd.physicalConfigDedicated = physicalConfigDedicated;

    return rrd;
}

void
RrcHeaderTestCase::AssertEqualRadioResourceConfigDedicated(
    NrRrcSap::RadioResourceConfigDedicated rrcd1,
    NrRrcSap::RadioResourceConfigDedicated rrcd2)
{
    NS_TEST_ASSERT_MSG_EQ(rrcd1.srbToAddModList.size(),
                          rrcd2.srbToAddModList.size(),
                          "SrbToAddModList different sizes");

    std::list<NrRrcSap::SrbToAddMod> srcSrbToAddModList = rrcd1.srbToAddModList;
    auto it1 = srcSrbToAddModList.begin();
    std::list<NrRrcSap::SrbToAddMod> dstSrbToAddModList = rrcd2.srbToAddModList;
    auto it2 = dstSrbToAddModList.begin();

    for (; it1 != srcSrbToAddModList.end(); it1++, it2++)
    {
        NS_TEST_ASSERT_MSG_EQ(it1->srbIdentity, it2->srbIdentity, "srbIdentity");
        NS_TEST_ASSERT_MSG_EQ(it1->logicalChannelConfig.priority,
                              it2->logicalChannelConfig.priority,
                              "logicalChannelConfig.priority");
        NS_TEST_ASSERT_MSG_EQ(it1->logicalChannelConfig.prioritizedBitRateKbps,
                              it2->logicalChannelConfig.prioritizedBitRateKbps,
                              "logicalChannelConfig.prioritizedBitRateKbps");
        NS_TEST_ASSERT_MSG_EQ(it1->logicalChannelConfig.bucketSizeDurationMs,
                              it2->logicalChannelConfig.bucketSizeDurationMs,
                              "logicalChannelConfig.bucketSizeDurationMs");
        NS_TEST_ASSERT_MSG_EQ(it1->logicalChannelConfig.logicalChannelGroup,
                              it2->logicalChannelConfig.logicalChannelGroup,
                              "logicalChannelConfig.logicalChannelGroup");
    }

    NS_TEST_ASSERT_MSG_EQ(rrcd1.drbToAddModList.size(),
                          rrcd2.drbToAddModList.size(),
                          "DrbToAddModList different sizes");

    std::list<NrRrcSap::DrbToAddMod> srcDrbToAddModList = rrcd1.drbToAddModList;
    auto it3 = srcDrbToAddModList.begin();
    std::list<NrRrcSap::DrbToAddMod> dstDrbToAddModList = rrcd2.drbToAddModList;
    auto it4 = dstDrbToAddModList.begin();

    for (; it3 != srcDrbToAddModList.end(); it3++, it4++)
    {
        NS_TEST_ASSERT_MSG_EQ(it3->epsBearerIdentity, it4->epsBearerIdentity, "epsBearerIdentity");
        NS_TEST_ASSERT_MSG_EQ(it3->drbIdentity, it4->drbIdentity, "drbIdentity");
        NS_TEST_ASSERT_MSG_EQ(it3->rlcConfig.choice, it4->rlcConfig.choice, "rlcConfig.choice");
        NS_TEST_ASSERT_MSG_EQ(it3->logicalChannelIdentity,
                              it4->logicalChannelIdentity,
                              "logicalChannelIdentity");
        NS_TEST_ASSERT_MSG_EQ(it3->epsBearerIdentity, it4->epsBearerIdentity, "epsBearerIdentity");

        NS_TEST_ASSERT_MSG_EQ(it3->logicalChannelConfig.priority,
                              it4->logicalChannelConfig.priority,
                              "logicalChannelConfig.priority");
        NS_TEST_ASSERT_MSG_EQ(it3->logicalChannelConfig.prioritizedBitRateKbps,
                              it4->logicalChannelConfig.prioritizedBitRateKbps,
                              "logicalChannelConfig.prioritizedBitRateKbps");
        NS_TEST_ASSERT_MSG_EQ(it3->logicalChannelConfig.bucketSizeDurationMs,
                              it4->logicalChannelConfig.bucketSizeDurationMs,
                              "logicalChannelConfig.bucketSizeDurationMs");
        NS_TEST_ASSERT_MSG_EQ(it3->logicalChannelConfig.logicalChannelGroup,
                              it4->logicalChannelConfig.logicalChannelGroup,
                              "logicalChannelConfig.logicalChannelGroup");
    }

    NS_TEST_ASSERT_MSG_EQ(rrcd1.drbToReleaseList.size(),
                          rrcd2.drbToReleaseList.size(),
                          "DrbToReleaseList different sizes");

    std::list<uint8_t> srcDrbToReleaseList = rrcd1.drbToReleaseList;
    std::list<uint8_t> dstDrbToReleaseList = rrcd2.drbToReleaseList;
    auto it5 = srcDrbToReleaseList.begin();
    auto it6 = dstDrbToReleaseList.begin();

    for (; it5 != srcDrbToReleaseList.end(); it5++, it6++)
    {
        NS_TEST_ASSERT_MSG_EQ(*it5, *it6, "element != in DrbToReleaseList");
    }

    NS_TEST_ASSERT_MSG_EQ(rrcd1.havePhysicalConfigDedicated,
                          rrcd2.havePhysicalConfigDedicated,
                          "HavePhysicalConfigDedicated");

    if (rrcd1.havePhysicalConfigDedicated)
    {
        NS_TEST_ASSERT_MSG_EQ(rrcd1.physicalConfigDedicated.haveSoundingRsUlConfigDedicated,
                              rrcd2.physicalConfigDedicated.haveSoundingRsUlConfigDedicated,
                              "haveSoundingRsUlConfigDedicated");

        NS_TEST_ASSERT_MSG_EQ(rrcd1.physicalConfigDedicated.soundingRsUlConfigDedicated.type,
                              rrcd2.physicalConfigDedicated.soundingRsUlConfigDedicated.type,
                              "soundingRsUlConfigDedicated.type");
        NS_TEST_ASSERT_MSG_EQ(
            rrcd1.physicalConfigDedicated.soundingRsUlConfigDedicated.srsBandwidth,
            rrcd2.physicalConfigDedicated.soundingRsUlConfigDedicated.srsBandwidth,
            "soundingRsUlConfigDedicated.srsBandwidth");

        NS_TEST_ASSERT_MSG_EQ(
            rrcd1.physicalConfigDedicated.soundingRsUlConfigDedicated.srsConfigIndex,
            rrcd2.physicalConfigDedicated.soundingRsUlConfigDedicated.srsConfigIndex,
            "soundingRsUlConfigDedicated.srsConfigIndex");

        NS_TEST_ASSERT_MSG_EQ(rrcd1.physicalConfigDedicated.haveAntennaInfoDedicated,
                              rrcd2.physicalConfigDedicated.haveAntennaInfoDedicated,
                              "haveAntennaInfoDedicated");

        if (rrcd1.physicalConfigDedicated.haveAntennaInfoDedicated)
        {
            NS_TEST_ASSERT_MSG_EQ(rrcd1.physicalConfigDedicated.antennaInfo.transmissionMode,
                                  rrcd2.physicalConfigDedicated.antennaInfo.transmissionMode,
                                  "antennaInfo.transmissionMode");
        }

        NS_TEST_ASSERT_MSG_EQ(rrcd1.physicalConfigDedicated.havePdschConfigDedicated,
                              rrcd2.physicalConfigDedicated.havePdschConfigDedicated,
                              "havePdschConfigDedicated");

        if (rrcd1.physicalConfigDedicated.havePdschConfigDedicated)
        {
            NS_TEST_ASSERT_MSG_EQ(rrcd1.physicalConfigDedicated.pdschConfigDedicated.pa,
                                  rrcd2.physicalConfigDedicated.pdschConfigDedicated.pa,
                                  "pdschConfigDedicated.pa");
        }
    }
}

/**
 * \ingroup nr-test
 *
 * \brief Rrc Connection Request Test Case
 */
class NrRrcConnectionRequestTestCase : public RrcHeaderTestCase
{
  public:
    NrRrcConnectionRequestTestCase();
    void DoRun() override;
};

NrRrcConnectionRequestTestCase::NrRrcConnectionRequestTestCase()
    : RrcHeaderTestCase("Testing RrcConnectionRequest")
{
}

void
NrRrcConnectionRequestTestCase::DoRun()
{
    packet = Create<Packet>();
    NS_LOG_DEBUG("============= NrRrcConnectionRequestTestCase ===========");

    NrRrcSap::RrcConnectionRequest msg;
    msg.ueIdentity = 0x83fecafecaULL;

    NrRrcConnectionRequestHeader source;
    source.SetMessage(msg);

    // Log source info
    TestUtils::LogPacketInfo<NrRrcConnectionRequestHeader>(source, "SOURCE");

    // Add header
    packet->AddHeader(source);

    // Log serialized packet contents
    TestUtils::LogPacketContents(packet);

    // Remove header
    NrRrcConnectionRequestHeader destination;
    packet->RemoveHeader(destination);

    // Log destination info
    TestUtils::LogPacketInfo<NrRrcConnectionRequestHeader>(destination, "DESTINATION");

    // Check that the destination and source headers contain the same values
    NS_TEST_ASSERT_MSG_EQ(source.GetMmec(), destination.GetMmec(), "Different m_mmec!");
    NS_TEST_ASSERT_MSG_EQ(source.GetMtmsi(), destination.GetMtmsi(), "Different m_mTmsi!");

    packet = nullptr;
}

/**
 * \ingroup nr-test
 *
 * \brief Rrc Connection Setup Test Case
 */
class NrRrcConnectionSetupTestCase : public RrcHeaderTestCase
{
  public:
    NrRrcConnectionSetupTestCase();
    void DoRun() override;
};

NrRrcConnectionSetupTestCase::NrRrcConnectionSetupTestCase()
    : RrcHeaderTestCase("Testing NrRrcConnectionSetupTestCase")
{
}

void
NrRrcConnectionSetupTestCase::DoRun()
{
    packet = Create<Packet>();
    NS_LOG_DEBUG("============= NrRrcConnectionSetupTestCase ===========");

    NrRrcSap::RrcConnectionSetup msg;
    msg.rrcTransactionIdentifier = 3;
    msg.radioResourceConfigDedicated = CreateRadioResourceConfigDedicated();

    NrRrcConnectionSetupHeader source;
    source.SetMessage(msg);

    // Log source info
    TestUtils::LogPacketInfo<NrRrcConnectionSetupHeader>(source, "SOURCE");

    // Add header
    packet->AddHeader(source);

    // Log serialized packet contents
    TestUtils::LogPacketContents(packet);

    // remove header
    NrRrcConnectionSetupHeader destination;
    packet->RemoveHeader(destination);

    // Log destination info
    TestUtils::LogPacketInfo<NrRrcConnectionSetupHeader>(destination, "DESTINATION");

    // Check that the destination and source headers contain the same values
    NS_TEST_ASSERT_MSG_EQ(source.GetRrcTransactionIdentifier(),
                          destination.GetRrcTransactionIdentifier(),
                          "RrcTransactionIdentifier");

    AssertEqualRadioResourceConfigDedicated(source.GetRadioResourceConfigDedicated(),
                                            destination.GetRadioResourceConfigDedicated());

    packet = nullptr;
}

/**
 * \ingroup nr-test
 *
 * \brief Rrc Connection Setup Complete Test Case
 */
class NrRrcConnectionSetupCompleteTestCase : public RrcHeaderTestCase
{
  public:
    NrRrcConnectionSetupCompleteTestCase();
    void DoRun() override;
};

NrRrcConnectionSetupCompleteTestCase::NrRrcConnectionSetupCompleteTestCase()
    : RrcHeaderTestCase("Testing NrRrcConnectionSetupCompleteTestCase")
{
}

void
NrRrcConnectionSetupCompleteTestCase::DoRun()
{
    packet = Create<Packet>();
    NS_LOG_DEBUG("============= NrRrcConnectionSetupCompleteTestCase ===========");

    NrRrcSap::RrcConnectionSetupCompleted msg;
    msg.rrcTransactionIdentifier = 3;

    NrRrcConnectionSetupCompleteHeader source;
    source.SetMessage(msg);

    // Log source info
    TestUtils::LogPacketInfo<NrRrcConnectionSetupCompleteHeader>(source, "SOURCE");

    // Add header
    packet->AddHeader(source);

    // Log serialized packet contents
    TestUtils::LogPacketContents(packet);

    // Remove header
    NrRrcConnectionSetupCompleteHeader destination;
    packet->RemoveHeader(destination);

    // Log destination info
    TestUtils::LogPacketInfo<NrRrcConnectionSetupCompleteHeader>(destination, "DESTINATION");

    // Check that the destination and source headers contain the same values
    NS_TEST_ASSERT_MSG_EQ(source.GetRrcTransactionIdentifier(),
                          destination.GetRrcTransactionIdentifier(),
                          "RrcTransactionIdentifier");

    packet = nullptr;
}

/**
 * \ingroup nr-test
 *
 * \brief Rrc Connection Reconfiguration Complete Test Case
 */
class NrRrcConnectionReconfigurationCompleteTestCase : public RrcHeaderTestCase
{
  public:
    NrRrcConnectionReconfigurationCompleteTestCase();
    void DoRun() override;
};

NrRrcConnectionReconfigurationCompleteTestCase::NrRrcConnectionReconfigurationCompleteTestCase()
    : RrcHeaderTestCase("Testing NrRrcConnectionReconfigurationCompleteTestCase")
{
}

void
NrRrcConnectionReconfigurationCompleteTestCase::DoRun()
{
    packet = Create<Packet>();
    NS_LOG_DEBUG("============= NrRrcConnectionReconfigurationCompleteTestCase ===========");

    NrRrcSap::RrcConnectionReconfigurationCompleted msg;
    msg.rrcTransactionIdentifier = 2;

    NrRrcConnectionReconfigurationCompleteHeader source;
    source.SetMessage(msg);

    // Log source info
    TestUtils::LogPacketInfo<NrRrcConnectionReconfigurationCompleteHeader>(source, "SOURCE");

    // Add header
    packet->AddHeader(source);

    // Log serialized packet contents
    TestUtils::LogPacketContents(packet);

    // remove header
    NrRrcConnectionReconfigurationCompleteHeader destination;
    packet->RemoveHeader(destination);

    // Log destination info
    TestUtils::LogPacketInfo<NrRrcConnectionReconfigurationCompleteHeader>(destination,
                                                                           "DESTINATION");

    // Check that the destination and source headers contain the same values
    NS_TEST_ASSERT_MSG_EQ(source.GetRrcTransactionIdentifier(),
                          destination.GetRrcTransactionIdentifier(),
                          "RrcTransactionIdentifier");

    packet = nullptr;
}

/**
 * \ingroup nr-test
 *
 * \brief Rrc Connection Reconfiguration Test Case
 */
class NrRrcConnectionReconfigurationTestCase : public RrcHeaderTestCase
{
  public:
    NrRrcConnectionReconfigurationTestCase();
    void DoRun() override;
};

NrRrcConnectionReconfigurationTestCase::NrRrcConnectionReconfigurationTestCase()
    : RrcHeaderTestCase("Testing NrRrcConnectionReconfigurationTestCase")
{
}

void
NrRrcConnectionReconfigurationTestCase::DoRun()
{
    packet = Create<Packet>();
    NS_LOG_DEBUG("============= NrRrcConnectionReconfigurationTestCase ===========");

    NrRrcSap::RrcConnectionReconfiguration msg{};
    msg.rrcTransactionIdentifier = 2;

    msg.haveMeasConfig = true;

    msg.measConfig.haveQuantityConfig = true;
    msg.measConfig.quantityConfig.filterCoefficientRSRP = 8;
    msg.measConfig.quantityConfig.filterCoefficientRSRQ = 7;

    msg.measConfig.haveMeasGapConfig = true;
    msg.measConfig.measGapConfig.type = NrRrcSap::MeasGapConfig::SETUP;
    msg.measConfig.measGapConfig.gapOffsetChoice = NrRrcSap::MeasGapConfig::GP0;
    msg.measConfig.measGapConfig.gapOffsetValue = 21;

    msg.measConfig.haveSmeasure = true;
    msg.measConfig.sMeasure = 57;

    msg.measConfig.haveSpeedStatePars = true;
    msg.measConfig.speedStatePars.type = NrRrcSap::SpeedStatePars::SETUP;
    msg.measConfig.speedStatePars.mobilityStateParameters.tEvaluation = 240;
    msg.measConfig.speedStatePars.mobilityStateParameters.tHystNormal = 60;
    msg.measConfig.speedStatePars.mobilityStateParameters.nCellChangeMedium = 5;
    msg.measConfig.speedStatePars.mobilityStateParameters.nCellChangeHigh = 13;
    msg.measConfig.speedStatePars.timeToTriggerSf.sfMedium = 25;
    msg.measConfig.speedStatePars.timeToTriggerSf.sfHigh = 75;

    msg.measConfig.measObjectToRemoveList.push_back(23);
    msg.measConfig.measObjectToRemoveList.push_back(13);

    msg.measConfig.reportConfigToRemoveList.push_back(7);
    msg.measConfig.reportConfigToRemoveList.push_back(16);

    msg.measConfig.measIdToRemoveList.push_back(4);
    msg.measConfig.measIdToRemoveList.push_back(18);

    // Set measObjectToAddModList
    NrRrcSap::MeasObjectToAddMod measObjectToAddMod;
    measObjectToAddMod.measObjectId = 3;
    measObjectToAddMod.measObjectEutra.carrierFreq = 21;
    measObjectToAddMod.measObjectEutra.allowedMeasBandwidth = 15;
    measObjectToAddMod.measObjectEutra.presenceAntennaPort1 = true;
    measObjectToAddMod.measObjectEutra.neighCellConfig = 3;
    measObjectToAddMod.measObjectEutra.offsetFreq = -12;
    measObjectToAddMod.measObjectEutra.cellsToRemoveList.push_back(5);
    measObjectToAddMod.measObjectEutra.cellsToRemoveList.push_back(2);
    measObjectToAddMod.measObjectEutra.blackCellsToRemoveList.push_back(1);
    measObjectToAddMod.measObjectEutra.haveCellForWhichToReportCGI = true;
    measObjectToAddMod.measObjectEutra.cellForWhichToReportCGI = 250;
    NrRrcSap::CellsToAddMod cellsToAddMod;
    cellsToAddMod.cellIndex = 20;
    cellsToAddMod.physCellId = 14;
    cellsToAddMod.cellIndividualOffset = 22;
    measObjectToAddMod.measObjectEutra.cellsToAddModList.push_back(cellsToAddMod);
    NrRrcSap::BlackCellsToAddMod blackCellsToAddMod;
    blackCellsToAddMod.cellIndex = 18;
    blackCellsToAddMod.physCellIdRange.start = 128;
    blackCellsToAddMod.physCellIdRange.haveRange = true;
    blackCellsToAddMod.physCellIdRange.range = 128;
    measObjectToAddMod.measObjectEutra.blackCellsToAddModList.push_back(blackCellsToAddMod);
    msg.measConfig.measObjectToAddModList.push_back(measObjectToAddMod);

    // Set reportConfigToAddModList
    NrRrcSap::ReportConfigToAddMod reportConfigToAddMod;
    reportConfigToAddMod.reportConfigId = 22;
    reportConfigToAddMod.reportConfigEutra.triggerType = NrRrcSap::ReportConfigEutra::EVENT;
    reportConfigToAddMod.reportConfigEutra.eventId = NrRrcSap::ReportConfigEutra::EVENT_A2;
    reportConfigToAddMod.reportConfigEutra.threshold1.choice =
        NrRrcSap::ThresholdEutra::THRESHOLD_RSRP;
    reportConfigToAddMod.reportConfigEutra.threshold1.range = 15;
    reportConfigToAddMod.reportConfigEutra.threshold2.choice =
        NrRrcSap::ThresholdEutra::THRESHOLD_RSRQ;
    reportConfigToAddMod.reportConfigEutra.threshold2.range = 10;
    reportConfigToAddMod.reportConfigEutra.reportOnLeave = true;
    reportConfigToAddMod.reportConfigEutra.a3Offset = -25;
    reportConfigToAddMod.reportConfigEutra.hysteresis = 18;
    reportConfigToAddMod.reportConfigEutra.timeToTrigger = 100;
    reportConfigToAddMod.reportConfigEutra.purpose =
        NrRrcSap::ReportConfigEutra::REPORT_STRONGEST_CELLS;
    reportConfigToAddMod.reportConfigEutra.triggerQuantity = NrRrcSap::ReportConfigEutra::RSRQ;
    reportConfigToAddMod.reportConfigEutra.reportQuantity =
        NrRrcSap::ReportConfigEutra::SAME_AS_TRIGGER_QUANTITY;
    reportConfigToAddMod.reportConfigEutra.maxReportCells = 5;
    reportConfigToAddMod.reportConfigEutra.reportInterval = NrRrcSap::ReportConfigEutra::MIN60;
    reportConfigToAddMod.reportConfigEutra.reportAmount = 16;
    msg.measConfig.reportConfigToAddModList.push_back(reportConfigToAddMod);

    // Set measIdToAddModList
    NrRrcSap::MeasIdToAddMod measIdToAddMod;
    NrRrcSap::MeasIdToAddMod measIdToAddMod2;
    measIdToAddMod.measId = 7;
    measIdToAddMod.measObjectId = 6;
    measIdToAddMod.reportConfigId = 5;
    measIdToAddMod2.measId = 4;
    measIdToAddMod2.measObjectId = 8;
    measIdToAddMod2.reportConfigId = 12;
    msg.measConfig.measIdToAddModList.push_back(measIdToAddMod);
    msg.measConfig.measIdToAddModList.push_back(measIdToAddMod2);

    msg.haveMobilityControlInfo = true;
    msg.mobilityControlInfo.targetPhysCellId = 4;
    msg.mobilityControlInfo.haveCarrierFreq = true;
    msg.mobilityControlInfo.carrierFreq.dlCarrierFreq = 3;
    msg.mobilityControlInfo.carrierFreq.ulCarrierFreq = 5;
    msg.mobilityControlInfo.haveCarrierBandwidth = true;
    msg.mobilityControlInfo.carrierBandwidth.dlBandwidth = 50;
    msg.mobilityControlInfo.carrierBandwidth.ulBandwidth = 25;
    msg.mobilityControlInfo.newUeIdentity = 11;
    msg.mobilityControlInfo.haveRachConfigDedicated = true;
    msg.mobilityControlInfo.rachConfigDedicated.raPreambleIndex = 2;
    msg.mobilityControlInfo.rachConfigDedicated.raPrachMaskIndex = 2;
    msg.mobilityControlInfo.radioResourceConfigCommon.rachConfigCommon.preambleInfo
        .numberOfRaPreambles = 4;
    msg.mobilityControlInfo.radioResourceConfigCommon.rachConfigCommon.raSupervisionInfo
        .preambleTransMax = 3;
    msg.mobilityControlInfo.radioResourceConfigCommon.rachConfigCommon.raSupervisionInfo
        .raResponseWindowSize = 6;

    msg.haveRadioResourceConfigDedicated = true;

    msg.radioResourceConfigDedicated = CreateRadioResourceConfigDedicated();

    msg.haveNonCriticalExtension = false; // Danilo
    NrRrcConnectionReconfigurationHeader source;
    source.SetMessage(msg);

    // Log source info
    TestUtils::LogPacketInfo<NrRrcConnectionReconfigurationHeader>(source, "SOURCE");

    // Add header
    packet->AddHeader(source);

    // Log serialized packet contents
    TestUtils::LogPacketContents(packet);

    // remove header
    NrRrcConnectionReconfigurationHeader destination;
    packet->RemoveHeader(destination);

    // Log destination info
    TestUtils::LogPacketInfo<NrRrcConnectionReconfigurationHeader>(destination, "DESTINATION");

    // Check that the destination and source headers contain the same values
    NS_TEST_ASSERT_MSG_EQ(source.GetRrcTransactionIdentifier(),
                          destination.GetRrcTransactionIdentifier(),
                          "RrcTransactionIdentifier");
    NS_TEST_ASSERT_MSG_EQ(source.GetHaveMeasConfig(),
                          destination.GetHaveMeasConfig(),
                          "GetHaveMeasConfig");
    NS_TEST_ASSERT_MSG_EQ(source.GetHaveMobilityControlInfo(),
                          destination.GetHaveMobilityControlInfo(),
                          "GetHaveMobilityControlInfo");
    NS_TEST_ASSERT_MSG_EQ(source.GetHaveRadioResourceConfigDedicated(),
                          destination.GetHaveRadioResourceConfigDedicated(),
                          "GetHaveRadioResourceConfigDedicated");

    if (source.GetHaveMobilityControlInfo())
    {
        NS_TEST_ASSERT_MSG_EQ(source.GetMobilityControlInfo().targetPhysCellId,
                              destination.GetMobilityControlInfo().targetPhysCellId,
                              "GetMobilityControlInfo().targetPhysCellId");
        NS_TEST_ASSERT_MSG_EQ(source.GetMobilityControlInfo().haveCarrierFreq,
                              destination.GetMobilityControlInfo().haveCarrierFreq,
                              "GetMobilityControlInfo().haveCarrierFreq");
        NS_TEST_ASSERT_MSG_EQ(source.GetMobilityControlInfo().haveCarrierBandwidth,
                              destination.GetMobilityControlInfo().haveCarrierBandwidth,
                              "GetMobilityControlInfo().haveCarrierBandwidth");
        NS_TEST_ASSERT_MSG_EQ(source.GetMobilityControlInfo().newUeIdentity,
                              destination.GetMobilityControlInfo().newUeIdentity,
                              "GetMobilityControlInfo().newUeIdentity");
        NS_TEST_ASSERT_MSG_EQ(source.GetMobilityControlInfo().haveRachConfigDedicated,
                              destination.GetMobilityControlInfo().haveRachConfigDedicated,
                              "GetMobilityControlInfo().haveRachConfigDedicated");

        if (source.GetMobilityControlInfo().haveCarrierFreq)
        {
            NS_TEST_ASSERT_MSG_EQ(source.GetMobilityControlInfo().carrierFreq.dlCarrierFreq,
                                  destination.GetMobilityControlInfo().carrierFreq.dlCarrierFreq,
                                  "GetMobilityControlInfo().carrierFreq.dlCarrierFreq");
            NS_TEST_ASSERT_MSG_EQ(source.GetMobilityControlInfo().carrierFreq.ulCarrierFreq,
                                  destination.GetMobilityControlInfo().carrierFreq.ulCarrierFreq,
                                  "GetMobilityControlInfo().carrierFreq.ulCarrierFreq");
        }

        if (source.GetMobilityControlInfo().haveCarrierBandwidth)
        {
            NS_TEST_ASSERT_MSG_EQ(source.GetMobilityControlInfo().carrierBandwidth.dlBandwidth,
                                  destination.GetMobilityControlInfo().carrierBandwidth.dlBandwidth,
                                  "GetMobilityControlInfo().carrierBandwidth.dlBandwidth");
            NS_TEST_ASSERT_MSG_EQ(source.GetMobilityControlInfo().carrierBandwidth.ulBandwidth,
                                  destination.GetMobilityControlInfo().carrierBandwidth.ulBandwidth,
                                  "GetMobilityControlInfo().carrierBandwidth.ulBandwidth");
        }

        if (source.GetMobilityControlInfo().haveRachConfigDedicated)
        {
            NS_TEST_ASSERT_MSG_EQ(
                source.GetMobilityControlInfo().rachConfigDedicated.raPreambleIndex,
                destination.GetMobilityControlInfo().rachConfigDedicated.raPreambleIndex,
                "GetMobilityControlInfo().rachConfigDedicated.raPreambleIndex");
            NS_TEST_ASSERT_MSG_EQ(
                source.GetMobilityControlInfo().rachConfigDedicated.raPrachMaskIndex,
                destination.GetMobilityControlInfo().rachConfigDedicated.raPrachMaskIndex,
                "GetMobilityControlInfo().rachConfigDedicated.raPrachMaskIndex");
        }
    }

    if (source.GetHaveRadioResourceConfigDedicated())
    {
        AssertEqualRadioResourceConfigDedicated(source.GetRadioResourceConfigDedicated(),
                                                destination.GetRadioResourceConfigDedicated());
    }

    packet = nullptr;
}

/**
 * \ingroup nr-test
 *
 * \brief Handover Preparation Info Test Case
 */
class NrHandoverPreparationInfoTestCase : public RrcHeaderTestCase
{
  public:
    NrHandoverPreparationInfoTestCase();
    void DoRun() override;
};

NrHandoverPreparationInfoTestCase::NrHandoverPreparationInfoTestCase()
    : RrcHeaderTestCase("Testing NrHandoverPreparationInfoTestCase")
{
}

void
NrHandoverPreparationInfoTestCase::DoRun()
{
    packet = Create<Packet>();
    NS_LOG_DEBUG("============= NrHandoverPreparationInfoTestCase ===========");

    NrRrcSap::HandoverPreparationInfo msg;
    msg.asConfig.sourceDlCarrierFreq = 3;
    msg.asConfig.sourceUeIdentity = 11;
    msg.asConfig.sourceRadioResourceConfig = CreateRadioResourceConfigDedicated();
    msg.asConfig.sourceMasterInformationBlock.dlBandwidth = 50;
    msg.asConfig.sourceMasterInformationBlock.systemFrameNumber = 1;

    msg.asConfig.sourceSystemInformationBlockType1.cellAccessRelatedInfo.csgIndication = true;
    msg.asConfig.sourceSystemInformationBlockType1.cellAccessRelatedInfo.cellIdentity = 5;
    msg.asConfig.sourceSystemInformationBlockType1.cellAccessRelatedInfo.csgIdentity = 4;
    msg.asConfig.sourceSystemInformationBlockType1.cellAccessRelatedInfo.plmnIdentityInfo
        .plmnIdentity = 123;

    msg.asConfig.sourceSystemInformationBlockType2.freqInfo.ulBandwidth = 100;
    msg.asConfig.sourceSystemInformationBlockType2.freqInfo.ulCarrierFreq = 10;
    msg.asConfig.sourceSystemInformationBlockType2.radioResourceConfigCommon.rachConfigCommon
        .preambleInfo.numberOfRaPreambles = 4;
    msg.asConfig.sourceSystemInformationBlockType2.radioResourceConfigCommon.rachConfigCommon
        .raSupervisionInfo.preambleTransMax = 3;
    msg.asConfig.sourceSystemInformationBlockType2.radioResourceConfigCommon.rachConfigCommon
        .raSupervisionInfo.raResponseWindowSize = 6;

    msg.asConfig.sourceMeasConfig.haveQuantityConfig = false;
    msg.asConfig.sourceMeasConfig.haveMeasGapConfig = false;
    msg.asConfig.sourceMeasConfig.haveSmeasure = false;
    msg.asConfig.sourceMeasConfig.haveSpeedStatePars = false;

    NrHandoverPreparationInfoHeader source;
    source.SetMessage(msg);

    // Log source info
    TestUtils::LogPacketInfo<NrHandoverPreparationInfoHeader>(source, "SOURCE");

    // Add header
    packet->AddHeader(source);

    // Log serialized packet contents
    TestUtils::LogPacketContents(packet);

    // remove header
    NrHandoverPreparationInfoHeader destination;
    packet->RemoveHeader(destination);

    // Log destination info
    TestUtils::LogPacketInfo<NrHandoverPreparationInfoHeader>(destination, "DESTINATION");

    // Check that the destination and source headers contain the same values
    AssertEqualRadioResourceConfigDedicated(source.GetAsConfig().sourceRadioResourceConfig,
                                            destination.GetAsConfig().sourceRadioResourceConfig);
    NS_TEST_ASSERT_MSG_EQ(source.GetAsConfig().sourceUeIdentity,
                          destination.GetAsConfig().sourceUeIdentity,
                          "sourceUeIdentity");
    NS_TEST_ASSERT_MSG_EQ(source.GetAsConfig().sourceMasterInformationBlock.dlBandwidth,
                          destination.GetAsConfig().sourceMasterInformationBlock.dlBandwidth,
                          "dlBandwidth");
    NS_TEST_ASSERT_MSG_EQ(source.GetAsConfig().sourceMasterInformationBlock.systemFrameNumber,
                          destination.GetAsConfig().sourceMasterInformationBlock.systemFrameNumber,
                          "systemFrameNumber");
    NS_TEST_ASSERT_MSG_EQ(
        source.GetAsConfig()
            .sourceSystemInformationBlockType1.cellAccessRelatedInfo.plmnIdentityInfo.plmnIdentity,
        destination.GetAsConfig()
            .sourceSystemInformationBlockType1.cellAccessRelatedInfo.plmnIdentityInfo.plmnIdentity,
        "plmnIdentity");
    NS_TEST_ASSERT_MSG_EQ(
        source.GetAsConfig().sourceSystemInformationBlockType1.cellAccessRelatedInfo.csgIndication,
        destination.GetAsConfig()
            .sourceSystemInformationBlockType1.cellAccessRelatedInfo.csgIndication,
        "csgIndication");
    NS_TEST_ASSERT_MSG_EQ(
        source.GetAsConfig().sourceSystemInformationBlockType1.cellAccessRelatedInfo.cellIdentity,
        destination.GetAsConfig()
            .sourceSystemInformationBlockType1.cellAccessRelatedInfo.cellIdentity,
        "cellIdentity");
    NS_TEST_ASSERT_MSG_EQ(
        source.GetAsConfig().sourceSystemInformationBlockType1.cellAccessRelatedInfo.csgIdentity,
        destination.GetAsConfig()
            .sourceSystemInformationBlockType1.cellAccessRelatedInfo.csgIdentity,
        "csgIdentity");
    NS_TEST_ASSERT_MSG_EQ(source.GetAsConfig().sourceDlCarrierFreq,
                          destination.GetAsConfig().sourceDlCarrierFreq,
                          "sourceDlCarrierFreq");

    packet = nullptr;
}

/**
 * \ingroup nr-test
 *
 * \brief Rrc Connection Reestablishment Request Test Case
 */
class NrRrcConnectionReestablishmentRequestTestCase : public RrcHeaderTestCase
{
  public:
    NrRrcConnectionReestablishmentRequestTestCase();
    void DoRun() override;
};

NrRrcConnectionReestablishmentRequestTestCase::NrRrcConnectionReestablishmentRequestTestCase()
    : RrcHeaderTestCase("Testing NrRrcConnectionReestablishmentRequestTestCase")
{
}

void
NrRrcConnectionReestablishmentRequestTestCase::DoRun()
{
    packet = Create<Packet>();
    NS_LOG_DEBUG("============= NrRrcConnectionReestablishmentRequestTestCase ===========");

    NrRrcSap::RrcConnectionReestablishmentRequest msg;
    msg.ueIdentity.cRnti = 12;
    msg.ueIdentity.physCellId = 21;
    msg.reestablishmentCause = NrRrcSap::HANDOVER_FAILURE;

    NrRrcConnectionReestablishmentRequestHeader source;
    source.SetMessage(msg);

    // Log source info
    TestUtils::LogPacketInfo<NrRrcConnectionReestablishmentRequestHeader>(source, "SOURCE");

    // Add header
    packet->AddHeader(source);

    // Log serialized packet contents
    TestUtils::LogPacketContents(packet);

    // remove header
    NrRrcConnectionReestablishmentRequestHeader destination;
    packet->RemoveHeader(destination);

    // Log destination info
    TestUtils::LogPacketInfo<NrRrcConnectionReestablishmentRequestHeader>(destination,
                                                                          "DESTINATION");

    // Check that the destination and source headers contain the same values
    NS_TEST_ASSERT_MSG_EQ(source.GetUeIdentity().cRnti, destination.GetUeIdentity().cRnti, "cRnti");
    NS_TEST_ASSERT_MSG_EQ(source.GetUeIdentity().physCellId,
                          destination.GetUeIdentity().physCellId,
                          "physCellId");
    NS_TEST_ASSERT_MSG_EQ(source.GetReestablishmentCause(),
                          destination.GetReestablishmentCause(),
                          "ReestablishmentCause");

    packet = nullptr;
}

/**
 * \ingroup nr-test
 *
 * \brief Rrc Connection Reestablishment Test Case
 */
class NrRrcConnectionReestablishmentTestCase : public RrcHeaderTestCase
{
  public:
    NrRrcConnectionReestablishmentTestCase();
    void DoRun() override;
};

NrRrcConnectionReestablishmentTestCase::NrRrcConnectionReestablishmentTestCase()
    : RrcHeaderTestCase("Testing NrRrcConnectionReestablishmentTestCase")
{
}

void
NrRrcConnectionReestablishmentTestCase::DoRun()
{
    packet = Create<Packet>();
    NS_LOG_DEBUG("============= NrRrcConnectionReestablishmentTestCase ===========");

    NrRrcSap::RrcConnectionReestablishment msg;
    msg.rrcTransactionIdentifier = 2;
    msg.radioResourceConfigDedicated = CreateRadioResourceConfigDedicated();

    NrRrcConnectionReestablishmentHeader source;
    source.SetMessage(msg);

    // Log source info
    TestUtils::LogPacketInfo<NrRrcConnectionReestablishmentHeader>(source, "SOURCE");

    // Add header
    packet->AddHeader(source);

    // Log serialized packet contents
    TestUtils::LogPacketContents(packet);

    // remove header
    NrRrcConnectionReestablishmentHeader destination;
    packet->RemoveHeader(destination);

    // Log destination info
    TestUtils::LogPacketInfo<NrRrcConnectionReestablishmentHeader>(destination, "DESTINATION");

    // Check that the destination and source headers contain the same values
    NS_TEST_ASSERT_MSG_EQ(source.GetRrcTransactionIdentifier(),
                          destination.GetRrcTransactionIdentifier(),
                          "rrcTransactionIdentifier");
    AssertEqualRadioResourceConfigDedicated(source.GetRadioResourceConfigDedicated(),
                                            destination.GetRadioResourceConfigDedicated());

    packet = nullptr;
}

/**
 * \ingroup nr-test
 *
 * \brief Rrc Connection Reestablishment Complete Test Case
 */
class NrRrcConnectionReestablishmentCompleteTestCase : public RrcHeaderTestCase
{
  public:
    NrRrcConnectionReestablishmentCompleteTestCase();
    void DoRun() override;
};

NrRrcConnectionReestablishmentCompleteTestCase::NrRrcConnectionReestablishmentCompleteTestCase()
    : RrcHeaderTestCase("Testing NrRrcConnectionReestablishmentCompleteTestCase")
{
}

void
NrRrcConnectionReestablishmentCompleteTestCase::DoRun()
{
    packet = Create<Packet>();
    NS_LOG_DEBUG("============= NrRrcConnectionReestablishmentCompleteTestCase ===========");

    NrRrcSap::RrcConnectionReestablishmentComplete msg;
    msg.rrcTransactionIdentifier = 3;

    NrRrcConnectionReestablishmentCompleteHeader source;
    source.SetMessage(msg);

    // Log source info
    TestUtils::LogPacketInfo<NrRrcConnectionReestablishmentCompleteHeader>(source, "SOURCE");

    // Add header
    packet->AddHeader(source);

    // Log serialized packet contents
    TestUtils::LogPacketContents(packet);

    // remove header
    NrRrcConnectionReestablishmentCompleteHeader destination;
    packet->RemoveHeader(destination);

    // Log destination info
    TestUtils::LogPacketInfo<NrRrcConnectionReestablishmentCompleteHeader>(destination,
                                                                           "DESTINATION");

    // Check that the destination and source headers contain the same values
    NS_TEST_ASSERT_MSG_EQ(source.GetRrcTransactionIdentifier(),
                          destination.GetRrcTransactionIdentifier(),
                          "rrcTransactionIdentifier");

    packet = nullptr;
}

/**
 * \ingroup nr-test
 *
 * \brief Rrc Connection Reject Test Case
 */
class NrRrcConnectionRejectTestCase : public RrcHeaderTestCase
{
  public:
    NrRrcConnectionRejectTestCase();
    void DoRun() override;
};

NrRrcConnectionRejectTestCase::NrRrcConnectionRejectTestCase()
    : RrcHeaderTestCase("Testing NrRrcConnectionRejectTestCase")
{
}

void
NrRrcConnectionRejectTestCase::DoRun()
{
    packet = Create<Packet>();
    NS_LOG_DEBUG("============= NrRrcConnectionRejectTestCase ===========");

    NrRrcSap::RrcConnectionReject msg;
    msg.waitTime = 2;

    NrRrcConnectionRejectHeader source;
    source.SetMessage(msg);

    // Log source info
    TestUtils::LogPacketInfo<NrRrcConnectionRejectHeader>(source, "SOURCE");

    // Add header
    packet->AddHeader(source);

    // Log serialized packet contents
    TestUtils::LogPacketContents(packet);

    // remove header
    NrRrcConnectionRejectHeader destination;
    packet->RemoveHeader(destination);

    // Log destination info
    TestUtils::LogPacketInfo<NrRrcConnectionRejectHeader>(destination, "DESTINATION");

    // Check that the destination and source headers contain the same values
    NS_TEST_ASSERT_MSG_EQ(source.GetMessage().waitTime,
                          destination.GetMessage().waitTime,
                          "Different waitTime!");

    packet = nullptr;
}

/**
 * \ingroup nr-test
 *
 * \brief Measurement Report Test Case
 */
class NrMeasurementReportTestCase : public RrcHeaderTestCase
{
  public:
    NrMeasurementReportTestCase();
    void DoRun() override;
};

NrMeasurementReportTestCase::NrMeasurementReportTestCase()
    : RrcHeaderTestCase("Testing NrMeasurementReportTestCase")
{
}

void
NrMeasurementReportTestCase::DoRun()
{
    packet = Create<Packet>();
    NS_LOG_DEBUG("============= NrMeasurementReportTestCase ===========");

    NrRrcSap::MeasurementReport msg;
    msg.measResults.measId = 5;
    msg.measResults.measResultPCell.rsrpResult = 18;
    msg.measResults.measResultPCell.rsrqResult = 21;
    msg.measResults.haveMeasResultNeighCells = true;

    NrRrcSap::MeasResultEutra mResEutra;
    mResEutra.physCellId = 9;
    mResEutra.haveRsrpResult = true;
    mResEutra.rsrpResult = 33;
    mResEutra.haveRsrqResult = true;
    mResEutra.rsrqResult = 22;
    mResEutra.haveCgiInfo = true;
    mResEutra.cgiInfo.plmnIdentity = 7;
    mResEutra.cgiInfo.cellIdentity = 6;
    mResEutra.cgiInfo.trackingAreaCode = 5;
    msg.measResults.measResultListEutra.push_back(mResEutra);

    msg.measResults.haveMeasResultServFreqList = false;

    NrMeasurementReportHeader source;
    source.SetMessage(msg);

    // Log source info
    TestUtils::LogPacketInfo<NrMeasurementReportHeader>(source, "SOURCE");

    // Add header
    packet->AddHeader(source);

    // Log serialized packet contents
    TestUtils::LogPacketContents(packet);

    // remove header
    NrMeasurementReportHeader destination;
    packet->RemoveHeader(destination);

    // Log destination info
    TestUtils::LogPacketInfo<NrMeasurementReportHeader>(destination, "DESTINATION");

    // Check that the destination and source headers contain the same values
    NrRrcSap::MeasResults srcMeas = source.GetMessage().measResults;
    NrRrcSap::MeasResults dstMeas = destination.GetMessage().measResults;

    NS_TEST_ASSERT_MSG_EQ(srcMeas.measId, dstMeas.measId, "Different measId!");
    NS_TEST_ASSERT_MSG_EQ(srcMeas.measResultPCell.rsrpResult,
                          dstMeas.measResultPCell.rsrpResult,
                          "Different rsrpResult!");
    NS_TEST_ASSERT_MSG_EQ(srcMeas.measResultPCell.rsrqResult,
                          dstMeas.measResultPCell.rsrqResult,
                          "Different rsrqResult!");
    NS_TEST_ASSERT_MSG_EQ(srcMeas.haveMeasResultNeighCells,
                          dstMeas.haveMeasResultNeighCells,
                          "Different haveMeasResultNeighCells!");

    if (srcMeas.haveMeasResultNeighCells)
    {
        auto itsrc = srcMeas.measResultListEutra.begin();
        auto itdst = dstMeas.measResultListEutra.begin();
        for (; itsrc != srcMeas.measResultListEutra.end(); itsrc++, itdst++)
        {
            NS_TEST_ASSERT_MSG_EQ(itsrc->physCellId, itdst->physCellId, "Different physCellId!");

            NS_TEST_ASSERT_MSG_EQ(itsrc->haveCgiInfo, itdst->haveCgiInfo, "Different haveCgiInfo!");
            if (itsrc->haveCgiInfo)
            {
                NS_TEST_ASSERT_MSG_EQ(itsrc->cgiInfo.plmnIdentity,
                                      itdst->cgiInfo.plmnIdentity,
                                      "Different cgiInfo.plmnIdentity!");
                NS_TEST_ASSERT_MSG_EQ(itsrc->cgiInfo.cellIdentity,
                                      itdst->cgiInfo.cellIdentity,
                                      "Different cgiInfo.cellIdentity!");
                NS_TEST_ASSERT_MSG_EQ(itsrc->cgiInfo.trackingAreaCode,
                                      itdst->cgiInfo.trackingAreaCode,
                                      "Different cgiInfo.trackingAreaCode!");
                NS_TEST_ASSERT_MSG_EQ(itsrc->cgiInfo.plmnIdentityList.size(),
                                      itdst->cgiInfo.plmnIdentityList.size(),
                                      "Different cgiInfo.plmnIdentityList.size()!");

                if (!itsrc->cgiInfo.plmnIdentityList.empty())
                {
                    auto itsrc2 = itsrc->cgiInfo.plmnIdentityList.begin();
                    auto itdst2 = itdst->cgiInfo.plmnIdentityList.begin();
                    for (; itsrc2 != itsrc->cgiInfo.plmnIdentityList.begin(); itsrc2++, itdst2++)
                    {
                        NS_TEST_ASSERT_MSG_EQ(*itsrc2, *itdst2, "Different plmnId elements!");
                    }
                }
            }

            NS_TEST_ASSERT_MSG_EQ(itsrc->haveRsrpResult,
                                  itdst->haveRsrpResult,
                                  "Different haveRsrpResult!");
            if (itsrc->haveRsrpResult)
            {
                NS_TEST_ASSERT_MSG_EQ(itsrc->rsrpResult,
                                      itdst->rsrpResult,
                                      "Different rsrpResult!");
            }

            NS_TEST_ASSERT_MSG_EQ(itsrc->haveRsrqResult,
                                  itdst->haveRsrqResult,
                                  "Different haveRsrqResult!");
            if (itsrc->haveRsrqResult)
            {
                NS_TEST_ASSERT_MSG_EQ(itsrc->rsrqResult,
                                      itdst->rsrqResult,
                                      "Different rsrqResult!");
            }
        }
    }

    packet = nullptr;
}

/**
 * \ingroup nr-test
 *
 * \brief Asn1Encoding Test Suite
 */
class NrAsn1EncodingSuite : public TestSuite
{
  public:
    NrAsn1EncodingSuite();
};

NrAsn1EncodingSuite::NrAsn1EncodingSuite()
    : TestSuite("nr-test-asn1-encoding", Type::UNIT)
{
    NS_LOG_FUNCTION(this);
    AddTestCase(new NrRrcConnectionRequestTestCase(), TestCase::Duration::QUICK);
    AddTestCase(new NrRrcConnectionSetupTestCase(), TestCase::Duration::QUICK);
    AddTestCase(new NrRrcConnectionSetupCompleteTestCase(), TestCase::Duration::QUICK);
    AddTestCase(new NrRrcConnectionReconfigurationCompleteTestCase(), TestCase::Duration::QUICK);
    AddTestCase(new NrRrcConnectionReconfigurationTestCase(), TestCase::Duration::QUICK);
    AddTestCase(new NrHandoverPreparationInfoTestCase(), TestCase::Duration::QUICK);
    AddTestCase(new NrRrcConnectionReestablishmentRequestTestCase(), TestCase::Duration::QUICK);
    AddTestCase(new NrRrcConnectionReestablishmentTestCase(), TestCase::Duration::QUICK);
    AddTestCase(new NrRrcConnectionReestablishmentCompleteTestCase(), TestCase::Duration::QUICK);
    AddTestCase(new NrRrcConnectionRejectTestCase(), TestCase::Duration::QUICK);
    AddTestCase(new NrMeasurementReportTestCase(), TestCase::Duration::QUICK);
}

/**
 * \ingroup nr-test
 * Static variable for test initialization
 */
NrAsn1EncodingSuite g_nrAsn1EncodingSuite;

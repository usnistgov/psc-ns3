/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Nicola Baldo <nbaldo@cttc.es>
 *         Budiarto Herman <budiarto.herman@magister.fi>
 */

#include <ns3/core-module.h>
#include <ns3/mobility-module.h>
#include <ns3/network-module.h>
#include <ns3/nr-module.h>

#include <cmath>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NrRrcTest");

/**
 * \ingroup nr-test
 *
 * \brief Test rrc connection establishment.
 */
class NrRrcConnectionEstablishmentTestCase : public TestCase
{
  public:
    /**
     *
     * \param isFdd FDD if true, TDD if false
     * \param nUes number of UEs in the test
     * \param nBearers number of bearers to be setup in each connection
     * \param tConnBase connection time base value for all UEs in ms
     * \param tConnIncrPerUe additional connection time increment for each UE index (0...nUes-1) in
     * ms
     * \param delayDiscStart expected duration to perform connection establishment in ms
     * \param errorExpected if true, test case will wait a bit longer to accommodate for
     * transmission error
     * \param useIdealRrc If set to false, real RRC protocol model will be used
     * \param admitRrcConnectionRequest If set to false, gNB will not allow UE connections
     * \param description additional description of the test case
     */
    NrRrcConnectionEstablishmentTestCase(bool isFdd,
                                         uint32_t nUes,
                                         uint32_t nBearers,
                                         uint32_t tConnBase,
                                         uint32_t tConnIncrPerUe,
                                         uint32_t delayDiscStart,
                                         bool errorExpected,
                                         bool useIdealRrc,
                                         bool admitRrcConnectionRequest,
                                         std::string description = "");

  protected:
    void DoRun() override;

    /**
     * Build name string function
     *
     * \param isFdd FDD if true, TDD if false
     * \param nUes number of UEs in the test
     * \param nBearers number of bearers to be setup in each connection
     * \param tConnBase connection time base value for all UEs in ms
     * \param tConnIncrPerUe additional connection time increment for each UE index (0...nUes-1)
     * in ms
     * \param delayDiscStart expected duration to perform connection establishment in ms
     * \param useIdealRrc If set to false, real RRC protocol model will be used
     * \param admitRrcConnectionRequest If set to false, gNB will not allow UE connections
     * \param description additional description of the test case
     * \returns the name string
     */
    static std::string BuildNameString(bool isFdd,
                                       uint32_t nUes,
                                       uint32_t nBearers,
                                       uint32_t tConnBase,
                                       uint32_t tConnIncrPerUe,
                                       uint32_t delayDiscStart,
                                       bool useIdealRrc,
                                       bool admitRrcConnectionRequest,
                                       std::string description = "");
    /**
     * Connect function
     * \param ueDevice the UE device
     * \param gnbDevice the gNB device
     */
    void Connect(Ptr<NetDevice> ueDevice, Ptr<NetDevice> gnbDevice);
    /**
     * Check connected function
     * \param ueDevice the UE device
     * \param gnbDevice the gNB device
     */
    void CheckConnected(Ptr<NetDevice> ueDevice, Ptr<NetDevice> gnbDevice);
    /**
     * Check not connected function
     * \param ueDevice the UE device
     * \param gnbDevice the gNB device
     */
    void CheckNotConnected(Ptr<NetDevice> ueDevice, Ptr<NetDevice> gnbDevice);
    /**
     * Connection established callback function
     * \param context the context string
     * \param imsi the IMSI
     * \param cellId the cell ID
     * \param rnti the RNTI
     */
    void ConnectionEstablishedCallback(std::string context,
                                       uint64_t imsi,
                                       uint16_t cellId,
                                       uint16_t rnti);
    /**
     * Connection timeout callback function
     * \param context the context string
     * \param imsi the IMSI
     * \param cellId the cell ID
     * \param rnti the RNTI
     * \param connEstFailCount the T300 timer expiration counter value
     */
    void ConnectionTimeoutCallback(std::string context,
                                   uint64_t imsi,
                                   uint16_t cellId,
                                   uint16_t rnti,
                                   uint8_t connEstFailCount);

    bool m_isFdd;              ///< flag indicating whether to use FDD or TDD setup
    uint32_t m_nUes;           ///< number of UEs in the test
    uint32_t m_nBearers;       ///< number of bearers to be setup in each connection
    uint32_t m_tConnBase;      ///< connection time base value for all UEs in ms
    uint32_t m_tConnIncrPerUe; ///< additional connection time increment for each UE index
                               ///< (0...nUes-1) in ms
    uint32_t m_delayConnEnd;   ///< expected duration to perform connection establishment in ms
    uint32_t
        m_delayDiscStart;    ///< delay between connection completed and disconnection request in ms
    uint32_t m_delayDiscEnd; ///< expected duration to complete disconnection in ms
    bool m_useIdealRrc;      ///< If set to false, real RRC protocol model will be used
    bool m_admitRrcConnectionRequest; ///< If set to false, gNB will not allow UE connections
    Ptr<NrHelper> m_nrHelper;         ///< NR helper

    /// key: IMSI
    std::map<uint64_t, bool> m_isConnectionEstablished;
};

std::string
NrRrcConnectionEstablishmentTestCase::BuildNameString(bool isFdd,
                                                      uint32_t nUes,
                                                      uint32_t nBearers,
                                                      uint32_t tConnBase,
                                                      uint32_t tConnIncrPerUe,
                                                      uint32_t delayDiscStart,
                                                      bool useIdealRrc,
                                                      bool admitRrcConnectionRequest,
                                                      std::string description)
{
    std::ostringstream oss;
    oss << "isFdd=" << isFdd << ", nUes=" << nUes << ", nBearers=" << nBearers
        << ", tConnBase=" << tConnBase << ", tConnIncrPerUe=" << tConnIncrPerUe
        << ", delayDiscStart=" << delayDiscStart;

    if (useIdealRrc)
    {
        oss << ", ideal RRC";
    }
    else
    {
        oss << ", real RRC";
    }

    if (admitRrcConnectionRequest)
    {
        oss << ", admitRrcConnectionRequest = true";
    }
    else
    {
        oss << ", admitRrcConnectionRequest = false";
    }

    if (!description.empty())
    {
        oss << ", " << description;
    }

    return oss.str();
}

NrRrcConnectionEstablishmentTestCase::NrRrcConnectionEstablishmentTestCase(
    bool isFdd,
    uint32_t nUes,
    uint32_t nBearers,
    uint32_t tConnBase,
    uint32_t tConnIncrPerUe,
    uint32_t delayDiscStart,
    bool errorExpected,
    bool useIdealRrc,
    bool admitRrcConnectionRequest,
    std::string description)
    : TestCase(BuildNameString(isFdd,
                               nUes,
                               nBearers,
                               tConnBase,
                               tConnIncrPerUe,
                               delayDiscStart,
                               useIdealRrc,
                               admitRrcConnectionRequest,
                               description)),
      m_isFdd(isFdd),
      m_nUes(nUes),
      m_nBearers(nBearers),
      m_tConnBase(tConnBase),
      m_tConnIncrPerUe(tConnIncrPerUe),

      m_delayDiscStart(delayDiscStart),
      m_delayDiscEnd(10),
      m_useIdealRrc(useIdealRrc),
      m_admitRrcConnectionRequest(admitRrcConnectionRequest)
{
    NS_LOG_FUNCTION(this << GetName());

    // see the description of d^e in the NR testing docs
    double dsi = 90;
    double nRaAttempts = 0;
    if (nUes <= 20)
    {
        nRaAttempts += 5;
    }
    else
    {
        NS_ASSERT(nUes <= 50);
        nRaAttempts += 10;
    }

    nRaAttempts += std::ceil(nUes / 4.0);
    double dra = nRaAttempts * 7;
    double dce = 10.0 + (2.0 * nUes) / 4.0;
    if (errorExpected)
    {
        /*
         * If transmission error happens, the UE has to repeat again from
         * acquiring system information.
         */
        dce += dsi + dce;
    }
    double nCrs;
    if (nUes <= 2)
    {
        nCrs = 0;
    }
    else if (nUes <= 5)
    {
        nCrs = 1;
    }
    else if (nUes <= 10)
    {
        nCrs = 2;
    }
    else if (nUes <= 20)
    {
        nCrs = 3;
    }
    else
    {
        nCrs = 4;
    }
    double dcr = (10.0 + (2.0 * nUes) / 4.0) * (m_nBearers + nCrs);

    m_delayConnEnd = round(dsi + dra + dce + dcr);
    NS_LOG_LOGIC(this << " " << GetName() << " dsi=" << dsi << " dra=" << dra << " dce=" << dce
                      << " dcr=" << dcr << " m_delayConnEnd=" << m_delayConnEnd);
}

void
NrRrcConnectionEstablishmentTestCase::DoRun()
{
    NS_LOG_FUNCTION(this << GetName());
    Config::Reset();

    if (m_nUes < 25)
    {
        Config::SetDefault("ns3::NrGnbRrc::SrsPeriodicity", UintegerValue(40));
    }
    else if (m_nUes < 60)
    {
        Config::SetDefault("ns3::NrGnbRrc::SrsPeriodicity", UintegerValue(80));
    }
    else if (m_nUes < 120)
    {
        Config::SetDefault("ns3::NrGnbRrc::SrsPeriodicity", UintegerValue(160));
    }
    else
    {
        Config::SetDefault("ns3::NrGnbRrc::SrsPeriodicity", UintegerValue(320));
    }

    // normal code
    m_nrHelper = CreateObject<NrHelper>();
    m_nrHelper->SetAttribute("CsiFeedbackFlags", UintegerValue(CsiFeedbackFlag::CQI_PDSCH_SISO));
    m_nrHelper->SetAttribute("UseIdealRrc", BooleanValue(m_useIdealRrc));
    auto bandwidthAndBWPPair =
        m_nrHelper->CreateBandwidthParts({{2.8e9, 10e6, static_cast<uint8_t>(m_isFdd ? 2 : 1)}},
                                         "UMa");
    if (m_isFdd)
    {
        Config::SetDefault("ns3::NrUeNetDevice::PrimaryUlIndex", UintegerValue(1));
    }
    NodeContainer gnbNodes;
    NodeContainer ueNodes;

    gnbNodes.Create(1);
    ueNodes.Create(m_nUes);

    // the following positions all ues at (0.1-0.9, 0.1-0.9, 0)
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                  "X",
                                  StringValue("0.5"),
                                  "Y",
                                  StringValue("0.5"),
                                  "Rho",
                                  StringValue("ns3::UniformRandomVariable[Min=0|Max=0.4]"));
    mobility.Install(ueNodes);

    // the following positions the gnb at (1, 1, 0)
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(1, 1, 0));
    mobility.SetPositionAllocator(positionAlloc);
    mobility.Install(gnbNodes);

    int64_t stream = 1;
    NetDeviceContainer gnbDevs;
    gnbDevs = m_nrHelper->InstallGnbDevice(gnbNodes, bandwidthAndBWPPair.second);
    stream += m_nrHelper->AssignStreams(gnbDevs, stream);

    NetDeviceContainer ueDevs;
    ueDevs = m_nrHelper->InstallUeDevice(ueNodes, bandwidthAndBWPPair.second);
    stream += m_nrHelper->AssignStreams(ueDevs, stream);

    if (m_isFdd)
    {
        for (uint32_t i = 0; i < gnbDevs.GetN(); i++)
        {
            m_nrHelper->GetGnbPhy(gnbDevs.Get(i), 0)
                ->SetAttribute("Pattern", StringValue("DL|DL|DL|DL|DL|DL|DL|DL|DL|DL|"));

            m_nrHelper->GetGnbPhy(gnbDevs.Get(i), 1)
                ->SetAttribute("Pattern", StringValue("UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|"));

            // Link the two FDD BWPs at gNBs
            m_nrHelper->GetBwpManagerGnb(gnbDevs.Get(i))->SetOutputLink(1, 0);
        }

        // Link the two FDD BWPs at UEs
        for (uint32_t i = 0; i < ueDevs.GetN(); i++)
        {
            m_nrHelper->GetBwpManagerUe(ueDevs.Get(i))->SetOutputLink(0, 1);
        }
    }

    // custom code used for testing purposes
    // instead of nrHelper->Attach () and nrHelper->ActivateXxx

    // Set AdmitConnectionRequest attribute
    for (auto it = gnbDevs.Begin(); it != gnbDevs.End(); ++it)
    {
        Ptr<NrGnbRrc> gnbRrc = (*it)->GetObject<NrGnbNetDevice>()->GetRrc();
        gnbRrc->SetAttribute("AdmitRrcConnectionRequest",
                             BooleanValue(m_admitRrcConnectionRequest));
    }

    uint32_t i = 0;
    uint32_t tmax = 0;
    for (auto it = ueDevs.Begin(); it != ueDevs.End(); ++it)
    {
        Ptr<NetDevice> ueDevice = *it;
        Ptr<NetDevice> gnbDevice = gnbDevs.Get(0);
        Ptr<NrUeNetDevice> ueNrDevice = ueDevice->GetObject<NrUeNetDevice>();

        uint32_t tc = m_tConnBase + m_tConnIncrPerUe * i; // time connection start
        uint32_t tcc = tc + m_delayConnEnd;               // time check connection completed;
        uint32_t td = tcc + m_delayDiscStart;             // time disconnect start
        uint32_t tcd = td + m_delayDiscEnd;               // time check disconnection completed
        tmax = std::max(tmax, tcd);

        // trick to resolve overloading
        // void (NrHelper::* overloadedAttachFunctionPointer) (Ptr<NetDevice>, Ptr<NetDevice>) =
        // &NrHelper::Attach; Simulator::Schedule (MilliSeconds (tc),
        // overloadedAttachFunctionPointer, nrHelper, *it, gnbDevice);
        Simulator::Schedule(MilliSeconds(tc),
                            &NrRrcConnectionEstablishmentTestCase::Connect,
                            this,
                            ueDevice,
                            gnbDevice);

        Simulator::Schedule(MilliSeconds(tcc),
                            &NrRrcConnectionEstablishmentTestCase::CheckConnected,
                            this,
                            *it,
                            gnbDevice);

        // disconnection not supported yet

        uint64_t imsi = ueNrDevice->GetImsi();
        m_isConnectionEstablished[imsi] = false;

        ++i;
    }

    // Connect to trace sources in UEs
    Config::Connect(
        "/NodeList/*/DeviceList/*/NrUeRrc/ConnectionEstablished",
        MakeCallback(&NrRrcConnectionEstablishmentTestCase::ConnectionEstablishedCallback, this));
    Config::Connect(
        "/NodeList/*/DeviceList/*/NrUeRrc/ConnectionTimeout",
        MakeCallback(&NrRrcConnectionEstablishmentTestCase::ConnectionTimeoutCallback, this));

    Simulator::Stop(MilliSeconds(tmax + 1));

    Simulator::Run();

    Simulator::Destroy();
}

void
NrRrcConnectionEstablishmentTestCase::Connect(Ptr<NetDevice> ueDevice, Ptr<NetDevice> gnbDevice)
{
    NS_LOG_FUNCTION(this);
    m_nrHelper->AttachToGnb(ueDevice, gnbDevice);

    for (uint32_t b = 0; b < m_nBearers; ++b)
    {
        NrEpsBearer::Qci q = NrEpsBearer::NGBR_VIDEO_TCP_DEFAULT;
        NrEpsBearer bearer(q);
        m_nrHelper->ActivateDataRadioBearer(ueDevice, bearer);
    }
}

void
NrRrcConnectionEstablishmentTestCase::CheckConnected(Ptr<NetDevice> ueDevice,
                                                     Ptr<NetDevice> gnbDevice)
{
    Ptr<NrUeNetDevice> ueNrDevice = ueDevice->GetObject<NrUeNetDevice>();
    Ptr<NrUeRrc> ueRrc = ueNrDevice->GetRrc();
    const uint64_t imsi = ueNrDevice->GetImsi();
    const uint16_t rnti = ueRrc->GetRnti();
    NS_LOG_FUNCTION(this << imsi << rnti);
    NS_ASSERT_MSG(m_isConnectionEstablished.find(imsi) != m_isConnectionEstablished.end(),
                  "Invalid IMSI " << imsi);

    if (!m_admitRrcConnectionRequest)
    {
        NS_TEST_ASSERT_MSG_EQ(m_isConnectionEstablished[imsi],
                              false,
                              "Connection with RNTI " << rnti << " should have been rejected");
        return;
    }

    /*
     * Verifying UE state in UE RRC. Try to increase the test case duration if
     * the following checks.
     */
    NS_TEST_ASSERT_MSG_EQ(m_isConnectionEstablished[imsi],
                          true,
                          "RNTI " << rnti << " fails to establish connection");
    NS_TEST_ASSERT_MSG_EQ(ueRrc->GetState(),
                          NrUeRrc::CONNECTED_NORMALLY,
                          "RNTI " << rnti << " is not at CONNECTED_NORMALLY state");

    // Verifying UE context state in eNodeB RRC.

    Ptr<NrGnbNetDevice> nrGnbDevice = gnbDevice->GetObject<NrGnbNetDevice>();
    Ptr<NrGnbRrc> gnbRrc = nrGnbDevice->GetRrc();
    const bool hasContext = gnbRrc->HasUeManager(rnti);

    if (hasContext)
    {
        Ptr<NrUeManager> ueManager = gnbRrc->GetUeManager(rnti);
        NS_ASSERT(ueManager);
        NS_TEST_ASSERT_MSG_EQ(ueManager->GetState(),
                              NrUeManager::CONNECTED_NORMALLY,
                              "The context of RNTI " << rnti << " is in invalid state");
    }
    else
    {
        NS_LOG_WARN(this << " RNTI " << rnti << " thinks that it has"
                         << " established connection but the eNodeB thinks"
                         << " that the UE has failed on connection setup.");
        /*
         * The standard specifies that this case would exceed the maximum
         * retransmission limit at UE RLC (SRB1), which will then trigger an RLF.
         * However, this behaviour is not implemented yet.
         */
    }

    // Verifying other attributes on both sides.

    uint16_t ueCellId = ueRrc->GetCellId();
    std::vector<uint16_t> gnbCellIds = nrGnbDevice->GetCellIds();
    uint16_t ueImsi = ueNrDevice->GetImsi();

    uint8_t ueDlBandwidth = ueRrc->GetDlBandwidth();
    uint8_t gnbDlBandwidth = nrGnbDevice->GetCellIdDlBandwidth(ueCellId);
    uint8_t ueUlBandwidth = ueRrc->GetUlBandwidth();
    uint8_t gnbUlBandwidth = nrGnbDevice->GetCellIdUlBandwidth(ueCellId);
    uint8_t ueDlEarfcn = ueRrc->GetDlEarfcn();
    uint8_t gnbDlEarfcn = nrGnbDevice->GetCellIdDlEarfcn(ueCellId);
    uint8_t ueUlEarfcn = ueRrc->GetUlEarfcn();
    uint8_t gnbUlEarfcn = nrGnbDevice->GetCellIdUlEarfcn(ueCellId);

    NS_TEST_ASSERT_MSG_EQ(gnbRrc->HasCellId(ueCellId), true, "inconsistent CellId");
    NS_TEST_ASSERT_MSG_EQ(ueDlBandwidth, gnbDlBandwidth, "inconsistent DlBandwidth");
    NS_TEST_ASSERT_MSG_EQ(ueUlBandwidth, gnbUlBandwidth, "inconsistent UlBandwidth");
    NS_TEST_ASSERT_MSG_EQ(ueDlEarfcn, gnbDlEarfcn, "inconsistent DlEarfcn");
    NS_TEST_ASSERT_MSG_EQ(ueUlEarfcn, gnbUlEarfcn, "inconsistent UlEarfcn");

    if (hasContext)
    {
        Ptr<NrUeManager> ueManager = gnbRrc->GetUeManager(rnti);
        NS_ASSERT(ueManager);
        NrUeManager::State state = ueManager->GetState();
        uint16_t gnbImsi = ueManager->GetImsi();
        NS_TEST_ASSERT_MSG_EQ(ueImsi, gnbImsi, "inconsistent Imsi");

        if (state == NrUeManager::CONNECTED_NORMALLY)
        {
            ObjectMapValue gnbDataRadioBearerMapValue;
            ueManager->GetAttribute("DataRadioBearerMap", gnbDataRadioBearerMapValue);
            NS_TEST_ASSERT_MSG_EQ(gnbDataRadioBearerMapValue.GetN(),
                                  m_nBearers,
                                  "wrong num bearers at eNB");
            ObjectMapValue ueDataRadioBearerMapValue;
            ueRrc->GetAttribute("DataRadioBearerMap", ueDataRadioBearerMapValue);
            NS_TEST_ASSERT_MSG_EQ(ueDataRadioBearerMapValue.GetN(),
                                  m_nBearers,
                                  "wrong num bearers at UE");

            auto gnbBearerIt = gnbDataRadioBearerMapValue.Begin();
            auto ueBearerIt = ueDataRadioBearerMapValue.Begin();
            while (gnbBearerIt != gnbDataRadioBearerMapValue.End() &&
                   ueBearerIt != ueDataRadioBearerMapValue.End())
            {
                Ptr<NrDataRadioBearerInfo> gnbDrbInfo =
                    gnbBearerIt->second->GetObject<NrDataRadioBearerInfo>();
                Ptr<NrDataRadioBearerInfo> ueDrbInfo =
                    ueBearerIt->second->GetObject<NrDataRadioBearerInfo>();
                // NS_TEST_ASSERT_MSG_EQ (gnbDrbInfo->m_epsBearer, ueDrbInfo->m_epsBearer,
                // "epsBearer differs");
                NS_TEST_ASSERT_MSG_EQ((uint32_t)gnbDrbInfo->m_epsBearerIdentity,
                                      (uint32_t)ueDrbInfo->m_epsBearerIdentity,
                                      "epsBearerIdentity differs");
                NS_TEST_ASSERT_MSG_EQ((uint32_t)gnbDrbInfo->m_drbIdentity,
                                      (uint32_t)ueDrbInfo->m_drbIdentity,
                                      "drbIdentity differs");
                // NS_TEST_ASSERT_MSG_EQ (gnbDrbInfo->m_rlcConfig, ueDrbInfo->m_rlcConfig,
                // "rlcConfig differs");
                NS_TEST_ASSERT_MSG_EQ((uint32_t)gnbDrbInfo->m_logicalChannelIdentity,
                                      (uint32_t)ueDrbInfo->m_logicalChannelIdentity,
                                      "logicalChannelIdentity differs");
                // NS_TEST_ASSERT_MSG_EQ (gnbDrbInfo->m_logicalChannelConfig,
                // ueDrbInfo->m_logicalChannelConfig, "logicalChannelConfig differs");

                ++gnbBearerIt;
                ++ueBearerIt;
            }

            NS_ASSERT_MSG(gnbBearerIt == gnbDataRadioBearerMapValue.End(),
                          "too many bearers at eNB");
            NS_ASSERT_MSG(ueBearerIt == ueDataRadioBearerMapValue.End(), "too many bearers at UE");
        }
    }
}

void
NrRrcConnectionEstablishmentTestCase::CheckNotConnected(Ptr<NetDevice> ueDevice,
                                                        Ptr<NetDevice> gnbDevice)
{
    Ptr<NrUeNetDevice> ueNrDevice = ueDevice->GetObject<NrUeNetDevice>();
    Ptr<NrUeRrc> ueRrc = ueNrDevice->GetRrc();
    const uint64_t imsi = ueNrDevice->GetImsi();
    const uint16_t rnti = ueRrc->GetRnti();
    NS_LOG_FUNCTION(this << imsi << rnti);
    NS_ASSERT_MSG(m_isConnectionEstablished.find(imsi) != m_isConnectionEstablished.end(),
                  "Invalid IMSI " << imsi);

    bool ueStateIsConnectedNormally = (NrUeRrc::CONNECTED_NORMALLY == ueRrc->GetState());

    Ptr<NrGnbNetDevice> nrGnbDevice = gnbDevice->GetObject<NrGnbNetDevice>();
    Ptr<NrGnbRrc> gnbRrc = nrGnbDevice->GetRrc();
    const bool hasContext = gnbRrc->HasUeManager(rnti);
    bool contextStateIsConnectedNormally = false;
    if (hasContext)
    {
        Ptr<NrUeManager> ueManager = gnbRrc->GetUeManager(rnti);
        NS_ASSERT(ueManager);
        contextStateIsConnectedNormally =
            (NrUeManager::CONNECTED_NORMALLY == ueManager->GetState());
    }
    NS_TEST_ASSERT_MSG_EQ(
        (!m_isConnectionEstablished[imsi] || !ueStateIsConnectedNormally || !hasContext ||
         !contextStateIsConnectedNormally),
        true,
        "it should not happen that connection is completed both at the UE and at the gNB side");
}

void
NrRrcConnectionEstablishmentTestCase::ConnectionEstablishedCallback(std::string context,
                                                                    uint64_t imsi,
                                                                    uint16_t cellId,
                                                                    uint16_t rnti)
{
    NS_LOG_FUNCTION(this << imsi << cellId);
    m_isConnectionEstablished[imsi] = true;
}

void
NrRrcConnectionEstablishmentTestCase::ConnectionTimeoutCallback(std::string context,
                                                                uint64_t imsi,
                                                                uint16_t cellId,
                                                                uint16_t rnti,
                                                                uint8_t connEstFailCount)
{
    NS_LOG_FUNCTION(this << imsi << cellId);
}

/**
 * \ingroup nr-test
 *
 * \brief Nr Rrc Connection Establishment Error Test Case
 */
class NrRrcConnectionEstablishmentErrorTestCase : public NrRrcConnectionEstablishmentTestCase
{
  public:
    /**
     *
     *
     * \param jumpAwayTime the time when all the UEs 'teleport' to a pre-defined
     *                     high-interference position and stay there for 100 ms
     * \param description additional description of the test case
     */
    NrRrcConnectionEstablishmentErrorTestCase(bool isFdd,
                                              Time jumpAwayTime,
                                              std::string description = "");

  protected:
    void DoRun() override;

  private:
    /// Jump away function
    void JumpAway();
    /// Jump back function
    void JumpBack();

    Time m_jumpAwayTime;             ///< jump away time
    Ptr<MobilityModel> m_ueMobility; ///< UE mobility model
};

NrRrcConnectionEstablishmentErrorTestCase::NrRrcConnectionEstablishmentErrorTestCase(
    bool isFdd,
    Time jumpAwayTime,
    std::string description)
    : NrRrcConnectionEstablishmentTestCase(isFdd, 1, 1, 0, 0, 1, true, false, true, description),
      m_jumpAwayTime(jumpAwayTime)
{
    NS_LOG_FUNCTION(this << GetName());
}

void
NrRrcConnectionEstablishmentErrorTestCase::DoRun()
{
    NS_LOG_FUNCTION(this << GetName());
    Config::Reset();

    if (m_nUes < 25)
    {
        Config::SetDefault("ns3::NrGnbRrc::SrsPeriodicity", UintegerValue(40));
    }
    else if (m_nUes < 60)
    {
        Config::SetDefault("ns3::NrGnbRrc::SrsPeriodicity", UintegerValue(80));
    }
    else if (m_nUes < 120)
    {
        Config::SetDefault("ns3::NrGnbRrc::SrsPeriodicity", UintegerValue(160));
    }
    else
    {
        Config::SetDefault("ns3::NrGnbRrc::SrsPeriodicity", UintegerValue(320));
    }

    // normal code
    m_nrHelper = CreateObject<NrHelper>();
    m_nrHelper->SetAttribute("CsiFeedbackFlags", UintegerValue(CsiFeedbackFlag::CQI_PDSCH_SISO));
    m_nrHelper->SetAttribute("UseIdealRrc", BooleanValue(m_useIdealRrc));

    auto bandwidthAndBWPPair =
        m_nrHelper->CreateBandwidthParts({{2.8e9, 10e6, static_cast<uint8_t>(m_isFdd ? 2 : 1)}},
                                         "UMa");
    if (m_isFdd)
    {
        Config::SetDefault("ns3::NrUeNetDevice::PrimaryUlIndex", UintegerValue(1));
    }

    NodeContainer gnbNodes;
    NodeContainer ueNodes;

    gnbNodes.Create(4);
    ueNodes.Create(1);

    MobilityHelper mobility;
    mobility.Install(ueNodes); // UE position at (0, 0, 0)
    m_ueMobility = ueNodes.Get(0)->GetObject<MobilityModel>();

    Ptr<ListPositionAllocator> gnbPosition = CreateObject<ListPositionAllocator>();
    gnbPosition->Add(Vector(0, 0, 0));
    gnbPosition->Add(Vector(100.0, 0, 0));
    gnbPosition->Add(Vector(0, 100.0, 0));
    gnbPosition->Add(Vector(100.0, 100.0, 0));
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.SetPositionAllocator(gnbPosition);
    mobility.Install(gnbNodes);

    int64_t stream = 1;
    NetDeviceContainer gnbDevs;
    gnbDevs = m_nrHelper->InstallGnbDevice(gnbNodes, bandwidthAndBWPPair.second);
    stream += m_nrHelper->AssignStreams(gnbDevs, stream);

    NetDeviceContainer ueDevs;
    ueDevs = m_nrHelper->InstallUeDevice(ueNodes, bandwidthAndBWPPair.second);
    stream += m_nrHelper->AssignStreams(ueDevs, stream);

    m_nrHelper->AttachToClosestGnb(ueDevs, gnbDevs);
    /*
    // custom code used for testing purposes
    // instead of nrHelper->Attach () and nrHelper->ActivateXxx

    // Set AdmitConnectionRequest attribute
    for (auto it = gnbDevs.Begin(); it != gnbDevs.End(); ++it)
    {
        Ptr<NrGnbRrc> gnbRrc = (*it)->GetObject<NrGnbNetDevice>()->GetRrc();
        gnbRrc->SetAttribute("AdmitRrcConnectionRequest",
                             BooleanValue(m_admitRrcConnectionRequest));
    }

    uint32_t i = 0;
    uint32_t tmax = 0;
    for (auto it = ueDevs.Begin(); it != ueDevs.End(); ++it)
    {
        Ptr<NetDevice> ueDevice = *it;
        Ptr<NetDevice> gnbDevice = gnbDevs.Get(0);
        Ptr<NrUeNetDevice> ueNrDevice = ueDevice->GetObject<NrUeNetDevice>();

        uint32_t tc = m_tConnBase + m_tConnIncrPerUe * i; // time connection start
        uint32_t tcc = tc + m_delayConnEnd;               // time check connection completed;
        uint32_t td = tcc + m_delayDiscStart;             // time disconnect start
        uint32_t tcd = td + m_delayDiscEnd;               // time check disconnection completed
        tmax = std::max(tmax, tcd);

        // trick to resolve overloading
        // void (NrHelper::* overloadedAttachFunctionPointer) (Ptr<NetDevice>, Ptr<NetDevice>) =
        // &NrHelper::Attach; Simulator::Schedule (MilliSeconds (tc),
        // overloadedAttachFunctionPointer, nrHelper, *it, gnbDevice);
        Simulator::Schedule(MilliSeconds(tc),
                            &NrRrcConnectionEstablishmentErrorTestCase::Connect,
                            this,
                            ueDevice,
                            gnbDevice);

        // disconnection not supported yet

        uint64_t imsi = ueNrDevice->GetImsi();
        m_isConnectionEstablished[imsi] = false;

        ++i;
    }*/

    // Connect to trace sources in UEs
    Config::Connect(
        "/NodeList/*/DeviceList/*/NrUeRrc/ConnectionEstablished",
        MakeCallback(&NrRrcConnectionEstablishmentErrorTestCase::ConnectionEstablishedCallback,
                     this));
    Config::Connect(
        "/NodeList/*/DeviceList/*/NrUeRrc/ConnectionTimeout",
        MakeCallback(&NrRrcConnectionEstablishmentErrorTestCase::ConnectionTimeoutCallback, this));

    // Simulator::Schedule(m_jumpAwayTime, &NrRrcConnectionEstablishmentErrorTestCase::JumpAway,
    // this); Simulator::Schedule(m_jumpAwayTime + MilliSeconds(99),
    //                     &NrRrcConnectionEstablishmentErrorTestCase::CheckNotConnected,
    //                     this,
    //                     ueDevs.Get(0),
    //                     gnbDevs.Get(0));
    // Simulator::Schedule(m_jumpAwayTime + MilliSeconds(100),
    //                     &NrRrcConnectionEstablishmentErrorTestCase::JumpBack,
    //                     this);

    Simulator::Stop(MilliSeconds(/*tmax*/ +1));

    Simulator::Run();

    Simulator::Destroy();
}

void
NrRrcConnectionEstablishmentErrorTestCase::JumpAway()
{
    NS_LOG_FUNCTION(this);
    // move to a really far away location so that transmission errors occur
    m_ueMobility->SetPosition(Vector(100000.0, 100000.0, 0.0));
}

void
NrRrcConnectionEstablishmentErrorTestCase::JumpBack()
{
    NS_LOG_FUNCTION(this);
    m_ueMobility->SetPosition(Vector(0.0, 0.0, 0.0));
}

/**
 * \ingroup nr-test
 *
 * \brief Nr Rrc Test Suite
 */
class NrRrcTestSuite : public TestSuite
{
  public:
    NrRrcTestSuite();
};

NrRrcTestSuite::NrRrcTestSuite()
    : TestSuite("nr-rrc", Type::SYSTEM)
{
    //  LogComponentEnableAll (LOG_PREFIX_ALL);
    //  LogComponentEnable ("NrRrcTest", LOG_LEVEL_ALL);
    //  LogComponentEnable ("NrGnbRrc", LOG_INFO);
    //  LogComponentEnable ("NrUeRrc", LOG_INFO);

    NS_LOG_FUNCTION(this);
    for (auto isFdd : {false, true})
    {
        // <----- all times in ms ----------------->
        for (auto useIdealRrc : {/*false,*/ true}) // todo: fix RRC real
        {
            // isFdd nUes tConnBase delayDiscStart useIdealRrc nBearers tConnIncrPerUe errorExpected
            // admitRrcConnectionRequest
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 1,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 1,
                                                                 0,
                                                                 100,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 1,
                                                                 1,
                                                                 0,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 1,
                                                                 1,
                                                                 100,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 1,
                                                                 2,
                                                                 0,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 1,
                                                                 2,
                                                                 100,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 2,
                                                                 0,
                                                                 20,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 2,
                                                                 0,
                                                                 20,
                                                                 10,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 2,
                                                                 0,
                                                                 20,
                                                                 100,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 2,
                                                                 1,
                                                                 20,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 2,
                                                                 1,
                                                                 20,
                                                                 10,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 2,
                                                                 1,
                                                                 20,
                                                                 100,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 2,
                                                                 2,
                                                                 20,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 2,
                                                                 2,
                                                                 20,
                                                                 10,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::QUICK);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 2,
                                                                 2,
                                                                 20,
                                                                 100,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 3,
                                                                 0,
                                                                 20,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 4,
                                                                 0,
                                                                 20,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 4,
                                                                 0,
                                                                 20,
                                                                 300,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 20,
                                                                 0,
                                                                 10,
                                                                 1,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 50,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 true),
                        TestCase::Duration::EXTENSIVE);

            // Test cases to check admitRrcConnectionRequest=false
            //                                                     nUes      tConnBase
            //                                                     delayDiscStart useIdealRrc
            //                                                        nBearers       tConnIncrPerUe
            //                                                        errorExpected
            //                                                        admitRrcConnectionRequest
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 1,
                                                                 0,
                                                                 0,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 false),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 1,
                                                                 2,
                                                                 100,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 false),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 2,
                                                                 0,
                                                                 20,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 false),
                        TestCase::Duration::EXTENSIVE);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 2,
                                                                 1,
                                                                 20,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 false),
                        TestCase::Duration::QUICK);
            AddTestCase(new NrRrcConnectionEstablishmentTestCase(isFdd,
                                                                 3,
                                                                 0,
                                                                 20,
                                                                 0,
                                                                 1,
                                                                 false,
                                                                 useIdealRrc,
                                                                 false),
                        TestCase::Duration::EXTENSIVE);
        }

        // Test cases with transmission error
        AddTestCase(
            new NrRrcConnectionEstablishmentErrorTestCase(isFdd,
                                                          Seconds(0.020214),
                                                          "failure at RRC Connection Request"),
            TestCase::Duration::QUICK);
        AddTestCase(
            new NrRrcConnectionEstablishmentErrorTestCase(isFdd,
                                                          Seconds(0.025),
                                                          "failure at RRC Connection Setup"),
            TestCase::Duration::QUICK);
        /*
         * With RLF implementation we now do support the Idle mode,
         * thus it solve Bug 1762 Comment #25.
         */
        AddTestCase(new NrRrcConnectionEstablishmentErrorTestCase(
                        isFdd,
                        Seconds(0.030),
                        "failure at RRC Connection Setup Complete"),
                    TestCase::Duration::QUICK);
    }
}

/**
 * \ingroup nr-test
 * Static variable for test initialization
 */
static NrRrcTestSuite g_nrRrcTestSuiteInstance;

// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-sl-helper.h"

#include "ns3/abort.h"
#include "ns3/bandwidth-part-ue.h"
#include "ns3/double.h"
#include "ns3/fatal-error.h"
#include "ns3/isotropic-antenna-model.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/nr-epc-ue-nas.h"
#include "ns3/nr-rrc-sap.h"
#include "ns3/nr-sl-bwp-manager-ue.h"
#include "ns3/nr-sl-chunk-processor.h"
#include "ns3/nr-sl-comm-resource-pool-factory.h"
#include "ns3/nr-sl-epc-ue-nas.h"
#include "ns3/nr-sl-mcs-controller.h"
#include "ns3/nr-sl-spectrum-phy.h"
#include "ns3/nr-sl-static-error-model.h"
#include "ns3/nr-sl-tft.h"
#include "ns3/nr-sl-ue-mac-scheduler-default.h"
#include "ns3/nr-sl-ue-mac-scheduler.h"
#include "ns3/nr-sl-ue-mac.h"
#include "ns3/nr-sl-ue-phy.h"
#include "ns3/nr-sl-ue-rrc.h"
#include "ns3/nr-ue-mac.h"
#include "ns3/nr-ue-net-device.h"
#include "ns3/nr-ue-phy.h"
#include "ns3/nr-ue-rrc.h"
#include "ns3/object-factory.h"
#include "ns3/object-map.h"
#include "ns3/pointer.h"
#include "ns3/simulator.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NrSlHelper");

NS_OBJECT_ENSURE_REGISTERED(NrSlHelper);

NrSlHelper::NrSlHelper()

{
    NS_LOG_FUNCTION(this);
    // Override default configuration of NrHelper factories
    SetUePhyTypeId(NrSlUePhy::GetTypeId());
    SetUeSpectrumTypeId(NrSlSpectrumPhy::GetTypeId());
    SetUeMacTypeId(NrSlUeMac::GetTypeId());
    SetBwpManagerTypeId(NrSlBwpManagerUe::GetTypeId());
    SetUeRrcTypeId(NrSlUeRrc::GetTypeId());
    SetEpcUeNasTypeId(NrSlEpcUeNas::GetTypeId());
    // Some SL specific factories
    m_ueSlSchedulerFactory.SetTypeId(NrSlUeMacSchedulerDefault::GetTypeId());
    m_ueSlErrorModelFactory.SetTypeId(NrSlStaticErrorModel::GetTypeId());
    m_mcsControllerFactory.SetTypeId(NrSlMcsController::GetTypeId());
}

NrSlHelper::~NrSlHelper()
{
    NS_LOG_FUNCTION(this);
}

TypeId
NrSlHelper::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrSlHelper")
                            .SetParent<NrHelper>()
                            .SetGroupName("nr")
                            .AddConstructor<NrSlHelper>()
                            .AddAttribute("CentralFrequency",
                                          "Default value in Hz for ConfigureSlNetwork",
                                          DoubleValue(793e6),
                                          MakeDoubleAccessor(&NrSlHelper::m_centralFrequency),
                                          MakeDoubleChecker<double>())
                            .AddAttribute("Bandwidth",
                                          "Default value in Hz for ConfigureSlNetwork",
                                          DoubleValue(10e6),
                                          MakeDoubleAccessor(&NrSlHelper::m_bandwidth),
                                          MakeDoubleChecker<double>())
                            .AddAttribute("Numerology",
                                          "Default value for ConfigureSlNetwork",
                                          UintegerValue(0),
                                          MakeUintegerAccessor(&NrSlHelper::m_numerology),
                                          MakeUintegerChecker<uint16_t>());
    return tid;
}

void
NrSlHelper::DoDispose()
{
    NS_LOG_FUNCTION(this);
    NrHelper::DoDispose();
}

void
NrSlHelper::ActivateNrSlBearer(Time activationTime, NetDeviceContainer ues, const Ptr<NrSlTft> tft)
{
    NS_LOG_FUNCTION(this << activationTime.As(Time::S) << ues.GetN()
                         << tft->GetSidelinkInfo().m_dstL2Id << tft->GetSidelinkInfo().m_lcId);
    Simulator::Schedule(activationTime, &NrSlHelper::DoActivateNrSlBearer, this, ues, tft);
}

void
NrSlHelper::DoActivateNrSlBearer(NetDeviceContainer ues, const Ptr<NrSlTft> tft)
{
    NS_LOG_FUNCTION(this << ues.GetN() << tft);
    for (NetDeviceContainer::Iterator i = ues.Begin(); i != ues.End(); ++i)
    {
        NS_LOG_DEBUG("Activating SL bearer on "
                     << (*i)->GetNode()->GetId() << " at " << Simulator::Now().As(Time::S)
                     << " destination L2 id " << tft->GetSidelinkInfo().m_dstL2Id << " LC id "
                     << +tft->GetSidelinkInfo().m_lcId);
        ActivateNrSlBearerForUe(*i, Create<NrSlTft>(tft));
    }
}

void
NrSlHelper::PrepareUeForSidelink(NetDeviceContainer c, const std::set<uint8_t>& slBwpIds)
{
    NS_LOG_FUNCTION(this);
    for (NetDeviceContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        Ptr<NetDevice> netDev = *i;
        Ptr<NrUeNetDevice> nrUeDev = netDev->GetObject<NrUeNetDevice>();
        PrepareSingleUeForSidelink(nrUeDev, slBwpIds);
    }
}

void
NrSlHelper::PrepareSingleUeForSidelink(Ptr<NrUeNetDevice> nrUeDev,
                                       const std::set<uint8_t>& slBwpIds)
{
    NS_LOG_FUNCTION(this);

    Ptr<NrSlUeRrc> nrSlUeRrc = nrUeDev->GetRrc()->GetObject<NrSlUeRrc>();
    NS_ASSERT_MSG(nrSlUeRrc, "Error, NrUeRrc object is not of type NrSlUeRrc");
    nrSlUeRrc->SetNrSlEnabled(true);

    // SL BWP manager configuration
    Ptr<NrSlBwpManagerUe> slBwpManager = DynamicCast<NrSlBwpManagerUe>(nrUeDev->GetBwpManager());
    slBwpManager->SetNrSlUeBwpmRrcSapUser(nrSlUeRrc->GetNrSlUeBwpmRrcSapUser());
    nrSlUeRrc->SetNrSlUeBwpmRrcSapProvider(slBwpManager->GetNrSlUeBwpmRrcSapProvider());

    nrSlUeRrc->SetNrSlMacSapProvider(slBwpManager->GetNrSlMacSapProviderFromBwpm());

    // Error model and UE MAC AMC
    // Retrieve the CC map from the device so we can set the SL scheduler
    std::map<uint8_t, Ptr<BandwidthPartUe>> ccMap = nrUeDev->GetCcMap();

    for (const auto& itBwps : slBwpIds)
    {
        auto nrSlUeMac = nrUeDev->GetMac(itBwps)->GetObject<NrSlUeMac>();
        if (!nrSlUeMac)
        {
            NS_LOG_DEBUG("Skipping installation of SL components on node "
                         << nrUeDev->GetNode()->GetId() << " bwpId " << +itBwps);
            // could be a relay with some BWPs not configured for sidelink
            continue;
        }
        NS_LOG_INFO("Installation of SL components on node " << nrUeDev->GetNode()->GetId()
                                                             << " bwpId " << +itBwps);
        // Store BWP id in NrSlUeRrc
        nrUeDev->GetRrc()->GetObject<NrSlUeRrc>()->StoreSlBwpId(itBwps);
        // SAPs between the RRC and the NR UE MAC
        nrSlUeRrc->SetNrSlUeCmacSapProvider(itBwps, nrSlUeMac->GetNrSlUeCmacSapProvider());
        nrSlUeMac->SetNrSlUeCmacSapUser(nrSlUeRrc->GetNrSlUeCmacSapUser());
        // SAPs between the RRC and the NR UE PHY
        auto nrSlUePhy = nrUeDev->GetPhy(itBwps)->GetObject<NrSlUePhy>();
        NS_ASSERT_MSG(nrSlUePhy, "No NrSlUePhy found");
        nrSlUePhy->SetNrSlUeCphySapUser(nrSlUeRrc->GetNrSlUeCphySapUser());
        nrSlUeRrc->SetNrSlUeCphySapProvider(itBwps, nrSlUePhy->GetNrSlUeCphySapProvider());
        // NR SL UE MAC scheduler
        Ptr<NrSlUeMacScheduler> sched = m_ueSlSchedulerFactory.Create<NrSlUeMacScheduler>();
        NS_ABORT_MSG_IF(sched == nullptr, "sched is null");
        // Connect NrSlUeMac and NrSlUeMacScheduler
        sched->SetNrSlUeMac(nrSlUeMac);
        nrSlUeMac->SetAttribute("NrSlUeMacScheduler", PointerValue(sched));
        // MCS controller (only if the scheduler derives from NrSlUeMacSchedulerDefault)
        if (sched->GetInstanceTypeId() == NrSlUeMacSchedulerDefault::GetTypeId() ||
            sched->GetInstanceTypeId().IsChildOf(NrSlUeMacSchedulerDefault::GetTypeId()))
        {
            auto defaultSched = sched->GetObject<NrSlUeMacSchedulerDefault>();
            Ptr<NrSlMcsController> controller = m_mcsControllerFactory.Create<NrSlMcsController>();
            NS_ABORT_MSG_IF(!controller, "Failed to create an MCS controller");
            // Connect NrSlUeMac and NrSlMcsController
            controller->SetNrSlUeMac(nrSlUeMac);
            controller->SetNrSlUeMacScheduler(defaultSched);
            defaultSched->SetAttribute("McsController", PointerValue(controller));
        }
        // Set AMC in the NR SL UE MAC scheduler
        Ptr<NrSlUeMacScheduler> schedNs3 = sched->GetObject<NrSlUeMacScheduler>();
        // SAPs between MAC and PHY
        nrSlUePhy->SetNrSlUePhySapUser(nrSlUeMac->GetNrSlUePhySapUser());
        nrSlUeMac->SetNrSlUePhySapProvider(nrSlUePhy->GetNrSlUePhySapProvider());
        // Error model type in NRSpectrumPhy for NR SL
        Ptr<NrSlSpectrumPhy> spectrumPhy =
            nrSlUePhy->GetSpectrumPhy()->GetObject<NrSlSpectrumPhy>();
        NS_ASSERT_MSG(spectrumPhy, "Did not find NrSlSpectrumPhy object");
        spectrumPhy->SetSlErrorModel(m_ueSlErrorModelFactory.Create<NrSlErrorModel>());
        // Set SL chunk processor
        Ptr<NrSlChunkProcessor> pSlSinr = Create<NrSlChunkProcessor>();
        pSlSinr->AddCallback(MakeCallback(&NrSlSpectrumPhy::UpdateSlSinrPerceived, spectrumPhy));
        spectrumPhy->AddSlSinrChunkProcessor(pSlSinr);
        Ptr<NrSlChunkProcessor> pSlSignal = Create<NrSlChunkProcessor>();
        pSlSignal->AddCallback(
            MakeCallback(&NrSlSpectrumPhy::UpdateSlSignalPerceived, spectrumPhy));
        spectrumPhy->AddSlSignalChunkProcessor(pSlSignal);

        std::function<void(const Ptr<Packet>&, const SpectrumValue&)> pscchPhyPduCallback;
        pscchPhyPduCallback = std::bind(&NrSlUePhy::PhyPscchPduReceived,
                                        nrSlUePhy,
                                        std::placeholders::_1,
                                        std::placeholders::_2);
        spectrumPhy->SetNrPhyRxPscchEndOkCallback(pscchPhyPduCallback);

        std::function<void(const Ptr<PacketBurst>&, const SpectrumValue&)> psschPhyPduOkCallback;
        psschPhyPduOkCallback = std::bind(&NrSlUePhy::PhyPsschPduReceived,
                                          nrSlUePhy,
                                          std::placeholders::_1,
                                          std::placeholders::_2);
        spectrumPhy->SetNrPhyRxPsschEndOkCallback(psschPhyPduOkCallback);

        std::function<void(uint32_t, SlHarqInfo)> psfchCallback;
        psfchCallback = std::bind(&NrSlUePhy::PhyPsfchReceived,
                                  nrSlUePhy,
                                  std::placeholders::_1,
                                  std::placeholders::_2);
        spectrumPhy->SetNrPhyRxSlPsfchCallback(psfchCallback);

        spectrumPhy->SetPhySlHarqFeedbackCallback(
            MakeCallback(&NrSlUePhy::EnqueueSlHarqFeedback, nrSlUePhy));

        // Set the SAP of NR UE MAC in SL BWP manager
        bool bwpmTest =
            slBwpManager->SetNrSlMacSapProviders(itBwps, nrSlUeMac->GetNrSlMacSapProvider());

        if (!bwpmTest)
        {
            NS_FATAL_ERROR("Error in SetNrSlMacSapProviders");
        }
    }

    // Since now all the BWP for SL are configured, we can configure src L2 id
    // for only SL BWP (s) (see NrUeRrc::DoSetSourceL2Id)
    uint64_t imsi = nrSlUeRrc->GetImsi();
    NS_ASSERT_MSG(imsi != NrUeRrc::IMSI_UNASSIGNED, "IMSI was not set in UE RRC");
    nrSlUeRrc->SetSourceL2Id(
        static_cast<uint32_t>(imsi & 0xFFFFFF)); // use lower 24 bits of IMSI as source

    nrSlUeRrc->SetNrSlBwpIdContainerInBwpm();
}

void
NrSlHelper::InstallNrSlPreConfiguration(NetDeviceContainer c,
                                        const NrSlRrcSap::SidelinkPreconfigNr preConfig)
{
    NS_LOG_FUNCTION(this);

    struct NrSlRrcSap::SlFreqConfigCommonNr slFreqConfigCommonNr =
        preConfig.slPreconfigFreqInfoList[0];
    NrSlRrcSap::SlPreconfigGeneralNr slPreconfigGeneralNr = preConfig.slPreconfigGeneral;

    for (NetDeviceContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        Ptr<NetDevice> netDev = *i;
        Ptr<NrUeNetDevice> nrUeDev = netDev->GetObject<NrUeNetDevice>();
        bool ueSlBwpConfigured =
            ConfigUeParams(nrUeDev, slFreqConfigCommonNr, slPreconfigGeneralNr);
        NS_ABORT_MSG_IF(ueSlBwpConfigured == false,
                        "No SL configuration found for IMSI " << nrUeDev->GetImsi());
        Ptr<NrSlUeRrc> nrSlUeRrc = nrUeDev->GetRrc()->GetObject<NrSlUeRrc>();
        nrSlUeRrc->SetNrSlPreconfiguration(preConfig);
    }
}

bool
NrSlHelper::ConfigUeParams(const Ptr<NrUeNetDevice>& dev,
                           const NrSlRrcSap::SlFreqConfigCommonNr& freqCommon,
                           const NrSlRrcSap::SlPreconfigGeneralNr& general)
{
    NS_LOG_FUNCTION(this);
    bool found = false;
    std::string tddPattern = general.slTddConfig.tddPattern;
    // Sanity check: Here we are retrieving the BWP id container
    // from UE RRC to make sure:
    // 1. PrepareUeForSidelink has been called already
    // 2. In the for loop below the index (slBwpList [index]) at which we find the
    // configuration is basically the index of the BWP, which user want to use for SL.
    // So, this index should be present in the BWP id container.
    Ptr<NrSlUeRrc> nrSlUeRrc = dev->GetRrc()->GetObject<NrSlUeRrc>();
    std::set<uint8_t> bwpIds = nrSlUeRrc->GetNrSlBwpIdContainer();

    for (std::size_t index = 0; index < freqCommon.slBwpList.size(); ++index)
    {
        // configure the parameters if both BWP generic and SL pools are configured.
        if (freqCommon.slBwpList[index].haveSlBwpGeneric &&
            freqCommon.slBwpList[index].haveSlBwpPoolConfigCommonNr)
        {
            NS_LOG_INFO("Configuring BWP id " << +index << " for SL");
            auto it = bwpIds.find(index);
            NS_ABORT_MSG_IF(it == bwpIds.end(),
                            "UE is not prepared to use BWP id " << +index << " for SL");
            auto nrSlUePhy = dev->GetPhy(index)->GetObject<NrSlUePhy>();
            nrSlUePhy->RegisterSlBwpId(static_cast<uint16_t>(index));
            nrSlUePhy->SetNumerology(freqCommon.slBwpList[index].slBwpGeneric.bwp.numerology);
            nrSlUePhy->SetSymbolsPerSlot(
                freqCommon.slBwpList[index].slBwpGeneric.bwp.symbolsPerSlots);
            nrSlUePhy->PreConfigSlBandwidth(freqCommon.slBwpList[index].slBwpGeneric.bwp.bandwidth);
            nrSlUePhy->SetNumRbPerRbg(freqCommon.slBwpList[index].slBwpGeneric.bwp.rbPerRbg);
            nrSlUePhy->SetPattern(tddPattern);
            found = true;
        }
    }

    return found;
}

void
NrSlHelper::SetSlErrorModelTypeId(const TypeId& typeId)
{
    NS_LOG_FUNCTION(this);
    m_ueSlErrorModelFactory.SetTypeId(typeId);
}

void
NrSlHelper::SetNrSlSchedulerTypeId(const TypeId& typeId)
{
    NS_LOG_FUNCTION(this);
    m_ueSlSchedulerFactory.SetTypeId(typeId);
}

void
NrSlHelper::SetUeSlSchedulerAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_ueSlSchedulerFactory.Set(n, v);
}

void
NrSlHelper::SetMcsControllerTypeId(const TypeId& typeId)
{
    NS_LOG_FUNCTION(this);
    m_mcsControllerFactory.SetTypeId(typeId);
}

void
NrSlHelper::SetSlResourceReservePeriodList(const std::list<uint16_t>& periodList)
{
    NS_LOG_FUNCTION(this);
    m_resourceReservePeriodList = periodList;
}

void
NrSlHelper::SetSlProbResourceKeep(double probability)
{
    NS_LOG_FUNCTION(this << probability);
    NS_ASSERT_MSG(probability >= 0.0 && probability <= 0.8,
                  "slProbResourceKeep must be between 0 and 0.8");
    m_slProbResourceKeep = probability;
}

void
NrSlHelper::SetSlMaxNumPerReserve(uint16_t maxNumPerReserve)
{
    NS_LOG_FUNCTION(this << maxNumPerReserve);
    NS_ASSERT_MSG(maxNumPerReserve >= 1 && maxNumPerReserve <= 3,
                  "slMaxNumPerReserve must be between 1 and 3");
    m_maxNumPerReserve = maxNumPerReserve;
}

void
NrSlHelper::SetSlMaxHarqProcesses(uint8_t total, uint8_t multiplePdu)
{
    NS_LOG_FUNCTION(this << +total << +multiplePdu);
    NS_ASSERT_MSG(total >= 1 && total <= 16, "total must be between 1 and 16");
    NS_ASSERT_MSG(multiplePdu >= 1 && multiplePdu <= total,
                  "multiplePdu must be between 1 and total");
    m_maxSlHarqProcesses = total;
    m_maxSlHarqProcessesMultiplePdu = multiplePdu;
}

uint16_t
NrSlHelper::GetPhySlPoolLength(uint16_t slBitmapLen,
                               uint16_t tddPatternLen,
                               uint16_t numUlTddPattern)
{
    NS_ABORT_MSG_IF(slBitmapLen % numUlTddPattern != 0,
                    "SL bit map size should be multiple of number of UL slots in the TDD pattern");
    NS_ABORT_MSG_IF(slBitmapLen < tddPatternLen,
                    "SL bit map size should be greater than or equal to the TDD pattern size");
    uint16_t poolLen = (slBitmapLen / numUlTddPattern) * tddPatternLen;
    return poolLen;
}

void
NrSlHelper::ActivateNrSlBearerForUe(const Ptr<NetDevice>& ueDevice, const Ptr<NrSlTft>& slTft) const
{
    Ptr<NrUeNetDevice> nrUeNetDevice = ueDevice->GetObject<NrUeNetDevice>();
    if (nrUeNetDevice)
    {
        auto nrSlEpcUeNas = nrUeNetDevice->GetNas()->GetObject<NrSlEpcUeNas>();
        NS_ASSERT_MSG(nrSlEpcUeNas, "NrSlEpcUeNas not found");
        Simulator::ScheduleWithContext(ueDevice->GetNode()->GetId(),
                                       Time(),
                                       &NrSlEpcUeNas::ActivateNrSlBearer,
                                       nrSlEpcUeNas,
                                       slTft);
    }
    else
    {
        NS_FATAL_ERROR("Invalid device type: " << ueDevice->GetTypeId().GetName());
    }
}

int64_t
NrSlHelper::AssignStreams(NetDeviceContainer c, int64_t stream)
{
    int64_t currentStream = stream;
    currentStream += NrHelper::AssignStreams(c, currentStream);
    return (currentStream - stream);
}

NetDeviceContainer
NrSlHelper::ConfigureSlNetwork(NodeContainer n, Ptr<SpectrumChannel> channel)
{
    NS_LOG_FUNCTION(this << n.GetN() << channel);

    // Create a single bandwidth part and add it to the container (allBwps)
    // needed by the NrSlHelper::Install() method
    std::unique_ptr<BandwidthPartInfo> bwpInfo(new BandwidthPartInfo());
    bwpInfo->m_bwpId = 0;
    bwpInfo->m_centralFrequency = m_centralFrequency;
    bwpInfo->m_lowerFrequency = m_centralFrequency - m_bandwidth / 2;
    bwpInfo->m_higherFrequency = m_centralFrequency + m_bandwidth / 2;
    bwpInfo->SetChannel(channel);
    BandwidthPartInfoPtrVector allBwps;
    allBwps.push_back(bwpInfo);

    /*
     * Antennas for all the UEs
     * We are not using beamforming in SL, rather we are using
     * quasi-omnidirectional transmission and reception, which is the default
     * configuration of the beams.
     */
    SetUeAntennaAttribute("NumRows", UintegerValue(1));
    SetUeAntennaAttribute("NumColumns", UintegerValue(1));
    SetUeAntennaAttribute("AntennaElement", PointerValue(CreateObject<IsotropicAntennaModel>()));

    SetUeMacAttribute("NumHarqProcess", UintegerValue(4));
    SetUeMacAttribute("MaxSidelinkProcess", UintegerValue(m_maxSlHarqProcesses));
    SetUeMacAttribute("MaxSidelinkProcessMultiplePdu",
                      UintegerValue(m_maxSlHarqProcessesMultiplePdu));

    uint8_t bwpIdForGbrMcptt = 0;

    // following parameter has no impact at the moment because:
    // 1. No support for PQI based mapping between the application and the LCs
    // 2. No scheduler to consider PQI
    // However, till such time all the NR SL examples should use GBR_MC_PUSH_TO_TALK
    // because we hard coded the PQI 65 in UE RRC.
    SetUeBwpManagerAlgorithmAttribute("GBR_MC_PUSH_TO_TALK", UintegerValue(bwpIdForGbrMcptt));

    std::set<uint8_t> bwpIdContainer;
    bwpIdContainer.insert(bwpIdForGbrMcptt);

    NetDeviceContainer netDevContainer = InstallUeDevice(n, allBwps);

    /*
     * Very important method to configure UE protocol stack, i.e., it would
     * configure all the SAPs among the layers, setup callbacks, configure
     * error model, configure AMC, and configure ChunkProcessor in Interference
     * API.
     */
    PrepareUeForSidelink(netDevContainer, bwpIdContainer);

    /*
     * Start preparing for all the sub Structs/RRC Information Element (IEs)
     * of NrSlRrcSap::SidelinkPreconfigNr. This is the main structure, which would
     * hold all the pre-configuration related to Sidelink.
     */

    // SlResourcePoolNr IE
    NrSlRrcSap::SlResourcePoolNr slResourcePoolNr;
    // get it from pool factory
    Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory>();
    // Configure specific parameters of interest:
    std::vector<std::bitset<1>> slBitmap = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    ptrFactory->SetSlTimeResources(slBitmap);
    ptrFactory->SetSlSensingWindow(100); // T0 in ms
    ptrFactory->SetSlSelectionWindow(5);
    ptrFactory->SetSlFreqResourcePscch(10); // PSCCH RBs
    ptrFactory->SetSlSubchannelSize(10);
    ptrFactory->SetSlMaxNumPerReserve(m_maxNumPerReserve);
    ptrFactory->SetSlResourceReservePeriodList(m_resourceReservePeriodList);
    // Once parameters are configured, we can create the pool
    NrSlRrcSap::SlResourcePoolNr pool = ptrFactory->CreatePool();
    slResourcePoolNr = pool;

    // Configure the SlResourcePoolConfigNr IE, which holds a pool and its id
    NrSlRrcSap::SlResourcePoolConfigNr slresoPoolConfigNr;
    slresoPoolConfigNr.haveSlResourcePoolConfigNr = true;
    // Pool id, ranges from 0 to 15
    uint16_t poolId = 0;
    NrSlRrcSap::SlResourcePoolIdNr slResourcePoolIdNr;
    slResourcePoolIdNr.id = poolId;
    slresoPoolConfigNr.slResourcePoolId = slResourcePoolIdNr;
    slresoPoolConfigNr.slResourcePool = slResourcePoolNr;

    // Configure the SlBwpPoolConfigCommonNr IE, which holds an array of pools
    NrSlRrcSap::SlBwpPoolConfigCommonNr slBwpPoolConfigCommonNr;
    // Array for pools, we insert the pool in the array as per its poolId
    slBwpPoolConfigCommonNr.slTxPoolSelectedNormal[slResourcePoolIdNr.id] = slresoPoolConfigNr;

    // Configure the BWP IE
    NrSlRrcSap::Bwp bwp;
    bwp.numerology = m_numerology;
    bwp.symbolsPerSlots = 14;
    bwp.rbPerRbg = 1;
    bwp.bandwidth = m_bandwidth / 1e5; // BWP IE specifies in units of 100 KHz

    // Configure the SlBwpGeneric IE
    NrSlRrcSap::SlBwpGeneric slBwpGeneric;
    slBwpGeneric.bwp = bwp;
    slBwpGeneric.slLengthSymbols = NrSlRrcSap::GetSlLengthSymbolsEnum(14);
    slBwpGeneric.slStartSymbol = NrSlRrcSap::GetSlStartSymbolEnum(0);

    // Configure the SlBwpConfigCommonNr IE
    NrSlRrcSap::SlBwpConfigCommonNr slBwpConfigCommonNr;
    slBwpConfigCommonNr.haveSlBwpGeneric = true;
    slBwpConfigCommonNr.slBwpGeneric = slBwpGeneric;
    slBwpConfigCommonNr.haveSlBwpPoolConfigCommonNr = true;
    slBwpConfigCommonNr.slBwpPoolConfigCommonNr = slBwpPoolConfigCommonNr;

    // Configure the SlFreqConfigCommonNr IE, which holds the array to store
    // the configuration of all Sidelink BWP (s).
    NrSlRrcSap::SlFreqConfigCommonNr slFreConfigCommonNr;
    // Array for BWPs. Here we will iterate over the BWPs, which
    // we want to use for SL.
    for (const auto& it : bwpIdContainer)
    {
        // it is the BWP id
        slFreConfigCommonNr.slBwpList[it] = slBwpConfigCommonNr;
    }

    // Configure the TddUlDlConfigCommon IE
    NrSlRrcSap::TddUlDlConfigCommon tddUlDlConfigCommon;
    tddUlDlConfigCommon.tddPattern = "UL|UL|UL|UL|UL|UL|UL|UL|UL|UL|";

    // Configure the SlPreconfigGeneralNr IE
    NrSlRrcSap::SlPreconfigGeneralNr slPreconfigGeneralNr;
    slPreconfigGeneralNr.slTddConfig = tddUlDlConfigCommon;

    // Configure the SlUeSelectedConfig IE
    NrSlRrcSap::SlUeSelectedConfig slUeSelectedPreConfig;
    slUeSelectedPreConfig.slProbResourceKeep = m_slProbResourceKeep;
    // Configure the SlPsschTxParameters IE
    NrSlRrcSap::SlPsschTxParameters psschParams;
    psschParams.slMaxTxTransNumPssch = 5;
    // Configure the SlPsschTxConfigList IE
    NrSlRrcSap::SlPsschTxConfigList pscchTxConfigList;
    pscchTxConfigList.slPsschTxParameters[0] = psschParams;
    slUeSelectedPreConfig.slPsschTxConfigList = pscchTxConfigList;

    /*
     * Finally, configure the SidelinkPreconfigNr This is the main structure
     * that needs to be communicated to NrSlUeRrc class
     */
    NrSlRrcSap::SidelinkPreconfigNr slPreConfigNr;
    slPreConfigNr.slPreconfigGeneral = slPreconfigGeneralNr;
    slPreConfigNr.slUeSelectedPreConfig = slUeSelectedPreConfig;
    slPreConfigNr.slPreconfigFreqInfoList[0] = slFreConfigCommonNr;

    InstallNrSlPreConfiguration(netDevContainer, slPreConfigNr);
    return netDevContainer;
}

} // namespace ns3

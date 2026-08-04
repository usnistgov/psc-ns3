// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-helper.h"

#include "nr-bearer-stats-calculator.h"
#include "nr-channel-helper.h"
#include "nr-epc-helper.h"
#include "nr-mac-rx-trace.h"
#include "nr-phy-rx-trace.h"

#include <ns3/bandwidth-part-gnb.h>
#include <ns3/bandwidth-part-ue.h>
#include <ns3/beam-manager.h>
#include <ns3/buildings-channel-condition-model.h>
#include <ns3/bwp-manager-algorithm.h>
#include <ns3/bwp-manager-gnb.h>
#include <ns3/bwp-manager-ue.h>
#include <ns3/config.h>
#include <ns3/deprecated.h>
#include <ns3/multi-model-spectrum-channel.h>
#include <ns3/names.h>
#include <ns3/nr-ch-access-manager.h>
#include <ns3/nr-chunk-processor.h>
#include <ns3/nr-csi-rs-filter.h>
#include <ns3/nr-epc-gnb-application.h>
#include <ns3/nr-epc-ue-nas.h>
#include <ns3/nr-epc-x2.h>
#include <ns3/nr-fh-control.h>
#include <ns3/nr-gnb-mac.h>
#include <ns3/nr-gnb-net-device.h>
#include <ns3/nr-gnb-phy.h>
#include <ns3/nr-initial-association.h>
#include <ns3/nr-mac-scheduler-tdma-rr.h>
#include <ns3/nr-pm-search-full.h>
#include <ns3/nr-rrc-protocol-ideal.h>
#include <ns3/nr-rrc-protocol-real.h>
#include <ns3/nr-sl-spectrum-phy.h>
#include <ns3/nr-ue-mac.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/nr-ue-phy.h>
#include <ns3/nr-ue-rrc.h>
#include <ns3/pointer.h>
#include <ns3/three-gpp-channel-model.h>
#include <ns3/three-gpp-propagation-loss-model.h>
#include <ns3/three-gpp-spectrum-propagation-loss-model.h>
#include <ns3/three-gpp-v2v-channel-condition-model.h>
#include <ns3/three-gpp-v2v-propagation-loss-model.h>
#include <ns3/uniform-planar-array.h>

#include <algorithm>

namespace ns3
{

/* ... */
NS_LOG_COMPONENT_DEFINE("NrHelper");

NS_OBJECT_ENSURE_REGISTERED(NrHelper);

NrHelper::NrHelper()
{
    NS_LOG_FUNCTION(this);
    m_channelFactory.SetTypeId(MultiModelSpectrumChannel::GetTypeId());
    m_gnbNetDeviceFactory.SetTypeId(NrGnbNetDevice::GetTypeId());
    m_ueNetDeviceFactory.SetTypeId(NrUeNetDevice::GetTypeId());
    m_epcUeNasFactory.SetTypeId(NrEpcUeNas::GetTypeId());
    m_ueRrcFactory.SetTypeId(NrUeRrc::GetTypeId());
    m_ueMacFactory.SetTypeId(NrUeMac::GetTypeId());
    m_gnbMacFactory.SetTypeId(NrGnbMac::GetTypeId());
    m_ueSpectrumFactory.SetTypeId(NrSpectrumPhy::GetTypeId());
    m_gnbSpectrumFactory.SetTypeId(NrSpectrumPhy::GetTypeId());
    m_uePhyFactory.SetTypeId(NrUePhy::GetTypeId());
    m_gnbPhyFactory.SetTypeId(NrGnbPhy::GetTypeId());
    m_ueChannelAccessManagerFactory.SetTypeId(NrAlwaysOnAccessManager::GetTypeId());
    m_gnbChannelAccessManagerFactory.SetTypeId(NrAlwaysOnAccessManager::GetTypeId());
    m_schedFactory.SetTypeId(NrMacSchedulerTdmaRR::GetTypeId());
    m_ueAntennaFactory.SetTypeId(UniformPlanarArray::GetTypeId());
    m_gnbAntennaFactory.SetTypeId(UniformPlanarArray::GetTypeId());
    m_gnbBwpManagerAlgoFactory.SetTypeId(BwpManagerAlgorithmStatic::GetTypeId());
    m_ueBwpManagerAlgoFactory.SetTypeId(BwpManagerAlgorithmStatic::GetTypeId());
    m_gnbUlAmcFactory.SetTypeId(NrAmc::GetTypeId());
    m_gnbDlAmcFactory.SetTypeId(NrAmc::GetTypeId());
    m_gnbBeamManagerFactory.SetTypeId(BeamManager::GetTypeId());
    m_ueBeamManagerFactory.SetTypeId(BeamManager::GetTypeId());
    m_spectrumPropagationFactory.SetTypeId(ThreeGppSpectrumPropagationLossModel::GetTypeId());
    m_initialAttachmentFactory.SetTypeId(NrInitialAssociation::GetTypeId());
    m_bwpManagerFactory.SetTypeId(BwpManagerUe::GetTypeId());

    // Initialization that is there just because the user can configure attribute
    // through the helper methods without making it sad that no TypeId is set.
    // When the TypeId is changed, the user-set attribute will be maintained.
    m_pathlossModelFactory.SetTypeId(ThreeGppPropagationLossModel::GetTypeId());
    m_channelConditionModelFactory.SetTypeId(ThreeGppChannelConditionModel::GetTypeId());
    m_fhControlFactory.SetTypeId(NrFhControl::GetTypeId());

    Config::SetDefault("ns3::NrEpsBearer::Release", UintegerValue(18));
}

NrHelper::~NrHelper()
{
    NS_LOG_FUNCTION(this);
    if (m_beamformingHelper)
    {
        m_beamformingHelper->Dispose();
    }
    m_beamformingHelper = nullptr;
}

TypeId
NrHelper::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NrHelper")
            .SetParent<Object>()
            .AddConstructor<NrHelper>()
            .AddAttribute("EnableMimoFeedback",
                          "Generate CQI feedback with RI and PMI for MIMO support",
                          BooleanValue(false),
                          MakeBooleanAccessor(&NrHelper::m_enableMimoFeedback),
                          MakeBooleanChecker())
            .AddAttribute(
                "CsiFeedbackFlags",
                "Signals and measurements that will be used for CQI feedback if available."
                "Supported configurations are: CQI_PDSCH_MIMO = 1, CQI_CSI_RS = 2, "
                "CQI_PDSCH_MIMO|CQI_CSI_RS = 3, "
                "CQI_CSI_RS|CQI_CSI_IM = 6, CQI_PDSCH_MIMO|CQI_CSI_RS|CQI_CSI_IM = 7, and "
                "CQI_PDSCH_SISO = 8.",
                UintegerValue(CQI_CSI_IM | CQI_CSI_RS | CQI_PDSCH_MIMO),
                MakeUintegerAccessor(&NrHelper::m_csiFeedbackFlags),
                MakeUintegerChecker<uint8_t>(0x0, 0x08))
            .AddAttribute("PmSearchMethod",
                          "Type of the precoding matrix search method.",
                          TypeIdValue(NrPmSearchFull::GetTypeId()),
                          MakeTypeIdAccessor(&NrHelper::SetPmSearchTypeId),
                          MakeTypeIdChecker())
            .AddAttribute("UseIdealRrc",
                          "If true, NrRrcProtocolIdeal will be used for RRC signaling. "
                          "If false, NrRrcProtocolReal will be used.",
                          BooleanValue(true),
                          MakeBooleanAccessor(&NrHelper::m_useIdealRrc),
                          MakeBooleanChecker())
            .AddAttribute("HandoverAlgorithm",
                          "The type of handover algorithm to be used for gNBs. "
                          "The allowed values for this attributes are the type names "
                          "of any class inheriting from ns3::LteHandoverAlgorithm.",
                          StringValue("ns3::NrNoOpHandoverAlgorithm"),
                          MakeStringAccessor(&NrHelper::SetHandoverAlgorithmType,
                                             &NrHelper::GetHandoverAlgorithmType),
                          MakeStringChecker());

    return tid;
}

std::pair<double, BandwidthPartInfoPtrVector>
NrHelper::CreateBandwidthParts(std::vector<CcBwpCreator::SimpleOperationBandConf> bandConfs,
                               const std::string& scenario,
                               const std::string& channelCondition,
                               const std::string& channelModel)
{
    CcBwpCreator ccBwpCreator;
    double totalBandwidth = 0.0;
    auto channelHelper = CreateObject<NrChannelHelper>();
    channelHelper->ConfigureFactories(scenario, channelCondition, channelModel);
    for (auto& bandConf : bandConfs)
    {
        m_bands.push_back(ccBwpCreator.CreateOperationBandContiguousCc(bandConf));
        totalBandwidth += bandConf.m_channelBandwidth;
    }
    std::vector<std::reference_wrapper<OperationBandInfo>> bandsRefs(m_bands.rbegin(),
                                                                     m_bands.rbegin() +
                                                                         bandConfs.size());

    channelHelper->AssignChannelsToBands(bandsRefs);
    return std::make_pair(totalBandwidth, CcBwpCreator::GetAllBwps(bandsRefs));
}

uint32_t
NrHelper::GetNumberBwp(const Ptr<const NetDevice>& gnbDevice)
{
    NS_LOG_FUNCTION(gnbDevice);
    Ptr<const NrGnbNetDevice> netDevice = DynamicCast<const NrGnbNetDevice>(gnbDevice);
    if (netDevice == nullptr)
    {
        return 0;
    }
    return netDevice->GetCcMapSize();
}

Ptr<NrGnbPhy>
NrHelper::GetGnbPhy(const Ptr<NetDevice>& gnbDevice, uint32_t bwpIndex)
{
    NS_LOG_FUNCTION(gnbDevice << bwpIndex);
    NS_ASSERT(bwpIndex < UINT8_MAX);
    Ptr<NrGnbNetDevice> netDevice = DynamicCast<NrGnbNetDevice>(gnbDevice);
    if (netDevice == nullptr)
    {
        return nullptr;
    }
    return netDevice->GetPhy(static_cast<uint8_t>(bwpIndex));
}

Ptr<NrGnbMac>
NrHelper::GetGnbMac(const Ptr<NetDevice>& gnbDevice, uint32_t bwpIndex)
{
    NS_LOG_FUNCTION(gnbDevice << bwpIndex);
    NS_ASSERT(bwpIndex < UINT8_MAX);
    Ptr<NrGnbNetDevice> netDevice = DynamicCast<NrGnbNetDevice>(gnbDevice);
    if (netDevice == nullptr)
    {
        return nullptr;
    }
    return netDevice->GetMac(static_cast<uint8_t>(bwpIndex));
}

Ptr<NrUeMac>
NrHelper::GetUeMac(const Ptr<NetDevice>& ueDevice, uint32_t bwpIndex)
{
    NS_LOG_FUNCTION(ueDevice << bwpIndex);
    NS_ASSERT(bwpIndex < UINT8_MAX);
    Ptr<NrUeNetDevice> netDevice = DynamicCast<NrUeNetDevice>(ueDevice);
    if (netDevice == nullptr)
    {
        return nullptr;
    }
    return netDevice->GetMac(static_cast<uint8_t>(bwpIndex));
}

Ptr<NrUePhy>
NrHelper::GetUePhy(const Ptr<NetDevice>& ueDevice, uint32_t bwpIndex)
{
    NS_LOG_FUNCTION(ueDevice << bwpIndex);
    NS_ASSERT(bwpIndex < UINT8_MAX);
    Ptr<NrUeNetDevice> netDevice = DynamicCast<NrUeNetDevice>(ueDevice);
    if (netDevice == nullptr)
    {
        return nullptr;
    }
    return netDevice->GetPhy(static_cast<uint8_t>(bwpIndex));
}

Ptr<BwpManagerGnb>
NrHelper::GetBwpManagerGnb(const Ptr<NetDevice>& gnbDevice)
{
    NS_LOG_FUNCTION(gnbDevice);

    Ptr<NrGnbNetDevice> netDevice = DynamicCast<NrGnbNetDevice>(gnbDevice);
    if (netDevice == nullptr)
    {
        return nullptr;
    }

    return netDevice->GetBwpManager();
}

Ptr<BwpManagerUe>
NrHelper::GetBwpManagerUe(const Ptr<NetDevice>& ueDevice)
{
    NS_LOG_FUNCTION(ueDevice);

    Ptr<NrUeNetDevice> netDevice = DynamicCast<NrUeNetDevice>(ueDevice);
    if (netDevice == nullptr)
    {
        return nullptr;
    }

    return netDevice->GetBwpManager();
}

Ptr<NrMacScheduler>
NrHelper::GetScheduler(const Ptr<NetDevice>& gnbDevice, uint32_t bwpIndex)
{
    NS_LOG_FUNCTION(gnbDevice << bwpIndex);

    Ptr<NrGnbNetDevice> netDevice = DynamicCast<NrGnbNetDevice>(gnbDevice);
    if (netDevice == nullptr)
    {
        return nullptr;
    }

    return netDevice->GetScheduler(bwpIndex);
}

void
NrHelper::SetSnrTest(bool snrTest)
{
    m_snrTest = snrTest;
}

bool
NrHelper::GetSnrTest() const
{
    return m_snrTest;
}

NetDeviceContainer
NrHelper::InstallUeDevice(const NodeContainer& c,
                          const std::vector<std::reference_wrapper<BandwidthPartInfoPtr>>& allBwps)
{
    NS_LOG_FUNCTION(this);
    NetDeviceContainer devices;
    for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        Ptr<Node> node = *i;
        Ptr<NetDevice> device = InstallSingleUeDevice(node, allBwps);
        device->SetAddress(Mac48Address::Allocate());
        devices.Add(device);
    }
    return devices;
}

NetDeviceContainer
NrHelper::InstallUeDevice(const NodeContainer& c,
                          const std::vector<std::reference_wrapper<BandwidthPartInfoPtr>>& allBwps,
                          const std::vector<ObjectFactory>& ueMacFactories,
                          const std::vector<ObjectFactory>& uePhyFactories)
{
    NS_LOG_FUNCTION(this);
    Initialize(); // Run DoInitialize (), if necessary
    NetDeviceContainer devices;
    for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        Ptr<Node> node = *i;
        Ptr<NetDevice> device =
            InstallSingleUeDevice(node, allBwps, ueMacFactories, uePhyFactories);
        device->SetAddress(Mac48Address::Allocate());
        devices.Add(device);
    }
    return devices;
}

NetDeviceContainer
NrHelper::InstallGnbDevice(const NodeContainer& c,
                           const std::vector<std::reference_wrapper<BandwidthPartInfoPtr>> allBwps)
{
    NS_LOG_FUNCTION(this);
    NetDeviceContainer devices;
    for (NodeContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        Ptr<Node> node = *i;
        Ptr<NetDevice> device = InstallSingleGnbDevice(node, allBwps);
        device->SetAddress(Mac48Address::Allocate());
        devices.Add(device);
    }
    return devices;
}

void
NrHelper::UpdateDeviceConfigs(const NetDeviceContainer& netDevs)
{
    for (uint32_t i = 0; i < netDevs.GetN(); i++)
    {
        auto ueNetDev = DynamicCast<NrUeNetDevice>(netDevs.Get(i));
        auto gnbNetDev = DynamicCast<NrGnbNetDevice>(netDevs.Get(i));
        if (ueNetDev)
        {
            std::cerr << "Deprecation warning: UpdateConfig is no longer needed for UE device types"
                      << std::endl;
        }
        if (gnbNetDev)
        {
            std::cerr
                << "Deprecation warning: UpdateConfig is no longer needed for gNB device types"
                << std::endl;
        }
    }
}

Ptr<NrUePhy>
NrHelper::CreateUePhy(const Ptr<Node>& n,
                      const BandwidthPartInfoPtr& bwp,
                      const Ptr<NrUeNetDevice>& dev,
                      const NrSpectrumPhy::NrPhyDlHarqFeedbackCallback& dlHarqCallback,
                      const NrSpectrumPhy::NrPhyRxCtrlEndOkCallback& phyRxCtrlCallback)
{
    NS_LOG_FUNCTION(this);
    Ptr<NrUePhy> phy = m_uePhyFactory.Create<NrUePhy>();
    ConfigureUePhy(phy, n, bwp, dev, dlHarqCallback, phyRxCtrlCallback);
    return phy;
}

void
NrHelper::ConfigureUePhy(Ptr<NrUePhy> phy,
                         const Ptr<Node>& n,
                         const std::unique_ptr<BandwidthPartInfo>& bwp,
                         const Ptr<NrUeNetDevice>& dev,
                         const NrSpectrumPhy::NrPhyDlHarqFeedbackCallback& dlHarqCallback,
                         const NrSpectrumPhy::NrPhyRxCtrlEndOkCallback& phyRxCtrlCallback)
{
    NS_LOG_FUNCTION(this);

    NS_ASSERT(bwp->GetChannel() != nullptr);

    phy->InstallCentralFrequency(bwp->m_centralFrequency);

    phy->ScheduleStartEventLoop(n->GetId(), 0, 0, 0);

    // connect CAM and PHY
    Ptr<NrChAccessManager> cam =
        DynamicCast<NrChAccessManager>(m_ueChannelAccessManagerFactory.Create());
    phy->SetCam(cam);
    // set device
    phy->SetDevice(dev);
    // Set CSI feedback type to UE device
    phy->SetCsiFeedbackType(m_csiFeedbackFlags);

    Ptr<MobilityModel> mm = n->GetObject<MobilityModel>();
    NS_ASSERT_MSG(
        mm,
        "MobilityModel needs to be set on node before calling NrHelper::InstallUeDevice ()");

    Ptr<NrSpectrumPhy> channelPhy;
    if (phy->GetInstanceTypeId() == NrUePhy::GetTypeId())
    {
        channelPhy = m_ueSpectrumFactory.Create<NrSpectrumPhy>(); // Create NrSpectrumPhy
    }
    else
    {
        channelPhy = CreateObject<NrSlSpectrumPhy>();
    }

    channelPhy->SetPhyDlHarqFeedbackCallback(dlHarqCallback);
    channelPhy->SetIsGnb(false);
    channelPhy->SetDevice(dev); // each NrSpectrumPhy should have a pointer to device

    auto antenna = m_ueAntennaFactory.Create(); // Create antenna object
    channelPhy->SetAntenna(antenna);

    cam->SetNrSpectrumPhy(channelPhy); // connect CAM

    Ptr<NrChunkProcessor> pData = Create<NrChunkProcessor>();
    pData->AddCallback(MakeCallback(&NrSpectrumPhy::UpdateSinrPerceived, channelPhy));
    channelPhy->AddDataSinrChunkProcessor(pData);

    Ptr<NrMimoChunkProcessor> pDataMimo{nullptr};
    auto phasedChannel = bwp->GetChannel()->GetPhasedArraySpectrumPropagationLossModel();
    if (phasedChannel)
    {
        pDataMimo = Create<NrMimoChunkProcessor>();
        pDataMimo->AddCallback(MakeCallback(&NrSpectrumPhy::UpdateMimoSinrPerceived, channelPhy));
        channelPhy->AddDataMimoChunkProcessor(pDataMimo);

        if (m_csiFeedbackFlags & CQI_PDSCH_MIMO)
        {
            // Report DL CQI, PMI, RI (channel quality, MIMO precoding matrix and rank indicators)
            pDataMimo->AddCallback(MakeCallback(&NrUePhy::PdschMimoReceived, phy));
        }

        if (m_csiFeedbackFlags & CQI_CSI_RS)
        {
            Ptr<NrMimoChunkProcessor> pCsiRs = Create<NrMimoChunkProcessor>();
            pCsiRs->AddCallback(MakeCallback(&NrUePhy::CsiRsReceived, phy));
            channelPhy->AddCsiRsMimoChunkProcessor(pCsiRs);
            // currently, CSI_IM can be enabled only if CSI-RS is enabled
            if (m_csiFeedbackFlags & CQI_CSI_IM)
            {
                Ptr<NrMimoChunkProcessor> pCsiIm = Create<NrMimoChunkProcessor>();
                pCsiIm->AddCallback(MakeCallback(&NrUePhy::CsiImEnded, phy));
                channelPhy->AddCsiImMimoChunkProcessor(pCsiIm);
            }
        }
    }
    if (!phasedChannel || (m_csiFeedbackFlags == CQI_PDSCH_SISO))
    {
        // SISO CQI feedback
        pData->AddCallback(MakeCallback(&NrUePhy::GenerateDlCqiReport, phy));
    }

    Ptr<NrChunkProcessor> pRs = Create<NrChunkProcessor>();
    pRs->AddCallback(MakeCallback(&NrUePhy::ReportRsReceivedPower, phy));
    channelPhy->AddRsPowerChunkProcessor(pRs);

    Ptr<NrChunkProcessor> pSinr = Create<NrChunkProcessor>();
    pSinr->AddCallback(MakeCallback(&NrSpectrumPhy::ReportDlCtrlSinr, channelPhy));
    channelPhy->AddDlCtrlSinrChunkProcessor(pSinr);

    channelPhy->SetChannel(bwp->GetChannel());
    channelPhy->InstallPhy(phy);
    channelPhy->SetMobility(mm);
    channelPhy->SetPhyRxDataEndOkCallback(MakeCallback(&NrUePhy::PhyDataPacketReceived, phy));
    channelPhy->SetPhyRxCtrlEndOkCallback(phyRxCtrlCallback);
    channelPhy->SetPhyRxPssCallback(MakeCallback(&NrUePhy::ReceivePss, phy));
    // Check if the antenna is a uniform planar array type
    auto uniformPlanarArray = DynamicCast<UniformPlanarArray>(antenna);
    if (uniformPlanarArray)
    {
        Ptr<BeamManager> beamManager = m_ueBeamManagerFactory.Create<BeamManager>();
        beamManager->Configure(uniformPlanarArray);
        channelPhy->SetBeamManager(beamManager);
    }
    phy->InstallSpectrumPhy(channelPhy);
}

Ptr<NetDevice>
NrHelper::InstallSingleUeDevice(
    const Ptr<Node>& n,
    const std::vector<std::reference_wrapper<BandwidthPartInfoPtr>> allBwps,
    std::vector<ObjectFactory> ueMacFactories,
    std::vector<ObjectFactory> uePhyFactories)
{
    NS_LOG_FUNCTION(this);

    Ptr<NrUeNetDevice> dev = m_ueNetDeviceFactory.Create<NrUeNetDevice>();
    dev->SetNode(n);

    std::map<uint8_t, Ptr<BandwidthPartUe>> ueCcMap;

    // Create, for each ue, its bandwidth parts
    for (uint32_t bwpId = 0; bwpId < allBwps.size(); ++bwpId)
    {
        Ptr<BandwidthPartUe> cc = CreateObject<BandwidthPartUe>();
        double bwInKhz = allBwps[bwpId].get()->m_channelBandwidth / 1000.0;
        NS_ABORT_MSG_IF(bwInKhz / 100.0 > 65535.0,
                        "A bandwidth of " << bwInKhz / 100.0 << " kHz cannot be represented");
        cc->SetUlBandwidth(static_cast<uint16_t>(bwInKhz / 100));
        cc->SetDlBandwidth(static_cast<uint16_t>(bwInKhz / 100));
        cc->SetDlEarfcn(0); // Used for nothing..
        cc->SetUlEarfcn(0); // Used for nothing..

        if (!ueMacFactories.size())
        {
            cc->SetMac(m_ueMacFactory.Create<NrUeMac>());
        }
        else
        {
            NS_ABORT_MSG_UNLESS(
                ueMacFactories.size() == allBwps.size(),
                "Configuration error size of factory vector does not match size of allBwps");
            cc->SetMac(ueMacFactories[bwpId].Create<NrUeMac>());
        }
        Ptr<NrUePhy> phy;
        if (!uePhyFactories.size())
        {
            phy = CreateUePhy(
                n,
                allBwps[bwpId].get(),
                dev,
                MakeCallback(&NrUeNetDevice::EnqueueDlHarqFeedback, dev),
                std::bind(&NrUeNetDevice::RouteIngoingCtrlMsgs, dev, std::placeholders::_1, bwpId));
        }
        else
        {
            NS_ABORT_MSG_UNLESS(
                uePhyFactories.size() == allBwps.size(),
                "Configuration error size of factory vector does not match size of allBwps");
            phy = uePhyFactories[bwpId].Create<NrUePhy>();
            ConfigureUePhy(
                phy,
                n,
                allBwps[bwpId].get(),
                dev,
                MakeCallback(&NrUeNetDevice::EnqueueDlHarqFeedback, dev),
                std::bind(&NrUeNetDevice::RouteIngoingCtrlMsgs, dev, std::placeholders::_1, bwpId));
        }
        phy->SetBwpId(bwpId);
        cc->SetPhy(phy);

        if (bwpId == 0)
        {
            cc->SetAsPrimary(true);
        }
        else
        {
            cc->SetAsPrimary(false);
        }

        ueCcMap.insert(std::make_pair(bwpId, cc));
    }

    Ptr<NrUeComponentCarrierManager> ccmUe =
        DynamicCast<NrUeComponentCarrierManager>(m_bwpManagerFactory.Create<BwpManagerUe>());
    DynamicCast<BwpManagerUe>(ccmUe)->SetBwpManagerAlgorithm(
        m_ueBwpManagerAlgoFactory.Create<BwpManagerAlgorithm>());

    UintegerValue primaryUlIndex;
    dev->GetAttribute("PrimaryUlIndex", primaryUlIndex);
    NS_ASSERT_MSG(primaryUlIndex.Get() < ueCcMap.size(),
                  "UL primary index out of bounds. Configure PrimaryUlIndex attribute of "
                  "NrUeNetDevice correctly.");

    Ptr<NrUeRrc> rrc = m_ueRrcFactory.Create<NrUeRrc>();
    rrc->SetPrimaryUlIndex(primaryUlIndex.Get());
    rrc->m_numberOfComponentCarriers = ueCcMap.size();
    // run InitializeSap to create the proper number of sap provider/users
    rrc->InitializeSap();
    rrc->SetNrMacSapProvider(ccmUe->GetNrMacSapProvider());
    // setting ComponentCarrierManager SAP
    rrc->SetNrCcmRrcSapProvider(ccmUe->GetNrCcmRrcSapProvider());
    ccmUe->SetNrCcmRrcSapUser(rrc->GetNrCcmRrcSapUser());
    ccmUe->SetNumberOfComponentCarriers(ueCcMap.size());

    if (m_useIdealRrc)
    {
        Ptr<NrUeRrcProtocolIdeal> rrcProtocol = CreateObject<NrUeRrcProtocolIdeal>();
        rrcProtocol->SetUeRrc(rrc);
        rrc->AggregateObject(rrcProtocol);
        rrcProtocol->SetNrUeRrcSapProvider(rrc->GetNrUeRrcSapProvider());
        rrc->SetNrUeRrcSapUser(rrcProtocol->GetNrUeRrcSapUser());
    }
    else
    {
        Ptr<nr::UeRrcProtocolReal> rrcProtocol = CreateObject<nr::UeRrcProtocolReal>();
        rrcProtocol->SetUeRrc(rrc);
        rrc->AggregateObject(rrcProtocol);
        rrcProtocol->SetNrUeRrcSapProvider(rrc->GetNrUeRrcSapProvider());
        rrc->SetNrUeRrcSapUser(rrcProtocol->GetNrUeRrcSapUser());
    }

    if (m_nrEpcHelper != nullptr)
    {
        rrc->SetUseRlcSm(false);
    }
    else
    {
        rrc->SetUseRlcSm(true);
    }
    Ptr<NrEpcUeNas> nas = m_epcUeNasFactory.Create<NrEpcUeNas>();

    nas->SetAsSapProvider(rrc->GetAsSapProvider());
    auto nrSlUeRrc = rrc->GetObject<NrSlUeRrc>();
    auto nrSlEpcUeNas = nas->GetObject<NrSlEpcUeNas>();
    if (nrSlUeRrc && nrSlEpcUeNas)
    {
        nrSlEpcUeNas->SetNrSlAsSapProvider(nrSlUeRrc->GetNrSlAsSapProvider());
        nrSlUeRrc->SetNrSlAsSapUser(nrSlEpcUeNas->GetNrSlAsSapUser());
    }
    nas->SetDevice(dev);
    nas->SetForwardUpCallback(MakeCallback(&NrUeNetDevice::Receive, dev));

    rrc->SetAsSapUser(nas->GetAsSapUser());
    if (nrSlUeRrc && nrSlEpcUeNas)
    {
        nrSlUeRrc->SetNrSlAsSapUser(nrSlEpcUeNas->GetNrSlAsSapUser());
    }

    for (auto& it : ueCcMap)
    {
        rrc->SetNrUeCmacSapProvider(it.second->GetMac()->GetUeCmacSapProvider(), it.first);
        it.second->GetMac()->SetUeCmacSapUser(rrc->GetNrUeCmacSapUser(it.first));

        it.second->GetPhy()->SetUeCphySapUser(rrc->GetNrUeCphySapUser());
        rrc->SetNrUeCphySapProvider(it.second->GetPhy()->GetUeCphySapProvider(), it.first);

        it.second->GetPhy()->SetPhySapUser(it.second->GetMac()->GetPhySapUser());
        it.second->GetMac()->SetPhySapProvider(it.second->GetPhy()->GetPhySapProvider());

        bool ccmTest =
            ccmUe->SetComponentCarrierMacSapProviders(it.first,
                                                      it.second->GetMac()->GetUeMacSapProvider());

        if (!ccmTest)
        {
            NS_FATAL_ERROR("Error in SetComponentCarrierMacSapProviders");
        }
    }

    dev->SetCcMap(ueCcMap);
    dev->SetAttribute("nrUeRrc", PointerValue(rrc));
    dev->SetAttribute("NrEpcUeNas", PointerValue(nas));
    dev->SetAttribute("NrUeComponentCarrierManager", PointerValue(ccmUe));
    dev->SetAttribute("Imsi", UintegerValue(n->GetId()));

    n->AddDevice(dev);

    if (m_nrEpcHelper != nullptr)
    {
        m_nrEpcHelper->AddUe(dev, dev->GetImsi());
    }

    rrc->InitializeSrb0();
    return dev;
}

Ptr<NrGnbPhy>
NrHelper::CreateGnbPhy(const Ptr<Node>& n,
                       const BandwidthPartInfoPtr& bwp,
                       const Ptr<NrGnbNetDevice>& dev,
                       const NrSpectrumPhy::NrPhyRxCtrlEndOkCallback& phyEndCtrlCallback)
{
    NS_LOG_FUNCTION(this);

    Ptr<NrGnbPhy> phy = m_gnbPhyFactory.Create<NrGnbPhy>();

    DoubleValue frequency;
    phy->InstallCentralFrequency(bwp->m_centralFrequency);

    phy->ScheduleStartEventLoop(n->GetId(), 0, 0, 0);

    // PHY <--> CAM
    Ptr<NrChAccessManager> cam =
        DynamicCast<NrChAccessManager>(m_gnbChannelAccessManagerFactory.Create());
    phy->SetCam(cam);
    phy->SetDevice(dev);

    Ptr<MobilityModel> mm = n->GetObject<MobilityModel>();
    NS_ASSERT_MSG(
        mm,
        "MobilityModel needs to be set on node before calling NrHelper::InstallGnbDevice ()");

    Ptr<NrSpectrumPhy> channelPhy = m_gnbSpectrumFactory.Create<NrSpectrumPhy>();
    auto antenna = m_gnbAntennaFactory.Create();
    channelPhy->SetAntenna(antenna);
    cam->SetNrSpectrumPhy(channelPhy);

    channelPhy->SetIsGnb(true);
    channelPhy->SetDevice(dev); // each NrSpectrumPhy should have a pointer to device
    channelPhy->SetChannel(
        bwp->GetChannel()); // each NrSpectrumPhy needs to have a pointer to the SpectrumChannel
    // object of the corresponding spectrum part
    channelPhy->InstallPhy(phy); // each NrSpectrumPhy should have a pointer to its NrPhy

    Ptr<NrChunkProcessor> pData =
        Create<NrChunkProcessor>(); // create pData chunk processor per NrSpectrumPhy
    Ptr<NrChunkProcessor> pSrs =
        Create<NrChunkProcessor>(); // create pSrs per processor per NrSpectrumPhy
    auto phasedChannel = bwp->GetChannel()->GetPhasedArraySpectrumPropagationLossModel();
    if (!m_snrTest)
    {
        // TODO: rename to GeneratePuschCqiReport, replace when enabling uplink MIMO
        pData->AddCallback(MakeCallback(&NrGnbPhy::GenerateDataCqiReport,
                                        phy)); // connect DATA chunk processor that will
        // call GenerateDataCqiReport function
        pData->AddCallback(MakeCallback(&NrSpectrumPhy::UpdateSinrPerceived,
                                        channelPhy)); // connect DATA chunk processor that will
        // call UpdateSinrPerceived function
        pSrs->AddCallback(MakeCallback(&NrSpectrumPhy::UpdateSrsSinrPerceived,
                                       channelPhy)); // connect SRS chunk processor that will
                                                     // call UpdateSrsSinrPerceived function
        if (phasedChannel)
        {
            auto pDataMimo = Create<NrMimoChunkProcessor>();
            pDataMimo->AddCallback(
                MakeCallback(&NrSpectrumPhy::UpdateMimoSinrPerceived, channelPhy));
            channelPhy->AddDataMimoChunkProcessor(pDataMimo);
        }
    }
    channelPhy->AddDataSinrChunkProcessor(pData); // set DATA chunk processor to NrSpectrumPhy
    channelPhy->AddSrsSinrChunkProcessor(pSrs);   // set SRS chunk processor to NrSpectrumPhy
    channelPhy->SetMobility(mm);                  // set mobility model to this NrSpectrumPhy
    channelPhy->SetPhyRxDataEndOkCallback(
        MakeCallback(&NrGnbPhy::PhyDataPacketReceived, phy));  // connect PhyRxDataEndOk callback
    channelPhy->SetPhyRxCtrlEndOkCallback(phyEndCtrlCallback); // connect PhyRxCtrlEndOk
                                                               // callback
    channelPhy->SetPhyUlHarqFeedbackCallback(
        MakeCallback(&NrGnbPhy::ReportUlHarqFeedback, phy)); // PhyUlHarqFeedback callback
    // Check if the antenna is a uniform planar array type
    auto uniformPlanarArray = DynamicCast<UniformPlanarArray>(antenna);
    if (uniformPlanarArray)
    {
        Ptr<BeamManager> beamManager = m_gnbBeamManagerFactory.Create<BeamManager>();
        beamManager->Configure(uniformPlanarArray);
        channelPhy->SetBeamManager(beamManager);
    }
    phy->InstallSpectrumPhy(channelPhy); // finally let know phy that there is this spectrum phy
    if ((m_csiFeedbackFlags & CQI_CSI_RS) && phasedChannel)
    {
        phy->EnableCsiRs();
    }
    return phy;
}

Ptr<NrGnbMac>
NrHelper::CreateGnbMac()
{
    NS_LOG_FUNCTION(this);

    Ptr<NrGnbMac> mac = m_gnbMacFactory.Create<NrGnbMac>();
    return mac;
}

Ptr<NrMacScheduler>
NrHelper::CreateGnbSched()
{
    NS_LOG_FUNCTION(this);

    auto sched = m_schedFactory.Create<NrMacSchedulerNs3>();
    auto dlAmc = m_gnbDlAmcFactory.Create<NrAmc>();
    auto ulAmc = m_gnbUlAmcFactory.Create<NrAmc>();

    sched->InstallDlAmc(dlAmc);
    sched->InstallUlAmc(ulAmc);

    return sched;
}

Ptr<NrFhControl>
NrHelper::CreateNrFhControl()
{
    NS_LOG_FUNCTION(this);

    Ptr<NrFhControl> fhControl = m_fhControlFactory.Create<NrFhControl>();
    return fhControl;
}

Ptr<NetDevice>
NrHelper::InstallSingleGnbDevice(
    const Ptr<Node>& n,
    const std::vector<std::reference_wrapper<BandwidthPartInfoPtr>> allBwps)
{
    NS_ABORT_MSG_IF(m_cellIdCounter == 65535, "max num gNBs exceeded");

    Ptr<NrGnbNetDevice> dev = m_gnbNetDeviceFactory.Create<NrGnbNetDevice>();

    NS_LOG_DEBUG("Creating gNB, cellId = " << m_cellIdCounter);
    uint16_t cellId = m_cellIdCounter++; // New cellId
    dev->SetCellId(cellId);
    dev->SetNode(n);

    // create component carrier map for this gNB device
    std::map<uint8_t, Ptr<BandwidthPartGnb>> ccMap;

    auto fhControl = CreateNrFhControl();
    fhControl->SetPhysicalCellId(cellId);

    if (m_fhEnabled)
    {
        dev->SetNrFhControl(fhControl);
    }

    for (uint32_t bwpId = 0; bwpId < allBwps.size(); ++bwpId)
    {
        NS_LOG_DEBUG("Creating BandwidthPart, id = " << bwpId);
        Ptr<BandwidthPartGnb> cc = CreateObject<BandwidthPartGnb>();
        double bwInKhz = allBwps[bwpId].get()->m_channelBandwidth / 1000.0;
        NS_ABORT_MSG_IF(bwInKhz / 100.0 > 65535.0,
                        "A bandwidth of " << bwInKhz / 100.0 << " kHz cannot be represented");

        cc->SetUlBandwidth(static_cast<uint16_t>(bwInKhz / 100));
        cc->SetDlBandwidth(static_cast<uint16_t>(bwInKhz / 100));
        cc->SetDlEarfcn(0);              // Argh... handover not working
        cc->SetUlEarfcn(0);              // Argh... handover not working
        cc->SetCellId(cellId);           // All CCs have the same cellId
        cc->SetCsgId(m_cellIdCounter++); // CSG IDs starts matching cellId, then gets incremented

        auto phy = CreateGnbPhy(
            n,
            allBwps[bwpId].get(),
            dev,
            std::bind(&NrGnbNetDevice::RouteIngoingCtrlMsgs, dev, std::placeholders::_1, bwpId));
        phy->SetBwpId(bwpId);
        cc->SetPhy(phy);

        auto mac = CreateGnbMac();
        cc->SetMac(mac);
        phy->GetCam()->SetNrGnbMac(mac);

        auto sched = CreateGnbSched();
        cc->SetNrMacScheduler(sched);

        if (bwpId == 0)
        {
            cc->SetAsPrimary(true);
        }
        else
        {
            cc->SetAsPrimary(false);
        }

        ccMap.insert(std::make_pair(bwpId, cc));
    }

    Ptr<NrGnbRrc> rrc = CreateObject<NrGnbRrc>();
    Ptr<NrGnbComponentCarrierManager> ccmGnbManager =
        DynamicCast<NrGnbComponentCarrierManager>(CreateObject<BwpManagerGnb>());
    DynamicCast<BwpManagerGnb>(ccmGnbManager)
        ->SetBwpManagerAlgorithm(m_gnbBwpManagerAlgoFactory.Create<BwpManagerAlgorithm>());

    // Convert Gnb carrier map to only PhyConf map
    // we want to make RRC to be generic, to be able to work with any type of carriers, not only
    // strictly LTE carriers
    std::map<uint8_t, Ptr<BandwidthPartGnb>> ccPhyConfMap;
    for (const auto& i : ccMap)
    {
        Ptr<BandwidthPartGnb> c = i.second;
        ccPhyConfMap.insert(std::make_pair(i.first, c));
    }

    // ComponentCarrierManager SAP
    rrc->SetNrCcmRrcSapProvider(ccmGnbManager->GetNrCcmRrcSapProvider());
    ccmGnbManager->SetNrCcmRrcSapUser(rrc->GetNrCcmRrcSapUser());
    // Set number of component carriers. Note: gNB CCM would also set the
    // number of component carriers in gNB RRC

    ccmGnbManager->SetNumberOfComponentCarriers(ccMap.size());
    rrc->ConfigureCarriers(ccPhyConfMap);

    // nr module currently uses only RRC ideal mode
    if (m_useIdealRrc)
    {
        Ptr<NrGnbRrcProtocolIdeal> rrcProtocol = CreateObject<NrGnbRrcProtocolIdeal>();
        rrcProtocol->SetNrGnbRrcSapProvider(rrc->GetNrGnbRrcSapProvider());
        rrc->SetNrGnbRrcSapUser(rrcProtocol->GetNrGnbRrcSapUser());
        rrc->AggregateObject(rrcProtocol);
    }
    else
    {
        Ptr<nr::NrGnbRrcProtocolReal> rrcProtocol = CreateObject<nr::NrGnbRrcProtocolReal>();
        rrcProtocol->SetNrGnbRrcSapProvider(rrc->GetNrGnbRrcSapProvider());
        rrc->SetNrGnbRrcSapUser(rrcProtocol->GetNrGnbRrcSapUser());
        rrc->AggregateObject(rrcProtocol);
    }

    if (m_nrEpcHelper != nullptr)
    {
        EnumValue<NrGnbRrc::NrEpsBearerToRlcMapping_t> epsBearerToRlcMapping;
        rrc->GetAttribute("EpsBearerToRlcMapping", epsBearerToRlcMapping);
        // it does not make sense to use RLC/SM when also using the EPC
        if (epsBearerToRlcMapping.Get() == NrGnbRrc::RLC_SM_ALWAYS)
        {
            rrc->SetAttribute("EpsBearerToRlcMapping", EnumValue(NrGnbRrc::RLC_UM_ALWAYS));
        }
    }

    // This RRC attribute is used to connect each new RLC instance with the MAC layer
    // (for function such as TransmitPdu, ReportBufferStatusReport).
    // Since in this new architecture, the component carrier manager acts a proxy, it
    // will have its own NrMacSapProvider interface, RLC will see it as through original MAC
    // interface NrMacSapProvider, but the function call will go now through
    // NrGnbComponentCarrierManager instance that needs to implement functions of this interface,
    // and its task will be to forward these calls to the specific MAC of some of the instances of
    // component carriers. This decision will depend on the specific implementation of the component
    // carrier manager.
    rrc->SetNrMacSapProvider(ccmGnbManager->GetNrMacSapProvider());
    rrc->SetForwardUpCallback(MakeCallback(&NrGnbNetDevice::Receive, dev));

    for (auto& it : ccMap)
    {
        it.second->GetPhy()->SetGnbCphySapUser(rrc->GetNrGnbCphySapUser(it.first));
        rrc->SetNrGnbCphySapProvider(it.second->GetPhy()->GetGnbCphySapProvider(), it.first);

        rrc->SetNrGnbCmacSapProvider(it.second->GetMac()->GetGnbCmacSapProvider(), it.first);
        it.second->GetMac()->SetGnbCmacSapUser(rrc->GetNrGnbCmacSapUser(it.first));

        // PHY <--> MAC SAP
        it.second->GetPhy()->SetPhySapUser(it.second->GetMac()->GetPhySapUser());
        it.second->GetMac()->SetPhySapProvider(it.second->GetPhy()->GetPhySapProvider());
        // PHY <--> MAC SAP END

        // Scheduler SAP
        it.second->GetMac()->SetNrMacSchedSapProvider(
            it.second->GetScheduler()->GetMacSchedSapProvider());
        it.second->GetMac()->SetNrMacCschedSapProvider(
            it.second->GetScheduler()->GetMacCschedSapProvider());

        it.second->GetScheduler()->SetMacSchedSapUser(it.second->GetMac()->GetNrMacSchedSapUser());
        it.second->GetScheduler()->SetMacCschedSapUser(
            it.second->GetMac()->GetNrMacCschedSapUser());
        // Scheduler SAP END

        it.second->GetMac()->SetNrCcmMacSapUser(ccmGnbManager->GetNrCcmMacSapUser());
        ccmGnbManager->SetCcmMacSapProviders(it.first,
                                             it.second->GetMac()->GetNrCcmMacSapProvider());

        // insert the pointer to the NrMacSapProvider interface of the MAC layer of the specific
        // component carrier
        ccmGnbManager->SetMacSapProvider(it.first, it.second->GetMac()->GetMacSapProvider());

        // FH Control SAPs
        if (m_fhEnabled)
        {
            // Multiple sched/phy instances (as many as BWPs) - 1 NrFhControl instance (1 per cell)
            it.second->GetScheduler()->SetNrFhSchedSapProvider(
                dev->GetNrFhControl()->GetNrFhSchedSapProvider());
            dev->GetNrFhControl()->SetNrFhSchedSapUser(
                it.first,
                it.second->GetScheduler()->GetNrFhSchedSapUser());
            it.second->GetPhy()->SetNrFhPhySapProvider(
                dev->GetNrFhControl()->GetNrFhPhySapProvider());
            dev->GetNrFhControl()->SetNrFhPhySapUser(it.first,
                                                     it.second->GetPhy()->GetNrFhPhySapUser());
        }
    }

    dev->SetAttribute("NrGnbComponentCarrierManager", PointerValue(ccmGnbManager));
    dev->SetCcMap(ccMap);
    dev->SetAttribute("NrGnbRrc", PointerValue(rrc));

    n->AddDevice(dev);

    if (m_nrEpcHelper != nullptr)
    {
        NS_LOG_INFO("adding this gNB to the EPC");
        m_nrEpcHelper->AddGnb(n, dev, dev->GetCellIds());
        Ptr<NrEpcGnbApplication> gnbApp = n->GetApplication(0)->GetObject<NrEpcGnbApplication>();
        NS_ASSERT_MSG(gnbApp != nullptr, "cannot retrieve NrEpcGnbApplication");

        // S1 SAPs
        rrc->SetS1SapProvider(gnbApp->GetS1SapProvider());
        gnbApp->SetS1SapUser(rrc->GetS1SapUser());

        // X2 SAPs
        Ptr<NrEpcX2> x2 = n->GetObject<NrEpcX2>();
        x2->SetEpcX2SapUser(rrc->GetEpcX2SapUser());
        rrc->SetEpcX2SapProvider(x2->GetEpcX2SapProvider());
    }
    return dev;
}

std::string
NrHelper::GetHandoverAlgorithmType() const
{
    return m_handoverAlgorithmFactory.GetTypeId().GetName();
}

void
NrHelper::SetHandoverAlgorithmType(std::string type)
{
    NS_LOG_FUNCTION(this << type);
    m_handoverAlgorithmFactory = ObjectFactory();
    m_handoverAlgorithmFactory.SetTypeId(type);
}

void
NrHelper::SetHandoverAlgorithmAttribute(std::string n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this << n);
    m_handoverAlgorithmFactory.Set(n, v);
}

void
NrHelper::AddX2Interface(NodeContainer gnbNodes)
{
    NS_LOG_FUNCTION(this);

    NS_ASSERT_MSG(m_nrEpcHelper, "X2 interfaces cannot be set up when the EPC is not used");

    for (auto i = gnbNodes.Begin(); i != gnbNodes.End(); ++i)
    {
        for (auto j = i + 1; j != gnbNodes.End(); ++j)
        {
            AddX2Interface(*i, *j);
        }
    }
}

void
NrHelper::AddX2Interface(Ptr<Node> gnbNode1, Ptr<Node> gnbNode2)
{
    NS_LOG_FUNCTION(this);
    NS_LOG_INFO("setting up the X2 interface");

    m_nrEpcHelper->AddX2Interface(gnbNode1, gnbNode2);
}

void
NrHelper::HandoverRequest(Time hoTime,
                          Ptr<NetDevice> ueDev,
                          Ptr<NetDevice> sourceGnbDev,
                          Ptr<NetDevice> targetGnbDev)
{
    NS_LOG_FUNCTION(this << ueDev << sourceGnbDev << targetGnbDev);
    NS_ASSERT_MSG(m_nrEpcHelper,
                  "Handover requires the use of the EPC - did you forget to call "
                  "NrHelper::SetEpcHelper () ?");
    uint16_t targetCellId = targetGnbDev->GetObject<NrGnbNetDevice>()->GetCellId();
    Simulator::Schedule(hoTime,
                        &NrHelper::DoHandoverRequest,
                        this,
                        ueDev,
                        sourceGnbDev,
                        targetCellId);
}

void
NrHelper::HandoverRequest(Time hoTime,
                          Ptr<NetDevice> ueDev,
                          Ptr<NetDevice> sourceGnbDev,
                          uint16_t targetCellId)
{
    NS_LOG_FUNCTION(this << ueDev << sourceGnbDev << targetCellId);
    NS_ASSERT_MSG(m_nrEpcHelper,
                  "Handover requires the use of the EPC - did you forget to call "
                  "NrHelper::SetEpcHelper () ?");
    Simulator::Schedule(hoTime,
                        &NrHelper::DoHandoverRequest,
                        this,
                        ueDev,
                        sourceGnbDev,
                        targetCellId);
}

void
NrHelper::DoHandoverRequest(Ptr<NetDevice> ueDev,
                            Ptr<NetDevice> sourceGnbDev,
                            uint16_t targetCellId)
{
    NS_LOG_FUNCTION(this << ueDev << sourceGnbDev << targetCellId);

    Ptr<NrGnbRrc> sourceRrc = sourceGnbDev->GetObject<NrGnbNetDevice>()->GetRrc();
    uint16_t rnti = ueDev->GetObject<NrUeNetDevice>()->GetRrc()->GetRnti();
    sourceRrc->SendHandoverRequest(rnti, targetCellId);
}

void
NrHelper::AttachToMaxRsrpGnb(const NetDeviceContainer& ueDevices,
                             const NetDeviceContainer& enbDevices)
{
    NS_LOG_FUNCTION(this);

    for (auto i = ueDevices.Begin(); i != ueDevices.End(); i++)
    {
        AttachToMaxRsrpGnb(*i, enbDevices);
    }
}

void
NrHelper::AttachToMaxRsrpGnb(const Ptr<NetDevice>& ueDevice, const NetDeviceContainer& enbDevices)
{
    NS_LOG_FUNCTION(this);

    NS_ASSERT_MSG(enbDevices.GetN() > 0, "empty enb device container");

    auto nrInitAssoc = m_initialAttachmentFactory.Create<NrInitialAssociation>();
    ueDevice->GetObject<NrUeNetDevice>()->SetInitAssoc(nrInitAssoc);

    nrInitAssoc->SetUeDevice(ueDevice);
    nrInitAssoc->SetGnbDevices(enbDevices);
    nrInitAssoc->SetColBeamAngles(m_initialParams.colAngles);
    nrInitAssoc->SetRowBeamAngles(m_initialParams.rowAngles);
    nrInitAssoc->FindAssociatedGnb();
    NS_ASSERT_MSG(nrInitAssoc->CheckNumBeamsAllowed(),
                  "Number of gNB beams in this frequency is not supported");
    auto maxRsrpEnbDevice = nrInitAssoc->GetAssociatedGnb();
    NS_ASSERT(maxRsrpEnbDevice);

    AttachToGnb(ueDevice, maxRsrpEnbDevice);
}

void
NrHelper::AttachToClosestGnb(const NetDeviceContainer& ueDevices,
                             const NetDeviceContainer& gnbDevices)
{
    NS_LOG_FUNCTION(this);

    for (NetDeviceContainer::Iterator i = ueDevices.Begin(); i != ueDevices.End(); i++)
    {
        AttachToClosestGnb(*i, gnbDevices);
    }
}

void
NrHelper::AttachToClosestGnb(const Ptr<NetDevice>& ueDevice, const NetDeviceContainer& gnbDevices)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT_MSG(gnbDevices.GetN() > 0, "empty gnb device container");
    Vector uepos = ueDevice->GetNode()->GetObject<MobilityModel>()->GetPosition();
    double minDistance = std::numeric_limits<double>::infinity();
    Ptr<NetDevice> closestGnbDevice;
    for (NetDeviceContainer::Iterator i = gnbDevices.Begin(); i != gnbDevices.End(); ++i)
    {
        Vector gnbpos = (*i)->GetNode()->GetObject<MobilityModel>()->GetPosition();
        double distance = CalculateDistance(uepos, gnbpos);
        if (distance < minDistance)
        {
            minDistance = distance;
            closestGnbDevice = *i;
        }
    }
    NS_ASSERT(closestGnbDevice);

    AttachToGnb(ueDevice, closestGnbDevice);
}

void
NrHelper::AttachToGnb(const Ptr<NetDevice>& ueDevice, const Ptr<NetDevice>& gnbDevice)
{
    Ptr<NrGnbNetDevice> gnbNetDev = gnbDevice->GetObject<NrGnbNetDevice>();
    Ptr<NrUeNetDevice> ueNetDev = ueDevice->GetObject<NrUeNetDevice>();

    NS_ABORT_IF(gnbNetDev == nullptr || ueNetDev == nullptr);

    if (!gnbNetDev->IsCellConfigured())
    {
        gnbNetDev->ConfigureCell();
    }
    for (uint32_t i = 0; i < gnbNetDev->GetCcMapSize(); ++i)
    {
        gnbNetDev->GetPhy(i)->RegisterUe(ueNetDev->GetImsi(), ueNetDev);
        ueNetDev->GetPhy(i)->RegisterToGnb(gnbNetDev->GetBwpId(i));
        ueNetDev->GetPhy(i)->SetDlAmc(
            DynamicCast<NrMacSchedulerNs3>(gnbNetDev->GetScheduler(i))->GetDlAmc());
        ueNetDev->GetPhy(i)->SetDlCtrlSyms(gnbNetDev->GetMac(i)->GetDlCtrlSyms());
        ueNetDev->GetPhy(i)->SetUlCtrlSyms(gnbNetDev->GetMac(i)->GetUlCtrlSyms());
        ueNetDev->GetPhy(i)->SetNumRbPerRbg(gnbNetDev->GetMac(i)->GetNumRbPerRbg());
        ueNetDev->GetPhy(i)->SetRbOverhead(gnbNetDev->GetPhy(i)->GetRbOverhead());
        ueNetDev->GetPhy(i)->SetSymbolsPerSlot(gnbNetDev->GetPhy(i)->GetSymbolsPerSlot());
        ueNetDev->GetPhy(i)->SetNumerology(gnbNetDev->GetPhy(i)->GetNumerology());
        ueNetDev->GetPhy(i)->SetPattern(gnbNetDev->GetPhy(i)->GetPattern());
        Ptr<NrEpcUeNas> ueNas = ueNetDev->GetNas();
        ueNas->Connect(gnbNetDev->GetBwpId(i), gnbNetDev->GetEarfcn(i));

        if (m_enableMimoFeedback)
        {
            // Initialize parameters for MIMO precoding matrix search (PMI feedback)
            auto pmSearch = m_pmSearchFactory.Create<NrPmSearch>();
            ueNetDev->GetPhy(i)->SetPmSearch(pmSearch);
            auto gnbAnt =
                gnbNetDev->GetPhy(i)->GetSpectrumPhy()->GetAntenna()->GetObject<PhasedArrayModel>();
            auto ueAnt =
                ueNetDev->GetPhy(i)->GetSpectrumPhy()->GetAntenna()->GetObject<PhasedArrayModel>();
            pmSearch->SetGnbParams(gnbAnt->IsDualPol(),
                                   gnbAnt->GetNumHorizontalPorts(),
                                   gnbAnt->GetNumVerticalPorts());
            pmSearch->SetUeParams(ueAnt->GetNumPorts());
            pmSearch->InitCodebooks();
        }
    }

    if (m_nrEpcHelper)
    {
        // activate default EPS bearer
        m_nrEpcHelper->ActivateEpsBearer(ueDevice,
                                         ueNetDev->GetImsi(),
                                         NrEpcTft::Default(),
                                         NrEpsBearer(NrEpsBearer::NGBR_VIDEO_TCP_DEFAULT));
    }

    // tricks needed for the simplified LTE-only simulations
    // if (m_nrEpcHelper == 0)
    //{
    ueNetDev->SetTargetGnb(gnbNetDev);
    //}

    if (m_beamformingHelper)
    {
        m_beamformingHelper->AddBeamformingTask(gnbNetDev, ueNetDev);
    }
}

uint8_t
NrHelper::ActivateDedicatedEpsBearer(NetDeviceContainer ueDevices,
                                     NrEpsBearer bearer,
                                     Ptr<NrEpcTft> tft)
{
    NS_LOG_FUNCTION(this);
    for (NetDeviceContainer::Iterator i = ueDevices.Begin(); i != ueDevices.End(); ++i)
    {
        uint8_t bearerId = ActivateDedicatedEpsBearer(*i, bearer, tft);
        return bearerId;
    }
    return 0;
}

uint8_t
NrHelper::ActivateDedicatedEpsBearer(Ptr<NetDevice> ueDevice, NrEpsBearer bearer, Ptr<NrEpcTft> tft)
{
    NS_LOG_FUNCTION(this);

    NS_ASSERT_MSG(m_nrEpcHelper, "dedicated EPS bearers cannot be set up when the EPC is not used");

    uint64_t imsi = ueDevice->GetObject<NrUeNetDevice>()->GetImsi();
    uint8_t bearerId = m_nrEpcHelper->ActivateEpsBearer(ueDevice, imsi, tft, bearer);
    return bearerId;
}

void
NrHelper::DeActivateDedicatedEpsBearer(Ptr<NetDevice> ueDevice,
                                       Ptr<NetDevice> gnbDevice,
                                       uint8_t bearerId)
{
    NS_LOG_FUNCTION(this << ueDevice << bearerId);
    NS_ASSERT_MSG(m_nrEpcHelper != nullptr,
                  "Dedicated EPS bearers cannot be de-activated when the EPC is not used");
    NS_ASSERT_MSG(bearerId != 1,
                  "Default bearer cannot be de-activated until and unless and UE is released");

    DoDeActivateDedicatedEpsBearer(ueDevice, gnbDevice, bearerId);
}

void
NrHelper::SetEpcUeNasTypeId(TypeId tid)
{
    NS_LOG_FUNCTION(this << GetTypeId().GetName());
    m_epcUeNasFactory.SetTypeId(tid);
}

void
NrHelper::SetUeRrcTypeId(TypeId tid)
{
    NS_LOG_FUNCTION(this << GetTypeId().GetName());
    m_ueRrcFactory.SetTypeId(tid);
}

void
NrHelper::SetUePhyTypeId(TypeId tid)
{
    NS_LOG_FUNCTION(this << GetTypeId().GetName());
    m_uePhyFactory.SetTypeId(tid);
}

void
NrHelper::SetUeMacTypeId(TypeId tid)
{
    NS_LOG_FUNCTION(this << GetTypeId().GetName());
    m_ueMacFactory.SetTypeId(tid);
}

void
NrHelper::SetUeMacAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_ueMacFactory.Set(n, v);
}

void
NrHelper::SetGnbMacAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_gnbMacFactory.Set(n, v);
}

void
NrHelper::SetGnbSpectrumAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_gnbSpectrumFactory.Set(n, v);
}

void
NrHelper::SetUeSpectrumTypeId(TypeId tid)
{
    NS_LOG_FUNCTION(this << GetTypeId().GetName());
    m_ueSpectrumFactory.SetTypeId(tid);
}

void
NrHelper::SetUeSpectrumAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_ueSpectrumFactory.Set(n, v);
}

void
NrHelper::SetUeChannelAccessManagerAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_ueChannelAccessManagerFactory.Set(n, v);
}

void
NrHelper::SetGnbChannelAccessManagerAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_gnbChannelAccessManagerFactory.Set(n, v);
}

void
NrHelper::SetSchedulerAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_schedFactory.Set(n, v);
}

void
NrHelper::SetUePhyAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_uePhyFactory.Set(n, v);
}

void
NrHelper::SetGnbPhyAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_gnbPhyFactory.Set(n, v);
}

void
NrHelper::SetUeAntennaAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_ueAntennaFactory.Set(n, v);
}

void
NrHelper::SetGnbAntennaAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_gnbAntennaFactory.Set(n, v);
}

void
NrHelper::SetUeAntennaTypeId(const std::string& typeId)
{
    NS_LOG_FUNCTION(this);
    m_ueAntennaFactory.SetTypeId(typeId);
}

void
NrHelper::SetGnbAntennaTypeId(const std::string& typeId)
{
    NS_LOG_FUNCTION(this);
    m_gnbAntennaFactory.SetTypeId(typeId);
}

void
NrHelper::SetUeChannelAccessManagerTypeId(const TypeId& typeId)
{
    NS_LOG_FUNCTION(this);
    m_ueChannelAccessManagerFactory.SetTypeId(typeId);
}

void
NrHelper::SetGnbChannelAccessManagerTypeId(const TypeId& typeId)
{
    NS_LOG_FUNCTION(this);
    m_gnbChannelAccessManagerFactory.SetTypeId(typeId);
}

void
NrHelper::SetSchedulerTypeId(const TypeId& typeId)
{
    NS_LOG_FUNCTION(this);
    m_schedFactory.SetTypeId(typeId);
}

void
NrHelper::SetUeBwpManagerAlgorithmTypeId(const TypeId& typeId)
{
    NS_LOG_FUNCTION(this);
    m_ueBwpManagerAlgoFactory.SetTypeId(typeId);
}

void
NrHelper::SetUeBwpManagerAlgorithmAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_ueBwpManagerAlgoFactory.Set(n, v);
}

void
NrHelper::SetGnbDlAmcAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_gnbDlAmcFactory.Set(n, v);
}

void
NrHelper::SetGnbUlAmcAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_gnbUlAmcFactory.Set(n, v);
}

void
NrHelper::SetGnbBeamManagerAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_gnbBeamManagerFactory.Set(n, v);
}

void
NrHelper::SetGnbBeamManagerTypeId(const TypeId& typeId)
{
    NS_LOG_FUNCTION(this);
    m_gnbBeamManagerFactory.SetTypeId(typeId);
}

void
NrHelper::SetFhControlAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_fhControlFactory.Set(n, v);
}

void
NrHelper::SetUlErrorModel(const std::string& errorModelTypeId)
{
    NS_LOG_FUNCTION(this);

    SetGnbUlAmcAttribute("ErrorModelType", TypeIdValue(TypeId::LookupByName(errorModelTypeId)));
    SetGnbSpectrumAttribute("ErrorModelType", TypeIdValue(TypeId::LookupByName(errorModelTypeId)));
}

void
NrHelper::SetDlErrorModel(const std::string& errorModelTypeId)
{
    NS_LOG_FUNCTION(this);

    SetGnbDlAmcAttribute("ErrorModelType", TypeIdValue(TypeId::LookupByName(errorModelTypeId)));
    SetUeSpectrumAttribute("ErrorModelType", TypeIdValue(TypeId::LookupByName(errorModelTypeId)));
}

void
NrHelper::EnableFhControl()
{
    m_fhEnabled = true;
}

void
NrHelper::ConfigureFhControl(NetDeviceContainer gnbNetDevices)
{
    for (NetDeviceContainer::Iterator i = gnbNetDevices.Begin(); i != gnbNetDevices.End(); ++i)
    {
        Ptr<NrGnbNetDevice> gnbNetDev = DynamicCast<NrGnbNetDevice>(*i);

        for (uint32_t j = 0; j < gnbNetDev->GetCcMapSize(); j++)
        {
            gnbNetDev->GetNrFhControl()->SetFhNumerology(j, gnbNetDev->GetPhy(j)->GetNumerology());

            gnbNetDev->GetNrFhControl()->SetErrorModelType(
                DynamicCast<NrMacSchedulerNs3>(gnbNetDev->GetScheduler(j))
                    ->GetDlAmc()
                    ->GetErrorModelType()
                    .GetName());
        }
    }
}

int64_t
NrHelper::AssignStreams(NetDeviceContainer c, int64_t stream)
{
    int64_t currentStream = stream;
    Ptr<NetDevice> netDevice;
    for (NetDeviceContainer::Iterator i = c.Begin(); i != c.End(); ++i)
    {
        netDevice = (*i);
        Ptr<NrGnbNetDevice> nrGnb = DynamicCast<NrGnbNetDevice>(netDevice);
        if (nrGnb)
        {
            for (uint32_t bwp = 0; bwp < nrGnb->GetCcMapSize(); bwp++)
            {
                currentStream += nrGnb->GetPhy(bwp)->GetSpectrumPhy()->AssignStreams(currentStream);
                currentStream += nrGnb->GetScheduler(bwp)->AssignStreams(currentStream);
                currentStream +=
                    DoAssignStreamsToChannelObjects(nrGnb->GetPhy(bwp)->GetSpectrumPhy(),
                                                    currentStream);
            }
        }

        Ptr<NrUeNetDevice> nrUe = DynamicCast<NrUeNetDevice>(netDevice);
        if (nrUe)
        {
            for (uint32_t bwp = 0; bwp < nrUe->GetCcMapSize(); bwp++)
            {
                currentStream += nrUe->GetPhy(bwp)->GetSpectrumPhy()->AssignStreams(currentStream);
                currentStream += nrUe->GetMac(bwp)->AssignStreams(currentStream);
                currentStream +=
                    DoAssignStreamsToChannelObjects(nrUe->GetPhy(bwp)->GetSpectrumPhy(),
                                                    currentStream);
            }
        }
    }

    return (currentStream - stream);
}

int64_t
NrHelper::DoAssignStreamsToChannelObjects(Ptr<NrSpectrumPhy> phy, int64_t currentStream)
{
    int64_t initialStream = currentStream;

    Ptr<ThreeGppPropagationLossModel> propagationLossModel =
        DynamicCast<ThreeGppPropagationLossModel>(
            phy->GetSpectrumChannel()->GetPropagationLossModel());
    if (!propagationLossModel)
    {
        currentStream +=
            phy->GetSpectrumChannel()->GetPropagationLossModel()->AssignStreams(currentStream);
        return currentStream - initialStream;
    }

    if (std::find(m_channelObjectsWithAssignedStreams.begin(),
                  m_channelObjectsWithAssignedStreams.end(),
                  propagationLossModel) == m_channelObjectsWithAssignedStreams.end())
    {
        currentStream += propagationLossModel->AssignStreams(currentStream);
        m_channelObjectsWithAssignedStreams.emplace_back(propagationLossModel);
    }

    Ptr<ChannelConditionModel> channelConditionModel =
        propagationLossModel->GetChannelConditionModel();

    if (std::find(m_channelObjectsWithAssignedStreams.begin(),
                  m_channelObjectsWithAssignedStreams.end(),
                  channelConditionModel) == m_channelObjectsWithAssignedStreams.end())
    {
        currentStream += channelConditionModel->AssignStreams(currentStream);
        m_channelObjectsWithAssignedStreams.emplace_back(channelConditionModel);
    }

    Ptr<ThreeGppSpectrumPropagationLossModel> spectrumLossModel =
        DynamicCast<ThreeGppSpectrumPropagationLossModel>(
            phy->GetSpectrumChannel()->GetPhasedArraySpectrumPropagationLossModel());

    if (spectrumLossModel)
    {
        if (std::find(m_channelObjectsWithAssignedStreams.begin(),
                      m_channelObjectsWithAssignedStreams.end(),
                      spectrumLossModel) == m_channelObjectsWithAssignedStreams.end())
        {
            Ptr<ThreeGppChannelModel> channel =
                DynamicCast<ThreeGppChannelModel>(spectrumLossModel->GetChannelModel());
            currentStream += channel->AssignStreams(currentStream);
            m_channelObjectsWithAssignedStreams.emplace_back(spectrumLossModel);
        }
    }

    return currentStream - initialStream;
}

void
NrHelper::SetGnbBwpManagerAlgorithmTypeId(const TypeId& typeId)
{
    NS_LOG_FUNCTION(this);
    m_gnbBwpManagerAlgoFactory.SetTypeId(typeId);
}

void
NrHelper::SetGnbBwpManagerAlgorithmAttribute(const std::string& n, const AttributeValue& v)
{
    NS_LOG_FUNCTION(this);
    m_gnbBwpManagerAlgoFactory.Set(n, v);
}

void
NrHelper::DoDeActivateDedicatedEpsBearer(Ptr<NetDevice> ueDevice,
                                         Ptr<NetDevice> gnbDevice,
                                         uint8_t bearerId)
{
    NS_LOG_FUNCTION(this << ueDevice << bearerId);

    // Extract IMSI and rnti
    uint64_t imsi = ueDevice->GetObject<NrUeNetDevice>()->GetImsi();
    uint16_t rnti = ueDevice->GetObject<NrUeNetDevice>()->GetRrc()->GetRnti();

    Ptr<NrGnbRrc> gnbRrc = gnbDevice->GetObject<NrGnbNetDevice>()->GetRrc();

    gnbRrc->DoSendReleaseDataRadioBearer(imsi, rnti, bearerId);
}

void
NrHelper::SetEpcHelper(Ptr<NrEpcHelper> NrEpcHelper)
{
    m_nrEpcHelper = NrEpcHelper;
}

void
NrHelper::SetBeamformingHelper(Ptr<BeamformingHelperBase> beamformingHelper)
{
    m_beamformingHelper = beamformingHelper;
    m_beamformingHelper->Initialize();
}

class NrDrbActivator : public SimpleRefCount<NrDrbActivator>
{
  public:
    NrDrbActivator(Ptr<NetDevice> ueDevice, NrEpsBearer bearer);
    static void ActivateCallback(Ptr<NrDrbActivator> a,
                                 std::string context,
                                 uint64_t imsi,
                                 uint16_t cellId,
                                 uint16_t rnti);
    void ActivateDrb(uint64_t imsi, uint16_t cellId, uint16_t rnti);

  private:
    bool m_active;
    Ptr<NetDevice> m_ueDevice;
    NrEpsBearer m_bearer;
    uint64_t m_imsi;
};

NrDrbActivator::NrDrbActivator(Ptr<NetDevice> ueDevice, NrEpsBearer bearer)
    : m_active(false),
      m_ueDevice(ueDevice),
      m_bearer(bearer),
      m_imsi(m_ueDevice->GetObject<NrUeNetDevice>()->GetImsi())
{
}

void
NrDrbActivator::ActivateCallback(Ptr<NrDrbActivator> a,
                                 std::string context,
                                 uint64_t imsi,
                                 uint16_t cellId,
                                 uint16_t rnti)
{
    NS_LOG_FUNCTION(a << context << imsi << cellId << rnti);
    a->ActivateDrb(imsi, cellId, rnti);
}

void
NrDrbActivator::ActivateDrb(uint64_t imsi, uint16_t cellId, uint16_t rnti)
{
    NS_LOG_FUNCTION(this << imsi << cellId << rnti << m_active);
    if ((!m_active) && (imsi == m_imsi))
    {
        Ptr<NrUeRrc> ueRrc = m_ueDevice->GetObject<NrUeNetDevice>()->GetRrc();
        NS_ASSERT(ueRrc->GetState() == NrUeRrc::CONNECTED_NORMALLY);
        uint16_t rnti = ueRrc->GetRnti();
        Ptr<const NrGnbNetDevice> nrGnbDevice =
            m_ueDevice->GetObject<NrUeNetDevice>()->GetTargetGnb();
        Ptr<NrGnbRrc> gnbRrc = nrGnbDevice->GetObject<NrGnbNetDevice>()->GetRrc();
        NS_ASSERT(gnbRrc->HasCellId(ueRrc->GetCellId()));
        Ptr<NrUeManager> ueManager = gnbRrc->GetUeManager(rnti);
        NS_ASSERT(ueManager->GetState() == NrUeManager::CONNECTED_NORMALLY ||
                  ueManager->GetState() == NrUeManager::CONNECTION_RECONFIGURATION);
        NrEpcGnbS1SapUser::DataRadioBearerSetupRequestParameters params;
        params.rnti = rnti;
        params.bearer = m_bearer;
        params.bearerId = 0;
        params.gtpTeid = 0; // don't care
        gnbRrc->GetS1SapUser()->DataRadioBearerSetupRequest(params);
        m_active = true;
    }
}

void
NrHelper::ActivateDataRadioBearer(NetDeviceContainer ueDevices, NrEpsBearer bearer)
{
    NS_LOG_FUNCTION(this);
    for (NetDeviceContainer::Iterator i = ueDevices.Begin(); i != ueDevices.End(); ++i)
    {
        ActivateDataRadioBearer(*i, bearer);
    }
}

void
NrHelper::ActivateDataRadioBearer(Ptr<NetDevice> ueDevice, NrEpsBearer bearer)
{
    NS_LOG_FUNCTION(this << ueDevice);
    NS_ASSERT_MSG(!m_nrEpcHelper, "this method must not be used when the EPC is being used");

    // Normally it is the EPC that takes care of activating DRBs
    // when the UE gets connected. When the EPC is not used, we achieve
    // the same behavior by hooking a dedicated DRB activation function
    // to the Gnb RRC Connection Established trace source

    Ptr<const NrGnbNetDevice> nrGnbDevice = ueDevice->GetObject<NrUeNetDevice>()->GetTargetGnb();

    std::ostringstream path;
    path << "/NodeList/" << nrGnbDevice->GetNode()->GetId() << "/DeviceList/"
         << nrGnbDevice->GetIfIndex() << "/NrGnbRrc/ConnectionEstablished";
    Ptr<NrDrbActivator> arg = Create<NrDrbActivator>(ueDevice, bearer);
    Config::Connect(path.str(), MakeBoundCallback(&NrDrbActivator::ActivateCallback, arg));
}

void
NrHelper::EnableTraces()
{
    EnableDlDataPhyTraces();
    EnableDlCtrlPhyTraces();
    EnableUlPhyTraces();
    // EnableGnbPacketCountTrace ();
    // EnableUePacketCountTrace ();
    // EnableTransportBlockTrace ();
    EnableRlcSimpleTraces();
    EnableRlcE2eTraces();
    EnablePdcpSimpleTraces();
    EnablePdcpE2eTraces();
    EnableGnbPhyCtrlMsgsTraces();
    EnableUePhyCtrlMsgsTraces();
    EnableGnbMacCtrlMsgsTraces();
    EnableUeMacCtrlMsgsTraces();
    EnableDlMacSchedTraces();
    EnableUlMacSchedTraces();
    EnablePathlossTraces();
}

Ptr<NrPhyRxTrace>
NrHelper::GetPhyRxTrace()
{
    if (!m_phyStats)
    {
        m_phyStats = CreateObject<NrPhyRxTrace>();
    }
    return m_phyStats;
}

Ptr<NrMacRxTrace>
NrHelper::GetMacRxTrace()
{
    if (!m_macStats)
    {
        m_macStats = CreateObject<NrMacRxTrace>();
    }
    return m_macStats;
}

void
NrHelper::EnableDlDataPhyTraces()
{
    NS_LOG_FUNCTION(this);
    Config::Connect("/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUePhy/DlDataSinr",
                    MakeBoundCallback(&NrPhyRxTrace::DlDataSinrCallback, GetPhyRxTrace()));

    Config::Connect(
        "/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUePhy/SpectrumPhy/RxPacketTraceUe",
        MakeBoundCallback(&NrPhyRxTrace::RxPacketTraceUeCallback, GetPhyRxTrace()));
}

void
NrHelper::EnableDlCtrlPhyTraces()
{
    NS_LOG_FUNCTION(this);
    Config::Connect("/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUePhy/DlCtrlSinr",
                    MakeBoundCallback(&NrPhyRxTrace::DlCtrlSinrCallback, GetPhyRxTrace()));
}

void
NrHelper::EnableGnbPhyCtrlMsgsTraces()
{
    NS_LOG_FUNCTION(this);
    Config::Connect("/NodeList/*/DeviceList/*/BandwidthPartMap/*/NrGnbPhy/GnbPhyRxedCtrlMsgsTrace",
                    MakeBoundCallback(&NrPhyRxTrace::RxedGnbPhyCtrlMsgsCallback, GetPhyRxTrace()));
    Config::Connect("/NodeList/*/DeviceList/*/BandwidthPartMap/*/NrGnbPhy/GnbPhyTxedCtrlMsgsTrace",
                    MakeBoundCallback(&NrPhyRxTrace::TxedGnbPhyCtrlMsgsCallback, GetPhyRxTrace()));
}

void
NrHelper::EnableGnbMacCtrlMsgsTraces()
{
    NS_LOG_FUNCTION(this);
    Config::Connect("/NodeList/*/DeviceList/*/BandwidthPartMap/*/NrGnbMac/GnbMacRxedCtrlMsgsTrace",
                    MakeBoundCallback(&NrMacRxTrace::RxedGnbMacCtrlMsgsCallback, GetMacRxTrace()));
    Config::Connect("/NodeList/*/DeviceList/*/BandwidthPartMap/*/NrGnbMac/GnbMacTxedCtrlMsgsTrace",
                    MakeBoundCallback(&NrMacRxTrace::TxedGnbMacCtrlMsgsCallback, GetMacRxTrace()));
}

void
NrHelper::EnableUePhyCtrlMsgsTraces()
{
    NS_LOG_FUNCTION(this);
    Config::Connect(
        "/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUePhy/UePhyRxedCtrlMsgsTrace",
        MakeBoundCallback(&NrPhyRxTrace::RxedUePhyCtrlMsgsCallback, GetPhyRxTrace()));
    Config::Connect(
        "/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUePhy/UePhyTxedCtrlMsgsTrace",
        MakeBoundCallback(&NrPhyRxTrace::TxedUePhyCtrlMsgsCallback, GetPhyRxTrace()));
    Config::Connect("/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUePhy/UePhyRxedDlDciTrace",
                    MakeBoundCallback(&NrPhyRxTrace::RxedUePhyDlDciCallback, GetPhyRxTrace()));
    Config::Connect(
        "/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUePhy/UePhyTxedHarqFeedbackTrace",
        MakeBoundCallback(&NrPhyRxTrace::TxedUePhyHarqFeedbackCallback, GetPhyRxTrace()));
}

void
NrHelper::EnableUeMacCtrlMsgsTraces()
{
    NS_LOG_FUNCTION(this);
    Config::Connect(
        "/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUeMac/UeMacRxedCtrlMsgsTrace",
        MakeBoundCallback(&NrMacRxTrace::RxedUeMacCtrlMsgsCallback, GetMacRxTrace()));
    Config::Connect(
        "/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUeMac/UeMacTxedCtrlMsgsTrace",
        MakeBoundCallback(&NrMacRxTrace::TxedUeMacCtrlMsgsCallback, GetMacRxTrace()));
}

void
NrHelper::EnableUlPhyTraces()
{
    NS_LOG_FUNCTION(this);
    Config::Connect(
        "/NodeList/*/DeviceList/*/BandwidthPartMap/*/NrGnbPhy/SpectrumPhy/RxPacketTraceGnb",
        MakeBoundCallback(&NrPhyRxTrace::RxPacketTraceGnbCallback, GetPhyRxTrace()));
}

void
NrHelper::EnableGnbPacketCountTrace()
{
    NS_LOG_FUNCTION(this);
    Config::Connect(
        "/NodeList/*/DeviceList/*/BandwidthPartMap/*/NrGnbPhy/SpectrumPhy/ReportGnbTxRxPacketCount",
        MakeBoundCallback(&NrPhyRxTrace::ReportPacketCountGnbCallback, GetPhyRxTrace()));
}

void
NrHelper::EnableUePacketCountTrace()
{
    NS_LOG_FUNCTION(this);
    Config::Connect("/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUePhy/SpectrumPhy/"
                    "ReportUeTxRxPacketCount",
                    MakeBoundCallback(&NrPhyRxTrace::ReportPacketCountUeCallback, GetPhyRxTrace()));
}

void
NrHelper::EnableTransportBlockTrace()
{
    NS_LOG_FUNCTION(this);
    Config::Connect("/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUePhy/ReportDownlinkTbSize",
                    MakeBoundCallback(&NrPhyRxTrace::ReportDownLinkTBSize, GetPhyRxTrace()));
}

void
NrHelper::EnableRlcSimpleTraces()
{
    NS_LOG_FUNCTION(this);
    Ptr<NrBearerStatsSimple> rlcStats = CreateObject<NrBearerStatsSimple>("RLC");
    m_radioBearerStatsConnectorSimpleTraces.EnableRlcStats(rlcStats);
}

void
NrHelper::EnablePdcpSimpleTraces()
{
    NS_LOG_FUNCTION(this);
    Ptr<NrBearerStatsSimple> pdcpStats = CreateObject<NrBearerStatsSimple>("PDCP");
    m_radioBearerStatsConnectorSimpleTraces.EnablePdcpStats(pdcpStats);
}

void
NrHelper::EnableRlcE2eTraces()
{
    NS_LOG_FUNCTION(this);
    Ptr<NrBearerStatsCalculator> rlcStats = CreateObject<NrBearerStatsCalculator>("RLC");
    m_radioBearerStatsConnectorCalculator.EnableRlcStats(rlcStats);
}

void
NrHelper::EnablePdcpE2eTraces()
{
    NS_LOG_FUNCTION(this);
    Ptr<NrBearerStatsCalculator> pdcpStats = CreateObject<NrBearerStatsCalculator>("PDCP");
    m_radioBearerStatsConnectorCalculator.EnablePdcpStats(pdcpStats);
}

Ptr<NrBearerStatsCalculator>
NrHelper::GetRlcStatsCalculator()
{
    return DynamicCast<NrBearerStatsCalculator>(
        m_radioBearerStatsConnectorCalculator.GetRlcStats());
}

Ptr<NrBearerStatsCalculator>
NrHelper::GetPdcpStatsCalculator()
{
    return DynamicCast<NrBearerStatsCalculator>(
        m_radioBearerStatsConnectorCalculator.GetPdcpStats());
}

void
NrHelper::EnableDlMacSchedTraces()
{
    NS_LOG_FUNCTION(this);
    if (!m_macSchedStats)
    {
        m_macSchedStats = CreateObject<NrMacSchedulingStats>();
    }
    Config::Connect(
        "/NodeList/*/DeviceList/*/BandwidthPartMap/*/NrGnbMac/DlScheduling",
        MakeBoundCallback(&NrMacSchedulingStats::DlSchedulingCallback, m_macSchedStats));
}

void
NrHelper::EnableUlMacSchedTraces()
{
    NS_LOG_FUNCTION(this);
    if (!m_macSchedStats)
    {
        m_macSchedStats = CreateObject<NrMacSchedulingStats>();
    }
    Config::Connect(
        "/NodeList/*/DeviceList/*/BandwidthPartMap/*/NrGnbMac/UlScheduling",
        MakeBoundCallback(&NrMacSchedulingStats::UlSchedulingCallback, m_macSchedStats));
}

void
NrHelper::EnablePathlossTraces()
{
    NS_LOG_FUNCTION(this);
    Config::Connect("/ChannelList/*/$ns3::SpectrumChannel/PathLoss",
                    MakeBoundCallback(&NrPhyRxTrace::PathlossTraceCallback, GetPhyRxTrace()));
}

void
NrHelper::EnableDlCtrlPathlossTraces(NetDeviceContainer& ueDevs)
{
    NS_LOG_FUNCTION(this);

    for (uint32_t i = 0; i < ueDevs.GetN(); i++)
    {
        Ptr<NrUeNetDevice> ueDev = DynamicCast<NrUeNetDevice>(ueDevs.Get(i));
        NS_ASSERT_MSG(ueDev,
                      "To EnableDlCtrlPathlossTracesfunction is passed device "
                      "container that contains non UE devices.");
        for (uint32_t j = 0; j < ueDev->GetCcMapSize(); j++)
        {
            Ptr<NrUePhy> nrUePhy = ueDev->GetPhy(j);
            Ptr<NrSpectrumPhy> nrSpectrumPhy = nrUePhy->GetSpectrumPhy();
            nrSpectrumPhy->EnableDlCtrlPathlossTrace();
        }
    }

    Config::Connect("/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUePhy/NrSpectrumPhy/"
                    "DlCtrlPathloss",
                    MakeBoundCallback(&NrPhyRxTrace::ReportDlCtrlPathloss, GetPhyRxTrace()));
}

void
NrHelper::EnableDlDataPathlossTraces(NetDeviceContainer& ueDevs)
{
    NS_LOG_FUNCTION(this);

    NS_ASSERT_MSG(ueDevs.GetN(),
                  "Passed an empty UE net device container EnableDlDataPathlossTraces function");

    for (uint32_t i = 0; i < ueDevs.GetN(); i++)
    {
        Ptr<NrUeNetDevice> ueDev = DynamicCast<NrUeNetDevice>(ueDevs.Get(i));
        NS_ASSERT_MSG(ueDev,
                      "To EnableDlDataPathlossTracesfunction is passed device "
                      "container that contains non UE devices.");
        for (uint32_t j = 0; j < ueDev->GetCcMapSize(); j++)
        {
            Ptr<NrUePhy> nrUePhy = ueDev->GetPhy(j);
            Ptr<NrSpectrumPhy> nrSpectrumPhy = nrUePhy->GetSpectrumPhy();
            nrSpectrumPhy->EnableDlDataPathlossTrace();
        }
    }

    Config::Connect("/NodeList/*/DeviceList/*/ComponentCarrierMapUe/*/NrUePhy/NrSpectrumPhy/"
                    "DlDataPathloss",
                    MakeBoundCallback(&NrPhyRxTrace::ReportDlDataPathloss, GetPhyRxTrace()));
}

void
NrHelper::SetBwpManagerTypeId(const TypeId& typeId)
{
    NS_LOG_FUNCTION(this);
    m_bwpManagerFactory.SetTypeId(typeId);
}

void
NrHelper::SetPmSearchTypeId(const TypeId& typeId)
{
    m_pmSearchFactory.SetTypeId(typeId);
}

void
NrHelper::SetInitialAssocTypeId(const TypeId& typeId)
{
    m_initialAttachmentFactory.SetTypeId(typeId);
}

void
NrHelper::SetPmSearchAttribute(const std::string& name, const AttributeValue& value)
{
    NS_LOG_FUNCTION(this);
    m_pmSearchFactory.Set(name, value);
}

void
NrHelper::SetInitialAssocAttribute(const std::string& name, const AttributeValue& value)
{
    NS_LOG_FUNCTION(this);
    m_initialAttachmentFactory.Set(name, value);
}

void
NrHelper::SetupGnbAntennas(const NrHelper::AntennaParams& ap)
{
    NS_ASSERT_MSG(((ap.nAntCols % ap.nHorizPorts) == 0),
                  "The number of horizontal ports of gNB must divide number of element columns");
    NS_ASSERT_MSG(((ap.nAntRows % ap.nVertPorts) == 0),
                  "The number of vertical ports of gNB must divide number of element rows");

    auto antFactory = ObjectFactory{};
    antFactory.SetTypeId(ap.antennaElem);
    SetGnbAntennaAttribute("AntennaElement", PointerValue(antFactory.Create()));
    SetGnbAntennaAttribute("NumColumns", UintegerValue(ap.nAntCols));
    SetGnbAntennaAttribute("NumRows", UintegerValue(ap.nAntRows));
    SetGnbAntennaAttribute("IsDualPolarized", BooleanValue(ap.isDualPolarized));
    SetGnbAntennaAttribute("NumHorizontalPorts", UintegerValue(ap.nHorizPorts));
    SetGnbAntennaAttribute("NumVerticalPorts", UintegerValue(ap.nVertPorts));
    SetGnbAntennaAttribute("BearingAngle", DoubleValue(ap.bearingAngle));
    SetGnbAntennaAttribute("PolSlantAngle", DoubleValue(ap.polSlantAngle));
    SetGnbAntennaAttribute("DowntiltAngle", DoubleValue(ap.downtiltAngle));
}

void
NrHelper::SetupUeAntennas(const NrHelper::AntennaParams& ap)
{
    NS_ASSERT_MSG(((ap.nAntCols % ap.nHorizPorts) == 0),
                  "The number of horizontal ports of UE must divide number of element columns");
    NS_ASSERT_MSG(((ap.nAntRows % ap.nVertPorts) == 0),
                  "The number of vertical ports of UE must divide number of element rows");

    auto antFactory = ObjectFactory{};
    antFactory.SetTypeId(ap.antennaElem);
    SetUeAntennaAttribute("AntennaElement", PointerValue(antFactory.Create()));
    SetUeAntennaAttribute("NumColumns", UintegerValue(ap.nAntCols));
    SetUeAntennaAttribute("NumRows", UintegerValue(ap.nAntRows));
    SetUeAntennaAttribute("IsDualPolarized", BooleanValue(ap.isDualPolarized));
    SetUeAntennaAttribute("NumHorizontalPorts", UintegerValue(ap.nHorizPorts));
    SetUeAntennaAttribute("NumVerticalPorts", UintegerValue(ap.nVertPorts));
    SetUeAntennaAttribute("BearingAngle", DoubleValue(ap.bearingAngle));
    SetUeAntennaAttribute("PolSlantAngle", DoubleValue(ap.polSlantAngle));
    SetUeAntennaAttribute("DowntiltAngle", DoubleValue(ap.downtiltAngle));
}

void
NrHelper::SetupMimoPmi(const NrHelper::MimoPmiParams& mp)
{
    // Set parameters for MIMO precoding matrix search
    SetAttribute("EnableMimoFeedback", BooleanValue(true));
    auto searchTypeId = TypeId::LookupByName(mp.pmSearchMethod);
    SetPmSearchTypeId(searchTypeId);
    SetPmSearchAttribute("RankLimit", UintegerValue(mp.rankLimit));
    SetPmSearchAttribute("SubbandSize", UintegerValue(mp.subbandSize));
    SetPmSearchAttribute("DownsamplingTechnique", StringValue(mp.downsamplingTechnique));
    if (searchTypeId == NrPmSearchFull::GetTypeId())
    {
        SetPmSearchAttribute("CodebookType", TypeIdValue(TypeId::LookupByName(mp.fullSearchCb)));
    }
}

void
NrHelper::AddNrCsiRsFilter(Ptr<SpectrumChannel> channel)
{
    Ptr<const SpectrumTransmitFilter> p = channel->GetSpectrumTransmitFilter();
    bool found = false;
    while (p && !found)
    {
        if (DynamicCast<const NrCsiRsFilter>(p))
        {
            NS_LOG_DEBUG("Found existing NrCsiRsFilter for spectrum channel " << channel);
            found = true;
        }
        else
        {
            NS_LOG_DEBUG("Found different SpectrumTransmitFilter for channel " << channel);
            p = p->GetNext();
        }
    }
    if (!found)
    {
        Ptr<NrCsiRsFilter> pCsiRsFilter = CreateObject<NrCsiRsFilter>();
        channel->AddSpectrumTransmitFilter(pCsiRsFilter);
        NS_LOG_DEBUG("Adding NrCsiRsFilter to channel " << channel);
    }
}

void
NrHelper::SetupInitialAssoc(const NrHelper::InitialAssocParams& params)
{
    // Set parameters for Initial Association Params
    m_initialParams = params;
    SetInitialAssocAttribute("HandoffMargin", DoubleValue(params.handoffMargin));
    SetInitialAssocAttribute("PrimaryCarrierIndex", DoubleValue(params.primaryCarrierIndex));
}

} // namespace ns3

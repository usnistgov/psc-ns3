// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-rrc-protocol-ideal.h"

#include "nr-gnb-net-device.h"
#include "nr-gnb-rrc.h"
#include "nr-ue-net-device.h"
#include "nr-ue-rrc.h"

#include <ns3/fatal-error.h>
#include <ns3/log.h>
#include <ns3/node-list.h>
#include <ns3/node.h>
#include <ns3/nstime.h>
#include <ns3/simulator.h>

NS_LOG_COMPONENT_DEFINE("nrRrcProtocolIdeal");

namespace ns3
{

static const Time RRC_IDEAL_MSG_DELAY = MilliSeconds(0);

NS_OBJECT_ENSURE_REGISTERED(NrUeRrcProtocolIdeal);

NrUeRrcProtocolIdeal::NrUeRrcProtocolIdeal()
    : m_ueRrcSapProvider(nullptr),
      m_gnbRrcSapProvider(nullptr)
{
    m_ueRrcSapUser = new MemberNrUeRrcSapUser<NrUeRrcProtocolIdeal>(this);
}

NrUeRrcProtocolIdeal::~NrUeRrcProtocolIdeal()
{
}

void
NrUeRrcProtocolIdeal::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_ueRrcSapUser;
    m_rrc = nullptr;
}

TypeId
NrUeRrcProtocolIdeal::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrUeRrcProtocolIdeal")
                            .SetParent<Object>()
                            .AddConstructor<NrUeRrcProtocolIdeal>();
    return tid;
}

void
NrUeRrcProtocolIdeal::SetNrUeRrcSapProvider(NrUeRrcSapProvider* p)
{
    m_ueRrcSapProvider = p;
}

NrUeRrcSapUser*
NrUeRrcProtocolIdeal::GetNrUeRrcSapUser()
{
    return m_ueRrcSapUser;
}

void
NrUeRrcProtocolIdeal::SetUeRrc(Ptr<NrUeRrc> rrc)
{
    m_rrc = rrc;
}

void
NrUeRrcProtocolIdeal::DoSetup(NrUeRrcSapUser::SetupParameters params)
{
    NS_LOG_FUNCTION(this);
    // We don't care about SRB0/SRB1 since we use ideal RRC messages.
}

void
NrUeRrcProtocolIdeal::DoSendRrcConnectionRequest(NrRrcSap::RrcConnectionRequest msg)
{
    // initialize the RNTI and get the GnbNrRrcSapProvider for the
    // gNB we are currently attached to
    m_rnti = m_rrc->GetRnti();
    SetGnbRrcSapProvider();

    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                        &NrGnbRrcSapProvider::RecvRrcConnectionRequest,
                        m_gnbRrcSapProvider,
                        m_rnti,
                        msg);
}

void
NrUeRrcProtocolIdeal::DoSendRrcConnectionSetupCompleted(NrRrcSap::RrcConnectionSetupCompleted msg)
{
    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                        &NrGnbRrcSapProvider::RecvRrcConnectionSetupCompleted,
                        m_gnbRrcSapProvider,
                        m_rnti,
                        msg);
}

void
NrUeRrcProtocolIdeal::DoSendRrcConnectionReconfigurationCompleted(
    NrRrcSap::RrcConnectionReconfigurationCompleted msg)
{
    // re-initialize the RNTI and get the GnbNrRrcSapProvider for the
    // gNB we are currently attached to
    m_rnti = m_rrc->GetRnti();
    SetGnbRrcSapProvider();

    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                        &NrGnbRrcSapProvider::RecvRrcConnectionReconfigurationCompleted,
                        m_gnbRrcSapProvider,
                        m_rnti,
                        msg);
}

void
NrUeRrcProtocolIdeal::DoSendRrcConnectionReestablishmentRequest(
    NrRrcSap::RrcConnectionReestablishmentRequest msg)
{
    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                        &NrGnbRrcSapProvider::RecvRrcConnectionReestablishmentRequest,
                        m_gnbRrcSapProvider,
                        m_rnti,
                        msg);
}

void
NrUeRrcProtocolIdeal::DoSendRrcConnectionReestablishmentComplete(
    NrRrcSap::RrcConnectionReestablishmentComplete msg)
{
    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                        &NrGnbRrcSapProvider::RecvRrcConnectionReestablishmentComplete,
                        m_gnbRrcSapProvider,
                        m_rnti,
                        msg);
}

void
NrUeRrcProtocolIdeal::DoSendMeasurementReport(NrRrcSap::MeasurementReport msg)
{
    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                        &NrGnbRrcSapProvider::RecvMeasurementReport,
                        m_gnbRrcSapProvider,
                        m_rnti,
                        msg);
}

void
NrUeRrcProtocolIdeal::DoSendIdealUeContextRemoveRequest(uint16_t rnti)
{
    NS_FATAL_ERROR("NrUeRrcProtocolIdeal does not have RLF functionality yet");
}

void
NrUeRrcProtocolIdeal::SetGnbRrcSapProvider()
{
    uint16_t bwpId = m_rrc->GetCellId();

    // walk list of all nodes to get the peer gNB
    Ptr<NrGnbNetDevice> gnbDev;
    NodeList::Iterator listEnd = NodeList::End();
    bool found = false;
    for (NodeList::Iterator i = NodeList::Begin(); (i != listEnd) && (!found); ++i)
    {
        Ptr<Node> node = *i;
        int nDevs = node->GetNDevices();
        for (int j = 0; (j < nDevs) && (!found); j++)
        {
            gnbDev = node->GetDevice(j)->GetObject<NrGnbNetDevice>();
            if (gnbDev != nullptr)
            {
                for (uint32_t h = 0; h < gnbDev->GetCcMapSize(); ++h)
                {
                    if (gnbDev->GetBwpId(h) == bwpId)
                    {
                        found = true;
                        break;
                    }
                }
            }
        }
    }
    NS_ASSERT_MSG(found, " Unable to find gNB with BwpID =" << bwpId);
    m_gnbRrcSapProvider = gnbDev->GetRrc()->GetNrGnbRrcSapProvider();
    Ptr<NrGnbRrcProtocolIdeal> gnbRrcProtocolIdeal =
        gnbDev->GetRrc()->GetObject<NrGnbRrcProtocolIdeal>();
    gnbRrcProtocolIdeal->SetUeRrcSapProvider(m_rnti, m_ueRrcSapProvider);
}

NS_OBJECT_ENSURE_REGISTERED(NrGnbRrcProtocolIdeal);

NrGnbRrcProtocolIdeal::NrGnbRrcProtocolIdeal()
    : m_gnbRrcSapProvider(nullptr)
{
    NS_LOG_FUNCTION(this);
    m_gnbRrcSapUser = new MemberNrGnbRrcSapUser<NrGnbRrcProtocolIdeal>(this);
}

NrGnbRrcProtocolIdeal::~NrGnbRrcProtocolIdeal()
{
    NS_LOG_FUNCTION(this);
}

void
NrGnbRrcProtocolIdeal::DoDispose()
{
    NS_LOG_FUNCTION(this);
    delete m_gnbRrcSapUser;
}

TypeId
NrGnbRrcProtocolIdeal::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrGnbRrcProtocolIdeal")
                            .SetParent<Object>()
                            .AddConstructor<NrGnbRrcProtocolIdeal>();
    return tid;
}

void
NrGnbRrcProtocolIdeal::SetNrGnbRrcSapProvider(NrGnbRrcSapProvider* p)
{
    m_gnbRrcSapProvider = p;
}

NrGnbRrcSapUser*
NrGnbRrcProtocolIdeal::GetNrGnbRrcSapUser()
{
    return m_gnbRrcSapUser;
}

NrUeRrcSapProvider*
NrGnbRrcProtocolIdeal::GetUeRrcSapProvider(uint16_t rnti)
{
    std::map<uint16_t, NrUeRrcSapProvider*>::const_iterator it;
    it = m_gnbRrcSapProviderMap.find(rnti);
    NS_ASSERT_MSG(it != m_gnbRrcSapProviderMap.end(), "could not find RNTI = " << rnti);
    return it->second;
}

void
NrGnbRrcProtocolIdeal::SetUeRrcSapProvider(uint16_t rnti, NrUeRrcSapProvider* p)
{
    std::map<uint16_t, NrUeRrcSapProvider*>::iterator it;
    it = m_gnbRrcSapProviderMap.find(rnti);
    NS_ASSERT_MSG(it != m_gnbRrcSapProviderMap.end(), "could not find RNTI = " << rnti);
    it->second = p;
}

void
NrGnbRrcProtocolIdeal::DoSetupUe(uint16_t rnti, NrGnbRrcSapUser::SetupUeParameters params)
{
    NS_LOG_FUNCTION(this << rnti);
    m_gnbRrcSapProviderMap[rnti] = nullptr;
}

void
NrGnbRrcProtocolIdeal::DoRemoveUe(uint16_t rnti)
{
    NS_LOG_FUNCTION(this << rnti);
    m_gnbRrcSapProviderMap.erase(rnti);
}

void
NrGnbRrcProtocolIdeal::DoSendSystemInformation(uint16_t cellId, NrRrcSap::SystemInformation msg)
{
    NS_LOG_FUNCTION(this << cellId);
    // walk list of all nodes to get UEs with this cellId
    Ptr<NrUeRrc> ueRrc;
    for (NodeList::Iterator i = NodeList::Begin(); i != NodeList::End(); ++i)
    {
        Ptr<Node> node = *i;
        int nDevs = node->GetNDevices();
        for (int j = 0; j < nDevs; ++j)
        {
            Ptr<NrUeNetDevice> nrUeDev = node->GetDevice(j)->GetObject<NrUeNetDevice>();
            if (nrUeDev)
            {
                Ptr<NrUeRrc> ueRrc = nrUeDev->GetRrc();
                NS_LOG_LOGIC("considering UE IMSI " << nrUeDev->GetImsi() << " that has cellId "
                                                    << ueRrc->GetCellId());
                if (ueRrc->GetCellId() == cellId)
                {
                    NS_LOG_LOGIC("sending SI to IMSI " << nrUeDev->GetImsi());
                    ueRrc->GetNrUeRrcSapProvider()->RecvSystemInformation(msg);
                    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                                        &NrUeRrcSapProvider::RecvSystemInformation,
                                        ueRrc->GetNrUeRrcSapProvider(),
                                        msg);
                }
            }
        }
    }
}

void
NrGnbRrcProtocolIdeal::DoSendRrcConnectionSetup(uint16_t rnti, NrRrcSap::RrcConnectionSetup msg)
{
    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                        &NrUeRrcSapProvider::RecvRrcConnectionSetup,
                        GetUeRrcSapProvider(rnti),
                        msg);
}

void
NrGnbRrcProtocolIdeal::DoSendRrcConnectionReconfiguration(
    uint16_t rnti,
    NrRrcSap::RrcConnectionReconfiguration msg)
{
    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                        &NrUeRrcSapProvider::RecvRrcConnectionReconfiguration,
                        GetUeRrcSapProvider(rnti),
                        msg);
}

void
NrGnbRrcProtocolIdeal::DoSendRrcConnectionReestablishment(
    uint16_t rnti,
    NrRrcSap::RrcConnectionReestablishment msg)
{
    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                        &NrUeRrcSapProvider::RecvRrcConnectionReestablishment,
                        GetUeRrcSapProvider(rnti),
                        msg);
}

void
NrGnbRrcProtocolIdeal::DoSendRrcConnectionReestablishmentReject(
    uint16_t rnti,
    NrRrcSap::RrcConnectionReestablishmentReject msg)
{
    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                        &NrUeRrcSapProvider::RecvRrcConnectionReestablishmentReject,
                        GetUeRrcSapProvider(rnti),
                        msg);
}

void
NrGnbRrcProtocolIdeal::DoSendRrcConnectionRelease(uint16_t rnti, NrRrcSap::RrcConnectionRelease msg)
{
    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                        &NrUeRrcSapProvider::RecvRrcConnectionRelease,
                        GetUeRrcSapProvider(rnti),
                        msg);
}

void
NrGnbRrcProtocolIdeal::DoSendRrcConnectionReject(uint16_t rnti, NrRrcSap::RrcConnectionReject msg)
{
    Simulator::Schedule(RRC_IDEAL_MSG_DELAY,
                        &NrUeRrcSapProvider::RecvRrcConnectionReject,
                        GetUeRrcSapProvider(rnti),
                        msg);
}

/*
 * The purpose of NrGnbRrcProtocolIdeal is to avoid encoding
 * messages. In order to do so, we need to have some form of encoding for
 * inter-node RRC messages like HandoverPreparationInfo and HandoverCommand. Doing so
 * directly is not practical (these messages includes a lot of
 * information elements, so encoding all of them would defeat the
 * purpose of NrGnbRrcProtocolIdeal. The workaround is to store the
 * actual message in a global map, so that then we can just encode the
 * key in a header and send that between gNBs over X2.
 *
 */

static std::map<uint32_t, NrRrcSap::HandoverPreparationInfo> g_handoverPreparationInfoMsgMap;
static uint32_t g_handoverPreparationInfoMsgIdCounter = 0;

/*
 * This header encodes the map key discussed above. We keep this
 * private since it should not be used outside this file.
 *
 */
class NrIdealHandoverPreparationInfoHeader : public Header
{
  public:
    uint32_t GetMsgId() const;
    void SetMsgId(uint32_t id);
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    void Print(std::ostream& os) const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;

  private:
    uint32_t m_msgId;
};

uint32_t
NrIdealHandoverPreparationInfoHeader::GetMsgId() const
{
    return m_msgId;
}

void
NrIdealHandoverPreparationInfoHeader::SetMsgId(uint32_t id)
{
    m_msgId = id;
}

TypeId
NrIdealHandoverPreparationInfoHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrIdealHandoverPreparationInfoHeader")
                            .SetParent<Header>()
                            .AddConstructor<NrIdealHandoverPreparationInfoHeader>();
    return tid;
}

TypeId
NrIdealHandoverPreparationInfoHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
NrIdealHandoverPreparationInfoHeader::Print(std::ostream& os) const
{
    os << " msgId=" << m_msgId;
}

uint32_t
NrIdealHandoverPreparationInfoHeader::GetSerializedSize() const
{
    return 4;
}

void
NrIdealHandoverPreparationInfoHeader::Serialize(Buffer::Iterator start) const
{
    start.WriteU32(m_msgId);
}

uint32_t
NrIdealHandoverPreparationInfoHeader::Deserialize(Buffer::Iterator start)
{
    m_msgId = start.ReadU32();
    return GetSerializedSize();
}

Ptr<Packet>
NrGnbRrcProtocolIdeal::DoEncodeHandoverPreparationInformation(NrRrcSap::HandoverPreparationInfo msg)
{
    uint32_t msgId = ++g_handoverPreparationInfoMsgIdCounter;
    NS_ASSERT_MSG(g_handoverPreparationInfoMsgMap.find(msgId) ==
                      g_handoverPreparationInfoMsgMap.end(),
                  "msgId " << msgId << " already in use");
    NS_LOG_INFO(" encoding msgId = " << msgId);
    g_handoverPreparationInfoMsgMap.insert(
        std::pair<uint32_t, NrRrcSap::HandoverPreparationInfo>(msgId, msg));
    NrIdealHandoverPreparationInfoHeader h;
    h.SetMsgId(msgId);
    Ptr<Packet> p = Create<Packet>();
    p->AddHeader(h);
    return p;
}

NrRrcSap::HandoverPreparationInfo
NrGnbRrcProtocolIdeal::DoDecodeHandoverPreparationInformation(Ptr<Packet> p)
{
    NrIdealHandoverPreparationInfoHeader h;
    p->RemoveHeader(h);
    uint32_t msgId = h.GetMsgId();
    NS_LOG_INFO(" decoding msgId = " << msgId);
    std::map<uint32_t, NrRrcSap::HandoverPreparationInfo>::iterator it =
        g_handoverPreparationInfoMsgMap.find(msgId);
    NS_ASSERT_MSG(it != g_handoverPreparationInfoMsgMap.end(), "msgId " << msgId << " not found");
    NrRrcSap::HandoverPreparationInfo msg = it->second;
    g_handoverPreparationInfoMsgMap.erase(it);
    return msg;
}

static std::map<uint32_t, NrRrcSap::RrcConnectionReconfiguration> g_handoverCommandMsgMap;
static uint32_t g_handoverCommandMsgIdCounter = 0;

/*
 * This header encodes the map key discussed above. We keep this
 * private since it should not be used outside this file.
 *
 */
class NrIdealHandoverCommandHeader : public Header
{
  public:
    uint32_t GetMsgId() const;
    void SetMsgId(uint32_t id);
    static TypeId GetTypeId();
    TypeId GetInstanceTypeId() const override;
    void Print(std::ostream& os) const override;
    uint32_t GetSerializedSize() const override;
    void Serialize(Buffer::Iterator start) const override;
    uint32_t Deserialize(Buffer::Iterator start) override;

  private:
    uint32_t m_msgId;
};

uint32_t
NrIdealHandoverCommandHeader::GetMsgId() const
{
    return m_msgId;
}

void
NrIdealHandoverCommandHeader::SetMsgId(uint32_t id)
{
    m_msgId = id;
}

TypeId
NrIdealHandoverCommandHeader::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NrIdealHandoverCommandHeader")
                            .SetParent<Header>()
                            .AddConstructor<NrIdealHandoverCommandHeader>();
    return tid;
}

TypeId
NrIdealHandoverCommandHeader::GetInstanceTypeId() const
{
    return GetTypeId();
}

void
NrIdealHandoverCommandHeader::Print(std::ostream& os) const
{
    os << " msgId=" << m_msgId;
}

uint32_t
NrIdealHandoverCommandHeader::GetSerializedSize() const
{
    return 4;
}

void
NrIdealHandoverCommandHeader::Serialize(Buffer::Iterator start) const
{
    start.WriteU32(m_msgId);
}

uint32_t
NrIdealHandoverCommandHeader::Deserialize(Buffer::Iterator start)
{
    m_msgId = start.ReadU32();
    return GetSerializedSize();
}

Ptr<Packet>
NrGnbRrcProtocolIdeal::DoEncodeHandoverCommand(NrRrcSap::RrcConnectionReconfiguration msg)
{
    uint32_t msgId = ++g_handoverCommandMsgIdCounter;
    NS_ASSERT_MSG(g_handoverCommandMsgMap.find(msgId) == g_handoverCommandMsgMap.end(),
                  "msgId " << msgId << " already in use");
    NS_LOG_INFO(" encoding msgId = " << msgId);
    g_handoverCommandMsgMap.insert(
        std::pair<uint32_t, NrRrcSap::RrcConnectionReconfiguration>(msgId, msg));
    NrIdealHandoverCommandHeader h;
    h.SetMsgId(msgId);
    Ptr<Packet> p = Create<Packet>();
    p->AddHeader(h);
    return p;
}

NrRrcSap::RrcConnectionReconfiguration
NrGnbRrcProtocolIdeal::DoDecodeHandoverCommand(Ptr<Packet> p)
{
    NrIdealHandoverCommandHeader h;
    p->RemoveHeader(h);
    uint32_t msgId = h.GetMsgId();
    NS_LOG_INFO(" decoding msgId = " << msgId);
    std::map<uint32_t, NrRrcSap::RrcConnectionReconfiguration>::iterator it =
        g_handoverCommandMsgMap.find(msgId);
    NS_ASSERT_MSG(it != g_handoverCommandMsgMap.end(), "msgId " << msgId << " not found");
    NrRrcSap::RrcConnectionReconfiguration msg = it->second;
    g_handoverCommandMsgMap.erase(it);
    return msg;
}

} // namespace ns3

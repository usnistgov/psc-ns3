// Copyright (c) 2023 New York University and NYU WIRELESS
// Users are encouraged to cite NYU WIRELESS publications regarding this work.
// Original source code is available in https://github.com/hiteshPoddar/NYUSIM_in_ns3
//
// SPDX-License-Identifier: MIT

#include "nyu-channel-condition-model.h"

#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/log.h"
#include "ns3/mobility-model.h"
#include "ns3/node.h"
#include "ns3/simulator.h"
#include "ns3/string.h"

#include <cmath>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("NYUChannelConditionModel");

NS_OBJECT_ENSURE_REGISTERED(NYUChannelConditionModel);

TypeId
NYUChannelConditionModel::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::NYUChannelConditionModel")
            .SetParent<ChannelConditionModel>()
            .SetGroupName("Propagation")
            .AddAttribute("UpdatePeriod",
                          "Specifies the time period after which the channel condition is "
                          "recomputed. If set to 0, the channel condition is never updated.",
                          TimeValue(MilliSeconds(0)),
                          MakeTimeAccessor(&NYUChannelConditionModel::m_updatePeriod),
                          MakeTimeChecker());
    return tid;
}

NYUChannelConditionModel::NYUChannelConditionModel()
    : ChannelConditionModel()
{
    m_uniformVar = CreateObject<UniformRandomVariable>();
    m_uniformVar->SetAttribute("Min", DoubleValue(0));
    m_uniformVar->SetAttribute("Max", DoubleValue(1));
}

NYUChannelConditionModel::~NYUChannelConditionModel()
{
}

void
NYUChannelConditionModel::DoDispose()
{
    m_channelConditionMap.clear();
    m_updatePeriod = Seconds(0.0);
}

Ptr<ChannelCondition>
NYUChannelConditionModel::GetChannelCondition(Ptr<const MobilityModel> a,
                                              Ptr<const MobilityModel> b) const
{
    Ptr<ChannelCondition> cond;

    // get the key for this channel
    uint32_t key = GetKey(a, b);

    bool notFound = false; // indicates if the channel condition is not present in the map
    bool update = false;   // indicates if the channel condition has to be updated

    // look for the channel condition in m_channelConditionMap
    auto mapItem = m_channelConditionMap.find(key);
    if (mapItem != m_channelConditionMap.end())
    {
        NS_LOG_DEBUG("found the channel condition in the map");
        cond = mapItem->second.m_condition;

        // check if it has to be updated
        if (!m_updatePeriod.IsZero() &&
            Simulator::Now() - mapItem->second.m_generatedTime > m_updatePeriod)
        {
            NS_LOG_DEBUG("it has to be updated");
            update = true;
        }
    }
    else
    {
        NS_LOG_DEBUG("channel condition not found");
        notFound = true;
    }
    // if the channel condition was not found or if it has to be updated
    // generate a new channel condition
    if (notFound || update)
    {
        cond = ComputeChannelCondition(a, b);
        // store the channel condition in m_channelConditionMap, used as cache.
        // For this reason you see a const_cast.
        Item mapItem;
        mapItem.m_condition = cond;
        mapItem.m_generatedTime = Simulator::Now();
        const_cast<NYUChannelConditionModel*>(this)->m_channelConditionMap[key] = mapItem;
    }
    return cond;
}

Ptr<ChannelCondition>
NYUChannelConditionModel::ComputeChannelCondition(Ptr<const MobilityModel> a,
                                                  Ptr<const MobilityModel> b) const
{
    NS_LOG_FUNCTION(this << a << b);
    Ptr<ChannelCondition> cond = CreateObject<ChannelCondition>();

    // compute the LOS probability
    double pLos = ComputePlos(a, b);

    // draw a random value
    double pRef = m_uniformVar->GetValue();

    NS_LOG_DEBUG("pRef " << pRef << " pLos " << pLos);

    // get the channel condition
    if (pRef <= pLos)
    {
        // LOS
        cond->SetLosCondition(ChannelCondition::LosConditionValue::LOS);
    }
    else
    {
        // NLOS
        cond->SetLosCondition(ChannelCondition::LosConditionValue::NLOS);
    }
    return cond;
}

int64_t
NYUChannelConditionModel::AssignStreams(int64_t stream)
{
    m_uniformVar->SetStream(stream);
    return 1;
}

double
NYUChannelConditionModel::Calculate2dDistance(const Vector& a, const Vector& b)
{
    double x = a.x - b.x;
    double y = a.y - b.y;
    double distance2D = sqrt(x * x + y * y);
    return distance2D;
}

uint32_t
NYUChannelConditionModel::GetKey(Ptr<const MobilityModel> a, Ptr<const MobilityModel> b)
{
    // use the nodes ids to obtain a unique key for the channel between a and b
    // sort the nodes ids so that the key is reciprocal
    uint32_t x1 = std::min(a->GetObject<Node>()->GetId(), b->GetObject<Node>()->GetId());
    uint32_t x2 = std::max(a->GetObject<Node>()->GetId(), b->GetObject<Node>()->GetId());

    // use the cantor function to obtain the key
    uint32_t key = (((x1 + x2) * (x1 + x2 + 1)) / 2) + x2;
    return key;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED(NYURmaChannelConditionModel);

TypeId
NYURmaChannelConditionModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NYURmaChannelConditionModel")
                            .SetParent<NYUChannelConditionModel>()
                            .SetGroupName("Propagation")
                            .AddConstructor<NYURmaChannelConditionModel>();
    return tid;
}

NYURmaChannelConditionModel::NYURmaChannelConditionModel()
    : NYUChannelConditionModel()
{
}

NYURmaChannelConditionModel::~NYURmaChannelConditionModel()
{
}

double
NYURmaChannelConditionModel::ComputePlos(Ptr<const MobilityModel> a,
                                         Ptr<const MobilityModel> b) const
{
    // NYU Channel model doesn't have a PLOS for RMa, thus using 3GPP Channel Model.
    // compute the 2D distance between a and b
    double distance2D = Calculate2dDistance(a->GetPosition(), b->GetPosition());

    // NOTE: no indication is given about the heights of the BS and the UT used
    // to derive the LOS probability

    // compute the LOS probability (see 3GPP TR 38.901, Sec. 7.4.2)
    double pLos = 0.0;
    if (distance2D <= 10.0)
    {
        pLos = 1.0;
    }
    else
    {
        pLos = exp(-(distance2D - 10.0) / 1000.0);
    }
    return pLos;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED(NYUUmaChannelConditionModel);

TypeId
NYUUmaChannelConditionModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NYUUmaChannelConditionModel")
                            .SetParent<NYUChannelConditionModel>()
                            .SetGroupName("Propagation")
                            .AddConstructor<NYUUmaChannelConditionModel>();
    return tid;
}

NYUUmaChannelConditionModel::NYUUmaChannelConditionModel()
    : NYUChannelConditionModel()
{
}

NYUUmaChannelConditionModel::~NYUUmaChannelConditionModel()
{
}

double
NYUUmaChannelConditionModel::ComputePlos(Ptr<const MobilityModel> a,
                                         Ptr<const MobilityModel> b) const
{
    // https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7999294 (table II, row 2)
    // compute the 2D distance between a and b
    double distance2D = Calculate2dDistance(a->GetPosition(), b->GetPosition());

    // retrieve h_UT, it should be smaller than 23 m
    double h_UT = std::min(a->GetPosition().z, b->GetPosition().z);
    if (h_UT > 23.0)
    {
        NS_LOG_WARN(
            "The height of the UT should be smaller than 23 m (see TR 38.901, Table 7.4.2-1)");
    }

    // NOTE: no indication is given about the UT height used to derive the
    // LOS probability compute the LOS probability

    double pLos = 0.0;
    if (distance2D <= 20.0)
    {
        pLos = 1.0;
    }
    else
    {
        // compute C'(h_UT)
        double c = 0.0;
        double g_2d = 0.0;
        if (h_UT <= 13.0)
        {
            c = 0;
        }
        else
        {
            g_2d = (1.25e1 - 6) * pow(distance2D, 3) * exp(-distance2D / 150);
            c = pow((h_UT - 13.0) / 10.0, 1.5) * g_2d;
        }
        pLos = pow(((20.0 / distance2D) * (1 - exp(-distance2D / 160)) + exp(-distance2D / 160)) *
                       (1 + c),
                   2);
    }
    return pLos;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED(NYUUmiChannelConditionModel);

TypeId
NYUUmiChannelConditionModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NYUUmiChannelConditionModel")
                            .SetParent<NYUChannelConditionModel>()
                            .SetGroupName("Propagation")
                            .AddConstructor<NYUUmiChannelConditionModel>();
    return tid;
}

NYUUmiChannelConditionModel::NYUUmiChannelConditionModel()
    : NYUChannelConditionModel()
{
}

NYUUmiChannelConditionModel::~NYUUmiChannelConditionModel()
{
}

double
NYUUmiChannelConditionModel::ComputePlos(Ptr<const MobilityModel> a,
                                         Ptr<const MobilityModel> b) const
{
    // compute the 2D distance between a and b
    double distance2D = Calculate2dDistance(a->GetPosition(), b->GetPosition());

    // NOTE: no indication is given about the UT height used to derive the
    // LOS probability compute the LOS probability
    // NYU Squared Model : https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7999294 (Table I,
    // Row 2)
    double pLos = 0.0;
    if (distance2D <= 22.0)
    {
        pLos = 1.0;
    }
    else
    {
        pLos = pow((22.0 / distance2D) * (1.0 - 22.0 / distance2D) + exp(-distance2D / 100.0), 2);
    }
    return pLos;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED(NYUInHChannelConditionModel);

TypeId
NYUInHChannelConditionModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NYUInHChannelConditionModel")
                            .SetParent<NYUChannelConditionModel>()
                            .SetGroupName("Propagation")
                            .AddConstructor<NYUInHChannelConditionModel>();
    return tid;
}

NYUInHChannelConditionModel::NYUInHChannelConditionModel()
    : NYUChannelConditionModel()
{
}

NYUInHChannelConditionModel::~NYUInHChannelConditionModel()
{
}

double
NYUInHChannelConditionModel::ComputePlos(Ptr<const MobilityModel> a,
                                         Ptr<const MobilityModel> b) const
{
    // NYU doesn't have a PLOS model for InH. Using 5GCM model for PLOS.
    // https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7999294 (table III, row 2)
    // compute the 2D distance between a and b
    double distance2D = Calculate2dDistance(a->GetPosition(), b->GetPosition());

    // NOTE: no indication is given about the UT height used to derive the
    // LOS probability compute the LOS probability

    double pLos = 0.0;
    if (distance2D <= 1.2)
    {
        pLos = 1.0;
    }
    else if (distance2D > 1.2 && distance2D < 6.5)
    {
        pLos = exp(-(distance2D - 1.2) / 4.7);
    }
    else
    {
        pLos = exp(-(distance2D - 6.5) / 32.6) * 0.32;
    }
    return pLos;
}

// ------------------------------------------------------------------------- //

NS_OBJECT_ENSURE_REGISTERED(NYUInFChannelConditionModel);

TypeId
NYUInFChannelConditionModel::GetTypeId()
{
    static TypeId tid = TypeId("ns3::NYUInFChannelConditionModel")
                            .SetParent<NYUChannelConditionModel>()
                            .SetGroupName("Propagation")
                            .AddConstructor<NYUInFChannelConditionModel>();
    return tid;
}

NYUInFChannelConditionModel::NYUInFChannelConditionModel()
    : NYUChannelConditionModel()
{
}

NYUInFChannelConditionModel::~NYUInFChannelConditionModel()
{
}

double
NYUInFChannelConditionModel::ComputePlos(Ptr<const MobilityModel> a,
                                         Ptr<const MobilityModel> b) const
{
    // NYU Channel model doesn't have a PLOS for InF. To be extended with NYU Probability model for
    // InF later
    double pLos = 0.0;
    double distance2D = Calculate2dDistance(a->GetPosition(), b->GetPosition());
    pLos = 2.38 * exp(pow(-distance2D, 0.16) / 0.91);
    return pLos;
}

} // end namespace ns3

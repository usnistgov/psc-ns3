// Copyright (c) 2022 CTTC
// Copyright (c) 2023 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "traffic-generator-3gpp-generic-video.h"

#include "ns3/double.h"
#include "ns3/enum.h"
#include "ns3/inet-socket-address.h"
#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("TrafficGenerator3gppGenericVideo");
NS_OBJECT_ENSURE_REGISTERED(TrafficGenerator3gppGenericVideo);

TypeId
TrafficGenerator3gppGenericVideo::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::TrafficGenerator3gppGenericVideo")
            .SetParent<TrafficGenerator>()
            .SetGroupName("Applications")
            .AddConstructor<TrafficGenerator3gppGenericVideo>()
            .AddAttribute("DataRate",
                          "The desired data rate in Mbps.",
                          DoubleValue(5),
                          MakeDoubleAccessor(&TrafficGenerator3gppGenericVideo::m_dataRate),
                          MakeDoubleChecker<double>())
            .AddAttribute("MinDataRate",
                          "The minimum desired data rate in Mbps.",
                          DoubleValue(0.1),
                          MakeDoubleAccessor(&TrafficGenerator3gppGenericVideo::m_minDataRate),
                          MakeDoubleChecker<double>())
            .AddAttribute("MaxDataRate",
                          "The maximum desired data rate in Mbps.",
                          DoubleValue(10),
                          MakeDoubleAccessor(&TrafficGenerator3gppGenericVideo::m_maxDataRate),
                          MakeDoubleChecker<double>())
            .AddAttribute(
                "LowerThresholdForDecreasingSlowly",
                "The lower packet loss bound for decreasing the video traffic slowly.",
                DoubleValue(0.10),
                MakeDoubleAccessor(
                    &TrafficGenerator3gppGenericVideo::m_lowerThresholdForDecreasingSlowly),
                MakeDoubleChecker<double>())
            .AddAttribute(
                "LowerThresholdForDecreasingQuickly",
                "The lower packet loss bound for decreasing the video traffic quickly.",
                DoubleValue(1),
                MakeDoubleAccessor(
                    &TrafficGenerator3gppGenericVideo::m_lowerThresholdForDecreasingQuickly),
                MakeDoubleChecker<double>())
            .AddAttribute("UpperThresholdForIncreasing",
                          "The upper packet loss bound for increasing the video traffic.",
                          DoubleValue(0.02),
                          MakeDoubleAccessor(
                              &TrafficGenerator3gppGenericVideo::m_upperThresholdForIncreasing),
                          MakeDoubleChecker<double>())
            .AddAttribute(
                "IncreaseDataRateMultiplier",
                "The multiplier when increasing the video traffic volume, e.g., 3 to increase 3 "
                "times. Used to decrease fps or data rate.",
                DoubleValue(1.1),
                MakeDoubleAccessor(&TrafficGenerator3gppGenericVideo::m_increaseDataRateMultiplier),
                MakeDoubleChecker<double>())
            .AddAttribute(
                "DecreaseDataRateSlowlyMultiplier",
                "The multiplier when decreasing the video traffic volume slowly, e.g, 0.75, to "
                "decrease 25%. Used to decrease fps or data rate.",
                DoubleValue(0.5),
                MakeDoubleAccessor(
                    &TrafficGenerator3gppGenericVideo::m_decreaseDataRateSlowlyMultiplier),
                MakeDoubleChecker<double>())
            .AddAttribute(
                "DecreaseDataRateQuicklyMultiplier",
                "The multiplier when decreasing the video traffic volume quickly, e.g. 0.2 to "
                "decrease 5 times. Used to decrease fps or data rate.",
                DoubleValue(0.5),
                MakeDoubleAccessor(
                    &TrafficGenerator3gppGenericVideo::m_decreaseDataRateQuicklyMultiplier),
                MakeDoubleChecker<double>())
            .AddAttribute("Fps",
                          "Frame generation rate (per second). E.g. typical value cold be 60fps.",
                          UintegerValue(60),
                          MakeUintegerAccessor(&TrafficGenerator3gppGenericVideo::m_fps),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("MinFps",
                          "The minimum frame generation rate (per second). ",
                          UintegerValue(10),
                          MakeUintegerAccessor(&TrafficGenerator3gppGenericVideo::m_minFps),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("MaxFps",
                          "The maximum frame generation rate (per second). ",
                          UintegerValue(240),
                          MakeUintegerAccessor(&TrafficGenerator3gppGenericVideo::m_maxFps),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute(
                "StdRatioPacketSize",
                "STD ratio wrt the mean packet size. "
                "See Table 5.1.1.1-1 of 3GPP TR 38.838 V17.0.0 (2021-12)."
                "Typical values are 10.5% and 3%.",
                DoubleValue(0.105),
                MakeDoubleAccessor(&TrafficGenerator3gppGenericVideo::m_stdRatioPacketSize),
                MakeDoubleChecker<double>(0, 1))
            .AddAttribute(
                "MinRatioPacketSize",
                "Min ratio wrt the mean packet size. "
                "See Table 5.1.1.1-1 of 3GPP TR 38.838 V17.0.0 (2021-12)."
                "Typical values are 50% and 91%.",
                DoubleValue(0.5),
                MakeDoubleAccessor(&TrafficGenerator3gppGenericVideo::m_minRatioPacketSize),
                MakeDoubleChecker<double>(0, 1))
            .AddAttribute(
                "MaxRatioPacketSize",
                "Max ratio wrt the mean packet size. "
                "See Table 5.1.1.1-1 of 3GPP TR 38.838 V17.0.0 (2021-12)."
                "Typical values are 150% and 109%.",
                DoubleValue(1.5),
                MakeDoubleAccessor(&TrafficGenerator3gppGenericVideo::m_maxRatioPacketSize),
                MakeDoubleChecker<double>(1, 2))
            .AddAttribute("MeanPacketArrivalJitter",
                          "The mean of packet arrival jitter in milliseconds.",
                          UintegerValue(0),
                          MakeUintegerAccessor(&TrafficGenerator3gppGenericVideo::m_meanJitter),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("StdPacketArrivalJitter",
                          "The STD of packet arrival jitter in milliseconds.",
                          UintegerValue(2),
                          MakeUintegerAccessor(&TrafficGenerator3gppGenericVideo::m_stdJitter),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("BoundJitter",
                          "The periodicity in milliseconds.",
                          UintegerValue(2),
                          MakeUintegerAccessor(&TrafficGenerator3gppGenericVideo::m_boundJitter),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("Remote",
                          "The address of the destination",
                          AddressValue(),
                          MakeAddressAccessor(&TrafficGenerator::SetRemote),
                          MakeAddressChecker())
            .AddAttribute("Protocol",
                          "The type of protocol to use.",
                          TypeIdValue(TcpSocketFactory::GetTypeId()),
                          MakeTypeIdAccessor(&TrafficGenerator::SetProtocol),
                          MakeTypeIdChecker())
            .AddAttribute(
                "AlgType",
                "Type of the algorithm for the codec adaptation",
                EnumValue(LoopbackAlgType::ADJUST_IPA_TIME),
                MakeEnumAccessor<LoopbackAlgType>(
                    &TrafficGenerator3gppGenericVideo::SetLoopbackAlgType,
                    &TrafficGenerator3gppGenericVideo::GetLoopbackAlgType),
                MakeEnumChecker(TrafficGenerator3gppGenericVideo::ADJUST_IPA_TIME,
                                "AIPAT",
                                TrafficGenerator3gppGenericVideo::ADJUST_PACKET_SIZE,
                                "APS",
                                TrafficGenerator3gppGenericVideo::ADJUST_FPS,
                                "AFPS",
                                TrafficGenerator3gppGenericVideo::WO,
                                "WO",
                                TrafficGenerator3gppGenericVideo::ADJUST_PACKET_SIZE_UP_AGG,
                                "APS_UPAGG"))
            .AddTraceSource("Tx",
                            "A new packet is created and is sent",
                            MakeTraceSourceAccessor(&TrafficGenerator::m_txTrace),
                            "ns3::TrafficGenerator::TxTracedCallback")
            .AddTraceSource(
                "Params",
                "Traffic parameters have been updated accordingly "
                "the loopback algorithm, and notified through this trace.",
                MakeTraceSourceAccessor(&TrafficGenerator3gppGenericVideo::m_paramsTrace),
                "ns3::TrafficGenerator3gppGenericVideo::ParamsTracedCallback");
    return tid;
}

TrafficGenerator3gppGenericVideo::LoopbackAlgType
TrafficGenerator3gppGenericVideo::GetLoopbackAlgType() const
{
    return m_loopbackAlgType;
}

void
TrafficGenerator3gppGenericVideo::SetLoopbackAlgType(LoopbackAlgType loopbackAlgType)
{
    m_loopbackAlgType = loopbackAlgType;
}

TrafficGenerator3gppGenericVideo::TrafficGenerator3gppGenericVideo()
    : TrafficGenerator()
{
    NS_LOG_FUNCTION(this);
}

TrafficGenerator3gppGenericVideo::~TrafficGenerator3gppGenericVideo()
{
    NS_LOG_FUNCTION(this);
}

void
TrafficGenerator3gppGenericVideo::StartApplication()
{
    NS_LOG_FUNCTION(this);
    // add the initial values to the trace

    InetSocketAddress address = InetSocketAddress::ConvertFrom(GetPeer());
    m_port = address.GetPort();

    m_paramsTrace(Simulator::Now(),
                  m_port,
                  m_dataRate,
                  m_fps,
                  m_meanPacketSize,
                  0.0,
                  Seconds(0),
                  Seconds(0));
    SendPacketBurst();
}

void
TrafficGenerator3gppGenericVideo::PacketBurstSent()
{
    NS_LOG_FUNCTION(this);
    // in 3GPP description of the Option 2 (video + audio/data) there is no notion of frames or
    // packet bursts, just packets
    NS_ABORT_MSG("This function should not be called for the video + audio/data traffic");
}

void
TrafficGenerator3gppGenericVideo::GenerateNextPacketBurstSize()
{
    NS_LOG_FUNCTION(this);
    SetPacketBurstSizeInPackets(1);
}

Time
TrafficGenerator3gppGenericVideo::GetNextPacketTime() const
{
    NS_LOG_FUNCTION(this);
    double packetJitter = 0;
    while (true)
    {
        packetJitter = m_packetJitter->GetValue();
        if (packetJitter <= m_boundJitter && packetJitter > -m_boundJitter)
        {
            break;
        }
        else
        {
            NS_LOG_DEBUG("Generated packet jitter is out of configured bounds. Generated value:"
                         << packetJitter);
        }
    }

    double packetTimeMs = (1e3 * 1 / (double)m_fps) + packetJitter;
    NS_ASSERT(packetTimeMs);
    NS_LOG_DEBUG("Next packet time in Milliseconds: " << packetTimeMs);
    return Seconds(1e-3 * packetTimeMs);
}

uint32_t
TrafficGenerator3gppGenericVideo::GetNextPacketSize() const
{
    NS_LOG_FUNCTION(this);
    uint32_t packetSize = 0;
    while (true)
    {
        packetSize = m_packetSize->GetValue();
        if (packetSize <= m_maxRatioPacketSize * m_meanPacketSize &&
            packetSize > m_minRatioPacketSize * m_meanPacketSize)
        {
            break;
        }
        else
        {
            NS_LOG_DEBUG("Generated packet size is out of configured bounbds. Generated value:"
                         << packetSize);
        }
    }
    return packetSize;
}

void
TrafficGenerator3gppGenericVideo::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    m_meanPacketSize = (m_dataRate * 1e6) / (m_fps) / 8;
    /*  double max = m_maxRatio * mean;
      double min = m_minRatio * mean;*/
    m_packetSize = CreateObject<NormalRandomVariable>();
    m_packetSize->SetAttribute("Mean", DoubleValue(m_meanPacketSize));
    m_packetSize->SetAttribute("Variance", DoubleValue(m_stdRatioPacketSize * m_meanPacketSize));

    m_packetJitter = CreateObject<NormalRandomVariable>();
    m_packetJitter->SetAttribute("Mean", DoubleValue(m_meanJitter));
    m_packetJitter->SetAttribute("Variance", DoubleValue(m_stdJitter));
    m_packetJitter->SetAttribute("Bound", DoubleValue(m_boundJitter));
    // chain up
    TrafficGenerator::DoInitialize();
}

void
TrafficGenerator3gppGenericVideo::ReceiveLoopbackInformation(double packetLoss,
                                                             uint32_t packetReceived,
                                                             double windowInSeconds,
                                                             Time packetDelay,
                                                             Time packetDelayJitter)
{
    NS_LOG_FUNCTION(this);

    if (!m_stopEvent.IsPending())
    {
        NS_LOG_WARN("The application stopped working ignore this function call...");
        return;
    }

    if (Simulator::Now() - m_startTime < Seconds(windowInSeconds))
    {
        return;
    }

    double tempDataRate = m_dataRate;
    uint32_t tempFps = m_fps;
    double tempMeanPacketSize = m_meanPacketSize;

    // TX in the last window
    double txPacketLossEstimation =
        std::max(0.0, std::min(1.0, 1 - ((double)packetReceived / (m_fps * windowInSeconds))));

    NS_LOG_INFO("Packets received:" << packetReceived
                                    << ", packets expected: " << m_fps * windowInSeconds
                                    << ", packet loss estimation:" << txPacketLossEstimation);

    // TODO change later how we decide which packet loss we use in the algorithm
    // the one that is reported by the XrLoopback, or this one calculated by
    // the traffic generator
    packetLoss = txPacketLossEstimation;

    if (m_loopbackAlgType == ADJUST_IPA_TIME)
    {
        if (packetLoss > m_lowerThresholdForDecreasingSlowly)
        {
            // update data rate
            m_dataRate = std::max(m_dataRate * m_decreaseDataRateSlowlyMultiplier, m_minDataRate);
            m_fps = std::max(m_fps * m_decreaseDataRateSlowlyMultiplier, m_minFps);
        }
        else if (packetLoss < m_upperThresholdForIncreasing)
        {
            m_dataRate = std::min(m_dataRate * m_increaseDataRateMultiplier, m_maxDataRate);
            m_fps = std::min(m_fps * m_increaseDataRateMultiplier, m_maxFps);
        }
        else
        {
            NS_LOG_INFO("Packet loss is in an accepted range to not change anything");
        }
    }
    else if (m_loopbackAlgType == ADJUST_PACKET_SIZE)
    {
        if (packetLoss > m_lowerThresholdForDecreasingSlowly &&
            packetLoss < m_lowerThresholdForDecreasingQuickly)
        {
            // decrease data rate "slowly"
            m_dataRate = std::max(m_dataRate * m_decreaseDataRateSlowlyMultiplier, m_minDataRate);
        }
        else if (packetLoss >= m_lowerThresholdForDecreasingQuickly)
        {
            // update data rate
            m_dataRate = std::max(m_dataRate * m_decreaseDataRateQuicklyMultiplier, m_minDataRate);
        }
        else if (packetLoss < m_upperThresholdForIncreasing)
        {
            // update data rate
            m_dataRate = std::min(m_dataRate * m_increaseDataRateMultiplier, m_maxDataRate);
        }
        else
        {
            NS_LOG_INFO("Packet loss is in an acceptable range to not change anything");
        }
    }
    else if (m_loopbackAlgType == ADJUST_PACKET_SIZE_UP_AGG)
    {
        if (packetLoss > m_lowerThresholdForDecreasingSlowly &&
            packetLoss < m_lowerThresholdForDecreasingQuickly)
        {
            // decrease data rate "slowly"
            m_dataRate = std::max(m_dataRate * m_decreaseDataRateSlowlyMultiplier, m_minDataRate);
        }
        else if (packetLoss >= m_lowerThresholdForDecreasingQuickly)
        {
            // update data rate
            m_dataRate = std::max(m_dataRate * m_decreaseDataRateQuicklyMultiplier, m_minDataRate);
        }
        else if (packetLoss < m_upperThresholdForIncreasing)
        {
            // update data rate
            m_dataRate = std::min(m_dataRate * 1.5, m_maxDataRate);
        }
        else
        {
            NS_LOG_INFO("Packet loss is in an acceptable range to not change anything");
        }
    }
    else if (m_loopbackAlgType == ADJUST_FPS)
    {
        if (packetLoss > m_lowerThresholdForDecreasingSlowly)
        {
            // update data rate
            m_fps = std::max(m_fps * m_decreaseDataRateSlowlyMultiplier, m_minFps);
        }
        else if (packetLoss < m_upperThresholdForIncreasing)
        {
            m_fps = std::min(m_fps * m_increaseDataRateMultiplier, m_maxFps);
        }
        else
        {
            NS_LOG_INFO("Packet loss is in an accepted range to not change anything");
        }
    }

    /*
      if (packetLoss > 0 && packetLoss < 0.1)
        {
          // update data rate
          m_dataRate *= 1.1;
        }
      else if (packetLoss > 0.5 && packetLoss < 1)
        {
          m_dataRate = std::max (m_dataRate/2 ,m_minDataRate);
        }
      else if (packetLoss == 1)
        {
          m_dataRate = std::max (m_dataRate/2 ,m_minDataRate);
          m_fps = std::min (m_fps * 2, m_maxFps);
        }
      else
        {
          NS_LOG_INFO ("Packet loss is in an accepted range to not change anything");
        }
        */
    // update mean packet size
    m_meanPacketSize = (m_dataRate * 1e6) / (m_fps) / 8;
    // update packet size random generator parameters
    m_packetSize->SetAttribute("Mean", DoubleValue(m_meanPacketSize));
    m_packetSize->SetAttribute("Variance", DoubleValue(m_stdRatioPacketSize * m_meanPacketSize));

    m_paramsTrace(Simulator::Now(),
                  m_port,
                  m_dataRate,
                  m_fps,
                  m_meanPacketSize,
                  packetLoss,
                  packetDelay,
                  packetDelayJitter);

    if (tempDataRate != m_dataRate || tempFps != m_fps || tempMeanPacketSize != m_meanPacketSize)
    {
        // m_paramsTrace (Simulator::Now (), GetTgId (), m_dataRate, m_fps, m_meanPacketSize);
        NS_LOG_DEBUG("Old data rate: " << tempDataRate << " new data rate: " << m_dataRate);
        NS_LOG_DEBUG("Old fps:       " << tempFps << " new fps:       " << m_fps);
        NS_LOG_DEBUG("Old mean packet size:       "
                     << tempMeanPacketSize << " new mean packet size:       " << m_meanPacketSize);
    }
}

int64_t
TrafficGenerator3gppGenericVideo::AssignStreams(int64_t stream)
{
    m_packetSize->SetStream(stream);
    m_packetJitter->SetStream(stream + 1);
    return 2;
}

} // Namespace ns3

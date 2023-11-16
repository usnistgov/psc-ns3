/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2022 University of Washington
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <iostream>
#include <set>
#include <vector>
#include <bitset>
#include <ns3/test.h>
#include <ns3/log.h>
#include <ns3/data-rate.h>
#include <ns3/lte-rrc-sap.h>
#include <ns3/nr-sl-comm-resource-pool.h>
#include <ns3/node-container.h>
#include <ns3/net-device-container.h>
#include <ns3/mobility-helper.h>
#include <ns3/position-allocator.h>
#include <ns3/vector.h>
#include <ns3/nstime.h>
#include <ns3/nr-helper.h>
#include <ns3/nr-sl-helper.h>
#include <ns3/nr-eesm-ir-t1.h>
#include <ns3/nr-point-to-point-epc-helper.h>
#include <ns3/cc-bwp-helper.h>
#include <ns3/boolean.h>
#include <ns3/pointer.h>
#include <ns3/uinteger.h>
#include <ns3/double.h>
#include <ns3/ptr.h>
#include <ns3/enum.h>
#include <ns3/nr-ue-net-device.h>
#include <ns3/isotropic-antenna-model.h>
#include <ns3/nr-control-messages.h>
#include <ns3/nr-sl-ue-mac-harq.h>
#include <ns3/nr-ue-mac.h>
#include <ns3/nr-ue-phy.h>
#include <ns3/nr-spectrum-phy.h>
#include <ns3/nr-sl-comm-resource-pool-factory.h>
#include <ns3/nr-sl-ue-mac-scheduler-default.h>
#include <ns3/lte-rrc-sap.h>
#include <ns3/internet-stack-helper.h>
#include <ns3/ipv4-static-routing-helper.h>
#include <ns3/ipv6-static-routing-helper.h>
#include <ns3/ipv4.h>
#include <ns3/ipv6.h>
#include <ns3/application-container.h>
#include <ns3/node-container.h>
#include <ns3/packet-sink-helper.h>
#include <ns3/packet-sink.h>
#include <ns3/on-off-helper.h>

/**
 * \file nr-test-sidelink-harq.cc
 * \ingroup test
 *
 */
namespace ns3 {

namespace test {

NS_LOG_COMPONENT_DEFINE ("SidelinkHarqTestSuite");

/**
 * \brief Utility function converting a string to an enum
 *
 * \param castType The cast type represented as a string
 * \return The cast type represented as an enum
 */
SidelinkInfo::CastType
ConvertToEnum (const std::string& castType)
{
  if (castType == "groupcast")
    {
      return SidelinkInfo::CastType::Groupcast;
    }
  else if (castType == "broadcast")
    {
      return SidelinkInfo::CastType::Broadcast;
    }
  else if (castType == "unicast")
    {
      return SidelinkInfo::CastType::Unicast;
    }
  NS_FATAL_ERROR ("Unknown cast type " << castType);
}

/**
 * Error model for forcing specific sequences of decode failures, for testing
 *
 * Inherit from NrEesmIrT1 error model for concrete implementations of
 * most other methods.
 * 
 * The vector passed in is used to override the m_tbler value on the first
 * UE receiver, after the actual error model is consulted.  If the size of
 * the vector is smaller than the number of calls to the error model, the
 * vector is repeatedly reused via modulo arithmetic on its index.
 */
class SlHarqTestErrorModel : public NrEesmIrT1
{
public:
  /**
   * \brief GetTypeId
   * \return the TypeId of the class
   */
  static TypeId GetTypeId ();

  SlHarqTestErrorModel ();

  virtual Ptr<NrErrorModelOutput> GetTbDecodificationStats (const SpectrumValue& sinr,
                                                            const std::vector<int>& map,
                                                            uint32_t size, uint8_t mcs,
                                                            const NrErrorModelHistory &history);
  void SetTblerVector (std::vector<double> tblerVector) {m_tblerVector = tblerVector;}
private:
  uint64_t m_count {0};
  std::vector<double> m_tblerVector;
};

NS_OBJECT_ENSURE_REGISTERED (SlHarqTestErrorModel);

SlHarqTestErrorModel::SlHarqTestErrorModel ()
{
}

TypeId
SlHarqTestErrorModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::test::SlHarqTestErrorModel")
    .SetParent<NrEesmIrT1> ()
    .AddConstructor<SlHarqTestErrorModel> ()
  ;
  return tid;
}

Ptr<NrErrorModelOutput>
SlHarqTestErrorModel::GetTbDecodificationStats (const SpectrumValue& sinr,
                                                const std::vector<int>& map,
                                                uint32_t size, uint8_t mcs,
                                                const NrErrorModelHistory &history)
{
  Ptr<NrErrorModelOutput> p = NrEesmIrT1::GetTbDecodificationStats (sinr, map, size, mcs, history);
  if (m_tblerVector.size () == 0)
    {
      // Do not alter configured error model
      return p;
    }
  else
    {
      p->m_tbler = m_tblerVector[m_count % m_tblerVector.size ()];
      m_count++;
      return p;
    }
}


class TestSidelinkHarq : public TestCase
{
public:
  TestSidelinkHarq (const std::string &name)
    : TestCase (name)
  {}

  void SetCastType (std::string castType) {m_castType = castType;};
  void SetNumUe (uint32_t numUe) {m_numUe = numUe;}
  void SetPsfchPeriod (uint32_t psfchPeriod) {m_psfchPeriod = psfchPeriod;}
  void SetHarqEnabled (bool harqEnabled) {m_harqEnabled = harqEnabled;}
  void SetTddPattern (std::string tddPattern) {m_tddPattern = tddPattern;}
  void SetSlBitmap (std::vector <std::bitset<1> >& slBitmap) {m_slBitmap = slBitmap;}
  void SetNumerology (uint16_t numerology) {m_numerologyBwpSl = numerology;}
  void SetSlSubchannelSize (uint16_t size) {m_slSubchannelSize = size;}
  void SetT2 (uint16_t t2) {m_t2 = t2;}
  void SetNumPackets (uint32_t numPackets) {m_numPackets = numPackets;}
  void SetDistance (double distance) {m_interUeDistance = distance;}
  // For now, will just install on first receiving UE; can expand to
  // support multiple UEs, or PSFCH channel, in the future
  void SetTblerVector (std::vector<double> tblerVector) {m_tblerVector = tblerVector;}
  void SetIpv6 (bool useIpv6) {m_useIpv6 = useIpv6;}

  void CheckAppRxCount (uint32_t expected);
  void CheckHarqAckCount (uint32_t expected);
  void CheckHarqNackCount (uint32_t expected);
  void CheckHarqAllocateCount (uint32_t expected);
  void CheckHarqDeallocateCount (uint32_t expected);
  void CheckHarqPacketBurstCount (uint32_t expected);
  void CheckHarqTimeoutCount (uint32_t expected);
  void CheckRxSlDataFrameCount (uint32_t expected);
  void CheckPscchDecodeFailure (uint32_t expected);
  void CheckSci2aDecodeFailure (uint32_t expected);
  void CheckTbDecodeFailure (uint32_t expected);

  /**
   * \brief Method to listen the packet sink application trace Rx.
   * \param context The trace context
   * \param packet The packet
   * \param from The address of the transmitting node
   * \param to The address of the receiving node
   * \param header The SeqTsSize header
   */
  void ReceivePacket (std::string context, Ptr<const Packet> packet, const Address &from, const Address & to, const SeqTsSizeHeader &header);

protected:
  /**
   * \brief Configure a single application on UE 0
   * \param remoteAddress Remote address to send to
   * \param localAddress Local address to bind to
   * \param groupAddress6 IPv6 multicast group address
   * \param ueContainer UE node container
   * \param finalSlBearersActivationTime start time for applications
   * \param finalSimTime simulation end time
   */
  virtual void ConfigureApplications (Address remoteAddress, Address localAddress, Ipv6Address groupAddress6,
    const NodeContainer& ueContainer, Time finalSlBearersActivationTime, Time finalSimTime);
  /**
   *
   */
  virtual void ConfigureTfts (Ptr<NrSlHelper> nrSlHelper, Ptr<NrPointToPointEpcHelper> epcHelper,
    const NodeContainer& ueContainer, const NetDeviceContainer& ueNetDevices,
    Ipv4Address groupAddress4, Ipv6Address groupAddress6, uint16_t port, uint32_t dstL2Id,
    Address& localAddress, Address& remoteAddress, Time delayBudget, Time finalSlBearersActivationTime);

  /**
   * \brief Method to listen the NrSpectrumPhy::RxDataTrace
   * \param context The trace context
   * \param sfnsf The SfnSf
   * \param rxPsd Received PSD values
   * \param duration Duration of the reception 
   * \param bwpId Bandwidth Part ID
   * \param cellId Cell ID
   */
 void ReceiveSlDataFrame (std::string context, const SfnSf & sfnsf, Ptr<const SpectrumValue> rxPsd, const Time & duration, uint16_t bwpId, uint16_t cellId);

  /**
   * \brief Method to listen the HARQ feedback trace
   * \param context The trace context
   * \param harqInfo The HARQ feedback information
   */
  void ReceiveHarqFeedback (std::string context, const SlHarqInfo& harqInfo);

  /**
   * \brief Method to listen the HARQ allocate trace
   * \param context The trace context
   * \param harqId HARQ process ID
   * \param dstL2Id Destination L2 ID
   * \param timeout Timeout
   * \param available Number of remaining available HARQ process IDs
   */
  void HarqAllocate (std::string context, uint8_t harqId, uint32_t dstL2Id, Time timeout, std::size_t available);

  /**
   * \brief Method to listen the HARQ deallocate trace
   * \param harqId HARQ process ID
   * \param available Number of remaining available HARQ process IDs
   */
  void HarqDeallocate (std::string context, uint8_t harqId, std::size_t available);

  /**
   * \brief Method to listen for requests for packet bursts (retransmissions)
   * \param dstL2Id Destination L2 ID
   * \param harqId HARQ process ID
   */
  void HarqPacketBurst (std::string context, uint32_t dstL2Id, uint8_t harqId);

  /**
   * \brief Method to listen the HARQ timeout trace
   * \param harqId HARQ process ID
   */
  void HarqTimeout (std::string context, uint8_t harqId);

  /**
   * \brief Method to listen for PSCCH decode failures
   * \param context The trace context
   * \param oldValue Old value of the count
   * \param newValue New value of the count
   */
  void TraceSlPscchDecodeFailure (std::string context, uint64_t oldValue, uint64_t newValue);

  /**
   * \brief Method to listen for SCI-2a decode failures
   * \param context The trace context
   * \param oldValue Old value of the count
   * \param newValue New value of the count
   */
  void TraceSlSci2aDecodeFailure (std::string context, uint64_t oldValue, uint64_t newValue);

  /**
   * \brief Method to listen for TB decode failures
   * \param context The trace context
   * \param oldValue Old value of the count
   * \param newValue New value of the count
   */
  void TraceSlTbDecodeFailure (std::string context, uint64_t oldValue, uint64_t newValue);

  // Utility
  uint32_t FindGridWidth (uint32_t numUe) const;

  // Virtual members
  virtual void DoRun (void) override;

  // Constructor arguments, initialized by member initializers
  uint32_t m_numUe {3};
  std::string m_castType {"groupcast"};
  bool m_harqEnabled {true};
  uint16_t m_numerologyBwpSl {2};
  uint16_t m_slSubchannelSize {50}; // PRBs
  uint16_t m_t1 {2};
  uint16_t m_t2 {33};
  Time m_delayBudget {MilliSeconds (20)};
  uint32_t m_numPackets {10};
  uint32_t m_udpPacketSize {200};
  DataRate m_dataRate {DataRate ("16Kbps")};
  std::string m_tddPattern {"DL|DL|DL|F|UL|UL|UL|UL|UL|UL|"};
  std::vector <std::bitset<1> > m_slBitmap {std::vector<std::bitset<1> > {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1}};
  uint16_t m_psfchPeriod {4};
  bool m_useIpv6 {true};
  double m_interUeDistance {20}; // meters
  std::vector<double> m_tblerVector; // for manual override of error model

  // Checks
  bool m_checkAppRxCount {false};
  bool m_checkHarqRxCount {false};
  bool m_checkHarqNackCount {false};
  bool m_checkHarqAllocateCount {false};
  bool m_checkHarqDeallocateCount {false};
  bool m_checkHarqPacketBurstCount {false};
  bool m_checkHarqTimeoutCount {false};
  bool m_checkRxSlDataFrameCount {false};
  bool m_checkPscchDecodeFailure {false};
  bool m_checkSci2aDecodeFailure {false};
  bool m_checkTbDecodeFailure {false};
  uint32_t m_appRxCount {0};
  uint32_t m_expectedAppRxCount {0};
  uint32_t m_harqAckCount {0};
  uint32_t m_expectedHarqAckCount {0};
  uint32_t m_harqNackCount {0};
  uint32_t m_expectedHarqNackCount {0};
  uint32_t m_harqAllocateCount {0};
  uint32_t m_expectedHarqAllocateCount {0};
  uint32_t m_harqDeallocateCount {0};
  uint32_t m_expectedHarqDeallocateCount {0};
  uint32_t m_harqPacketBurstCount {0};
  uint32_t m_expectedHarqPacketBurstCount {0};
  uint32_t m_harqTimeoutCount {0};
  uint32_t m_expectedHarqTimeoutCount {0};
  uint32_t m_rxSlDataFrameCount {0};
  uint32_t m_expectedRxSlDataFrameCount {0};
  uint32_t m_pscchDecodeFailureCount {0};
  uint32_t m_expectedPscchDecodeFailureCount {0};
  uint32_t m_sci2aDecodeFailureCount {0};
  uint32_t m_expectedSci2aDecodeFailureCount {0};
  uint32_t m_tbDecodeFailureCount {0};
  uint32_t m_expectedTbDecodeFailureCount {0};
};

void
TestSidelinkHarq::ConfigureApplications (Address remoteAddress, Address localAddress, Ipv6Address groupAddress6,
   const NodeContainer& ueContainer, Time finalSlBearersActivationTime, Time finalSimTime)
{
  NS_LOG_FUNCTION (this);
  //Set Application in the UEs
  OnOffHelper sidelinkClient ("ns3::UdpSocketFactory", remoteAddress);
  sidelinkClient.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
  sidelinkClient.SetAttribute ("MaxBytes", UintegerValue (m_numPackets * m_udpPacketSize));
  sidelinkClient.SetConstantRate (m_dataRate, m_udpPacketSize);

  ApplicationContainer clientApps = sidelinkClient.Install (ueContainer.Get (0));
  //onoff application will send the first packet at :
  clientApps.Start (finalSlBearersActivationTime);
  clientApps.Stop (finalSimTime);

  //Output app start, stop and duration

  PacketSinkHelper sidelinkSink ("ns3::UdpSocketFactory", localAddress);
  sidelinkSink.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
  NodeContainer sinks;
  for (uint32_t u = 1; u < ueContainer.GetN (); ++u)
    {
      sinks.Add (ueContainer.Get (u));
    }
  ApplicationContainer serverApps = sidelinkSink.Install (sinks);
  serverApps.Start (Seconds (1.0));

  if (m_useIpv6)
    {
      for (uint16_t ac = 0; ac < clientApps.GetN (); ac++)
        {
          clientApps.Get (ac)->GetNode ()->GetObject<Ipv6L3Protocol> ()->AddMulticastAddress (groupAddress6);
        }

      // Set Rx traces
      for (uint16_t ac = 0; ac < serverApps.GetN (); ac++)
        {
          serverApps.Get (ac)->GetNode ()->GetObject<Ipv6L3Protocol> ()->AddMulticastAddress (groupAddress6);
        }
    }

  for (uint32_t i = 0; i < serverApps.GetN (); i++)
    {
      Ptr<PacketSink> sink = DynamicCast<PacketSink> (serverApps.Get (i));
      sink->TraceConnect ("RxWithSeqTsSize", std::to_string (serverApps.Get (i)->GetNode ()->GetId ()), 
        MakeCallback (&TestSidelinkHarq::ReceivePacket, this));
    }
}

void
TestSidelinkHarq::ConfigureTfts (Ptr<NrSlHelper> nrSlHelper, Ptr<NrPointToPointEpcHelper> epcHelper,
    const NodeContainer& ueContainer, const NetDeviceContainer& ueNetDevices,
    Ipv4Address groupAddress4, Ipv6Address groupAddress6, uint16_t port, uint32_t dstL2Id,
    Address& localAddress, Address& remoteAddress, Time delayBudget, Time finalSlBearersActivationTime)
{
  NS_LOG_FUNCTION (this);
  Ptr<LteSlTft> tft;
  SidelinkInfo slInfo;
  slInfo.m_castType = ConvertToEnum (m_castType);
  slInfo.m_dstL2Id = dstL2Id;
  slInfo.m_rri = MilliSeconds (100);
  slInfo.m_harqEnabled = m_harqEnabled;
  slInfo.m_pdb = delayBudget;
  if (!m_useIpv6)
    {
      Ipv4InterfaceContainer ueIpIface;
      ueIpIface = epcHelper->AssignUeIpv4Address (ueNetDevices);
      // set the default gateway for the UE
      Ipv4StaticRoutingHelper ipv4RoutingHelper;
      for (uint32_t u = 0; u < ueContainer.GetN (); ++u)
        {
          Ptr<Node> ueNode = ueContainer.Get (u);
          // Set the default gateway for the UE
          Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
          ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
        }
      localAddress = InetSocketAddress (Ipv4Address::GetAny (), port);
      if (m_castType == "groupcast" || m_castType == "broadcast")
        {
          slInfo.m_dstL2Id = 224;
          remoteAddress = InetSocketAddress (groupAddress4, port);
          // The first node is a transmitter; others are receivers
          NetDeviceContainer transmitDevices;
          transmitDevices.Add (ueNetDevices.Get (0));
          tft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT, groupAddress4, slInfo);
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, transmitDevices, tft);
          NetDeviceContainer receiveDevices;
          for (uint32_t u = 1; u < ueContainer.GetN (); ++u)
            {
              receiveDevices.Add (ueNetDevices.Get (u));
            }
          tft = Create<LteSlTft> (LteSlTft::Direction::RECEIVE, groupAddress4, slInfo);
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, receiveDevices, tft);
        }
      else
        {
          slInfo.m_dstL2Id = 2;  // RNTI of node 1
          remoteAddress = InetSocketAddress (ueIpIface.GetAddress (1, 0), port);
          tft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT, ueIpIface.GetAddress (1,0), slInfo);
          //Activate on only two NetDevices-- client and server
          NetDeviceContainer transmit;
          transmit.Add (ueNetDevices.Get (0));
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, transmit, tft);
          tft = Create<LteSlTft> (LteSlTft::Direction::RECEIVE, ueIpIface.GetAddress (1,0), slInfo);
          NetDeviceContainer receive;
          receive.Add (ueNetDevices.Get (1));
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, receive, tft);
        }
    }
  else
    {
      Ipv6InterfaceContainer ueIpIface;
      ueIpIface = epcHelper->AssignUeIpv6Address (ueNetDevices);

      // set the default gateway for the UE
      Ipv6StaticRoutingHelper ipv6RoutingHelper;
      for (uint32_t u = 0; u < ueContainer.GetN (); ++u)
        {
          Ptr<Node> ueNode = ueContainer.Get (u);
          // Set the default gateway for the UE
          Ptr<Ipv6StaticRouting> ueStaticRouting = ipv6RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv6> ());
          ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress6 (), 1);
        }
      localAddress = Inet6SocketAddress (Ipv6Address::GetAny (), port);
      if (m_castType == "groupcast" || m_castType == "broadcast")
        {
          slInfo.m_dstL2Id = 224;
          remoteAddress = Inet6SocketAddress (groupAddress6, port);
          // The first node is a transmitter; others are receivers
          NetDeviceContainer transmitDevices;
          transmitDevices.Add (ueNetDevices.Get (0));
          tft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT, groupAddress6, slInfo);
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, transmitDevices, tft);
          NetDeviceContainer receiveDevices;
          for (uint32_t u = 1; u < ueContainer.GetN (); ++u)
            {
              receiveDevices.Add (ueNetDevices.Get (u));
            }
          tft = Create<LteSlTft> (LteSlTft::Direction::RECEIVE, groupAddress6, slInfo);
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, receiveDevices, tft);
        }
      else
        {
          slInfo.m_dstL2Id = 2;  // RNTI of node 1
          remoteAddress = Inet6SocketAddress (ueIpIface.GetAddress (1, 0), port);
          tft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT, ueIpIface.GetAddress (1, 0), slInfo);
          //Activate on only two NetDevices-- client and server
          NetDeviceContainer transmit;
          transmit.Add (ueNetDevices.Get (0));
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, transmit, tft);
          tft = Create<LteSlTft> (LteSlTft::Direction::RECEIVE, ueIpIface.GetAddress (1, 0), slInfo);
          NetDeviceContainer receive;
          receive.Add (ueNetDevices.Get (1));
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, receive, tft);
        }
    }
}

// Utility function to find the proper grid width for a given number of UEs
uint32_t
TestSidelinkHarq::FindGridWidth (uint32_t numUe) const
{
  uint32_t temp = static_cast<uint32_t> (floor (sqrt (numUe)) * floor (sqrt (numUe)));
  if (temp == numUe)
    {
      // perfect square
      return static_cast<uint32_t> (floor (sqrt (numUe)));
    }
  else 
    {
      return static_cast<uint32_t> (floor (sqrt (numUe))) + 1;
    }
}

void
TestSidelinkHarq::CheckAppRxCount (uint32_t expected)
{
  m_checkAppRxCount = true;
  m_expectedAppRxCount = expected;
}

void
TestSidelinkHarq::CheckHarqAckCount (uint32_t expected)
{
  m_checkHarqRxCount = true;
  m_expectedHarqAckCount = expected;
}

void
TestSidelinkHarq::CheckHarqNackCount (uint32_t expected)
{
  m_checkHarqNackCount = true;
  m_expectedHarqNackCount = expected;
}

void
TestSidelinkHarq::CheckHarqAllocateCount (uint32_t expected)
{
  m_checkHarqAllocateCount = true;
  m_expectedHarqAllocateCount = expected;
}

void
TestSidelinkHarq::CheckHarqDeallocateCount (uint32_t expected)
{
  m_checkHarqDeallocateCount = true;
  m_expectedHarqDeallocateCount = expected;
}

void
TestSidelinkHarq::CheckHarqPacketBurstCount (uint32_t expected)
{
  m_checkHarqPacketBurstCount = true;
  m_expectedHarqPacketBurstCount = expected;
}

void
TestSidelinkHarq::CheckHarqTimeoutCount (uint32_t expected)
{
  m_checkHarqTimeoutCount = true;
  m_expectedHarqTimeoutCount = expected;
}

void
TestSidelinkHarq::CheckRxSlDataFrameCount (uint32_t expected)
{
  m_checkRxSlDataFrameCount = true;
  m_expectedRxSlDataFrameCount = expected;
}

void
TestSidelinkHarq::CheckPscchDecodeFailure (uint32_t expected)
{
  m_checkPscchDecodeFailure = true;
  m_expectedPscchDecodeFailureCount = expected;
}

void
TestSidelinkHarq::CheckSci2aDecodeFailure (uint32_t expected)
{
  m_checkSci2aDecodeFailure = true;
  m_expectedSci2aDecodeFailureCount = expected;
}

void
TestSidelinkHarq::CheckTbDecodeFailure (uint32_t expected)
{
  m_checkTbDecodeFailure = true;
  m_expectedTbDecodeFailureCount = expected;
}

void
TestSidelinkHarq::ReceivePacket (std::string context, Ptr<const Packet> packet, const Address &from, const Address & to, const SeqTsSizeHeader &header)
{
  NS_LOG_DEBUG ("Receive packet for " << context);
  m_appRxCount++;
}

void
TestSidelinkHarq::ReceiveHarqFeedback (std::string context, const SlHarqInfo& harqInfo)
{
  NS_LOG_DEBUG ("Receive HARQ feedback for " << context);
  if (harqInfo.IsReceivedOk ())
    {
      NS_LOG_DEBUG ("Receive HARQ ACK feedback for " << context);
      m_harqAckCount++;
    }
  else
    {
      NS_LOG_DEBUG ("Receive HARQ NACK feedback for " << context);
      m_harqNackCount++;
    }
}

void
TestSidelinkHarq::HarqAllocate (std::string context, uint8_t harqId, uint32_t dstL2Id, Time timeout, std::size_t available)
{
  NS_LOG_DEBUG ("HARQ allocation for " << context << " ID " << +harqId);
  m_harqAllocateCount++;
}

void
TestSidelinkHarq::HarqDeallocate (std::string context, uint8_t harqId, std::size_t available)
{
  NS_LOG_DEBUG ("HARQ deallocation for " << context << " ID " << +harqId);
  m_harqDeallocateCount++;
}

void
TestSidelinkHarq::HarqPacketBurst (std::string context, uint32_t dstL2Id, uint8_t harqId)
{
  NS_LOG_DEBUG ("HARQ packet burst for " << context << " dstL2Id " << dstL2Id << " ID " << +harqId);
  m_harqPacketBurstCount++;
}

void
TestSidelinkHarq::HarqTimeout (std::string context, uint8_t harqId)
{
  NS_LOG_DEBUG ("HARQ timeout for " << context << " ID " << +harqId);
  m_harqTimeoutCount++;
}

void
TestSidelinkHarq::ReceiveSlDataFrame (std::string context, const SfnSf & sfnsf, Ptr<const SpectrumValue> rxPsd, const Time & duration, uint16_t bwpId, uint16_t cellId)
{
  NS_LOG_DEBUG ("Receive SL data frame for " << context);
  m_rxSlDataFrameCount++;
}

void
TestSidelinkHarq::TraceSlPscchDecodeFailure (std::string context, uint64_t oldValue, uint64_t newValue)
{
  NS_LOG_DEBUG ("SL PSCCH decode failure for " << context << " value " << newValue);
  m_pscchDecodeFailureCount++;
}

void
TestSidelinkHarq::TraceSlSci2aDecodeFailure (std::string context, uint64_t oldValue, uint64_t newValue)
{
  NS_LOG_DEBUG ("SL SCI-2A decode failure for " << context << " value " << newValue);
  m_sci2aDecodeFailureCount++;
}

void
TestSidelinkHarq::TraceSlTbDecodeFailure (std::string context, uint64_t oldValue, uint64_t newValue)
{
  NS_LOG_DEBUG ("SL transport block decode failure for " << context << " value " << newValue);
  m_tbDecodeFailureCount++;
}

void
TestSidelinkHarq::DoRun ()
{
  NS_LOG_FUNCTION (this);
  Time slBearersActivationTime = Seconds (0.9);
  Time delayBudget = MilliSeconds (20);
  double centralFrequencyBandSl = 5.89e9; // band n47  TDD //Here band is analogous to channel
  uint16_t bandwidthBandSl = 400; //Multiple of 100 KHz; 400 = 40 MHz
  double txPower = 23; //dBm

  // Simulation times
  Time finalSlBearersActivationTime = slBearersActivationTime + Seconds (0.01);
  Time simTime = Seconds (m_numPackets * m_udpPacketSize * 8.0 / m_dataRate.GetBitRate ());
  Time finalSimTime = simTime + finalSlBearersActivationTime + MilliSeconds (100);

  NodeContainer ueContainer (m_numUe);

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  Ptr<GridPositionAllocator> positionAllocUe = CreateObject<GridPositionAllocator> ();
  positionAllocUe->SetAttribute ("GridWidth", UintegerValue (FindGridWidth (m_numUe)));
  positionAllocUe->SetAttribute ("DeltaX", DoubleValue (m_interUeDistance));
  positionAllocUe->SetAttribute ("DeltaY", DoubleValue (m_interUeDistance));
  positionAllocUe->SetAttribute ("Z", DoubleValue (1.6));
  mobility.SetPositionAllocator (positionAllocUe);
  mobility.Install (ueContainer);

  // NR configuration
  Ptr<NrPointToPointEpcHelper> epcHelper = CreateObject<NrPointToPointEpcHelper> ();
  Ptr<NrHelper> nrHelper = CreateObject<NrHelper> ();
  nrHelper->SetEpcHelper (epcHelper);

  BandwidthPartInfoPtrVector allBwps;
  CcBwpCreator ccBwpCreator;
  const uint8_t numCcPerBand = 1;

  /* Create the configuration for the CcBwpHelper. SimpleOperationBandConf
   * creates a single BWP per CC
   */
  CcBwpCreator::SimpleOperationBandConf bandConfSl (centralFrequencyBandSl, bandwidthBandSl, numCcPerBand, BandwidthPartInfo::RMa_LoS);

  // By using the configuration created, it is time to make the operation bands
  OperationBandInfo bandSl = ccBwpCreator.CreateOperationBandContiguousCc (bandConfSl);

  Config::SetDefault ("ns3::ThreeGppChannelModel::UpdatePeriod",TimeValue (MilliSeconds(10)));
  nrHelper->SetChannelConditionModelAttribute ("UpdatePeriod", TimeValue (MilliSeconds (10)));
  nrHelper->SetPathlossAttribute ("ShadowingEnabled", BooleanValue (false));

  nrHelper->InitializeOperationBand (&bandSl);
  allBwps = CcBwpCreator::GetAllBwps ({bandSl});

  epcHelper->SetAttribute ("S1uLinkDelay", TimeValue (MilliSeconds (0)));

  nrHelper->SetUeAntennaAttribute ("NumRows", UintegerValue (1));
  nrHelper->SetUeAntennaAttribute ("NumColumns", UintegerValue (2));
  nrHelper->SetUeAntennaAttribute ("AntennaElement", PointerValue (CreateObject<IsotropicAntennaModel> ()));

  nrHelper->SetUePhyAttribute ("TxPower", DoubleValue (txPower));

  //NR Sidelink attribute of UE MAC, which are would be common for all the UEs
  nrHelper->SetUeMacAttribute ("EnableSensing", BooleanValue (false));
  nrHelper->SetUeMacAttribute ("T1", UintegerValue (m_t1));
  nrHelper->SetUeMacAttribute ("T2", UintegerValue (m_t2));
  nrHelper->SetUeMacAttribute ("ActivePoolId", UintegerValue (0));
  nrHelper->SetUeMacAttribute ("NumSidelinkProcess", UintegerValue (4));

  uint8_t bwpIdForGbrMcptt = 0;

  nrHelper->SetBwpManagerTypeId (TypeId::LookupByName ("ns3::NrSlBwpManagerUe"));
  //following parameter has no impact at the moment because:
  //1. No support for PQI based mapping between the application and the LCs
  //2. No scheduler to consider PQI
  //However, till such time all the NR SL examples should use GBR_MC_PUSH_TO_TALK
  //because we hard coded the PQI 65 in UE RRC.
  nrHelper->SetUeBwpManagerAlgorithmAttribute ("GBR_MC_PUSH_TO_TALK", UintegerValue (bwpIdForGbrMcptt));

  std::set<uint8_t> bwpIdContainer;
  bwpIdContainer.insert (bwpIdForGbrMcptt);

  NetDeviceContainer ueNetDevices = nrHelper->InstallUeDevice (ueContainer, allBwps);

  /*
   * Case (iii): Go node for node and change the attributes we have to setup
   * per-node.
   */

  // When all the configuration is done, explicitly call UpdateConfig ()
  for (auto it = ueNetDevices.Begin (); it != ueNetDevices.End (); ++it)
    {
      DynamicCast<NrUeNetDevice> (*it)->UpdateConfig ();
    }

  Ptr<NrSlHelper> nrSlHelper = CreateObject <NrSlHelper> ();
  // Put the pointers inside NrSlHelper
  nrSlHelper->SetEpcHelper (epcHelper);

  std::string errorModel = "ns3::test::SlHarqTestErrorModel";
  nrSlHelper->SetSlErrorModel (errorModel);
  nrSlHelper->SetUeSlAmcAttribute ("AmcModel", EnumValue (NrAmc::ErrorModel));

  nrSlHelper->SetNrSlSchedulerTypeId (NrSlUeMacSchedulerDefault::GetTypeId());
  nrSlHelper->SetUeSlSchedulerAttribute ("FixNrSlMcs", BooleanValue (true));
  nrSlHelper->SetUeSlSchedulerAttribute ("InitialNrSlMcs", UintegerValue (14));

  nrSlHelper->PrepareUeForSidelink (ueNetDevices, bwpIdContainer);

  if (m_tblerVector.size ())
    {
      // Install error vector override on first UE receiver
      Ptr<NrUeNetDevice> dev = ueNetDevices.Get (1)->GetObject<NrUeNetDevice> ();
      Ptr<NrSpectrumPhy> sPhy = dev->GetPhy (0)->GetSpectrumPhy ();
      NS_ABORT_MSG_UNLESS (sPhy, "No NrSpectrumPhy pointer");
      Ptr<SlHarqTestErrorModel> slHarqTestErrorModel = CreateObject<SlHarqTestErrorModel> ();
      slHarqTestErrorModel->SetTblerVector (m_tblerVector);
      sPhy->SetSlErrorModel (slHarqTestErrorModel);
    }

  LteRrcSap::SlResourcePoolNr slResourcePoolNr;
  Ptr<NrSlCommResourcePoolFactory> ptrFactory = Create<NrSlCommResourcePoolFactory> ();
  ptrFactory->SetSlTimeResources (m_slBitmap);
  ptrFactory->SetSlSensingWindow (100); // T0 in ms
  ptrFactory->SetSlSelectionWindow (5);
  ptrFactory->SetSlFreqResourcePscch (10); // PSCCH RBs
  ptrFactory->SetSlSubchannelSize (m_slSubchannelSize);
  ptrFactory->SetSlMaxNumPerReserve (3);
  ptrFactory->SetSlPsfchPeriod (m_psfchPeriod);
  ptrFactory->SetSlMinTimeGapPsfch (3);
  std::list<uint16_t> resourceReservePeriodList = {0, 100}; // in ms
  ptrFactory->SetSlResourceReservePeriodList (resourceReservePeriodList);
  //Once parameters are configured, we can create the pool
  LteRrcSap::SlResourcePoolNr pool = ptrFactory->CreatePool ();
  slResourcePoolNr = pool;
 
  LteRrcSap::SlResourcePoolConfigNr slresoPoolConfigNr;
  slresoPoolConfigNr.haveSlResourcePoolConfigNr = true;
  //Pool id, ranges from 0 to 15
  uint16_t poolId = 0;
  LteRrcSap::SlResourcePoolIdNr slResourcePoolIdNr;
  slResourcePoolIdNr.id = poolId;
  slresoPoolConfigNr.slResourcePoolId = slResourcePoolIdNr;
  slresoPoolConfigNr.slResourcePool = slResourcePoolNr;

  LteRrcSap::SlBwpPoolConfigCommonNr slBwpPoolConfigCommonNr;
  slBwpPoolConfigCommonNr.slTxPoolSelectedNormal [slResourcePoolIdNr.id] = slresoPoolConfigNr;

  LteRrcSap::Bwp bwp;
  bwp.numerology = m_numerologyBwpSl;
  bwp.symbolsPerSlots = 14;
  bwp.rbPerRbg = 1;
  bwp.bandwidth = bandwidthBandSl;

  //Configure the SlBwpGeneric IE
  LteRrcSap::SlBwpGeneric slBwpGeneric;
  slBwpGeneric.bwp = bwp;
  slBwpGeneric.slLengthSymbols = LteRrcSap::GetSlLengthSymbolsEnum (14);
  slBwpGeneric.slStartSymbol = LteRrcSap::GetSlStartSymbolEnum (0);

  //Configure the SlBwpConfigCommonNr IE
  LteRrcSap::SlBwpConfigCommonNr slBwpConfigCommonNr;
  slBwpConfigCommonNr.haveSlBwpGeneric = true;
  slBwpConfigCommonNr.slBwpGeneric = slBwpGeneric;
  slBwpConfigCommonNr.haveSlBwpPoolConfigCommonNr = true;
  slBwpConfigCommonNr.slBwpPoolConfigCommonNr = slBwpPoolConfigCommonNr;

  //Configure the SlFreqConfigCommonNr IE, which hold the array to store
  //the configuration of all Sidelink BWP (s).
  LteRrcSap::SlFreqConfigCommonNr slFreConfigCommonNr;
  //Array for BWPs. Here we will iterate over the BWPs, which
  //we want to use for SL.
  for (const auto &it:bwpIdContainer)
    {
      // it is the BWP id
      slFreConfigCommonNr.slBwpList [it] = slBwpConfigCommonNr;
    }

  LteRrcSap::TddUlDlConfigCommon tddUlDlConfigCommon;
  tddUlDlConfigCommon.tddPattern = m_tddPattern;

  LteRrcSap::SlPreconfigGeneralNr slPreconfigGeneralNr;
  slPreconfigGeneralNr.slTddConfig = tddUlDlConfigCommon;

  LteRrcSap::SlUeSelectedConfig slUeSelectedPreConfig;
  slUeSelectedPreConfig.slProbResourceKeep = 0;
  //Configure the SlPsschTxParameters IE
  LteRrcSap::SlPsschTxParameters psschParams;
  psschParams.slMaxTxTransNumPssch = 5;
  //Configure the SlPsschTxConfigList IE
  LteRrcSap::SlPsschTxConfigList pscchTxConfigList;
  pscchTxConfigList.slPsschTxParameters [0] = psschParams;
  slUeSelectedPreConfig.slPsschTxConfigList = pscchTxConfigList;

  LteRrcSap::SidelinkPreconfigNr slPreConfigNr;
  slPreConfigNr.slPreconfigGeneral = slPreconfigGeneralNr;
  slPreConfigNr.slUeSelectedPreConfig = slUeSelectedPreConfig;
  slPreConfigNr.slPreconfigFreqInfoList [0] = slFreConfigCommonNr;

  //Communicate the above pre-configuration to the NrSlHelper
  nrSlHelper->InstallNrSlPreConfiguration (ueNetDevices, slPreConfigNr);

  /*
   * Fix the random streams
   */
  int64_t stream = 1;
  stream += nrHelper->AssignStreams (ueNetDevices, stream);
  stream += nrSlHelper->AssignStreams (ueNetDevices, stream);

  /*
   * Configure the IP stack, and activate NR sidelink bearer (s)
   *
   * This example supports IPV4 and IPV6
   */

  InternetStackHelper internet;
  internet.Install (ueContainer);
  stream += internet.AssignStreams (ueContainer, stream);
  uint32_t dstL2Id = 0;
  Ipv4Address groupAddress4 ("225.0.0.0");     //use multicast address as destination
  Ipv6Address groupAddress6 ("ff0e::1");     //use multicast address as destination
  Address remoteAddress;
  Address localAddress;
  uint16_t port = 8000;

  ConfigureTfts (nrSlHelper, epcHelper, ueContainer, ueNetDevices, groupAddress4, groupAddress6,
    port, dstL2Id, localAddress, remoteAddress, delayBudget, finalSlBearersActivationTime);

  ConfigureApplications (remoteAddress, localAddress, groupAddress6, ueContainer, finalSlBearersActivationTime, finalSimTime);

  for (uint32_t i = 0; i < ueNetDevices.GetN (); i++)
    {
      Ptr<NrUeMac> nrUeMac = nrHelper->GetUeMac (ueNetDevices.Get (i), bwpIdForGbrMcptt);
      PointerValue p;
      nrUeMac->GetAttribute ("NrSlUeMacHarq", p);
      Ptr<NrSlUeMacHarq> harq = p.Get<NrSlUeMacHarq> ();
      harq->TraceConnect ("RxHarqFeedback", std::to_string (ueNetDevices.Get (i)->GetNode ()->GetId ()), MakeCallback (&TestSidelinkHarq::ReceiveHarqFeedback, this));
      harq->TraceConnect ("Allocate", std::to_string (ueNetDevices.Get (i)->GetNode ()->GetId ()), MakeCallback (&TestSidelinkHarq::HarqAllocate, this));
      harq->TraceConnect ("Deallocate", std::to_string (ueNetDevices.Get (i)->GetNode ()->GetId ()), MakeCallback (&TestSidelinkHarq::HarqDeallocate, this));
      harq->TraceConnect ("RequestPacketBurst", std::to_string (ueNetDevices.Get (i)->GetNode ()->GetId ()), MakeCallback (&TestSidelinkHarq::HarqPacketBurst, this));
      harq->TraceConnect ("Timeout", std::to_string (ueNetDevices.Get (i)->GetNode ()->GetId ()), MakeCallback (&TestSidelinkHarq::HarqTimeout, this));
    }

  for (uint32_t i = 0; i < ueNetDevices.GetN (); i++)
    {
      Ptr<NrUePhy> nrUePhy = nrHelper->GetUePhy (ueNetDevices.Get (i), bwpIdForGbrMcptt);
      Ptr<NrSpectrumPhy> spectrumPhy = nrUePhy->GetSpectrumPhy (0);
      spectrumPhy->TraceConnect ("RxDataTrace", std::to_string (ueNetDevices.Get (i)->GetNode ()->GetId ()), MakeCallback (&TestSidelinkHarq::ReceiveSlDataFrame, this));
      spectrumPhy->TraceConnect ("SlPscchDecodeFailure", std::to_string (ueNetDevices.Get (i)->GetNode ()->GetId ()), MakeCallback (&TestSidelinkHarq::TraceSlPscchDecodeFailure, this));
      spectrumPhy->TraceConnect ("SlSci2aDecodeFailure", std::to_string (ueNetDevices.Get (i)->GetNode ()->GetId ()), MakeCallback (&TestSidelinkHarq::TraceSlSci2aDecodeFailure, this));
      spectrumPhy->TraceConnect ("SlTbDecodeFailure", std::to_string (ueNetDevices.Get (i)->GetNode ()->GetId ()), MakeCallback (&TestSidelinkHarq::TraceSlTbDecodeFailure, this));
    }

  Simulator::Stop (finalSimTime);
  Simulator::Run ();
  Simulator::Destroy ();

  if (m_checkAppRxCount)
    {
      NS_TEST_ASSERT_MSG_EQ (m_appRxCount, m_expectedAppRxCount, "Did not receive expected number of packets");
    }
  if (m_checkHarqRxCount)
    {
      NS_TEST_ASSERT_MSG_EQ (m_harqAckCount, m_expectedHarqAckCount, "Did not receive expected number of HARQ ACKs");
    }
  if (m_checkHarqNackCount)
    {
      NS_TEST_ASSERT_MSG_EQ (m_harqNackCount, m_expectedHarqNackCount, "Did not receive expected number of HARQ NACKs");
    }
  if (m_checkHarqAllocateCount)
    {
      NS_TEST_ASSERT_MSG_EQ (m_harqAllocateCount, m_expectedHarqAllocateCount, "Did not receive expected number of HARQ allocations");
    }
  if (m_checkHarqDeallocateCount)
    {
      NS_TEST_ASSERT_MSG_EQ (m_harqDeallocateCount, m_expectedHarqDeallocateCount, "Did not receive expected number of HARQ deallocations");
    }
  if (m_checkHarqPacketBurstCount)
    {
      NS_TEST_ASSERT_MSG_EQ (m_harqPacketBurstCount, m_expectedHarqPacketBurstCount, "Did not receive expected number of HARQ packet bursts");
    }
  if (m_checkHarqTimeoutCount)
    {
      NS_TEST_ASSERT_MSG_EQ (m_harqTimeoutCount, m_expectedHarqTimeoutCount, "Did not receive expected number of HARQ timeouts");
    }
  if (m_checkRxSlDataFrameCount)
    {
      NS_TEST_ASSERT_MSG_EQ (m_rxSlDataFrameCount, m_expectedRxSlDataFrameCount, "Did not receive expected number of SL data frames");
    }
  if (m_checkPscchDecodeFailure)
    {
      NS_TEST_ASSERT_MSG_EQ (m_pscchDecodeFailureCount, m_expectedPscchDecodeFailureCount, "Did not receive expected number of PSCCH (SCI 1) decode failures");
    }
  if (m_checkSci2aDecodeFailure)
    {
      NS_TEST_ASSERT_MSG_EQ (m_sci2aDecodeFailureCount, m_expectedSci2aDecodeFailureCount, "Did not receive expected number of SCI-2A decode failures");
    }
  if (m_checkTbDecodeFailure)
    {
      NS_TEST_ASSERT_MSG_EQ (m_tbDecodeFailureCount, m_expectedTbDecodeFailureCount, "Did not receive expected number of transport block decode failures");
    }
}

class TestSidelinkHarqTwoSenders : public TestSidelinkHarq
{
public:
  TestSidelinkHarqTwoSenders (const std::string &name)
    : TestSidelinkHarq (name)
  {}

protected:
  // Documented in TestSidelinkHarq
  virtual void ConfigureTfts (Ptr<NrSlHelper> nrSlHelper, Ptr<NrPointToPointEpcHelper> epcHelper,
    const NodeContainer& ueContainer, const NetDeviceContainer& ueNetDevices,
    Ipv4Address groupAddress4, Ipv6Address groupAddress6, uint16_t port, uint32_t dstL2Id,
    Address& localAddress, Address& remoteAddress, Time delayBudget, Time finalSlBearersActivationTime) override;
  virtual void ConfigureApplications (Address remoteAddress, Address localAddress, Ipv6Address groupAddress6,
    const NodeContainer& ueContainer, Time finalSlBearersActivationTime, Time finalSimTime) override;
};

void
TestSidelinkHarqTwoSenders::ConfigureTfts (Ptr<NrSlHelper> nrSlHelper, Ptr<NrPointToPointEpcHelper> epcHelper,
    const NodeContainer& ueContainer, const NetDeviceContainer& ueNetDevices,
    Ipv4Address groupAddress4, Ipv6Address groupAddress6, uint16_t port, uint32_t dstL2Id,
    Address& localAddress, Address& remoteAddress, Time delayBudget, Time finalSlBearersActivationTime)
{
  NS_LOG_FUNCTION (this);
  NS_ABORT_MSG_IF (ueContainer.GetN () < 3, "Must be at least 3 UEs");
  Ptr<LteSlTft> transmitTft;
  Ptr<LteSlTft> receiveTft;
  SidelinkInfo slInfo;
  slInfo.m_castType = ConvertToEnum (m_castType);
  slInfo.m_dstL2Id = dstL2Id;
  slInfo.m_rri = MilliSeconds (100);
  slInfo.m_harqEnabled = m_harqEnabled;
  slInfo.m_pdb = delayBudget;
  if (!m_useIpv6)
    {
      Ipv4InterfaceContainer ueIpIface;
      ueIpIface = epcHelper->AssignUeIpv4Address (ueNetDevices);
      // set the default gateway for the UE
      Ipv4StaticRoutingHelper ipv4RoutingHelper;
      for (uint32_t u = 0; u < ueContainer.GetN (); ++u)
        {
          Ptr<Node> ueNode = ueContainer.Get (u);
          // Set the default gateway for the UE
          Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
          ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
        }
      localAddress = InetSocketAddress (Ipv4Address::GetAny (), port);
      if (m_castType == "groupcast" || m_castType == "broadcast")
        {
          slInfo.m_dstL2Id = 224;
          remoteAddress = InetSocketAddress (groupAddress4, port);
          // Two senders in this test case
          NetDeviceContainer transmitDevices;
          transmitDevices.Add (ueNetDevices.Get (0));
          transmitDevices.Add (ueNetDevices.Get (2));
          transmitTft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT, groupAddress4, slInfo);
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, transmitDevices, transmitTft);
          NetDeviceContainer receiveDevices;
          for (uint32_t u = 0; u < ueContainer.GetN (); ++u)
            {
              receiveDevices.Add (ueNetDevices.Get (u));
            }
          receiveTft = Create<LteSlTft> (LteSlTft::Direction::RECEIVE, groupAddress4, slInfo);
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, receiveDevices, receiveTft);
        }
      else
        {
          slInfo.m_dstL2Id = 2;  // RNTI of node 1
          remoteAddress = InetSocketAddress (ueIpIface.GetAddress (1, 0), port);
          transmitTft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT, ueIpIface.GetAddress (1,0), slInfo);
          NetDeviceContainer transmit;
          transmit.Add (ueNetDevices.Get (0));
          transmit.Add (ueNetDevices.Get (2));
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, transmit, transmitTft);
          receiveTft = Create<LteSlTft> (LteSlTft::Direction::RECEIVE, ueIpIface.GetAddress (1,0), slInfo);
          NetDeviceContainer receive;
          receive.Add (ueNetDevices.Get (1));
          if (ueContainer.GetN () > 3)
            {
              for (uint32_t u = 3; u < ueContainer.GetN (); ++u)
                {
                  receive.Add (ueNetDevices.Get (u));
                }
            }
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, receive, receiveTft);
        }
    }
  else
    {
      Ipv6InterfaceContainer ueIpIface;
      ueIpIface = epcHelper->AssignUeIpv6Address (ueNetDevices);

      // set the default gateway for the UE
      Ipv6StaticRoutingHelper ipv6RoutingHelper;
      for (uint32_t u = 0; u < ueContainer.GetN (); ++u)
        {
          Ptr<Node> ueNode = ueContainer.Get (u);
          // Set the default gateway for the UE
          Ptr<Ipv6StaticRouting> ueStaticRouting = ipv6RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv6> ());
          ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress6 (), 1);
        }
      localAddress = Inet6SocketAddress (Ipv6Address::GetAny (), port);
      if (m_castType == "groupcast" || m_castType == "broadcast")
        {
          slInfo.m_dstL2Id = 224;
          remoteAddress = Inet6SocketAddress (groupAddress6, port);
          // Two senders in this test case
          NetDeviceContainer transmitDevices;
          transmitDevices.Add (ueNetDevices.Get (0));
          transmitDevices.Add (ueNetDevices.Get (2));
          transmitTft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT, groupAddress6, slInfo);
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, transmitDevices, transmitTft);
          NetDeviceContainer receiveDevices;
          for (uint32_t u = 0; u < ueContainer.GetN (); ++u)
            {
              receiveDevices.Add (ueNetDevices.Get (u));
            }
          receiveTft = Create<LteSlTft> (LteSlTft::Direction::RECEIVE, groupAddress6, slInfo);
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, receiveDevices, receiveTft);
        }
      else
        {
          slInfo.m_dstL2Id = 2;  // RNTI of node 1
          remoteAddress = Inet6SocketAddress (ueIpIface.GetAddress (1, 0), port);
          transmitTft = Create<LteSlTft> (LteSlTft::Direction::TRANSMIT, ueIpIface.GetAddress (1, 0), slInfo);
          NetDeviceContainer transmit;
          transmit.Add (ueNetDevices.Get (0));
          transmit.Add (ueNetDevices.Get (2));
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, transmit, transmitTft);
          receiveTft = Create<LteSlTft> (LteSlTft::Direction::RECEIVE, ueIpIface.GetAddress (1,0), slInfo);
          NetDeviceContainer receive;
          receive.Add (ueNetDevices.Get (1));
          if (ueContainer.GetN () > 3)
            {
              for (uint32_t u = 3; u < ueContainer.GetN (); ++u)
                {
                  receive.Add (ueNetDevices.Get (u));
                }
            }
          nrSlHelper->ActivateNrSlBearer (finalSlBearersActivationTime, receive, receiveTft);
        }
    }
}

void
TestSidelinkHarqTwoSenders::ConfigureApplications (Address remoteAddress, Address localAddress, Ipv6Address groupAddress6,
    const NodeContainer& ueContainer, Time finalSlBearersActivationTime, Time finalSimTime)
{
  NS_LOG_FUNCTION (this << ueContainer.GetN () << finalSlBearersActivationTime << finalSimTime);
  NS_ABORT_MSG_IF (ueContainer.GetN () < 3, "Must be at least 3 UEs");
  // Send packets to ueContainer.Get (1), from ueContainer.Get (0) and
  // ueContainer.Get (2)
  OnOffHelper sidelinkClient ("ns3::UdpSocketFactory", remoteAddress);
  sidelinkClient.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
  sidelinkClient.SetAttribute ("MaxBytes", UintegerValue (m_numPackets * m_udpPacketSize));
  sidelinkClient.SetConstantRate (m_dataRate, m_udpPacketSize);
  NodeContainer clientNodes;
  clientNodes.Add (ueContainer.Get (0));
  clientNodes.Add (ueContainer.Get (2));

  ApplicationContainer clientApps = sidelinkClient.Install (clientNodes);
  //onoff application will send the first packet at:
  NS_LOG_DEBUG ("Starting at " << finalSlBearersActivationTime.As (Time::S));
  clientApps.Start (finalSlBearersActivationTime);
  clientApps.Stop (finalSimTime);

  PacketSinkHelper sidelinkSink ("ns3::UdpSocketFactory", localAddress);
  sidelinkSink.SetAttribute ("EnableSeqTsSizeHeader", BooleanValue (true));
  NodeContainer sinks;
  for (uint32_t u = 0; u < ueContainer.GetN (); ++u)
    {
      sinks.Add (ueContainer.Get (u));
    }
  ApplicationContainer serverApps = sidelinkSink.Install (sinks);
  serverApps.Start (Seconds (1.0));

  if (m_useIpv6)
    {
      for (uint16_t ac = 0; ac < clientApps.GetN (); ac++)
        {
          clientApps.Get (ac)->GetNode ()->GetObject<Ipv6L3Protocol> ()->AddMulticastAddress (groupAddress6);
        }

      // Set Rx traces
      for (uint16_t ac = 0; ac < serverApps.GetN (); ac++)
        {
          serverApps.Get (ac)->GetNode ()->GetObject<Ipv6L3Protocol> ()->AddMulticastAddress (groupAddress6);
        }
    }

  for (uint32_t i = 0; i < serverApps.GetN (); i++)
    {
      Ptr<PacketSink> sink = DynamicCast<PacketSink> (serverApps.Get (i));
      sink->TraceConnect ("RxWithSeqTsSize", std::to_string (serverApps.Get (i)->GetNode ()->GetId ()), 
        MakeCallback (&TestSidelinkHarq::ReceivePacket, this));
    }
}

class TestSidelinkHarqHasPsfch : public TestCase
{
public:
  TestSidelinkHarqHasPsfch (const std::string &name)
    : TestCase (name)
  {}

private:
  virtual void DoRun (void) override;
  Ptr<NrSlCommResourcePool> CreatePool (const std::string& tddPatternString,
    const std::vector <std::bitset<1> >& slBitmap, uint16_t psfchPeriod); 
};

Ptr<NrSlCommResourcePool>
TestSidelinkHarqHasPsfch::CreatePool (const std::string& tddPatternString,
  const std::vector <std::bitset<1> >& slBitmap, uint16_t psfchPeriod) 
{
  // There are three basic pieces of configuration passed in:
  // 1) the repeating TDD pattern of slots
  // 2) the repeating pattern of SL eligibility in UL slots
  // 3) the PSFCH period

  // Next, we need to plumb the above configuration into the objects that
  // are used in simulations.
  // The above method requires GetSlResourcePoolNr which relies on
  // m_slPreconfigFreqInfoList and its bwp list to be set
  Ptr<NrSlCommResourcePool> pool = CreateObject<NrSlCommResourcePool> ();
  // Configure the slBitmap, which is buried in a chain of IE elements
  LteRrcSap::SidelinkPreconfigNr slPreconfigNr;
  LteRrcSap::SlFreqConfigCommonNr slFreqConfigCommonNr;
  LteRrcSap::SlBwpConfigCommonNr slBwpConfigCommonNr;
  LteRrcSap::SlResourcePoolConfigNr slResourcePoolConfigNr;
  // Start configuring from the bottom up
  slResourcePoolConfigNr.slResourcePool.slTimeResource = slBitmap; // from above
  slResourcePoolConfigNr.slResourcePool.slPsfchConfig.slPsfchPeriod = 
    LteRrcSap::GetSlPsfchPeriodEnum (psfchPeriod);  // from above
  slResourcePoolConfigNr.slResourcePoolId.id = 0;
  slBwpConfigCommonNr.haveSlBwpPoolConfigCommonNr = true;
  slBwpConfigCommonNr.slBwpPoolConfigCommonNr.slTxPoolSelectedNormal [0] = slResourcePoolConfigNr;
  slFreqConfigCommonNr.slBwpList [0] = slBwpConfigCommonNr;
  slPreconfigNr.slPreconfigFreqInfoList [0] = slFreqConfigCommonNr;
  pool->SetNrSlPreConfigFreqInfoList (slPreconfigNr.slPreconfigFreqInfoList);

  // Next, configure the TDD pattern, first converting from string to a vector.
  std::vector<NrSlUeRrc::LteNrTddSlotType> tddPattern = NrSlUeRrc::ConvertTddPattern (tddPatternString);
  pool->SetTddPattern (tddPattern);

  // The call to SlotHasPsfch () relies on the NrSlPhysicalPoolMap being
  // configured.  We can construct it by hand here.
  std::unordered_map<uint8_t, std::unordered_map <uint16_t, std::vector <std::bitset<1>>> > mapPerBwp;
  std::unordered_map <uint16_t, std::vector <std::bitset<1>>> mapPerPool;
  std::vector < std::bitset<1> > phyPool = NrSlUeRrc::GetPhysicalSlPool (slBitmap, tddPattern);
  mapPerPool.emplace (std::make_pair (0, phyPool));
  mapPerBwp.emplace (std::make_pair (0, mapPerPool));
  pool->SetNrSlPhysicalPoolMap (mapPerBwp);
  return pool;
}

void
TestSidelinkHarqHasPsfch::DoRun ()
{
  // We want to be able to call the following method for different values of
  // absIndexCurrentSlot and check the return value:
  // bool NrSlCommResourcePool::SlotHasPsfch (uint64_t absIndexCurrentSlot,
  //     uint8_t bwpId, uint16_t poolId)

  // We'll only configure one BWP and one pool, so the indices will always
  // be zero, so only the absIndexCurrentSlot value will determine the
  // result of the test.

  // The repeating TDD pattern can be configured as a string here
  std::string tddPattern = "DL|DL|DL|F|UL|UL|UL|UL|UL|UL|";

  // The repeating pattern of SL eligibility is a vector of bits.  This
  // does not need to be the same periodicity as the TDD pattern 
  std::vector <std::bitset<1> > slBitmap = {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1};

  uint16_t psfchPeriod = 4;

  Ptr<NrSlCommResourcePool> pool;
  pool = CreatePool (tddPattern, slBitmap, psfchPeriod);
 
  // Check expected pattern for first repeating period
  // The following pattern is expected for the first configuration, over a
  // period of 80 slots.  The notation below shows the first 20 slots
  // (f)rame, (sf) subframe, (sl)ot, and (abs)olute slot number, 
  // whether DL, F, or UL, and whether a SL slot.  For SL slots, 0 means
  // no PSFCH, and P means PSFCH is present.   There are four columns to
  // represent the period after which the pattern repeats.  Threfore, in
  // the first 20 slots, the abs=7 slot has PSFCH, but abs=27, 47, 67
  // do not have PSFCH.  abs=6 does not have PSFCH, but abs=26 does, and
  // this is denoted as '0 P'.
  // 
  // slots with PSFCH:  7, 18, 26, 37, 45, 49, 64, 68, 79 
  // 
  // f 0 sf 0 sl 0 abs 0 DL
  // f 0 sf 0 sl 1 abs 1 DL
  // f 0 sf 0 sl 2 abs 2 DL
  // f 0 sf 0 sl 3 abs 3 F
  // f 0 sf 1 sl 0 abs 4 UL SL 0 0 0 P
  // f 0 sf 1 sl 1 abs 5 UL SL 0 0 P
  // f 0 sf 1 sl 2 abs 6 UL SL 0 P
  // f 0 sf 1 sl 3 abs 7 UL SL P
  // f 0 sf 2 sl 0 abs 8 UL SL 0 0 0 P
  // f 0 sf 2 sl 1 abs 9 UL SL 0 0 P
  // f 0 sf 2 sl 2 abs 10 DL
  // f 0 sf 2 sl 3 abs 11 DL
  // f 0 sf 3 sl 0 abs 12 DL
  // f 0 sf 3 sl 1 abs 13 F
  // f 0 sf 3 sl 2 abs 14 UL
  // f 0 sf 3 sl 3 abs 15 UL
  // f 0 sf 4 sl 0 abs 16 UL
  // f 0 sf 4 sl 1 abs 17 UL SL 0 P
  // f 0 sf 4 sl 2 abs 18 UL SL P
  // f 0 sf 4 sl 3 abs 19 UL SL 0 0 0 P

  // based on the above pattern, check some slots.
#define CHECK(a,b) NS_TEST_ASSERT_MSG_EQ (pool->SlotHasPsfch (a, 0, 0), bool (b), "SlotHasPsfch failure for " << a)

  CHECK (0, false);
  CHECK (7, true); // Absolute slot 7 has the first PSFCH
  CHECK (17, false);
  CHECK (18, true);
  CHECK (26, true);
  CHECK (37, true);
  CHECK (45, true);
  CHECK (49, true);
  CHECK (50, false);
  CHECK (51, false);
  CHECK (52, false);
  CHECK (53, false);
  CHECK (64, true);
  CHECK (65, false);
  CHECK (79, true);
  // Check next period:  80+18  = 98 should be true, 99 false
  CHECK (98, true);
  CHECK (99, false);
  // Move ahead in time to near the end of simulation time of 12 seconds
  CHECK (47644, false);
  CHECK (47645, true);
  CHECK (47646, false);
  CHECK (47647, false);

  // Repeat some tests for different PSFCH periods
  psfchPeriod = 2;
  pool = CreatePool (tddPattern, slBitmap, psfchPeriod);
  CHECK (0, false);
  CHECK (5, true);
  CHECK (6, false);
  CHECK (7, true);
  CHECK (17, false);
  CHECK (18, true);
  CHECK (45, true);
  CHECK (79, true);
  CHECK (98, true);
  // Move ahead in time to near the end of simulation time of 12 seconds
  CHECK (47644, false);
  CHECK (47645, true);
  CHECK (47646, false);
  CHECK (47647, true);

  psfchPeriod = 1;
  pool = CreatePool (tddPattern, slBitmap, psfchPeriod);
  CHECK (0, false); // DL slot
  CHECK (1, false); // DL slot
  CHECK (2, false); // DL slot
  CHECK (3, false); // F slots are not SL eligible
  CHECK (4, true);
  CHECK (5, true);
  CHECK (6, true);
  CHECK (7, true);
  CHECK (8, true);
  CHECK (9, true);
  CHECK (10, false);
  CHECK (11, false);
  CHECK (12, false);
  CHECK (13, false);
  CHECK (14, false);
  CHECK (15, false);
  CHECK (16, false);
  CHECK (17, true);
  CHECK (18, true);
  CHECK (19, true);
  CHECK (45, true);
  CHECK (79, true);
  CHECK (98, true);
  // Move ahead in time to near the end of simulation time of 12 seconds
  CHECK (47644, true);
  CHECK (47645, true);
  CHECK (47646, true);
  CHECK (47647, true);

  psfchPeriod = 0;
  pool = CreatePool (tddPattern, slBitmap, psfchPeriod);
  CHECK (0, false);
  CHECK (7, false);
  CHECK (17, false);
  CHECK (18, false);
  CHECK (45, false);
  CHECK (79, false);
  CHECK (98, false);
  // Move ahead in time to near the end of simulation time of 12 seconds
  CHECK (47644, false);
  CHECK (47645, false);
  CHECK (47646, false);
  CHECK (47647, false);

}

class TestSidelinkHarqSuite : public TestSuite
{
public:
  TestSidelinkHarqSuite () : TestSuite ("nr-test-sidelink-harq", UNIT)
  {
    // Step through basic parameter combinations from sidelink-harq-example.cc
    // No packet losses in this case
    TestSidelinkHarq* testCase;
    testCase = new TestSidelinkHarq ("broadcast 2 UE");
    testCase->SetCastType ("broadcast");
    testCase->SetNumUe (2);
    testCase->CheckAppRxCount (10); // 10 packets * 1 receivers
    testCase->CheckHarqAckCount (10); // 10 HARQ FB should be received
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("groupcast 2 UE");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (2);
    testCase->CheckAppRxCount (10); // 10 packets * 1 receivers
    testCase->CheckHarqAckCount (10); // 10 HARQ FB should be received
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("unicast 2 UE");
    testCase->SetCastType ("unicast");
    testCase->SetNumUe (2);
    testCase->CheckAppRxCount (10); // 10 packets * 1 receivers
    testCase->CheckHarqAckCount (10); // 10 HARQ FB should be received
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("broadcast 3 UE");
    testCase->SetCastType ("broadcast");
    testCase->SetNumUe (3);
    testCase->CheckAppRxCount (20); // 10 packets * 2 receivers
    testCase->CheckHarqAckCount (20); // 20 HARQ FB should be received
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("groupcast 3 UE");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (3);
    testCase->CheckAppRxCount (20); // 10 packets * 2 receivers
    testCase->CheckHarqAckCount (20); // 20 HARQ FB should be received
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("unicast 3 UE");
    testCase->SetCastType ("unicast");
    testCase->SetNumUe (3);
    testCase->CheckAppRxCount (10); // 10 packets * 1 receivers
    testCase->CheckHarqAckCount (10); // 10 HARQ FB should be received
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("broadcast 4 UE");
    testCase->SetCastType ("broadcast");
    testCase->SetNumUe (4);
    testCase->CheckAppRxCount (30); // 10 packets * 3 receivers
    testCase->CheckHarqAckCount (30); // 10 HARQ FB * 3 receivers
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("groupcast 4 UE");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (4);
    testCase->CheckAppRxCount (30); // 10 packets * 3 receivers
    testCase->CheckHarqAckCount (30); // 10 HARQ FB * 3 receivers
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("unicast 4 UE");
    testCase->SetCastType ("unicast");
    testCase->SetNumUe (4);
    testCase->CheckAppRxCount (10); // 10 packets * 1 receivers
    testCase->CheckHarqAckCount (10); // 10 HARQ FB * 1 receivers
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("groupcast 5 UE");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (5);
    testCase->CheckAppRxCount (40); // 10 packets * 4 receivers
    testCase->CheckHarqAckCount (40); // 10 HARQ FB * 4 receivers
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("groupcast 9 UE");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (9);
    testCase->CheckAppRxCount (80); // 10 packets * 8 receivers
    testCase->CheckHarqAckCount (80); // 10 HARQ FB * 8 receivers
    AddTestCase (testCase);

    // HARQ disabled should lead to no HARQ feedback even if PSFCH configured
    testCase = new TestSidelinkHarq ("groupcast 3 UE HARQ disabled");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (3);
    testCase->SetHarqEnabled (false);
    testCase->CheckAppRxCount (20); // 10 packets * 2 receivers
    testCase->CheckHarqAckCount (0); // HARQ disabled
    AddTestCase (testCase);

    // HARQ enabled but PSFCH period of zero should lead to no HARQ feedback
    testCase = new TestSidelinkHarq ("groupcast 3 UE PSFCH period 0");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (3);
    testCase->SetPsfchPeriod (0);
    testCase->CheckAppRxCount (20); // 10 packets * 2 receivers
    testCase->CheckHarqAckCount (0); // HARQ disabled by lack of PSFCH
    AddTestCase (testCase);

    // Alternative TDD and SL bitmap patterns
    // See Sphinx documentation about constraints:
    // 1) slBitmap size should be multiple of number of UL slots in TDD pattern
    // 2) resource reservation period (in slots) must be multiple of resultant
    //    phyPool size
    std::vector <std::bitset<1> > slBitmap;
    testCase = new TestSidelinkHarq ("alternative TDD and SL pattern 1");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (3);
    testCase->SetTddPattern (std::string ("DL|DL|DL|F|UL|UL|UL|UL|UL|UL|"));
    slBitmap = {1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1};
    // phyPool.size () will be 20 in the above; reservation period slots = 400
    testCase->SetSlBitmap (slBitmap);
    testCase->CheckAppRxCount (20); // 10 packets * 2 receivers
    testCase->CheckHarqAckCount (20); // 20 HARQ FB should be received
    AddTestCase (testCase);

    // Alternative TDD and SL bitmap patterns
    testCase = new TestSidelinkHarq ("alternative TDD and SL pattern 2");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (3);
    testCase->SetTddPattern (std::string ("UL|UL|F|F|DL|DL|UL|DL|DL|DL|"));
    slBitmap = {1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0};
    // phyPool.size () will be 40 in the above; reservation period slots = 400
    testCase->SetSlBitmap (slBitmap);
    testCase->CheckAppRxCount (20); // 10 packets * 2 receivers
    testCase->CheckHarqAckCount (20); // 20 HARQ FB should be received
    AddTestCase (testCase);

    // Test blind retransmission
    testCase = new TestSidelinkHarq ("blind retransmission groupcast");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (3);
    testCase->SetPsfchPeriod (0);
    testCase->CheckAppRxCount (20); // 10 packets * 2 receivers
    testCase->CheckRxSlDataFrameCount (100); // 10 packets * 2 receivers * 5 transmissions
    testCase->CheckHarqAckCount (0); // no HARQ FB should be received
    AddTestCase (testCase);

    // Test behavior for PSSCH loss without HARQ
    testCase = new TestSidelinkHarq ("PSCCH loss without HARQ");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (2);
    testCase->SetNumPackets (1);
    testCase->CheckAppRxCount (1); // 1 packet * 1 receiver
    testCase->CheckRxSlDataFrameCount (1); // 1 packet * 1 receiver * 1 transmissions
    testCase->CheckHarqAckCount (1); // no HARQ FB should be received
    AddTestCase (testCase);

    // Check numerologies 0, 1, 3, 4 (2 is default numerology and is tested
    // above).  T2 and SlSubchannelSize vary with numerology.
    testCase = new TestSidelinkHarq ("numerology 0");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (3);
    testCase->SetNumerology (0);
    testCase->SetT2 (10);
    testCase->CheckAppRxCount (20); // 10 packets * 2 receivers
    testCase->CheckHarqAckCount (20); // 20 HARQ FB should be received
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("numerology 1");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (3);
    testCase->SetNumerology (1);
    testCase->SetT2 (17);
    testCase->CheckAppRxCount (20); // 10 packets * 2 receivers
    testCase->CheckHarqAckCount (20); // 20 HARQ FB should be received
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("numerology 3");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (3);
    testCase->SetNumerology (3);
    testCase->SetSlSubchannelSize (25);
    testCase->SetT2 (50);
    testCase->CheckAppRxCount (20); // 10 packets * 2 receivers
    testCase->CheckHarqAckCount (20); // 20 HARQ FB should be received
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("numerology 4");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (3);
    testCase->SetNumerology (4);
    testCase->SetSlSubchannelSize (10);
    testCase->SetT2 (100);
    testCase->CheckAppRxCount (20); // 10 packets * 2 receivers
    testCase->CheckHarqAckCount (20); // 20 HARQ FB should be received
    AddTestCase (testCase);

    // Test various loss conditions.  First test is a no loss (control) case
    testCase = new TestSidelinkHarq ("no loss");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (2);
    testCase->SetNumPackets (1);
    testCase->CheckHarqAllocateCount (1); // 1 initial transmission
    testCase->CheckHarqPacketBurstCount (1); // One transmission, no HARQ retransmissions needed
    testCase->CheckPscchDecodeFailure (0);
    testCase->CheckSci2aDecodeFailure (0);
    testCase->CheckTbDecodeFailure (0);
    testCase->CheckHarqTimeoutCount (0);
    testCase->CheckHarqAckCount (1); // Received positive HARQ feedback
    testCase->CheckHarqDeallocateCount (1); // 1 HARQ deallocation observed
    testCase->CheckHarqNackCount (0);
    testCase->CheckAppRxCount (1); // Application receives data
    AddTestCase (testCase);

    // Losses can be forced by setting the TBLER to 1 for NrSpectrumPhy
    // receptions for selected packets.  In these initial simple first cases,
    // the first reception is PSCCH, the second is the 2nd-stage SCI header,
    // and the third is the TB.

    // Loss of SCI 1 should result in no HARQ feedback
    testCase = new TestSidelinkHarq ("loss of SCI 1");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (2);
    testCase->SetNumPackets (1);
    // Distance is so large that the SCI-1 will fail to decode
    testCase->SetDistance (10000);
    // The SCI 2A and transport block will not even be attempted to decode
    testCase->CheckHarqAllocateCount (1); // 1 initial transmission
    testCase->CheckHarqPacketBurstCount (2); // One transmission, one HARQ retransmission
    testCase->CheckPscchDecodeFailure (1); // 1 PSCCH transmission failure
    testCase->CheckSci2aDecodeFailure (0);
    testCase->CheckTbDecodeFailure (0);
    testCase->CheckHarqTimeoutCount (1); // 1 timeout observed
    testCase->CheckHarqDeallocateCount (1); // 1 HARQ deallocation observed
    testCase->CheckHarqAckCount (0);
    testCase->CheckHarqNackCount (0);
    testCase->CheckAppRxCount (0);
    AddTestCase (testCase);

    // Loss of 2nd stage SCI should result in no HARQ feedback
    testCase = new TestSidelinkHarq ("loss of 2nd stage SCI");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (2);
    testCase->SetNumPackets (1);
    // Manual override of TB error rate (PSCCH, SCI-2A, SCI-2A)
    // There is no attempted decode of TB because SCI-2A is lost twice
    testCase->SetTblerVector ({0, 1, 1});
    // The key here is that 2 SCI-2A failures are observed and no HARQ
    // feedback is generated, because there is no decoded process ID
    testCase->CheckHarqAllocateCount (1); // 1 initial transmission
    testCase->CheckHarqPacketBurstCount (2); // One transmission, one HARQ retransmission due to no ACK
    testCase->CheckPscchDecodeFailure (0); // 0 PSCCH transmission failure
    testCase->CheckSci2aDecodeFailure (2);
    testCase->CheckTbDecodeFailure (0);
    testCase->CheckHarqTimeoutCount (1); // 1 timeout observed
    testCase->CheckHarqDeallocateCount (1); // 1 HARQ deallocation observed
    testCase->CheckHarqAckCount (0);
    testCase->CheckHarqNackCount (0);
    testCase->CheckAppRxCount (0);
    AddTestCase (testCase);

    // Loss of both TB should result in HARQ feedback and timeout
    testCase = new TestSidelinkHarq ("loss of both TB");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (2);
    testCase->SetNumPackets (1);
    // Manual override of TB error rate (PSCCH, SCI-2A, TB, SCI-2A, TB)
    // There is no attempted decode of TB because SCI-2A is lost twice
    testCase->SetTblerVector ({0, 0, 1, 0, 1});
    // The key here is that 2 TB failures are observed and HARQ NACK
    // feedback is generated, and HARQ timeout occurs
    testCase->CheckHarqAllocateCount (1); // 1 initial transmission
    testCase->CheckHarqPacketBurstCount (2); // One transmission, one HARQ retransmission
    testCase->CheckPscchDecodeFailure (0); // 0 PSCCH transmission failure
    testCase->CheckSci2aDecodeFailure (0);
    testCase->CheckTbDecodeFailure (2);
    testCase->CheckHarqTimeoutCount (1); // 1 timeout observed
    testCase->CheckHarqDeallocateCount (1); // 1 HARQ deallocation observed
    testCase->CheckHarqAckCount (0);
    testCase->CheckHarqNackCount (2);
    testCase->CheckAppRxCount (0);
    AddTestCase (testCase);

    // Loss of first TB should result in HARQ NACK, but success of second
    // TB should cause a HARQ ACK, and no timeout, and the app should receive
    // the packet.
    testCase = new TestSidelinkHarq ("loss of first TB");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (2);
    testCase->SetNumPackets (1);
    // Manual override of TB error rate (PSCCH, SCI-2A, TB, SCI-2A, TB)
    testCase->SetTblerVector ({0, 0, 1, 0, 0});
    // The key here is that the first TB failure causes HARQ NACK, but
    // second one causes HARQ ACK and no timeout
    testCase->CheckHarqAllocateCount (1); // 1 initial transmission
    testCase->CheckHarqPacketBurstCount (2); // One transmission, one HARQ retransmission
    testCase->CheckPscchDecodeFailure (0); // 0 PSCCH transmission failure
    testCase->CheckSci2aDecodeFailure (0);
    testCase->CheckTbDecodeFailure (1);
    testCase->CheckHarqTimeoutCount (0);
    testCase->CheckHarqDeallocateCount (1); // 1 HARQ deallocation observed
    testCase->CheckHarqAckCount (1);   // second TB reception leads to ACK
    testCase->CheckHarqNackCount (1);  // first TB loss leads to NACK
    testCase->CheckAppRxCount (1);  // Packet is delivered
    AddTestCase (testCase);

    // Test IPv4 in three simple cases
    testCase = new TestSidelinkHarq ("broadcast 2 UE IPv4");
    testCase->SetCastType ("broadcast");
    testCase->SetNumUe (2);
    testCase->SetIpv6 (false);
    testCase->CheckAppRxCount (10); // 10 packets * 1 receivers
    testCase->CheckHarqAckCount (10); // 10 HARQ FB should be received
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("groupcast 2 UE IPv4");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (2);
    testCase->SetIpv6 (false);
    testCase->CheckAppRxCount (10); // 10 packets * 1 receivers
    testCase->CheckHarqAckCount (10); // 10 HARQ FB should be received
    AddTestCase (testCase);

    testCase = new TestSidelinkHarq ("unicast 2 UE IPv4");
    testCase->SetCastType ("unicast");
    testCase->SetNumUe (2);
    testCase->SetIpv6 (false);
    testCase->CheckAppRxCount (10); // 10 packets * 1 receivers
    testCase->CheckHarqAckCount (10); // 10 HARQ FB should be received
    AddTestCase (testCase);

    // Two senders send to same UE at same time.  Depending on the
    // scheduler and random variable run number, they may pick the same
    // resource for their transmissions, causing a collision reception
    // failure.  However, PSFCH should not collide, and HARQ should recover
    // failures unless collisions persist on the retransmission.
    // Some values for the below may fail to pass if RngRun is different.
    // For instance, if 1st stage SCI collides, one of the two UEs
    // will likely fail to deliver a packet because loss of SCI-1 will
    // cause loss of associated TBs
    testCase = new TestSidelinkHarqTwoSenders ("unicast 3 UE two senders");
    testCase->SetCastType ("unicast");
    testCase->SetNumUe (3);
    testCase->SetNumPackets (1);
    testCase->CheckHarqAllocateCount (2); // two senders
    // Each SL data frame will have two receivers at this level of tracing
    testCase->CheckRxSlDataFrameCount (4); //  two senders * two receivers
    testCase->CheckAppRxCount (2); // 2 packets * 1 receivers
    testCase->CheckHarqAckCount (2); // 2 HARQ ACK should be received
    AddTestCase (testCase);

    // Repeat with groupcast
    testCase = new TestSidelinkHarqTwoSenders ("groupcast 3 UE two senders");
    testCase->SetCastType ("groupcast");
    testCase->SetNumUe (3);
    testCase->SetNumPackets (1);
    testCase->CheckHarqAllocateCount (2); // two senders
    // Each SL data frame will have two receivers at this level of tracing
    // This test can have two outcomes.  If the two senders cause feedback
    // to be generated at the same time, all attempts to receive PSFCH
    // will fail due to half-duplex considerations, and there will be 5
    // retransmissions for a total of 10 transmissions.  If the feedback
    // is generated at different times, 4 received frames should be observed.
    // testCase->CheckRxSlDataFrameCount (4); //  two senders * two receivers
    testCase->CheckAppRxCount (4); // 2 packets * 2 receivers
    // This test can have two outcomes.  If the two senders cause feedback
    // to be generated at the same time, all attempts to receive PSFCH
    // will fail due to half-duplex considerations.  If the feedback is
    // generated at different times, 4 HARQ ACKs should be received
    // testCase->CheckHarqAckCount (4); // 4 HARQ ACK should be received
    AddTestCase (testCase);

    // Repeat with broadcast
    testCase = new TestSidelinkHarqTwoSenders ("broadcast 3 UE two senders");
    testCase->SetCastType ("broadcast");
    testCase->SetNumUe (3);
    testCase->SetNumPackets (1);
    testCase->CheckHarqAllocateCount (2); // two senders
    // Each SL data frame will have two receivers at this level of tracing
    // This test can have two outcomes.  If the two senders cause feedback
    // to be generated at the same time, all attempts to receive PSFCH
    // will fail due to half-duplex considerations, and there will be 5
    // retransmissions for a total of 10 transmissions.  If the feedback
    // is generated at different times, 4 received frames should be observed.
    // testCase->CheckRxSlDataFrameCount (4); //  two senders * two receivers
    testCase->CheckAppRxCount (4); // 2 packets * 2 receivers
    // This test can have two outcomes.  If the two senders cause feedback
    // to be generated at the same time, all attempts to receive PSFCH
    // will fail due to half-duplex considerations.  If the feedback is
    // generated at different times, 4 HARQ ACKs should be received
    // testCase->CheckHarqAckCount (4); // 4 HARQ ACK should be received
    AddTestCase (testCase);

    // One final specialized test of PSFCH assignment to SL slots
    AddTestCase (new TestSidelinkHarqHasPsfch ("PSFCH assignment to SL slots"), QUICK);
  }
};

static TestSidelinkHarqSuite testSidelinkHarqSuite; //!< sidelink HARQ test suite

}  // namespace test
}  // namespace ns3

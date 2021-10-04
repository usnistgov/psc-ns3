/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 *
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/psc-module.h"
#include "ns3/random-variable-stream.h"

// Example of a PSC Application.
//
// In this example, the client is configured with ON session durations 
// of 10 s with 5 s between sessions. In each session, the client will
// send 10 requests of 50 Bytes at 1 s interval. For each request, the 
// server will send a response of 20 Bytes.  
// The server can be set to not send a response back to the source client
// with the echoClient parameter set to false.
// Other parameters include:
// - verbose: enables log (requires compilation in debug mode)
// - enableTraces: enables a trace file capturing packets sent and received
//                 by the client and server
// - time: the simulation time

// Network Topology 
//
// n0 ==================  n1  
//    LAN 6001:db80::/64


using namespace ns3;
using namespace psc;

NS_LOG_COMPONENT_DEFINE ("PscApplicationExample");

/**
 * \brief Function to trace packet transmissions
 * 
 * \param streamWrapper The file stream wripper
 * \param appName The name of the application 
 * \param header The header information
 */
void
TraceAppTxTraffic (Ptr<OutputStreamWrapper> streamWrapper, std::string appName, SeqTsSizeHeader header)
{
  *streamWrapper->GetStream () << appName <<
    " " << Simulator::Now ().GetSeconds () <<
    " TX " << header.GetSize () <<
    " " << header.GetSeq () <<  std::endl;}

/**
 * \brief Function to trace packet reception
 * 
 * \param streamWrapper The file stream wripper
 * \param appName The name of the application 
 * \param header The header information
 */
void
TraceAppRxTraffic (Ptr<OutputStreamWrapper> streamWrapper, std::string appName, SeqTsSizeHeader header)
{
  *streamWrapper->GetStream () << appName <<
    " " << Simulator::Now ().GetSeconds () <<
    " RX " << header.GetSize () <<
    " " << header.GetSeq () << std::endl;
}


int
main (int argc, char *argv[])
{
  bool verbose = true;
  bool enableTraces = false;
  bool echoClient = true;
  uint32_t nCsma = 2;
  double startTime = 2;
  double simTime = 30;

  Ptr<OutputStreamWrapper> appTrafficOutputStreamWrapper;

  CommandLine cmd;
  cmd.AddValue ("echoClient", "Set EchoClient attribute", echoClient);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("enableTraces", "Enable trace file output", enableTraces);
  cmd.AddValue ("time", "Simulation time", simTime);

  cmd.Parse (argc,argv);

  if (verbose)
    {
      LogComponentEnable ("PscApplicationExample", LOG_LEVEL_ALL);
      LogComponentEnable ("PscApplication", LOG_LEVEL_ALL);
      LogComponentEnable ("PscApplicationClient", LOG_LEVEL_ALL);
      LogComponentEnable ("PscApplicationServer", LOG_LEVEL_ALL);
      LogComponentEnable ("PscApplicationConfiguration", LOG_LEVEL_ALL);
      LogComponentEnable ("PscApplicationHelper", LOG_LEVEL_ALL);
    }

  NS_LOG_INFO ("CSMA nodes: " << nCsma);
  NS_LOG_INFO ("Simulation Time: " << simTime);

  NodeContainer csmaNodes;
  csmaNodes.Create (nCsma);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (1)));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  InternetStackHelper stack;
  stack.Install (csmaNodes);

  Ipv6AddressHelper ipv6h;
  ipv6h.SetBase (Ipv6Address ("6001:db80::"), Ipv6Prefix (64));
  Ipv6InterfaceContainer csmaInterfaces;
  csmaInterfaces = ipv6h.Assign (csmaDevices);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Application configuration
  Ptr<PscApplicationConfiguration> appConfig = CreateObject <PscApplicationConfiguration> (
      "PscApplicationExample",
      UdpSocketFactory::GetTypeId (),     //Socket type
      5000                                //port number
      );

  appConfig->SetApplicationPattern (
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (10)),   //Number of packets to send per session
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (1)),   //Packet interval (in s)
    CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (5)),   //Time between sessions
    50,        //Client packet size (bytes)
    20);       //Server packet size (bytes)

  // Use helper to deploy the applications at client and server nodes
  Ptr<PscApplicationHelper> appHelper = CreateObject<PscApplicationHelper> ();

  ApplicationContainer apps = appHelper->Install (appConfig, csmaNodes.Get (0),
                                csmaNodes.Get (1),
                                csmaNodes.Get (1)->GetObject<Ipv6L3Protocol> ()->GetAddress (1, 1).GetAddress (),
                                echoClient,
                                Seconds (startTime),
                                Seconds (simTime));

  if (enableTraces)
  {
    appTrafficOutputStreamWrapper = Create<OutputStreamWrapper> ("PscApplicationExample_trace.txt", std::ios::out);
    //print header line
    *appTrafficOutputStreamWrapper->GetStream () << "Name\tTime\tAction\tPayload\tSeqNum" << std::endl;
    for (uint32_t i = 0; i < apps.GetN (); i++)
    {
      DynamicCast<PscApplication> (apps.Get (i))->TraceConnectWithoutContext (
        "Rx", MakeBoundCallback (&TraceAppRxTraffic, appTrafficOutputStreamWrapper));
      DynamicCast<PscApplication> (apps.Get (i))->TraceConnectWithoutContext (
        "Tx", MakeBoundCallback (&TraceAppTxTraffic, appTrafficOutputStreamWrapper));
    }
  }

  Simulator::Stop (Seconds (simTime));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

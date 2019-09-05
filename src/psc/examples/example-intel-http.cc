/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public service. You may use,
 * copy and distribute copies of the software in any medium, provided that you
 * keep intact this entire notice. You may improve,modify and create derivative
 * works of the software or any portion of the software, and you may copy and
 * distribute such modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and nature of
 * any such change. Please explicitly acknowledge the National Institute of
 * Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT
 * AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR WARRANTS THAT THE
 * OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED OR ERROR-FREE, OR THAT
 * ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT WARRANT OR MAKE ANY
 * REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE OR THE RESULTS THEREOF,
 * INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY, RELIABILITY,
 * OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

#include <ns3/core-module.h>
#include <ns3/network-module.h>
#include <ns3/internet-module.h>
#include <ns3/point-to-point-module.h>
#include <ns3/applications-module.h>
#include <ns3/csma-module.h>
#include <ns3/psc-module.h>

using namespace ns3;
using namespace psc;

/*
 * Example script that showcases how to instantiate and use the Intel HTTP 
 * Models.
 *
 * By default, this scenario sets up a simple topology of two nodes linked 
 * by a Point-to-Point link with 2 ms delay and no loss. In the first node 
 * the script deploys an instance of the HTTP Client, and the second node 
 * hosts the instance of the HTTP Server. Once the applications are 
 * deployed, the simulation runs for 2000 seconds. At the end of the 
 * simulation, PCAP traces of the Point-to-Point link will be available 
 * with the name "example-one-client-intel-http-trace".
 *
 * This script can generate variations of this default scenario by
 * using the command-line flags "multipleApps" and "lossy". When using
 * "multipleApps" the script will showcase how multiple servers can be 
 * deployed in a single node to serve multiple clients. In this case the
 * topology changes to 3 nodes connected through a CSMA link. The first node
 * becomes the HTTP server, with two instances deployed in ports 80 and 81, 
 * and the other two nodes each host an instance of the HTTP Client, each
 * one connecting to one of the server ports. Similarly to the default
 * case, at the end of the simulation (2000 seconds), PCAP traces will be
 * available, this time with the name "example-many-clients-intel-http-trace".
 *
 * The "lossy" command-line option will configure the link used in the 
 * scenario to have a 5 % packet loss error rate on each NetDevice connected
 * to that link, to demonstrate the behavior of the model in lossy 
 * environments. This option can be used with both the default scenario and 
 * the variant with multiple clients.
 */

NS_LOG_COMPONENT_DEFINE ("ExampleAppHttpIntel");

enum class LossMode { Lossless, Lossy };

void
SingleServerClient (LossMode mode)
{
  // Create the nodes
  NodeContainer nodes;
  nodes.Create (2);

  // Configure the point-to-point link between the nodes
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Gbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  // Configure IP addresses
  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  // Configure the applications
  ApplicationContainer apps;

  // HTTP Server (node 1)
  IntelHttpServerHelper httpServerHelper;
  apps.Add (httpServerHelper.Install (nodes.Get (1)));

  // HTTP Client (node 0)
  IntelHttpClientHelper httpClientHelper (interfaces.GetAddress (1));
  apps.Add (httpClientHelper.Install (nodes.Get (0)));

  apps.Start (Seconds (2));
  apps.Stop (Seconds (2000));

  // Log the traffic in the point-to-point link in a pcap file
  pointToPoint.EnablePcapAll ("example-one-client-intel-http-trace");

  if (mode == LossMode::Lossy)
    {
      Ptr<RateErrorModel> errorModel = CreateObject<RateErrorModel> ();
      errorModel->SetAttribute ("ErrorUnit",
                                EnumValue (RateErrorModel::ErrorUnit::ERROR_UNIT_PACKET));
      errorModel->SetAttribute ("ErrorRate", DoubleValue (0.05));

      for (auto device = devices.Begin (); device != devices.End (); device++)
        {
          (*device)->SetAttribute ("ReceiveErrorModel", PointerValue (errorModel));
        }
    }

  // Run the simulation for two thousand seconds, to show the sessions on and off pattern
  Simulator::Stop (Seconds (2000));
  Simulator::Run ();
  Simulator::Destroy ();
}

void
MultipleServerClients (LossMode mode)
{
  // Create the nodes
  NodeContainer nodes;
  nodes.Create (3);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Gbps"));
  NetDeviceContainer devices = csma.Install (nodes);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  ApplicationContainer apps;

  // HTTP Server (Node 0)
  Ptr<Node> serverNode = nodes.Get (0);
  const uint16_t serverPorts[] = {80u, 81u};
  IntelHttpServerHelper serverHelper;
  // Install a server application for each client
  for (auto port : serverPorts)
    {
      serverHelper.Set ("Port", UintegerValue (port));
      apps.Add (serverHelper.Install (serverNode));
    }

  // HTTP clients (Nodes 1 - 2)
  Ipv4Address serverAddress = interfaces.GetAddress (0);
  IntelHttpClientHelper clientHelper (serverAddress);

  // Attach each client to a different server port
  clientHelper.Set ("RemotePort", UintegerValue (serverPorts[0]));
  apps.Add (clientHelper.Install (nodes.Get (1)));

  clientHelper.Set ("RemotePort", UintegerValue (serverPorts[1]));
  apps.Add (clientHelper.Install (nodes.Get (2)));

  apps.Start (Seconds (2));
  apps.Stop (Seconds (2000));

  // Log the traffic in the link in a pcap file
  csma.EnablePcapAll ("example-many-clients-intel-http-trace");

  if (mode == LossMode::Lossy)
    {
      Ptr<RateErrorModel> errorModel = CreateObject<RateErrorModel> ();
      errorModel->SetAttribute ("ErrorUnit",
                                EnumValue (RateErrorModel::ErrorUnit::ERROR_UNIT_PACKET));
      errorModel->SetAttribute ("ErrorRate", DoubleValue (0.05));

      for (auto device = devices.Begin (); device != devices.End (); device++)
        {
          (*device)->SetAttribute ("ReceiveErrorModel", PointerValue (errorModel));
        }
    }

  // Run the simulation for two thousand seconds, to show the sessions on and off pattern
  Simulator::Stop (Seconds (2000));
  Simulator::Run ();
  Simulator::Destroy ();
}

int
main (int argc, char *argv[])
{
  bool useMultiple = false;
  bool useLossyNetwork = false;

  CommandLine cmd;
  cmd.AddValue ("multipleApps", "Demonstrate using multiple client Nodes with one server Node",
                useMultiple);
  cmd.AddValue ("lossy", "Add 5 % loss to each net device in this example", useLossyNetwork);

  cmd.Parse (argc, argv);

  LossMode mode = LossMode::Lossless;
  if (useLossyNetwork)
    mode = LossMode::Lossy;

  if (useMultiple)
    MultipleServerClients (mode);
  else
    SingleServerClient (mode);
}

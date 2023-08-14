/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service. You may
 * use, copy and distribute copies of the software in any medium, provided that
 * you keep intact this entire notice. You may improve, modify and create
 * derivative works of the software or any portion of the software, and you may
 * copy and distribute such modifications or works. Modified works should carry
 * a notice stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the National
 * Institute of Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
 * NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
 * DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
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

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/psc-module.h"
#include "ns3/wifi-module.h"

#include <iostream>
#include <fstream>

using namespace ns3;
using namespace psc;

/*
 * Source file of example to demonstrate the operation of the MCPTT pusher
 * model using various orhcestrators.
 *
 * This scenario conists of N users participating in a single off-network
 * MCPTT group call over Wi-Fi. The activity of the users, or "pushers", in
 * this call are controlled by several instances of
 * McpttPusherOrchestratorInterface sub-classes. Each sub-class enables a
 * articular feature, which, in this example, includes talk spurts,
 * talk sessions, and contention. The duration of talk spurts and
 * talk sessions are controlled by the orchestrator using
 * statistics gathered from public safety call logs. However, the interarrival
 * time of talk spurts and talk sessions are determined by the values of the
 * "vaf" and "saf" parameters, respectively. A talk session is simply a time
 * span when pushers become active, thus, resulting in the pushers pushing
 * and releasing the PTT buton on their device. A talk spurt is simply the
 * time from when a PTT button is pushed until that PTT button is released.
 * Contention occurs when a PTT push is made during an already ongoing
 * talk spurt. The amount of contention for the group is controlled by the
 * "cp" parameter, which indicates uniformly how often contention will occur
 * relative to the total number of talk spurts.
 *
 * Executing this scenario with "--verbose=true" will output all orchestrator
 * events and a summary of statistics once the scenario is over. Including the
 * "--trace=true" agrument will also produce trace files that capture the
 * state transitions for pushers and sessions between their respective
 * "pushed/released" and "active/inactive" states.  These traces can be
 * plotted using the "example-mcptt-pusher-orchestrator.sh" script if it is
 * included with this example to show the timeline of events.
 *
 * The user can also control the number of users (N) using the "--users"
 * argument, the size of data packets that are sent during a talk spurt with
 * "--media-size", the rate at which the data packets are sent with
 * "--data-rate", the x and y boundaries in which the users can be placed
 * using "--max-x" and "--max-y", and the duration of the simulation with
 * "--start-time" and "--stop-time"
 *
 */

NS_LOG_COMPONENT_DEFINE ("ExampleMcpttPusherOrchestrator");

bool s_trace = false;
bool s_verbose = false;
double s_totalSpurtTime = 0.0;
uint32_t s_totalPtts = 0;
uint32_t s_totalContentions = 0;
double s_totalSessionDurationTime = 0.0;
std::map<std::string, Ptr<OutputStreamWrapper> > s_fileStreams;
std::map<std::string, Time> s_eventSpans;

// Associates a state change with a means of identification.
class StateTracker : public SimpleRefCount<StateTracker>
{
public:
  StateTracker ();
  ~StateTracker ();
  void Trigger (bool oldState, bool newState);

private:
  uint32_t m_id;
  std::string m_name;
  Callback<void, StateTracker&, bool, bool> m_destination;

public:
  uint32_t GetId (void) const;
  std::string GetName (void) const;
  void SetDestination (const Callback<void, StateTracker&, bool, bool>& destination);
  void SetId (const uint32_t id);
  void SetName (const std::string& name);
};

void PttDurationCallback (uint32_t userId, Time duration);
void PttIatCallback (uint32_t userId, Time iat);
void ContentionPttDurationCallback (uint32_t userId, Time duration);
void ContentionPttIatCallback (uint32_t userId, Time iat);
void SessionDurationCallback (Time duration);
void SessionIatCallback (Time iat);
void PusherStateCallback (StateTracker&, bool oldState, bool newState);
void SessionStateCallback (bool oldState, bool newState);

int main (int argc, char *argv[])
{
  uint32_t appCount = 2;
  DataRate dataRate = DataRate ("24kb/s");
  TypeId floorTid = McpttFloorParticipant::GetTypeId ();
  double maxX = 5.0;
  double maxY = 5.0;
  uint32_t msgSize = 60; //60 + RTP header = 60 + 12 = 72
  Time startTime = Seconds (2.0);
  Time stopTime = Seconds (62.0);
  double vaf = 1.0;
  double cp = 0.0;
  double saf = 0.5;
  TypeId socketFacTid = UdpSocketFactory::GetTypeId ();
  Ipv4Address peerAddress = Ipv4Address ("255.255.255.255");

  CommandLine cmd;
  cmd.AddValue ("trace", "Enable traces.", s_trace);
  cmd.AddValue ("users", "Number of users to include in the group.", appCount);
  cmd.AddValue ("data-rate", "The data rate at which media will be sent at.", dataRate);
  cmd.AddValue ("max-x", "The largest x-coordinate that a UE can have.", maxX);
  cmd.AddValue ("max-y", "The largest y-coordinate that a UE can have.", maxY);
  cmd.AddValue ("media-size", "The size (in bytes) of the media packets that will be sent.", msgSize);
  cmd.AddValue ("start-time", "The number of seconds into the simulation that the applications should start.", startTime);
  cmd.AddValue ("stop-time", "The number of seconds into the simulation that the applications should stop.", stopTime);
  cmd.AddValue ("vaf", "The voice activity factor to use for the orchestrator.", vaf);
  cmd.AddValue ("cp", "The probability of contention.", cp);
  cmd.AddValue ("saf", "The voice activity factor to use for the orchestrator.", saf);
  cmd.AddValue ("verbose", "Whether or not to print out PTT duration and PTT IAT information.", s_verbose);
  cmd.Parse (argc, argv);

  Config::SetDefault ("ns3::psc::McpttMsgStats::CallControl", BooleanValue (false));

  NodeContainer nodes;
  nodes.Create (appCount);

  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211g); //2.4Ghz
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue ("ErpOfdmRate6Mbps"));

  WifiMacHelper wifiMac;
  YansWifiPhyHelper wifiPhy;
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel",
                                  "Frequency", DoubleValue (2.407e9)); //2.4Ghz

  wifiMac.SetType ("ns3::AdhocWifiMac");

  YansWifiPhyHelper phy = wifiPhy;
  phy.SetChannel (wifiChannel.Create ());

  WifiMacHelper mac = wifiMac;
  NetDeviceContainer devices = wifi.Install (phy, mac, nodes);

  NS_LOG_INFO ("Building physical topology...");
  Ptr<RandomBoxPositionAllocator> rndBoxPosAllocator = CreateObject <RandomBoxPositionAllocator> ();
  rndBoxPosAllocator->SetX (CreateObjectWithAttributes<UniformRandomVariable> ("Min", DoubleValue (0.0), "Max", DoubleValue (maxX)));
  rndBoxPosAllocator->SetY (CreateObjectWithAttributes<UniformRandomVariable> ("Min", DoubleValue (0.0), "Max", DoubleValue (maxY)));
  rndBoxPosAllocator->SetZ (CreateObjectWithAttributes<ConstantRandomVariable> ("Constant", DoubleValue (1.5)));
  rndBoxPosAllocator->AssignStreams (1);

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  for (uint32_t count = 0; count < appCount; count++)
    {
      Vector position = rndBoxPosAllocator->GetNext ();

      NS_LOG_INFO ("UE " << (count + 1) << " located at " << position << ".");

      positionAlloc->Add (position);
    }

  MobilityHelper mobility;
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  NS_LOG_INFO ("Installing internet stack on all nodes...");
  InternetStackHelper internet;
  internet.Install (nodes);

  NS_LOG_INFO ("Assigning IP addresses to each net device...");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (devices);

  NS_LOG_INFO ("Creating applications...");
  ApplicationContainer clientApps;
  McpttHelper mcpttHelper;
  mcpttHelper.SetPttApp ("ns3::psc::McpttPttApp",
                         "PushOnStart", BooleanValue (true));
  mcpttHelper.SetMediaSrc ("ns3::psc::McpttMediaSrc",
                           "Bytes", UintegerValue (msgSize),
                           "DataRate", DataRateValue (dataRate));
  mcpttHelper.SetPusher ("ns3::psc::McpttPusher",
                         "Automatic", BooleanValue (false));

  clientApps.Add (mcpttHelper.Install (nodes));
  clientApps.Start (startTime);
  clientApps.Stop (stopTime);

  McpttCallHelper callHelper;
  callHelper.ConfigureOffNetworkBasicGrpCall (clientApps, peerAddress, appCount);

  Ptr<McpttPusherOrchestratorInterface> orchestrator = 0;
  Ptr<McpttPusherOrchestratorSpurtCdf> spurtOrchestrator = CreateObject<McpttPusherOrchestratorSpurtCdf> ();
  spurtOrchestrator->SetAttribute ("ActivityFactor", DoubleValue (vaf));
  spurtOrchestrator->TraceConnectWithoutContext ("PttDurationTrace", MakeCallback (&PttDurationCallback));
  spurtOrchestrator->TraceConnectWithoutContext ("PttInterarrivalTimeTrace", MakeCallback (&PttIatCallback));
  orchestrator = spurtOrchestrator;

  Ptr<McpttPusherOrchestratorContention> contentionOrchestrator = CreateObject<McpttPusherOrchestratorContention> ();
  contentionOrchestrator->SetAttribute ("ContentionProbability", DoubleValue (cp));
  contentionOrchestrator->SetAttribute ("Orchestrator", PointerValue (orchestrator));
  contentionOrchestrator->TraceConnectWithoutContext ("PttDurationTrace", MakeCallback (&ContentionPttDurationCallback));
  contentionOrchestrator->TraceConnectWithoutContext ("PttInterarrivalTimeTrace", MakeCallback (&ContentionPttIatCallback));
  orchestrator = contentionOrchestrator;

  Ptr<McpttPusherOrchestratorSessionCdf> sessionOrchestrator = CreateObject<McpttPusherOrchestratorSessionCdf> ();
  sessionOrchestrator->SetAttribute ("ActivityFactor", DoubleValue (saf));
  sessionOrchestrator->SetAttribute ("Orchestrator", PointerValue (orchestrator));
  sessionOrchestrator->TraceConnectWithoutContext ("SessionDurationTrace", MakeCallback (&SessionDurationCallback));
  sessionOrchestrator->TraceConnectWithoutContext ("SessionInterarrivalTimeTrace", MakeCallback (&SessionIatCallback));
  sessionOrchestrator->TraceConnectWithoutContext ("SessionState", MakeCallback (&SessionStateCallback));
  orchestrator = sessionOrchestrator;

  orchestrator->StartAt (startTime);
  orchestrator->StopAt (stopTime);

  mcpttHelper.AddPushersToOrchestrator (orchestrator, clientApps);

  std::stringstream ss;
  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream = 0;
  ss.str (std::string ());

  if (s_trace)
    {
      ss << "session-state.csv";
      stream = asciiTraceHelper.CreateFileStream (ss.str ());
      s_fileStreams["session"] = stream;
      *stream->GetStream () << "Session" << std::endl;
      *stream->GetStream () << Seconds (0).GetSeconds () << " " << "inactive" << " " << 2 << std::endl;

      ss.str (std::string ());
      ss << "contention-state.csv";
      stream = asciiTraceHelper.CreateFileStream (ss.str ());
      s_fileStreams["contention"] = stream;
      *stream->GetStream () << "Contention" << std::endl;
    }

  std::vector<Ptr<StateTracker> > stateTrackers;
  for (uint32_t appIdx = 0; appIdx < clientApps.GetN (); appIdx++)
    {
      ss.str (std::string ());
      uint32_t pusherId = appIdx + 1;
      ss << "pusher-" << pusherId;

      Ptr<StateTracker> tracker = Create<StateTracker> ();
      tracker->SetId (pusherId);
      tracker->SetName (ss.str ());
      tracker->SetDestination (MakeCallback (&PusherStateCallback));
      stateTrackers.push_back (tracker);

      Ptr<McpttPusher> pusher = clientApps.Get (appIdx)->GetObject<McpttPttApp> ()->GetPusher ();
      pusher->TraceConnectWithoutContext ("PushingState", MakeCallback (&StateTracker::Trigger, tracker));

      if (s_trace)
        {
          ss.str (std::string ());
          ss << "pusher-state-" << pusherId << ".csv";
          stream = asciiTraceHelper.CreateFileStream (ss.str ());
          s_fileStreams[tracker->GetName ()] = stream;
          *stream->GetStream () << "\"Pusher " << pusherId << "\"" << std::endl;
          *stream->GetStream () << Seconds (0).GetSeconds () << " " << "released" << " " << (2 * pusherId + 2) << std::endl;
        }
    }

  if (s_trace)
    {
      NS_LOG_INFO ("Enabling MCPTT traces...");
      Ptr<McpttTraceHelper> traceHelper = CreateObject<McpttTraceHelper> ();
      traceHelper->EnableMsgTraces ();
      traceHelper->EnableStateMachineTraces ();
    }

  NS_LOG_INFO ("Starting simulation...");
  Simulator::Stop (stopTime + Seconds (2));
  Simulator::Run ();
  Simulator::Destroy ();

  if (s_trace)
    {
      for (std::vector<Ptr<StateTracker> >::iterator it = stateTrackers.begin (); it != stateTrackers.end (); it++)
        {
          std::map<std::string, Ptr<OutputStreamWrapper> >::iterator streamIt = s_fileStreams.find ((*it)->GetName ());
          *(streamIt->second)->GetStream () << stopTime.GetSeconds () << " " << "released" << " " << (2 * (*it)->GetId () + 2) << std::endl;
          streamIt->second = 0;
          (*it) = 0;
        }

      std::map<std::string, Ptr<OutputStreamWrapper> >::iterator it = s_fileStreams.find ("session");
      *(it->second)->GetStream () << stopTime.GetSeconds () << " " << "inactive" << " " << 2 << std::endl;
      it->second = 0;

      it = s_fileStreams.find ("contention");
      it->second = 0;
    }

  if (s_verbose)
    {
      std::cout << "Talkspurt time = " << s_totalSpurtTime << " s (" << (s_totalSpurtTime / (stopTime - startTime).GetSeconds ()) * 100.0 << " %)" << std::endl;
      std::cout << "Session time = " << s_totalSessionDurationTime << " s (" << (s_totalSessionDurationTime / (stopTime - startTime).GetSeconds ()) * 100.0 << " %)" << std::endl;
      std::cout << "Contention = " << s_totalContentions << " (" << ((double)s_totalContentions / s_totalPtts) * 100.0 << " %)" << std::endl;
    }

  NS_LOG_INFO ("Done.");

  return 0;
}

StateTracker::StateTracker ()
{
  m_id = 0;
  m_name = "";
  m_destination = MakeNullCallback<void, StateTracker&, bool, bool> ();
}

StateTracker::~StateTracker ()
{}

void
StateTracker::Trigger (bool oldState, bool newState)
{
  if (!m_destination.IsNull ())
    {
      m_destination (*this, oldState, newState);
    }
}

uint32_t
StateTracker::GetId (void) const
{
  return m_id;
}

std::string
StateTracker::GetName (void) const
{
  return m_name;
}

void
StateTracker::SetDestination (const Callback<void, StateTracker&, bool, bool>& destination)
{
  m_destination = destination;
}

void
StateTracker::SetId (const uint32_t id)
{
  m_id = id;
}

void
StateTracker::SetName (const std::string& name)
{
  m_name = name;
}

void
PttDurationCallback (uint32_t userId, Time duration)
{
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << "s - PTT Duration: " << duration.GetSeconds () << std::endl;
    }

  s_totalPtts++;
}

void
PttIatCallback (uint32_t userId, Time iat)
{
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << "s - PTT IAT: " << iat.GetSeconds () << std::endl;
    }
}

void
ContentionPttDurationCallback (uint32_t userId, Time duration)
{
  if (s_trace)
    {
      Ptr<OutputStreamWrapper> stream = 0;
      std::map<std::string, Ptr<OutputStreamWrapper> >::iterator it = s_fileStreams.find ("contention");
      stream = it->second;
      *stream->GetStream () << Simulator::Now ().GetSeconds () << " contention " << 1 << std::endl;
    }

  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << "s - Contention PTT Duration: " << duration.GetSeconds () << std::endl;
    }

  s_totalContentions++;
}

void
ContentionPttIatCallback (uint32_t userId, Time iat)
{
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << "s - Contention PTT IAT: " << iat.GetSeconds () << std::endl;
    }
}

void
SessionDurationCallback (Time duration)
{
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << "s - Session Duration: " << duration.GetSeconds () << std::endl;
    }
}

void
SessionIatCallback (Time iat)
{
  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << "s - Session IAT: " << iat.GetSeconds () << std::endl;
    }
}

void
PusherStateCallback (StateTracker& stateId, bool oldState, bool newState)
{
  if (s_trace)
    {
      Ptr<OutputStreamWrapper> stream = 0;
      std::map<std::string, Ptr<OutputStreamWrapper> >::iterator it = s_fileStreams.find (stateId.GetName ());
      stream = it->second;

      *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << (newState ? "pushed" : "released") << " " << (newState ? (2 * stateId.GetId () + 3) : (2 * stateId.GetId () + 2)) << std::endl;
    }

  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << " Pusher " << stateId.GetId () << "'s state = " << (newState ? "pushed" : "released") << std::endl;
    }

  if (newState)
    {
      s_eventSpans[stateId.GetName ()] = Simulator::Now ();
    }
  else if (s_eventSpans[stateId.GetName ()] != Seconds (0))
    {
      s_totalSpurtTime += (Simulator::Now () - s_eventSpans[stateId.GetName ()]).GetSeconds ();
      s_eventSpans[stateId.GetName ()] = Seconds (0);
    }
}

void
SessionStateCallback (bool oldState, bool newState)
{
  if (s_trace)
    {
      Ptr<OutputStreamWrapper> stream = 0;
      std::map<std::string, Ptr<OutputStreamWrapper> >::iterator it = s_fileStreams.find ("session");
      stream = it->second;
      *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << (newState ? "active" : "inactive") << " " << (newState ? 3 : 2) << std::endl;
    }

  if (s_verbose)
    {
      std::cout << Simulator::Now ().GetSeconds () << " Session = " << (newState ? "active" : "inactive") << std::endl;
    }

  if (newState)
    {
      s_eventSpans["session"] = Simulator::Now ();
    }
  else if (s_eventSpans["session"] != Seconds (0))
    {
      s_totalSessionDurationTime += (Simulator::Now () - s_eventSpans["session"]).GetSeconds ();
      s_eventSpans["session"] = Seconds (0);
    }
}


/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices;                           // What shall be installed at nodes in simulation
  devices = pointToPoint.Install (nodes);

  InternetStackHelper stack;                            // Sets up internet frame
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");        // Defines ip4v base address for simulation(ip,netmask)

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  UdpEchoServerHelper echoServer (9);                   // Opens port number

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1)); // Server is assigned node 1
  serverApps.Start (Seconds (1.0));                                     // Server opens connection after 1.0s
  serverApps.Stop (Seconds (10.0));                                     // Closes after 10.0s

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);        
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0)); // Client is assgined node 0
  clientApps.Start (Seconds (2.0));                                     // Client starts data transfer after 2.0s
  clientApps.Stop (Seconds (10.0));                                     // Time limit for transfer

  AnimationInterface anim ("first1.xml");                               // Animation file is created for first.cc
  anim.SetConstantPosition(nodes.Get(0), 10.0, 10.0);                   // Sets node position on interface
  anim.SetConstantPosition(nodes.Get(1), 20.0, 20.0);


AsciiTraceHelper ascii;                                                 // Ascii Format tracing
pointToPoint.EnableAsciiAll(ascii.CreateFileStream("first.tr"));        // Assigns Data trace file for TraceMetrics




  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

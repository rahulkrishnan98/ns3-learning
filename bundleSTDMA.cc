#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/bp-endpoint-id.h"
#include "ns3/bundle-protocol.h"
#include "ns3/bp-static-routing-protocol.h"
#include "ns3/bundle-protocol-helper.h"
#include "ns3/bundle-protocol-container.h"
#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/node-list.h"
#include "ns3/packet-socket-helper.h"
#include "ns3/packet-socket-address.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/network-module.h"
#include "ns3/on-off-helper.h"
#include "ns3/stdma-module.h"
#include "ns3/netanim-module.h"
#include <string.h>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <numeric>
using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("BundleProtocolSimpleExample");
void Send (Ptr<BundleProtocol> sender, uint32_t size, BpEndpointId src, BpEndpointId dst)
{
  std::cout << Simulator::Now ().GetMilliSeconds () << " Send a PDU with size " << size << std::endl;

  Ptr<Packet> packet = Create<Packet> (size);
  sender->Send (packet, src, dst);
}

void Receive (Ptr<BundleProtocol> receiver, BpEndpointId eid)
{

  Ptr<Packet> p = receiver->Receive (eid);
  while (p != NULL)
    {
      std::cout << Simulator::Now ().GetMilliSeconds () << " Receive bundle size " << p->GetSize () << std::endl;
      p = receiver->Receive (eid);
    }
}
int main(int argc, char *argv[])
{
    // // STDMA init
    // stdma::StdmaHelper stdma;
    // stdma.SetStandard(WIFI_PHY_STANDARD_80211p_CCH);
    // stdma::StdmaMacHelper stdmaMac = stdma::StdmaMacHelper::Default();

    // Config::SetDefault ("stdma::StdmaMac::FrameDuration", TimeValue(Seconds(1.0)));
    // Config::SetDefault ("stdma::StdmaMac::MaximumPacketSize", UintegerValue(400));
    // Config::SetDefault ("stdma::StdmaMac::ReportRate", UintegerValue(10));
    // Config::SetDefault ("stdma::StdmaMac::Timeout", RandomVariableValue (UniformVariable(8, 8)));

    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(2);
    NodeContainer wifiApNode;
    wifiApNode.Create(1);

    NodeContainer wifiNodes;
    wifiNodes.Add(wifiApNode.Get(0));
    for(uint32_t i=0;i<2;i++){
        wifiNodes.Add(wifiStaNodes.Get(i));
    }
    YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
    phy.SetChannel(wifiChannel.Create());

    WifiHelper wifi;
    wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

    //For mobile nodes
    NqosWifiMacHelper mac = NqosWifiMacHelper::Default ();    
    Ssid ssid = Ssid("base-station");
    mac.SetType ("ns3::StaWifiMac",
                "Ssid", SsidValue (ssid),
                "ActiveProbing", BooleanValue (false));
    
    NetDeviceContainer staDevices;
    staDevices = wifi.Install (phy, mac, wifiStaNodes);

    //For access point
    mac.SetType ("ns3::ApWifiMac",
            "Ssid", SsidValue (ssid));
  
    NetDeviceContainer apDevice;
    apDevice = wifi.Install(phy,mac,wifiApNode);

    MobilityHelper mobility;

    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue (0.0),
                                  "MinY", DoubleValue (0.0),
                                  "DeltaX", DoubleValue (5.0),
                                  "DeltaY", DoubleValue (10.0),
                                  "GridWidth", UintegerValue (3),
                                  "LayoutType", StringValue ("RowFirst"));

    //Stationary mobility model for  ap node
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiNodes);

    

    InternetStackHelper stack;
    stack.Install(wifiNodes);

    NS_LOG_INFO ("Assign IP Addresses.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i = ipv4.Assign (staDevices);
    i.Add(ipv4.Assign(apDevice));

    

    NS_LOG_INFO ("Create bundle applications.");
 
    std::ostringstream l4type;
    l4type << "Tcp";
    Config::SetDefault ("ns3::BundleProtocol::L4Type", StringValue (l4type.str ()));
    Config::SetDefault ("ns3::BundleProtocol::BundleSize", UintegerValue (400)); 
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (512));

    // build endpoint ids
    BpEndpointId eidSender ("dtn", "node0");
    BpEndpointId eidRecv ("dtn", "node1");

    // set bundle static routing
    Ptr<BpStaticRoutingProtocol> route = CreateObject<BpStaticRoutingProtocol> ();
    route->AddRoute (eidSender, InetSocketAddress (i.GetAddress (2), 9));
    route->AddRoute (eidRecv, InetSocketAddress (i.GetAddress (0), 9));

    // sender  
    BundleProtocolHelper bpSenderHelper;
    bpSenderHelper.SetRoutingProtocol (route);
    bpSenderHelper.SetBpEndpointId (eidSender);
    BundleProtocolContainer bpSenders = bpSenderHelper.Install (wifiNodes.Get (0));
    bpSenders.Start (Seconds (0.1));
    bpSenders.Stop (Seconds (1.0));

  // receiver
    BundleProtocolHelper bpReceiverHelper;
    bpReceiverHelper.SetRoutingProtocol (route);
    bpReceiverHelper.SetBpEndpointId (eidRecv);
    BundleProtocolContainer bpReceivers = bpReceiverHelper.Install (wifiNodes.Get (1));
    bpReceivers.Start (Seconds (0.0));
    bpReceivers.Stop (Seconds (1.0));

  // send 1000 bytes bundle 
    uint32_t size = 1000;
    Simulator::Schedule (Seconds (0.2), &Send, bpSenders.Get (0), size, eidSender, eidRecv);

  // receive function
    Simulator::Schedule (Seconds (0.8), &Receive, bpReceivers.Get (0), eidRecv);

    Simulator::Stop (Seconds (1.0));
    Simulator::Run ();
    Simulator::Destroy ();
}
#include <fstream>
#include <iostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/applications-module.h"
#include "ns3/itu-r-1411-los-propagation-loss-model.h"
#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/config-store-module.h"
#include "ns3/integer.h"
#include "ns3/wave-bsm-helper.h"
#include "ns3/wave-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Experiment");

class RoutingStats
{
public:
  /**
   * \brief Constructor
   * \return none
   */
  RoutingStats ();

  /**
   * \brief Returns the number of bytes received
   * \return the number of bytes received
   */
  uint32_t GetRxBytes ();

  /**
   * \brief Returns the cumulative number of bytes received
   * \return the cumulative number of bytes received
   */
  uint32_t GetCumulativeRxBytes ();

  /**
   * \brief Returns the count of packets received
   * \return the count of packets received
   */
  uint32_t GetRxPkts ();

  /**
   * \brief Returns the cumulative count of packets received
   * \return the cumulative count of packets received
   */
  uint32_t GetCumulativeRxPkts ();

  /**
   * \brief Increments the number of (application-data)
   * bytes received, not including MAC/PHY overhead
   * \param rxBytes the number of bytes received
   * \return none
   */
  void IncRxBytes (uint32_t rxBytes);

  /**
   * \brief Increments the count of packets received
   * \return none
   */
  void IncRxPkts ();

  /**
   * \brief Sets the number of bytes received.
   * \param rxBytes the number of bytes received
   * \return none
   */
  void SetRxBytes (uint32_t rxBytes);

  /**
   * \brief Sets the number of packets received
   * \param rxPkts the number of packets received
   * \return none
   */
  void SetRxPkts (uint32_t rxPkts);

  /**
   * \brief Returns the number of bytes transmitted
   * \return the number of bytes transmitted
   */
  uint32_t GetTxBytes ();

  /**
   * \brief Returns the cumulative number of bytes transmitted
   * \param socket the receiving socket
   * \return none
   */
  uint32_t GetCumulativeTxBytes ();

  /**
   * \brief Returns the number of packets transmitted
   * \return the number of packets transmitted
   */
  uint32_t GetTxPkts ();

  /**
   * \brief Returns the cumulative number of packets transmitted
   * \return the cumulative number of packets transmitted
   */
  uint32_t GetCumulativeTxPkts ();

  /**
   * \brief Increment the number of bytes transmitted
   * \param txBytes the number of addtional bytes transmitted
   * \return none
   */
  void IncTxBytes (uint32_t txBytes);

  /**
   * \brief Increment the count of packets transmitted
   * \return none
   */
  void IncTxPkts ();

  /**
   * \brief Sets the number of bytes transmitted
   * \param txBytes the number of bytes transmitted
   * \return none
   */
  void SetTxBytes (uint32_t txBytes);

  /**
   * \brief Sets the number of packets transmitted
   * \param txPkts the number of packets transmitted
   * \return none
   */
  void SetTxPkts (uint32_t txPkts);

private:
  uint32_t m_RxBytes;
  uint32_t m_cumulativeRxBytes;
  uint32_t m_RxPkts;
  uint32_t m_cumulativeRxPkts;
  uint32_t m_TxBytes;
  uint32_t m_cumulativeTxBytes;
  uint32_t m_TxPkts;
  uint32_t m_cumulativeTxPkts;
};

RoutingStats::RoutingStats ()
  : m_RxBytes (0),
    m_cumulativeRxBytes (0),
    m_RxPkts (0),
    m_cumulativeRxPkts (0),
    m_TxBytes (0),
    m_cumulativeTxBytes (0),
    m_TxPkts (0),
    m_cumulativeTxPkts (0)
{
}

uint32_t
RoutingStats::GetRxBytes ()
{
  return m_RxBytes;
}

uint32_t
RoutingStats::GetCumulativeRxBytes ()
{
  return m_cumulativeRxBytes;
}

uint32_t
RoutingStats::GetRxPkts ()
{
  return m_RxPkts;
}

uint32_t
RoutingStats::GetCumulativeRxPkts ()
{
  return m_cumulativeRxPkts;
}

void
RoutingStats::IncRxBytes (uint32_t rxBytes)
{
  m_RxBytes += rxBytes;
  m_cumulativeRxBytes += rxBytes;
}

void
RoutingStats::IncRxPkts ()
{
  m_RxPkts++;
  m_cumulativeRxPkts++;
}

void
RoutingStats::SetRxBytes (uint32_t rxBytes)
{
  m_RxBytes = rxBytes;
}

void
RoutingStats::SetRxPkts (uint32_t rxPkts)
{
  m_RxPkts = rxPkts;
}

uint32_t
RoutingStats::GetTxBytes ()
{
  return m_TxBytes;
}

uint32_t
RoutingStats::GetCumulativeTxBytes ()
{
  return m_cumulativeTxBytes;
}

uint32_t
RoutingStats::GetTxPkts ()
{
  return m_TxPkts;
}

uint32_t
RoutingStats::GetCumulativeTxPkts ()
{
  return m_cumulativeTxPkts;
}

void
RoutingStats::IncTxBytes (uint32_t txBytes)
{
  m_TxBytes += txBytes;
  m_cumulativeTxBytes += txBytes;
}

void
RoutingStats::IncTxPkts ()
{
  m_TxPkts++;
  m_cumulativeTxPkts++;
}

void
RoutingStats::SetTxBytes (uint32_t txBytes)
{
  m_TxBytes = txBytes;
}

void
RoutingStats::SetTxPkts (uint32_t txPkts)
{
  m_TxPkts = txPkts;
}


class RoutingHelper : public Object
{
public:
  /**
   * \brief Get class TypeId
   * \return the TypeId for the class
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Constructor
   * \return none
   */
  RoutingHelper ();

  /**
   * \brief Destructor
   * \return none
   */
  virtual ~RoutingHelper ();

  /**
   * \brief Installs routing funcationality on nodes and their
   * devices and interfaces.
   * \param c node container
   * \param d net device container
   * \param i IPv4 interface container
   * \param totalTime the total time that nodes should attempt to
   * route data
   * \param protocol the routing protocol (1=OLSR;2=AODV;3=DSDV;4=DSR)
   * \param nSinks the number of nodes which will act as data sinks
   * \param routingTables dump routing tables at t=5 seconds (0=no;1=yes)
   * \return none
   */
  void Install (NodeContainer & c,
                NetDeviceContainer & d,
                Ipv4InterfaceContainer & i,
                double totalTime,
                int protocol,
                uint32_t nSinks,
                int routingTables);

  /**
   * \brief Trace the receipt of an on-off-application generated packet
   * \param context this object
   * \param packet a received packet
   * \return none
   */
  void OnOffTrace (std::string context, Ptr<const Packet> packet);

  /**
   * \brief Returns the RoutingStats instance
   * \return the RoutingStats instance
   */
  RoutingStats & GetRoutingStats ();

  /**
   * \brief Enable/disable logging
   * \param log non-zero to enable logging
   * \return none
   */
  void SetLogging (int log);

private:
  /**
   * \brief Sets up the protocol protocol on the nodes
   * \param c node container
   * \return none
   */
  void SetupRoutingProtocol (NodeContainer & c);

  /**
   * \brief Assigns IPv4 addresses to net devices and their interfaces
   * \param d net device container
   * \param adhocTxInterfaces IPv4 interface container
   * \return none
   */
  void AssignIpAddresses (NetDeviceContainer & d,
                          Ipv4InterfaceContainer & adhocTxInterfaces);

  /**
   * \brief Sets up routing messages on the nodes and their interfaces
   * \param c node container
   * \param adhocTxInterfaces IPv4 interface container
   * \return none
   */
  void SetupRoutingMessages (NodeContainer & c,
                             Ipv4InterfaceContainer & adhocTxInterfaces);

  /**
   * \brief Sets up a routing packet for tranmission
   * \param addr destination address
   * \parm node source node
   * \return Socket to be used for sending/receiving a routed data packet
   */
  Ptr<Socket> SetupRoutingPacketReceive (Ipv4Address addr, Ptr<Node> node);

  /**
   * \brief Process a received routing packet
   * \param socket the receiving socket
   * \return none
   */
  void ReceiveRoutingPacket (Ptr<Socket> socket);

  double m_TotalSimTime;        // seconds
  uint32_t m_protocol;       // routing protocol; 0=NONE, 1=OLSR, 2=AODV, 3=DSDV, 4=DSR
  uint32_t m_port;
  uint32_t m_nSinks;              // number of sink nodes (< all nodes)
  int m_routingTables;      // dump routing table (at t=5 sec).  0=No, 1=Yes
  RoutingStats routingStats;
  std::string m_protocolName;
  int m_log;
};

NS_OBJECT_ENSURE_REGISTERED (RoutingHelper);

TypeId
RoutingHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RoutingHelper")
    .SetParent<Object> ()
    .AddConstructor<RoutingHelper> ();
  return tid;
}

RoutingHelper::RoutingHelper ()
  : m_TotalSimTime (300.01),
    m_protocol (0),
    m_port (9),
    m_nSinks (0),
    m_routingTables (0),
    m_log (0)
{
}

RoutingHelper::~RoutingHelper ()
{
}

void
RoutingHelper::Install (NodeContainer & c,
                        NetDeviceContainer & d,
                        Ipv4InterfaceContainer & i,
                        double totalTime,
                        int protocol,
                        uint32_t nSinks,
                        int routingTables)
{
  m_TotalSimTime = totalTime;
  m_protocol = protocol;
  m_nSinks = nSinks;
  m_routingTables = routingTables;

  SetupRoutingProtocol (c);
  AssignIpAddresses (d, i);
  SetupRoutingMessages (c, i);
}

Ptr<Socket>
RoutingHelper::SetupRoutingPacketReceive (Ipv4Address addr, Ptr<Node> node)
{
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  Ptr<Socket> sink = Socket::CreateSocket (node, tid);
  InetSocketAddress local = InetSocketAddress (addr, m_port);
  sink->Bind (local);
  sink->SetRecvCallback (MakeCallback (&RoutingHelper::ReceiveRoutingPacket, this));

  return sink;
}

void
RoutingHelper::SetupRoutingProtocol (NodeContainer & c)
{
  InternetStackHelper stack;
  stack.Install(c);
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  if (m_log != 0)
    {
      NS_LOG_UNCOND ("Routing Setup for " << m_protocolName);
    }
}

void
RoutingHelper::AssignIpAddresses (NetDeviceContainer & d,
                                  Ipv4InterfaceContainer & adhocTxInterfaces)
{
  NS_LOG_INFO ("Assigning IP addresses");
  Ipv4AddressHelper addressAdhoc;
  // we may have a lot of nodes, and want them all
  // in same subnet, to support broadcast
  addressAdhoc.SetBase ("10.1.0.0", "255.255.0.0");
  adhocTxInterfaces = addressAdhoc.Assign (d);
}

void
RoutingHelper::SetupRoutingMessages (NodeContainer & c,
                                     Ipv4InterfaceContainer & adhocTxInterfaces)
{
  // Setup routing transmissions
  OnOffHelper onoff1 ("ns3::UdpSocketFactory",Address ());
  onoff1.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onoff1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));

  Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable> ();
  int64_t stream = 2;
  var->SetStream (stream);
  for (uint32_t i = 0; i < m_nSinks; i++)
    {
   
      if (m_protocol != 0)
        {
          if(m_log != 0){
            std::cout<<"Node "<<i<<" is set up as Packet Sink\n";
            std::cout<<"Node "<<i+m_nSinks<<" is set up as Packet Source\n";
          }
          
          Ptr<Socket> sink = SetupRoutingPacketReceive (adhocTxInterfaces.GetAddress (i), c.Get (i));
        }

      AddressValue remoteAddress (InetSocketAddress (adhocTxInterfaces.GetAddress (i), m_port));
      onoff1.SetAttribute ("Remote", remoteAddress);

      ApplicationContainer temp = onoff1.Install (c.Get (i + m_nSinks));
      temp.Start (Seconds (var->GetValue (1.0,2.0)));
      temp.Stop (Seconds (m_TotalSimTime));
    }
}

static inline std::string
PrintReceivedRoutingPacket (Ptr<Socket> socket, Ptr<Packet> packet)
{
  SocketAddressTag tag;
  bool found;
  found = packet->PeekPacketTag (tag);
  std::ostringstream oss;

  oss << Simulator::Now ().GetSeconds () << " " << socket->GetNode ()->GetId ();

  if (found)
    {
      InetSocketAddress addr = InetSocketAddress::ConvertFrom (tag.GetAddress ());
      oss << " received one packet from " << addr.GetIpv4 ();
    }
  else
    {
      oss << " received one packet!";
    }
  return oss.str ();
}

void
RoutingHelper::ReceiveRoutingPacket (Ptr<Socket> socket)
{
  Ptr<Packet> packet;
  while ((packet = socket->Recv ()))
    {
      // application data, for goodput
      uint32_t RxRoutingBytes = packet->GetSize ();
      GetRoutingStats ().IncRxBytes (RxRoutingBytes);
      GetRoutingStats ().IncRxPkts ();
      if (m_log != 0)
        {
          NS_LOG_UNCOND (m_protocolName + " " + PrintReceivedRoutingPacket (socket, packet));
        }
    }
}

void
RoutingHelper::OnOffTrace (std::string context, Ptr<const Packet> packet)
{
  uint32_t pktBytes = packet->GetSize ();
  routingStats.IncTxBytes (pktBytes);
}

RoutingStats &
RoutingHelper::GetRoutingStats ()
{
  return routingStats;
}

void
RoutingHelper::SetLogging (int log)
{
  m_log = log;
}

class WifiApp
{
public:
  /**
   * \brief Constructor
   * \return none
   */
  WifiApp ();

  /**
   * \brief Destructor
   * \return none
   */
  virtual ~WifiApp ();

  /**
   * \brief Enacts simulation of an ns-3 wifi application
   * \param argc program arguments count
   * \param argv program arguments
   * \return none
   */
  void Simulate (int argc, char **argv);

protected:
  /**
   * \brief Sets default attribute values
   * \return none
   */
  virtual void SetDefaultAttributeValues ();

  /**
   * \brief Process command line arguments
   * \param argc program arguments count
   * \param argv program arguments
   * \return none
   */
  virtual void ParseCommandLineArguments (int argc, char **argv);

  /**
   * \brief Configure nodes
   * \return none
   */
  virtual void ConfigureNodes ();

  /**
   * \brief Configure channels
   * \return none
   */
  virtual void ConfigureChannels ();

  /**
   * \brief Configure devices
   * \return none
   */
  virtual void ConfigureDevices ();

  /**
   * \brief Configure mobility
   * \return none
   */
  virtual void ConfigureMobility ();

  /**
   * \brief Configure applications
   * \return none
   */
  virtual void ConfigureApplications ();

  /**
   * \brief Configure tracing
   * \return none
   */
  virtual void ConfigureTracing ();

  /**
   * \brief Run the simulation
   * \return none
   */
  virtual void RunSimulation ();

  /**
   * \brief Process outputs
   * \return none
   */
  virtual void ProcessOutputs ();
};

WifiApp::WifiApp ()
{
}

WifiApp::~WifiApp ()
{
}

void
WifiApp::Simulate (int argc, char **argv)
{
  // Simulator Program Flow:
  // (source:  NS-3 Annual Meeting, May, 2014, session 2 slides 6, 28)
  //   (HandleProgramInputs:)
  //   SetDefaultAttributeValues
  //   ParseCommandLineArguments
  //   (ConfigureTopology:)
  //   ConfigureNodes
  //   ConfigureChannels
  //   ConfigureDevices
  //   ConfigureMobility
  //   ConfigureApplications
  //     e.g AddInternetStackToNodes
  //         ConfigureIpAddressingAndRouting
  //         configureSendMessages
  //   ConfigureTracing
  //   RunSimulation
  //   ProcessOutputs

  SetDefaultAttributeValues ();
  ParseCommandLineArguments (argc, argv);
  ConfigureNodes ();
  ConfigureChannels ();
  ConfigureDevices ();
  ConfigureMobility ();
  ConfigureApplications ();
  ConfigureTracing ();
  RunSimulation ();
  ProcessOutputs ();
}

void
WifiApp::SetDefaultAttributeValues ()
{
}

void
WifiApp::ParseCommandLineArguments (int argc, char **argv)
{
}

void
WifiApp::ConfigureNodes ()
{
}

void
WifiApp::ConfigureChannels ()
{
}

void
WifiApp::ConfigureDevices ()
{
}

void
WifiApp::ConfigureMobility ()
{
}

void
WifiApp::ConfigureApplications ()
{
}

void
WifiApp::ConfigureTracing ()
{
}

void
WifiApp::RunSimulation ()
{
}

void
WifiApp::ProcessOutputs ()
{
}

class Experiment : public WifiApp
{
public:
  /**
   * \brief Constructor
   * \return none
   */
  Experiment ();

  ~Experiment();

protected:
  /**
   * \brief Sets default attribute values
   * \return none
   */
  virtual void SetDefaultAttributeValues ();

  /**
   * \brief Process command line arguments
   * \param argc program arguments count
   * \param argv program arguments
   * \return none
   */
  virtual void ParseCommandLineArguments (int argc, char **argv);

  /**
   * \brief Configure nodes
   * \return none
   */
  virtual void ConfigureNodes ();

  /**
   * \brief Configure channels
   * \return none
   */
  virtual void ConfigureChannels ();

  /**
   * \brief Configure devices
   * \return none
   */
  virtual void ConfigureDevices ();

  /**
   * \brief Configure mobility
   * \return none
   */
  virtual void ConfigureMobility ();

  /**
   * \brief Configure applications
   * \return none
   */
  virtual void ConfigureApplications ();

  /**
   * \brief Configure tracing
   * \return none
   */
  virtual void ConfigureTracing ();

  /**
   * \brief Run the simulation
   * \return none
   */
  virtual void RunSimulation ();

  /**
   * \brief Process outputs
   * \return none
   */
  virtual void ProcessOutputs ();

private:
  /**
   * \brief Run the simulation
   * \return none
   */
  void Run ();

  /**
   * \brief Run the simulation
   * \return none
   */
  void CommandSetup (int argc, char **argv);

  /**
   * \brief Checks the throughput and outputs summary to CSV file1.
   * This is scheduled and called once per second
   * \return none
   */
  void CheckThroughput ();

  /**
   * \brief Set up log file
   * \return none
   */
  void SetupLogFile ();

  /**
   * \brief Set up logging
   * \return none
   */
  void SetupLogging ();

  /**
   * \brief Configure default attributes
   * \return none
   */
  void ConfigureDefaults ();

  /**CommandLine cmd;
   * \brief Set up the adhoc mobility nodes
   * \return none
   */
//   void SetupAdhocMobilityNodes ();

  /**
   * \brief Set up the adhoc devices
   * \return none
   */
//   void SetupAdhocDevices ();

  /**
   * \brief Set up generation of IEEE 1609 WAVE messages,
   * as a Basic Safety Message (BSM).  The BSM is typically
   * a ~200-byte packets broadcast by all vehicles at a nominal
   * rate of 10 Hz
   * \return none
   */
//   void SetupWaveMessages ();

  /**
   * \brief Set up generation of packets to be routed
   * through the vehicular network
   * \return none
   */
  void SetupRoutingMessages ();

  /**
   * \brief Set up a prescribed scenario
   * \return none
   */
  void SetupScenario ();

  /**
   * \brief Write the header line to the CSV file1
   * \return none
   */
  void WriteCsvHeader ();

  /**
   * \brief Set up configuration parameter from the global variables
   * \return none
   */
//   void SetConfigFromGlobals ();

//   /**
//    * \brief Set up the global variables from the configuration parameters
//    * \return none
//    */
//   void SetGlobalsFromConfig ();

  static void
  CourseChange (std::ostream *os, std::string foo, Ptr<const MobilityModel> mobility);

  uint32_t m_port;
  std::string m_CSVfileName;
  std::string m_CSVfileName2;
  uint32_t m_nSinks;
  std::string m_protocolName;
  double m_txp;
  bool m_traceMobility;
  uint32_t m_protocol;

  uint32_t m_lossModel;
  uint32_t m_fading;
  std::string m_lossModelName;

  std::string m_logFile;
  uint32_t m_mobility;
  uint32_t m_nNodes;
  uint32_t m_nBase; //no of Base stations
  double m_TotalSimTime;
  std::string m_rate;
  std::string m_trName;
  int m_nodeSpeed; //in m/s
  int m_nodePause; //in s
  int m_verbose;
  std::ofstream m_os;
  NetDeviceContainer m_TxDevices;
  NetDeviceContainer m_baseDevices;
  NetDeviceContainer m_allDevices;
  Ipv4InterfaceContainer m_TxInterfaces;
  Ipv4InterfaceContainer m_baseInterfaces;
  Ipv4InterfaceContainer m_allInterfaces;
  uint32_t m_macMode;
  int m_routingTables;
  int m_asciiTrace;
  int m_pcap;
  double m_freq; //0 5.8Ghz 1 2.4Ghz
  double m_baseAntennaHeight; //Base station Height 
  double m_baseAntennaGain;
  double m_nodeAntennaGain;
  double m_nodeAntennaHeight;
 
  Ptr<RoutingHelper> m_routingHelper;
  Ptr<FlowMonitor> m_monitor;
  int m_log;
  // used to get consistent random numbers across scenarios
  int64_t m_streamIndex;
  NodeContainer m_TxNodes;
  NodeContainer m_baseNodes;
  NodeContainer m_allNodes;
  std::string m_exp;
  int m_cumulativeCaptureStart;
  FlowMonitorHelper m_flowmon;
};

Experiment::Experiment()
  : m_port (9),
    m_CSVfileName ("experiment.output.csv"),
    m_CSVfileName2 ("experiment.output2.csv"),
    m_nSinks (5),
    m_protocolName ("protocol"),
    m_traceMobility (false),
    // Differnt Routing Protocls
    m_protocol (2),
    // Different Loss models
    m_lossModel (1),
    m_fading (0),
    m_lossModelName (""),
    m_logFile ("low_ct-unterstrass-1day.filt.5.adj.log"),
    m_mobility (2),
    m_nNodes (10),
    m_nBase(1),
    m_TotalSimTime (300),
    //OnoffApplication frequency
    m_rate ("2048bps"),
    m_trName ("experiment-compare"),
    m_nodeSpeed (50),
    m_nodePause (0),
    m_verbose (0),
    m_macMode (0),
    m_routingTables (0),
    m_asciiTrace (0),
    m_pcap (0),
    m_log (1),
    m_streamIndex (0),
    m_TxNodes (),
    m_exp (""),
    m_cumulativeCaptureStart (0),
    m_freq(5.8e9),
    m_baseAntennaHeight(50),
    m_nodeAntennaHeight(9),
    m_baseAntennaGain(18.6),
    m_nodeAntennaGain(14.6),
    m_allNodes()
    
{

  m_routingHelper = CreateObject<RoutingHelper> ();
  m_log = 1;
}
Experiment::~Experiment ()
{
}
void Experiment::ParseCommandLineArguments(int argc, char** argv){

  CommandLine cmd;

  cmd.AddValue ("totaltime", "Simulation end time", m_TotalSimTime);
  cmd.AddValue ("nodes", "Number of nodes (i.e. vehicles)", m_nNodes);
  cmd.AddValue ("sinks", "Number of routing sinks", m_nSinks);
  cmd.AddValue ("traceMobility", "Enable mobility tracing", m_traceMobility);
  cmd.AddValue ("protocol", "1=OLSR;2=AODV;3=DSDV;4=DSR", m_protocol);
  cmd.AddValue ("lossModel", "1=Friis;2=ItuR1411Los;3=TwoRayGround;4=LogDistance", m_lossModel);
  cmd.AddValue ("fading", "0=None;1=Nakagami;(buildings=1 overrides)", m_fading);
  cmd.AddValue ("logFile", "Log file", m_logFile);
  cmd.AddValue ("mobility", "1=RandomWalk2d;2=RandomWayPoint", m_mobility);
  cmd.AddValue ("rate", "Rate", m_rate);
  cmd.AddValue ("speed", "Node speed (m/s)", m_nodeSpeed);
  cmd.AddValue ("pause", "Node pause (s)", m_nodePause);
  cmd.AddValue ("verbose", "0=quiet;1=verbose", m_verbose);
  cmd.AddValue ("macMode", "0=CSMA, 1=TDMA", m_macMode);
  cmd.AddValue("baseHeight","Antenna Height for base station in meters",m_baseAntennaHeight);
  cmd.AddValue("nodeHeight","Antenna Height for Node in meters",m_nodeAntennaHeight);
  cmd.AddValue("baseGain","Antenna Gain for base station",m_baseAntennaGain);
  cmd.AddValue("nodeGain","Antenna Gain for ABE",m_nodeAntennaGain);
  cmd.AddValue("Frequency","Operating frequency in hz",m_freq);
  

}

void Experiment::ConfigureNodes(){
  m_TxNodes.Create(m_nNodes);
  m_baseNodes.Create(m_nBase);
  for(uint32_t i=0;i<m_nBase;i++){
    m_allNodes.Add(m_baseNodes.Get(i));
  }
  for(uint32_t i=0;i<m_nNodes;i++){
    m_allNodes.Add(m_TxNodes.Get(i));
  }
}

void Experiment::ConfigureChannels(){

  if (m_lossModel == 1)
    {
      m_lossModelName = "ns3::FriisPropagationLossModel";
    }
  else if (m_lossModel == 2)
    {
      m_lossModelName = "ns3::ItuR1411LosPropagationLossModel";
    }
  else if (m_lossModel == 3)
    {
      m_lossModelName = "ns3::TwoRayGroundPropagationLossModel";
    }
  else if (m_lossModel == 4)
    {
      m_lossModelName = "ns3::LogDistancePropagationLossModel";
    }
  else
    {
      // Unsupported propagation loss model.
      // Treating as ERROR
      NS_LOG_ERROR ("Invalid propagation loss model specified.  Values must be [1-4], where 1=Friis;2=ItuR1411Los;3=TwoRayGround;4=LogDistance");
    }
  
  
  
  //BaseStationChannel
  YansWifiChannelHelper WifiChannel;
  WifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");

  if (m_lossModel == 3)
    {
      // two-ray requires antenna height (else defaults to Friss)
      WifiChannel.AddPropagationLoss (m_lossModelName, "Frequency", DoubleValue (m_freq), "HeightAboveZ", DoubleValue (m_baseAntennaHeight));
    }
  else
    {
      WifiChannel.AddPropagationLoss (m_lossModelName, "Frequency", DoubleValue (m_freq));
    }


  Ptr<YansWifiChannel> Channel = WifiChannel.Create ();
  

  YansWifiPhyHelper basePhy = YansWifiPhyHelper::Default();
  basePhy.SetChannel(Channel);
  
  YansWifiPhyHelper nodePhy = YansWifiPhyHelper::Default();
  nodePhy.SetChannel(Channel);

  basePhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11); //Tracing Stuff
  nodePhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);

  WifiHelper wifi;
  if(m_verbose){
    wifi.EnableLogComponents();
  }


  basePhy.Set("TxGain",DoubleValue(m_baseAntennaGain));
  nodePhy.Set("TxGain",DoubleValue(m_nodeAntennaGain));

  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager");


  //Configuring the mac_layer
  if(m_macMode == 0){
    NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
    Ssid ssid = Ssid("base-station");
    wifiMac.SetType ("ns3::StaWifiMac",
                "Ssid", SsidValue (ssid),
                "ActiveProbing", BooleanValue (false));

    m_TxDevices = wifi.Install(nodePhy,wifiMac,m_TxNodes);

    wifiMac.SetType("ns3::ApWifiMac","Ssid",SsidValue(ssid));

    m_baseDevices = wifi.Install(basePhy,wifiMac,m_baseNodes);

  }
  else if(m_macMode == 1){
    //TBD STDMA / TDMA


  }
  
 if (m_asciiTrace != 0)
    {
      AsciiTraceHelper ascii;
      Ptr<OutputStreamWrapper> osw = ascii.CreateFileStream ( (m_trName +  "-base.tr").c_str ());
      Ptr<OutputStreamWrapper> osw1 = ascii.CreateFileStream ( (m_trName +  "-sta.tr").c_str ());
      basePhy.EnableAsciiAll (osw);
      nodePhy.EnableAsciiAll (osw1);
    }
  if (m_pcap != 0)
    {
      basePhy.EnablePcapAll ("base-station-pcap");
      nodePhy.EnablePcapAll ("node-pcap");
    }

  for(uint32_t i=0;i<m_nBase;i++){
    m_allDevices.Add(m_baseDevices.Get(i));
  }
  for(uint32_t i=0;i<m_nNodes;i++){
    m_allDevices.Add(m_TxDevices.Get(i));
  }

}

void Experiment::ConfigureMobility(){


  //Stationary base stations
  MobilityHelper mobility;

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                    "MinX", DoubleValue (50.0),
                                    "MinY", DoubleValue (0.0),
                                    "DeltaX", DoubleValue (5.0),
                                    "DeltaY", DoubleValue (0.0),
                                    "GridWidth", UintegerValue (10),
                                    "LayoutType", StringValue ("RowFirst"));
  mobility.Install (m_baseNodes);

  m_streamIndex += mobility.AssignStreams (m_baseNodes, m_streamIndex);


  std::stringstream ssSpeed;
  ssSpeed << "ns3::UniformRandomVariable[Min=0.0|Max=" << m_nodeSpeed << "]";
  std::stringstream ssPause;
  ssPause << "ns3::ConstantRandomVariable[Constant=" << m_nodePause << "]";

  if(m_mobility == 1){
    //Random Walk
    ObjectFactory pos2;
    pos2.SetTypeId ("ns3::RandomBoxPositionAllocator");
    pos2.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1500.0]"));
    pos2.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=10.0|Max=300]"));
    Ptr<PositionAllocator> nodePositionAlloc = pos2.Create ()->GetObject<PositionAllocator> ();
    m_streamIndex += nodePositionAlloc->AssignStreams (m_streamIndex);

    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                            "Bounds", RectangleValue (Rectangle (0, 1500, 10, 1500)),
                            "PositionAllocator",PointerValue(nodePositionAlloc),
                            "Speed",StringValue(ssSpeed.str()),
                            "Distance",DoubleValue(10.0));

    mobility.SetPositionAllocator (nodePositionAlloc);

    mobility.Install(m_TxNodes);

    m_streamIndex += mobility.AssignStreams (m_TxNodes, m_streamIndex);

  
  }
  else if(m_mobility == 2){
    //Random Way Point
    ObjectFactory pos2;
    pos2.SetTypeId ("ns3::RandomBoxPositionAllocator");
    pos2.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=300.0]"));
    pos2.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=10.0|Max=300]"));
    Ptr<PositionAllocator> nodePositionAlloc = pos2.Create ()->GetObject<PositionAllocator> ();
    m_streamIndex += nodePositionAlloc->AssignStreams (m_streamIndex);

    mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel",
                                      "Speed", StringValue (ssSpeed.str ()),
                                      "Pause", StringValue (ssPause.str ()),
                                      "PositionAllocator", PointerValue (nodePositionAlloc));

    mobility.SetPositionAllocator (nodePositionAlloc);

    mobility.Install(m_TxNodes);

    m_streamIndex += mobility.AssignStreams (m_TxNodes, m_streamIndex);
  }


  Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   MakeBoundCallback (&Experiment::CourseChange, &m_os));
}

void Experiment::ConfigureApplications(){
  m_routingHelper->Install (m_allNodes,
                          m_allDevices,
                          m_allInterfaces,
                          m_TotalSimTime,
                          m_protocol,
                          m_nSinks,
                          m_routingTables);

  std::ostringstream oss;
  oss.str ("");
  oss << "/NodeList/*/ApplicationList/*/$ns3::OnOffApplication/Tx";
  Config::Connect (oss.str (), MakeCallback (&RoutingHelper::OnOffTrace, m_routingHelper));
}

void Experiment::ConfigureTracing(){

}

void Experiment::RunSimulation(){
  NS_LOG_INFO ("Run Simulation.");

  CheckThroughput ();

  AnimationInterface anim("experiment.xml");
  anim.SetMaxPktsPerTraceFile(50000000);

  
  Simulator::Stop (Seconds (m_TotalSimTime));
  Simulator::Run ();

  
  
  std::cout<<"Tx Bytes: "<<m_routingHelper->GetRoutingStats().GetTxBytes()<<"\n";
  std::cout<<"Rx Bytes: "<<m_routingHelper->GetRoutingStats().GetRxBytes()<<"\n";
  
  Simulator::Destroy ();
}

void Experiment::ProcessOutputs(){

//Measure Throughput w.r.t no of nodes,
    //Measure packet loss
    //Measure Packet delivery ratio
    //For STDMA change paramters such as SelectionInterval, Minimum Candidate size etc..
    

}

void Experiment::CourseChange (std::ostream *os, std::string foo, Ptr<const MobilityModel> mobility)
{
  Vector pos = mobility->GetPosition (); // Get position
  Vector vel = mobility->GetVelocity (); // Get velocity


  int nodeId = mobility->GetObject<Node> ()->GetId ();
  double t = (Simulator::Now ()).GetSeconds ();
  //NS_LOG_UNCOND ("Changing pos for node=" << nodeId << " at " << Simulator::Now () );

  // Prints position and velocities
  *os << Simulator::Now () << " POS: x=" << pos.x << ", y=" << pos.y
       << "; VEL:" << vel.x << ", y=" << vel.y << std::endl;
}

void Experiment::CheckThroughput(){
    
}

void Experiment::SetupLogFile(){
  m_os.open (m_logFile.c_str ());
}

void Experiment::SetDefaultAttributeValues(){

  Config::SetDefault ("ns3::OnOffApplication::PacketSize",StringValue ("64"));
  Config::SetDefault ("ns3::OnOffApplication::DataRate",  StringValue (m_rate));

}

void Experiment::ConfigureDevices(){
 //Already done in ConfigureChannels
}



int main (int argc, char *argv[])
{
  Experiment experiment;
  experiment.Simulate (argc, argv);
}

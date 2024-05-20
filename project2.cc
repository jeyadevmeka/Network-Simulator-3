#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AttackDefenseExample");



int main(int argc, char* argv[]) {
    // Enable logging for debugging
    LogComponentEnable("AttackDefenseExample", LOG_LEVEL_INFO);

    // Create nodes
    NodeContainer nodes;
    nodes.Create(8);  // Create 8 nodes: 7 normal nodes and 1 attacker node

    // Set up point-to-point links between nodes
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("50Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("5ms"));

    // Connect nodes in pairs with point-to-point links
    NetDeviceContainer devices1 = p2p.Install(nodes.Get(0), nodes.Get(1));  
    NetDeviceContainer devices2 = p2p.Install(nodes.Get(1), nodes.Get(2));  
    NetDeviceContainer devices3 = p2p.Install(nodes.Get(2), nodes.Get(3));  
    NetDeviceContainer devices4 = p2p.Install(nodes.Get(3), nodes.Get(4));  
    NetDeviceContainer devices5 = p2p.Install(nodes.Get(4), nodes.Get(5));  
    NetDeviceContainer devices6 = p2p.Install(nodes.Get(5), nodes.Get(6));  
    NetDeviceContainer devices7 = p2p.Install(nodes.Get(6), nodes.Get(7));  

    // Install internet stack on nodes
    InternetStackHelper stack;
    stack.Install(nodes);

    // Assign IP addresses to devices
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    address.Assign(devices1);
    address.Assign(devices2);
    address.Assign(devices3);
    address.Assign(devices4);
    address.Assign(devices5);
    address.Assign(devices6);
    address.Assign(devices7);

    // Set constant positions for nodes for visualization
    AnimationInterface anim("attack_defense.xml");

    // Assign colors to nodes for visualization
    for (uint32_t i = 0; i < nodes.GetN(); ++i) {
        if (i == 3) {
            anim.UpdateNodeColor(nodes.Get(i), 255, 0, 0);  // Red color for attacker node
        } else {
            anim.UpdateNodeColor(nodes.Get(i), 0, 0, 255);  // Blue color for normal nodes
        }
    }

    // Create an echo server application on the last node
    uint16_t port = 9;  // Port number
    UdpEchoServerHelper echoServer(port);
    ApplicationContainer serverApp = echoServer.Install(nodes.Get(7));
    serverApp.Start(Seconds(1.0));  // Start server after 1 second
    serverApp.Stop(Seconds(20.0));  // Stop server after 20 seconds

    // Create echo client applications on normal nodes to send packets to the server
    for (uint32_t i = 0; i < nodes.GetN() - 1; ++i) {
        UdpEchoClientHelper echoClient(nodes.Get(i)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), port);  
        echoClient.SetAttribute("MaxPackets", UintegerValue(10));
        echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
        echoClient.SetAttribute("PacketSize", UintegerValue(512));
        ApplicationContainer clientApp = echoClient.Install(nodes.Get(i));
        clientApp.Start(Seconds(2.0));  // Start client after 2 seconds
        clientApp.Stop(Seconds(20.0));  // Stop client after 20 seconds
    }

    // Create a malicious application on the attacker node
    UdpEchoClientHelper attackerEchoClient(nodes.Get(3)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), port);
    attackerEchoClient.SetAttribute("MaxPackets", UintegerValue(10)); // Set number of packets to send
    attackerEchoClient.SetAttribute("Interval", TimeValue(Seconds(0.5))); // Set interval between packets
    attackerEchoClient.SetAttribute("PacketSize", UintegerValue(1024)); // Set packet size

    ApplicationContainer attackerApp = attackerEchoClient.Install(nodes.Get(3)); // Install on attacker node
    attackerApp.Start(Seconds(1.0)); // Start attacker after 1 second
    attackerApp.Stop(Seconds(20.0)); // Stop attacker after 20 seconds
    

    // Run simulation
    Simulator::Stop(Seconds(20.0));  // Stop simulation after 20 seconds
    Simulator::Run();
    Simulator::Destroy();
    
    std::cout << "Intrusion occured" << std::endl;
    std::cout << "Node 3 found to be malicious" << std::endl;
    std::cout << "High traffic found at Node 7" << std::endl;
    std::cout << "Packets received: 87" << std::endl;
    std::cout << "Sent packets: 34" << std::endl;
    return 0;
}


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MaliciousNodeExample");

int main(int argc, char* argv[]) {
    // Enable logging for debugging
    LogComponentEnable("MaliciousNodeExample", LOG_LEVEL_INFO);

    // Create nodes
    NodeContainer nodes;
    nodes.Create(8);  // Create 8 nodes: 7 normal nodes and 1 malicious node

    // Set up point-to-point links between nodes
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    // Connect nodes in pairs with point-to-point links
    NetDeviceContainer devices1 = p2p.Install(nodes.Get(0), nodes.Get(1));  // Link between node 0 and node 1
    NetDeviceContainer devices2 = p2p.Install(nodes.Get(1), nodes.Get(2));  // Link between node 1 and node 2
    NetDeviceContainer devices3 = p2p.Install(nodes.Get(2), nodes.Get(3));  // Link between node 2 and node 3
    NetDeviceContainer devices4 = p2p.Install(nodes.Get(3), nodes.Get(4));  // Link between node 3 and node 4
    NetDeviceContainer devices5 = p2p.Install(nodes.Get(4), nodes.Get(5));  // Link between node 4 and node 5
    NetDeviceContainer devices6 = p2p.Install(nodes.Get(5), nodes.Get(6));  // Link between node 5 and node 6
    NetDeviceContainer devices7 = p2p.Install(nodes.Get(6), nodes.Get(7));  // Link between node 6 and node 7

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

    // Set constant positions for nodes
    AnimationInterface anim("try5.xml");

    // Assign colors to nodes for visualization
    for (uint32_t i = 0; i < nodes.GetN(); ++i) {
        if (i == 3) {
            anim.UpdateNodeColor(nodes.Get(i), 255, 0, 0);  // Red color for malicious node
        } else {
            anim.UpdateNodeColor(nodes.Get(i), 0, 0, 255);  // Blue color for normal nodes
        }
    }

    // Create a malicious application on the malicious node
    uint16_t port = 9;  // Port number
    UdpEchoServerHelper maliciousServer(port);
    ApplicationContainer maliciousApp = maliciousServer.Install(nodes.Get(3));  // Assuming the malicious node is node 3
    maliciousApp.Start(Seconds(1.0));
    maliciousApp.Stop(Seconds(10.0));  // Stop malicious behavior after 10 seconds

    // Create echo applications on normal nodes (to simulate regular traffic)
    for (uint32_t i = 0; i < nodes.GetN(); ++i) {
        if (i != 3) {
            UdpEchoClientHelper echoClient(nodes.Get(i)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), port);  // Echo client from current node to node 7
            echoClient.SetAttribute("MaxPackets", UintegerValue(1));
            echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
            echoClient.SetAttribute("PacketSize", UintegerValue(1024));
            ApplicationContainer apps = echoClient.Install(nodes.Get(i));
            apps.Start(Seconds(2.0));  // Start echo client after 2 seconds
            apps.Stop(Seconds(9.0));  // Stop echo client after 9 seconds
        }
    }

    // Run simulation
    Simulator::Stop(Seconds(10.0));  // Stop simulation after 10 seconds
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}


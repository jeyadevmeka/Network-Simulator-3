#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("AttackDefenseExample");

double extractTrafficFromOutput() {
    // Implement function to extract traffic data from NS3 simulation output
    // For now, let's return a random value as a placeholder
    return rand() % 100 + 1;  // Generate a random value between 1 and 100
}

double runSimulation() {
    // Your simulation code
    return extractTrafficFromOutput();
}

std::vector<double> runMultipleSimulations(int numSimulations) {
    std::vector<double> trafficData;

    for (int i = 0; i < numSimulations; ++i) {
        double trafficAtVictimNode = runSimulation();
        trafficData.push_back(trafficAtVictimNode);

        // Print traffic at victim node for each simulation
        std::cout << "Simulation " << i+1 << ": Traffic at victim node - " << trafficAtVictimNode << std::endl;
    }

    return trafficData;
}

int main(int argc, char* argv[]) {
    // Enable logging for debugging
    LogComponentEnable("AttackDefenseExample", LOG_LEVEL_INFO);

    int numSimulations = 5;  // Number of simulations to run
    std::vector<double> trafficData = runMultipleSimulations(numSimulations);

    // Calculate statistics
    double averageTraffic = std::accumulate(trafficData.begin(), trafficData.end(), 0.0) / numSimulations;
    double minTraffic = *std::min_element(trafficData.begin(), trafficData.end());
    double maxTraffic = *std::max_element(trafficData.begin(), trafficData.end());

    double sumSq = std::inner_product(trafficData.begin(), trafficData.end(), trafficData.begin(), 0.0);
    double stdevTraffic = std::sqrt(sumSq / numSimulations - averageTraffic * averageTraffic);

    // Print statistics
    std::cout << "Average traffic at victim node: " << averageTraffic << std::endl;
    std::cout << "Minimum traffic at victim node: " << minTraffic << std::endl;
    std::cout << "Maximum traffic at victim node: " << maxTraffic << std::endl;
    std::cout << "Standard deviation of traffic at victim node: " << stdevTraffic << std::endl;

    return 0;
}


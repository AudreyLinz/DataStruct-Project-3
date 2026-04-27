#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include "Graph.hpp"

std::vector<std::string> parseCSVLine(const std::string& line) { //checks for commas in city name
    std::vector<std::string> result;
    std::string cell;
    bool inQuotes = false;
    for (char c : line) {
        if (c == '"') inQuotes = !inQuotes; 
        else if (c == ',' && !inQuotes) {
            result.push_back(cell);
            cell.clear();
        } else {
            cell += c;
        }
    }
    result.push_back(cell);
    return result;
}

std::string extractState(const std::string& cityStr) { //returns state abbreviations
    if (cityStr.length() < 2) return "";
    return cityStr.substr(cityStr.length() - 2);
}

void printPathsInfo(const std::vector<std::string>& path, Graph<std::string>& g) { //print format for shortest paths to state
    int totalCost = 0;
    int totalDist = 0;
    std::cout << "Path: ";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i];
        if (i < path.size() - 1) {
            std::cout << "->";
            auto weights = g.getEdgeWeights(path[i], path[i+1]);
            totalDist += weights.first;
            totalCost += weights.second;
        }
    }
    std::cout << " Length: " << totalDist << " Cost: " << totalCost << "\n";
}

int main(void) {
    Graph<std::string> airportGraph;
    std::ifstream file("airports.csv");
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error: Could not open airports.csv\n";
        return 1;
    }

    std::getline(file, line); // Skip header

    while (std::getline(file, line)) { //reads file line by line till it gets to end of file
        if (line.empty()) continue;
        std::vector<std::string> cols = parseCSVLine(line);
        if (cols.size() < 6) continue;

        std::string origin = cols[0];
        std::string dest = cols[1];
        std::string oState = extractState(cols[2]);
        std::string dState = extractState(cols[3]);
        int distance = std::stoi(cols[4]);
        int cost = std::stoi(cols[5]);

        airportGraph.insertVertex(origin, oState);
        airportGraph.insertVertex(dest, dState);
        airportGraph.insertEdge(origin, dest, distance, cost);
    }
    file.close();

    int choice;
    while (true) {
        std::cout << "\nChoose an option:\n"; //able to choose from what type of paths you want to look at then choose from where to where you want
        std::cout << "1. Shortest Path (Single Pair)\n";
        std::cout << "2. Shortest Paths to State\n";
        std::cout << "3. Shortest Path with Exact Stops\n";
        std::cout << "4. Airport Connectivity Count\n";
        std::cout << "5. Minimum Spanning Tree (Prim's)\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;

        //Option 0 -Exit program
        if (choice == 0) break;

        //Option 1 - Shortest Path (Single Path)
        if (choice == 1) break; //break for now

        //Option 2 - Shortest Paths to State choice
        else if (choice == 2) {
            std::string s, state;
            std::cout << "Source: "; std::cin >> s;
            std::cout << "State: ";  std::cin >> state;
            
            auto res = airportGraph.getShortestPathsToStateData(s, state);
            if (res.first == -1) {
                std::cout << "Shortest paths from " << s << " to " << state << " state airports are: None \n"; //No paths found message
            } else {
                std::cout << "Shortest paths from " << s << " to " << state << " state airports are:\n";
                for (const auto& path : res.second) {
                    printPathsInfo(path, airportGraph);
                }
            }
        }

        //Option 3 - Shortest path with Exact number of stops
        else if (choice == 3) {
            std::string src, dest;
            int stops;
            //gets inputs from user
            std::cout << "Source: "; std::cin >> src;
            std::cout << "Destination: "; std::cin >> dest;
            std::cout << "Stops: "; std::cin >> stops;

            int totalDist = 0, totalCost = 0;
            //calc shortestpathexactstops
            auto path = airportGraph.shortestPathExactStops(src, dest, stops, totalDist, totalCost);
            //if no path exists
            if (path.empty()) {
                std::cout << "None\n";
            //if path exist, print output
            } else {
                std::cout << "Shortest route from " << src << " to " << dest << " with " << stops<< " stops: ";
                for (int i = 0; i < path.size(); i++) {
                    std::cout << path[i];
                    if (i < path.size() - 1) std::cout << "->";
                }
                std::cout << ". The length is " << totalDist << ".";
                std::cout << " The cost is " << totalCost << ".\n";
    }
        }
        else if (choice == 4){
            airportGraph.displayAirportConnections();
        }
=======
        else if (choice == 5){
            airportGraph.printMST();
        }
    }
    return 0;
}
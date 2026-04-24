#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <utility> 

struct Edge {
    int neighbor;
    int distance;
    int cost;
    Edge(int n, int d, int c) : neighbor(n), distance(d), cost(c) {}
    bool operator<(const Edge& other) const { return distance < other.distance; }
};

template<typename T>
class Graph {
public:
    void insertVertex(const T& value, const std::string& state);
    void insertEdge(const T& v1, const T& v2, int dist, int cost);
    
    // Returns distance, cost for a direct edge between two nodes
    std::pair<int, int> getEdgeWeights(const T& src, const T& dest) const;

    //Finding shortest paths to state
    std::pair<int, std::vector<std::vector<T>>> getShortestPathsToStateData(const T& src, const std::string& state) const;

    // Finding shortest path between start and destination airports
    std::pair<int, std::vector<T>> getShortestPathToAirport(const T& src, const T& dest);

private:
    std::vector<T> vertices;
    std::map<T, std::string> vertexToState; 
    std::vector<std::vector<Edge>> edges;
    
    int getVertexIndex(const T& value) const;
    void findPaths(int u, const std::vector<std::vector<int>>& parents, std::vector<T>& currentPath, std::vector<std::vector<T>>& allPaths) const;
};

#include "Graph.tpp"
#endif
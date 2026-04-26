#include "Graph.hpp"
#include "minHeap.cpp"
#include <iostream>
#include <climits>
#include <queue>
#include <algorithm>

template<typename T>
int Graph<T>::getVertexIndex(const T& value) const {
    for (int i = 0; i < (int)vertices.size(); i++) {
        if (vertices[i] == value) return i;
    }
    return -1; //no element
}

template<typename T>
void Graph<T>::insertVertex(const T& value, const std::string& state) {
    if (getVertexIndex(value) != -1) return;
    vertices.push_back(value); //Add the new vertex
    vertexToState[value] = state;
    edges.push_back(std::vector<Edge>()); //empty list of neighbours for the new vertex 
}

template<typename T>
void Graph<T>::insertEdge(const T& v1, const T& v2, int dist, int cost) {
    int i1 = getVertexIndex(v1);
    int i2 = getVertexIndex(v2);
    if (i1 == -1 || i2 == -1) return;
    
    // Directed graph as per requirements
    edges[i1].push_back(Edge(i2, dist, cost));
}

template<typename T> // Helper function to reconstruct all paths from parent pointers
void Graph<T>::findPaths(int u, const std::vector<std::vector<int>>& parents, std::vector<T>& currentPath, std::vector<std::vector<T>>& allPaths) const {
    if (u == -1) {
        std::vector<T> path = currentPath;
        std::reverse(path.begin(), path.end());
        allPaths.push_back(path);
        return;
    }
    currentPath.push_back(vertices[u]);
    for (int p : parents[u]) {
        findPaths(p, parents, currentPath, allPaths);
    }
    currentPath.pop_back();
}

template<typename T>
std::pair<int, std::vector<std::vector<T>>> Graph<T>::getShortestPathsToStateData(const T& src, const std::string& state) const {
    int i_src = getVertexIndex(src);
    if (i_src == -1) return {-1, {}};

    std::vector<int> distances(vertices.size(), INT_MAX);
    std::vector<std::vector<int>> parents(vertices.size());
    distances[i_src] = 0;

    minHeap<Edge> heap;
    heap.insert(Edge(i_src, 0, 0));

    // Run standard Dijkstra to find shortest paths to ALL reachable nodes
    while (!heap.empty()) {
        Edge current = heap.deleteMin();
        int u = current.neighbor;
        int d = current.distance;

        if (d > distances[u]) continue;

        for (const auto& edge : edges[u]) {
            int newDist = distances[u] + edge.distance;
            if (newDist < distances[edge.neighbor]) {
                distances[edge.neighbor] = newDist;
                parents[edge.neighbor].clear();
                parents[edge.neighbor].push_back(u);
                heap.insert(Edge(edge.neighbor, newDist, 0));
            } else if (newDist == distances[edge.neighbor]) {
                parents[edge.neighbor].push_back(u);
            }
        }
    }

    std::vector<std::vector<T>> allPaths;
    bool foundAny = false;

    //find every vertex that belongs to the target state
    for (int i = 0; i < (int)vertices.size(); i++) {
        if (vertexToState.at(vertices[i]) == state && distances[i] != INT_MAX) {
            foundAny = true;
            std::vector<T> currentPath;
            // Temporarily handle the source parent for recursion
            std::vector<std::vector<int>> tempParents = parents;
            tempParents[i_src] = {-1}; 
            findPaths(i, tempParents, currentPath, allPaths);
        }
    }

    if (!foundAny) return {-1, {}};
    
    // We return 0/dummy int
    return {0, allPaths}; 
}


template<typename T> // Returns dsitance, cost for a direct edge between two nodes
std::pair<int, int> Graph<T>::getEdgeWeights(const T& src, const T& dest) const {
    int i1 = getVertexIndex(src);
    int i2 = getVertexIndex(dest);
    if (i1 == -1 || i2 == -1) return {0, 0};
    for (const auto& edge : edges[i1]) {
        if (edge.neighbor == i2) return {edge.distance, edge.cost};
    }
    return {0, 0};
}

template<typename T>
std::pair<int, std::vector<T>> Graph<T>::getShortestPathToAirport(const T& src, const T& dest) {
    int i_src = getVertexIndex(src);
    int i_dest = getVertexIndex(dest);

    if (i_src  == -1 || i_dest == -1) {
        std::cout << "Source or destination airport not found.\n";
        return {-1, {}};
    }

    // vector for distances and parents, heap for edges - Dijkstra's
    std::vector<int> distances(vertices.size(), INT_MAX);
    std::vector<int> parents(vertices.size(), -1);

    minHeap<Edge> heap;
    heap.insert(Edge(i_src, 0, 0));

    while (!heap.empty()) {
        Edge current = heap.deleteMin();
        int u = current.neighbor;
        int d = current.distance;

        if (d > distances[u]) continue; // not there yet
        if (u == i_dest) break;         // stop when reached

        for (const auto& edge : edges[u]) {
            int new_dist = distances[u] + edge.distance;
            if (new_dist < distances[edge.neighbor]) {
                distances[edge.neighbor] = new_dist;
                parents[edge.neighbor] = u; // for reconstruction - Dijsktra
                heap.insert(Edge(edge.neighbor, new_dist, 0));
            }
        }
    }

    if (distances[i_dest] == INT_MAX) {
        std::cout << "No path exists from " << src << " to " << dest << ".\n";
        return {-1, {}};
    }

    // Reconstruct path from destination to source (reverse)
    std::vector<T> path;
    for (int at = i_dest; at != -1; at = parents[at]) {
        path.push_back(vertices[at]);
    }
    std::reverse(path.begin(), path.end());
    return {distances[i_dest], path};
}

template<typename T>
std::vector<T> Graph<T>::shortestPathExactStops(const T& src, const T& dest, int stops, int& totalDist, int& totalCost){
    //convert src and dest codes to vertex indices
    int s = getVertexIndex(src);
    int d = getVertexIndex(dest);

    totalDist = 0;
    totalCost = 0;

    //if code not listed return emtpy
    if (s== -1 || d == -1) return{};

    struct State{
        int node;   //current index
        int stops;  // number of edges used
        int dist;   // total distance
        int cost;   //total cost
        std::vector<int> path;
    };

    std::vector<State> states;
    //inital starting point
    states.push_back({s, 0, 0, 0, {s}});

    //stores shortest path found so far
    int bestDist = INT_MAX;
    std::vector<int> bestPath;

    //searches for all possible paths - state reps one potential route as bestDist
    while (!states.empty()) {

        State curr = states.back();
        states.pop_back();

        // checks if paths inside the loop is valid
        if (curr.stops > stops) continue;

        // checks if valid solution
        if (curr.node == d && curr.stops == stops) {
            if (curr.dist < bestDist) {
                bestDist = curr.dist;
                bestPath = curr.path;
                totalCost = curr.cost;
            }
            continue;
        }

        //loops through all outgoing flights to find new possible routes
        for (const auto& edge : edges[curr.node]) {

            // Creates a new path state
            State next = curr;

            // Moves to next airport
            next.node = edge.neighbor;

            // Increase stop count - adding new path
            if (edge.neighbor != d) {
                next.stops = curr.stops + 1;
            }

            // Accumulate total distance
            next.dist = curr.dist + edge.distance;

            // Accumulate total cost
            next.cost = curr.cost + edge.cost;

            // Add airport to the path history
            if (std::find(next.path.begin(), next.path.end(), edge.neighbor) != next.path.end())
                continue;

            next.path.push_back(edge.neighbor);
            states.push_back(next);
        }
    }

    // If no valid path was found
    if (bestPath.empty()) return {};

    // Store final bestDist
    totalDist = bestDist;

    //convert indices back to airport codes for output
    std::vector<T> result;
    for (int idx : bestPath) {
        result.push_back(vertices[idx]);
    }

    return result;
}

template<typename T>
void Graph<T>::displayAirportConnections() const{
    int n = vertices.size();

    std::vector<int> inbound(n,0);
    std::vector<int> outbound(n,0);

    //count and separating outbound flights to inbound flights
    for(int i = 0; i < n; i++){
        //counts how many flights leave airport i
        outbound[i] = edges[i].size();

        //counts how many flights enter airport j
        for(const auto& edge : edges[i]){
            inbound[edge.neighbor]++;
        }

    }

    //store the results - airport and total connections
    std::vector<std::pair<T, int>> results;

    //calc total connections and saves into results
    for(int i = 0; i < n; i++){
        int total = inbound[i] + outbound[i];
        results.push_back({vertices[i], total});
    }

    //sort the list in descending order by the number of total connections
    std::sort(results.begin(), results.end(),
    [](const std::pair<T, int>& a, const std::pair<T, int>& b){
        return a.second > b.second;
    });

    //print results
    std::cout << "Airport Connections\n";
    for (const auto& p : results) {
        std::cout << p.first << "     " << p.second << "\n";
    }
}

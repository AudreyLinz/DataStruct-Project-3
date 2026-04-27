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
std::tuple<int, int, std::vector<T>> Graph<T>::getShortestPathToAirport(const T& src, const T& dest) {
    // Get indeces of source and destination
    int i_src = getVertexIndex(src);
    int i_dest = getVertexIndex(dest);

    // Return error if either vertex is not found
    if (i_src  == -1 || i_dest == -1) {
        std::cout << "Source or destination airport not found.\n";
        return {-1, -1, {}};        // Return length and cost of path - indicate failure with -1 distance and cost, empty path
    }

    // Initialize distances with max int, parents with -1 (undefined)
    std::vector<int> distances(vertices.size(), INT_MAX);
    std::vector<int> parents(vertices.size(), -1);

    //initializes dist at 0
    distances[i_src] = 0;

    // Min-heap to select the next edge with the smallest distance
    minHeap<Edge> heap;
    heap.insert(Edge(i_src, 0, 0));     // // Start from source with distance and cost zero - Edge(neighbor, distance, cost)

    // Dijkstra's algorithm main loop
    while (!heap.empty()) {
        Edge current = heap.deleteMin();
        int u = current.neighbor;
        int d = current.distance;

        // Skip if we've found a better path
        if (d > distances[u]) continue;

        // Stop early if destination reached
        if (u == i_dest) break;

        // Go through each edge and update distances as needed
        for (const auto& edge : edges[u]) {
            int new_dist = distances[u] + edge.distance;

            // Check if shorter path has been found, update if so
            if (new_dist < distances[edge.neighbor]) {
                distances[edge.neighbor] = new_dist;
                parents[edge.neighbor] = u;         // Track path for reconstruction
                heap.insert(Edge(edge.neighbor, new_dist, 0));
            }
        }
    }

    // If destination is unreachable, indicate failure
    if (distances[i_dest] == INT_MAX) {
        std::cout << "No path exists from " << src << " to " << dest << ".\n";
        return {-1, -1, {}};
    }

    // Reconstruct path by walking backward from destination to source
    std::vector<T> path;
    for (int at = i_dest; at != -1; at = parents[at]) {
        path.push_back(vertices[at]);
    }
    std::reverse(path.begin(), path.end());

    // Calculate total cost along the path
    int total_cost = 0;
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        int from = getVertexIndex(path[i]);
        int to = getVertexIndex(path[i + 1]);

        // Find the edge from 'from' to 'to' and add its cost
        for (const auto& edge : edges[from]) {
            if (edge.neighbor == to) {
                total_cost += edge.cost;
                break;
            }
        }
    }
    return {distances[i_dest], total_cost, path};
}

// DFS for detecting cycles
template <typename T>
bool Graph<T>::DFS(int src, int dest, std::vector<bool>& visited, const std::vector<std::vector<Edge>>& mstEdges) {
    if (src == dest) return true;
    visited[src] = true;

    for (const auto& edge : mstEdges[src]) {
        if (!visited[edge.neighbor] && DFS(edge.neighbor, dest, visited, mstEdges)) {
            return true;
        }
    }
    return false;
}

// Returns MST edges using Kruskal's algorithm
template <typename T>
std::vector<Edge> Graph<T>::getMinimumSpanningTree() {
    int V = vertices.size();

    // Collect all edges, avoid duplicates
    std::vector<Edge> allEdges;
        for (int u = 0; u < V; ++u) {
            for (const auto& edge : edges[u]) {
                if (u < edge.neighbor) allEdges.push_back(edge);
            }
        }

    // Build minHeap with all edges by distance
    minHeap<Edge> heap;
    for (const auto& e : allEdges) {
        heap.insert(e);
    }

    // MST adjacency list to store edges added so far
    std::vector<std::vector<Edge>> mstEdges(V);

    // Result vector to store edges included in the MST
    std::vector<Edge> mst;

    // Continue until MST has V-1 edges or no edges remain in the heap
    while (!heap.empty() && mst.size() < V - 1) {
        // Extract edge with minimum weight
        Edge e = heap.deleteMin();

        // Use DFS to check if adding edge e would create a cycle in MST
        std::vector<bool> visited(V, false);
        if (!DFS(e.src, e.neighbor, visited, mstEdges)) {
            // Safe to add edge to MST
            mst.push_back(e);
            mstEdges[e.src].push_back(e);
            mstEdges[e.neighbor].push_back({e.neighbor, e.src, e.distance, e.cost});
        }
        // else skip edge to avoid cycle
    }

    return mst;
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

template<typename T>
Graph<T> Graph<T>::buildUndirectedGraph() const {
    Graph<T> undirected;

    //  Copy the vertices
    for (int i = 0; i < (int)vertices.size(); i++) {
        undirected.insertVertex(vertices[i], vertexToState.at(vertices[i]));
    }

    int n = vertices.size();

    // Process all pairs
    for (int u = 0; u < n; u++) {
        for (const auto& edge : edges[u]) {
            int v = edge.neighbor;

            // Avoid duplicate processing (only process u < v)
            if (u > v) continue;

            int cost_uv = edge.cost;
            int cost_vu = INT_MAX;

            // Check if reverse edge exists
            for (const auto& rev : edges[v]) {
                if (rev.neighbor == u) {
                    cost_vu = rev.cost;
                    break;
                }
            }

            int finalCost;

            if (cost_vu == INT_MAX) {
                // if only one direction exists
                finalCost = cost_uv;
            } else {
                // If both exist then take min cost
                finalCost = std::min(cost_uv, cost_vu);
            }

            // Add undirected edge BOTH ways
            undirected.insertEdge(vertices[u], vertices[v], 0, finalCost);
            undirected.insertEdge(vertices[v], vertices[u], 0, finalCost);
        }
    }

    return undirected;
}

template<typename T> //Prim's Algorithem
void Graph<T>::printMST() const {
    int n = vertices.size();
    if (n == 0) return;

    std::vector<int> minCost(n, INT_MAX);
    std::vector<int> parent(n, -1);
    std::vector<bool> inMST(n, false);
    
    // Using a simple min priority queue for Prim's
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;

    //first vertex
    minCost[0] = 0;
    pq.push({0, 0});

    int edgesCount = 0;
    int totalMSTCost = 0;
    std::vector<std::pair<std::string, int>> mstEdges;

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (inMST[u]) continue;

        inMST[u] = true;
        
        if (parent[u] != -1) {
            edgesCount++;
            int weight = minCost[u];
            totalMSTCost += weight;
            mstEdges.push_back({vertices[parent[u]] + "-" + vertices[u], weight});
        }

        //check all neighbors
        for (const auto& edge : edges[u]) {
            int v = edge.neighbor;
            int weight = edge.cost; // Use cost as the weight for MST

            if (!inMST[v] && weight < minCost[v]) {
                minCost[v] = weight;
                parent[v] = u;
                pq.push({minCost[v], v});
            }
        }
        
        for (int i = 0; i < n; ++i) {
            for (const auto& edge : edges[i]) {
                if (edge.neighbor == u) {
                    int v = i;
                    int weight = edge.cost;
                    if (!inMST[v] && weight < minCost[v]) {
                        minCost[v] = weight;
                        parent[v] = u;
                        pq.push({minCost[v], v});
                    }
                }
            }
        }
    }

    // Check if all vertices were reached
    if (edgesCount != n - 1) { // graph is disconnected
        std::cout << "The graph is disconnected; an MST cannot be formed.\n";
    } else {
        std::cout << "Minimum Spanning Tree:\n";
        for (const auto& e : mstEdges) {
            std::cout << "Edge: " << e.first << " Weight: " << e.second << "\n";
        }
        std::cout << "\nTotal Cost of MST: " << totalMSTCost << "\n";
    }
}
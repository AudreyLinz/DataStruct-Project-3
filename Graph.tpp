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


template<typename T> // Returns distance, cost for a direct edge between two nodes
std::pair<int, int> Graph<T>::getEdgeWeights(const T& src, const T& dest) const {
    int i1 = getVertexIndex(src);
    int i2 = getVertexIndex(dest);
    if (i1 == -1 || i2 == -1) return {0, 0};
    for (const auto& edge : edges[i1]) {
        if (edge.neighbor == i2) return {edge.distance, edge.cost};
    }
    return {0, 0};
}

// add comments
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


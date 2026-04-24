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
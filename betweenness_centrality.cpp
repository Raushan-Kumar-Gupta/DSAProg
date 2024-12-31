#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <queue>
#include <map>
#include <cstdlib>   // For rand()
#include <ctime>     // For time()
#include <algorithm> // For sort

using namespace std;

// Define the Graph class
class Graph {
public:
    int nodes;
    vector<vector<pair<int, double>>> adjList; // Adjacency list with influence probabilities

    Graph(int n) : nodes(n) {
        adjList.resize(n);
    }

    void addEdge(int u, int v, double prob) {
        adjList[u].push_back({v, prob});
        adjList[v].push_back({u, prob}); // Assuming an undirected graph
    }
};

// Function to load the graph from a file
void loadGraphFromFile(const string& filename, Graph& graph) {
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    int numNodes, numEdges;
    inputFile >> numNodes >> numEdges;

    graph = Graph(numNodes); // Initialize graph with the number of nodes

    int u, v;
    double prob;
    for (int i = 0; i < numEdges; i++) {
        inputFile >> u >> v >> prob;
        graph.addEdge(u, v, prob); // Add edge with its influence probability
    }

    inputFile.close();
}

// Betweenness Centrality Heuristic
vector<int> betweennessCentrality(Graph& graph, int k) {
    vector<double> centrality(graph.nodes, 0.0);

    // Calculate betweenness centrality for each node
    for (int src = 0; src < graph.nodes; src++) {
        vector<int> shortestPaths(graph.nodes, 0);
        vector<double> dependency(graph.nodes, 0.0);
        vector<vector<int>> predecessors(graph.nodes);
        queue<int> q;
        vector<int> distance(graph.nodes, -1);

        // Initialization
        shortestPaths[src] = 1;
        distance[src] = 0;
        q.push(src);

        // BFS for shortest paths
        while (!q.empty()) {
            int curr = q.front();
            q.pop();
            for (const auto& neighbor : graph.adjList[curr]) {
                int nextNode = neighbor.first;

                // Discover a new node
                if (distance[nextNode] == -1) {
                    distance[nextNode] = distance[curr] + 1;
                    q.push(nextNode);
                }

                // Check for shortest path via current node
                if (distance[nextNode] == distance[curr] + 1) {
                    shortestPaths[nextNode] += shortestPaths[curr];
                    predecessors[nextNode].push_back(curr);
                }
            }
        }

        // Calculate dependencies in reverse order
        vector<int> stack;
        for (int i = 0; i < graph.nodes; i++) {
            if (distance[i] != -1) {
                stack.push_back(i);
            }
        }
        reverse(stack.begin(), stack.end());

        for (int node : stack) {
            for (int pred : predecessors[node]) {
                dependency[pred] += (double(shortestPaths[pred]) / shortestPaths[node]) * (1 + dependency[node]);
            }
            if (node != src) {
                centrality[node] += dependency[node];
            }
        }
    }

    // Pair centrality values with nodes and sort
    vector<pair<double, int>> centralityNodes;
    for (int i = 0; i < graph.nodes; i++) {
        centralityNodes.push_back({centrality[i], i});
    }
    sort(centralityNodes.rbegin(), centralityNodes.rend());

    // Select the top-k nodes
    vector<int> seedNodes;
    for (int i = 0; i < k; i++) {
        seedNodes.push_back(centralityNodes[i].second);
    }
    return seedNodes;
}

// Independent Cascade Model
set<int> independentCascade(Graph& graph, const vector<int>& seedNodes) {
    set<int> activated(seedNodes.begin(), seedNodes.end()); // Initially activated nodes
    set<int> newActivated(seedNodes.begin(), seedNodes.end());

    srand(time(0)); // Seed for random number generation

    while (!newActivated.empty()) {
        set<int> nextActivated;
        for (int node : newActivated) {
            for (const auto& neighbor : graph.adjList[node]) {
                int neighborNode = neighbor.first;
                double prob = neighbor.second;
                if (activated.find(neighborNode) == activated.end() && (rand() / (double)RAND_MAX) <= prob) {
                    nextActivated.insert(neighborNode);
                }
            }
        }
        activated.insert(nextActivated.begin(), nextActivated.end());
        newActivated = nextActivated;
    }

    return activated;
}

// Main function
int main() {
    // Step 1: Get inputs from the user
    string filename;
    int k;

    cout << "Enter the graph file name: ";
    cin >> filename;

    cout << "Enter the number of seed nodes: ";
    cin >> k;

    // Step 2: Load the graph from the file
    Graph graph(0);
    loadGraphFromFile(filename, graph);

    // Step 3: Select seed nodes using Betweenness Centrality
    vector<int> seedNodes = betweennessCentrality(graph, k);

    // Step 4: Calculate influenced nodes using the Independent Cascade Model
    set<int> influencedNodes = independentCascade(graph, seedNodes);

    // Step 5: Display the results
    cout << "Selected Seed Nodes (Betweenness Centrality): ";
    for (int node : seedNodes) {
        cout << node << " ";
    }
    cout << endl;

    cout << "Total Nodes Influenced: " << influencedNodes.size() << endl;

    cout << "Influenced Nodes: ";
    for (int node : influencedNodes) {
        cout << node << " ";
    }
    cout << endl;

    return 0;
}

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

// Function to calculate marginal gain
int calculateMarginalGain(Graph& graph, set<int> currentSet, int node) {
    currentSet.insert(node);
    set<int> influencedNodes = independentCascade(graph, vector<int>(currentSet.begin(), currentSet.end()));
    return influencedNodes.size();
}

// Cost-Effective Lazy Forward Selection
vector<int> celf(Graph& graph, int k) {
    vector<int> seedNodes;
    set<int> currentSet;
    vector<pair<int, int>> gains; // {marginal gain, node}

    // Step 1: Initialize the gain for all nodes
    for (int i = 0; i < graph.nodes; i++) {
        int gain = calculateMarginalGain(graph, currentSet, i);
        gains.push_back({gain, i});
    }

    // Step 2: Sort nodes by marginal gain
    sort(gains.rbegin(), gains.rend());

    // Step 3: Select k seed nodes
    for (int i = 0; i < k; i++) {
        int node = gains[0].second;
        seedNodes.push_back(node);
        currentSet.insert(node);

        // Recalculate marginal gains for remaining nodes
        gains.erase(gains.begin());
        for (auto& gain : gains) {
            gain.first = calculateMarginalGain(graph, currentSet, gain.second);
        }

        // Re-sort the gains
        sort(gains.rbegin(), gains.rend());
    }

    return seedNodes;
}

// Main function to test Cost-Effective Lazy Forward Selection
int main() {
    // Step 1: Get runtime input for the filename and the number of seed nodes
    string filename;
    cout << "Enter the graph file name: ";
    cin >> filename;

    int k;
    cout << "Enter the number of seed nodes to select: ";
    cin >> k;

    // Step 2: Load the graph from the file
    Graph graph(0);
    loadGraphFromFile(filename, graph);

    // Step 3: Select seed nodes using CELF
    vector<int> seedNodes = celf(graph, k);

    // Step 4: Calculate influenced nodes using the Independent Cascade Model
    set<int> influencedNodes = independentCascade(graph, seedNodes);

    // Step 5: Display the results
    cout << "Selected Seed Nodes (CELF): ";
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

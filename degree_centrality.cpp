#include <iostream>
#include <vector>
#include <set>
#include <fstream>
#include <algorithm> // For sort
#include <cstdlib>   // For rand()
#include <ctime>     // For time()

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

// Degree Centrality Heuristic
vector<int> degreeCentrality(Graph& graph, int k) {
    vector<pair<int, int>> degrees; // {degree, node}
    for (int i = 0; i < graph.nodes; i++) {
        degrees.push_back({graph.adjList[i].size(), i});
    }

    // Sort by degree in descending order
    sort(degrees.rbegin(), degrees.rend());

    // Select the top-k nodes
    vector<int> seedNodes;
    for (int i = 0; i < k; i++) {
        seedNodes.push_back(degrees[i].second);
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

// Main function to test Degree Centrality with Influence Calculation
int main() {
    // Step 1: Input the graph filename and number of seed nodes
    string filename;
    int k;

    cout << "Enter the graph filename: ";
    cin >> filename;

    cout << "Enter the number of seed nodes: ";
    cin >> k;

    // Step 2: Load the graph from the file
    Graph graph(0);
    loadGraphFromFile(filename, graph);

    // Step 3: Select seed nodes using Degree Centrality
    vector<int> seedNodes = degreeCentrality(graph, k);

    // Step 4: Calculate influenced nodes using the Independent Cascade Model
    set<int> influencedNodes = independentCascade(graph, seedNodes);

    // Step 5: Display the results
    cout << "Selected Seed Nodes (Degree Centrality): ";
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

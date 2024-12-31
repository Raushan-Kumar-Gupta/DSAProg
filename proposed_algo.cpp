#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

// Edge structure for storing graph
struct Edge {
    int target;
    double influenceProbability;
};

// Graph representation using adjacency list
map<int, vector<Edge> > graph;

// Function to read the graph from the test file
void buildGraph(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    int u, v;
    double p;
    while (file >> u >> v >> p) {
        Edge edge;
        edge.target = v;
        edge.influenceProbability = p;
        graph[u].push_back(edge);
    }

    file.close();
}

// Simulate Independent Cascade (IC) model for influence spread
set<int> simulateInfluence(int node, const set<int>& alreadyInfluenced, int numSimulations = 100) {
    set<int> newlyInfluenced;

    for (int sim = 0; sim < numSimulations; ++sim) {
        queue<int> q;
        set<int> visited;
        q.push(node);

        while (!q.empty()) {
            int current = q.front();
            q.pop();

            for (size_t i = 0; i < graph[current].size(); ++i) {
                int neighbor = graph[current][i].target;
                double prob = graph[current][i].influenceProbability;

                if (alreadyInfluenced.find(neighbor) == alreadyInfluenced.end() &&
                    newlyInfluenced.find(neighbor) == newlyInfluenced.end()) {
                    // Attempt to activate neighbor
                    double randValue = static_cast<double>(rand()) / RAND_MAX;
                    if (randValue < prob) {
                        newlyInfluenced.insert(neighbor);
                        q.push(neighbor);
                    }
                }
            }
        }
    }

    return newlyInfluenced;
}

// Custom Influence Maximization Algorithm
pair<vector<int>, set<int> > marginalGainMaximization(int k) {
    set<int> alreadyInfluenced; // Tracks all activated nodes
    vector<int> seedSet;

    for (int i = 0; i < k; ++i) {
        int bestNode = -1;
        int maxGain = -1;
        set<int> bestNewlyInfluenced;

        for (map<int, vector<Edge> >::iterator it = graph.begin(); it != graph.end(); ++it) {
            int node = it->first;

            // Skip already selected seed nodes
            if (find(seedSet.begin(), seedSet.end(), node) != seedSet.end()) {
                continue;
            }

            // Calculate marginal gain
            set<int> influencedByNode = simulateInfluence(node, alreadyInfluenced);
            int marginalGain = influencedByNode.size();

            if (marginalGain > maxGain) {
                maxGain = marginalGain;
                bestNode = node;
                bestNewlyInfluenced = influencedByNode;
            }
        }

        if (bestNode != -1) {
            seedSet.push_back(bestNode);

            // Update already influenced nodes
            alreadyInfluenced.insert(bestNewlyInfluenced.begin(), bestNewlyInfluenced.end());
        }
    }

    return make_pair(seedSet, alreadyInfluenced);
}

// Main function
int main() {
    srand(time(0)); // Seed for random number generation

    string filename;
    cout << "Enter the test file name: ";
    cin >> filename;

    // Build the graph
    buildGraph(filename);

    int k;
    cout << "Enter the number of seed nodes to select: ";
    cin >> k;

    // Run the influence maximization algorithm
    pair<vector<int>, set<int> > result = marginalGainMaximization(k);
    vector<int> seedNodes = result.first;
    set<int> influencedNodes = result.second;

    // Output the results
    cout << "Selected seed nodes:" << endl;
    for (size_t i = 0; i < seedNodes.size(); ++i) {
        cout << seedNodes[i] << " ";
    }
    cout << endl;

    cout << "Total number of influenced nodes: " << influencedNodes.size() << endl;

    cout << "List of influenced nodes:" << endl;
    for (set<int>::iterator it = influencedNodes.begin(); it != influencedNodes.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;

    return 0;
}

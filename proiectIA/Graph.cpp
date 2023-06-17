#include "Graph.h"
#include <math.h>
#include <random>

int Graph::getInDeg(int node) {
	if (node >= this->size || node < 0)
		return 0;
	int count = 0;
	for (int i = 0; i < this->size; i++) {
		if (this->Adj[i][node])
			count++;
	}
	return count;
}

int Graph::getOutDeg(int node) {
	if (node >= this->size || node < 0)
		return 0;
	int count = 0;
	for (int i = 0; i < this->size; i++) {
		if (this->Adj[node][i])
			count++;
	}
	return count;
}

int Graph::getEdges() {
	int count = 0;
	for (int i = 0; i < this->size; i++)
		for (int j = 0; j < this->size; j++)
			if (this->Adj[i][j])
				count++;
	return count;
}

Graph Graph::generateGraph(int noNodes, float density) {
	srand((unsigned)time(0));
	std::vector<std::vector<bool>> adj(noNodes, std::vector<bool>(noNodes, false));
	float n = 0.0f;
	int count = 0;
	const float EPSILON = 0.0000001f;
	while (fabs(density - n) > EPSILON) {
		int node1 = rand() % noNodes;
		int node2 = rand() % noNodes;
		if (adj[node1][node2])
			continue;
		adj[node1][node2] = 1;
		n = static_cast<float>(++count) / (noNodes * (noNodes - 1));
	}
	Graph g = Graph(adj);
	return g;
}

Graph Graph::generateGraphNoDens(int noNodes) {
	srand((unsigned)time(0));
	std::vector<std::vector<bool>> adj(noNodes, std::vector<bool>(noNodes, false));
	for (int i = 0; i < noNodes; i++) {
		for (int j = 0; j < noNodes; j++)
			adj[i][j] = rand() % 2;
	}
	Graph g = Graph(adj);
	return g;
}
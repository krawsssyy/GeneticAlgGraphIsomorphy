#pragma once
#include <vector>
class Graph {
public:
	int size;
	std::vector<std::vector<bool>> Adj;
	Graph() {
		this->size = 0;
	}
	Graph(std::vector<std::vector<bool>> adj) {
		this->size = adj.size();
		this->Adj = std::vector<std::vector<bool>>(this->size, std::vector<bool>(this->size, false));
		for (int i = 0; i < this->size; i++) {
			for (int j = 0; j < this->size; j++) {
				this->Adj[i][j] = adj[i][j];
			}
		}
	}
	~Graph() {}
	int getInDeg(int node);
	int getOutDeg(int node);
	static Graph generateGraph(int noNodes, float density);
	int getEdges();
	static Graph generateGraphNoDens(int noNodes);
};
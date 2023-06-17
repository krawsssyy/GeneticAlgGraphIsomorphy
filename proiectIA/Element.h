#pragma once
#include "Graph.h"
class Element {
public:
	Graph bigGraph;
	Graph smallGraph;
	int sizeOfSmall;
	float densityOfBig;
	Element() : sizeOfSmall(0), densityOfBig(0.0f) {}
	Element(Graph bg, Graph sg, int sos, float dob) {
		this->bigGraph = Graph(bg.Adj);
		this->smallGraph = Graph(sg.Adj);
		this->sizeOfSmall = sos;
		this->densityOfBig = dob;
	}
	Element(const Element& e) {
		this->bigGraph = Graph(e.bigGraph.Adj);
		this->smallGraph = Graph(e.smallGraph.Adj);
		this->sizeOfSmall = e.sizeOfSmall;
		this->densityOfBig = e.densityOfBig;
	}
	~Element() {}
};
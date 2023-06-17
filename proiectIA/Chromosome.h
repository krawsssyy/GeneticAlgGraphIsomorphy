#pragma once
#include "Element.h"
class Chromosome {
public:
	Element base;
	std::pair<int, float> fitness;
	std::vector<int> permutation;
	Chromosome() {}
	Chromosome(std::vector<int> perm, Element e, std::pair<int, float> fit) : base(e), fitness(fit) {
		for (int i = 0; i < perm.size(); i++)
			this->permutation.emplace_back(perm[i]);
	}
	Chromosome(const Chromosome& c) {
		for (int i = 0; i < c.permutation.size(); i++)
			this->permutation.emplace_back(c.permutation[i]);
		this->base = Element(c.base);
		this->fitness = std::pair<int, float>(c.fitness);
	}
	~Chromosome() {}
};
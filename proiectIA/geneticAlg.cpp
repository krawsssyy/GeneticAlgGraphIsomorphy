#include "Chromosome.h"
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <ppl.h>
#include <concurrent_vector.h>
using namespace std;

void loadPopulation(vector<Element>& pop) {
	ifstream big("bgo.txt");
	ifstream small("sgo.txt");
	string line;
	vector<Graph> bigs;
	vector<Graph> smalls;
	vector<int> sizes;
	vector<float> denss;
	for (int i = 0; i < 200; i++) {
		getline(big, line);
		vector<vector<bool>> adj(100);
		for (int j = 0; j < 100; j++) {
			getline(big, line);
			int pos = 0;
			string token;
			while ((pos = line.find(" ")) != string::npos) {
				token = line.substr(0, pos);
				if (token == "" || token == " " || token == "\n")
					break;
				if (token == "0")
					adj[j].emplace_back(false);
				else
					adj[j].emplace_back(true);
				line.erase(0, pos + 1);
			}
		}
		getline(big, line);
		bigs.emplace_back(Graph(adj));
		if (i % 40 < 10)
			denss.emplace_back(0.01f);
		else if (9 < i % 40 && i % 40 < 20)
			denss.emplace_back(0.05f);
		else if (19 < i % 40 && i % 40 < 30)
			denss.emplace_back(0.1f);
		else
			denss.emplace_back(0.2f);
		getline(small, line);
		int currSize = stoi(line);
		sizes.emplace_back(currSize);
		vector<vector<bool>> Adj(currSize);
		for (int j = 0; j < currSize; j++) {
			getline(small, line);
			int pos = 0;
			string token;
			while ((pos = line.find(" ")) != string::npos) {
				token = line.substr(0, pos);
				if (token == "" || token == " " || token == "\n")
					break;
				if (token == "0")
					Adj[j].emplace_back(false);
				else
					Adj[j].emplace_back(true);
				line.erase(0, pos + 1);
			}
		}
		getline(small, line);
		smalls.emplace_back(Graph(Adj));
	}
	small.close();
	big.close();
	for (int i = 0; i < 200; i++) 
		pop.emplace_back(Element(bigs[i], smalls[i], sizes[i], denss[i]));
}

int fitnessEdges(vector<int> perm, Element e) {
	int fit = 0;
	vector<pair<int, int>> maps;
	for (int k = 0; k < e.sizeOfSmall; k++) {
		pair<int, int> p = pair<int, int>(k, perm[k]);
		maps.emplace_back(p);
	}
	for (int i = 0; i < e.sizeOfSmall; i++) {
		for (int j = 0; j < e.sizeOfSmall; j++) {
			if (e.smallGraph.Adj[i][j]) {
				if (!e.bigGraph.Adj[maps[i].second][maps[j].second])
					fit++;
			}
		}
	}
	for (int k = 0; k < maps.size(); k++) {
		for (int i = 0; i < 100; i++) {
			if (e.bigGraph.Adj[maps[k].second][i]) {
				int res = -1;
				for (int j = 0; j < maps.size(); j++) {
					if (i == maps[j].second) {
						res = maps[j].first;
						break;
					}
				}
				if (res != -1) {
					if (!e.smallGraph.Adj[maps[k].first][res])
						fit++;
				}
				else
					fit++;
			}
		}
	}
	return fit;
}

int fitnessDegrees(vector<int> perm, Element e) {
	int fit = 0;
	vector < pair< pair<int, int>, pair<int, int> >> degrees;
	for (int k = 0; k < e.sizeOfSmall; k++) {
		pair<int, int> p = pair<int, int>(e.smallGraph.getInDeg(k), e.bigGraph.getInDeg(perm[k]));
		pair<int, int> p2 = pair<int, int>(e.smallGraph.getOutDeg(k), e.bigGraph.getOutDeg(perm[k]));
		pair< pair<int, int>, pair<int, int> > fin = pair< pair<int, int>, pair<int, int> >(p, p2);
		degrees.emplace_back(fin);
	}
	for (int i = 0; i < e.sizeOfSmall; i++) {
		if (!(degrees[i].second.second >= degrees[i].second.first && degrees[i].first.second >= degrees[i].first.first))
			fit++;
	}
	return fit;
}

pair<int, float> weightedFitness(vector<int> perm, Element e, float w1, float w2) {
	int fit1 = fitnessEdges(perm, e);
	return pair<int, float>(fit1, w1 * fit1 + w2 * fitnessDegrees(perm, e));
}

vector<int> generateRandomPermutation() {
	vector<int> perm;
	vector<int> start;
	srand((unsigned)time(0));
	for (int i = 0; i < 100; i++)
		start.emplace_back(i);
	while (start.size()) {
		int index = rand() % start.size();
		perm.emplace_back(start[index]);
		swap(start[index], start[start.size() - 1]);
		start.pop_back();
	}
	return perm;
}

void loadInitPop(vector<Chromosome>& pop, vector<Element> elems, int i, int j, float w1, float w2) {
	int padding = 40 * i + 10 * j;
	while (pop.size() < 100) {
		for (int i = 0; i < 10; i++) {
			vector<int> perm = generateRandomPermutation();
			pop.emplace_back(Chromosome(perm, elems[padding + i], weightedFitness(perm, elems[padding + i], w1, w2)));
		}
	}
}

Chromosome rouletteSelection(vector<Chromosome> pop) {
	float total = 0;
	for (int i = 0; i < 100; i++)
		total += (1 / pop[i].fitness.second);
	vector<float> partials;
	for (int i = 0; i < 100; i++) {
		partials.emplace_back(((1 / pop[i].fitness.second) / total));
	}
	vector<float> sums;
	float sum = 0.0f;
	sums.emplace_back(sum);
	for (int i = 0; i < 100; i++) {
		sum += partials[i];
		sums.emplace_back(sum);
	}
	srand((unsigned)time(0));
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	int i;
	for (i = 1; i < 101; i++) {
		if (r < sums[i])
			break;
	}
	return Chromosome(pop[i - 1]);
}

Chromosome cycleCrossover(Chromosome parOne, Chromosome parTwo, float w1, float w2) {
	vector<int> offOne(100, -1), offTwo(100, -1);
	offOne[0] = parOne.permutation[0];
	int elem = parTwo.permutation[0];
	while (elem != offOne[0]) {
		vector<int>::iterator it = find(parOne.permutation.begin(), parOne.permutation.end(), elem);
		int index = it - parOne.permutation.begin();
		offOne[index] = elem;
		elem = parTwo.permutation[index];
	}
	vector<int>::iterator it = find(offOne.begin(), offOne.end(), -1);
	while (it != offOne.end()) {
		int index = it - offOne.begin();
		offOne[index] = parTwo.permutation[index];
		it = find(offOne.begin(), offOne.end(), -1);
	}
	offTwo[0] = parTwo.permutation[0];
	elem = parOne.permutation[0];
	while (elem != offTwo[0]) {
		vector<int>::iterator itt = find(parTwo.permutation.begin(), parTwo.permutation.end(), elem);
		int index = it - parTwo.permutation.begin();
		offTwo[index] = elem;
		elem = parOne.permutation[index];
	}
	it = find(offTwo.begin(), offTwo.end(), -1);
	while (it != offTwo.end()) {
		int index = it - offTwo.begin();
		offTwo[index] = parOne.permutation[index];
		it = find(offTwo.begin(), offTwo.end(), -1);
	}
	pair<int, float> fit1 = weightedFitness(offOne, parOne.base, w1, w2);
	pair<int, float> fit2 = weightedFitness(offTwo, parTwo.base, w1, w2);
	Chromosome c;
	if (fit1.second > fit2.second)
		c = Chromosome(offOne, parOne.base, fit1);
	else
		c = Chromosome(offTwo, parTwo.base, fit2);
	return c;
}

void twoSwapMutation(Chromosome& offspring) {
	srand((unsigned)time(0));
	int ind1 = rand() % 100;
	int ind2 = rand() % 100;
	while (ind1 == ind2)
		ind2 = rand() % 100;
	swap(offspring.permutation[ind1], offspring.permutation[ind2]);
}

void twoOptMutation(Chromosome& offspring) {
	srand((unsigned)time(0));
	int ind1 = rand() % 100;
	int ind2 = rand() % 100;
	while (ind1 == ind2)
		ind2 = rand() % 100;
	if (ind1 > ind2) {
		int aux = ind2;
		ind2 = ind1;
		ind1 = aux;
	}
	int off;
	if (!((ind2 - ind1) % 2))
		off = (ind2 - ind1) / 2;
	else
		off = (ind2 - ind1) / 2 + 1;
	int count = 0;
	for (int i = ind1; i < (ind1 + off); i++) 
		swap(offspring.permutation[i], offspring.permutation[ind2 - count++]);
}

Chromosome getBest(vector<Chromosome> pop) {
	int index = 0;
	float bestFit = pop[0].fitness.second;
	for(int i = 0; i < 100; i++) 
		if (pop[i].fitness.second < bestFit) {
			bestFit = pop[i].fitness.second;
			index = i;
		}
	return pop[index];
}

void localOptimization(Chromosome& mutation, float w1, float w2) {
	for (int i = 0; i < 99; i++) {
		for (int j = i + 1; j < 100; j++) {
			swap(mutation.permutation[i], mutation.permutation[j]);
			pair<int, float> fit = weightedFitness(mutation.permutation, mutation.base, w1, w2);
			if (fit.second < mutation.fitness.second)
				mutation.fitness = fit;
			else
				swap(mutation.permutation[i], mutation.permutation[j]);
		}
	}
}


Chromosome geneticAlgorithm( vector<Element> elems, int i, int j, float w1, float w2) {
	vector<Chromosome> pop;
	loadInitPop(pop, elems, i, j, w1, w2);
	int count = 0;
	while (count < 300) {
		vector<Chromosome> aux;
		for (int i = 0; i < 30; i++) {
			Chromosome p1 = rouletteSelection(pop), p2 = rouletteSelection(pop);
			Chromosome offspring = cycleCrossover(p1, p2, w1, w2);
			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			if (r < 0.2f)
				twoOptMutation(offspring);// twoSwapMutation(offspring);
			localOptimization(offspring, w1, w2);
			aux.emplace_back(offspring);
		}
		sort(pop.begin(), pop.end(), [](const Chromosome& lhs, const Chromosome& rhs) {
			return lhs.fitness.second > rhs.fitness.second;
			});
		for (int i = 70; i < 100; i++) {
			pop[i] = aux[i - 70];
		}
		count++;
		vector<Chromosome>::iterator it = find_if(pop.begin(), pop.end(), [] (const Chromosome& c) { return c.fitness.first == 0; });
		if (it != pop.end())
			return pop[it - pop.begin()];
	}
	return getBest(pop);
}

int main() {
	vector<Element> pop;
	loadPopulation(pop);
	string res = "";
	Concurrency::concurrent_vector<Chromosome> results;
	Concurrency::parallel_for(size_t(0), size_t(5), [&](size_t i) {
		Concurrency::parallel_for(size_t(0), size_t(4), [&](size_t j) {
			results.push_back(geneticAlgorithm(pop, i, j, 0.5f, 0.5f));
			});
		});
	for (int i = 0; i < results.size(); i++) {
		res += ("Solutie " + to_string(i) + ":");
		for (int j = 0; j < results[i].permutation.size(); j++)
			res += (to_string(results[i].permutation[j]) + " ");
		res += '\n';
		res += ("Fitness: " + to_string(results[i].fitness.second) + "(" + to_string(results[i].fitness.first) + ");M=" + to_string(results[i].base.sizeOfSmall) + ";n=" + to_string(results[i].base.densityOfBig) + '\n');
	}
	ofstream f("output.txt");
	f << res;
	f.close();
	return 0;
}
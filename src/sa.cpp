/*
 * sa.cpp
 *
 *  Created on: Jul 13, 2008
 *      Author: pedro
 */

#include <assert.h>
#include <math.h>
#include <iostream>

#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/random.hpp>

#include "headers/steiner.hpp"
#include "headers/steiner_solution.hpp"
#include "headers/sa.hpp"

using namespace std;

SaSteiner::SaSteiner(string instance_path) {
	instance = Steiner(instance_path);

	//pre process
}

void SaSteiner::run() {

	//parameters
	int max_outer_iterations = 5;
	int max_inner_iterations = 10;
	float temperature = 10 * (instance.V);
	float lowest_temp = 0.0001;
	float alpha = 0.995f;

	boost::mt19937 rng;
	boost::uniform_01<boost::mt19937> uniform(rng);

	//initial solution & energy
	solution = SteinerSolution(instance);
	SteinerSolution::generate_chins_solution(solution);
	energy = solution.find_cost();

	cout << "initial energy: " << energy << endl;

	//working variables
	int outer_iterations = 0, k = 0, delta;
	double p;
	SteinerSolution new_solution;
	int new_energy;

	while (outer_iterations < max_outer_iterations || temperature < lowest_temp) {

		for (int i = 0; i < max_inner_iterations; i++) {

			//copy solution
			new_solution = SteinerSolution(solution);

			//node-based neighborhood search
			pair<Vertex, int> search = new_solution.node_based_search();

			new_energy = search.second; //new_solution.find_cost();

			delta = new_energy - energy;

			if (delta < 0) {
				cout << "new better energy: " << new_energy << endl;
				solution = new_solution;
				energy = new_energy;
				record_best(new_solution, new_energy);
			} else {
				p = exp(-delta / temperature);

				if (uniform() < p) {
					solution = new_solution;
					energy = new_energy;
					record_best(new_solution, new_energy);
					cout << "accepting worse new solution: " << new_energy << "| p = " << p << endl;
				}
			}

			k++;
		}

		temperature = temperature * alpha;
		outer_iterations++;
	}

	cout << "simulation finished: " << k << " total iterations." << endl;
}

void SaSteiner::record_best(const SteinerSolution& new_solution, const int& new_energy) {
	if(new_energy < energy) {
		energy = new_energy;
		solution = SteinerSolution(new_solution);
		cout << "!! new best at " << energy << endl;
	}
}


SaSteiner::~SaSteiner() {
}

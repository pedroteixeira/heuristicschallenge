/*
 * sa.cpp
 *
 *  Created on: Jul 13, 2008
 *      Author: pedro
 */

#include <assert.h>
#include <math.h>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>

#include "headers/steiner.hpp"
#include "headers/sa.hpp"

using namespace std;

SaSteiner::SaSteiner(string instance_path) {
	instance = new Steiner(instance_path);

	//pre process
}

void SaSteiner::run() {

	//parameters
	int secondsToLive = 60;
	int max_outer_iterations = 100;
	int max_inner_iterations = 600 * instance->V;
	float temperature = secondsToLive * instance->V;
	float alpha = 0.995f;

	//initial solution & energy
	vector<Edge> solution;
	Graph solution_subgraph;
	int energy;

	instance->generate_chins_solution(solution_subgraph, solution);
	energy = instance->find_cost(solution_subgraph, solution);

	cout << "energy: " << energy << endl;

	int outer_iterations = 0, k = 0, delta;

	while (outer_iterations < max_outer_iterations) {

		for (int i = 0; i < max_inner_iterations; i++) {

			//neighbourhood exchange to find new solution

			//cost of new solution

			//delta = new_energy - energy;

			if (delta < 0) {
				//record_best(new_solution, new_energy);
			} else {
				//p = exp(-delta / temperature);
				//if (rng.uniform() < p) {
				//	memcpy(solution, new_solution, n * sizeof(int));
				//	energy = new_energy;
				//	record_best(new_solution, new_energy);
				//}
			}

			k++;
		}

		outer_iterations++;
	}

}

SaSteiner::~SaSteiner() {
	delete instance;
}

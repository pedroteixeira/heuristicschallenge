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
#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <boost/progress.hpp>

#include "headers/steiner.hpp"
#include "headers/steiner_solution.hpp"
#include "headers/steiner_nodesearch.hpp"
#include "headers/sa.hpp"

using namespace std;

SaSteiner::SaSteiner(string instance_path) {
	instance = new Steiner(instance_path);

	//TODO: pre process
}

void SaSteiner::run() {

	//parameters
	int max_outer_iterations = 10;
	int max_inner_iterations = 1;
	float temperature = 10 * (instance->V);
	float lowest_temp = 0.0001;
	float alpha = 0.995f;
	boost::timer t0;

	boost::mt19937 rng;
	boost::uniform_01<boost::mt19937> uniform(rng);

	//initialize solution
	SteinerSolution solution(instance);
	SteinerSolution::generate_chins_solution(solution);
	solution.build_candidates_out_vertices();

	//initial energy
	int energy = solution.find_cost();

	solution.graph.writedot("chins.dot");

	cout << "initial energy: " << energy << endl;

	//working variables
	int outer_iterations = 0, k = 0, delta;
	double p;

	while (outer_iterations < max_outer_iterations && temperature > lowest_temp) {

		for (int i = 0; i < max_inner_iterations; i++) {

			//copy solution
			SteinerSolution new_solution(solution);

			//key-path based neighborhood search
			pair<int, int> result = SteinerNodeLocalSearch::search(new_solution);

			//solution.exchange_key_path();

			int new_energy = new_solution.find_cost();

			delta = new_energy - energy;

			if (delta < 0) {
				cout << "new better energy: " << new_energy << endl;

				solution = new_solution;
				energy = new_energy;
				record_best(new_solution, new_energy);

			} else {
				p = exp(-delta / temperature);

				if (uniform() < p) {
					cout << "accepting worse/same new solution: " << new_energy << "| p = " << p << endl;

					solution = new_solution;
					energy = new_energy;
					record_best(new_solution, new_energy);
				}
				else {
					//keep solution
				}
			}

			k++;
		}

		temperature = temperature * alpha;
		outer_iterations++;
	}

	cout << "simulation finished: " << k << " total iterations in " << t0.elapsed() << " seconds." << endl;
}

void SaSteiner::record_best(SteinerSolution& new_solution, int new_energy) {
	if(new_energy < best_energy) {
		best_energy = new_energy;

		SteinerSolution tmp = new_solution; //deep copy
		best_solution = &tmp;
		cout << "!! new best at " << best_energy << endl;
	}
}


SaSteiner::~SaSteiner() {
	delete instance;
}

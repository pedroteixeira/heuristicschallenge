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
#include "headers/steiner_heuristics.hpp"
#include "headers/steiner_nodesearch.hpp"
#include "headers/steiner_pathsearch.hpp"
#include "headers/sa.hpp"


using namespace std;

SaSteiner::SaSteiner(string instance_path) {
	instance = new Steiner(instance_path);

	//TODO: pre process
}

void SaSteiner::run() {

	//parameters
	int max_outer_iterations = 20;
	int max_inner_iterations = 100;
	float temperature =  instance->V;
	float lowest_temp = 0.0001;
	float alpha = 0.9f;
	boost::timer t0;

	boost::mt19937 rng;
	boost::uniform_01<boost::mt19937> uniform(rng);

	//initialize solution
	SteinerSolution solution = SteinerHeuristics::generate_chins_solution(*instance);

	//initial energy
	int energy = solution.find_cost();
	best_energy = energy;

	cout << "initial energy: " << energy << endl;

	//working variables
	int outer_iterations = 0, k = 0, delta;
	double p, random;

	while (outer_iterations < max_outer_iterations && temperature > lowest_temp) {

		for (int i = 0; i < max_inner_iterations; i++) {

			//copy solution
			SteinerSolution new_solution(solution);

			//key-path based neighborhood search

			//SteinerNodeLocalSearch::remove(new_solution);
			SteinerNodeLocalSearch::insert(new_solution);
			SteinerPathLocalSearch::search(new_solution);

			int new_energy = new_solution.find_cost();

			delta = new_energy - energy;

			if (delta < 0) {
				cout << "new better energy: " << new_energy << "\n";

				solution = new_solution;
				energy = new_energy;
				record_best(new_solution, new_energy);

			} else {

				p = exp(-delta / temperature);
				random = uniform();

				if (random < p) {
					cout << "accepting worse or same new solution: " << new_energy << "| p = " << p << "\n";

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
		cout << k << "iterations.  Temperature decreased to " << temperature << "\n";
		outer_iterations++;
	}

	cout << "simulation finished: " << k << " total iterations in " << t0.elapsed() << " seconds. \n";
	cout << "best energy found was: " << best_energy << "\n";
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

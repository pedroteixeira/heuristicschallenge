/*
 * steiner_grasp.cpp
 *
 *  Created on: Jul 28, 2008
 *      Author: pedro
 */

#include <assert.h>
#include <math.h>
#include <iostream>

#include <boost/graph/random.hpp>
#include <boost/random.hpp>
#include <boost/progress.hpp>

#include "headers/steiner.hpp"
#include "headers/steiner_solution.hpp"
#include "headers/steiner_heuristics.hpp"
#include "headers/steiner_nodesearch.hpp"
#include "headers/steiner_pathsearch.hpp"
#include "headers/steiner_grasp.hpp"

using namespace std;

SteinerGRASP::SteinerGRASP(Steiner& steiner) :
	instance(steiner) {
}

void SteinerGRASP::run() {

	size_t max_inner_iterations = 10;
	int k = 0;

	best_cost = INT_MAX;
	size_t best_root;
	boost::timer t0;

	//pick random initial terminal
	boost::uniform_int<> range(0, instance.V - 1);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(instance.rng, range);

	for (size_t i = 0; i < (size_t)instance.V; i++) {
		//int root = die();
		int root = i;
		for (size_t j = 0; j < max_inner_iterations; j++) {
			SteinerSolution solution = SteinerHeuristics::generate_chins_solution(root, instance);

			int cost = solution.find_cost();

			//SteinerNodeLocalSearch::insert(solution);
			//int insert_cost = solution.find_cost();
			//cout << "cost when applying node local search: " << insert_cost << "\n";
			//SteinerPathLocalSearch::search(solution);

			if (cost < best_cost) {
				best_cost = cost;
				best_root = root;
				cout << "*** new current best " << cost << " with " << root << "\n";
			}

			k++;
		}
	}

	cout << "GRASP finished in " << k << " total iterations in " << t0.elapsed() << " seconds. \n";
	cout << "best found was " << best_cost << " with root terminal " << best_root << "\n";
}

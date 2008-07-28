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

	int lamda = 1.01;

	best_cost = INT_MAX / lamda;
	size_t best_root;
	for (size_t i = 0; i < instance.terminals.size(); i++) {
		if (i != 14)
			continue;

		SteinerSolution solution = SteinerHeuristics::generate_chins_solution(i, instance);

		int cost = solution.find_cost();
		cout << "cost of chins: " << cost << "\n";

		SteinerNodeLocalSearch::insert(solution);
		int insert_cost = solution.find_cost();

		cout << "cost when applying node local search: " << insert_cost << "\n";

		//SteinerPathLocalSearch::search(solution);

		cost = min(insert_cost, cost);
		if (cost < best_cost) {
			best_cost = cost;
			best_root = i;
		}

		cout << "\n\n";
	}

	cout << "best found was " << best_cost << " with root terminal " << best_root << "\n";
}

/*
 * steiner_solution.hpp
 *
 *  Created on: Jul 14, 2008
 *      Author: pedro
 */

#ifndef STEINER_SOLUTION_HPP_
#define STEINER_SOLUTION_HPP_

#define BOOST_RELAXED_HEAP_DEBUG 3
#define BOOST_GRAPH_DIJKSTRA_TESTING 1

#include <boost/random.hpp>
#include "steiner.hpp"

class SteinerSolution {
public:
	SteinerSolution();
	SteinerSolution(Steiner*);
	SteinerSolution(const SteinerSolution&);
	SteinerSolution& operator = ( const SteinerSolution& source );
	std::list<Edge> tree;
	Steiner* instance;
	Graph graph;

	int find_cost();
	void exchange_key_path();
	static void generate_chins_solution(SteinerSolution&);

private:
	void init();
	void copy(const SteinerSolution&, SteinerSolution&);

	void find_mst_tree();
};

#endif /* STEINER_SOLUTION_HPP_ */

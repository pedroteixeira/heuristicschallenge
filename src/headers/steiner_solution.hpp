/*
 * steiner_solution.hpp
 *
 *  Created on: Jul 14, 2008
 *      Author: pedro
 */

#ifndef STEINER_SOLUTION_HPP_
#define STEINER_SOLUTION_HPP_

#include <boost/random.hpp>
#include "util.hpp"
#include "steiner.hpp"

class SteinerSolution {
public:
	SteinerSolution();
	SteinerSolution(const Steiner&);
	SteinerSolution(const SteinerSolution&);
	Graph graph;
	std::vector<Edge> tree;
	Steiner instance;

	int find_cost();
	void exchange_key_path();
	void insert_steiner_node();
	static void generate_chins_solution(SteinerSolution&);

private:
	boost::mt19937 rng;
	std::vector<Vertex> out_vertices;
	void init();
	void find_mst_tree();
};

#endif /* STEINER_SOLUTION_HPP_ */

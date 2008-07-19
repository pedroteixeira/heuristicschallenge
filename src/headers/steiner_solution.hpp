/*
 * steiner_solution.hpp
 *
 *  Created on: Jul 14, 2008
 *      Author: pedro
 */

#ifndef STEINER_SOLUTION_HPP_
#define STEINER_SOLUTION_HPP_

#include <boost/random.hpp>
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
	void find_mst_tree();
	void exchange_key_path();
	void insert_steiner_node(Vertex);
	void remove_steiner_node(Vertex);
	std::pair<Vertex, int>  node_based_search();
	static void generate_chins_solution(SteinerSolution&);

private:
	std::list<Vertex> out_vertices;
	void build_candidates_out_vertices();
	void update_candidates_out_vertices(Vertex);
	bool check_candidate_for_out_vertex(Vertex);
	void init();
};

#endif /* STEINER_SOLUTION_HPP_ */

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
#include "util.hpp"

class SteinerSolution {
public:
	SteinerSolution(Steiner*);
	SteinerSolution(const SteinerSolution&);
	SteinerSolution& operator = ( const SteinerSolution& source );
	std::list<Edge> tree;
	Steiner* instance;
	Graph graph;
	IntSet out_vertices;

	int find_cost();
	void exchange_key_path();
	void build_candidates_out_vertices();
	void find_mst_tree();

	void add_edge_from_original(int, int);

	static void generate_chins_solution(SteinerSolution&);

	//'events'
	void on_steiner_node_inserted(int);
	void on_steiner_node_removed(int);

private:
	void init();
	void copy(const SteinerSolution&, SteinerSolution&);

	void update_candidates_out_vertices(int);
	bool is_new_candidate_for_out_vertex(int);
};

#endif /* STEINER_SOLUTION_HPP_ */

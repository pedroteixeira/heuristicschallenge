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
	SteinerSolution(Steiner&);
	SteinerSolution(const SteinerSolution&);
	SteinerSolution& operator = ( const SteinerSolution& source );
	std::list< std::pair< int,int > > tree;
	Steiner& instance;
	Graph graph;
	IntSet out_key_nodes;
	IntSet in_key_nodes;
	IntSet virtual_terminals;

	bool is_terminal(int);
	int find_cost();

	bool is_out_key_node(int i);
	bool is_in_key_node(int i);
	void build_candidates_out_key_nodes();
	void build_candidates_in_key_nodes();

	void add_edge_from_original(int, int);
	void add_path(int, int, std::vector<int>&);

	void grow_graph();
	void find_mst_tree();
	void undo_last_mst();


	//'events'
	void on_key_node_inserted(int);
	void on_key_node_removed(int);

private:
	void copy(const SteinerSolution&, SteinerSolution&);

	void compact_graph();
	void update_candidates_out_key_nodes(int);
	bool is_new_candidate_for_out_key_node(int);

	std::list< std::pair< int,int > > undo_tree;
};

#endif /* STEINER_SOLUTION_HPP_ */

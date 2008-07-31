/*
 * key_nodesearch.hpp
 *
 *  Created on: Jul 22, 2008
 *      Author: pedro
 */

#ifndef STEINER_NODESEARCH_HPP_
#define STEINER_NODESEARCH_HPP_

#include "graph.hpp"
#include "steiner_solution.hpp"


struct WayToConnect {
	int shortest_distance;
	int to_vertex;
	int from_vertex;

public:
	WayToConnect() {}
};

class SteinerNodeLocalSearch {
public:
	static void insert(SteinerSolution&);
	static void remove(SteinerSolution&);

	SteinerSolution key_node_insert(const SteinerSolution&);


private:
	static void connect_graph(int, SteinerSolution&); // TODO: move to sution
	static void remove_key_node(int, SteinerSolution&);
	static void insert_key_node(int, SteinerSolution&);
};

#endif /* STEINER_NODESEARCH_HPP_ */

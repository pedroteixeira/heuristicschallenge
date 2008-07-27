/*
 * steiner_nodesearch.hpp
 *
 *  Created on: Jul 22, 2008
 *      Author: pedro
 */

#ifndef STEINER_NODESEARCH_HPP_
#define STEINER_NODESEARCH_HPP_

#include "graph.hpp"
#include "steiner_solution.hpp"


class SteinerNodeLocalSearch {
public:
	static void search(SteinerSolution&);


private:
	static void remove_steiner_node(int, SteinerSolution&);
	static bool insert_steiner_node(int, SteinerSolution&);
};

#endif /* STEINER_NODESEARCH_HPP_ */

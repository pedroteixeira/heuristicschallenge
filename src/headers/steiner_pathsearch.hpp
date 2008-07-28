/*
 * steiner_pathsearch.hpp
 *
 *  Created on: Jul 26, 2008
 *      Author: pedro
 */

#ifndef STEINER_PATHSEARCH_HPP_
#define STEINER_PATHSEARCH_HPP_

#include "graph.hpp"
#include "steiner_solution.hpp"


class SteinerPathLocalSearch {
public:
	static void search(SteinerSolution&);


private:
	static void exchange_path(SteinerSolution&, std::list<Vertex>&, std::vector<int>&);
};

#endif /* STEINER_PATHSEARCH_HPP_ */

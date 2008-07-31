/*
 * steiner_heuristics.hpp
 *
 *  Created on: Jul 27, 2008
 *      Author: pedro
 */

#ifndef STEINER_HEURISTICS_HPP_
#define STEINER_HEURISTICS_HPP_

#include "steiner_solution.hpp"
#include "steiner.hpp"

class SteinerHeuristics {
public:

	static SteinerSolution generate_chins_solution(const std::list<int>&, Steiner&);
	static SteinerSolution generate_chins_solution(Steiner&);

	static SteinerSolution generate_network_distance_solution(Steiner&);

private:
	Steiner& steiner;
};
#endif /* STEINER_HEURISTICS_HPP_ */

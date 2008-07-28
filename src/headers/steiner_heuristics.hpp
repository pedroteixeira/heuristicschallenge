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
	static SteinerSolution generate_chins_solution(int, Steiner&);
	static SteinerSolution generate_chins_solution(Steiner&);
};
#endif /* STEINER_HEURISTICS_HPP_ */

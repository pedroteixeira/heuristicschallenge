/*
 * steiner_grasp.hpp
 *
 *  Created on: Jul 28, 2008
 *      Author: pedro
 */

#ifndef STEINER_GRASP_HPP_
#define STEINER_GRASP_HPP_

#include "steiner.hpp"

class SteinerGRASP {
public:
	SteinerGRASP(Steiner&);
	void run();

private:
	Steiner& instance;

	SteinerSolution* best_solution;
	int best_cost;
};


#endif /* STEINER_GRASP_HPP_ */

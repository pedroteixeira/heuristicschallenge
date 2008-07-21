/*
 * sa.hpp
 *
 *  Created on: Jul 13, 2008
 *      Author: pedro
 */

#ifndef SA_HPP_
#define SA_HPP_

#include "steiner.hpp"
#include "steiner_solution.hpp"

class SaSteiner {
public:
	SaSteiner(std::string);
	~SaSteiner();
	void run();
private:
	Steiner* instance;
	SteinerSolution best_solution;
	int best_energy;
	void record_best(SteinerSolution&, int);
};


#endif /* SA_HPP_ */

/***************************************************************************
    rrls.cpp - Implementation Random Restart Local Search for 
    comparison with mmas.
                             -------------------
    begin                : Sun Mar 10, 2002
    copyright            : (C) 2002 by Krzysztof Socha
    email                : krzysztof.socha@ulb.ac.be
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "defs.h"

#include "Control.h"
#include "Problem.h"
#include "Solution.h"
#include "Random.h"

int main( int argc, char** argv) {

  // create a control object from the command line arguments
  Control control(argc, argv);

  // check if specific parameters have been passed
	
	// -ant
	int n_of_ants = control.getIntParameter("-ant");
	if (n_of_ants<1) {
		n_of_ants = DEFAULT_N_OF_ANTS;
		cerr << "Warning: " << n_of_ants << " used as default number of random solutions generated" << endl;
	}
	Solution** solution = new Solution*[n_of_ants];
	
	// create a problem, control tells you what input file stream to use
  Problem *problem = new Problem(control.getInputStream());

	// create a Random object
  Random *rnd = new Random((unsigned) control.getSeed());

	// create a buffer for holding iteration and global best solution
	Solution *best_solution = new Solution(problem, rnd);
	
	// run a number of tries, control knows how many tries there should be done
  while (control.triesLeft()) {
    
		// tell control we are starting a new try
    control.beginTry();
		
		// initialize best solution with random value
		best_solution->RandomInitialSolution();
		best_solution->computeFeasibility();
    control.setCurrentCost(best_solution);

		while (control.timeLeft()) {		
	
			// create a set of ants
			for (int i=0;i<n_of_ants;i++)
				solution[i] = new Solution(problem, rnd);

			// let the ants do the job - create some solutions
			for (int i=0;i<n_of_ants;i++)
				solution[i]->RandomInitialSolution();
						
			// find the best solution
			int best_fitness = 99999;
			int ant_idx = -1;
			for (int i=0;i<n_of_ants;i++)
			{
				int fitness = solution[i]->computeHcv();
				if (fitness<best_fitness) {
					best_fitness = fitness;
					ant_idx = i;
				}
			}
			// apply local search until local optimum reached or a time limit reached
			solution[ant_idx]->localSearch(DEFAULT_MAX_STEPS, control.getTimeLimit()-control.getTime());
			solution[ant_idx]->computeFeasibility();
			
			// output the new best solution, if found
			if (solution[ant_idx]->feasible) {

				solution[ant_idx]->computeScv();
				if (solution[ant_idx]->scv<=best_solution->scv) {
					best_solution->copy(solution[ant_idx]);
					best_solution->hcv = 0;
					control.setCurrentCost(best_solution);
				}
			}
			else {
				solution[ant_idx]->computeHcv();
				if (solution[ant_idx]->hcv<=best_solution->hcv) {
					best_solution->copy(solution[ant_idx]);
					control.setCurrentCost(best_solution);
					best_solution->scv = 99999;
				}
			}	

			// destroy current solutions
			for(int i=0;i<n_of_ants;i++)
				delete solution[i];		
		}
		// output best feasible solution found
    control.endTry(best_solution);
  }

  delete problem;
	delete solution;
	delete best_solution;
	delete rnd;

	return 0;
}

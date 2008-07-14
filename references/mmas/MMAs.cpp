/***************************************************************************
    MMAs.cpp - Implementation of MAX-MIN Ant System 
	             for solving UTTP (University Timetabling Problem).
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

// library includes
#include "Control.h"
#include "Problem.h"
#include "Solution.h"
#include "Random.h"

// mmas classes includes
#include "Ant.h"
#include "MMAsProblem.h"

int main( int argc, char** argv) {

  // create a control object from the command line arguments
  Control control(argc, argv);

  // check if MMAs specific parameters have been passed
	
	// -ant
	int n_of_ants = control.getIntParameter("-ant");
	if (n_of_ants<1) {
		n_of_ants = DEFAULT_N_OF_ANTS;
		cerr << "Warning: " << n_of_ants << " used as default number of ants" << endl;
	}
	else
	  cerr << n_of_ants << " used as the number of ants" << endl;
	Ant** ant = new Ant*[n_of_ants];
	
	// -alpha is always equal to 1.0
	cerr << "Warning: 1.0 used as default alpha coefficient" << endl;
		
	// -beta is never used, so always beta=0.0
	cerr << "Warning: 0.0 used as default beta coefficient" << endl;

	// -evp
	double pheromone_evaporation = 1.0 - control.getDoubleParameter("-evp");
	if (pheromone_evaporation==1.0) {
		pheromone_evaporation = DEFAULT_PHEROMONE_EVAPORATION;
		cerr << "Warning: " << pheromone_evaporation << " used as default pheromone evaporation coefficient" << endl;
	}
	else
	  cerr << pheromone_evaporation << " used as the pheromone evaporation coefficient" << endl;
	
	// -min
	double minimal_pheromone = control.getDoubleParameter("-min");
	if (minimal_pheromone==0.0) {
		minimal_pheromone = DEFAULT_MINIMAL_PHEROMONE;
		cerr << "Warning: " << minimal_pheromone << " used as default minimal pheromone level" << endl;
	}
	else
	  cerr << minimal_pheromone << " used as the minimal pheromone level" << endl;

	// create a problem, control tells you what input file stream to use
  MMAsProblem *problem = new MMAsProblem(control.getInputStream(), pheromone_evaporation, minimal_pheromone);

	// create a Random object
  Random *rnd = new Random((unsigned) control.getSeed());

	// create a buffer for holding global best solution
	Solution *best_solution = new Solution(problem, rnd);
	
	// run a number of tries, control knows how many tries there should be done
  while (control.triesLeft()) {
    
		// tell control we are starting a new try
    control.beginTry();
		
		// reset the pheromone level to the initial value;
		problem->pheromoneReset();

		// initialize best solution with random value
		best_solution->RandomInitialSolution();
		best_solution->computeFeasibility();
		control.setCurrentCost(best_solution);

    // do if we still have time for current try
    while (control.timeLeft()) {		
	
			// create a set of ants
			for (int i=0;i<n_of_ants;i++)
				ant[i] = new Ant(problem, rnd);

			// let the ants do the job - create some solutions
			for (int i=0;i<n_of_ants;i++)
				ant[i]->Move();
			
			// evaporate the pheromone			
			problem->evaporatePheromone();
	
			// find the the best solution
			int best_fitness = 99999;
			int ant_idx = -1;
			for (int i=0;i<n_of_ants;i++)
			{
				int fitness = ant[i]->computeFitness();
				if (fitness<best_fitness) {
					best_fitness = fitness;
					ant_idx = i;
				}
			}
			
			// apply local search until local optimum is reached or a time limit reached
			ant[ant_idx]->solution->localSearch(DEFAULT_MAX_STEPS, control.getTimeLimit()-control.getTime());
			
			// and see if the solution is feasible
			ant[ant_idx]->solution->computeFeasibility();
			
			// output the new best solution, if found
			if (ant[ant_idx]->solution->feasible) {

				ant[ant_idx]->solution->computeScv();
				if (ant[ant_idx]->solution->scv<=best_solution->scv) {
					best_solution->copy(ant[ant_idx]->solution);
					best_solution->hcv = 0;
					control.setCurrentCost(best_solution);
				}				
			}
			else {
				ant[ant_idx]->solution->computeHcv();
				if (ant[ant_idx]->solution->hcv<=best_solution->hcv) {
					best_solution->copy(ant[ant_idx]->solution);
					control.setCurrentCost(best_solution);
					best_solution->scv = 99999;
				}
			}	

			// perform pheromone update with the global best solution
			Solution *tmp_solution = ant[ant_idx]->solution;
			ant[ant_idx]->solution = best_solution;

			// and deposit pheromone for this best one
			ant[ant_idx]->computeFitness();
			problem->pheromoneMinMax();
			ant[ant_idx]->depositPheromone();

			ant[ant_idx]->solution = tmp_solution;

			// let the ants die
			for(int i=0;i<n_of_ants;i++)
				delete ant[i];		

		}
		// end try - output the best solution found
    control.endTry(best_solution);
  }

  delete problem;
	delete ant;
	delete best_solution;
	delete rnd;

	return 0;
}

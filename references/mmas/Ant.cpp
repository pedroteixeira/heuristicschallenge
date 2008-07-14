/***************************************************************************
    Ant.cpp - Implementation of the ant. 
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

#include "Ant.h"

Ant::Ant(MMAsProblem* problem, Random *rnd) {
	
	// memeber variables initialization
	this->problem = problem;
	solution = new Solution(problem,rnd);
	fitness = -1;
}

Ant::~Ant() {
	delete solution;
}

void Ant::Move() {

	// itarate through all the events to complete the path
	for (int i=0;i<problem->n_of_events;i++) {

		// chose next event from the list
		int e = problem->sorted_event_list[i];

		// finding the range for normalization
		double range = 0.0;
		for (int j=0;j<N_OF_TIMESLOTS;j++)
			range += problem->event_timeslot_pheromone[e][j];

		// choose a random number between 0.0 and sum of the pheromone level 
		// for this event and current sum of heuristic information
		double rnd = solution->rg->next() * range;
		
		// choose a timeslot for the event based on the pheromone table and the random number
		double total = 0.0;
		int timeslot = -1;
		for(int j=0;j<N_OF_TIMESLOTS;j++)	{
			// check the pheromone 
			total += problem->event_timeslot_pheromone[e][j];
			if (total>=rnd) {
				timeslot = j;
				break;
			}
		}
		// put an event i into timeslot t
		solution->sln[e].first = timeslot;
		solution->timeslot_events[timeslot].push_back(e);
	}

	// assign rooms to events in each non-empty timeslot
	for(int i=0;i<N_OF_TIMESLOTS;i++)
		if((int)solution->timeslot_events[i].size())
			solution->assignRooms(i);
}

void Ant::depositPheromone()
{
	// calculate pheromone update
	for (int i=0;i<problem->n_of_events;i++) {
		int timeslot = solution->sln[i].first;
		problem->event_timeslot_pheromone[i][timeslot] += 1.0;
	}
}

int Ant::computeFitness()
{
	// simple fitness function
	fitness = solution->computeHcv();
	return fitness;
}

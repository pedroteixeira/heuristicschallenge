/***************************************************************************
    MMAsProblem.cpp - Implementation of the problem as seen 
		                  by MMAs. 
                             -------------------
    begin                : Mon Mar 11, 2002
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

#include "MMAsProblem.h"

MMAsProblem::MMAsProblem(istream& ifs, double evap, double phe_min) : Problem(ifs) {
	//member variables initialization
	event_timeslot_pheromone = DoubleMatrixAlloc(n_of_events,N_OF_TIMESLOTS);
	this->evap = evap;
	this->phe_min = phe_min;

	if (evap<1.0)
	  phe_max = 1.0/(1.0 - evap);
	else
	  phe_max = 999999;

	// creating a set of pre-sorted event lists

	// sorting events based on level of correlations
	int *event_correlation = new int[n_of_events];
	for (int i=0;i<n_of_events;i++) { 
		// summing up the correlations for each event
		event_correlation[i] = 0;
		for (int j=0;j<n_of_events;j++)
			event_correlation[i] += eventCorrelations[i][j];
	}
	for(int i=0;i<n_of_events;i++) { 
	  // sorting the list
		int max_correlation = -1;
		int event_index = -1;
		for (int j=0;j<n_of_events;j++) {
			if (event_correlation[j]>max_correlation) {
				max_correlation = event_correlation[j];
				event_index = j;
			}
		}
		event_correlation[event_index] = -2;
		sorted_event_list.push_back(event_index);
	}
	delete [] event_correlation;
}

MMAsProblem::~MMAsProblem() {
	free(event_timeslot_pheromone);
}

void MMAsProblem::pheromoneReset() {
	// initialize pheromon levels between events and timeslots to the maximal values
	for (int i=0;i<n_of_events;i++)
		for (int j=0;j<N_OF_TIMESLOTS;j++) {
			event_timeslot_pheromone[i][j] = phe_max;
		}
}

void MMAsProblem::evaporatePheromone() {
	// evaporate some pheromone
	for (int i=0;i<n_of_events;i++)
		for (int j=0;j<N_OF_TIMESLOTS;j++)
			event_timeslot_pheromone[i][j] *= evap;
}

void MMAsProblem::pheromoneMinMax() {
	// limit pheromone values according to MAX-MIN
	for (int i=0;i<n_of_events;i++)
		for (int j=0;j<N_OF_TIMESLOTS;j++) {
			if (event_timeslot_pheromone[i][j] < phe_min)
				event_timeslot_pheromone[i][j] = phe_min;
			if (event_timeslot_pheromone[i][j] > phe_max)
				event_timeslot_pheromone[i][j] = phe_max;
		}
}

/***************************************************************************
    MMAsProblem.h - Header file for the implementation of the problem as seen
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

#ifndef MMASPROBLEM_H
#define MMASPROBLEM_H

#include "defs.h"

#include <math.h>

#include "Problem.h"
#include "util.h"

class MMAsProblem : public Problem  
{
public:
	void pheromoneMinMax();
	void evaporatePheromone();
	double evap;
	double phe_max;
	double phe_min;
	double alpha;
	double** event_timeslot_pheromone; // matrix keeping pheromone between events and timeslots
	vector<int> sorted_event_list; // vector keeping sorted lists of events

	void pheromoneReset();

	MMAsProblem(istream& ifs, double evap, double phe_min);
	virtual ~MMAsProblem();
};

#endif MMASPROBLEM_H

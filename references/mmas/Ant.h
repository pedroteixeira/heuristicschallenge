/***************************************************************************
    Ant.h - Header file for the implementation of the ant. 
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

#ifndef ANT_H
#define ANT_H

#include "defs.h"

#include <vector>
#include <math.h>

#include "Solution.h"
#include "Random.h"

#include "MMAsProblem.h"

class Ant  
{
public:
	int fitness;
	int computeFitness();
	Solution* solution;

	void depositPheromone();
	void Move();

	Ant(MMAsProblem* problem, Random *rnd);
	virtual ~Ant();

private:
	MMAsProblem* problem;
};

#endif ANT_H

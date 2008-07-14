/***************************************************************************
                          Solution.h  -  description
                             -------------------
    begin                : Thu Nov 22 2001
    copyright            : (C) 2001 by Olivia Rossi-Doria
    email                : o.rossi-doria@napier.ac.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SOLUTION_H
#define SOLUTION_H

#include "Problem.h"
#include "Timer.h"
#include "Random.h"

#include <vector>
#include <map>
#include <algorithm>

// a solution is a list of event assignments to timeslots and rooms
// stored in a vector of pairs of integers, one for each event,
// representing the timeslot and room assigned to that event respectively 

class Solution{
  public:

  vector<pair<int, int> > sln; // vector of (timeslot, room) assigned for each event
  map<int, vector<int> > timeslot_events; // for each timeslot a vector of events taking place in it
  Problem* data; // a pointer to the problem data
  Timer timer;
  Random* rg;

  bool feasible;
  int scv;   // keeps the number of soft constraint violations (ComputeScv() has to be called)
  int hcv;  // keeps the number of hard constraint violations (computeHcv() has to be called)
 
  Solution(Problem* pd, Random* rnd); //constructor with pointers to the problem data and to the random object 

  void RandomInitialSolution(); // produce a random initial solution
  bool computeFeasibility(); // check feasibilityXFProblem.cpp
  int computeScv();         // compute soft constraint violations
  void copy(Solution *orig); // copy a pointer to a Solution into this one

  int computeHcv(); // compute hard constraint violations

  void Move1(int e, int t); // move event e to timeslot t (type 1 move)
  void Move2(int e1,int e2); // swaps events e1 and e2 (type 2 move)
  void Move3(int e1, int e2, int e3); // 3-cycle permutation of events e1, e2 and e3 (type 3 move)
  void randomMove(); // do one of the three possible moves for random events and timeslots
  void localSearch(int maxSteps, double LS_limit = 999999, double prob1 = 1.0, double prob2 = 1.0, double prob3 = 0.0);//apply local search with the given parameters
  void assignRooms(int t); // assign rooms to events for timeslot t (first apply matching algorithm and then assign unplaced rooms)
   
 private:
  // data structure for the matching algorithm
  int** size; 
  int** flow;
  vector<int> val;
  vector<int> dad; 

  void slnInit();  // initialize solution
  int eventHcv(int e);   // evaluate number of hcv caused by event e
  int eventAffectedHcv(int e); // evaluate the hcv that might be affected when event e is moved from its timeslot
  //int affectedRoomHcv(int e); // evaluete the "only one class can be in each room at any timeslot" hcv for all the other events in the timeslot of event e
  int affectedRoomInTimeslotHcv( int t); // evaluate all the room hcv as above for all the events in timeslot t
  int eventScv(int e);   // evaluate number of scv caused by event e
  int singleClassesScv(int e); // evaluate the number of single classes that event e actually solves in the day (or created if e leaves its timeslot)
  void maxMatching(int V);    // do the max cardinality matching using a deterministic network flow algorithm
  bool networkFlow(int V);  // network flow algorithm
};

#endif
        
   


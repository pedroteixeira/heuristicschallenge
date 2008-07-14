/***************************************************************************
                          Problem.h  -  description
                             -------------------
    begin                : Wed Nov 21 2001
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

#ifndef PROBLEM_H
#define PROBLEM_H

#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include "util.h"

#include <vector>
#include <map>

class Problem {
 public:

  int n_of_events;
  int n_of_rooms;
  int n_of_features;
  int n_of_students;
  vector<int> studentNumber; // vector of student numbers, one for each event 
  vector<int> roomSize; // vector of room sizes one for each room
  int** student_events; // student attendance matrix
  int** eventCorrelations; // matrix keeping pre-processed information on events having students in common
  int** room_features; // matrix keeping information on features satisfied by rooms
  int** event_features; // matrix keeping information on features required by events

  int** possibleRooms; // matrix keeping pre-processed information on which room are suitable for each event
  
  Problem( istream& );
  ~Problem();
};

#endif

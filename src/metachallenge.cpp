//============================================================================
// Name        : metachallenge.cpp
// Author      : Pedro Teixeira
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "headers/steiner.hpp"
#include "headers/ant.hpp"
#include "headers/sa.hpp"
#include "headers/steiner_grasp.hpp"

#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>

#include <boost/random.hpp>

int main() {

	//log4cxx::BasicConfigurator::configure();

	const string steiner_1_path = "data/steiner-1-2353.stp"; //0.2 para < 2420, 0.5 para < 2390
	const string steiner_2_path = "data/steiner-2-3413.stp"; //0.2 para < 3495, 0.5 para < 3465


	Steiner instance(steiner_2_path);
	SteinerGRASP grasp(instance);
	grasp.run();



	return 0;
}

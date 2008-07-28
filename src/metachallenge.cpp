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

	//SaSteiner sa_steiner(steiner_1_path);
	//sa_steiner.run();

	Steiner instance(steiner_1_path);
	SteinerGRASP grasp(instance);
	grasp.run();



	return 0;
}

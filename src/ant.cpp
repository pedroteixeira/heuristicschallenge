/*
 * ant.cpp
 *
 *  Created on: Jul 12, 2008
 *      Author: pedro
 */

#include <assert.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>

#include "headers/steiner.hpp"
#include "headers/ant.hpp"

using namespace std;

AntSteiner::AntSteiner(string instance_path) {
	instance = new Steiner(instance_path);

	//pre process
}

void AntSteiner::run() {



}

AntSteiner::~AntSteiner() {
	delete instance;
}

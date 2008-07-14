/*
 * sa.cpp
 *
 *  Created on: Jul 13, 2008
 *      Author: pedro
 */

#include <assert.h>
#include <math.h>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>

#include "headers/steiner.hpp"
#include "headers/sa.hpp"

using namespace std;

SaSteiner::SaSteiner(string instance_path) {
	instance = new Steiner(instance_path);

	//pre process
}

void SaSteiner::run() {

	//parameters
	int secondsToLive = 60;
	int iterations = 600 * instance->V;
	float temperature = secondsToLive * instance->V;
	float alpha = 0.995f;

	//initial solution & energy
	vector < Edge > solution;
	instance->generate_chins_solution( solution );




}

SaSteiner::~SaSteiner() {
	delete instance;
}

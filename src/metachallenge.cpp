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

int main() {

	const string steiner_1_path = "data/steiner-1-2353.stp"; //ótimo 2353

	SaSteiner sa_steiner(steiner_1_path);
	sa_steiner.run();



	return 0;
}

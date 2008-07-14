#ifndef CONTROL_H
#define CONTROL_H

/*
	This is the Control class for timetabling. All metaheuristics
	implementations should use it for their command line options and
	output.
*/

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include "Solution.h"
#include "Timer.h"


class Control {
private:
	map< string, string > parameters;
	Timer timer;
	
	int nrTry, maxTry;
	bool feasible;
	int currentScv;
	int bestScv;
	int bestEvaluation;
	int seed;
	double timeLimit;
	int problemType; 
	int maxSteps;
	double LS_limit;
	double prob1, prob2, prob3;

	ostream *os;
	istream *is;
	
public:
	Control( int, char** ); // construct a control object, given a set of command line options
	~Control();
	
	bool parameterExists( string ); // check whether a certain parameter is given at the command line
	int getIntParameter( string ); // get the integer contents of a parameter
	double getDoubleParameter( string ); // get the double contents of a parameter
	string getStringParameter( string ); // get the string contents of a parameter
	
	void resetTime(); // reset the timer
	double getTime(); // get the timer's time
	bool timeLeft() { return ( getTime() < timeLimit ); }; // check whether our run has time left
	
	void beginTry(); // begin a run
	void endTry(Solution*); // end a run
	
	void setCurrentCost(Solution* ); // tell control the quality of your solution
	
	istream& getInputStream() { return *is; } // get the input stream for constructing problems
	ostream& getOutputStream() { return *os; } // get the output stream used for writing output data
	int getSeed() { return seed; } // get the seed for the random number generator
	int getNumberOfTries() { return maxTry; } // get the max number of tries
	bool triesLeft() { return ( nrTry < maxTry ); } // check whether more tries need to be run
	double getTimeLimit() { return timeLimit; } // get the time limit of one run
	int getMaxSteps(){ return maxSteps; } // get the type of problem instance
	double getLS_limit() { return LS_limit;} // get the time limit for a single local search run
	double getProb1() { return prob1;} // get the probability to be performed for each move of type 1 in the local search
	double getProb2() { return prob2;} // get the probability for each move of type 2 in the local search
	double getProb3() { return prob3;} // get the probability for each move of type 3 in the local search  
};

#endif

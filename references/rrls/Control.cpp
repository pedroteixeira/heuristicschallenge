#include "Control.h"

Control::Control( int argc, char** argv ) {

	// parse the command line options to set all vars

	if( ( argc % 2 == 0 ) || ( argc == 1 ) ) {
		cerr << "Parse error: Number of command line parameters incorrect\n";
		cerr << "Usage:" << endl;
		cerr << argv[ 0 ] << " -i InputFile [-o OutputFile] [-n NumberOfTries] [-s RandomSeed] [-t TimeLimit] [-p ProblemType]" << endl;
		exit(1);
	}
	
	for( int i = 1; i < argc / 2 + 1; i++ ) {
		parameters[ argv[ i * 2 - 1 ] ] = argv[ i * 2 ];
	}
	
	nrTry = 0;
	
	// check for input parameter
	
	if( parameterExists( "-i") ) {
		is = new ifstream( getStringParameter( "-i" ).c_str() );
	} else {
		cerr << "Error: No input file given, exiting" << endl;
		cerr << "Usage:" << endl;
		cerr << argv[ 0 ] << " -i InputFile [-o OutputFile] [-n NumberOfTries] [-s RandomSeed] [-t TimeLimit] [-p ProblemType]" << endl;
		exit(1);
	}
	
	// check for ouput parameter
	
	if( parameterExists( "-o" ) ) {
		os = new ofstream( getStringParameter( "-o" ).c_str() );
	} else {
		cerr << "Warning: No output file given, writing to stdout" << endl;
		os = &cout;
	}
	
	// check for number of tries parameter
	
	if( parameterExists( "-n" ) ) {
		maxTry = getIntParameter( "-n" );
		cout << "Max number of tries " << maxTry << endl;
	} else {
		cerr << "Warning: Number of tries is set to default (10)" << endl;
		maxTry = 10; // default number of tries
	}

	// check for time limit parameter
	
	if( parameterExists( "-t" ) ) {
		timeLimit = getDoubleParameter( "-t" );
		cout <<"Time limit " << timeLimit << endl;
	} else {
		cerr << "Warning: Time limit is set to default (90 sec)" << endl;
		timeLimit = 90; // default time limit
	}

        // check for problem instance type parameter for the local search

	if( parameterExists( "-p" ) ) {
		problemType = getIntParameter( "-p" );
		cout <<"Problem instance type " << problemType << endl;
	} else {
	  //cerr << "Warning: The problem instance type is set by default to 1 (easy)" << endl;
		problemType = 1; // default problem type
	}


        // check for maximum steps parameter for the local search
	
	if( parameterExists( "-m" ) ) {
		maxSteps = getIntParameter( "-m" );
		cout <<"Max number of steps in the local search " << maxSteps << endl;
	} else {
	  //cerr << "Warning: The maximum number of steps for the local search is set by default to 100" << endl;
		maxSteps = 100; // default max steps
	}

        // check for time limit parameter for the local search
	
	if( parameterExists( "-l" ) ) {
		LS_limit = getDoubleParameter( "-l" );
		cout <<"Local search time limit " << LS_limit << endl;
	} else {
		cerr << "Warning: The local search time limit is set to default (99999 sec)" << endl;
		LS_limit = 99999; // default local search time limit
	}

        // check for probability parameter for each move in the local search
	
	if( parameterExists( "-p1" ) ) {
		prob1 = getDoubleParameter( "-p1" );
		cout << "LS move 1 probability " << prob1 <<endl;
	} else {
		cerr << "Warning: The local search move 1 probability is set to default 1.0" << endl;
		prob1 = 1.0; // default local search probability for each move of type 1 to be performed
	}

	if( parameterExists( "-p2" ) ) {
		prob2 = getDoubleParameter( "-p2" );
		cout <<"LS move 2 probability " << prob2 << endl;
	} else {
		cerr << "Warning: The local search move 2 probability is set to default 1.0" << endl;
		prob2 = 1.0; // default local search probability for each move to be performed
	}

	if( parameterExists( "-p3" ) ) {
		prob3 = getDoubleParameter( "-p3" );
		cout <<"LS move 3 probability " << prob3 <<  endl;
	} else {
		cerr << "Warning: The local search move 3 probability is set to default 0.0" << endl;
		prob3 = 0.0; // default local search probability for each move to be performed
	}

	// check for random seed
	
	if( parameterExists( "-s" ) ) {
		seed = getIntParameter( "-s" );
		srand( seed );
	} else {
		seed = time( NULL );
		cerr << "Warning: " << seed << " used as default random seed" << endl;
		srand( seed );
	}
}

Control::~Control() {
}

bool
Control::parameterExists( string paramName ) {
	for( map< string, string >::iterator i = parameters.begin(); i != parameters.end(); i++ ) {
		if( i-> first == paramName )
			return true;
	}
	return false;
}

int
Control::getIntParameter( string paramName ) {
	if( parameterExists( paramName ) )
		return atoi( parameters[paramName].c_str() );
	else {
		return 0;
	}
}

double
Control::getDoubleParameter( string paramName ) {
	if( parameterExists( paramName ) )
		return atof( parameters[paramName].c_str() );
	else {
		return 0;
	}
}

string
Control::getStringParameter( string paramName ) {
	if( parameterExists( paramName ) )
		return parameters[paramName];
	else {
		return 0;
	}
}

void
Control::resetTime() {
	timer.resetTime();
}

double
Control::getTime() {
	return timer.elapsedTime( Timer::VIRTUAL );
}

void
Control::beginTry() {
	srand( seed++ );
	(*os) << "begin try " << ++nrTry << endl;
	resetTime();
	feasible = false;
	bestScv = INT_MAX;
	bestEvaluation = INT_MAX;
}

void
Control::endTry( Solution *bestSolution) {
  (*os) << "begin solution " << nrTry << endl;
  if(bestSolution->feasible){
    (*os) << "feasible: evaluation function = " << bestSolution->scv <<endl;
    for(int i = 0; i < (*bestSolution).data->n_of_events; i++)
      (*os) << bestSolution->sln[i].first << " " ;
    (*os) << endl;
    for(int i = 0; i < (*bestSolution).data->n_of_events; i++)
      (*os) << bestSolution->sln[i].second << " " ;
    (*os) << endl;
  }
  else{
    (*os) << "unfeasible: evaluation function = " << (bestSolution->computeHcv() * 1000000) + bestSolution->computeScv() <<endl;
  }
  (*os) << "end solution " << nrTry << endl;
  (*os) << "end try " << nrTry << endl;

  // The following output might be used if the ./checksln program wants to be used to check the validity of solutions.
  // Remember that the output file has then to have the .sln extension and has to have the same name as the .tim instance file
  /*for(int i = 0; i < (*bestSolution).data->n_of_events; i++){
    (*os) << bestSolution->sln[i].first << " "<< bestSolution->sln[i].second;
    (*os) << endl;
    }*/
}

void
Control::setCurrentCost(Solution *currentSolution ) {
  //if( timeLeft() ) {
	  int currentScv = currentSolution->scv;
	  if( currentSolution->feasible && currentScv < bestScv ) {
	    bestScv = currentScv;
	    bestEvaluation = currentScv;
	    double time = getTime();
	    (*os) << "best " << bestScv << " time ";
			os->flags( ios::fixed );
			(*os) << ( time < 0 ? 0.0 : time ) << endl;
	  }
	  else if(!currentSolution->feasible){ 
	    int currentEvaluation = (currentSolution->computeHcv() * 1000000) + currentSolution->computeScv();
	    if(currentEvaluation < bestEvaluation){
	      bestEvaluation = currentEvaluation;
	      double time = getTime();
	    (*os) << "best " << bestEvaluation << " time ";
			os->flags( ios::fixed );
			(*os) << ( time < 0 ? 0.0 : time ) << endl;
	    }
	  }
	  //}
}


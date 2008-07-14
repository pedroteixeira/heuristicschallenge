TARGET = mmas
CXXFLAGS = -Wall -ansi -O3 

all: ${TARGET}

mmas: MMAsProblem.cpp Ant.cpp MMAs.cpp Problem.cpp Solution.cpp Timer.C Random.cc Control.cpp util.cpp
	${CXX} ${CXXFLAGS} -o $@ $^

clean:
	@rm -f *~ *.o ${TARGET} core DEADJOE

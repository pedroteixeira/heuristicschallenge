/*
 * steiner.hpp
 *
 *  Created on: Jul 12, 2008
 *      Author: pedro
 */

#ifndef STEINER_HPP_
#define STEINER_HPP_

#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include "util.hpp"

class Steiner {
public:
	int V; //number of vertices
	int E; //number of edges
	Graph graph;
	std::vector<int> terminals;
	Steiner(std::string);
	~Steiner();

	void generate_chins_solution(std::vector<Edge>&);

private:
	void read_graph_section(std::ifstream&);
	void read_terminals_section(std::ifstream&);
	void read_coordinates_section(std::ifstream&);
};

#endif /* STEINER_HPP_ */

/*
 * steiner.hpp
 *
 *  Created on: Jul 12, 2008
 *      Author: pedro
 */

#ifndef STEINER_HPP_
#define STEINER_HPP_

#include "util.hpp"

class Steiner {
public:
	int V; //number of vertices
	int E; //number of edges
	Graph graph;
	std::vector<int> terminals;
	std::map<int, std::vector<int> > distances_from_terminal;
	std::map<int, std::vector<Vertex> > parents_from_terminal;
	Steiner(std::string);
	Steiner();
	~Steiner();

private:

	void read_graph_section(std::ifstream&);
	void read_terminals_section(std::ifstream&);
	void read_coordinates_section(std::ifstream&);
};

#endif /* STEINER_HPP_ */

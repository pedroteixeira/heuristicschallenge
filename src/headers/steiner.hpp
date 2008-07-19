/*
 * steiner.hpp
 *
 *  Created on: Jul 12, 2008
 *      Author: pedro
 */

#ifndef STEINER_HPP_
#define STEINER_HPP_

#include "util.hpp"
#include "graph.hpp"
#include <boost/random.hpp>

class Steiner {
public:
	int V; //number of vertices
	int E; //number of edges
	Graph graph;
	std::vector<Vertex> terminals;
	std::map<int, VertexInfo> vertices_info;
	std::map<Vertex, DistanceMap > distances_from_terminal;
	std::map<Vertex, PredecessorMap > parents_from_terminal;
	Steiner(std::string);
	Steiner();

	boost::mt19937 rng;

private:

	void read_graph_section(std::ifstream&);
	void read_terminals_section(std::ifstream&);
	void read_coordinates_section(std::ifstream&);
};

#endif /* STEINER_HPP_ */

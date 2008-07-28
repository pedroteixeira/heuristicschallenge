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
	Steiner(const Steiner&);
	Steiner(std::string);

	int V; //number of vertices
	int E; //number of edges
	Graph graph;
	IntSet terminals;
	std::map<int, VertexInfo> vertices_info;

	boost::mt19937 rng;

	bool is_terminal(int v);
	std::pair<std::vector<int>, std::vector<int> > get_shortest_distances(int v);
	int get_parent_in_shortest_path(int, int);

private:

	std::map<int, std::vector<int> > shortest_distances;
	std::map<int, std::vector<int> > shortest_parents;

	void read_graph_section(std::ifstream&);
	void read_terminals_section(std::ifstream&);
	void read_coordinates_section(std::ifstream&);
};

#endif /* STEINER_HPP_ */

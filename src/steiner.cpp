/*
 * steiner.cpp
 *
 *  Created on: Jul 12, 2008
 *      Author: pedro
 */

#include <assert.h>
#include <math.h>
#include <fstream>
#include <iostream>

#include <boost/progress.hpp>
#include <boost/multi_array.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>

#include "headers/steiner.hpp"
#include "headers/steiner_solution.hpp"
using namespace std;



Steiner::Steiner() {
	//TODO:
}

Steiner::Steiner(string path) {

	ifstream in_data(path.c_str());

	if (!in_data) {
		cerr << "cannot open file " << path << endl;
		throw "invalid path";
	}

	cout << "reading instance from" << path << endl;

	string line;
	while (getline(in_data, line)) {
		if (line.find("Section Graph") != string::npos) {
			read_graph_section(in_data);
		} else if (line.find("Section Terminals") != string::npos) {
			read_terminals_section(in_data);
		} else if (line.find("Section Coordinates") != string::npos) {
			read_coordinates_section(in_data);
		}
	}

	cout << "done reading." << endl;

	//pre-calculations


	boost::timer timer;
	cout << "pre-calculating distances from all terminals..." << endl;
	for (vector<int>::const_iterator i = terminals.begin(); i != terminals.end(); ++i) {
		int terminal = (*i);
		vector<Vertex> parents(V);
		vector<int> distances(V);

		boost::dijkstra_shortest_paths(graph, terminal, boost::weight_map(boost::get(&EdgeInfo::weight,
				graph)).distance_map(&distances[0]).predecessor_map(&parents[0]));

		distances_from_terminal[terminal] = distances;
		parents_from_terminal[terminal] = parents;
	}
	cout << "distances computed in " << timer.elapsed() << " seconds." << endl;

}

inline void Steiner::read_graph_section(ifstream & in_data) {

	in_data.ignore(INT_MAX, ' ') >> V;
	in_data.ignore(INT_MAX, ' ') >> E;

	cout << "instance has " << V << " vertices and " << E << " edges." << endl;

	graph = Graph(V);

	string line;
	int node1, node2, weight;

	for (int i = 0; i < E; i++) {
		in_data.ignore(INT_MAX, ' ') >> node1 >> node2 >> weight;

		//add an edge to the graph
		EdgeInfo edge;
		edge.weight = weight;
		boost::add_edge(node1 - 1, node2 - 1, edge, graph); //indices are 0-based
	}

	assert((int)boost::num_edges(graph) == E);
	assert((int)boost::num_vertices(graph) == V);
}

inline void Steiner::read_terminals_section(ifstream & in_data) {

	int number_terminals;
	in_data.ignore(INT_MAX, ' ') >> number_terminals;

	cout << "instance has " << number_terminals << " terminals." << endl;

	int terminal_node;
	for (int i = 0; i < number_terminals; i++) {
		in_data.ignore(INT_MAX, ' ') >> terminal_node; //indices are 0-based
		terminals.push_back(terminal_node - 1);
	}

	assert((int)terminals.size() == number_terminals);

}

inline void Steiner::read_coordinates_section(ifstream & in_data) {

	//only for drawing
	for (int i = 0; i < V; i++) { /* indices are 0-based */
		in_data.ignore(INT_MAX, ' ').ignore(INT_MAX, ' ') >> graph[i].x >> graph[i].y;
	}

}

Steiner::~Steiner() {
}

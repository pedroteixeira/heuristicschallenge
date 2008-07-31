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
#include <boost/graph/graph_utility.hpp>

#include "headers/steiner.hpp"
#include "headers/graph.hpp"
#include "headers/steiner_solution.hpp"
using namespace std;

Steiner::Steiner(const Steiner& steiner) {
	cout << "Steiner::Steiner(const Steiner& steiner)" << endl;

}

Steiner::Steiner(string path) {
	cout << "Steiner::Steiner(string path)" << endl;

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
}

inline void Steiner::read_graph_section(ifstream & in_data) {

	in_data.ignore(INT_MAX, ' ') >> V;
	in_data.ignore(INT_MAX, ' ') >> E;

	cout << "instance has " << V << " vertices and " << E << " edges." << endl;

	string line;
	int node1, node2, weight;

	for (int i = 0; i < E; i++) {
		in_data.ignore(INT_MAX, ' ') >> node1 >> node2 >> weight;

		//add an edge to the graph (indices are 0-based)
		graph.add_edge(node1 - 1, node2 - 1, weight);
	}

	assert(graph.num_edges() == E);
	assert(graph.num_vertices() == V);
}

inline void Steiner::read_terminals_section(ifstream & in_data) {

	int number_terminals;
	in_data.ignore(INT_MAX, ' ') >> number_terminals;

	cout << "instance has " << number_terminals << " terminals." << endl;

	int terminal_node;
	for (int i = 0; i < number_terminals; i++) {
		in_data.ignore(INT_MAX, ' ') >> terminal_node;

		//indices are 0-based
		terminals.push_back(terminal_node - 1);
	}

	assert((int)terminals.size() == number_terminals);

}

inline void Steiner::read_coordinates_section(ifstream & in_data) {

	//only for drawing
	for (int i = 0; i < V; i++) { /* indices are 0-based */
		VertexInfo info;
		in_data.ignore(INT_MAX, ' ').ignore(INT_MAX, ' ') >> info.x >> info.y;
		vertices_info[i] = info;
	}
}

bool Steiner::is_terminal(int v) {
	return terminals.get<1> ().find(v) != terminals.get<1> ().end(); //make it O(1)
}



int Steiner::get_parent_in_shortest_path(int from, int child) {

	vector<int> distances, parents;
	boost::tie(distances, parents) = get_shortest_distances(from);

	return parents[child];
}

std::pair<std::vector<int>, std::vector<int> > Steiner::get_shortest_distances(int from) {

	if (shortest_distances.find(from) == shortest_distances.end()) {

		DistanceMap distances;
		PredecessorMap parents;

		graph.dijkstra_shortest_paths(from, distances, parents);

		//convert and store
		vector<int> vector_distances(V);
		vector<int> vector_predecessors;
		for (int i = 0; i < V; i++) {
			Vertex v = graph.get_vertex(i);
			vector_distances.insert(vector_distances.begin() + i, distances[v]);
			vector_predecessors.insert(vector_predecessors.begin() + i, graph.index_for_vertex(parents[v]));
		}

		shortest_distances[from] = vector_distances;
		shortest_parents[from] = vector_predecessors;
	}

	return make_pair(shortest_distances[from], shortest_parents[from]);
}



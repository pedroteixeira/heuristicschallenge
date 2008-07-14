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
#include <boost/graph/adjacency_list.hpp>

#include "headers/steiner.hpp"
using namespace std;

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
		Edge edge;
		edge.weight = weight;
		boost::add_edge(node1, node2, edge, graph);
	}

	assert((int)boost::num_edges(graph) == E);
}

inline void Steiner::read_terminals_section(ifstream & in_data) {

	in_data.ignore(INT_MAX, ' ') >> number_terminals;

	cout << "instance has " << number_terminals << " terminals."<<endl;

	terminals = new int[number_terminals];

	string line;
	for (int i = 0; i < number_terminals; i++) {
		getline(in_data, line);
		in_data.ignore(INT_MAX, ' ') >> terminals[i];
	}

}

inline void Steiner::read_coordinates_section(ifstream & in_data) {

	//only for drawing
	for (int i = 1; i <= V; i++) { /* indices of instances are 1-based */
		in_data.ignore(INT_MAX, ' ').ignore(INT_MAX, ' ') >> graph[i].x >> graph[i].y;
	}


	boost::graph_traits<Graph>::edge_iterator ei, ei_end;
	for ( boost::tie(ei, ei_end) = boost::edges(graph); ei != ei_end; ++ei) {
		boost::graph_traits<Graph>::edge_descriptor e = *ei;
		boost::graph_traits<Graph>::vertex_descriptor u = boost::source(e, graph), v = boost::target(e, graph);

		graph[e].distance = sqrt((graph[u].x - graph[v].x) * (graph[u].x - graph[v].x) + (graph[u].y - graph[v].y)
				* (graph[u].y - graph[v].y));

		/*
		 cout << u << "(" << graph[u].x << "," << graph[u].y << ")"
		 << v << "(" << graph[v].x << "," << graph[v].y << ")"
		 << " " <<  graph[e].weight  << " " << graph[e].distance << endl;
		 */

	}
}

Steiner::~Steiner() {
	delete terminals;
}

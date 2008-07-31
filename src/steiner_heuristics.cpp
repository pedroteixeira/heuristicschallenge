/*
 * steiner_heuristics.cpp
 *
 *  Created on: Jul 27, 2008
 *      Author: pedro
 */

#include "headers/steiner_solution.hpp"
#include "headers/steiner_heuristics.hpp"

#include <boost/random.hpp>

using namespace std;


SteinerSolution SteinerHeuristics::generate_network_distance_solution(Steiner& instance) {

	SteinerSolution solution(instance);

	//add all terminals and set weight to shortest distance
	//variant is to connect only closest.

	for (size_t i = 0; i < instance.terminals.size(); i++) {
		int t1 = instance.terminals[i];
		vector<int> distances, parents;
		boost::tie(distances, parents) = instance.get_shortest_distances(t1);

		for (size_t j = i + 1; j < instance.terminals.size(); j++) {
			int t2 = instance.terminals[j];
			solution.graph.add_edge(t1, t2, distances[t2]);
		}
	}

	solution.find_mst_tree();

	//replace edges
	for (size_t i = 0; i < instance.terminals.size(); i++) {
		int t1 = instance.terminals[i];
		vector<int> distances, parents;
		boost::tie(distances, parents) = instance.get_shortest_distances(t1);

		for (size_t j = i + 1; j < instance.terminals.size(); j++) {
			int t2 = instance.terminals[j];

			if(solution.graph.contains_edge(t1, t2)) {
				solution.graph.remove_edge(t1, t2);
				solution.add_path(t1, t2, parents);
			}
		}
	}


	//solution.find_mst_tree();
	//induce on original
	solution.grow_graph();
	solution.find_mst_tree();


	//replace by corresponding paths
	return solution;
}

/**
 * Heuristics to generate good solution.
 */
SteinerSolution SteinerHeuristics::generate_chins_solution(Steiner& instance) {
	//pick random initial terminal
	boost::uniform_int<> range(0, instance.terminals.size() - 1);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(instance.rng, range);
	int t0 = die();

	list<int> virtual_terminals;
	virtual_terminals.push_back(t0);

	return generate_chins_solution(virtual_terminals, instance);
}

SteinerSolution SteinerHeuristics::generate_chins_solution(const list<int>& virtual_terminals, Steiner& instance) {

	SteinerSolution solution(instance);

	//initialize list temp terminals
	IntSet terminals_left(solution.instance.terminals);

	IntSet vertices_in_solution;

	foreach(int vt, virtual_terminals) {
		terminals_left.push_back(vt);
	}

	//pick random root
	boost::uniform_int<> range(0, terminals_left.size() - 1);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(instance.rng, range);
	int root = terminals_left[die()];

	terminals_left.remove(root);
	vertices_in_solution.push_back(root);


	while (terminals_left.size() > 0) {

		int closest_distance = INT_MAX;
		int closest_terminal, closest_vertex_in_tree;
		foreach(int t, terminals_left) {
			vector<int> distances, parents;
			boost::tie(distances, parents) = instance.get_shortest_distances(t);

			//choose terminal closest to any of the vertices added (NOTE: better only terminals?)
			foreach(int v, vertices_in_solution) {
				int distance_to_v = distances[v];

				if (distance_to_v < closest_distance) {
					closest_distance = distance_to_v;
					closest_terminal = t;
					closest_vertex_in_tree = v;
				}
			}
		}

		terminals_left.remove(closest_terminal);

		//traverse shortest path from terminal
		int parent = closest_vertex_in_tree, last_parent = closest_vertex_in_tree;
		while (last_parent != closest_terminal) {
			//add all vertices in this path
			bool not_in_solution = vertices_in_solution.get<1> ().find(parent) == vertices_in_solution.get<1> ().end();
			if (not_in_solution) {
				vertices_in_solution.push_back(parent);
			}

			last_parent = parent;


			parent = instance.get_parent_in_shortest_path(closest_terminal, parent);
		};
	}



	//create a sub graph structure with only the vertices added so far
	foreach(int v, vertices_in_solution) {

		Vertex vertex = solution.instance.graph.get_vertex(v);

		//bring any feasible adges (i.e. connecting with vertices in chins solution)
		boost::graph_traits<BoostGraph>::adjacency_iterator vi, viend;
		for ( boost::tie(vi, viend) = boost::adjacent_vertices(vertex, solution.instance.graph.boostgraph); vi != viend; ++vi) {

			int index_neighboor = solution.instance.graph.index_for_vertex(*vi);

			//add only vertices that were considered
			bool is_feasible = vertices_in_solution.get<1> ().find(index_neighboor) != vertices_in_solution.get<1> ().end();

			if (is_feasible) {
				solution.add_edge_from_original(v, index_neighboor); //add edge with same weight to this sub graph
			}
		}
	}


	//initialize
	solution.find_mst_tree();

	//Experiment
	solution.grow_graph();
	solution.find_mst_tree();

	return solution;
}

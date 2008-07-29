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

/**
 * Heuristics to generate good solution.
 */
SteinerSolution SteinerHeuristics::generate_chins_solution(Steiner& instance) {
	//pick random initial terminal
	boost::uniform_int<> range(0, instance.terminals.size() - 1);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(instance.rng, range);
	int t0 = die();

	return generate_chins_solution(t0, instance);
}

SteinerSolution SteinerHeuristics::generate_chins_solution(int root, Steiner& instance) {

	SteinerSolution solution(instance);

	//initialize list temp terminals
	IntSet terminals_left(solution.instance.terminals);

	IntSet vertices_in_solution;

	terminals_left.remove(root);
	vertices_in_solution.push_back(root);

	while (terminals_left.size() > 0) {

		//randomize (give preference to closer terminals?)

		boost::uniform_int<> range(0, terminals_left.size() - 1);
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(instance.rng, range);
		int closest_terminal = terminals_left[die()];

		vector<int> distances, parents;
		boost::tie(distances, parents) = instance.get_shortest_distances(closest_terminal);

		int closest_distance = INT_MAX, closest_vertex_in_tree;
		foreach(int v, vertices_in_solution) {
			int distance_to_v = distances[v];

			if (distance_to_v < closest_distance) {
				closest_distance = distance_to_v;
				closest_vertex_in_tree = v;
			}
		}

		//choose terminal closest to any of the vertices added
		/*
		 int closest_distance = INT_MAX;
		 int closest_terminal, closest_vertex_in_tree;
		 foreach(int t, terminals_left) {
		 vector<int> distances, parents;
		 boost::tie(distances, parents) = instance.get_shortest_distances(t);

		 foreach(int v, vertices_in_solution) {
		 int distance_to_v = distances[v];

		 if (distance_to_v < closest_distance) {
		 closest_distance = distance_to_v;
		 closest_terminal = t;
		 closest_vertex_in_tree = v;
		 }
		 }
		 }
		 */

		terminals_left.remove(closest_terminal);

		//traverse shortest path from terminal
		int parent = closest_vertex_in_tree, last_parent = closest_vertex_in_tree;
		while (last_parent != closest_terminal) {
			//add all vertices in this path
			bool not_in_solution = vertices_in_solution.get<1> ().find(parent) == vertices_in_solution.get<1> ().end();
			if (not_in_solution)
				vertices_in_solution.push_back(parent);

			last_parent = parent;

			parent = instance.get_parent_in_shortest_path(closest_terminal, parent);
		};

	}

	//cout << "creating subgraph for solution..."<< endl;

	//create a sub graph structure with only the vertices added so far
	foreach(int v, vertices_in_solution) {

		Vertex vertex = solution.instance.graph.get_vertex(v);

		//bring any feasible adges (i.e. connecting with vertices in chins solution)
		boost::graph_traits<BoostGraph>::adjacency_iterator vi, viend;
		for ( boost::tie(vi, viend) = boost::adjacent_vertices(vertex, solution.instance.graph.boostgraph); vi != viend; ++vi) {

			int index_neighboor = solution.instance.graph.index_for_vertex(*vi);

			//add only vertices that were considered (and avoid parallel edges)
			bool is_feasible = vertices_in_solution.get<1> ().find(index_neighboor) != vertices_in_solution.get<1> ().end();

			if (is_feasible) { //no need to check !solution.graph.contains_edge(v, index_neighboor)
				solution.add_edge_from_original(v, index_neighboor); //add edge with same weight to this sub graph
			}
		}
	}

	//cout << "chins subgraph created with " << solution.graph.num_edges() << " edges and " << solution.graph.num_vertices() << " vertices." << endl;

	//initialize
	solution.find_mst_tree();
	return solution;
}

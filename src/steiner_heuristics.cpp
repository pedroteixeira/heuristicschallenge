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
	boost::uniform_int<> range(0, instance.terminals.size()-1);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > die(instance.rng, range);
	int t0 = die();

	generate_chins_solution(t0, instance);
}

SteinerSolution SteinerHeuristics::generate_chins_solution(int root_terminal, Steiner& instance) {

	SteinerSolution solution(instance);

	//initialize list temp terminals (TODO: copy in random)
	list<int> terminals_left(solution.instance.terminals.size());
	std::copy(solution.instance.terminals.begin(), solution.instance.terminals.end(), terminals_left.begin());

	list<int> vertices_in_solution;

	terminals_left.remove(root_terminal);
	vertices_in_solution.push_back(root_terminal);

	while (terminals_left.size() > 0) {
		//choose terminal closest to any of the vertices added
		int closest_distance = INT_MAX;
		int closest_terminal, closest_vertex_in_tree;
		foreach(int t, terminals_left) {

			vector<int> distances, parents;
			boost::tie(distances, parents) = instance.get_shortest_distances(t);

			foreach(int v, vertices_in_solution) {
				int distance_to_v = distances[v];

				if(distance_to_v < closest_distance) {
					closest_distance = distance_to_v;
					closest_terminal = t;
					closest_vertex_in_tree = v;
				}
			}
		}

		terminals_left.remove(closest_terminal);

		assert(closest_distance != INT_MAX);
		/*
		 cout << "closest node to terminal " << closest_terminal << " is node " << closest_vertex_in_tree
		 << " [" << solution.instance.distances_from_terminal[closest_terminal][ closest_vertex_in_tree ]
		 << "]"
		 << " and path is : " << endl;
		 */

		//traverse shortest path from terminal
		int parent = closest_vertex_in_tree, last_parent = closest_vertex_in_tree;
		while(last_parent != closest_terminal) {
			//cout << parent << "  ";

			//add all vertices in this path
			bool not_in_solution = find(vertices_in_solution.begin(), vertices_in_solution.end(), parent)==vertices_in_solution.end();
			if(not_in_solution)
			vertices_in_solution.push_back(parent);

			last_parent = parent;

			parent = instance.get_parent_in_shortest_path(closest_terminal, parent);
		};
		//cout << endl;
	}

	cout << "creating subgraph for solution..."<< endl;

	//create a sub graph structure with only the vertices added so far
	foreach(int v, vertices_in_solution) {

		Vertex vertex = solution.instance.graph.get_vertex(v);

		//bring any feasible adges (i.e. connecting with vertices in chins solution)
		boost::graph_traits<BoostGraph>::adjacency_iterator vi, viend;
		for (boost::tie(vi,viend) = boost::adjacent_vertices(vertex, solution.instance.graph.boostgraph); vi != viend; ++vi) {

			int index_neighboor = solution.instance.graph.index_for_vertex(*vi);

			//add only vertices that were considered (and avoid parallel edges)
			bool is_feasible = find(vertices_in_solution.begin(), vertices_in_solution.end(), index_neighboor)!=vertices_in_solution.end(); //TODO: can make it O(1)

			if(is_feasible && !solution.graph.contains_edge(v, index_neighboor)) {
				solution.add_edge_from_original(v, index_neighboor); //add edge with same weight to this sub graph
			}
		}
	}

	cout << "chins subgraph created with " << solution.graph.num_edges() << " edges and " << solution.graph.num_vertices() << " vertices." << endl;

	solution.graph.writedot("chins.dot");


	//initialize
	solution.find_mst_tree();
	return solution;
}

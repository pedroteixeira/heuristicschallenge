/*
 * steiner_solution.cpp
 *
 *  Created on: Jul 14, 2008
 *      Author: pedro
 */

#include <assert.h>
#include <math.h>
#include <fstream>
#include <iostream>

#include <boost/progress.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/random.hpp>

#include "headers/steiner_solution.hpp"
using namespace std;

void SteinerSolution::update_candidates_out_vertices(Vertex v) {

	boost::graph_traits<BoostGraph>::adjacency_iterator ni, ni_end;
	boost::tie(ni, ni_end) = boost::adjacent_vertices(v, instance.graph.boostgraph);
	for (; ni != ni_end; ++ni) {
		if (check_candidate_for_out_vertex(*ni)) {
			out_vertices.push_back(*ni);
		} else {
			out_vertices.remove(*ni);
		}
	}
}

bool SteinerSolution::check_candidate_for_out_vertex(Vertex v) {

	bool is_disconnected = (boost::degree(v, graph.boostgraph) == 0);

	int neighboors_connected = 0;
	boost::graph_traits<BoostGraph>::adjacency_iterator ni, ni_end;
	boost::tie(ni, ni_end) = boost::adjacent_vertices(v, instance.graph.boostgraph);
	for (; ni != ni_end; ++ni) {

		int v_index = instance.graph.index_for_vertex(*ni);
		bool exists_in_solution = graph.contains_vertex(v_index);

		if (exists_in_solution && boost::degree(graph.get_vertex(v_index), graph.boostgraph) > 0)
			neighboors_connected++;
	}

	if (is_disconnected && neighboors_connected > 1) {
		return true;
	}

	return false;
}

void SteinerSolution::build_candidates_out_vertices() {

	out_vertices.clear();

	//select those nodes when added would have edges to connect to solution
	boost::graph_traits<BoostGraph>::vertex_iterator vi, vi_end;
	boost::tie(vi, vi_end) = boost::vertices(graph.boostgraph);
	for (; vi != vi_end; ++vi) {
		if (check_candidate_for_out_vertex(*vi)) {
			out_vertices.push_back(*vi);
		}
	}

	cout << "built " << out_vertices.size() << " candidates for out verticex." << endl;

}

void SteinerSolution::find_mst_tree() {
	//find MST on sub_graph
	tree.clear();
	boost::kruskal_minimum_spanning_tree(graph.boostgraph, back_inserter(tree));

	//compact
	int edges_removed = 0;

	for (size_t i = 0; i < tree.size(); i++) {
		Edge e = tree[i];

		Vertex u = boost::source(e, graph.boostgraph);
		Vertex v = boost::target(e, graph.boostgraph);

		bool is_u_lonely = (boost::degree(u, graph.boostgraph) == 1 && find(instance.terminals.begin(), instance.terminals.end(), u)
				== instance.terminals.end());
		bool is_v_lonely = (boost::degree(v, graph.boostgraph) == 1 && find(instance.terminals.begin(), instance.terminals.end(), v)
				== instance.terminals.end());

		//removing 1-degree non-terminal nodes
		if (is_u_lonely || is_v_lonely) {
			tree.erase(tree.begin() + i); //TODO: consider switching from vector to list
			i--;
			boost::remove_edge(e, graph.boostgraph);
			edges_removed++;
		}

		if (is_u_lonely)
			boost::remove_vertex(u, graph.boostgraph);

		if (is_v_lonely)
			boost::remove_vertex(v, graph.boostgraph);

	}

	if (edges_removed > 0)
		cout << edges_removed << " edges lonely removed from solution. " << endl;

}

void SteinerSolution::exchange_key_path() {
	//classify nodes
	//degree > 2 or termina = critical
	Edge e = boost::random_edge(graph.boostgraph, instance.rng);
	Vertex u, v;
	boost::tie(u, v) = boost::incident(e, graph.boostgraph);

	//find key path
}

int SteinerSolution::find_cost() {
	boost::property_map<BoostGraph, boost::edge_weight_t>::type weightmap = boost::get(boost::edge_weight, graph.boostgraph);

	int total = 0;
	foreach(Edge e, tree) {
		total += get(weightmap, e);
	}
	return total;
}

void SteinerSolution::generate_chins_solution(SteinerSolution& solution) {

	//initialize list temp terminals (TODO: copy in random)
	list<Vertex> terminals_left(solution.instance.terminals.size());
	copy(solution.instance.terminals.begin(), solution.instance.terminals.end(), terminals_left.begin());

	assert(terminals_left.size()> 0);

	vector<Vertex> vertices_in_solution;

	//pick initial terminal
	Vertex t0 = terminals_left.front();
	terminals_left.pop_front();
	vertices_in_solution.push_back(t0);

	while (terminals_left.size() > 0) {
		//choose terminal closest to any of the vertices added
		int closest_distance = INT_MAX;
		Vertex closest_terminal, closest_vertex_in_tree;
		foreach(Vertex t, terminals_left) {

			DistanceMap distances = solution.instance.distances_from_terminal[t];
			PredecessorMap parents = solution.instance.parents_from_terminal[t];

			foreach(Vertex v, vertices_in_solution) {
				if(distances[v] < closest_distance) {
					closest_distance = distances[v];
					closest_terminal = t;
					closest_vertex_in_tree = v;
				}
			}
		}

		assert(closest_distance != INT_MAX);

		terminals_left.remove(closest_terminal);

		//add all vertices in this path (that were not already added)
		Vertex parent = closest_vertex_in_tree, next;
		while(parent != closest_terminal) {
			next = solution.instance.parents_from_terminal[closest_terminal][parent];

			if(find(vertices_in_solution.begin(), vertices_in_solution.end(), parent)==vertices_in_solution.end())
			vertices_in_solution.push_back(parent);

			parent = next;
		}
		//finish adding last vertex in the path
		if(find(vertices_in_solution.begin(), vertices_in_solution.end(), closest_terminal)==vertices_in_solution.end())
		vertices_in_solution.push_back(closest_terminal);
	}

	//create a sub graph structure with only the vertices added so far
	foreach(Vertex v, vertices_in_solution) {
		boost::graph_traits<BoostGraph>::adjacency_iterator vi, viend;
		for (boost::tie(vi,viend) = boost::adjacent_vertices(v, solution.instance.graph.boostgraph); vi != viend; ++vi) {

			if(find(vertices_in_solution.begin(), vertices_in_solution.end(), *vi)!=vertices_in_solution.end()) {

				//add edge with same EdgeInfo TODO: find best way to share these info for subgraphs
				int weight = solution.instance.graph.get_edge_weight(v, *vi);
				boost::add_edge(v, *vi, weight, solution.graph.boostgraph);
			}
		}
	}

	//find MST on sub_graph
	solution.find_mst_tree();
}

/* constructors */
SteinerSolution::SteinerSolution() {
	//TODO: understand alternative to value initialization
}

SteinerSolution::SteinerSolution(const Steiner& steiner) {
	instance = steiner;
	init();
}

SteinerSolution::SteinerSolution(const SteinerSolution& solution) {
	instance = solution.instance;
	graph = Graph(solution.graph);

	//map edges from vector to the ones according to the new graph data structure
	foreach(Edge original_edge, solution.tree) {
		Edge e; bool found;
		Vertex u = boost::source(original_edge, solution.graph.boostgraph);
		Vertex v = boost::target(original_edge, solution.graph.boostgraph);
		boost::tie(e, found) = boost::edge(u,v, graph.boostgraph );
		tree.push_back(e);
	}

	assert(tree.size() == solution.tree.size());
	assert(boost::num_vertices(solution.graph.boostgraph) == boost::num_vertices(graph.boostgraph));
	assert(boost::num_edges(solution.graph.boostgraph) == boost::num_edges(graph.boostgraph));
	init();
}

void SteinerSolution::init() {

	build_candidates_out_vertices();

}


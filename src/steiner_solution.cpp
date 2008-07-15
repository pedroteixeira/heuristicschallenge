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

void SteinerSolution::insert_steiner_node() {

	assert(out_vertices.size() > 0);

	boost::uniform_int<> distrib(0, out_vertices.size()-1);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rand_gen(rng, distrib);
	size_t random_index = rand_gen();

	assert(random_index < out_vertices.size() && random_index >= 0);

	Vertex vertex = out_vertices[random_index];
	out_vertices.erase(out_vertices.begin() + random_index);

	cout << "inserting new vertex " << vertex << endl;

	//add edges to this graph
	boost::graph_traits<Graph>::adjacency_iterator vi, vi_end;
	boost::tie(vi, vi_end) = boost::adjacent_vertices(vertex, instance.graph);

	for (; vi != vi_end; ++vi) {

		Vertex v = (*vi);

		//only add if it's connected to solution tree TODO: should do dijkstra to add more nodes?
		if (v < boost::num_vertices(graph) && boost::out_degree(v, graph) > 0) {

			//get weight from original graph
			Edge original_edge;
			bool found;
			boost::tie(original_edge, found) = boost::edge(vertex, v, instance.graph);
			assert(found);

			//add to solution graph only if necessary
			Edge e;
			boost::tie(e, found) = boost::edge(vertex, v, graph);
			if (!found) {
				cout << "adding edge to solution of weight " << instance.graph[original_edge].weight
						<< endl;
				boost::add_edge(vertex, v, instance.graph[original_edge], graph);
			}

		}

	}

	cout << "test " << endl;
	find_mst_tree();

}

void SteinerSolution::find_mst_tree() {
	//find MST on sub_graph
	tree.clear();
	boost::kruskal_minimum_spanning_tree(graph, back_inserter(tree), boost::weight_map(boost::get(
			&EdgeInfo::weight, graph)));

	//compact
	for (size_t i = 0; i < tree.size(); i++) {
		Edge e = tree[i];

		Vertex u = boost::source(e, graph);
		Vertex v = boost::target(e, graph);

		if ((boost::degree(u, graph) == 1 && find(instance.terminals.begin(),
				instance.terminals.end(), u) == instance.terminals.end()) || (boost::degree(v, graph)
				== 1 && find(instance.terminals.begin(), instance.terminals.end(), v)
				== instance.terminals.end())) {

			cout << "removing 1-degree non-terminal vertex" << endl;
			tree.erase(tree.begin() + i);

			boost::remove_edge(e, graph);
		}
	}

}

void SteinerSolution::exchange_key_path() {
	//classify nodes
	//degree > 2 or termina = critical
	Edge e = boost::random_edge(graph, rng);
	Vertex u, v;
	boost::tie(u, v) = boost::incident(e, graph);

	//find key path
}

int SteinerSolution::find_cost() {
	int total = 0;
	foreach(Edge e, tree) {
		total += graph[e].weight;
	}
	return total;
}

void SteinerSolution::generate_chins_solution(SteinerSolution& solution) {

	vector<int> vertices_in_solution;

	//initialize list temp terminals (TODO: copy in random)
	list<int> terminals_left(solution.instance.terminals.size());
	copy(solution.instance.terminals.begin(), solution.instance.terminals.end(),
			terminals_left.begin());

	assert(terminals_left.size() > 0);

	//pick initial terminal
	int t0 = terminals_left.front();
	terminals_left.pop_front();
	vertices_in_solution.push_back(t0);

	while (terminals_left.size() > 0) {
		//choose terminal closest to any of the vertices added
		int closest_distance = INT_MAX;
		int closest_terminal, closest_vertex_in_tree;
		foreach(int t, terminals_left) {

			vector<int> distances = solution.instance.distances_from_terminal[t];
			vector<Vertex> parents = solution.instance.parents_from_terminal[t];

			foreach(int v, vertices_in_solution) {
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
	foreach(int v, vertices_in_solution) {
		boost::graph_traits<Graph>::adjacency_iterator vi, viend;
		for (boost::tie(vi,viend) = boost::adjacent_vertices(v, solution.instance.graph); vi != viend; ++vi) {

			if(find(vertices_in_solution.begin(), vertices_in_solution.end(), *vi)!=vertices_in_solution.end()) {

				Edge e; bool found;
				boost::tie(e, found) = boost::edge(v, *vi, solution.instance.graph); assert(found);

				//add edge with same EdgeInfo TODO: find best way to share these info for subgraphs
				boost::add_edge(v, *vi, solution.instance.graph[e], solution.graph);
			}
		}
	}

	//find MST on sub_graph
	solution.tree.clear();
	boost::kruskal_minimum_spanning_tree(solution.graph, back_inserter(solution.tree),
			boost::weight_map(boost::get(&EdgeInfo::weight, solution.graph)));

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
		Vertex u = boost::source(original_edge, solution.graph);
		Vertex v = boost::target(original_edge, solution.graph);
		boost::tie(e, found) = boost::edge(u,v, graph );
		tree.push_back(e);
	}

	assert(tree.size() == solution.tree.size());
	assert(boost::num_vertices(solution.graph) == boost::num_vertices(graph));
	assert(boost::num_edges(solution.graph) == boost::num_edges(graph));
	init();
}

void SteinerSolution::init() {

	boost::graph_traits<Graph>::vertex_iterator vi, vi_end;
	boost::tie(vi, vi_end) = boost::vertices(graph);

	for (; vi != vi_end; ++vi) {
		if (boost::degree(*vi, graph) == 0) {
			out_vertices.push_back(*vi);
		}
	}

}


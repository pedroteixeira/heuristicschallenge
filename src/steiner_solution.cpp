/*
 * steiner_solution.cpp
 *
 *  Created on: Jul 14, 2008
 *      Author: pedro
 */

#include <assert.h>
#include <math.h>
#include <iostream>
#include <stack>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/connected_components.hpp>

#include <boost/progress.hpp>

#include "headers/steiner_solution.hpp"

using namespace std;

void SteinerSolution::find_mst_tree() {

	undo_tree = tree;
	tree.clear();

	//run dijkstra
	//boost::kruskal_minimum_spanning_tree(graph.boostgraph, back_inserter(tree));

	//run prim's (has better complexity E log V instead of E log E)

	PredecessorMap p = boost::get(boost::vertex_predecessor, graph.boostgraph);
	boost::prim_minimum_spanning_tree(graph.boostgraph, p);

	//convert from parent map to list of edges
	foreach(Vertex v, boost::vertices(graph.boostgraph)) {
		if(v != p[v]) {
			int iu = graph.index_for_vertex(p[v]);
			int iv = graph.index_for_vertex(v);
			tree.push_back(make_pair(iu, iv));
		}
	}

	//compact
	compact_graph();

}

void SteinerSolution::compact_graph() {
	int edges_removed = 0;
	list<pair<int, int> >::iterator iter = tree.begin(), next;
	for (next = iter; iter != tree.end(); iter = next) {
		++next;

		int iu = (*iter).first;
		int iv = (*iter).second;
		Vertex u = graph.get_vertex(iu);
		Vertex v = graph.get_vertex(iv);

		bool is_u_lonely = boost::degree(u, graph.boostgraph) <= 1 && !is_terminal(iu);
		bool is_v_lonely = boost::degree(v, graph.boostgraph) <= 1 && !is_terminal(iv);

		IntSet vertices_to_remove;
		//removing 1-degree non-terminal nodes
		if (is_u_lonely || is_v_lonely || (vertices_to_remove.get<1> ().find(iu) != vertices_to_remove.get<1> ().end())
				|| (vertices_to_remove.get<1> ().find(iv) != vertices_to_remove.get<1> ().end())) {

			tree.erase(iter);
			graph.remove_edge(u, v);
			edges_removed++;
		}

		if (is_u_lonely)
			vertices_to_remove.push_back(iu);

		if (is_v_lonely)
			vertices_to_remove.push_back(iv);

		foreach(int x, vertices_to_remove) {
			out_key_nodes.remove(x); //keep consistent
			graph.remove_vertex(x);
		}
	}

	//eliminate all edges in graph that are not in tree (beware of invalidation of edges desccriptors)
	edges_removed = 0;

	boost::graph_traits<BoostGraph>::edge_iterator ei, ei_end, ei_next;
	boost::tie(ei, ei_end) = boost::edges(graph.boostgraph);
	for (ei_next = ei; ei != ei_end; ei = ei_next) {
		++ei_next;

		Vertex u = boost::source(*ei, graph.boostgraph);
		Vertex v = boost::target(*ei, graph.boostgraph);
		int iu = graph.index_for_vertex(u);
		int iv = graph.index_for_vertex(v);

		bool exists_in_tree = false;
		typedef pair<int, int> IntPair;
		foreach(IntPair edge, tree) {
			if((edge.first == iu && edge.second == iv) || (edge.first == iv && edge.second == iu)) {
				exists_in_tree = true; break;
			}
		}

		if (!exists_in_tree) {
			graph.remove_edge(*ei);
			edges_removed++;
		}
	}

	//temp sanity check
	list<Vertex> cycle;
	if (graph.has_cycle(cycle)) {
		graph.writedot("treewithcycle.dot");

		cout << "cycle is: ";
		foreach(Vertex v, cycle) {
			cout << graph.index_for_vertex(v) << ", ";
		}

		assert(false);
	}

	DistanceMap components;
	int num = boost::connected_components(graph.boostgraph, components);
	assert(num == 1);
}

void SteinerSolution::grow_graph() {
	foreach(Vertex v, boost::vertices(graph.boostgraph)) {

		int iv = graph.index_for_vertex(v);
		foreach(Vertex n, boost::adjacent_vertices(instance.graph.get_vertex(iv), instance.graph.boostgraph)) {
			int in = instance.graph.index_for_vertex(n);

			if(graph.contains_vertex(in)) {
				add_edge_from_original(iv, in);
			}
		}
	}
}

void SteinerSolution::undo_last_mst() {
	tree = undo_tree;
	//restore all edges and vertices
	typedef pair<int, int> IntPair;
	foreach(IntPair edge, tree) {
		add_edge_from_original(edge.first, edge.second);
	}
	find_mst_tree();
}

bool SteinerSolution::is_terminal(int v) {
	return instance.is_terminal(v) || (virtual_terminals.get<1> ().find(v) != virtual_terminals.get<1> ().end());
}

void SteinerSolution::add_edge_from_original(int u, int v) {
	graph.add_edge(u, v, instance.graph.get_edge_weight(u, v));
}

void SteinerSolution::add_path(int from, int to, vector<int>& parents) {

	int last_parent = to, parent = parents[last_parent], first = from;

	while (last_parent != first) {
		//cout << last_parent << "," << parent << " - ";
		add_edge_from_original(last_parent, parent);

		last_parent = parent;
		parent = parents[parent];
	}
	//cout << "\n";

}

/**
 * mantain out vertices
 */
bool SteinerSolution::is_new_candidate_for_out_key_node(int i) {

	bool is_already_in_solution = graph.contains_vertex(i);
	if (is_already_in_solution) //excludes terminals also
		return false;

	bool already_added_as_candidate = find(out_key_nodes.begin(), out_key_nodes.end(), i) != out_key_nodes.end(); //TODO: can make it O(1) with hashed index
	if (already_added_as_candidate)
		return false;

	//check if it can connect to at least 2 neighboors in this solution
	int neighboors_connected = 0;
	foreach(Vertex n_in_original, boost::adjacent_vertices(instance.graph.get_vertex(i), instance.graph.boostgraph)) {

		int index_n = instance.graph.index_for_vertex(n_in_original);
		bool n_exists_in_solution = graph.contains_vertex(index_n);

		if(n_exists_in_solution && boost::degree(graph.get_vertex(index_n), graph.boostgraph)> 0)
		neighboors_connected++;
	}

	return neighboors_connected> 1;
}

void SteinerSolution::update_candidates_out_key_nodes(int i) {

	foreach(Vertex n, boost::adjacent_vertices(instance.graph.get_vertex(i), instance.graph.boostgraph)) {
		int index_n = instance.graph.index_for_vertex(n);
		if (is_new_candidate_for_out_key_node(index_n)) {
			out_key_nodes.push_back(index_n);
		} else {
			out_key_nodes.remove(index_n);
		}
	}
}

void SteinerSolution::build_candidates_out_key_nodes() {
	out_key_nodes.clear();

	//select those nodes when added would have edges to connect to solution
	for (int i = 0; i < instance.V; i++) {
		if (is_new_candidate_for_out_key_node(i)) {
			out_key_nodes.push_back(i);
		}
	}

	cout << "built list of " << out_key_nodes.size() << " candidates for 'out vertices'." << endl;
}

void SteinerSolution::on_key_node_inserted(int i) {
	virtual_terminals.push_back(i);
	out_key_nodes.remove(i);
	in_key_nodes.push_back(i);
	update_candidates_out_key_nodes(i);

}

void SteinerSolution::on_key_node_removed(int i) {
	virtual_terminals.remove(i);
	out_key_nodes.push_back(i);
	in_key_nodes.remove(i);
	update_candidates_out_key_nodes(i);
}

bool SteinerSolution::is_out_key_node(int i) {
	return out_key_nodes.get<1> ().find(i) != out_key_nodes.get<1> ().end();
}

bool SteinerSolution::is_in_key_node(int i) {
	return in_key_nodes.get<1> ().find(i) != in_key_nodes.get<1> ().end();
}

/**
 * steiner nodes already in solution
 */

void SteinerSolution::build_candidates_in_key_nodes() {
	foreach(Vertex v, boost::vertices(graph.boostgraph)) {
		int iv = graph.index_for_vertex(v);
		bool is_steiner = !is_terminal(iv);

		if(is_steiner && graph.get_degree(v)> 2)
		in_key_nodes.push_back(iv);
	}
}

/**
 * Find cost of current solution.
 */
int SteinerSolution::find_cost() {
	int total = 0;
	typedef pair<int, int> IntPair;
	foreach(IntPair edge, tree) {
		total += graph.get_edge_weight(edge.first, edge.second);
	}
	return total;
}

/**
 * constructors
 * */

SteinerSolution::SteinerSolution(Steiner& steiner) :
	instance(steiner) {
	cout << "SteinerSolution::SteinerSolution(Steiner* steiner)" << endl;
}

SteinerSolution::SteinerSolution(const SteinerSolution& solution) :
	instance(solution.instance) {
	copy(solution, *this);
}

SteinerSolution& SteinerSolution::operator = ( const SteinerSolution& source ) {
	if(this != &source) {
		copy(source, *this);
	}
	return *this;
}

void SteinerSolution::copy(const SteinerSolution& source, SteinerSolution& to) {
	//cout << "copying SteinerSolution" << endl;

	to.instance = source.instance;
	to.out_key_nodes = source.out_key_nodes;
	to.in_key_nodes = source.in_key_nodes;

	to.tree = source.tree;

	to.graph = source.graph; //deep copy

	assert(to.tree.size() == source.tree.size());
	assert(boost::num_vertices(source.graph.boostgraph) == boost::num_vertices(to.graph.boostgraph));
	assert(boost::num_edges(source.graph.boostgraph) == boost::num_edges(to.graph.boostgraph));
}


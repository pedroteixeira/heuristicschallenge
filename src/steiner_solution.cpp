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
	tree.clear();


	DistanceMap c;
	int num = boost::connected_components(graph.boostgraph, c);
	assert(num == 1);

	//run dijkstra
	boost::kruskal_minimum_spanning_tree(graph.boostgraph, back_inserter(tree));

	//run prim's (has better complexity E log V instead of E log E)
	/*
	 PredecessorMap p = boost::get(boost::vertex_predecessor, graph.boostgraph);
	 boost::prim_minimum_spanning_tree(graph.boostgraph, p);

	 //convert from parent map to list of edges
	 foreach(Vertex v, boost::vertices(graph.boostgraph)) {
	 if(v != p[v]) {
	 Edge e; bool found;
	 boost::tie(e, found) = boost::edge(p[v], v, graph.boostgraph);
	 assert(found);
	 tree.push_back(e);
	 }
	 }
	 */

	//compact
	last_cropped_edges.clear();
	IntSet vertices_to_remove;

	int edges_removed = 0;
	list<Edge>::iterator iter = tree.begin(), next;
	for (next = iter; iter != tree.end(); iter = next) {
		++next;

		Vertex u = boost::source(*iter, graph.boostgraph);
		Vertex v = boost::target(*iter, graph.boostgraph);
		int iu = graph.index_for_vertex(u);
		int iv = graph.index_for_vertex(v);

		bool is_u_lonely = boost::degree(u, graph.boostgraph) <= 1 && !is_terminal(iu);
		bool is_v_lonely = boost::degree(v, graph.boostgraph) <= 1 && !is_terminal(iv);

		//removing 1-degree non-terminal nodes
		if (is_u_lonely || is_v_lonely || (vertices_to_remove.get<1> ().find(iu) != vertices_to_remove.get<1> ().end())
				|| (vertices_to_remove.get<1> ().find(iv) != vertices_to_remove.get<1> ().end())) {

			tree.erase(iter);
			graph.remove_edge(*iter);
			last_cropped_edges.push_back(pair<int,int>(iu, iv));
			edges_removed++;
		}

		if (is_u_lonely)
			vertices_to_remove.push_back(iu);

		if (is_v_lonely)
			vertices_to_remove.push_back(iv);

	}

	foreach(int v, vertices_to_remove) {
		out_vertices.remove(v); //keep consistent
		graph.remove_vertex(v);
	}

	if (edges_removed> 0) {
		cout << edges_removed << " lonely edges and " << vertices_to_remove.size() << " vertices removed from solution tree. " << endl;
	}

	//eliminate all edges in graph that are not in tree (beware of invalidation of edges desccriptors)
	edges_removed = 0;


	boost::graph_traits<BoostGraph>::edge_iterator ei, ei_end, ei_next;
	boost::tie(ei, ei_end) = boost::edges(graph.boostgraph);
	for (ei_next = ei; ei != ei_end; ei = ei_next) {
		++ei_next;

		bool exists_in_tree = find(tree.begin(), tree.end(), *ei) != tree.end();

		if (!exists_in_tree) {
			Vertex u = boost::source(*ei, graph.boostgraph);
			Vertex v = boost::target(*ei, graph.boostgraph);
			int iu = graph.index_for_vertex(u);
			int iv = graph.index_for_vertex(v);

			graph.remove_edge(*ei);
			last_cropped_edges.push_back(pair<int,int>(iu, iv)); //short memory

			edges_removed++;
			cout << "removing " << iu << ", " << iv << "\n";
		}
	}

	if (edges_removed> 0) {
		cout << edges_removed << " edges removed from graph to match solution tree of size " << tree.size() << "\n";
	}

	num = boost::connected_components(graph.boostgraph, c);
	assert(num == 1);

}

void SteinerSolution::undo_last_cropped_edges() {
	typedef pair<int, int> IntPair;
	foreach(IntPair edge, last_cropped_edges) {
		add_edge_from_original(edge.first, edge.second);
	}
}

bool SteinerSolution::is_terminal(int v) {
	return instance->is_terminal(v) || (virtual_terminals.get<1> ().find(v) != virtual_terminals.get<1> ().end());
}

void SteinerSolution::add_edge_from_original(int u, int v) {
	graph.add_edge(u, v, instance->graph.get_edge_weight(u, v));
}

/**
 * mantain out vertices
 */
bool SteinerSolution::is_new_candidate_for_out_vertex(int i) {

	bool is_already_in_solution = graph.contains_vertex(i);
	if (is_already_in_solution) //excludes terminals also
		return false;

	bool already_added_as_candidate = find(out_vertices.begin(), out_vertices.end(), i) != out_vertices.end(); //TODO: can make it O(1) with hashed index
	if (already_added_as_candidate)
		return false;

	//check if it can connect to at least 2 neighboors in this solution
	int neighboors_connected = 0;
	foreach(Vertex n_in_original, boost::adjacent_vertices(instance->graph.get_vertex(i), instance->graph.boostgraph)) {

		int index_n = instance->graph.index_for_vertex(n_in_original);
		bool n_exists_in_solution = graph.contains_vertex(index_n);

		if(n_exists_in_solution && boost::degree(graph.get_vertex(index_n), graph.boostgraph)> 0)
		neighboors_connected++;
	}

	return neighboors_connected> 1;
}

void SteinerSolution::update_candidates_out_vertices(int i) {

	foreach(Vertex n, boost::adjacent_vertices(instance->graph.get_vertex(i), instance->graph.boostgraph)) {
		int index_n = instance->graph.index_for_vertex(n);
		if (is_new_candidate_for_out_vertex(index_n)) {
			out_vertices.push_back(index_n);
		} else {
			out_vertices.remove(index_n);
		}
	}
}

void SteinerSolution::build_candidates_out_vertices() {
	out_vertices.clear();

	//select those nodes when added would have edges to connect to solution
	for (int i = 0; i < instance->V; i++) {
		if (is_new_candidate_for_out_vertex(i)) {
			out_vertices.push_back(i);
		}
	}

	cout << "built list of " << out_vertices.size() << " candidates for 'out vertices'." << endl;
}

void SteinerSolution::on_steiner_node_inserted(int i) {
	virtual_terminals.push_back(i);
	out_vertices.remove(i);
	update_candidates_out_vertices(i);

}

void SteinerSolution::on_steiner_node_removed(int i) {
	virtual_terminals.remove(i);
	out_vertices.push_back(i);
	update_candidates_out_vertices(i);
}

/**
 * Find cost of current solution.
 */
int SteinerSolution::find_cost() {
	int total = 0;
	foreach(Edge e, tree) {
		total += graph.get_edge_weight(e);
	}
	return total;
}

/**
 * Heuristics to generate good solution.
 */
void SteinerSolution::generate_chins_solution(SteinerSolution& solution) {

	//initialize list temp terminals (TODO: copy in random)
	list<int> terminals_left(solution.instance-> terminals.size());
	std::copy(solution.instance->terminals.begin(), solution.instance->terminals.end(), terminals_left.begin());

	assert(terminals_left.size()> 0);

	list<int> vertices_in_solution;

	//pick initial terminal
	int t0 = terminals_left.front();
	terminals_left.pop_front();
	vertices_in_solution.push_back(t0);

	while (terminals_left.size() > 0) {
		//choose terminal closest to any of the vertices added
		int closest_distance = INT_MAX;
		int closest_terminal, closest_vertex_in_tree;
		foreach(int t, terminals_left) {

			vector<int> distances = solution.instance->distances_from_terminal[t]; //TODO: Confirm = does not copy
			vector<int> parents = solution.instance->parents_from_terminal[t];

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
		 << " [" << solution.instance->distances_from_terminal[closest_terminal][ closest_vertex_in_tree ]
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
			parent = solution.instance->parents_from_terminal[closest_terminal][parent];
		};
		//cout << endl;
	}

	cout << "creating subgraph for solution..."<< endl;

	//create a sub graph structure with only the vertices added so far
	foreach(int v, vertices_in_solution) {

		Vertex vertex = solution.instance->graph.get_vertex(v);

		//bring any feasible adges (i.e. connecting with vertices in chins solution)
		boost::graph_traits<BoostGraph>::adjacency_iterator vi, viend;
		for (boost::tie(vi,viend) = boost::adjacent_vertices(vertex, solution.instance->graph.boostgraph); vi != viend; ++vi) {

			int index_neighboor = solution.instance->graph.index_for_vertex(*vi);

			//add only vertices that were considered (and avoid parallel edges)
			bool is_feasible = find(vertices_in_solution.begin(), vertices_in_solution.end(), index_neighboor)!=vertices_in_solution.end(); //TODO: can make it O(1)

			if(is_feasible && !solution.graph.contains_edge(v, index_neighboor)) {
				solution.add_edge_from_original(v, index_neighboor); //add edge with same weight to this sub graph
			}
		}
	}

	cout << "chins subgraph created with " << solution.graph.num_edges() << " edges and " << solution.graph.num_vertices() << " vertices." << endl;

	solution.graph.writedot("chins.dot");

	//trim and find MST on sub_graph
	solution.find_mst_tree();
}

/**
 * constructors
 * */

SteinerSolution::SteinerSolution(Steiner* steiner) {
	cout << "SteinerSolution::SteinerSolution(Steiner* steiner)" << endl;

	instance = steiner;
	init();
}

SteinerSolution::SteinerSolution(const SteinerSolution& solution) {
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
	to.out_vertices = source.out_vertices;
	to.graph = source.graph; //deep copy

	//map edges from solution tree to the ones according to the new graph data structure
	to.tree.clear();
	for (list<Edge>::const_iterator iter = source.tree.begin(); iter != source.tree.end(); iter++) {

		Vertex u = boost::source(*iter, source.graph.boostgraph);
		Vertex v = boost::target(*iter, source.graph.boostgraph);

		int iu = source.graph.index_for_vertex(u);
		int iv = source.graph.index_for_vertex(v);
		Vertex nu = to.graph.get_vertex(iu);
		Vertex nv = to.graph.get_vertex(iv);

		Edge e;
		bool found;
		boost::tie(e, found) = boost::edge(nu, nv, to.graph.boostgraph );
		assert(found);
		to.tree.push_back(e);
	}

	assert(to.tree.size() == source.tree.size());
	assert(boost::num_vertices(source.graph.boostgraph) == boost::num_vertices(to.graph.boostgraph));
	assert(boost::num_edges(source.graph.boostgraph) == boost::num_edges(to.graph.boostgraph));
}

void SteinerSolution::init() {
}


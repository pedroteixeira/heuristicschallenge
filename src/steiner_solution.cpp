/*
 * steiner_solution.cpp
 *
 *  Created on: Jul 14, 2008
 *      Author: pedro
 */

#include <assert.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <stack>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/undirected_dfs.hpp>

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

class prune_visitor: public boost::default_dfs_visitor {
public:
	prune_visitor(SteinerSolution& s, map<int, int>& d, list<Vertex>& l) :
		solution(s), vertices_to_prune(l), degree(d) {
	}

	void finish_vertex(Vertex u, const BoostGraph & g) {
		int iu = solution.graph.index_for_vertex(u);
		if (degree[iu] <= 1 && !solution.is_terminal(iu)) {

			foreach(Vertex n, boost::adjacent_vertices(u, g)) {
				int in = solution.graph.index_for_vertex(n);
				degree[in] = degree[in]-1;
			}

			vertices_to_prune.push_back(u);
		}
	}

	SteinerSolution& solution;
	list<Vertex>& vertices_to_prune;
	map<int, int>& degree;
};

void SteinerSolution::compact_graph() {

	//eliminate all edges in graph that are not in tree (beware of invalidation of edges desccriptors)
	int edges_removed = 0;

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
			if ((edge.first == iu && edge.second == iv) || (edge.first == iv && edge.second == iu)) {
				exists_in_tree = true;
				break;
			}
		}

		if (!exists_in_tree) {
			graph.remove_edge(*ei);
			edges_removed++;
		}
	}

	//prune graph

	map<int, int> degree;
	foreach(Vertex u, boost::vertices(graph.boostgraph)) {
		degree.insert(make_pair( graph.index_for_vertex(u), boost::degree(u, graph.boostgraph)));
	}

	list<Vertex> vertices_to_prune;
	prune_visitor vis(*this, degree, vertices_to_prune);
	boost::undirected_dfs(graph.boostgraph,
			boost::root_vertex(*boost::vertices(graph.boostgraph).first) .visitor(vis).edge_color_map(boost::get(
					boost::edge_color, graph.boostgraph)));

	foreach(Vertex u, vertices_to_prune) {
		graph.remove_vertex(u);
	}

	//cout << vertices_to_prune.size() << " vertices pruned from tree.\n";

	//check TODO: find out how to prune better
	boost::graph_traits<BoostGraph>::vertex_iterator vi, vi_end, vi_next;
	boost::tie(vi, vi_end) = boost::vertices(graph.boostgraph);
	for (vi_next = vi; vi != vi_end; vi = vi_next) {
		++vi_next;
		if (boost::degree(*vi, graph.boostgraph) <= 1 && !is_terminal(*vi)) {
			graph.remove_vertex(*vi);
		}
	}

	//trim tree structure

	edges_removed = 0;
	list<pair<int, int> >::iterator iter = tree.begin(), next;
	for (next = iter; iter != tree.end(); iter = next) {
		++next;

		int iu = (*iter).first;
		int iv = (*iter).second;

		if (!graph.contains_vertex(iu) || !graph.contains_vertex(iv)) {
			tree.erase(iter);
			edges_removed++;
		}
	}

	//temp sanity check


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

bool SteinerSolution::is_terminal(Vertex v) {
	return is_terminal(graph.index_for_vertex(v));
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
 * Find cost of current solution.
 */
int SteinerSolution::find_cost() const {
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
	//cout << "SteinerSolution::SteinerSolution(Steiner* steiner)" << endl;
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

	to.tree = source.tree;

	to.graph = source.graph; //deep copy

	assert(to.tree.size() == source.tree.size());
	assert(boost::num_vertices(source.graph.boostgraph) == boost::num_vertices(to.graph.boostgraph));
	assert(boost::num_edges(source.graph.boostgraph) == boost::num_edges(to.graph.boostgraph));
}

void SteinerSolution::check_integrity() {
	foreach(int terminal, instance.terminals) {
		assert(graph.contains_vertex(terminal));
	}

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

void SteinerSolution::writetext(string path) {

}

void SteinerSolution::writetex(string path) {
	ofstream tex_file(path.c_str());

	tex_file << "\\documentclass[12pt]{amsart} \n";
	tex_file << "\\usepackage[all,dvips,arc,curve,color,frame]{xy} \n";
	tex_file << "\\newxyColor{pink}{1.0 0.4 0.5}{rgb}{} \n";

	tex_file << "\\begin{document} \n";

	tex_file << "\\xygraph{ \n";
	tex_file << "!{<0mm,0mm>;<1mm,0mm>:<0mm,1mm>::} \n";

	foreach(Vertex u, boost::vertices(graph.boostgraph)) {
		int iu = graph.index_for_vertex(u);

		VertexInfo info = instance.vertices_info[iu];
		tex_file << " !{(" << info.x << "," << info.y << ")}*+{\\bullet}=\"" << iu << "\" \n";
	}

	foreach(Edge
			e, boost::edges(graph.boostgraph)) {
				Vertex u = boost::source(e, graph.boostgraph);
				Vertex v = boost::target(e, graph.boostgraph);
				int iu = graph.index_for_vertex(u);
				int iv = graph.index_for_vertex(v);

				tex_file << "\"" << iu << "\"-\"" << iv << "\" \n";
			}
			tex_file << "} } \n";
			tex_file << "\\end{document} \n";
}


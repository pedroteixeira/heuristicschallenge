/*
 * graph.cpp
 *
 *  Created on: Jul 16, 2008
 *      Author: pedro
 */

using namespace std;

#include "headers/util.hpp"
#include "headers/graph.hpp"


Graph::Graph() {
	init();
}

Graph::Graph(const Graph& graph) {
	boostgraph = BoostGraph(graph.boostgraph);
	init();
}

void Graph::init() {
	indexmap = boost::get(boost::vertex_index, boostgraph);
}


void Graph::dijkstra_shortest_paths(const Vertex& from, DistanceMap& d, PredecessorMap& p) {

	d = boost::get(boost::vertex_distance, boostgraph);
	p = boost::get(boost::vertex_predecessor, boostgraph);

	boost::dijkstra_shortest_paths(boostgraph, from,
			boost::distance_map(d).predecessor_map(p));
}

void Graph::add_edge(int u_index, int v_index, int weight) {

	Vertex u, v;

	//re-use or create nodes
	BiMap::left_map::const_iterator iter = index_bimap.left.find(u_index);
	if(iter != index_bimap.left.end())
		u = iter->second;
	else
		u = boost::add_vertex(boostgraph);

	iter = index_bimap.left.find(v_index);
		if(iter != index_bimap.left.end())
			v = iter->second;
		else
			v = boost::add_vertex(boostgraph);

	index_bimap.insert( MapPair(u_index, u ) );
	index_bimap.insert( MapPair(v_index, v ) );

	boost::add_edge(u, v, weight, boostgraph);
}

Vertex Graph::get_vertex(int v) {
	return index_bimap.left.at(v);
}

int Graph::index_for_vertex(Vertex v) {
	return index_bimap.right.at(v);
}

bool Graph::contains_vertex(int v) {
	return index_bimap.left.find(v) != index_bimap.left.end();
}

int Graph::get_edge_weight(Vertex v, Vertex u) {
	Edge e; bool found;
	boost::tie(e, found) = boost::edge(v, u, boostgraph); assert(found);

	boost::property_map<BoostGraph, boost::edge_weight_t>::type weightmap;
	weightmap = boost::get(boost::edge_weight, boostgraph);
	return boost::get(weightmap, e);
}


int Graph::num_vertices() {
	return boost::num_vertices(boostgraph);
}

int Graph::num_edges() {
	return boost::num_edges(boostgraph);
}







/*

void Util::render_dot(string path, Graph boostgraph) {

	ofstream dot_file(path.c_str());

	dot_file << "boostgraph D {\n" << "  rankdir=LR\n" << "  size=\"4,3\"\n" << "  ratio=\"fill\"\n"
			<< "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";

	boost::boostgraph_traits<Graph>::edge_iterator ei, ei_end;
	for ( boost::tie(ei, ei_end) = boost::edges(boostgraph); ei != ei_end; ++ei) {
		boost::boostgraph_traits<Graph>::edge_descriptor e = *ei;
		boost::boostgraph_traits<Graph>::vertex_descriptor u = boost::source(e, boostgraph), v = boost::target(e, boostgraph);
		dot_file << u << " -- " << v << "[label=\"" << boostgraph[e].weight << "\"";

		dot_file << ", color=\"black\"";

		dot_file << "]; \n";
	}
	dot_file << "}";


	boost::dynamic_properties dp;
		dp.property("id", boost::get(boost::vertex_index, solution_subboostgraph));
		dp.property("weight", boost::get(&EdgeInfo::weight, solution_subboostgraph));
		ofstream out("tree.dot");
		boost::write_boostgraphviz(out, solution_subboostgraph, dp, std::string("id"));


}
*/

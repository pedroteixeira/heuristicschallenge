/*
 * graph.cpp
 *
 *  Created on: Jul 16, 2008
 *      Author: pedro
 */

using namespace std;

#include "headers/util.hpp"
#include "headers/graph.hpp"

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/undirected_dfs.hpp>
#include <iostream>

Graph::Graph() {
	init();
}

Graph::Graph(const Graph& graph) {
	copy(graph, *this);
}

Graph& Graph::operator = ( const Graph& graph ) {
	if(this != &graph)
	copy(graph, *this);
	return *this;
}

void Graph::copy(const Graph& from, Graph& to) {
	//cout << "copying graph" << endl;
	to.init();

	assert( to.num_edges() == 0 );
	assert( to.num_vertices() == 0 );

	foreach(Edge e, boost::edges(from.boostgraph)) {
		Vertex u = boost::source(e, from.boostgraph);
		Vertex v = boost::target(e, from.boostgraph);

		int u_index = from.index_bimap.right.at(u);
		int v_index = from.index_bimap.right.at(v);
		to.add_edge(u_index, v_index, from.get_edge_weight(u, v));

	}

	assert(from.num_vertices() == to.num_vertices());
	assert(from.num_edges() == to.num_edges());
}

void Graph::init() {
	boostgraph.clear();
	index_bimap.clear();
	indexmap = boost::get(boost::vertex_index, boostgraph);
	weightmap = boost::get(boost::edge_weight, boostgraph);
}

void Graph::dijkstra_shortest_paths(int from, DistanceMap& d, PredecessorMap& p) {

	d = boost::get(boost::vertex_distance, boostgraph);
	p = boost::get(boost::vertex_predecessor, boostgraph);

	boost::dijkstra_shortest_paths(boostgraph, index_bimap.left.at(from), boost::distance_map(d).predecessor_map(p));
}

void Graph::dijkstra_shortest_paths(Vertex from, DistanceMap& d, PredecessorMap& p) {
	dijkstra_shortest_paths(index_for_vertex(from), d, p);
}

void Graph::add_edge(int u_index, int v_index, int weight) {

	Vertex u, v;
	int V = num_vertices();

	//re-use or create nodes
	BiMap::left_map::const_iterator iter = index_bimap.left.find(u_index);
	if (iter != index_bimap.left.end())
		u = iter->second;
	else {
		u = boost::add_vertex(boostgraph);
		index_bimap.insert(MapPair(u_index, u));
		indexmap[u] = V++; //keep indexmap contiguous
	}

	iter = index_bimap.left.find(v_index);
	if (iter != index_bimap.left.end())
		v = iter->second;
	else {
		v = boost::add_vertex(boostgraph);
		index_bimap.insert(MapPair(v_index, v));
		indexmap[v] = V++; //keep indexmap contiguous
	}

	Edge e;
	bool inserted;
	boost::tie(e, inserted) = boost::add_edge(u, v, weight, boostgraph);

	/*
	 //TODO: is it better to be explicit about parallel edges? (using setS implies overhead)
	 bool found;
	 boost::tie(e, found) = boost::edge(u, v, boostgraph);

	 if (!found) {
	 bool inserted;
	 boost::tie(e, inserted) = boost::add_edge(u, v, weight, boostgraph);
	 assert(inserted);
	 } else {
	 cerr << "edge already added : " << u_index << ", " << v_index << "\n";
	 }
	 */
}

void Graph::remove_edge(int u, int v) {
	remove_edge(get_vertex(u), get_vertex(v));
}

void Graph::remove_edge(Vertex u, Vertex v) {
	boost::remove_edge(u, v, boostgraph);
}

void Graph::remove_edge(Edge e) {
	boost::remove_edge(e, boostgraph);
}

void Graph::remove_vertex(int v) {
	if (contains_vertex(v))
		remove_vertex(get_vertex(v));
}

void Graph::remove_vertex(Vertex v) {

	boost::clear_vertex(v, boostgraph);
	boost::remove_vertex(v, boostgraph);

	int index_v = index_bimap.right.at(v);
	index_bimap.right.erase(v);
	index_bimap.left.erase(index_v);

	//keep indexmap contiguous
	int index = 0;
	boost::graph_traits<BoostGraph>::vertex_iterator vi, viend;
	for ( boost::tie(vi, viend) = boost::vertices(boostgraph); vi != viend; ++vi) {
		indexmap[*vi] = index++;
	}
}

int Graph::get_degree(Vertex v) const {
	return boost::degree(v, boostgraph);
}

int Graph::get_degree(int v) const {
	return get_degree(get_vertex(v));
}

Vertex Graph::get_vertex(int v) const {
	return index_bimap.left.at(v);
}

int Graph::index_for_vertex(Vertex v) const {
	return index_bimap.right.at(v);
}

bool Graph::contains_edge(int ui, int vi) const {
	Vertex u, v;
	BiMap::left_map::const_iterator iter = index_bimap.left.find(ui);
	if (iter != index_bimap.left.end())
		u = iter->second;
	else
		return false;

	iter = index_bimap.left.find(vi);
	if (iter != index_bimap.left.end())
		v = iter->second;
	else
		return false;

	return boost::is_adjacent(boostgraph, u, v);

}

bool Graph::contains_vertex(int v) const {
	return index_bimap.left.find(v) != index_bimap.left.end();
}

int Graph::get_edge_weight(Vertex v, Vertex u) const {
	Edge e;
	bool found;
	boost::tie(e, found) = boost::edge(v, u, boostgraph);
	assert(found);
	return boost::get(weightmap, e);
}

int Graph::get_edge_weight(int u, int v) const {
	return get_edge_weight(get_vertex(u), get_vertex(v));
}

int Graph::get_edge_weight(Edge e) const {
	return boost::get(weightmap, e);
}

int Graph::num_vertices() const {
	return boost::num_vertices(boostgraph);
}

int Graph::num_edges() const {
	return boost::num_edges(boostgraph);
}

struct cycle_detector: public boost::dfs_visitor<> {
	cycle_detector(bool& has_cycle, list<Vertex>& cycle, Graph& graph) :
		_has_cycle(has_cycle), _cycle(cycle), _graph(graph) {
	}

	void back_edge(Edge e, const BoostGraph& g) {
		_has_cycle = true;

		Vertex u = boost::source(e, g);
		Vertex v = boost::target(e, g);

		if (u != _cycle.back())
			_cycle.push_back(u);
		else
			_cycle.push_back(v);

		throw -1;
	}

	void discover_vertex(Vertex u, const BoostGraph & g) {
		_cycle.push_back(u);
	}

	void finish_vertex(Vertex u, const BoostGraph & g) {
		_cycle.clear();
	}
protected:
	bool& _has_cycle;
	list<Vertex>& _cycle;
	Graph& _graph;
};

bool Graph::has_cycle(list<Vertex>& cycle) {
	bool has_cycle = false;

	cycle_detector vis(has_cycle, cycle, *this);
	/*
	 try {
	 boost::depth_first_search(boostgraph, boost::visitor(vis));
	 } catch(int) {
	 return true;
	 }
	 return false;
	 */

	try {
		boost::undirected_dfs(boostgraph,
				boost::root_vertex(*boost::vertices(boostgraph).first) .visitor(vis) .edge_color_map(boost::get(
						boost::edge_color, boostgraph)));

	} catch (int) {
		return true;
	}

	return false;

}

void Graph::print() {

	boost::property_map<BoostGraph, boost::vertex_index_t>::type vertex_index;
	vertex_index = boost::get(boost::vertex_index, boostgraph);

	boost::print_graph(boostgraph);

	boost::graph_traits<BoostGraph>::edge_iterator ei, ei_end;
	for ( boost::tie(ei, ei_end) = boost::edges(boostgraph); ei != ei_end; ++ei)
		std::cout << "(" << get(vertex_index, source(*ei, boostgraph)) << "," << get(vertex_index, target(*ei, boostgraph))
				<< ") " << "[" << boost::get(weightmap, *ei) << "]";
	std::cout << std::endl;

}


void Graph::writedot(string path) {

	ofstream dot_file(path.c_str());

	dot_file << "graph G {\n";

	foreach(Edge e, boost::edges(boostgraph)) {

		Vertex u = boost::source(e, boostgraph);
		Vertex v = boost::target(e, boostgraph);
		dot_file << index_for_vertex(u) << " -- " << index_for_vertex(v);

		dot_file << "\n";

		//<< "[label=\"" << get_edge_weight(u,v) << "\"";
		//dot_file << ", color=\"black\"";
		//dot_file << "]; \n";
	}
	dot_file << "}";

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

/*
 * graph.hpp
 *
 *  Created on: Jul 16, 2008
 *      Author: pedro
 */

#ifndef GRAPH_HPP_
#define GRAPH_HPP_

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/bimap.hpp>

struct VertexInfo {
	int index;
	int x;
	int y;
};

struct EdgeInfo {
	int weight;
};

typedef boost::adjacency_list_traits<boost::listS, boost::listS, boost::undirectedS>::vertex_descriptor
		vertex_descriptor;

typedef boost::adjacency_list < boost::listS, boost::listS, boost::undirectedS,
boost::property<boost::vertex_index_t, int,
boost::property<boost::vertex_distance_t, int,
boost::property<boost::vertex_predecessor_t, vertex_descriptor> > >,
boost::property<boost::edge_weight_t, int> > BoostGraph;

typedef boost::graph_traits<BoostGraph>::edge_descriptor Edge;
typedef boost::graph_traits<BoostGraph>::vertex_descriptor Vertex;

typedef boost::property_map<BoostGraph, boost::vertex_distance_t>::type DistanceMap;
typedef boost::property_map<BoostGraph, boost::vertex_predecessor_t>::type PredecessorMap;

typedef boost::bimap< int, Vertex> BiMap;
typedef BiMap::value_type MapPair;


typedef boost::multi_index::multi_index_container<Vertex,
	boost::multi_index::indexed_by<
	boost::multi_index::random_access<>,
	boost::multi_index::hashed_unique<boost::multi_index::identity<Vertex> >
	>
	> VertexSet;

typedef VertexSet::nth_index<1>::type VertexHashSet;

class Graph {
public:
	Graph();
	Graph(const Graph&);
	Graph& operator = ( const Graph& source );

	BoostGraph boostgraph;


	int index_for_vertex(Vertex) const;
	Vertex get_vertex(int) const;
	bool contains_vertex(int) const;
	bool contains_edge(int, int) const;
	int get_edge_weight(Vertex, Vertex) const;
	int get_edge_weight(Edge) const;
	int num_vertices() const;
	int num_edges() const;
	int get_degree(int v) const;
	int get_degree(Vertex v) const;

	void add_edge(int, int, int);
	void remove_edge(Edge e);
	void remove_vertex(Vertex v);
	void remove_vertex(int v);

	void dijkstra_shortest_paths(int, DistanceMap&, PredecessorMap&);
	void dijkstra_shortest_paths(Vertex, DistanceMap&, PredecessorMap&);

	void print();
	void writedot(std::string);

private:
	BiMap index_bimap;
	boost::property_map<BoostGraph, boost::vertex_index_t>::type indexmap;
	boost::property_map<BoostGraph, boost::edge_weight_t>::type weightmap;

	void init();
	void copy(const Graph&, Graph&);
};

#endif /* GRAPH_HPP_ */

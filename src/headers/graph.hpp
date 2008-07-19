/*
 * graph.hpp
 *
 *  Created on: Jul 16, 2008
 *      Author: pedro
 */

#ifndef GRAPH_HPP_
#define GRAPH_HPP_

#include <boost/graph/adjacency_list.hpp>
#include <boost/bimap.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>


struct VertexInfo {
	int index;
	int x;
	int y;
};

struct EdgeInfo {
	int weight;
};

typedef boost::adjacency_list < boost::listS, boost::listS, boost::undirectedS, VertexInfo, EdgeInfo > BoostGraph;

typedef boost::graph_traits < BoostGraph >::edge_descriptor Edge;
typedef boost::graph_traits < BoostGraph >::vertex_descriptor Vertex;

typedef boost::bimap< int, Vertex > BiMap;
typedef BiMap::value_type MapPair;

class Graph {
public:
	Graph();
	Graph(int);
	BoostGraph boostgraph;
	Vertex get_vertex(int);
	VertexInfo get_vertex_info(int);
	void add_edge(int, int, EdgeInfo);
	int num_vertices();
	int num_edges();
	void dijkstra_shortest_paths(const Vertex&, std::vector<Vertex>&, std::vector<Vertex>&);

private:
	BiMap index_bimap;
	boost::property_map<BoostGraph, boost::vertex_index_t>::type indexmap;

};


#endif /* GRAPH_HPP_ */

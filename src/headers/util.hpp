/*
 * util.hpp
 *
 *  Created on: Jul 12, 2008
 *      Author: pedro
 */

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

struct VertexInfo {
	int x;
	int y;
};

struct EdgeInfo {
	int weight;
};

typedef boost::adjacency_list < boost::listS, boost::vecS, boost::undirectedS, VertexInfo, EdgeInfo > Graph;

typedef boost::graph_traits < Graph >::edge_descriptor Edge;
typedef boost::graph_traits < Graph >::vertex_descriptor Vertex;


class Util {
public:
	static void render_dot(std::string, Graph);
};

#endif /* UTIL_HPP_ */

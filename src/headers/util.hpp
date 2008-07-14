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

struct Node {
	int x;
	int y;
};

struct Edge {
	int weight;
};

typedef boost::adjacency_list < boost::listS, boost::vecS, boost::undirectedS, Node, Edge > Graph;

class Util {
public:
	static void render_dot(std::string, Graph);
};

#endif /* UTIL_HPP_ */

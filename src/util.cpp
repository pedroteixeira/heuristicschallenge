/*
 * Util.cpp
 *
 *  Created on: Jul 12, 2008
 *      Author: pedro
 */

#include <fstream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/tuple/tuple.hpp>
using namespace std;

#include "headers/util.hpp"

void Util::render_dot(string path, Graph graph) {

	ofstream dot_file(path.c_str());

	dot_file << "graph D {\n" << "  rankdir=LR\n" << "  size=\"4,3\"\n" << "  ratio=\"fill\"\n"
			<< "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";

	boost::graph_traits<Graph>::edge_iterator ei, ei_end;
	for ( boost::tie(ei, ei_end) = boost::edges(graph); ei != ei_end; ++ei) {
		boost::graph_traits<Graph>::edge_descriptor e = *ei;
		boost::graph_traits<Graph>::vertex_descriptor u = boost::source(e, graph), v = boost::target(e, graph);
		dot_file << u << " -- " << v << "[label=\"" << graph[e].weight << "\"";

		dot_file << ", color=\"black\"";

		dot_file << "]; \n";
	}
	dot_file << "}";

	/*
	boost::dynamic_properties dp;
		dp.property("id", boost::get(boost::vertex_index, solution_subgraph));
		dp.property("weight", boost::get(&EdgeInfo::weight, solution_subgraph));
		ofstream out("tree.dot");
		boost::write_graphviz(out, solution_subgraph, dp, std::string("id"));
		*/

}



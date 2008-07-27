/*
 * steiner_pathsearch.cpp
 *
 *  Created on: Jul 26, 2008
 *      Author: pedro
 */

#include "headers/steiner_pathsearch.hpp"
#include "headers/steiner_solution.hpp"
#include "headers/graph.hpp"
#include <stack>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>

using namespace std;

class dfs_time_visitor: public boost::default_dfs_visitor {

public:
	dfs_time_visitor(Graph& g, VertexSet& cs, list<list<Vertex> >& paths) :
		graph(g), critical_set(cs), critical_hashed(cs.get<1> ()), on_key_path(false), keypaths(paths) {
	}

	void discover_vertex(Vertex u, const BoostGraph & g) {

		bool is_critical = critical_hashed.find(u) != critical_hashed.end();

		if (is_critical) {
			if (on_key_path) {
				//end current path
				path.push_back(u);
				list<Vertex> tmp = path;
				keypaths.push_back(tmp);
				path.clear();
			} else {
				//start new from here
				on_key_path = true;
			}
			//add key node to current path
			path.push_back(u);
			critical_stack.push(u);
		} else if (on_key_path) {
			//add seiner node to current path
			path.push_back(u);
		}
	}

	void examine_edge(Edge e, const BoostGraph & g) {
	}

	void finish_vertex(Vertex u, const BoostGraph & g) {
		//update track
		bool is_critical = critical_hashed.find(u) != critical_hashed.end();
		if (is_critical) {
			path.clear();
			//throw away finished
			if (!critical_stack.empty())
				critical_stack.pop();
			//and restore previous critical (to deal with branches)
			if (!critical_stack.empty()) {
				path.push_back(critical_stack.top());
				on_key_path = true;
			} else {
				on_key_path = false; //I guess only necessary if graph is disconnected
			}
		}
	}

	Graph& graph;
	VertexSet& critical_set;
	VertexHashSet& critical_hashed;
	list<Vertex> path;
	list<list<Vertex> >& keypaths;
	stack<Vertex> critical_stack;
	bool on_key_path;
};

void SteinerPathLocalSearch::search(SteinerSolution& solution) {

	VertexSet critical_set;

	//TODO: the following can be cached or done in pre-processing
	//classify nodes
	foreach(Vertex u, boost::vertices(solution.graph.boostgraph)) {
		int iu = solution.graph.index_for_vertex(u);
		if(boost::degree(u, solution.graph.boostgraph)> 2 || solution.instance->is_terminal(iu)) //is critical
		critical_set.push_back(u);
	}
	assert(critical_set.size()> 0);

	//build all key paths for this solution
	list< list<Vertex> > keypaths;
	dfs_time_visitor vis(solution.graph, critical_set, keypaths);

	boost::depth_first_search(solution.graph.boostgraph, boost::visitor(vis));

	foreach(list<Vertex> keypath, keypaths) {

		//calculate weight for this path
		int path_weight = 0;
		Vertex previous;
		for(list<Vertex>::iterator iter = keypath.begin(); iter != keypath.end(); iter++) {
			if(iter != keypath.begin()) {
				Edge e; bool found;
				boost::tie(e, found) = boost::edge(previous, *iter, solution.graph.boostgraph);
				if(!found) { //TODO: understand!
					cerr << "is graph disconected? " << solution.graph.index_for_vertex(previous) << ", " << solution.graph.index_for_vertex(*iter) << "\n";
				} else {
					path_weight += solution.graph.get_edge_weight(*iter, previous);
				}
			}
			previous = *iter;
			cout << solution.graph.index_for_vertex(*iter) << " - ";
		}
		cout << "[" << path_weight << "] \n";


		//find shortest distance between two critical nodes and compare
		//TODO: cache distances
		DistanceMap distances;
		PredecessorMap parents;
		solution.graph.dijkstra_shortest_paths(keypath.front(), distances, parents);
		int best_distance = distances[keypath.back()];

		if(best_distance < path_weight) {
			cout << "!! best distance FOUND is " << best_distance << " over " << path_weight << ".\n";

			exchange_path(solution, keypath, parents);
			solution.find_mst_tree();

			break;
		}
	}

	cout << endl;
}

void SteinerPathLocalSearch::exchange_path(SteinerSolution& solution, list<Vertex>& keypath,
		PredecessorMap& shortestpath) {

	//remove old path
	cout << "removing old path: ";
	Vertex previous;
	foreach(Vertex v, keypath) {
		if(v != keypath.front()) {
			solution.graph.remove_edge(previous, v);
			cout << solution.graph.index_for_vertex(previous) << ", " << solution.graph.index_for_vertex(v) << " - ";
		}
		previous = v;
	}
	cout << "\n";

	//add new (better) path
	cout << "adding new path: ";
	Vertex last_parent = keypath.back(), parent = shortestpath[last_parent], first = keypath.front();
	while (last_parent != first) {
		cout << solution.graph.index_for_vertex(last_parent) << "," << solution.graph.index_for_vertex(parent) << " - ";
		solution.add_edge_from_original(solution.graph.index_for_vertex(last_parent), solution.graph.index_for_vertex(parent));

		last_parent = parent;
		parent = shortestpath[parent];
	}
	cout << "\n";


}

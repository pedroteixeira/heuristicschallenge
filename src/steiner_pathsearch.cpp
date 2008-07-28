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
#include <boost/graph/undirected_dfs.hpp>
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

				//cout << "starting new path from key-node " << graph.index_for_vertex(u) << "\n";
			} else {
				//start new from here
				on_key_path = true;
				//cout << "on key path \n";
			}
			//add key node to current path
			//cout << "adding key-node " << graph.index_for_vertex(u) <<"\n";

			path.push_back(u);
			critical_stack.push(u);
		}
		else if (on_key_path) {

			//cout << "adding steiner " << graph.index_for_vertex(u) <<"\n";
			//add seiner node to current path
			path.push_back(u);
		} else {

			//cout << "ignore steiner node off path " << graph.index_for_vertex(u) <<"\n";

		}
	}

	void finish_vertex(Vertex u, const BoostGraph & g) {

		//update track
		bool is_critical = critical_hashed.find(u) != critical_hashed.end();
		if (is_critical) {
			//throw away finished
			if (!critical_stack.empty())
				critical_stack.pop();
			//cout << "finished key-node " << graph.index_for_vertex(u) <<"\n";
		}

		if(is_critical || graph.get_degree(u) == 1) {
			path.clear();
			//and restore previous critical (to deal with branches)
			if (!critical_stack.empty()) {
				path.push_back(critical_stack.top());
				on_key_path = true;

				//cout << "keep as current key-node " << graph.index_for_vertex(critical_stack.top()) <<"\n";
			} else {
				on_key_path = false; //I guess only necessary if graph is disconnected

				//cout << "off key path\n";
			}
		}
	}

	Graph& graph;
	VertexSet& critical_set;
	VertexHashSet& critical_hashed;
	list<Vertex> path;
	bool on_key_path;
	list<list<Vertex> >& keypaths;
	stack<Vertex> critical_stack;

};

void SteinerPathLocalSearch::search(SteinerSolution& solution) {

	solution.graph.writedot("solution_beforesearch.dot");

	VertexSet critical_set;

	//TODO: the following can be cached or done in pre-processing
	//classify nodes
	foreach(Vertex u, boost::vertices(solution.graph.boostgraph)) {
		int iu = solution.graph.index_for_vertex(u);
		if(boost::degree(u, solution.graph.boostgraph)> 2 || solution.is_terminal(iu)) //is critical
		critical_set.push_back(u);
	}
	assert(critical_set.size()> 0);

	//build all key paths for this solution
	list< list<Vertex> > keypaths;
	dfs_time_visitor vis(solution.graph, critical_set, keypaths);

	boost::undirected_dfs(solution.graph.boostgraph, boost::root_vertex(*boost::vertices(solution.graph.boostgraph).first)
						.visitor(vis)
	           .edge_color_map(boost::get(boost::edge_color, solution.graph.boostgraph)));

	cout << keypaths.size() << " key-node paths were found.\n";

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
		vector<int> distances, parents;
		int from = solution.graph.index_for_vertex(keypath.front());
		int to = solution.graph.index_for_vertex(keypath.back());
		boost::tie(distances, parents) = solution.instance.get_shortest_distances(from);
		int best_distance = distances[to];

		if(best_distance < path_weight) {
			cout << "!! best distance FOUND is " << best_distance << " over " << path_weight << ".\n";

			exchange_path(solution, keypath, parents);
			solution.find_mst_tree();

			break;
		}
	}

	cout << endl;
}

void SteinerPathLocalSearch::exchange_path(SteinerSolution& solution, list<Vertex>& keypath, vector<int>& shortestpath) {

	//solution.graph.writedot("nodepath_before.dot");

	//remove old path
	cout << "removing old path: ";
	foreach(Vertex v, keypath) {
		cout << solution.graph.index_for_vertex(v) << ", ";
		if(v != keypath.front() && v != keypath.back()) {
			assert(solution.graph.get_degree(v) == 2);  //TODO: temp check
			solution.graph.remove_vertex(v);
		}
	}
	cout << "\n";

	//add new (better) path
	cout << "adding new path: ";
	solution.add_path(solution.graph.index_for_vertex(keypath.front()), solution.graph.index_for_vertex(keypath.back()), shortestpath);

	//solution.graph.writedot("nodepath_after.dot");

}
